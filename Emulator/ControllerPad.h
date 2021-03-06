//
//  ControllerPad.h
//  Emulator
//
//  Created by Tyler Kieft on 10/7/12.
//
//

#ifndef __Emulator__ControllerPad__
#define __Emulator__ControllerPad__

#include "SDL.h"
#include "defines.h"

class ControllerPad {
 private:
  int current_read_key;
  byte previous_value;

  bool controller_1_select;
  bool controller_1_start;
  bool controller_1_a;
  bool controller_1_b;
  bool controller_1_left;
  bool controller_1_up;
  bool controller_1_right;
  bool controller_1_down;

  bool record_key_private(SDL_Keysym sym, bool value);

 public:
  ControllerPad();
  bool record_key_down(SDL_Keysym sym);
  bool record_key_up(SDL_Keysym sym);

  byte read_controller_1_state();
  byte read_controller_2_state();
  void write_value(byte value);
};

#endif /* defined(__Emulator__ControllerPad__) */
