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
#include "SDL.h"
#include "Emulator.h"

#ifdef __cplusplus
extern "C"
#endif

int my_main(int argc, char *argv[]) {
    // initialize SDL
    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO ) < 0)
    {
        std::cout << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_version version;
    SDL_GetVersion(&version);
    std::cout
        << "Emulator - Tyler Kieft - SDL Version "
        << (int)version.major << "."
        << (int)version.minor << "."
        << (int)version.patch << std::endl;

    // initialize the engine
    Emulator emulator;
    emulator.load_rom("/Users/tkieft/code/NES Emulator Dev Resources/Super Mario Bros. (JU) [!].nes");
    //emulator.load_rom("/Users/tkieft/code/NES Emulator Dev Resources/NEStress/NEStress.nes");

    // main loop
    while (true)
    {
        int ticks = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            } else if (event.type == SDL_KEYDOWN) {
                emulator.handle_key_down(event.key.keysym);
            } else if (event.type == SDL_KEYUP) {
                emulator.handle_key_up(event.key.keysym);
            }
        }

        emulator.emulate_frame();

        // subtract the # of ms it took us to render this frame from
        // the # of ms we have to render each frame
        int ms_to_sleep = 1000 / FPS - (SDL_GetTicks() - ticks);
        if (ms_to_sleep > 0) {
            SDL_Delay(ms_to_sleep);
        }
    }
}
