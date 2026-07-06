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

/** @file main.c
 ** @brief Programa principal del proyecto
 **/

/* === Headers files inclusions ================================================================ */

#include "bsp.h"
#include "reloj.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* === Macros definitions ====================================================================== */

#define TICKS_PER_SECOND 1000

/* === Private data type declarations ========================================================== */

typedef enum {
    HORA_SIN_AJUSTAR,
    MOSTRANDO_HORA,
    AJUSTE_HORA_ACTUAL,
    AJUSTE_MINUTOS_ACTUAL,
    AJUSTE_HORA_ALARMA,
    AJUSTE_MINUTOS_ALARMA
} estado_t;

/* === Private function declarations =========================================================== */

void SonarAlarma(clock_t reloj);
estado_t LogicaEstadoSiguiente(estado_t estado_actual, board_t placa, clock_t reloj);

/* === Private variable definitions ============================================================ */

static board_t placa;
static clock_t reloj;
static estado_t estado;

static volatile uint32_t time_on_f1 = 0;
static volatile uint32_t time_on_f2 = 0;
static volatile uint32_t idle_time = 0;

static hora_t DEFAULT_TIME = {0, 0, 0, 0, 0, 0};
static hora_t hora_ajuste = {0, 0, 0, 0, 0, 0};
static hora_t hora_actual;

static bool confirmar_ajuste = false;

static volatile uint32_t timer_rebote = 0;

/* === Public variable definition  ============================================================= */

/* === Private function definitions ============================================================ */

void SonarAlarma(clock_t reloj) {
}

estado_t LogicaEstadoSiguiente(estado_t estado_actual, board_t placa, clock_t reloj) {
    estado_t estado_siguiente = estado_actual;
    switch (estado_actual) {
    case HORA_SIN_AJUSTAR:
        if ((time_on_f1 / TICKS_PER_SECOND) >= 3) {
            estado_siguiente = AJUSTE_MINUTOS_ACTUAL;
        }
        break;
    case MOSTRANDO_HORA:
        if ((time_on_f1 / TICKS_PER_SECOND) >= 3) {
            estado_siguiente = AJUSTE_MINUTOS_ACTUAL;
        }
        if ((time_on_f2 / TICKS_PER_SECOND) >= 3) {
            estado_siguiente = AJUSTE_MINUTOS_ALARMA;
        }
        if (DigitalInputHasActivated(placa->accept) || DigitalInputHasActivated(placa->cancel)) {
            estado_siguiente = MOSTRANDO_HORA;
        }
        break;
    case AJUSTE_MINUTOS_ACTUAL:
        if (DigitalInputHasActivated(placa->accept)) {
            estado_siguiente = AJUSTE_HORA_ACTUAL;
        }
        if (DigitalInputHasActivated(placa->cancel) || idle_time / TICKS_PER_SECOND >= 30) {
            if (RelojGetCurrentTime(reloj, NULL)) {
                estado_siguiente = MOSTRANDO_HORA;
            } else {
                estado_siguiente = HORA_SIN_AJUSTAR;
            }
        }
        break;
    case AJUSTE_HORA_ACTUAL:
        if (DigitalInputHasActivated(placa->accept)) {
            estado_siguiente = MOSTRANDO_HORA;
            confirmar_ajuste = true;
        }
        if (DigitalInputHasActivated(placa->cancel) || idle_time / TICKS_PER_SECOND >= 30) {
            if (RelojGetCurrentTime(reloj, NULL)) {
                estado_siguiente = MOSTRANDO_HORA;
            } else {
                estado_siguiente = HORA_SIN_AJUSTAR;
            }
        }
        break;
    case AJUSTE_MINUTOS_ALARMA:
        if (DigitalInputHasActivated(placa->accept)) {
            estado_siguiente = AJUSTE_HORA_ALARMA;
        }
        if (DigitalInputHasActivated(placa->cancel) || idle_time / TICKS_PER_SECOND >= 30) {
            estado_siguiente = MOSTRANDO_HORA;
        }
        break;
    case AJUSTE_HORA_ALARMA:
        if (DigitalInputHasActivated(placa->accept) || DigitalInputHasActivated(placa->cancel) ||
            idle_time / TICKS_PER_SECOND >= 30) {
            estado_siguiente = MOSTRANDO_HORA;
        }
        break;
    default:
        estado_siguiente = HORA_SIN_AJUSTAR;
        break;
    }
    return estado_siguiente;
}

