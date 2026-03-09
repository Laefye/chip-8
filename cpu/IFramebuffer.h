//
// Created by Laefye on 10.03.2026.
//

#ifndef CHIP8EMU_IFRAMEBUFFER_H
#define CHIP8EMU_IFRAMEBUFFER_H
#include <cstdint>

namespace CHIP8 {
    class IFramebuffer {
    public:
        virtual ~IFramebuffer() = default;
        virtual void clear() = 0;
        virtual bool draw_sprite(int x, int y, const std::uint8_t* sprite_data, int num_rows) = 0;
    };
}

#endif //CHIP8EMU_IFRAMEBUFFER_H