#ifndef _MIAU_H_
#define _MIAU_H_

#define MIAU_VERSION "0.1.0"

#define MIAUAPI

#define MIAU_OK	    0
#define MIAU_ERROR -1

#define MIAU_TRUE  1
#define MIAU_FALSE 0

// MIAU DEFINITIONS
#define MIAU_MAX_SEQUENCERS 16
#define MIAU_MAX_CHANNELS 4
#define MIAU_MAX_PATTERNS 8
#define MIAU_MAX_FRAMES 64

#define MIAU_MAX_NOTES 16

// 0x010A23
// event|note octave|effect arg0|arg1

#define MIAU_CREATE_BREAK() (0x10000000)
#define MIAU_CREATE_NOTE(note, octave) (0x20000000 | ((note & 0xf) << 24) | ((octave & 0xf) << 20))

#define MIAU_GET_NOTE(event) (((event) & 0x0f000000) >> 24)
#define MIAU_GET_OCTAVE(event) (((event) & 0x00f00000) >> 20)

enum {
    MIAU_SINE = 1,
    MIAU_SQUARE,
    MIAU_SAWTOOTH,
    MIAU_TRIANGLE,
    MIAU_NOISE
};

enum {
    MIAU_NOP = 0,
    MIAU_BREAK,
    MIAU_PLAY_NOTE
};

// 00 00 00 00
// 0000|000|0 0000|0000 00000000

enum {
    MIAU_C,
    MIAU_CS,
    MIAU_D,
    MIAU_DS,
    MIAU_E,
    MIAU_F,
    MIAU_FS,
    MIAU_G,
    MIAU_GS,
    MIAU_A,
    MIAU_AS,
    MIAU_B
};

enum {
    MIAU_EV_NONE,
    MIAU_EV_ARPEGGIO
};

#if defined(__cplusplus)
extern "C" {
#endif

typedef unsigned int mi_Event;
typedef struct mi_Note mi_Note;
typedef struct mi_Channel mi_Channel;
typedef struct mi_Frame mi_Frame;
typedef struct mi_Pattern mi_Pattern;
typedef struct mi_Sequencer mi_Sequencer;

typedef struct mi_System mi_System;

typedef struct {
    int freq;
    int channels;
    int samples;
} mi_Config;

MIAUAPI mi_System* miau_init(const mi_Config* config);
MIAUAPI void miau_quit(mi_System*);

MIAUAPI void miau_save_project(mi_System*, const char* filename);

MIAUAPI void miau_generate_sample(mi_System*, unsigned char* stream, int len);

// Sequencer
MIAUAPI mi_Sequencer* miau_get_sequencer(mi_System*, int index);
MIAUAPI mi_Channel* miau_sequencer_get_channel(mi_Sequencer* seq, int index);
MIAUAPI mi_Frame* miau_sequencer_get_frame(mi_Sequencer* seq, int index);

MIAUAPI void miau_sequencer_set_speed(mi_Sequencer* seq, float speed);
MIAUAPI float miau_sequencer_get_speed(mi_Sequencer* seq);
MIAUAPI void miau_sequencer_set_playing(mi_Sequencer* seq, int playing);
MIAUAPI int miau_sequencer_get_playing(mi_Sequencer* seq);
MIAUAPI void miau_sequencer_restart(mi_Sequencer* seq);

// Channel
MIAUAPI void miau_channel_set_waveform(mi_Channel*, int waveform);
MIAUAPI void miau_channel_play_event(mi_Channel*, mi_Event ev);
MIAUAPI mi_Event miau_channel_get_event(mi_Channel*);
MIAUAPI mi_Pattern* miau_channel_get_pattern(mi_Channel*, int index);

// Frame
MIAUAPI void miau_frame_set_pattern(mi_Frame*, int channel, char pattern);

// Pattern
MIAUAPI void miau_pattern_clear(mi_Pattern*);
MIAUAPI void miau_pattern_set_event(mi_Pattern*, int index, mi_Event event);
MIAUAPI mi_Event miau_pattern_get_event(mi_Pattern*, int index);
#if defined(__cplusplus)
}
#endif
#endif /* _MIAU_H_ */
