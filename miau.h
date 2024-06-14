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
#define MIAU_MAX_NOTES 16

enum {
    MIAU_SINE = 1,
    MIAU_SQUARE,
    MIAU_SAWTOOTH,
    MIAU_TRIANGLE,
    MIAU_NOISE
};

enum {
    MIAU_BREAK = 0,
    MIAU_C0, MIAU_CS0, MIAU_D0, MIAU_DS0, MIAU_E0, MIAU_F0, MIAU_FS0, MIAU_G0, MIAU_GS0, MIAU_A0, MIAU_AS0, MIAU_B0,
    MIAU_C1, MIAU_CS1, MIAU_D1, MIAU_DS1, MIAU_E1, MIAU_F1, MIAU_FS1, MIAU_G1, MIAU_GS1, MIAU_A1, MIAU_AS1, MIAU_B1,
    MIAU_C2, MIAU_CS2, MIAU_D2, MIAU_DS2, MIAU_E2, MIAU_F2, MIAU_FS2, MIAU_G2, MIAU_GS2, MIAU_A2, MIAU_AS2, MIAU_B2,
    MIAU_C3, MIAU_CS3, MIAU_D3, MIAU_DS3, MIAU_E3, MIAU_F3, MIAU_FS3, MIAU_G3, MIAU_GS3, MIAU_A3, MIAU_AS3, MIAU_B3,
    MIAU_C4, MIAU_CS4, MIAU_D4, MIAU_DS4, MIAU_E4, MIAU_F4, MIAU_FS4, MIAU_G4, MIAU_GS4, MIAU_A4, MIAU_AS4, MIAU_B4,
    MIAU_C5, MIAU_CS5, MIAU_D5, MIAU_DS5, MIAU_E5, MIAU_F5, MIAU_FS5, MIAU_G5, MIAU_GS5, MIAU_A5, MIAU_AS5, MIAU_B5,
    MIAU_C6, MIAU_CS6, MIAU_D6, MIAU_DS6, MIAU_E6, MIAU_F6, MIAU_FS6, MIAU_G6, MIAU_GS6, MIAU_A6, MIAU_AS6, MIAU_B6,
    MIAU_C7, MIAU_CS7, MIAU_D7, MIAU_DS7, MIAU_E7, MIAU_F7, MIAU_FS7, MIAU_G7, MIAU_GS7, MIAU_A7, MIAU_AS7, MIAU_B7
};


typedef struct mi_Note mi_Note;
typedef struct mi_Pattern mi_Pattern;
typedef struct mi_Channel mi_Channel;
typedef struct mi_Sequencer mi_Sequencer;

typedef struct mi_System mi_System;

typedef struct {
    int freq;
    int channels;
    int samples;
} mi_Config;

MIAUAPI mi_System* miau_init(const mi_Config* config);
MIAUAPI void miau_quit(mi_System*);

MIAUAPI void miau_generate_sample(mi_System*, unsigned char* stream, int len);

// Sequencer
MIAUAPI mi_Sequencer* miau_get_sequencer(mi_System*, int index);
MIAUAPI mi_Channel* miau_sequencer_get_channel(mi_Sequencer* seq, int index);

MIAUAPI void miau_sequencer_set_speed(mi_Sequencer* seq, float speed);
MIAUAPI void miau_sequencer_set_playing(mi_Sequencer* seq, int playing);
MIAUAPI void miau_sequencer_restart(mi_Sequencer* seq);

// Channel
MIAUAPI void miau_channel_set_waveform(mi_Channel*, int waveform);
MIAUAPI mi_Pattern* miau_channel_get_pattern(mi_Channel*, int index);

// Pattern
MIAUAPI void miau_pattern_set_note(mi_Pattern*, int index, int note, int effect);
#if 0
MIAUAPI mi_Channel* miau_create_channel(int waveform, int size);
MIAUAPI void miau_destroy_channel(mi_Channel* c);

MIAUAPI void miau_set_channel_waveform(mi_Channel* c, int waveform);
MIAUAPI void miau_resize_channel(mi_Channel* c, int size);

MIAUAPI void miau_channel_push(mi_Channel* c, int note, int effect);
MIAUAPI void miau_channel_seek(mi_Channel* c, int index);
MIAUAPI int miau_channel_tell(mi_Channel* c);

MIAUAPI mi_Sound* miau_create_sound(int channels, int size);
MIAUAPI void miau_destroy_sound(mi_Sound*  sound);
MIAUAPI void miau_sound_setup_channel(mi_Sound* sound, int index, int waveform, int size);
MIAUAPI mi_Channel* miau_sound_get_channel(mi_Sound* sound, int index);

MIAUAPI void miau_play_sound(mi_Sound* sound);
MIAUAPI void miau_pause_sound(mi_Sound* sound);
MIAUAPI void miau_stop_sound(mi_Sound* sound);
#endif
#endif /* _MIAU_H_ */
