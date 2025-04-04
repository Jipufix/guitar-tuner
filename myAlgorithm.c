/*
 * myAlgorithm.c
 *
 *  Main code for an implementation of a "Guitar Tuner"
 *  Documentation for FFT: https://arm-software.github.io/CMSIS_5/DSP/html/arm_fft_bin_example_f32_8c-example.html#a6
 *
 *  The website used to test was: https://www.szynalski.com/tone-generator
 *
 *  *NOTE*: Sometimes at frequencies less than C2 Hz, you have to lower the volume slightly since there's some distortion
 *  to higher frequencies
 *
 *  Created on: November 30th, 2023
 *      Author: Ashton Char
 */

#include "myAlgorithm.h"
#include "filters.h"	// function prototype for stereoInt16ToMonoFloat() and monoFloatToStereoInt16()
#include "processAudio.h"

#include "arm_math.h"
#include "arm_const_structs.h"

// private function prototypes


#define FFT_SIZE 4096					// size of FFT (number of data points)
#define IFFT_FLAG 0						// 0 for FFT, 1 for IFFT
#define DO_BIT_REVERSE 1				// 1 for bit reverse of the output (the usual order), 0 for no bit reverse
#define REFERENCE_FREQUENCY 440.00		// Note A4 to be used as reference
#define NOTE_CHAR_LENGTH 4				// The length of the char* we are using to represent a note
#define REPRESENTED_INTERVALS 9			// Number of octaves to be checked
#define ERROR_MESSAGE "No signal is detected"

// Variables used to find the maximum magnitude/dominant frequency
static float32_t maxSoFar;
static int16_t maxIndex;

float32_t monoBufferIn[MONO_BUFFER_SIZE], monoBufferOut[MONO_BUFFER_SIZE];
float32_t complexBuffer[2*FFT_SIZE];	// complex input/output buffer: real[0], imag[0], real[1], imag[1], ...
float32_t magnitude[FFT_SIZE];			// contains the occurrences of each frequency in the frequency domain
float32_t frequencyAxis[FFT_SIZE]; 		// used to convert from index of FFT to specific frequency
char result[NOTE_CHAR_LENGTH];			// the actual note detected
char notes[] = {'A', 'A', 'B', 'C', 'C', 'D', 'D', 'E', 'F', 'F', 'G', 'G'};
char modifiers[] = {'\0', '#', '\0', '\0', '#', '\0', '#', '\0', '\0', '#', '\0', '#'};

/*
// LPF IIR SETUP
#define IIR_NUM_STAGES 2  						// number of stages (or number of SOS)
#define IIR_NUM_COEFFICIENTS (5*IIR_NUM_STAGES)	// number of coefficients
float32_t IIR_Coefficients[IIR_NUM_COEFFICIENTS] = {
		0.411074,			//stage1 b0
		0.822149,			//stage1 b1
		0.411074,			//stage1 b2
		0.011913,			//stage1 a1
		-0.753642,			//stage1 a2
		0.135049,			//stage2 b0
		0.270098,			//stage2 b1
		0.135049,			//stage2 b2
		0.737892,			//stage2 a1
		-0.310095,			//stage2 a2
};
float_t IIR_State[4*IIR_NUM_STAGES] = {0};	// State array
arm_biquad_casd_df1_inst_f32 IIR_Structure;	// define instance of the floating-point Biquad cascade structure
*/


// Initialize Structure for the myAlgorithm Options menu
// If your DSP algorithm requires a menu, use this structure to define
// the number of menu items and the text for each item.
// If numItems is set to 0, the menu is disabled.
struct MenuInfoStructure MyAlogorithmMenu =
{
	.title = "MyAlgorithm Options:",		// title of menu
	.numItems = 0,					// number of items in menu or 0 to disable the menu
	.itemName = {"Option 0","Option 1","Option 2","Option 3","Option 4","Option 5","Option 6","Option 7","Option 8","Option 9"}, // name for each item in the menu
	.locationX = 235,				// x-axis location of menu
	.locationY = 70,				// y-axis location of menu
	.width = 235,					// width of menu
	.selectedItem = 0				// number of the currently selected menu item
};


