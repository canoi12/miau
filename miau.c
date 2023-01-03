#include "miau.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_CHANNELS 32
#define AMPLITUDE 5000
#define PI2 6.28318530718

struct mi_Note {
    double frequency;
};

struct mi_Pattern {
    int num_notes;
    int current_index;
    mi_Note* notes;
};

struct mi_Sequencer {
};

typedef double(*mi_WaveFunc)(mi_Channel* c);

struct mi_Channel {
    int waveform;
    int num_notes;
    int current_note;
    mi_Note* notes;
    double phase;
};

struct mi_Sound {
    int num_channels;
    int is_playing;
    mi_Channel* channels;
};

struct mi_AudioSystem {
    double sample_rate;
    int channels;
    int samples;
    mi_Sound* current_sound;
};

static mi_Config _config = {
    .freq = 44100,
    .channels = 1,
    .samples = 2048
};
static struct mi_AudioSystem _system;

const double frequency_list[] = {
    0.0,
    // C   // C#  // D   // D#
    16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87, // octave 1
    32.70, 34.65, 36.71, 38.89, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.27, 61.74, // octave 2
    65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110.00, 116.54, 123.47, // octave 3
    130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94, // octave 4
    261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88, // octave 5 (A4 defined as 440 Hz)
    523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77, // octave 6
    1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53, // octave 7
    2093.00, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07 // octave 8
};

int miau_init(const mi_Config* c) {
    c = c ? c : &_config;
    struct mi_AudioSystem *as = &_system;
    memset(as, 0, sizeof(*as));
    as->sample_rate = c->freq;
    as->channels = c->channels;
    as->samples = c->channels;
    return 0;
}

void miau_quit(void) {}

double _generate_sample(mi_Channel* ch) {
    // mi_Note* note = &(ch->notes[ch->current_note]);
    mi_Note* note = &(ch->notes[1]);
    double freq = note->frequency;
    double phase = ch->phase;
    double sample = 0.0;
    switch (ch->waveform) {
	case MIAU_SINE:
	    sample = sin(phase * PI2);
	    break;
	case MIAU_SQUARE:
	    sample = phase < 0.5 ? 1.0 : -1.0;
	    break;
	case MIAU_SAWTOOTH:
	    sample = 2.0 * fmod(phase, 1.0) - 1.0;
	    break;
	case MIAU_TRIANGLE:
	    sample = 2.0 * fabs(phase - 0.5) - 1.0;
	    break;
	case MIAU_NOISE:
	    sample = (double)rand() / RAND_MAX * 2.0 - 1.0;
	    break;
    }
    phase += freq / _system.sample_rate;
    if (phase >= 1.0) phase -= 1.0;
    ch->phase = phase;
    // printf("sample: %f, phase: %f\n", sample, phase);
    return sample;
}

void miau_generate_sample(short* stream, int len) {
    mi_Sound* snd = _system.current_sound; 
    if (!snd) return;
    if (!snd->is_playing) return;
    // printf("testando\n");
    double sample;
    mi_Channel* ch = &(snd->channels[0]);
    // printf("waveform: %d\n", ch->waveform);
    // printf("note freq: %f\n", ch->notes[ch->current_note].frequency);
    for (int i = 0; i < len; i++) {
	sample = _generate_sample(ch);
	stream[i] += AMPLITUDE * sample;
    }
}

/* Internal only */
int miau_init_channel_internal(mi_Channel* c, int waveform, int size) {
    if (!c) return -1;
    if (waveform < MIAU_SINE || waveform > MIAU_NOISE) return -1;
    if (size <= 0 || size > 256) return -1;
    c->waveform = waveform;
    c->current_note = 0;
    c->num_notes = size;
    c->phase = 0.0;
    c->notes = malloc(sizeof(mi_Note) * size);
    if (!c->notes) {
	fprintf(stderr, "Failed to alloc memory for channel notes\n");
	exit(EXIT_FAILURE);
    }
    return 0;
}

