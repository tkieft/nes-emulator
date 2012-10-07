//
//  OpenGLRenderer.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/30/12.
//
//

#include "OpenGLRenderer.h"

#include <stdio.h>
#include <stdlib.h>

#define GetGLError()									\
{														\
    GLenum err = glGetError();                          \
    while (err != GL_NO_ERROR) {                        \
        printf("GLError %s set in File:%s Line:%d\n",   \
                GetGLErrorString(err),                  \
                __FILE__,                               \
                __LINE__);                              \
        err = glGetError();                             \
    }                                                   \
}



void OpenGLRenderer::render() {
	// Draw black to the framebuffer
    glClear(GL_COLOR_BUFFER_BIT);
    
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, textureFBO);
//
//    glBlitFramebuffer(0, 0, 256, 240, 0, 0, 256, 240,  GL_COLOR_BUFFER_BIT, GL_LINEAR);
//    
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void OpenGLRenderer::resize(GLuint width, GLuint height) {
	glViewport(0, 0, width, height);
	
	m_viewWidth = width;
	m_viewHeight = height;
}

OpenGLRenderer::OpenGLRenderer(PPU *ppu) {
    printf("%s %s", glGetString(GL_RENDERER), glGetString(GL_VERSION));
    
    ////////////////////////////////////////////////////
    // Build all of our and setup initial state here  //
    // Don't wait until our real time run loop begins //
    ////////////////////////////////////////////////////
    
    // This is always 0 in MacOS (but not iOS)
    m_defaultFBOName = 0;
    
    m_viewWidth = 100;
    m_viewHeight = 100;
    
    this->ppu = ppu;
    
    ////////////////////////////////////////////////
    // Set up OpenGL state that will never change //
    ////////////////////////////////////////////////

    // Always use this clear color
    glClearColor(0, 0, 0, 0);

//    /////////////////////////////////////
//    // Set up OpenGL Rendering Objects //
//    /////////////////////////////////////
//    
//    glGenTextures(1, &texture);
//    
//    glBindTexture(GL_TEXTURE_2D, texture);
//    
//    //except this time we will initialize the texture
//    //so that some cells start out alive.
//    GLubyte* data = (GLubyte *) malloc(256*256*4*sizeof(GLubyte));
//    GLubyte val;
//    for (int i = 0; i < 256 * 240 * 4; i+=4) {
//        if ((double)rand() / RAND_MAX > 0.5) {
//            val = 0;
//        }
//        else {
//            val = 255;
//        }
//        data[i] = data[i+1] = data[i+2] = val;
//        data[i+3] = 255;
//    }
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 240, 0, GL_RGBA,
//                 GL_UNSIGNED_BYTE, data);
//    
//    glGenFramebuffers(1, &textureFBO);
//    
//    glBindFramebuffer(GL_FRAMEBUFFER, textureFBO);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
//    
//    // Go back to the default framebuffer
//    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBOName);
//    glViewport(0, 0, m_viewWidth, m_viewHeight);
    
    // Draw our scene once without presenting the rendered image.
    //   This is done in order to pre-warm OpenGL
    // We don't need to present the buffer since we don't actually want the
    //   user to see this, we're only drawing as a pre-warm stage
    render();
    
    // Check for errors to make sure all of our setup went ok
    GetGLError();
}

OpenGLRenderer::~OpenGLRenderer() {
}
