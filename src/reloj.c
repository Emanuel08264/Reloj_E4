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

/** @file reloj.c
 ** @brief Implementacion de las funciones de reloj
 **/

/* === Headers files inclusions ================================================================ */

#include "reloj.h"
#include <string.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

struct clock_s {
    hora_t current_time;
    bool time_is_valid;
    unsigned int ticks_per_second;
    unsigned int ticks_count;
};

/* === Private function declarations =========================================================== */

static bool incrementar_y_verificar(uint8_t * digito, uint8_t limite);

/* === Private variable definitions ============================================================ */

/* === Public variable definition  ============================================================= */

/* === Private function definitions ============================================================ */
static bool incrementar_y_verificar(uint8_t * digito, uint8_t limite) {
    (*digito)++;
    if (*digito == limite) {
        *digito = 0;
        return true;
    }
    return false;
}
/* === Public function implementation ========================================================== */

clock_t RelojCreate(unsigned int ticks_per_second, void * alarm_handler) {
    static struct clock_s instancia;

    clock_t self = &instancia;
    memset(self->current_time, 0, sizeof(hora_t));
    self->time_is_valid = false;
    self->ticks_count = 0;
    self->ticks_per_second = ticks_per_second;
    return self;
}

bool RelojGetCurrentTime(clock_t self, hora_t current_time) {
    memcpy(current_time, self->current_time, sizeof(hora_t));
    return self->time_is_valid;
}

bool RelojSetupCurrentTime(clock_t self, const hora_t current_time) {
    memcpy(self->current_time, current_time, sizeof(hora_t));
    self->time_is_valid = true;
    return true;
}

void RelojNewTick(clock_t self) {
    self->ticks_count++;
    if (self->ticks_count < self->ticks_per_second) {
        return;
    }
    self->ticks_count = 0;

    if (incrementar_y_verificar(&self->current_time[5], 10)) {
        if (incrementar_y_verificar(&self->current_time[4], 6)) {
            incrementar_y_verificar(&self->current_time[3], 10);
        }
    }
}

/* === End of documentation ==================================================================== */