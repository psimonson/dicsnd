/*
 * dicsnd.c - Source for a simple audio file format.
 *
 * Author: Philip R. Simonson
 * Date  : 12/30/2021
 *
 *****************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "dicsnd.h"

/* For error checking. */
unsigned char DIC_errno;

/* Errors for DIC music.
 */
const char *DIC_error(void)
{
	switch(DIC_errno) {
		case DIC_RDERR:
			return "Reading file failed";
		case DIC_WRERR:
			return "Writing file failed";
		case DIC_HLPERR:
			return "Please run DIC_add_chunk() - No chunks found";
		case DIC_BADERR:
			return "DIC signature not found";
		case DIC_MEMERR:
			return "Out of memory";
		case DIC_OKERR:
		break;
		default:
			return "Unknown error";
	}
	return "No error";
}

/* Initialize the dicsnd structure.
 */
dicsnd_t *DIC_init(void)
{
	static dicsnd_t snd;
	memcpy(snd.tag, "PRS", 4);
	snd.nchunks = 0;
	snd.chunks = NULL;
	return &snd;
}

/* Free sound resources.
 */
void DIC_free(dicsnd_t *snd)
{
	if(snd != NULL) {
		unsigned int i;

		for(i = 0; i < snd->nchunks; i++)
			free(snd->chunks[i].s16);
		free(snd->chunks);
	}
}

/* Add chunk to sound.
 */
void DIC_add_chunk(dicsnd_t *snd)
{
	dicchk_t *chunks;

	chunks = (dicchk_t*)realloc(snd->chunks, sizeof(dicchk_t) * (snd->nchunks + 1));
	if(!chunks) {
		DIC_errno = DIC_MEMERR;
		return;
	}

	++snd->nchunks;
	memcpy(chunks[snd->nchunks - 1].tag, "CHNK", 4);
	chunks[snd->nchunks - 1].count = 0;
	chunks[snd->nchunks - 1].s16 = NULL;
	snd->chunks = chunks;
}

/* Add data to chunk.
 */
void DIC_add_s16(dicsnd_t *snd, int count, ...)
{
	const unsigned int size = snd->chunks[snd->nchunks - 1].count + count;
	short int *s16;
	va_list ap;
	int i;

	if(count <= 0) return;
	if(!snd->chunks) {
		DIC_errno = DIC_HLPERR;
		return;
	}

	s16 = (short int *)realloc(snd->chunks[snd->nchunks - 1].s16, sizeof(short int) * size);
	if(!s16) {
		DIC_errno = DIC_MEMERR;
		return;
	}

	va_start(ap, count);
	for(i = 0; i < count; i++) {
		short int tmp = va_arg(ap, int);
		s16[snd->chunks[snd->nchunks - 1].count + i] = tmp;
	}
	va_end(ap);

	snd->chunks[snd->nchunks - 1].count = size;
	snd->chunks[snd->nchunks - 1].s16 = s16;
}

/* Load DIC sound from file.
 */
