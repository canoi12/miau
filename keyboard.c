//
// Created by Canoi Gomes on 30/06/2024.
//
#include "miau.h"

#include <stdio.h>

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

void audio_callback(void* userdata, Uint8* stream, int len) {
    mi_System* s = (mi_System*)userdata;
    miau_generate_sample(s, stream, len);
};

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
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
    miau_sequencer_set_speed(seq, 32.f);

    mi_Channel* ch = miau_sequencer_get_channel(seq, 0);
    miau_channel_set_waveform(ch, MIAU_TRIANGLE);
    mi_Pattern* pat = miau_channel_get_pattern(ch, 0);

    SDL_Window* window = SDL_CreateWindow(
            "keyboard",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            460, 192,
            SDL_WINDOW_SHOWN
            );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: running = 0; break;
                case SDL_WINDOWEVENT: {
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_CLOSE: running = 0; break;
                    }
                }
                break;
                case SDL_KEYDOWN: {
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_1:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_BREAK());
                            break;
                        case SDL_SCANCODE_Z:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_C, 4));
                            break;
                        case SDL_SCANCODE_S:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_CS, 4));
                            break;
                        case SDL_SCANCODE_X:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_D, 4));
                            break;
                        case SDL_SCANCODE_D:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_DS, 4));
                            break;
                        case SDL_SCANCODE_C:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_E, 4));
                            break;
                        case SDL_SCANCODE_V:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_F, 4));
                            break;
                        case SDL_SCANCODE_G:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_FS, 4));
                            break;
                        case SDL_SCANCODE_B:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_G, 4));
                            break;
                        case SDL_SCANCODE_H:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_GS, 4));
                            break;
                        case SDL_SCANCODE_N:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_A, 4));
                            break;
                        case SDL_SCANCODE_J:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_AS, 4));
                            break;
                        case SDL_SCANCODE_M:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_B, 4));
                            break;
                        case SDL_SCANCODE_Q:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_C, 5));
                            break;
                        case SDL_SCANCODE_2:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_CS, 5));
                            break;
                        case SDL_SCANCODE_W:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_D, 5));
                            break;
                        case SDL_SCANCODE_3:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_DS, 5));
                            break;
                        case SDL_SCANCODE_E:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_E, 5));
                            break;
                        case SDL_SCANCODE_R:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_F, 5));
                            break;
                        case SDL_SCANCODE_5:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_FS, 5));
                            break;
                        case SDL_SCANCODE_T:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_G, 5));
                            break;
                        case SDL_SCANCODE_6:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_GS, 5));
                            break;
                        case SDL_SCANCODE_Y:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_A, 5));
                            break;
                        case SDL_SCANCODE_7:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_AS, 5));
                            break;
                        case SDL_SCANCODE_U:
                            miau_pattern_set_event(pat, 0, MIAU_CREATE_NOTE(MIAU_B, 5));
                            break;
                    }
                    miau_sequencer_set_playing(seq, 1);
                }
                break;
            }
        }
        SDL_RenderClear(renderer);

        SDL_Rect rect = {0, 48, 32, 128};
        int ev = miau_pattern_get_event(pat, 0);
        int notes[] = {MIAU_C, MIAU_D, MIAU_E, MIAU_F, MIAU_G, MIAU_A, MIAU_B};
        int octave = 4;
        for (int i = 0; i < 14; i++) {
            int ii = i % 7;
            if (i == 7) octave += 1;
            if (MIAU_GET_NOTE(ev) == notes[ii] && MIAU_GET_OCTAVE(ev) == octave) {
                SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }

            SDL_RenderFillRect(renderer, &rect);
            rect.x += 33;
        }
        SDL_Rect spos = {18, 48, 28, 96};
        int snotes[] = {MIAU_CS, MIAU_DS, MIAU_FS, MIAU_GS, MIAU_AS};
        octave = 4;
        for (int i = 0; i < 10; i++) {
            int ii = i % 5;
            if (i == 5) {
                spos.x += 33;
                octave += 1;
            }
            if (MIAU_GET_NOTE(ev) == snotes[ii] && MIAU_GET_OCTAVE(ev) == octave) {
                SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }

            SDL_RenderFillRect(renderer, &spos);
            spos.x += 33;
            if (snotes[ii] == MIAU_DS) spos.x += 33;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    miau_quit(s);
    SDL_CloseAudioDevice(dev);
    SDL_Quit();
    return 0;
}