#include <iostream>
#include "stdint.h"
#include <chrono>
#include <thread>
#include "SDL2/SDL.h"
#include "chip8.h"

using namespace std;

// Keypad map
uint8_t keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

int main(int argc, char **argv) {

    // Command usage
    if(argc != 2){
        cout << "Usage: Chip8 <ROM file>" << endl;
        return 1;
    }

    Chip8 chip8 = Chip8();          

    int w = 1024;                   // Window width
    int h = 512;                    // Window height

    // The window we'll be rendering to
    SDL_Window* window = NULL;

    // Initialize SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        printf( "SDL init. failed! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    // Create window
    window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    if(window == NULL){
        printf( "Window creation err! SDL_Error: %s\n", SDL_GetError());
        exit(2);
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, w, h);

    // Create texture that stores frame buffer
    SDL_Texture* sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    // Temporary pixel buffer
    uint32_t pixels[2048];

    load:
    // Attempt to load ROM
    if(!chip8.loadApp(argv[1]))
        return 2;

    // Emulation loop
    while(true){
        chip8.emulateCycle();
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT) exit(0);
            // Process keydown events
            if (e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == SDLK_ESCAPE)
                    exit(0);
                if(e.key.keysym.sym == SDLK_F1)
                    goto load;      
                for(int i = 0; i < 16; ++i){
                    if(e.key.keysym.sym == keymap[i]){
                        chip8.keypad[i] = 1;
                    }
                }
            }
            // Process keyup events
            if(e.type == SDL_KEYUP){
                for(int i = 0; i < 16; ++i){
                    if(e.key.keysym.sym == keymap[i]){
                        chip8.keypad[i] = 0;
                    }
                }
            }
        }

        // If draw occurred, redraw SDL screen
        if(chip8.drawingFlag){

            chip8.drawingFlag = false;

            // Store pixels in temporary buffer
            for(int i = 0; i < 2048; ++i){
                uint8_t pixel = chip8.gfxBuffer[i];
                pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }

            SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(Uint32));               // Update SDL texture
            SDL_RenderClear(renderer);                                                      // Clear screen and render
            SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
        this_thread::sleep_for(chrono::microseconds(1200));
    }
}