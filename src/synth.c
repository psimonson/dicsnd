#include "synth.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define TWO_PI 6.283185307179586476925286766559

static float generate_wave_sample(WaveType wave_type, Note *note, float time) {
	if (note->amplitude > 5.0f) { note->amplitude = 5.0f; }
	else if (note->amplitude < 0.0f) { note->amplitude = 0.0f; }

    switch (wave_type) {
        case WAVE_SINE:
            return sinf(TWO_PI * note->frequency * time);
        case WAVE_SQUARE:
            return (sinf(TWO_PI * note->frequency * time) > 0) ? 1.0f : -1.0f;
        case WAVE_TRIANGLE:
            return asinf(sinf(TWO_PI * note->frequency * time)) * (2.0f / M_PI);
        case WAVE_SAWTOOTH:
            return (2.0f / M_PI) * (note->frequency * M_PI * fmodf(time, 1.0f / note->frequency) - (M_PI / 2.0f));
        case WAVE_NOISE:
            return ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        case WAVE_SILENCE:
            return 0.0f;
        default:
            return 0.0f;
    }
}

static float apply_envelope(const Note *note, float time) {
    if (time < note->instrument.envelope.attack) {
        return note->amplitude * (time / note->instrument.envelope.attack);
    } else if (time < note->instrument.envelope.attack + note->instrument.envelope.duration) {
        return note->amplitude * note->instrument.envelope.sustain;
    } else if (time < note->instrument.envelope.attack + note->instrument.envelope.duration + note->instrument.envelope.release) {
        return note->amplitude * (note->instrument.envelope.sustain * (1.0f - (time - note->instrument.envelope.attack - note->instrument.envelope.duration) / note->instrument.envelope.release));
    } else {
        return 0.0f;
    }
}

Song* create_song(size_t track_count, float duration) {
    Song* song = (Song*)malloc(sizeof(Song));
    song->tracks = (Track*)calloc(track_count, sizeof(Track));
    song->track_count = track_count;
    song->duration = duration;
    return song;
}

void add_track(Song* song, size_t track_index, Note* notes, size_t note_count) {
    if (track_index >= song->track_count) return;
    Track* track = &song->tracks[track_index];
    track->notes = (Note*)malloc(note_count * sizeof(Note));
    memcpy(track->notes, notes, note_count * sizeof(Note));
    track->note_count = note_count;
}

void free_song(Song* song) {
    for (size_t i = 0; i < song->track_count; ++i) {
        free(song->tracks[i].notes);
    }
    free(song->tracks);
    free(song);
}

void save_song(const Song* song, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) return;

    fwrite("SONG", 4, 1, file);  // Write header
    fwrite(&song->track_count, sizeof(size_t), 1, file);
    fwrite(&song->duration, sizeof(float), 1, file);

    for (size_t i = 0; i < song->track_count; ++i) {
        fwrite(&song->tracks[i].note_count, sizeof(size_t), 1, file);
        fwrite(song->tracks[i].notes, sizeof(Note), song->tracks[i].note_count, file);
    }

    fclose(file);
}

Song* load_song(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    char header[5] = {0};
    fread(header, 4, 1, file);
    if (strcmp(header, "SONG") != 0) {
        fclose(file);
        return NULL;
    }

    Song* song = (Song*)malloc(sizeof(Song));
    fread(&song->track_count, sizeof(size_t), 1, file);
    fread(&song->duration, sizeof(float), 1, file);

    song->tracks = (Track*)calloc(song->track_count, sizeof(Track));
    for (size_t i = 0; i < song->track_count; ++i) {
        fread(&song->tracks[i].note_count, sizeof(size_t), 1, file);
        song->tracks[i].notes = (Note*)malloc(song->tracks[i].note_count * sizeof(Note));
        fread(song->tracks[i].notes, sizeof(Note), song->tracks[i].note_count, file);
    }

    fclose(file);
    return song;
}

void play_song(Song* song) {
    size_t samples = (size_t)(song->duration * SAMPLE_RATE);
    float* buffer = (float*)malloc(samples * sizeof(float));
    memset(buffer, 0, samples * sizeof(float));

    for (size_t t = 0; t < song->track_count; ++t) {
        const Track* track = &song->tracks[t];
        for (size_t n = 0; n < track->note_count; ++n) {
            Note* note = &track->notes[n];
            size_t start_sample = (size_t)(note->time * SAMPLE_RATE);
            size_t end_sample = start_sample + (size_t)(note->duration * SAMPLE_RATE);

            for (size_t i = start_sample; i < end_sample && i < samples; ++i) {
                float time = (float)(i - start_sample) / SAMPLE_RATE;
                float sample = generate_wave_sample(note->instrument.wave_type, note, time);
                sample *= apply_envelope(note, time);
                buffer[i] += sample;
            }
        }
    }

    // Normalize buffer
    float max_sample = 0.0f;
    for (size_t i = 0; i < samples; ++i) {
        if (fabs(buffer[i]) > max_sample) {
            max_sample = fabs(buffer[i]);
        }
    }
    if (max_sample > 1.0f) {
        for (size_t i = 0; i < samples; ++i) {
            buffer[i] /= max_sample;
        }
    }

    // Output buffer to stdout as raw audio
    for (size_t i = 0; i < samples; ++i) {
        int16_t sample = (int16_t)(buffer[i] * 32767.0f);
        fwrite(&sample, sizeof(int16_t), 1, stdout);
    }

    free(buffer);
}

