//
//  Renderer.h
//  Emulator
//
//  Created by Tyler Kieft on 10/6/12.
//
//

#ifndef __Emulator__Renderer__
#define __Emulator__Renderer__

class Renderer {
public:
    virtual ~Renderer() {}
    virtual void render() = 0;
    virtual void resize(int width, int height) = 0;
    virtual void update_patterns() = 0;
};

#endif /* defined(__Emulator__Renderer__) */
