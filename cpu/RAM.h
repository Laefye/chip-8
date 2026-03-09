#ifndef CHIP8EMU_RAM_H
#define CHIP8EMU_RAM_H
#include <cstdint>
#include <vector>

namespace CHIP8 {
    class RAM {
        uint8_t memory[0x1000]{};
    public:
        uint8_t* get_memory();

        uint16_t get_word(uint16_t address) const;

        void load_rom(const std::vector<uint8_t>& rom);

        void set_bcd(uint8_t value, uint16_t address);
    };
} // CHIP8


#endif //CHIP8EMU_RAM_H