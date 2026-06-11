/*********************************************************************************************************************
Copyright 2026, Electronica 4
Facultad de Ciencias Exactas y Tecnología
Universidad Nacional de Tucuman

Copyright 2026, Emanuel Santillan <emanuelsantillan209gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*************************************************************************************************/

/** @file digital.c
 ** @brief Funciones para el manejo de señales digitales
 **/

/* === Headers files inclusions ================================================================ */

#include "digital.h"
#include "chip.h"
#include <stdlib.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

struct digital_output_s {
    int32_t port;
    int8_t pin;
    bool inverted;
};

struct digital_input_s {
    int32_t port;
    int8_t pin;
    bool inverted;
    bool last_state;
};

/* === Private function declarations =========================================================== */

/* === Private variable definitions ============================================================ */

/* === Public variable definition  ============================================================= */

/* === Private function definitions ============================================================ */

/* === Public function implementation ========================================================== */

digital_output_t DigitalOutputCreate(int32_t port, int8_t pin, bool inverted) {
    digital_output_t self = NULL;
    self = malloc(sizeof(struct digital_output_s));
    if (self) {
        self->port = port;
        self->pin = pin;
        self->inverted = inverted;
        DigitalOutputDeactivate(self);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->port, self->pin, true);
    }

    return self;
}

void DigitalOutputActivate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, !self->inverted);
}

void DigitalOutputDeactivate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, self->inverted);
}

void DigitalOutputToggle(digital_output_t self) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, self->port, self->pin);
}

bool DigitalOutputGetState(digital_output_t self) {
    bool state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, self->port, self->pin);
    if (self->inverted) {
        state = !state;
    }
    return state;
}

digital_input_t DigitalInputCreate(int32_t port, int8_t pin, bool inverted) {
    digital_input_t self = NULL;
    self = malloc(sizeof(struct digital_input_s));
    if (self) {
        self->port = port;
        self->pin = pin;
        self->inverted = inverted;
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->port, self->pin, false);
        self->last_state = DigitalInputGetState(self);
    }
    return self;
}

bool DigitalInputGetState(digital_input_t self) {
    bool state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, self->port, self->pin);
    if (self->inverted) {
        state = !state;
    }
    return state;
}

void DigitalInputUpdate(digital_input_t self) {
    self->last_state = DigitalInputGetState(self);
}

bool DigitalInputHasChanged(digital_input_t self) {
    bool changed = (DigitalInputGetState(self) != self->last_state);

    return changed;
}

bool DigitalInputHasActivated(digital_input_t self) {
    bool current_state = DigitalInputGetState(self);
    bool changed = current_state != self->last_state;
    bool activated = (changed && current_state);

    return activated;
}

bool DigitalInputHasDeactivated(digital_input_t self) {
    bool current_state = DigitalInputGetState(self);
    bool changed = current_state != self->last_state;
    bool deactivated = (changed && !current_state);

    return deactivated;
}

/* === End of documentation ==================================================================== */