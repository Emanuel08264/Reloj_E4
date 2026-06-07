/*********************************************************************************************************************
Copyright 2026, Electronica 4
Facultad de Ciencias Exactas y Tecnología
Universidad Nacional de Tucuman

Copyright 2026, Emanuel Santillan <emanuelsantillan209gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*************************************************************************************************/

/** \brief EDU-CIAA-NXP board sample application
 **
 ** \addtogroup samples Samples
 ** \brief Samples applications with MUJU Framwork
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "placa.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/**
 * @brief Function to switch on and off a led with two keys
 */
static void SwitchLed(board_t self);

/**
 * @brief Function to switch on and off a led with a single key
 */
static void ToggleLed(board_t self);

/**
 * @brief Function to turn on a led while a key is pressed
 */
static void TestLed(board_t self);

/**
 * @brief Function to generate a delay of approximately 100 ms
 */
static void Delay(void);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void SwitchLed(board_t self) {
    if (DigitalInputHasActivated(self->on_led_rojo_k)) {
        DigitalOutputActivate(self->led_rojo);
    }
    if (DigitalInputHasActivated(self->off_led_rojo_k)) {
        DigitalOutputDeactivate(self->led_rojo);
    }
}

static void ToggleLed(board_t self) {
    if (DigitalInputHasActivated(self->toggle_led_amarillo_k)) {
        DigitalOutputToggle(self->led_amarillo);
    }
}

static void TestLed(board_t self) {
    if (DigitalInputGetState(self->test_led_verde_k)) {
        DigitalOutputActivate(self->led_verde);
    } else {
        DigitalOutputDeactivate(self->led_verde);
    }
}

static void Delay(void) {
    for (int index = 0; index < 100; index++) {
        for (int delay = 0; delay < 25000; delay++) {
            __asm("NOP");
        }
    }
}

/* === Public function implementation ========================================================== */

int main(void) {

    board_t placa = BoardCreate();

    while (true) {

        SwitchLed(placa);
        ToggleLed(placa);
        TestLed(placa);

        UpdateAllInputs(placa);

        Delay();
    }

    return 0;
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
