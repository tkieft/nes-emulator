//
//  OpenGLRenderer.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/30/12.
//
//

#include "OpenGLRenderer.h"

#include <iostream>
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
    
    for (int row = 0; row < 30; row++) {
        for (int col = 0; col < 32; col++) {

            uint8_t character = ppu->read_memory(0x2000 + row * 32 + col);
            
            // FRIGGIN ATTRIBUTE TABLE! TODO: Optimize this.
            int attr_byte_address = (row / 4) * 8 + (col / 4);
            int attr_byte = ppu->read_memory(0x23C0 + attr_byte_address);
            
            int rowNum = (row & 0x02) >> 1;
            int colNum = (col & 0x02) >> 1;
            
            if (rowNum == 1 && colNum == 1) {
                attr_byte &= (0x03 << 6);
                attr_byte >>= 6;
            } else if (rowNum == 1 && colNum == 0) {
                attr_byte &= (0x03 << 4);
                attr_byte >>= 4;
            } else if (rowNum == 0 && colNum == 1) {
                attr_byte &= (0x03 << 2);
                attr_byte >>= 2;
            } else {
                attr_byte &= 0x03;
            }
                        
            glBindFramebuffer(GL_READ_FRAMEBUFFER, pattern_fbos[attr_byte * cPATTERNS + character + 256]); // TODO: differentiate between sprite & screen bgs

            // OpenGL coords are centered at the bottom left, so we must invert row.
            glBlitFramebuffer(0, 0, 8, 8, col * 8, (29 - row) * 8, (col + 1) * 8, (29 - row + 1) * 8,  GL_COLOR_BUFFER_BIT, GL_LINEAR);
        }
    }
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void OpenGLRenderer::resize(GLuint width, GLuint height) {
	glViewport(0, 0, width, height);
	
	m_viewWidth = width;
	m_viewHeight = height;
}

OpenGLRenderer::OpenGLRenderer(PPU *ppu) {
    printf("%s %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));
    
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
    glClear(GL_COLOR_BUFFER_BIT);

    /////////////////////////////////////
    // Set up OpenGL Rendering Objects //
    /////////////////////////////////////
    
    glGenTextures(cPATTERNS * 4, patterns);
    glGenFramebuffers(cPATTERNS * 4, pattern_fbos);
    
    // Check for errors to make sure all of our setup went ok
    GetGLError();
}

void OpenGLRenderer::update_patterns() {
    std::cout << "Updating patterns..." << std::endl;
    
    // Patterns are 8px by 8px, each pixel has RGBA color components
    GLubyte data[8 * 8 * 4];
    
    for (int attr_bits = 0; attr_bits < 4; attr_bits++) {
        for (int i = 0; i < cPATTERNS; i++) {
            glBindTexture(GL_TEXTURE_2D, patterns[attr_bits * cPATTERNS + i]);
            glBindFramebuffer(GL_FRAMEBUFFER, pattern_fbos[attr_bits * cPATTERNS + i]);
            
            generate_texture_data_for_pattern(i, data, attr_bits);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, patterns[attr_bits * cPATTERNS + i], 0);
        }
    }
    
    // Go back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBOName);
    glViewport(0, 0, m_viewWidth, m_viewHeight);
}

void OpenGLRenderer::generate_texture_data_for_pattern(int i, GLubyte *data, int attr_bits) {
    int patternStart = i * cPATTERN_SIZE;
    
    for (int patternByte = 0; patternByte < 8; patternByte++) {
        // OpenGL coords start from the bottom left
        uint8_t lowerByte = ppu->read_memory(patternStart + 7 - patternByte);
        uint8_t higherByte = ppu->read_memory(patternStart + 7 - patternByte + 8);
        
        /* Left to right is high to low */
        for (int patternBit = 7; patternBit >= 0; patternBit--) {
            // Generate the 4-bit palette entry key. The high bits are the attr. table bits,
            // and the low bits come from the pattern table.
            uint8_t palette_entry = (attr_bits << 2) |
                                    ((lowerByte & (1 << patternBit)) >> patternBit) |
            (patternBit == 0 ? ((higherByte & (1 << patternBit)) << 1) :
                               ((higherByte & (1 << patternBit)) >> (patternBit - 1)));
            

            // TODO: Sprite & BG palettes
            color_t color = NES_PALETTE[ppu->read_memory(PALETTE_TABLE_START + palette_entry)];

            int dataStart = patternByte * 8 * 4 + (7 - patternBit) * 4;
            data[dataStart    ] = color.r;
            data[dataStart + 1] = color.g;
            data[dataStart + 2] = color.b;
            data[dataStart + 3] = 255; // Alpha always = 1
        }
    }
}

OpenGLRenderer::~OpenGLRenderer() {
}
