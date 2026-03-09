#include <iostream>
#include <fstream>
#include <CPU.h>
#include <iterator>
#include <thread>
#include <vector>
#include <chrono>
#include <thread>
#include "SDLFramebuffer.h"

std::vector<uint8_t> load_rom(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open ROM file: " + filename);
    }
    std::vector<uint8_t> rom;
    file.unsetf(std::ios::skipws);
    file.seekg(0, std::ios::end);
    const std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    rom.reserve(file_size);
    rom.insert(rom.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());
    return rom;
}

class DummyFramebuffer : public CHIP8::IFramebuffer {
public:
    void clear() override {
        std::cout << "Clear framebuffer" << std::endl;
    }
    bool draw_sprite(int x, int y, const std::uint8_t* sprite_data, int num_rows) override {
        std::cout << "Draw sprite at (" << x << ", " << y << ") with height " << num_rows << std::endl;
        return false;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <ROM file>" << std::endl;
        return 1;
    }

    CHIP8::Keyboard keyboard;
    SDLFramebuffer framebuffer{keyboard};

    CHIP8::RAM ram;
    ram.load_rom(load_rom(argv[1]));


    CHIP8::CPU cpu{ram, framebuffer, keyboard, framebuffer};

    using clock     = std::chrono::steady_clock;
    using duration  = clock::duration;  // ← вот исправление

    const duration cpu_tick   = std::chrono::duration_cast<duration>(std::chrono::duration<double>(1.0 / 700.0));
    const duration frame_tick = std::chrono::duration_cast<duration>(std::chrono::duration<double>(1.0 / 60.0));

    auto cpu_next   = clock::now();
    auto frame_next = clock::now();

    while (true) {
        auto now = clock::now();

        if (now >= cpu_next) {
            if (!cpu.cycle()) break;
            cpu_next += cpu_tick;
        }

        if (now >= frame_next) {
            if (!framebuffer.render()) break;
            cpu.decrement();
            frame_next += frame_tick;
        }

        auto next_wake = std::min(cpu_next, frame_next);
        std::this_thread::sleep_until(next_wake);
    }
}