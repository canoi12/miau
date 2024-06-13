#include "miau.h"
#include <SDL2/SDL.h>

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
    mi_Channel* ch = miau_sequencer_get_channel(seq, 0);
    mi_Pattern* pt = miau_channel_get_pattern(ch, 0);
    miau_channel_set_waveform(ch, MIAU_TRIANGLE);

    ch = miau_sequencer_get_channel(seq, 1);
    miau_channel_set_waveform(ch, MIAU_SQUARE);
    mi_Pattern* p2 = miau_channel_get_pattern(ch, 0);
    miau_pattern_set_note(p2, 0, MIAU_C5, 0);
    miau_pattern_set_note(p2, 1, MIAU_A4, 0);

    int base_note = MIAU_C4;
    #if 0
    miau_pattern_set_note(pt, 0, MIAU_C4, 0);
    miau_pattern_set_note(pt, 1, MIAU_D4, 0);
    miau_pattern_set_note(pt, 2, MIAU_E4, 0);
    miau_pattern_set_note(pt, 3, MIAU_F4, 0);
    miau_pattern_set_note(pt, 4, MIAU_G4, 0);
    miau_pattern_set_note(pt, 5, MIAU_A4, 0);
    miau_pattern_set_note(pt, 6, MIAU_B4, 0);
    miau_pattern_set_note(pt, 7, MIAU_C5, 0);
    miau_pattern_set_note(pt, 8, MIAU_D5, 0);
    miau_pattern_set_note(pt, 9, MIAU_E5, 0);
    miau_pattern_set_note(pt, 10, MIAU_F5, 0);
    miau_pattern_set_note(pt, 12, MIAU_A4, 0);
    #endif
    for (int i = 0; i < 16; i++) {
      miau_pattern_set_note(pt, i, base_note+i, 0);
    }
#if 0
    mi_Sound* snd = miau_create_sound(1, 32);
    mi_Channel* c = miau_sound_get_channel(snd, 0);
    miau_set_channel_waveform(c, MIAU_SINE);
    // miau_channel_push(c, MIAU_G4, 0);
    // miau_channel_push(c, MIAU_A4, 0);
    miau_channel_push(c, MIAU_B4, 0);

    miau_play_sound(snd);
  #endif
    // miau_sequencer_set_playing(seq, 1);
    printf("playing sound\n");

    SDL_Delay(3000);
    int a;
    // scanf("%d", &a);

    printf("stopping sound\n");
    // miau_destroy_sound(snd);
    miau_quit(s);

    SDL_CloseAudioDevice(dev);
    SDL_Quit();
    return 0;
}
