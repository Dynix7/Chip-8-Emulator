#include <Chip8.hpp>
#include <cstdint>
#include <algorithm>
#include <array>
#include <vector>
#include <fstream>
#include <string>


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

//http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1
// opcode description from here
void Chip8::runOpcode() {
    opcode = static_cast<std::uint16_t>(memory[pc] << 8) | static_cast<std::uint16_t>(memory[pc+1]); // Reads opcode
    pc += 2; //increments the program counter by 2 since it reads 16 bits per opcodes

    std::uint16_t NNN = opcode & 0x0FFF;
    std::uint8_t NN = opcode & 0x00FF;
    std::uint8_t N = opcode & 0x000F;
    
    std::uint8_t x = opcode & 0x0F00;
    std::uint8_t y = opcode & 0x00F0;

    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) { // Clears Display
                video.fill(0);
            } else if (opcode == 0x00EE) {
                pc = stack[--sp]; //Return from a subroutine
            }
            break;

        case 0x1000:
            pc = NNN; //Jump to location nnn.
            break;

        case 0x2000:
            stack[++sp] = pc //Call subroutine at nnn.
            pc = NNN;
            break;

        case 0x3000:
            if (V[x] == NN) { //Skip next instruction if Vx = kk.
                pc += 2;
            }
            break;
        
        case 0x4000:
            if (V[x] != NN) { //Skip next instruction if Vx != kk.
                pc += 2;
            }
            break;
        
        case 0x5000:
            if (V[x] == V[y]) { //Skip next instruction if Vx = Vy.
                pc += 2;
            }
            break;
        
        case 0x6000:
            V[x] = NN; //Set Vx = kk.
            break;
        
        case 0x7000: 
            V[x] += NN; //Set Vx = Vx + kk.
            break;
        
        case 0x8000:
            switch (N) {
                case 0x0000:
                    V[x] = V[y]; //Set Vx = Vy.
                    break;
                
                case 0x0001:
                    V[x] = V[x] | V[y]; //Set Vx = Vx OR Vy.
                    break;
                
                case 0x0002:
                    V[x] = V[x] & V[y]; //Set Vx = Vx AND Vy.
                    break;
                
                case 0x0003:
                    V[x] = V[x] ^ V[y]; //Set Vx = Vx XOR Vy.
                    break;
                
                case 0x0004:
                    std::uint16_t sum = V[x] + V[y]; //Set Vx = Vx + Vy, set VF = carry.
                    V[0xF] = (sum > 0xFF);
                    V[x] = static_cast<std::uint8_t>(sum);
                    break;
                
                case 0x0005:
                    V[0xF] = (V[x] > V[y]); //Set Vx = Vx - Vy, set VF = NOT borrow.
                    V[x] -= V[y];
                    break;
                
                case 0x0006:
                    V[0xF] = (V[x] == 0x1); //Set Vx = Vx SHR 1.
                    V[x] >>= 1;
                    break;
                
                case 0x0007:
                    V[0xF] = (V[y] > V[x]); //Set Vx = Vy - Vx, set VF = NOT borrow.
                    V[x] = V[y] - V[x];
                    break;
                
                case 0x000E:
                    V[0xF] = (V[x] & 0x1); //Set Vx = Vx SHL 1.
                    V[x] <<= 1;
                    break;
            }
            break;
    }
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

