#include "audio.h"

#include "log.h"
#include "utils.h"

#include <SDL_audio.h>
#include <SDL_error.h>
#include <SDL_stdinc.h>

#define TWO_PI		   (M_PI * 2.0f)
#define FREQUENCY	   44100.0f
#define SINE_FREQUENCY 1000.0f

static struct sample_data_t {
	double position;
	double per_sine;
} SampleData;

static SDL_AudioDeviceID device = 0;
static SDL_AudioSpec spec;

static void audio_callback(void *userdata, uint8_t *stream, int32_t lenght);

int8_t audio_init(void) {
	SDL_AudioSpec desired_spec;

	/* Initialize sample data structure. */
	SampleData.position = 0.0f;
	SampleData.per_sine = SINE_FREQUENCY / FREQUENCY;

	/* Set desired audio specification. */
	desired_spec.freq = FREQUENCY;
	desired_spec.format = AUDIO_U8;
	desired_spec.channels = 1;
	desired_spec.samples = 2048;
	desired_spec.callback = audio_callback;
	desired_spec.userdata = &SampleData;

	device = SDL_OpenAudioDevice(
		NULL, 0, &desired_spec, &spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE
	);
	if (device == 0) {
		return STATUS_ERROR;
	}

	log_info("Audio device created and initialized!");
	return STATUS_OK;
}

void audio_quit(void) {
	if (device != 0) {
		log_info("Closing audio device: %s...", SDL_GetAudioDeviceName(device, 0));
		SDL_CloseAudioDevice(device);
	}
}

void audio_pause(bool pause) {
	SDL_PauseAudioDevice(device, pause ? 1 : 0);
}

static void audio_callback(void *userdata, uint8_t *stream, int32_t lenght) {
	struct sample_data_t *sample = (struct sample_data_t *)userdata;

	for (int32_t i = 0; i < lenght; i += 1) {
		stream[i] = sin(sample->position / sample->per_sine * TWO_PI) * 127.5;
		sample->position += TWO_PI * sample->per_sine;
	}
}
