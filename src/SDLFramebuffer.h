//
// Created by Laefye on 10.03.2026.
//

#ifndef CHIP8EMU_SDLFRAMEBUFFER_H
#define CHIP8EMU_SDLFRAMEBUFFER_H
#include <SDL3/SDL.h>
#include <IFramebuffer.h>
#include <ISoundDevice.h>
#include <atomic>

#include "Keyboard.h"

class SDLFramebuffer : public CHIP8::IFramebuffer, public CHIP8::ISoundDevice  {
    SDL_Window* window;
    SDL_Renderer* renderer;

    uint8_t framebuffer[64 * 32]{};
    CHIP8::Keyboard &keyboard;

    std::atomic_bool is_sound_playing = false;

    static constexpr int SAMPLE_RATE = 44100;
    static constexpr float FREQUENCY = 440.0f;
    SDL_AudioStream* audio_stream;
    float phase = 0.0f;

    static void audio_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount);
public:
    explicit SDLFramebuffer(CHIP8::Keyboard &keyboard);
    ~SDLFramebuffer() override;

    void clear() override;
    bool draw_sprite(int x, int y, const std::uint8_t *sprite_data, int num_rows) override;

    bool render() const;

    void play_sound() override;
    void stop_sound() override;
};


#endif //CHIP8EMU_SDLFRAMEBUFFER_H