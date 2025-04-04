/*
 * myAlgorithm.h
 *
 *  Created on: Jul 27, 2023
 *      Author: hoffbeck
 */

#ifndef INC_PROJECTS_STM32746G_DISCOVERY_EXAMPLES_BSP_INC_MYALGORITHM_H_
#define INC_PROJECTS_STM32746G_DISCOVERY_EXAMPLES_BSP_INC_MYALGORITHM_H_

#include "arm_math.h"		// definition of int16_t, etc.
#include "main.h"  			// definition of I2S_AUDIOFREQ_16K, etc.
#include "drawMenu.h"		// definition of MenuInfoStructure

// Set initial input level (0 - 100)
#define INITIAL_INPUT_LEVEL 85

// Set initial output level (0 - 100)
#define INITIAL_OUTPUT_LEVEL 30

// Set audio sampling rate (in Hz) for input and output
// Supported sampling rates (in Hz): 8k, 11k, 16k, 22k, 32k, 44k, 48k, 96k, 192k (see stm32f7xx_hal_i2s.h)
#define SAMPLING_RATE_HZ   I2S_AUDIOFREQ_16K 		// set sampling rate (in Hz)

// Set total buffer size (in number of samples) for input audio buffer and output audio buffer
// Interrupts occur when the buffer is half full and when it is full.
// The buffer is stereo with left and right samples interleaved.
#define DOUBLE_AUDIO_BUFFER_SIZE 8192	// double audio buffer size (in samples)

// Size of stereo buffer to be processed each time there is an interrupt
// when the buffer is half full and when it is full.
#define STEREO_BUFFER_SIZE DOUBLE_AUDIO_BUFFER_SIZE/2	// stereo audio buffer size (in samples)

// Size of mono buffer
#define MONO_BUFFER_SIZE STEREO_BUFFER_SIZE/2			// mono audio buffer size (in samples)

void myAlgorithmInit(void);
void myAlgorithm(int16_t *stereoIn, int16_t *stereoOut, int16_t stereoSize);
char* frequencyToNote(float32_t frequency);
void findMax(int16_t checkedRange);

extern struct MenuInfoStructure MyAlogorithmMenu;


#endif /* INC_PROJECTS_STM32746G_DISCOVERY_EXAMPLES_BSP_INC_MYALGORITHM_H_ */
