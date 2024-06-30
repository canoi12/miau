#include "miau.h"

#include <stdio.h>

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

mi_Event events[] = {
        MIAU_CREATE_NOTE(MIAU_G, 4), MIAU_CREATE_NOTE(MIAU_A, 4), MIAU_CREATE_NOTE(MIAU_B, 4), MIAU_CREATE_NOTE(MIAU_D, 5),
        MIAU_CREATE_NOTE(MIAU_D, 5), MIAU_CREATE_NOTE(MIAU_B, 4), MIAU_CREATE_NOTE(MIAU_C, 5), MIAU_CREATE_NOTE(MIAU_G, 4),
        MIAU_CREATE_NOTE(MIAU_A, 4), MIAU_CREATE_NOTE(MIAU_B, 4), MIAU_CREATE_NOTE(MIAU_D, 5), MIAU_CREATE_NOTE(MIAU_D, 5),
        MIAU_CREATE_NOTE(MIAU_C, 5), MIAU_CREATE_NOTE(MIAU_B,  4), MIAU_CREATE_BREAK(), MIAU_CREATE_BREAK()
};

mi_Event events1[] = {
        MIAU_CREATE_NOTE(MIAU_G, 4), MIAU_CREATE_BREAK(), MIAU_CREATE_NOTE(MIAU_A, 4), MIAU_CREATE_BREAK(), MIAU_CREATE_NOTE(MIAU_B, 4),
        MIAU_CREATE_BREAK(), MIAU_CREATE_NOTE(MIAU_D, 5), MIAU_CREATE_BREAK(), MIAU_CREATE_NOTE(MIAU_D, 5), MIAU_CREATE_BREAK(),
        MIAU_CREATE_NOTE(MIAU_B, 4), MIAU_CREATE_BREAK(), MIAU_CREATE_NOTE(MIAU_C, 5)
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
    miau_channel_set_waveform(ch, MIAU_SQUARE);
    int size = sizeof(events1) / sizeof(mi_Event);
    for (int i = 0; i < size; i++) {
        // miau_pattern_set_note(pt, i, notes[i], 0);
        miau_pattern_set_event(pt, i, events1[i]);
    }
    pt = miau_channel_get_pattern(ch, 1);
    for (int i = 0; i < size; i++) {
        miau_pattern_set_event(pt, i, events1[size - i - 1]);
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
    miau_pattern_set_event(p2, 6, MIAU_CREATE_NOTE(MIAU_D, 3));

    miau_sequencer_set_playing(seq, 1);
    printf("playing sound\n");

    SDL_Delay(4000);

    printf("stopping sound\n");
    miau_quit(s);

    SDL_CloseAudioDevice(dev);
    SDL_Quit();
    return 0;
}
