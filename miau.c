#include "miau.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define AMPLITUDE 28000
#define PI2 6.28318530718

struct mi_Note {
    double frequency;
    unsigned int effect;
};

struct mi_Pattern {
    mi_Note notes[MIAU_MAX_NOTES];
};

struct mi_Channel {
    int waveform;
    double phase;
    int pattern_count;
    mi_Pattern patterns[MIAU_MAX_PATTERNS];
};

struct mi_Sequencer {
    int playing;
    float time;
    float speed;
    int current_line;
    mi_Channel channels[MIAU_MAX_CHANNELS];
};

typedef double(*mi_WaveFunc)(mi_Channel* c);

struct mi_System {
    double sample_rate;
    int channels;
    int samples;
    mi_Sequencer sequencers[MIAU_MAX_SEQUENCERS];
};

static mi_Config _config = {
    .freq = 44100,
    .channels = 1,
    .samples = 2048
};

static mi_Note _break = {0.0, 0};

const double frequency_list[] = {
    0.0,
    // C     // C#    // D     // D#    // E     // F     // F#    // G     // G#    // A     // A#    // B
    16.35,   17.32,   18.35,   19.45,   20.60,   21.83,   23.12,   24.50,   25.96,   27.50,   29.14,   30.87,   // octave 1
    32.70,   34.65,   36.71,   38.89,   41.20,   43.65,   46.25,   49.00,   51.91,   55.00,   58.27,   61.74,   // octave 2
    65.41,   69.30,   73.42,   77.78,   82.41,   87.31,   92.50,   98.00,   103.83,  110.00,  116.54,  123.47,  // octave 3
    130.81,  138.59,  146.83,  155.56,  164.81,  174.61,  185.00,  196.00,  207.65,  220.00,  233.08,  246.94,  // octave 4
    261.63,  277.18,  293.66,  311.13,  329.63,  349.23,  369.99,  392.00,  415.30,  440.00,  466.16,  493.88,  // octave 5 (A4 defined as 440 Hz)
    523.25,  554.37,  587.33,  622.25,  659.25,  698.46,  739.99,  783.99,  830.61,  880.00,  932.33,  987.77,  // octave 6
    1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53, // octave 7
    2093.00, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07  // octave 8
};

mi_System* miau_init(const mi_Config* c) {
    c = c ? c : &_config;
    mi_System* s = malloc(sizeof(*s));
    if (!s) {
        fprintf(stderr, "[miau] failed to alloc memory for miau system\n");
        return NULL;
    }
    memset(s, 0, sizeof(*s));
    s->sample_rate = c->freq;
    s->channels = c->channels;
    s->samples = c->channels;
    return s;
}

void miau_quit(mi_System* s) {
    if (!s) return;
    free(s);
}

static double _process_channel(mi_System* s, int line, mi_Channel* ch) {
    double sample = 0.0;
    mi_Note* n = ch->patterns[0].notes + line;
    double freq = n->frequency;
    switch (ch->waveform) {
        case MIAU_SINE:
            sample = sin(ch->phase * PI2);
            break;
        case MIAU_SQUARE:
            sample = ch->phase < 0.5 ? 1.0 : -1.0;
            break;
        case MIAU_SAWTOOTH:
            sample = 2.0 * fmod(ch->phase, 1.0) - 1.0;
            break;
        case MIAU_TRIANGLE:
            sample = 2.0 * fabs(ch->phase - 0.5) - 1.0;
            break;
        case MIAU_NOISE:
            sample = (double)rand() / RAND_MAX * 2.0 - 1.0;
            break;
    }
    ch->phase += freq / s->sample_rate;
    if (ch->phase >= 1.0) ch->phase -= 1.0;

    return sample;
}

void miau_generate_sample(mi_System* s, unsigned char* stream, int len) {
    if (!stream) return;
    if (len <= 0) return;
    memset(stream, 0, len);
    short* buffer = (short*)stream;
    len = len / sizeof(short);

    mi_Sequencer* seq = s->sequencers;

    seq->time += ((float)len / (float)s->sample_rate) * seq->speed;
    if (seq->time >= 1.f) {
        seq->time = 0.f;
        seq->current_line += 1;
    }

    if (seq->current_line >= MIAU_MAX_NOTES) {
        seq->current_line = 0;
    }

    int line = seq->current_line;

    fprintf(stdout, "line(%d) time(%f)\n", line, seq->time);
    for (int i = 0; i < len; i++) {
        double sample = 0;
        for (int j = 0; j < MIAU_MAX_CHANNELS; j++) {
            sample += _process_channel(s, line, &(seq->channels[j]));
        }
        sample = (sample / MIAU_MAX_CHANNELS) * AMPLITUDE;
        buffer[i] += (short)sample;
    }
}

// Sequencer

mi_Sequencer* miau_get_sequencer(mi_System* s, int index) {
    if (!s) return NULL;
    if (index < 0 || index >= MIAU_MAX_SEQUENCERS) return NULL;
    return s->sequencers + index;
}

void miau_sequencer_set_speed(mi_Sequencer* seq, float speed) {
    if (!seq) return;
    seq->speed = speed;
}

void miau_sequencer_set_playing(mi_Sequencer* seq, int playing) {
    if (!seq) return;
    seq->playing = playing;
}

mi_Channel* miau_sequencer_get_channel(mi_Sequencer* seq, int index) {
    if (!seq) return NULL;
    if (index < 0 || index > MIAU_MAX_CHANNELS) return NULL;
    return seq->channels + index;
}

// Channel
void miau_channel_set_waveform(mi_Channel* c, int waveform) {
    if (!c) return;
    if (waveform < MIAU_SINE || waveform > MIAU_NOISE) return;
    c->waveform = waveform;
}

mi_Pattern* miau_channel_get_pattern(mi_Channel* c, int index) {
    if (!c) return NULL;
    if (index < 0 || index > MIAU_MAX_PATTERNS) return NULL;
    return c->patterns + index;
}

// Pattern
void miau_pattern_set_note(mi_Pattern* p, int index, int note, int effect) {
    if (!p) return;
    if (index < 0 || index > MIAU_MAX_NOTES) return;
    if (note < MIAU_BREAK || note > MIAU_B7) return;
    mi_Note* n = p->notes + index;
    n->frequency = frequency_list[note];
    n->effect = effect;
}

#if 0
/* Internal only */
int miau_init_channel_internal(mi_Channel* c, int waveform, int size) {
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
    if (size <= 0 || size > 256) return c;
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
#endif