dicsnd_t *DIC_load(const char *filename)
{
	dicsnd_t *snd;
	FILE *fp;

	/* Open sound file. */
	fp = fopen(filename, "rb");
	if(!fp) {
		DIC_errno = DIC_RDERR;
		return NULL;
	}

	/* Initialize DIC sound. */
	snd = DIC_init();
	if(!snd) {
		DIC_errno = DIC_MEMERR;
		fclose(fp);
		return NULL;
	}

	/* Get header from file. */
	if(fread(snd, sizeof(dicsnd_t) - sizeof(void*), 1, fp) != 1) {
		DIC_errno = DIC_RDERR;
		DIC_free(snd);
		fclose(fp);
		return NULL;
	}

	/* Check header for validation. */
	if(memcmp(snd->tag, "PRS", 4) != 0) {
		DIC_errno = DIC_BADERR;
		DIC_free(snd);
		fclose(fp);
		return NULL;
	}

	/* Add all chunks to snd file. */
	snd->chunks = (dicchk_t *)calloc(snd->nchunks, sizeof(dicchk_t));
	if(!snd->chunks) {
		DIC_errno = DIC_MEMERR;
		DIC_free(snd);
		fclose(fp);
		return NULL;
	}

	/* Loop through and add all data. */
	for(unsigned int i = 0; i < snd->nchunks; i++) {
		if(fread(snd->chunks[i].tag, sizeof(unsigned char), 4, fp) != 4) {
			DIC_errno = DIC_RDERR;
			DIC_free(snd);
			fclose(fp);
			return NULL;
		}

		if(fread(&snd->chunks[i].count, sizeof(unsigned int), 1, fp) != 1) {
			DIC_errno = DIC_RDERR;
			DIC_free(snd);
			fclose(fp);
			return NULL;
		}

		snd->chunks[i].s16 = (short int *)calloc(snd->chunks[i].count, sizeof(short int));
		if(!snd->chunks[i].s16) {
			DIC_errno = DIC_MEMERR;
			DIC_free(snd);
			fclose(fp);
			return NULL;
		}

		if(fread(snd->chunks[i].s16, sizeof(short int), snd->chunks[i].count, fp) != snd->chunks[i].count) {
			DIC_errno = DIC_RDERR;
			DIC_free(snd);
			fclose(fp);
			return NULL;
		}
	}

	fclose(fp);
	DIC_errno = DIC_OKERR;
	return snd;
}

/* Write DIC sound to file.
 */
void DIC_write(dicsnd_t *snd, const char *filename)
{
	FILE *fp;

	fp = fopen(filename, "wb");
	if(!fp) {
		DIC_errno = DIC_WRERR;
		return;
	}

	if(fwrite(snd, sizeof(dicsnd_t) - sizeof(void*), 1, fp) != 1) {
		DIC_errno = DIC_WRERR;
		fclose(fp);
		remove(filename);
		return;
	}

	for(unsigned int i = 0; i < snd->nchunks; i++) {
		if(fwrite(snd->chunks[i].tag, sizeof(unsigned char), 4, fp) != 4) {
			DIC_errno = DIC_WRERR;
			fclose(fp);
			remove(filename);
			return;
		}

		if(fwrite(&snd->chunks[i].count, sizeof(unsigned int), 1, fp) != 1) {
			DIC_errno = DIC_WRERR;
			fclose(fp);
			remove(filename);
			return;
		}

		if(fwrite(snd->chunks[i].s16, sizeof(short int), snd->chunks[i].count, fp) != snd->chunks[i].count) {
			DIC_errno = DIC_WRERR;
			fclose(fp);
			remove(filename);
			return;
		}
	}

	fclose(fp);
	DIC_errno = DIC_OKERR;
}

/* Print all info about the DIC sound.
 */
void DIC_print(dicsnd_t *snd)
{
	printf("DIC Info\n============\n");
	printf("Chunks: %u\n===============\n", snd->nchunks);
	for(unsigned int i = 0; i < snd->nchunks; i++) {
		printf("Chunk Info %u\n==================\n", i);
		printf("Total 16 bit integers: %u\n", snd->chunks[i].count);
		printf("===================\n");
	}
}

/* Generating a square wave form.
 */
short int *DIC_square_wave(short unsigned int sample_rate,
	size_t sample_count, int freq)
{
	int full_cycle = (float)sample_rate / (float)freq;
	int half_cycle = full_cycle / 2.0f;
	short int *samples;
	int cycle_index;
	long unsigned int i;

	samples = (short int *)calloc(sample_count, sizeof(short int));
	if(!samples) {
		return NULL;
	}

	cycle_index = 0;
	for(i = 0; i < sample_count; i++) {
		samples[i] = cycle_index < half_cycle ? 10000 : -10000;
		cycle_index = (cycle_index + 1) % full_cycle;
	}
	return samples;
}
