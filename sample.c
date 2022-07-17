/*
 * main.c - Source for playing a simple wav file.
 *
 * Author: Philip R. Simonson
 * Date  : 12/30/2021
 *
 *****************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dicsnd.h"

int main(void)
{
	dicsnd_t *snd;

	/* Create sound file. */
	snd = DIC_init();
	DIC_add_chunk(snd);
	if(DIC_errno != DIC_OKERR) {
		fprintf(stderr, "Error: %s\n", DIC_error());
		DIC_free(snd);
		return 1;
	}
	DIC_add_s16(snd, 8, 250, 100, 550, 452, 500, 65, 625, 0);
	DIC_add_s16(snd, 8, 500, 1000, 500, 250, 100, 42, 0, 0);
	DIC_add_chunk(snd);
	if(DIC_errno != DIC_OKERR) {
		fprintf(stderr, "Error: %s\n", DIC_error());
		DIC_free(snd);
		return 1;
	}
	DIC_add_s16(snd, 8, 250, 440, 532, 250, 100, 42, 0, 0);
	DIC_print(snd);
	DIC_write(snd, "sample.dic");
	if(DIC_errno != DIC_OKERR) {
		fprintf(stderr, "Error: %s\n", DIC_error());
		DIC_free(snd);
		return 1;
	}
	DIC_free(snd);

	return 0;
}
