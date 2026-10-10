#include <Chip8.hpp>
#include <cstdint>
#include <algorithm>
#include <array>
#include <vector>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdio>


bool Chip8::loadFont() {
    constexpr std::size_t fontStart = 0x50; // cuz standard is to start font from 0x50
    constexpr std::array<std::uint8_t, 80> fontData = {	
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    std::copy(fontData.begin(), fontData.end(), memory.begin() + fontStart);
    return true;
}

bool Chip8::loadProgram(const std::string &romName) {
    constexpr std::size_t romStart = 0x200;
    constexpr std::streamoff maxRomSize = 4096 - 0x200;
    std::ifstream file(romName, std::ios::binary);

    if (!file) {
        std::cerr << "Error: Unable To Open File" << '\n';
        return false;
    }

    // Goes to end of file, checks size, and returns to beginning
    file.seekg(0, std::ios::end);
    std::streampos romEndPos = file.tellg();  //returns position
    if (romEndPos == std::streampos(-1)) {
        std::cerr << "Error Getting File Size" << '\n';
        return false;
    }
    std::streamoff romSize = static_cast<std::streamoff>(romEndPos); // casts into a streamoffset type
    file.seekg(0, std::ios::beg);

    if (romSize > maxRomSize || romSize <= 0) {
        std::cerr << "Error with ROM Size" << '\n';
        return false;
    }

    // Creates vector with the size of the the rom
    std::vector<std::uint8_t> rom(static_cast<std::size_t>(romSize));

    //.read() wants a char * so its cast into that, then another cast into streamsize
    if (!file.read(reinterpret_cast<char *>(rom.data()), static_cast<std::streamsize>(romSize))) {
        std::cerr << "Read File Failed" << '\n';
        return false;
    }
    
    // Copies data from the rom into the memory starting at 0x200
    std::copy(rom.begin(), rom.end(), memory.begin() + romStart);
    return true;
}

