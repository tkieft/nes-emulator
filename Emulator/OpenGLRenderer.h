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
#include "nes_palette.h"

// The pattern tables from 0x0000 - 0x2000 hold 512 8x8 patterns
static const int cPATTERNS = 512;
// 16 bytes per pattern
static const int cPATTERN_SIZE = 16;

class OpenGLRenderer : public Renderer {
private:
    PPU *ppu;
    
    GLuint m_defaultFBOName;
    GLuint m_viewWidth;
    GLuint m_viewHeight;
    
    // We'll multiply by four to have one texture for each possible value of attr table
    GLuint patterns[cPATTERNS * 4];
    GLuint pattern_fbos[cPATTERNS * 4];
    
    void generate_texture_data_for_pattern(int i, GLubyte *data, int attr_bits);

public:
    OpenGLRenderer(PPU *ppu);
    virtual ~OpenGLRenderer();
    virtual void resize(GLuint width, GLuint height);
    virtual void render();
    virtual void update_patterns();
};

#endif