// myAlgorithmInit: Use this function to initialize your DSP algorithm (if needed).
//                  This function is called once after power-up or reset.
// Input arguments: none
// Return value: none
void myAlgorithmInit(void)
{
	// Calculate the frequency axis
	for (int16_t i = 0; i < FFT_SIZE; i++)
	{
		frequencyAxis[i] = i * ((float32_t)SAMPLING_RATE_HZ / (float32_t)FFT_SIZE);
	}

	// Initialize the 4 kHz LPF
	/* arm_biquad_cascade_df1_init_f32(&IIR_Structure,IIR_NUM_STAGES,IIR_Coefficients,IIR_State); */
}


// myAlgorithm: When the program is set to MyAlgorithm, this function is called each
//				time there is an interrupt (when audio buffer is half full and when it is full).
//				Use this function to implement your DSP algorithm.
// Input arguments:
// 	  stereoIn - pointer to stereo input buffer (interleaved left channel followed by right channel)
//    stereoOut - pointer to stereo output buffer
//    stereoSize - number of samples in input (and output) buffer (half are left channel and half are right)
// Return value: none
void myAlgorithm(int16_t *stereoIn, int16_t *stereoOut, int16_t stereoSize)
{
	stereoInt16ToMonoFloat(stereoIn, monoBufferOut, stereoSize);

	// LPF is redundant since second findMax function call "cuts off" the irrelevant frequencies
	/* arm_biquad_cascade_df1_f32(&IIR_Structure,monoBufferIn,monoBufferOut,stereoSize/2); */

	for (int16_t n = 0; n < FFT_SIZE; n++)
	{
		complexBuffer[2*n]   		= monoBufferOut[n];  	// real part of input
		complexBuffer[2*n + 1] 		= 0.0;					// imaginary part of input
	}

	arm_cfft_f32(&arm_cfft_sR_f32_len4096, complexBuffer, IFFT_FLAG, DO_BIT_REVERSE);		// FFT
	arm_cmplx_mag_f32(complexBuffer, magnitude, FFT_SIZE);

	findMax(FFT_SIZE);
	if (maxIndex > (FFT_SIZE / 2))							// Check if found frequency is irrelevant (> 8 kHz)
	{
		findMax(FFT_SIZE / 2);
	}

	displayText(frequencyToNote(frequencyAxis[maxIndex]));
	monoFloatToStereoInt16(monoBufferOut, stereoOut, stereoSize/2);		//Pass-through
}

// frequencytoNote: Converts the given frequency to the corresponding letter note based
//					on the Western Musical Scale.
//
//					A normal guitar ranges from E2 (82 Hz) to E7 (~2.7 kHz). However,
//					some head room is given until around B7 (4 kHz). Technically, the
//					function can differentiate from C2 (65 Hz) to B8 (8000 kHz), but the
//					the incrementation doesn't work at low frequencies and an LPF was
//					placed at 4 kHz to ignore any irrelevant high frequencies.
//
//					Formulas were derived from this website:
//					https://newt.phys.unsw.edu.au/jw/notes.html#
// Input(s):
//		frequency - The average frequency of the sampled audio
// Output(s):
//		The corresponding letter note of the given frequency
char* frequencyToNote(float32_t frequency)
{
	if (frequency < 80 || frequency > 4000) {return ERROR_MESSAGE;}			// Checks to make sure frequency is in range

	int16_t semitones = round(12 * log2(frequency / REFERENCE_FREQUENCY));
	int16_t octave = ceil((float32_t)(semitones + 46) / 12.0);
    char note = notes[(semitones % 12 + 12) % 12];
    char modifier = modifiers[(semitones % 12 + 12) % 12];

    if (modifier == '\0')
    {
    	snprintf(result, NOTE_CHAR_LENGTH, "%c%d", note, octave);
    }
    else
    {
    	snprintf(result, NOTE_CHAR_LENGTH, "%c%c%d", note, modifier, octave);
    }


    return result;
}

// Find the maximum magnitude (aka the most occurring frequency)
void findMax(int16_t checkedRange)
{
	maxIndex = 0; maxSoFar = magnitude[0];								// Initialize the first value as the biggest
	for (int16_t i = 1; i < checkedRange ; i++)
	{
		if (magnitude[i] > maxSoFar)
		{
			maxSoFar = magnitude[i];
			maxIndex = i;
		}
	}
}

