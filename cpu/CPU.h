#ifndef CHIP8EMU_CPU_H
#define CHIP8EMU_CPU_H
#include <cstdint>
#include "RAM.h"
#include "IFramebuffer.h"
#include "ISoundDevice.h"
#include "Keyboard.h"


namespace CHIP8 {
    class CPU {
        uint8_t V[16]{};
        uint16_t I{};
        uint16_t PC{0x200};
        uint16_t stack[16]{};
        uint16_t stack_pointer{};
        RAM& ram;
        IFramebuffer& framebuffer;
        ISoundDevice& sound_device;
        Keyboard& keyboard;
        uint8_t delay_timer{};
        uint8_t sound_timer{};
    public:
        explicit CPU(RAM& ram, IFramebuffer& framebuffer, Keyboard& keyboard, ISoundDevice& soundDevice);

        void print_state() const;
        bool cycle();

        void decrement();
    };
} // CHIP8

#endif //CHIP8EMU_CPU_H