static void incrementar_y_decrementar(hora_t ajuste, bool minutos) {
    uint8_t minutos_totales;
    uint8_t horas_totales;

    minutos_totales = ajuste[2] * 10 + ajuste[3];
    horas_totales = ajuste[0] * 10 + ajuste[1];
    if (timer_rebote == 0) {
        if (minutos) {
            if (DigitalInputHasActivated(placa->f3)) {
                if (minutos_totales != 0) {
                    minutos_totales--;
                } else {
                    minutos_totales = 59;
                }
                timer_rebote = 100;
            }
            if (DigitalInputHasActivated(placa->f4)) {
                if (minutos_totales != 59) {
                    minutos_totales++;
                } else {
                    minutos_totales = 0;
                }
                timer_rebote = 100;
            }
        } else {
            if (DigitalInputHasActivated(placa->f3)) {
                if (horas_totales != 0) {
                    horas_totales--;
                } else {
                    horas_totales = 23;
                }
                timer_rebote = 100;
            }
            if (DigitalInputHasActivated(placa->f4)) {
                if (horas_totales != 23) {
                    horas_totales++;
                } else {
                    horas_totales = 0;
                }
                timer_rebote = 100;
            }
        }
    }
    ajuste[0] = horas_totales / 10;
    ajuste[1] = horas_totales % 10;
    ajuste[2] = minutos_totales / 10;
    ajuste[3] = minutos_totales % 10;
}

/* === Public function implementation ========================================================== */

int main(void) {
    placa = BoardCreate();
    reloj = RelojCreate(TICKS_PER_SECOND, SonarAlarma);
    estado = HORA_SIN_AJUSTAR;
    estado_t estado_anterior = MOSTRANDO_HORA; // Se inicializa con cualquier estado distinto a HORA_SIN_AJUSTAR para
                                               // que se ejecute la lógica de actualización de pantalla al inicio
    DisplayWriteBCD(placa->display, DEFAULT_TIME, 4);
    Systick_Init(TICKS_PER_SECOND);

    while (true) {
        estado = LogicaEstadoSiguiente(estado, placa, reloj);
        if (estado != estado_anterior) {
            idle_time = 0;
            switch (estado) {
            case HORA_SIN_AJUSTAR:
                DisplayWriteBCD(placa->display, DEFAULT_TIME, 4);
                DisplayFlashDigits(placa->display, 0, 3, TICKS_PER_SECOND / 2);
                break;
            case MOSTRANDO_HORA:
                DisplayFlashDigits(placa->display, 0, 0, 0);
                if (estado_anterior == AJUSTE_HORA_ACTUAL && confirmar_ajuste) {
                    RelojSetupCurrentTime(reloj, hora_ajuste);
                    confirmar_ajuste = false;
                }
                break;
            case AJUSTE_MINUTOS_ACTUAL:
                DisplayFlashDigits(placa->display, 2, 3, TICKS_PER_SECOND / 4);
                RelojGetCurrentTime(reloj, hora_ajuste);
                break;
            case AJUSTE_HORA_ACTUAL:
                DisplayFlashDigits(placa->display, 0, 1, TICKS_PER_SECOND / 4);
                break;
            case AJUSTE_MINUTOS_ALARMA:
                break;
            case AJUSTE_HORA_ALARMA:
                break;
            default:
                break;
            }
        }
        estado_anterior = estado;
        switch (estado) {
        case HORA_SIN_AJUSTAR:
            break;
        case MOSTRANDO_HORA:
            RelojGetCurrentTime(reloj, hora_actual);
            DisplayWriteBCD(placa->display, hora_actual, 4);
            break;
        case AJUSTE_MINUTOS_ACTUAL:
            incrementar_y_decrementar(hora_ajuste, true);
            DisplayWriteBCD(placa->display, hora_ajuste, 4);
            break;
        case AJUSTE_HORA_ACTUAL:
            incrementar_y_decrementar(hora_ajuste, false);
            DisplayWriteBCD(placa->display, hora_ajuste, 4);
            break;
        case AJUSTE_MINUTOS_ALARMA:
            break;
        case AJUSTE_HORA_ALARMA:
            break;
        default:
            break;
        }
        UpdateAllInputs(placa);
    }
}

void SysTick_Handler(void) {
    DisplayRefresh(placa->display);
    RelojNewTick(reloj);

    if (!DigitalInputGetState(placa->accept) && !DigitalInputGetState(placa->cancel) &&
        !DigitalInputGetState(placa->f1) && !DigitalInputGetState(placa->f2) && !DigitalInputGetState(placa->f3) &&
        !DigitalInputGetState(placa->f4)) {
        idle_time++;
    } else {
        idle_time = 0;
    }

    if (DigitalInputGetState(placa->f1)) {
        time_on_f1++;
    } else {
        time_on_f1 = 0;
    }
    if (DigitalInputGetState(placa->f2)) {
        time_on_f2++;
    } else {
        time_on_f2 = 0;
    }

    if (timer_rebote > 0) {
        timer_rebote--;
    }
}
/* === End of documentation ==================================================================== */