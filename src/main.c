#include "synth.h"

int main() {
    // Create a song that lasts half a minute (20 seconds)
    Song* song = create_song(4, 20.0f);

    // Create instruments
    Instrument guitar = { WAVE_PLUCK, { 0.1f, 1.0f, 0.7f, 0.2f } };  // Low E string
    Instrument drum = { WAVE_SINE, { 0.1f, 0.2f, 0.3f, 0.2f } };      // Low frequency sine wave
    Instrument cymbal = { WAVE_NOISE, { 0.001f, 0.2f, 0.6f, 0.3f } }; // High-frequency noise
    Instrument piano = { WAVE_SINE, { 0.01f, 0.5f, 0.5f, 0.3f } };   // Middle C

    // Create notes for each track
    // Track 1: Guitar playing a simple repetitive melody
    Note track1_notes[] = {
        { 0.0f, 0.5f, 352.0f, 1.0f, guitar },
        { 1.0f, 0.5f, 262.0f, 1.0f, guitar },
        { 2.0f, 0.5f, 453.0f, 1.0f, guitar },
        { 3.0f, 0.5f, 182.0f, 1.0f, guitar },
        // Repeat similar notes every 4 seconds for the duration of the song
    };
    size_t track1_note_count = sizeof(track1_notes) / sizeof(track1_notes[0]);

    // Track 2: Drum beat
    Note track2_notes[60]; // 30-second song with a beat every half second
    for (int i = 0; i < 60; ++i) {
        track2_notes[i] = (Note){ i * 0.5f, 0.5f, 60.0f, 5.0f, drum };
    }

    // Track 3: Cymbal crash every 5 seconds
    Note track3_notes[3]; // 3 cymbal crashes in 20 seconds
    for (int i = 0; i < 3; ++i) {
        track3_notes[i] = (Note){ i * 5.0f, 0.5f, 8000.0f, 1.0f, cymbal };
    }

    // Track 4: Piano melody
    Note track4_notes[] = {
        { 0.0f, 0.5f, 262.0f, 1.0f, piano },
        { 1.0f, 0.5f, 352.0f, 1.0f, piano },
        { 2.0f, 0.5f, 400.0f, 1.0f, piano },
        { 3.0f, 0.5f, 352.0f, 1.0f, piano },
        // Repeat similar notes every 4 seconds for the duration of the song
    };
    size_t track4_note_count = sizeof(track4_notes) / sizeof(track4_notes[0]);

    // Add tracks to song
    add_track(song, 0, track1_notes, track1_note_count);     // Guitar
    add_track(song, 1, track2_notes, 60);                   // Drum
    add_track(song, 2, track3_notes, 3);                    // Cymbal
    add_track(song, 3, track4_notes, track4_note_count);     // Piano

    // Save song to file
    save_song(song, "example.dic");

    // Load song from file
    Song* loaded_song = load_song("example.dic");

    // Play song
    play_song(loaded_song);

    // Free memory
    free_song(song);
    free_song(loaded_song);
	free_pluck_string();

    return 0;
}

