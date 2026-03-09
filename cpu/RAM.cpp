//
// Created by Laefye on 09.03.2026.
//

#include "RAM.h"

namespace CHIP8 {
    uint8_t* RAM::get_memory() {
        return memory;
    }

    uint16_t RAM::get_word(const uint16_t address) const {
        return (memory[address] << 8) | memory[address + 1];
    }

    void RAM::load_rom(const std::vector<uint8_t> &rom) {
        std::copy(rom.begin(), rom.end(), memory + 0x200);
    }

    void RAM::set_bcd(uint8_t value, uint16_t address) {
        memory[address] = value / 100;
        memory[address + 1] = (value / 10) % 10;
        memory[address + 2] = value % 10;
    }
}
