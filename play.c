/*
 * play.c - Source for playing a DIC sound file.
 *
 * Author: Philip R. Simonson
 * Date  : 07/16/2022
 *
 ****************************************************************************
 */

#define _DEFAULT_SOURCE
#include <alsa/asoundlib.h>
#include <alsa/pcm.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dicsnd.h"

#define SAMPLE_RATE 48000
#define SAMPLE_COUNT 4096

static char *device = "default";

int main(void)
{
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_t *pcm;
	dicsnd_t *snd;

	if(snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
		printf("Playback open error.\n");
		return 1;
	}

	snd_pcm_hw_params_alloca(&hw_params);
	snd_pcm_hw_params_any(pcm, hw_params);
	snd_pcm_hw_params_set_access(pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(pcm, hw_params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(pcm, hw_params, 1);
	snd_pcm_hw_params_set_rate(pcm, hw_params, SAMPLE_RATE, 0);
	snd_pcm_hw_params_set_periods(pcm, hw_params, 0, 0);
	snd_pcm_hw_params_set_period_time(pcm, hw_params, 10000, 0);
	snd_pcm_hw_params(pcm, hw_params);

	/* Load DIC sample file. */
	snd = DIC_load("sample.dic");
	if(!snd) {
		fprintf(stderr, "Error: %s\n", DIC_error());
		snd_pcm_close(pcm);
		return 1;
	}

	/* Loop through sound file and play it. */
	for(unsigned int i = 0; i < snd->nchunks; i++) {
		for(unsigned int j = 0; j < snd->chunks[i].count; j++) {
			short int *samples = DIC_square_wave(SAMPLE_RATE,
							SAMPLE_COUNT,
							snd->chunks[i].s16[j]);
			snd_pcm_writei(pcm, samples, SAMPLE_COUNT);
			free(samples);
		}
	}

	snd_pcm_drain(pcm);
	snd_pcm_close(pcm);
	DIC_free(snd);
	return 0;
}
