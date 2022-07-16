/*
 * dicsnd.h - Header for a simple audio file format.
 *
 * Author: Philip R. Simonson
 * Date  : 12/30/2021
 *
 *****************************************************************************
 */

#ifndef DICSND_H
#define DICSND_H

typedef struct dicchk {
	unsigned char tag[4];
	unsigned int count;
	short int *s16;
} dicchk_t;

typedef struct dicsnd {
	unsigned char tag[4];
	unsigned int nchunks;
	dicchk_t *chunks;
} dicsnd_t;

enum { DIC_OKERR, DIC_MEMERR, DIC_HLPERR, DIC_WRERR, DIC_UNKERR };
extern unsigned char DIC_errno;

/* Errors for DIC music. */
const char *DIC_error(void);

/* Initialize the dicsnd structure. */
dicsnd_t *DIC_init(void);

/* Free sound resources. */
void DIC_free(dicsnd_t *snd);

/* Add chunk to sound. */
void DIC_add_chunk(dicsnd_t *snd);

/* Add data to chunk. */
void DIC_add_s16(dicsnd_t *snd, int count, ...);

/* Write DIC sound to file on disk. */
void DIC_write(dicsnd_t *snd, const char *filename);

/* Print info about DIC sound. */
void DIC_print(dicsnd_t *snd);

/* Generate square wave form. */
short int *DIC_square_wave(size_t sample_count, int freq);

#endif
