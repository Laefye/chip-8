//
// Created by Laefye on 10.03.2026.
//

#ifndef CHIP8EMU_SOUNDDEVICE_H
#define CHIP8EMU_SOUNDDEVICE_H

namespace CHIP8 {
    class ISoundDevice {
    public:
        virtual ~ISoundDevice() = default;
        virtual void play_sound() = 0;
        virtual void stop_sound() = 0;
    };
}

#endif //CHIP8EMU_SOUNDDEVICE_H