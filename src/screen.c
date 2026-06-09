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

/** @file screen.c
 ** @brief Implementación de la pantalla para el reloj
 **/

/* === Headers files inclusions ================================================================ */

#include "screen.h"
#include <string.h>
#include <stdbool.h>

/* === Macros definitions ====================================================================== */

#define N_DIGITS              4
#define DISPLAY_MAX_INSTANCES 1

/* === Private data type declarations ========================================================== */

struct display_s {
    uint8_t digit;
    uint8_t vram[N_DIGITS];
    bool ocupado;
    struct display_driver_s driver[1];
    uint8_t flash_from;       // digito desde el cual se va a iniciar el parpadeo
    uint8_t flash_to;         // digito hasta el cual se va a finalizar el parpadeo
    uint16_t flash_frecuency; // frecuencia de parpadeo en Hz
    uint16_t flash_counter;   // contador para llevar el tiempo de parpadeo
};

/* === Private function declarations =========================================================== */

static display_t DisplayAllocate(void) {
    display_t self = NULL;
    static struct display_s instances[DISPLAY_MAX_INSTANCES] = {0};
    for (int indice = 0; indice < DISPLAY_MAX_INSTANCES; indice++) {
        if (!instances[indice].ocupado) {
            instances[indice].ocupado = true;
            self = &instances[indice];
            break;
        }
    }
    return self;
}

/* === Private variable definitions ============================================================ */

static const uint8_t lut[] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             // 0
    SEGMENT_B | SEGMENT_C,                                                             // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,                         // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         // 3
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,                                     // 4
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,                         // 5
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,             // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,                         // 9
    SEGMENT_A | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G                          // E error
};

/* === Public variable definition  ============================================================= */

/* === Private function definitions ============================================================ */

/* === Public function implementation ========================================================== */

display_t DisplayCreate(uint8_t digits, display_driver_t driver) {
    display_t self = DisplayAllocate();

    if (self != NULL) {
        self->digit = 0;
        memcpy(self->driver, driver, sizeof(struct display_driver_s));
        memset(self->vram, 0, sizeof(self->vram));
        self->flash_from = 0;
        self->flash_to = 0;
        self->flash_frecuency = 0;
        self->flash_counter = 0;
    }

    return self;
}

void DisplayWriteBCD(display_t display, uint8_t * number, uint8_t size) {
    memset(display->vram, 0, sizeof(display->vram));
    for (int i = 0; i < size; i++) {
        if (i >= N_DIGITS) {
            break;
        }
        if (number[i] < 10) {
            display->vram[i] = lut[number[i]];
        } else {
            display->vram[i] = lut[10];
        }
    }
}

void DisplayRefresh(display_t display) {

    display->driver->UpdateSegments(0);

    uint8_t segments_to_light = display->vram[display->digit];

    if (display->flash_frecuency > 0) {
        if (display->flash_from <= display->digit && display->digit <= display->flash_to) {
            if (display->flash_counter >= display->flash_frecuency / 2) {
                segments_to_light = 0;
            }
        }
    }

    if (display->digit == 0 && display->flash_frecuency > 0) {
        display->flash_counter++;
        if (display->flash_counter >= display->flash_frecuency) {
            display->flash_counter = 0;
        }
    }

    display->driver->UpdateDigits(display->digit);
    display->driver->UpdateSegments(segments_to_light);

    display->digit++;
    if (display->digit >= N_DIGITS) {
        display->digit = 0;
    }
}

void DisplayFlashDigits(display_t display, uint8_t from, uint8_t to, uint16_t frecuency) {
    if (from >= N_DIGITS || to >= N_DIGITS || from > to) {
        for (int i = 0; i < N_DIGITS; i++) {
            display->vram[i] = lut[10];
        }
        display->flash_from = 0;
        display->flash_to = 0;
        display->flash_frecuency = 0;
        return;
    }

    display->flash_from = from;
    display->flash_to = to;
    display->flash_frecuency = frecuency;
    display->flash_counter = 0;
}

void DisplayToggleDots(display_t display, uint8_t from, uint8_t to) {
    if (from >= N_DIGITS) {
        from = N_DIGITS - 1;
    }
    if (to >= N_DIGITS) {
        to = N_DIGITS - 1;
    }

    for (int i = from; i <= to; i++) {
        display->vram[i] ^= SEGMENT_P;
    }
}

/* === End of documentation ==================================================================== */