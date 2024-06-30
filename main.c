#include "miau.h"

#include <stdio.h>

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

/*
mi_Sequencer* seq;

seq = miau_create_sequencer(4);
miau_sequencer_setup_channel(seq, 0, MIAU_SQUARE);

miau_create_pattern(32);
miau_pattern_add_note(pat, G4, effect)
miau_pattern_add_note(pat, A4, effect)
miau_pattern_add_note(pat, B4, effect)
miau_pattern_add_note(pat, BREAK, effect)
miau_pattern_add_note(pat, D5, effect)
miau_pattern_add_note(pat, BREAK, effect)
miau_pattern_add_note(pat, D5, effect)
miau_sequencer_add_pattern(seq, 0, pat);
> ch0 ch1 ch2 ch3
  pat1 pat0
  pat2 pat0

miau_sequencer_start(seq);
while (sequencer_is_playing(seq));
miau_sequencer_stop(seq);
*/

mi_Event events[] = {
        MIAU_CREATE_NOTE(MIAU_G, 4), MIAU_CREATE_NOTE(MIAU_A, 4), MIAU_CREATE_NOTE(MIAU_B, 4), MIAU_CREATE_NOTE(MIAU_D, 5),
        MIAU_CREATE_NOTE(MIAU_D, 5), MIAU_CREATE_NOTE(MIAU_B, 4), MIAU_CREATE_NOTE(MIAU_C, 5), MIAU_CREATE_NOTE(MIAU_G, 4),
        MIAU_CREATE_NOTE(MIAU_A, 4), MIAU_CREATE_NOTE(MIAU_B, 4), MIAU_CREATE_NOTE(MIAU_D, 5), MIAU_CREATE_NOTE(MIAU_D, 5),
        MIAU_CREATE_NOTE(MIAU_C, 5), MIAU_CREATE_NOTE(MIAU_B,  4), 0x1 << 28, 0x1 << 28
};

void audio_callback(void* userdata, Uint8* stream, int len) {
    mi_System* s = (mi_System*)userdata;
    miau_generate_sample(s, stream, len);
};

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_AUDIO);
    mi_System* s = miau_init(NULL);

    SDL_AudioSpec spec;
    spec.freq = 44100;
    spec.channels = 1;
    spec.samples = 2048;
    spec.format = AUDIO_S16SYS;
    spec.callback = audio_callback;
    spec.userdata = s;

    SDL_AudioDeviceID dev;
    dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 1);
    SDL_PauseAudioDevice(dev, 0);

    mi_Sequencer* seq = miau_get_sequencer(s, 0);
    miau_sequencer_set_speed(seq, 8.f);
    mi_Channel* ch = miau_sequencer_get_channel(seq, 0);
    mi_Pattern* pt = miau_channel_get_pattern(ch, 0);
    miau_channel_set_waveform(ch, MIAU_TRIANGLE);
    for (int i = 0; i < 16; i++) {
        // miau_pattern_set_note(pt, i, notes[i], 0);
        miau_pattern_set_event(pt, i, events[i]);
    }
    pt = miau_channel_get_pattern(ch, 1);
    for (int i = 0; i < 16; i++) {
        miau_pattern_set_event(pt, i, events[16 - i - 1]);
    }

    miau_save_project(s, "music.miau");

    mi_Frame* frame = miau_sequencer_get_frame(seq, 1);
    miau_frame_set_pattern(frame, 0, 1);

    ch = miau_sequencer_get_channel(seq, 1);
    miau_channel_set_waveform(ch, MIAU_TRIANGLE);
    mi_Pattern* p2 = miau_channel_get_pattern(ch, 0);
    miau_pattern_set_event(p2, 0, MIAU_CREATE_NOTE(MIAU_B, 4));
    miau_pattern_set_event(p2, 1, MIAU_CREATE_NOTE(MIAU_CS, 5));
    miau_pattern_set_event(p2, 2, MIAU_CREATE_NOTE(MIAU_DS, 5));

    mi_Pattern* p3 = miau_channel_get_pattern(ch, )

    miau_sequencer_set_playing(seq, 1);
    printf("playing sound\n");

    SDL_Delay(4000);
    int a;
    // scanf("%d", &a);

    printf("stopping sound\n");
    // miau_destroy_sound(snd);
    miau_quit(s);

    SDL_CloseAudioDevice(dev);
    SDL_Quit();
    return 0;
}
