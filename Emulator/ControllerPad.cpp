    //
//  ControllerPad.cpp
//  Emulator
//
//  Created by Tyler Kieft on 10/7/12.
//
//

#include "ControllerPad.h"

ControllerPad::ControllerPad() {
    controller_1_select = false;
    controller_1_start = false;
    controller_1_a = false;
    controller_1_b = false;
    controller_1_left = false;
    controller_1_up = false;
    controller_1_right = false;
    controller_1_down = false;
}

uint8_t ControllerPad::read_controller_1_state() {
    current_read_key += 1;

    switch (current_read_key) {
        case 1:
            return controller_1_a       ? 0x01 : 0x00;
        case 2:
            return controller_1_b       ? 0x01 : 0x00;
        case 3:
            return controller_1_select  ? 0x01 : 0x00;
        case 4:
            return controller_1_start   ? 0x01 : 0x00;
        case 5:
            return controller_1_up      ? 0x01 : 0x00;
        case 6:
            return controller_1_down    ? 0x01 : 0x00;
        case 7:
            return controller_1_left    ? 0x01 : 0x00;
        case 8:
            return controller_1_right   ? 0x01 : 0x00;
        default:
            return 0x01; // Authentic controllers return 0x01 on subsequent reads?
    }
}

uint8_t ControllerPad::read_controller_2_state() {
    // TODO
    return 0;
}

void ControllerPad::write_value(uint8_t value) {
    if (value == 0x00 && previous_value == 0x01) {
        current_read_key = 0;
    }

    previous_value = value;
}

bool ControllerPad::record_key_down(SDL_Keysym sym) {
    return record_key_private(sym, true);
}

bool ControllerPad::record_key_up(SDL_Keysym sym) {
    return record_key_private(sym, false);
}

bool ControllerPad::record_key_private(SDL_Keysym sym, bool value) {
    switch (sym.scancode) {
        case SDL_SCANCODE_TAB:
            controller_1_select = value;
            return true;

        case SDL_SCANCODE_RETURN:
            controller_1_start = value;
            return true;

        case SDL_SCANCODE_LEFT:
            controller_1_left = value;
            return true;

        case SDL_SCANCODE_RIGHT:
            controller_1_right = value;
            return true;

        case SDL_SCANCODE_UP:
            controller_1_up = value;
            return true;

        case SDL_SCANCODE_DOWN:
            controller_1_down = value;
            return true;

        case SDL_SCANCODE_Z:
            controller_1_a = value;
            return true;

        case SDL_SCANCODE_X:
            controller_1_b = value;
            return true;

        default:
            return false;
    }
}
