#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "Chip8.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define SDL_FLAGS (SDL_INIT_VIDEO)

struct SDLInstance {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
};
    


int main(int argc, char * argv[]) {
    if (!SDL_Init(SDL_FLAGS)) {
        std::cerr << "Error Starting SDL" << '\n';
    } else {
        std::cout << "Started SDL" << '\n';
    }


    SDL_Quit();
    return 0;
}