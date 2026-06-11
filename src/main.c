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

/* === Headers files inclusions =============================================================== */

#include "bsp.h"
#include "screen.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

static void Delay(void);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void Delay(void) {
    for (volatile int i = 0; i < 5000; i++) {
        __asm__("nop");
    }
}

/* === Public function implementation ========================================================== */

int main(void) {
    board_t placa = BoardCreate();

    uint8_t init_number[] = {6, 7, 6, 7};
    DisplayWriteBCD(placa->display, init_number, sizeof(init_number));

    int8_t editing_digit = -1;
    uint32_t delay_anti_rebote = 0;

    while (true) {

        delay_anti_rebote++;
        if (delay_anti_rebote >= 50) {
            delay_anti_rebote = 0;

            bool mode_has_changed = false;

            if (DigitalInputHasActivated(placa->f4)) {
                if (editing_digit == 0) {
                    editing_digit = -1;
                } else {
                    editing_digit = 0;
                }
                mode_has_changed = true;
            }
            if (DigitalInputHasActivated(placa->f3)) {
                if (editing_digit == 1) {
                    editing_digit = -1;
                } else {
                    editing_digit = 1;
                }
                mode_has_changed = true;
            }
            if (DigitalInputHasActivated(placa->f2)) {
                if (editing_digit == 2) {
                    editing_digit = -1;
                } else {
                    editing_digit = 2;
                }
                mode_has_changed = true;
            }
            if (DigitalInputHasActivated(placa->f1)) {
                if (editing_digit == 3) {
                    editing_digit = -1;
                } else {
                    editing_digit = 3;
                }
                mode_has_changed = true;
            }

            if (mode_has_changed) {
                if (editing_digit != -1) {
                    DisplayFlashDigits(placa->display, editing_digit, editing_digit, 50);
                } else {
                    DisplayFlashDigits(placa->display, 0, 0, 0);
                }
            }

            if (editing_digit != -1) {
                bool digit_has_changed = false;

                if (DigitalInputHasActivated(placa->accept)) {
                    if (init_number[editing_digit] < 9) {
                        init_number[editing_digit]++;
                    } else {
                        init_number[editing_digit] = 0;
                    }
                    digit_has_changed = true;
                }

                if (DigitalInputHasActivated(placa->cancel)) {
                    if (init_number[editing_digit] > 0) {
                        init_number[editing_digit]--;
                    } else {
                        init_number[editing_digit] = 9;
                    }
                    digit_has_changed = true;
                }

                if (init_number[editing_digit] == 0) {
                    DisplayToggleDots(placa->display, editing_digit, editing_digit);
                }

                if (digit_has_changed) {
                    DisplayWriteBCD(placa->display, init_number, sizeof(init_number));
                }
            }

            UpdateAllInputs(placa);
        }

        Delay();
        DisplayRefresh(placa->display);
    }

    return 0;
}
/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
