//
//  Renderer.h
//  Emulator
//
//  Created by Tyler Kieft on 10/6/12.
//
//

#ifndef __Emulator__Renderer__
#define __Emulator__Renderer__

#include "glUtil.h"

class Renderer {
public:
    virtual ~Renderer() {}
    virtual void render() = 0;
    virtual void resize(GLuint width, GLuint height) = 0;
};

#endif /* defined(__Emulator__Renderer__) */
