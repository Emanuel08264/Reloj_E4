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
 ** @brief Implementación de las funciones de reloj
 **/

/* === Headers files inclusions ================================================================ */

#include "reloj.h"
#include <string.h>

/* === Macros definitions ====================================================================== */

#define SECONDS_PER_DAY 86400

/* === Private data type declarations ========================================================== */

/**
 * @brief Estructura interna que mantiene el estado y configuración del reloj.
 */
struct clock_s {
    unsigned int time;             /**< Hora actual del reloj en segundos absolutos (0 a 86399) */
    bool time_is_valid;            /**< Bandera que indica si el reloj fue puesto en hora */
    unsigned int ticks_per_second; /**< Frecuencia de ticks necesaria para avanzar 1 segundo */
    unsigned int ticks_count;      /**< Contador interno de ticks acumulados */
    bool alarm_enabled;            /**< Estado de la alarma (true = encendida, false = apagada) */
    unsigned int alarm;            /**< Hora original de la alarma en segundos absolutos */
    alarm_handler_t alarm_handler; /**< Callback a ejecutar cuando se dispara la alarma */
    bool snooze;                   /**< Bandera que indica si hay un pospuesto activo */
    unsigned int snooze_alarm;     /**< Hora pospuesta de la alarma en segundos absolutos */
};

/* === Private function declarations =========================================================== */

/**
 * @brief Convierte un arreglo de tiempo BCD a una cantidad de segundos absolutos.
 *
 * @param hora Arreglo de 6 bytes con la hora en formato [H_decenas, H_unidades, M_decenas, M_unidades, S_decenas,
 * S_unidades].
 * @return unsigned int Tiempo total equivalente en segundos desde las 00:00:00 (valor entre 0 y 86399).
 */
static unsigned int BCDToDecimal(const hora_t hora);

/**
 * @brief Convierte una cantidad de segundos absolutos a un arreglo de tiempo BCD.
 * * @param time_in_seconds Tiempo total en segundos (debe ser menor a 86400).
 * @param hora Arreglo de 6 bytes donde se escribirá el resultado en formato [H_decenas, H_unidades, M_decenas,
 * M_unidades, S_decenas, S_unidades].
 */
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
    self->snooze = false;
    self->snooze_alarm = 0;
    return self;
}

bool RelojGetCurrentTime(clock_t self, hora_t current_time) {
    DecimalToBCD(self->time, current_time);
    return self->time_is_valid;
}

bool RelojSetupCurrentTime(clock_t self, const hora_t current_time) {
    uint8_t horas = 10 * current_time[0] + current_time[1];
    uint8_t minutos = 10 * current_time[2] + current_time[3];
    uint8_t segundos = 10 * current_time[4] + current_time[5];

    if (horas > 23 || minutos > 59 || segundos > 59) {
        return false;
    }

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
    if (self->alarm == self->time && self->alarm_handler != NULL && self->alarm_enabled == true &&
        self->snooze == false) {
        self->alarm_handler(self);
    }
    if (self->snooze_alarm == self->time && self->alarm_handler != NULL && self->alarm_enabled == true &&
        self->snooze == true) {
        self->alarm_handler(self);
        self->snooze = false;
    }
}

bool RelojSetupAlarm(clock_t self, const hora_t alarm_time) {
    uint8_t horas = 10 * alarm_time[0] + alarm_time[1];
    uint8_t minutos = 10 * alarm_time[2] + alarm_time[3];
    uint8_t segundos = 10 * alarm_time[4] + alarm_time[5];

    if (horas > 23 || minutos > 59 || segundos > 59) {
        return false;
    }
    self->alarm = BCDToDecimal(alarm_time);
    self->alarm_enabled = true;
    self->snooze = false;
    return true;
}

bool RelojGetAlarm(clock_t self, hora_t alarm_time) {
    DecimalToBCD(self->alarm, alarm_time);
    return self->alarm_enabled;
}

void RelojToggleAlarm(clock_t self) {
    self->alarm_enabled = !self->alarm_enabled;
    if (!self->alarm_enabled && self->snooze) {
        self->snooze = false;
    }
}

void RelojSnoozeAlarm(clock_t self, const unsigned int snooze_time) {
    self->snooze_alarm = (self->alarm + 60 * snooze_time) % SECONDS_PER_DAY;
    self->snooze = true;
}

/* === End of documentation ==================================================================== */