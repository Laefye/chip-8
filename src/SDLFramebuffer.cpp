#include "SDLFramebuffer.h"

#include <cmath>
#include <iterator>
#include <vector>

SDLFramebuffer::SDLFramebuffer(CHIP8::Keyboard &keyboard) : keyboard(keyboard) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    window = SDL_CreateWindow("CHIP-8 Emulator", 640, 320, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderLogicalPresentation(renderer, 64, 32, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    SDL_AudioSpec audio_spec;
    audio_spec.freq = SAMPLE_RATE;
    audio_spec.format = SDL_AUDIO_S16;
    audio_spec.channels = 1;

    audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, &SDLFramebuffer::audio_callback, this);
    SDL_ResumeAudioStreamDevice(audio_stream);
}

SDLFramebuffer::~SDLFramebuffer() {
    SDL_DestroyAudioStream(audio_stream);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SDLFramebuffer::clear() {
    std::fill(std::begin(framebuffer), std::end(framebuffer), 0);
}

bool SDLFramebuffer::draw_sprite(int x, int y, const std::uint8_t *sprite_data, int num_rows) {
    bool collision = false;
    for (int row = 0; row < num_rows; ++row) {
        uint8_t sprite_row = sprite_data[row];
        for (int col = 0; col < 8; ++col) {
            if (sprite_row & (0x80 >> col)) {
                int fb_x = (x + col) % 64;
                int fb_y = (y + row) % 32;
                int index = fb_y * 64 + fb_x;
                if (framebuffer[index]) {
                    collision = true;
                }
                framebuffer[index] ^= 1;
            }
        }
    }
    return collision;
}

uint8_t from_sdk_key(const SDL_Keycode key) {
    switch (key) {
        case SDLK_1: return 0x1;
        case SDLK_2: return 0x2;
        case SDLK_3: return 0x3;
        case SDLK_4: return 0xC;
        case SDLK_Q: return 0x4;
        case SDLK_W: return 0x5;
        case SDLK_E: return 0x6;
        case SDLK_R: return 0xD;
        case SDLK_A: return 0x7;
        case SDLK_S: return 0x8;
        case SDLK_D: return 0x9;
        case SDLK_F: return 0xE;
        case SDLK_Z: return 0xA;
        case SDLK_X: return 0x0;
        case SDLK_C: return 0xB;
        case SDLK_V: return 0xF;
        default: return 0x00; // Invalid key
    }
}

bool SDLFramebuffer::render() const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (framebuffer[y * 64 + x]) {
                SDL_RenderPoint(renderer, static_cast<float>(x), static_cast<float>(y));
            }
        }
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }
        if (event.type == SDL_EVENT_KEY_DOWN) {
            keyboard.keys[from_sdk_key(event.key.key)] = true;
        } else if (event.type == SDL_EVENT_KEY_UP) {
            keyboard.keys[from_sdk_key(event.key.key)] = false;
        }
    }

    SDL_RenderPresent(renderer);
    return true;
}

void SDLFramebuffer::play_sound() {
    is_sound_playing = true;
}

void SDLFramebuffer::stop_sound() {
    is_sound_playing = false;
}

void SDLFramebuffer::audio_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
    auto* fb = static_cast<SDLFramebuffer*>(userdata);

    const int samples = additional_amount / static_cast<int>(sizeof(Sint16));
    std::vector<Sint16> buf(samples);

    constexpr float phase_step = 2.0f * M_PI * FREQUENCY / SAMPLE_RATE;

    for (int i = 0; i < samples; ++i) {
        if (fb->is_sound_playing) {
            buf[i] = (fb->phase < M_PI) ? 8000 : -8000;
        } else {
            buf[i] = 0;
        }
        fb->phase += phase_step;
        if (fb->phase >= 2.0f * M_PI)
            fb->phase -= 2.0f * M_PI;
    }

    SDL_PutAudioStreamData(stream, buf.data(), additional_amount);
}
