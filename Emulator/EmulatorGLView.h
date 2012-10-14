//
//  EmulatorGLView.h
//  Emulator
//
//  Created by Tyler Kieft on 9/29/12.
//
//

#import <Cocoa/Cocoa.h>

#include "glUtil.h"

@interface EmulatorGLView : NSOpenGLView {
	CVDisplayLinkRef displayLink;
}

// -----------------------------------
// First Responder Methods
// -----------------------------------

- (BOOL)acceptsFirstResponder;

// -----------------------------------
// Handle Key Events
// -----------------------------------
- (void)keyDown:(NSEvent *)event;
- (void)keyUp:(NSEvent *)event;

@end
