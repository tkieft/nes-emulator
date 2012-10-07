//
//  TerminalRenderer.cpp
//  Emulator
//
//  Created by Tyler Kieft on 10/6/12.
//
//

#include <iostream>
#include <iomanip>

#include "TerminalRenderer.h"

using namespace std;

TerminalRenderer::TerminalRenderer(PPU *ppu) {
    this->ppu = ppu;
}

TerminalRenderer::~TerminalRenderer() {
}

void TerminalRenderer::render() {
    // 30 rows, 32 cols
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 32; j++) {
            uint8_t character = ppu->read_memory(0x2000 + i * 32 + j);
            if (character != 0x24)
                cout << setw(2) << setfill('0') << hex << (int)character << " ";
            else
                cout << "   ";
        }
        
        cout << endl;
    }

    cout << endl << endl << endl;
}

void TerminalRenderer::resize(GLuint width, GLuint height) {
    // nop
}