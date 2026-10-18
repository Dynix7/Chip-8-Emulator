#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "Chip8.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define SDL_FLAGS (SDL_INIT_VIDEO)

struct SDLInstance {
    const char* WINDOW_NAME = "Chip8 Emulator";
    const int VIDEO_WIDTH = 64;
    const int VIDEO_HEIGHT = 32;
    const int WINDOW_SCALE = 16;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;

    SDL_Event event;
    bool running = true;
};
    
Chip8 chip8;
struct SDLInstance ChipVideo;

void processInputs();
void cleanupSDL();

int main(int argc, char * argv[]) {
    if (!SDL_Init(SDL_FLAGS)) {
        std::cerr << "Error Starting SDL" << '\n';
    }
    ChipVideo.window = SDL_CreateWindow(ChipVideo.WINDOW_NAME, ChipVideo.VIDEO_WIDTH * ChipVideo.WINDOW_SCALE, 
    ChipVideo.VIDEO_HEIGHT * ChipVideo.WINDOW_SCALE, 0);
    if (!ChipVideo.window) {
        std::cerr <<"Error Creating Window" << '\n';
    }  

    ChipVideo.renderer = SDL_CreateRenderer(ChipVideo.window, nullptr);
    if (!ChipVideo.renderer) {
        std::cerr << "Error Creating Renderer" << '\n';
    }

    ChipVideo.texture = SDL_CreateTexture(ChipVideo.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, ChipVideo.VIDEO_WIDTH, ChipVideo.VIDEO_HEIGHT);
    if (!ChipVideo.texture) {
        std::cerr << "Error Creating Texture" << '\n';
    }

    if (chip8.loadProgram("IBMLogo.ch8")) {
        std::cout << "Program Loaded" << '\n';
    } else {
        std::cerr << "Error Loading Program" << '\n';
    }

    while (ChipVideo.running) {
        processInputs();

        SDL_RenderClear(ChipVideo.renderer);
        SDL_UpdateTexture(ChipVideo.texture, nullptr, chip8.framebuffer().data(), sizeof(chip8.framebuffer()[0]) * ChipVideo.VIDEO_WIDTH);
        SDL_RenderTexture(ChipVideo.renderer, ChipVideo.texture, nullptr, nullptr);
        SDL_RenderPresent(ChipVideo.renderer);

        chip8.cycle();
        SDL_Delay(100);
    }

    cleanupSDL();
    return 0;
}

void processInputs() {
    while (SDL_PollEvent(&(ChipVideo.event))) {
        switch (ChipVideo.event.type) {
            case SDL_EVENT_QUIT:
                ChipVideo.running = false;
                break;
            default:
                break;
        }
    }
}

void cleanupSDL() {
    SDL_DestroyWindow(ChipVideo.window);
    ChipVideo.window = nullptr;

    SDL_DestroyRenderer(ChipVideo.renderer);
    ChipVideo.renderer = nullptr;

    SDL_DestroyTexture(ChipVideo.texture);
    ChipVideo.texture = nullptr;

    SDL_Quit();

    std::cout << "Cleaned Up SDL" << '\n';
}