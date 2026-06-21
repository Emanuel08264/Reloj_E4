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

#define SECONDS_PER_DAY 86400

/* === Private data type declarations ========================================================== */

struct clock_s {
    unsigned int time;
    bool time_is_valid;
    unsigned int ticks_per_second;
    unsigned int ticks_count;
    bool alarm_enabled;
    unsigned int alarm;
    alarm_handler_t alarm_handler;
};

/* === Private function declarations =========================================================== */

static unsigned int BCDToDecimal(const hora_t hora);
static void DecimalToBCD(unsigned int time_in_seconds, hora_t hora);

/* === Private variable definitions ============================================================ */

/* === Public variable definition  ============================================================= */

/* === Private function definitions ============================================================ */

static unsigned int BCDToDecimal(const hora_t hora) {
    unsigned int time_in_seconds;
    uint8_t horas_totales;
    uint8_t minutos_totales;
    uint8_t segundos_totales;

    horas_totales = 10 * hora[0] + hora[1];
    minutos_totales = 10 * hora[2] + hora[3];
    segundos_totales = 10 * hora[4] + hora[5];
    time_in_seconds = 3600 * horas_totales + 60 * minutos_totales + segundos_totales;

    return time_in_seconds;
}

static void DecimalToBCD(unsigned int time_in_seconds, hora_t hora) {
    uint8_t horas_totales;
    uint8_t minutos_totales;
    uint8_t segundos_totales;

    horas_totales = time_in_seconds / 3600;
    minutos_totales = (time_in_seconds % 3600) / 60;
    segundos_totales = time_in_seconds % 60;

    hora[0] = horas_totales / 10;
    hora[1] = horas_totales % 10;
    hora[2] = minutos_totales / 10;
    hora[3] = minutos_totales % 10;
    hora[4] = segundos_totales / 10;
    hora[5] = segundos_totales % 10;
}

/* === Public function implementation ========================================================== */

clock_t RelojCreate(unsigned int ticks_per_second, alarm_handler_t alarm_handler) {
    static struct clock_s instancia;

    clock_t self = &instancia;
    self->time = 0;
    self->time_is_valid = false;
    self->ticks_count = 0;
    self->ticks_per_second = ticks_per_second;
    self->alarm_enabled = false;
    self->alarm = 0;
    self->alarm_handler = alarm_handler;
    return self;
}

bool RelojGetCurrentTime(clock_t self, hora_t current_time) {
    DecimalToBCD(self->time, current_time);
    return self->time_is_valid;
}

bool RelojSetupCurrentTime(clock_t self, const hora_t current_time) {
    self->time = BCDToDecimal(current_time);
    self->time_is_valid = true;
    return true;
}

void RelojNewTick(clock_t self) {
    self->ticks_count++;
    if (self->ticks_count < self->ticks_per_second) {
        return;
    }
    self->ticks_count = 0;

    self->time++;

    if (self->time == SECONDS_PER_DAY) {
        self->time = 0;
    }
    if (self->alarm == self->time && self->alarm_handler != NULL && self->alarm_enabled == true) {
        self->alarm_handler(self);
    }
}

void RelojSetupAlarm(clock_t self, const hora_t alarm_time) {
    self->alarm = BCDToDecimal(alarm_time);
    self->alarm_enabled = true;
}

bool RelojGetAlarm(clock_t self, hora_t alarm_time) {
    DecimalToBCD(self->alarm, alarm_time);
    return self->alarm_enabled;
}

void RelojToggleAlarm(clock_t self) {
    self->alarm_enabled = !self->alarm_enabled;
}

void RelojSnoozeAlarm(clock_t self, const unsigned int snooze_time) {
    self->alarm = self->alarm + 60 * snooze_time;
}

/* === End of documentation ==================================================================== */