mi_Channel* miau_create_channel(int waveform, int size) {
    mi_Channel* c = NULL;
    if (waveform < MIAU_SINE || waveform > MIAU_BREAK) return c;
    if (size < 0) return c;
    c = malloc(sizeof(*c));
    if (c == NULL) {
	fprintf(stderr, "Failed to alloc memory for channel\n");
	exit(EXIT_FAILURE);
    }
    if (miau_init_channel_internal(c, waveform, size) < 0) free(c);
    return c;
}

void miau_destroy_channel(mi_Channel* c) {
    if (!c) return;
    if (c->notes) free(c->notes);
    free(c);
}

void miau_set_channel_waveform(mi_Channel* c, int waveform) {
    if (!c) return;
    if (waveform < MIAU_SINE || waveform > MIAU_BREAK) return;
    c->waveform = waveform;
}

void miau_resize_channel(mi_Channel* c, int size) {
    if (!c) return;
    if (size < 0) return;
    c->notes = realloc(c->notes, size);
    c->num_notes = size;
    if (c->current_note >= size) c->current_note = size-1;
}

void miau_channel_push(mi_Channel* c, int note, int effect) {
    if (!c) return;
    if (note < MIAU_BREAK || note > MIAU_B7) return;
    mi_Note* n = &(c->notes[c->current_note]);
    n->frequency = frequency_list[note];
    c->current_note++;
}

void miau_channel_seek(mi_Channel* c, int index) {
    if (!c) return;
    if (index < 0 || index >= c->num_notes) return;
    c->current_note = index;
}

int miau_channel_tell(mi_Channel* c) {
    if (!c) return -1;
    return c->current_note;
}

void miau_channel_step(mi_Channel* c) {
    if (!c) return;
    if (c->current_note >= c->num_notes) return;
	c->current_note++;
}

mi_Sound* miau_create_sound(int channels, int size) {
    mi_Sound* snd = NULL;
    if (channels <= 0 || channels > MAX_CHANNELS) return snd;
    if (size <= 0 || size > 256) return snd;

    snd = malloc(sizeof(*snd));
    if (!snd) {
	fprintf(stderr, "Failed to alloc memory for sound\n");
	exit(EXIT_FAILURE);
    }

    snd->num_channels = channels;
    snd->channels = malloc(sizeof(mi_Channel) * channels);
    if (!snd->channels) {
	fprintf(stderr, "Failed to alloc memory for sound channels\n");
	exit(EXIT_FAILURE);
    }
    memset(snd->channels, 0, sizeof(mi_Channel) * channels);
    for (int i = 0; i < snd->num_channels; i++) {
	miau_init_channel_internal(&(snd->channels[i]), MIAU_SINE, size);
    }
    snd->is_playing = 0;
    return snd;
}

void miau_destroy_sound(mi_Sound* snd) {
    if (!snd) return;
    if (_system.current_sound == snd) _system.current_sound = NULL;
    if (!snd->channels) {
	free(snd);
	return;
    }
    for (int i = 0; i < snd->num_channels; i++) {
	mi_Channel* c = &(snd->channels[i]);
	if (c->notes) free(c->notes);
    }
    free(snd->channels);
    free(snd);
}

mi_Channel* miau_sound_get_channel(mi_Sound* snd, int index) {
    if (!snd) return NULL;
    if (index < 0 || index >= snd->num_channels) return NULL;
    return &(snd->channels[index]);
}

void miau_play_sound(mi_Sound* snd) {
    if (!snd) return;
    if (_system.current_sound && snd != _system.current_sound) miau_stop_sound(_system.current_sound);
    _system.current_sound = snd;
    snd->is_playing = MIAU_TRUE;
}

void miau_pause_sound(mi_Sound* snd) {
    if (!snd) return;
    snd->is_playing = MIAU_FALSE;
}

void miau_stop_sound(mi_Sound* snd) {
    if (!snd) return;
    _system.current_sound = NULL;
    snd->is_playing = MIAU_FALSE;
}
