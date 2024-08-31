#ifndef SYNTH_H
#define SYNTH_H

#include <stddef.h>
#include <stdint.h>

#define SAMPLE_RATE 44100

typedef enum {
    WAVE_SINE,
    WAVE_SQUARE,
    WAVE_TRIANGLE,
    WAVE_SAWTOOTH,
	WAVE_PLUCK,
    WAVE_NOISE,
    WAVE_SILENCE
} WaveType;

typedef struct {
    float *buffer;
    size_t buffer_size;
    size_t position;
    float decay;
} PluckString;

typedef struct {
    float attack;    // in seconds
    float duration;  // in seconds
    float sustain;   // sustain level (0 to 1)
    float release;   // in seconds
} Envelope;

typedef struct {
    WaveType wave_type;
    Envelope envelope;
} Instrument;

typedef struct {
    float time;      // in seconds
    float duration;  // in seconds
    float frequency; // note pitch
	float amplitude; // note volume
    Instrument instrument;
} Note;

typedef struct {
    Note* notes;
    size_t note_count;
} Track;

typedef struct {
    Track* tracks;
    size_t track_count;
    float duration; // in seconds
} Song;

void free_pluck_string();

Song* create_song(size_t track_count, float duration);
void add_track(Song* song, size_t track_index, Note* notes, size_t note_count);
void free_song(Song* song);

void save_song(const Song* song, const char* filename);
Song* load_song(const char* filename);
void play_song(Song* song);

#endif // SYNTH_H

