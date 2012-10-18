//
//  main.cpp
//  Emulator
//
//  Created by Tyler Kieft on 10/14/12.
//
//

#include <iostream>
#include <string>
#include <unistd.h>
#include "SDL/SDL.h"
#include "Emulator.h"

#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char *argv[]) {
    Emulator emulator;
    emulator.load_rom("/Users/tkieft/Downloads/Super Mario Bros. (JU) [!].nes");
    
    // initialize the engine
    const SDL_version* version = SDL_Linked_Version();
    std::cout << "Emulator - Tyler Kieft - SDL Version "
              << (int) version->major << "."
              << (int) version->minor << "."
              << (int) version->patch << std::endl;
    
    // main loop
    while (true)
    {
        int ticks = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) {
                return 0;
            } else if (event.type == SDL_KEYDOWN) {
                emulator.handle_key_down(event.key.keysym.sym);
            } else if (event.type == SDL_KEYUP) {
                emulator.handle_key_up(event.key.keysym.sym);
            }
        }

        
        emulator.emulate_frame();

        while( SDL_GetTicks() - ticks < 1000 / FPS ) {
        }
    }
    
    return 0;
}