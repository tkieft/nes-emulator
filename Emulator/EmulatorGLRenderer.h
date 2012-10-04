//
//  EmulatorGLRenderer.h
//  Emulator
//
//  Created by Tyler Kieft on 9/30/12.
//
//

#ifndef Emulator_GL_Renderer_h
#define Emulator_GL_Renderer_h

#include "glUtil.h"

class EmulatorGLRenderer {
private:
    GLuint m_defaultFBOName;
    GLuint m_viewWidth;
    GLuint m_viewHeight;
    
    GLuint texture;
    GLuint textureFBO;

public:
    EmulatorGLRenderer(GLuint defaultFBOName);
    void resize(GLuint width, GLuint height);
    void render();
};

#endif