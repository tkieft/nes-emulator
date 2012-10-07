//
//  OpenGLRenderer.h
//  Emulator
//
//  Created by Tyler Kieft on 9/30/12.
//
//

#ifndef Open_GL_Renderer_h
#define Open_GL_Renderer_h

#include "glUtil.h"
#include "Renderer.h"
#include "PPU.h"

class OpenGLRenderer : public Renderer {
private:
    PPU *ppu;
    
    GLuint m_defaultFBOName;
    GLuint m_viewWidth;
    GLuint m_viewHeight;
    
    GLuint texture;
    GLuint textureFBO;

public:
    OpenGLRenderer(PPU *ppu);
    virtual ~OpenGLRenderer();
    virtual void resize(GLuint width, GLuint height);
    virtual void render();
};

#endif