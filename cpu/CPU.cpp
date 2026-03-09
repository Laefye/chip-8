#include "CPU.h"
#include <iostream>
#include <cstdlib>

namespace CHIP8 {
    CPU::CPU(RAM& ram, IFramebuffer& framebuffer, Keyboard& keyboard, ISoundDevice& soundDevice)
        : ram(ram), framebuffer(framebuffer), sound_device(soundDevice), keyboard(keyboard) {

    }

    struct Instruction {
        uint16_t opcode;
        uint8_t x;
        uint8_t y;
        uint8_t n;
        uint16_t nn;
        uint16_t nnn;
    };

    void decode_instruction(const uint16_t opcode, Instruction& instr) {
        instr.opcode = opcode;
        instr.x = (opcode & 0x0F00) >> 8;
        instr.y = (opcode & 0x00F0) >> 4;
        instr.n = opcode & 0x000F;
        instr.nn = opcode & 0x00FF;
        instr.nnn = opcode & 0x0FFF;
    }

    void CPU::print_state() const {
        for (int i = 0; i < 16; ++i) {
            std::cout << "V[" << std::hex << i << "] = " << std::hex << static_cast<int>(V[i]) << std::endl;
        }
        std::cout << "I = " << std::hex << I << std::endl;
        std::cout << "PC = " << std::hex << PC << std::endl;
    }

