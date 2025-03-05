#ifndef __PICO_AUDIO_RECORDER_
#define __PICO_AUDIO_RECORDER_
#include "hardware/pio.h"
#include "ff.h"

#define INMP441_pio         	pio0
#define INMP441_SM          	0

#define INMP441_SCK          	18
#define INMP441_SD           	20

#define INMP441_CHANNEL_STEREO	2
#define INMP441_CHANNEL_MONO	1
#define INMP441_SAMPLE_RATE_32K  	32000
#define INMP441_SAMPLE_RATE_44_1K  	44100

#define INMP441_BIT_PER_SAMPLE_32	32
#define INMP441_BIT_PER_SAMPLE_16	16

#define BUTTON_PIN      		21
#define RED_PIN					16
#define GREEN_PIN				17

#define PIO_DMA_BUFFER_SIZE 8192

enum RECORDER_STATE{
    STATE_STOP=0,
	STATE_STOPING,
    STATE_START_RECORDING,
    STATE_RECORDING,
};

typedef struct _WaveHeader{
	char riff[4];
	uint32_t size;
	char wave[4];
	char fmt[4];
	uint32_t fmt_size;
	uint16_t format; //1:PCM
	uint16_t channels; // channels
	uint32_t sampleRate;  // sample rate
	uint32_t rbc;//sampleRate*bitsPerSample*channels/8
	uint16_t bc; //bitsPerSample*channels/8
	uint16_t bitsPerSample; //bitsPerSample
	char data[4];
	uint32_t data_size;
} WAVE_HEADER;

void inmp441_pio_init(PIO pio,  uint sm, uint sd_pin, uint sideset_pin);
void inmp441_starting_recording_to_file_wav();
uint8_t get_recorder_state();
void    set_recorder_state(uint8_t state);

#endif
