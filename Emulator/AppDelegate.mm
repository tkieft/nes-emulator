//
//  AppDelegate.h
//  helloworld
//
//  Created by Tyler Kieft on 5/17/20.
//  Copyright © 2020 Tyler Kieft. All rights reserved.
//

#import "AppDelegate.h"

#include <iostream>

#include "SDL.h"

#include "Emulator.h"

@implementation AppDelegate

- (void)applicationWillTerminate:(NSNotification *)notification {
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    // initialize SDL
    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO ) < 0) {
        std::cout << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
        ::exit(1);
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
    while (true) {
        Uint32 ticks = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                [NSApp terminate:self];
            } else if (event.type == SDL_KEYDOWN) {
                emulator.handle_key_down(event.key.keysym);
            } else if (event.type == SDL_KEYUP) {
                emulator.handle_key_up(event.key.keysym);
            }
        }

        emulator.emulate_frame();

        // subtract the # of ms it took us to render this frame from
        // the # of ms we have to render each frame
        int ms_to_sleep = 1000 / kFPS - static_cast<int>(SDL_GetTicks() - ticks);
        if (ms_to_sleep > 0) {
            SDL_Delay(static_cast<Uint32>(ms_to_sleep));
        }
    }
}
@end