    bool CPU::cycle() {
        Instruction instruction{};
        decode_instruction(ram.get_word(PC), instruction);
        if (instruction.opcode == 0x00E0) {
            framebuffer.clear();
            PC += 2;
        } else if (instruction.opcode == 0x00EE) {
            PC = stack[--stack_pointer];
        } else if ((instruction.opcode & 0xF000) == 0x1000) {
            PC = instruction.nnn;
        } else if ((instruction.opcode & 0xF000) == 0x2000) {
            stack[stack_pointer++] = PC + 2;
            PC = instruction.nnn;
        } else if ((instruction.opcode & 0xF000) == 0x3000) {
            if (V[instruction.x] == instruction.nn) {
                PC += 4;
            } else {
                PC += 2;
            }
        } else if ((instruction.opcode & 0xF000) == 0x4000) {
            if (V[instruction.x] != instruction.nn) {
                PC += 4;
            } else {
                PC += 2;
            }
        } else if ((instruction.opcode & 0xF000) == 0x5000) {
            if (V[instruction.x] == V[instruction.y]) {
                PC += 4;
            } else {
                PC += 2;
            }
        } else if ((instruction.opcode & 0xF000) == 0x6000) {
            V[instruction.x] = instruction.nn;
            PC += 2;
        } else if ((instruction.opcode & 0xF000) == 0x7000) {
            V[instruction.x] += instruction.nn;
            PC += 2;
        } else if ((instruction.opcode & 0xF000) == 0x8000) {
            if ((instruction.opcode & 0x000F) == 0x0000) {
                V[instruction.x] = V[instruction.y];
                PC += 2;
            } else if ((instruction.opcode & 0x000F) == 0x0001) {
                V[instruction.x] |= V[instruction.y];
                V[0xF] = 0;
                PC += 2;
            } else if ((instruction.opcode & 0x000F) == 0x0002) {
                V[instruction.x] &= V[instruction.y];
                V[0xF] = 0;
                PC += 2;
            } else if ((instruction.opcode & 0x000F) == 0x0003) {
                V[instruction.x] ^= V[instruction.y];
                V[0xF] = 0;
                PC += 2;
            } else if ((instruction.opcode & 0x000F) == 0x0004) {
                const uint16_t sum = V[instruction.x] + V[instruction.y];
                V[0xF] = (sum > 0xFF) ? 1 : 0;
                V[instruction.x] = sum & 0xFF;
                PC += 2;
            } else if ((instruction.opcode & 0x000F) == 0x0005) {
                V[0xF] = (V[instruction.x] > V[instruction.y]) ? 1 : 0;
                V[instruction.x] -= V[instruction.y];
                PC += 2;
            } else if ((instruction.opcode & 0x000F) == 0x0006) {
                V[0xF] = V[instruction.x] & 0x1;
                V[instruction.x] >>= 1;
                PC += 2;
            } else if ((instruction.opcode & 0x000F) == 0x0007) {
                V[0xF] = (V[instruction.y] > V[instruction.x]) ? 1 : 0;
                V[instruction.x] = V[instruction.y] - V[instruction.x];
                PC += 2;
            } else if ((instruction.opcode & 0x000F) == 0x000E) {
                V[0xF] = (V[instruction.x] & 0x80) >> 7;
                V[instruction.x] <<= 1;
                PC += 2;
            } else {
                std::cout << "Unknown opcode: " << std::hex << instruction.opcode << std::endl;
                return false;
            }
        } else if ((instruction.opcode & 0xF000) == 0x9000) {
            if (V[instruction.x] != V[instruction.y]) {
                PC += 4;
            } else {
                PC += 2;
            }
        } else if ((instruction.opcode & 0xF000) == 0xA000) {
            I = instruction.nnn;
            PC += 2;
        } else if ((instruction.opcode & 0xF000) == 0xB000) {
            PC = instruction.nnn + V[0];
        } else if ((instruction.opcode & 0xF000) == 0xC000) {
            V[instruction.x] = (std::rand() % 256) & instruction.nn;
            PC += 2;
        } else if ((instruction.opcode & 0xF000) == 0xD000) {
            V[0xF] = framebuffer.draw_sprite(V[instruction.x], V[instruction.y], ram.get_memory() + I, instruction.n);
            PC += 2;
        } else if ((instruction.opcode & 0xF000) == 0xE000) {
            if (instruction.nn == 0x9E) {
                if (keyboard.keys[V[instruction.x]]) {
                    PC += 4;
                } else {
                    PC += 2;
                }
            } else if (instruction.nn == 0xA1) {
                if (!keyboard.keys[V[instruction.x]]) {
                    PC += 4;
                } else {
                    PC += 2;
                }
            }
        } else if ((instruction.opcode & 0xF000) == 0xF000) {
            if (instruction.nn == 0x07) {
                V[instruction.x] = delay_timer;
                PC += 2;
            } else if (instruction.nn == 0x0A) {
                bool key_pressed = false;
                for (int i = 0; i < 16; ++i) {
                    if (keyboard.keys[i]) {
                        V[instruction.x] = i;
                        key_pressed = true;
                        break;
                    }
                }
                if (!key_pressed) {
                    // скипаем инструкцию, если клавиша не нажата, и ждем следующего цикла
                    return true;
                }
                PC += 2;
            } else if (instruction.nn == 0x15) {
                delay_timer = V[instruction.x];
                PC += 2;
            } else if (instruction.nn == 0x18) {
                sound_timer = V[instruction.x];
                sound_device.play_sound();
                PC += 2;
            } else if (instruction.nn == 0x1E) {
                I += V[instruction.x];
                PC += 2;
            } else if (instruction.nn == 0x29) {
                I = V[instruction.x] * 5;
                PC += 2;
            } else if (instruction.nn == 0x33) {
                ram.set_bcd(V[instruction.x], I);
                PC += 2;
            } else if (instruction.nn == 0x55) {
                for (int i = 0; i <= instruction.x; ++i) {
                    ram.get_memory()[I + i] = V[i];
                }
                PC += 2;
            } else if (instruction.nn == 0x65) {
                for (int i = 0; i <= instruction.x; ++i) {
                    V[i] = ram.get_memory()[I + i];
                }
                PC += 2;
            } else {
                std::cout << "Unknown opcode: " << std::hex << instruction.opcode << std::endl;
                return false;
            }
        } else {
            std::cout << "Unknown opcode: " << std::hex << instruction.opcode << std::endl;
            return false;
        }
        return true;
    }

    void CPU::decrement() {
        if (delay_timer > 0) {
            --delay_timer;
        }
        if (sound_timer > 0) {
            if (--sound_timer == 0) {
                sound_device.stop_sound();
            }
        }
    }
} // CHIP8