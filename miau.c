#include "miau.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define AMPLITUDE 28000
#define PI2 6.28318530718

#define EVENT_MASK(event) ((event & 0xf0000000) >> 28)
#define NOTE_MASK(event) ((event & 0x0f000000) >> 24)
#define OCTAVE_MASK(event) ((event & 0x00f00000) >> 20)
#define EFFECT_MASK ((event & 0x000f0000) >> 16)
#define ARG0_MASK ((event & 0x0000f000) >> 12)
#define ARG1_MASK ((event & 0x00000f00) >> 8)

static const char* note_symbols[] = {"C-", "C#", "D-", "D#", "E", "F-", "F#", "G-", "G#", "A-", "A#", "B-"};

struct mi_Note {
    char value;
    double frequency;
    unsigned int effect;
};

struct mi_Pattern {
    mi_Event events[MIAU_MAX_NOTES];
};

struct mi_Channel {
    int waveform;
    double phase;
    double gain;
    double pulse_width;
    int pattern_count;
    mi_Note note;
    mi_Pattern patterns[MIAU_MAX_PATTERNS];
};

struct mi_Frame {
    char data[MIAU_MAX_CHANNELS];
};

struct mi_Sequencer {
    int playing;
    float time;
    float speed;
    int current_line, current_frame;
    mi_Channel channels[MIAU_MAX_CHANNELS];
    mi_Frame frames[MIAU_MAX_FRAMES];
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
    s->samples = c->samples;
    return s;
}

void miau_quit(mi_System* s) {
    if (!s) return;
    free(s);
}

void miau_save_project(mi_System* s, const char* filename) {
    if (!s || !filename) return;
    FILE* fp = fopen(filename, "wb");
    for (int i = 0; i < MIAU_MAX_CHANNELS; i++) {
        mi_Channel* ch = s->sequencers[0].channels + i;
        fwrite(ch->patterns, sizeof(mi_Pattern), MIAU_MAX_PATTERNS, fp);
    }
    fclose(fp);
}

static double _process_channel(mi_System* s, mi_Channel* ch) {
    double sample = 0.0;
    mi_Note* n = &ch->note;
    double freq = n->frequency;
    switch (ch->waveform) {
        case MIAU_SINE:
            sample = sin(ch->phase * PI2);
            break;
        case MIAU_SQUARE:
            sample = ch->phase < ch->pulse_width ? 1.0 : -1.0;
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

    return sample * ch->gain;
}

static void s_update_sequencer(mi_System* s, mi_Sequencer* seq, int len) {
    seq->time += ((float)len / (float)s->sample_rate) * seq->speed;
    if (seq->time >= 1.f) {
        seq->time = 0.f;
        seq->current_line += 1;
    }

    if (seq->current_line >= MIAU_MAX_NOTES) {
        seq->current_line = 0;
        seq->current_frame += 1;
    }

    if (seq->current_frame >= MIAU_MAX_FRAMES) {
        seq->playing = 0;
    }

    for (int i = 0; i < MIAU_MAX_CHANNELS; i++) {
        fprintf(stdout, "[miau] channel %d\n", i);
        int pattern = (int)seq->frames[seq->current_frame].data[i];
        mi_Channel* ch = seq->channels + i;
        mi_Event ev = ch->patterns[pattern].events[seq->current_line];
        if (EVENT_MASK(ev) == MIAU_NOP) continue;
        if (EVENT_MASK(ev) == MIAU_BREAK) {
            memset(&ch->note, 0, sizeof(mi_Note));
            fprintf(stdout, "[miau] break;\n");
        }
        if (EVENT_MASK(ev) == MIAU_PLAY_NOTE) {
            mi_Note* n = &(ch->note);
            int note = NOTE_MASK(ev);
            int octave = OCTAVE_MASK(ev);
            int offset = octave * 12;
            n->frequency = frequency_list[note + offset];
            fprintf(stdout, "[miau] (%d) %s%d, freq %f\n", offset, note_symbols[note], octave, n->frequency);
        }
    }
}

void miau_generate_sample(mi_System* s, unsigned char* stream, int len) {
    if (!stream) return;
    if (len <= 0) return;
    memset(stream, 0, len);
    short* buffer = (short*)stream;
    len = len / sizeof(short);

    for (int i = 0; i < MIAU_MAX_SEQUENCERS; i++) {
        if (!s->sequencers[i].playing) continue;
        s_update_sequencer(s, s->sequencers + i, len);
    }

    // fprintf(stdout, "line(%d) time(%f)\n", , seq->time);
    for (int i = 0; i < len; i++) {
        double sample = 0;
        for (int j = 0; j < MIAU_MAX_SEQUENCERS; j++) {
            mi_Sequencer* seq = s->sequencers + j;
            if (!seq->playing) continue;
            for (int c = 0; c < MIAU_MAX_CHANNELS; c++)
                sample += _process_channel(s, seq->channels + c);
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

mi_Frame* miau_sequencer_get_frame(mi_Sequencer* seq, int index) {
    if (!seq) return NULL;
    if (index < 0 || index > MIAU_MAX_FRAMES) return NULL;
    return seq->frames + index;
}

// Channel
void miau_channel_set_waveform(mi_Channel* c, int waveform) {
    if (!c) return;
    if (waveform < MIAU_SINE || waveform > MIAU_NOISE) return;
    c->waveform = waveform;
    switch (waveform) {
        case MIAU_SQUARE: {
            c->gain = 1.0;
            c->pulse_width = 0.5;
        }
        break;
        case MIAU_TRIANGLE: {
            c->gain = 2.0;
        }
        break;
    }
}

mi_Pattern* miau_channel_get_pattern(mi_Channel* c, int index) {
    if (!c) return NULL;
    if (index < 0 || index > MIAU_MAX_PATTERNS) return NULL;
    return c->patterns + index;
}

// Frame

void miau_frame_set_pattern(mi_Frame* frame, int channel, char pattern) {
    if (!frame) return;
    if (channel < 0 || channel >= MIAU_MAX_CHANNELS) return;
    frame->data[channel] = pattern;
}

// Pattern

void miau_pattern_set_event(mi_Pattern* p, int index, mi_Event event) {
    if (!p) return;
    if (index < 0 || index >= MIAU_MAX_NOTES) return;
    p->events[index] = event;
}