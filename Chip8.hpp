#pragma once
#include <cstdint>
#include <array>
#include <string>

class Chip8 {
    private:
        std::array<std::uint8_t, 4096> memory = {}; //font + program
        std::array<std::uint8_t, 16> V = {}; //V0 to VF
        std::array<std::uint16_t, 16> stack = {}; //return address stack

        std::uint16_t i = 0; //holds memory address for operations
        std::uint16_t pc = 0x200; //program counter
        std::uint8_t sp = 0; //stack pointer
        std::uint16_t opcode = 0; //current opcode

        std::uint8_t delayTimer = 0;
        std::uint8_t soundTimer = 0;

        std::array<std::uint32_t, 64 * 32> video = {};
        std::array<bool, 16> keypad = {};

        bool loadFont();
        void runOpcode();

    public:

        Chip8();
        bool loadProgram(const std::string &romName);
        void cycle();

        const std::array<std::uint32_t, 64 * 32>& framebuffer();
        
        


};