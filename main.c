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
    short* buffer = (short*)stream;
    int length = len / sizeof(short);
    miau_generate_sample(buffer, length);
};

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec spec;
    spec.freq = 44100;
    spec.channels = 1;
    spec.samples = 2048;
    spec.format = AUDIO_S16SYS;
    spec.callback = audio_callback;

    SDL_AudioDeviceID dev;
    dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 1);
    SDL_PauseAudioDevice(dev, 0);

    miau_init(NULL);

    mi_Sound* snd = miau_create_sound(1, 32);
    mi_Channel* c = miau_sound_get_channel(snd, 0);
    miau_set_channel_waveform(c, MIAU_SINE);
    miau_channel_push(c, MIAU_G4, 0);
    miau_channel_push(c, MIAU_A4, 0);
    miau_channel_push(c, MIAU_B4, 0);

    miau_play_sound(snd);
    printf("playing sound\n");

    SDL_Delay(3000);

    printf("stoping sound\n");
    miau_destroy_sound(snd);

    SDL_CloseAudioDevice(dev);
    SDL_Quit();
    return 0;
}
