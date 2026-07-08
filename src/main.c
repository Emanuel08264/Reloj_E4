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
#include <string.h>

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
static bool confirmar_alarma = false;

static volatile uint32_t tick_counter = 0;
static volatile bool punto_segundo = false;

static bool alarma_sonando = false;

static bool actualizar_teclas = false;

/* === Public variable definition  ============================================================= */

/* === Private function definitions ============================================================ */

/**
 * @brief Callback de interrupción para el evento de alarma.
 * * Activa la bandera de alarma y habilita la salida física del buzzer.
 * * @param reloj Puntero a la instancia del reloj que disparó el evento.
 */
void SonarAlarma(clock_t reloj) {
    alarma_sonando = true;
    DigitalOutputActivate(placa->buzzer);
}

/**
 * @brief Evalúa las entradas y determina el próximo estado del sistema.
 * * Implementa las transiciones de la máquina de estados del reloj.
 * Si la alarma está sonando, bloquea las transiciones hacia los menús de ajuste.
 * * @param estado_actual El estado actual en el que se encuentra el reloj.
 * @param placa Puntero a la estructura que contiene el hardware (botones).
 * @param reloj Puntero al objeto reloj con la lógica de tiempo.
 * @return estado_t El nuevo estado al que debe pasar el sistema.
 */
estado_t LogicaEstadoSiguiente(estado_t estado_actual, board_t placa, clock_t reloj) {
    estado_t estado_siguiente = estado_actual;
    switch (estado_actual) {
    case HORA_SIN_AJUSTAR:
        if ((time_on_f1 / TICKS_PER_SECOND) >= 3) {
            estado_siguiente = AJUSTE_MINUTOS_ACTUAL;
        }
        break;
    case MOSTRANDO_HORA:
        if (alarma_sonando) {
            break;
        }
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
        if (DigitalInputHasActivated(placa->cancel) || idle_time / TICKS_PER_SECOND >= 30) {
            estado_siguiente = MOSTRANDO_HORA;
        }
        if (DigitalInputHasActivated(placa->accept)) {
            estado_siguiente = MOSTRANDO_HORA;
            confirmar_alarma = true;
        }

        break;
    default:
        estado_siguiente = HORA_SIN_AJUSTAR;
        break;
    }
    return estado_siguiente;
}

/**
 * @brief Gestiona el ajuste manual de horas o minutos.
 * * Realiza el incremento o decremento cíclico de los campos de hora/minuto
 * según las teclas F3/F4.
 * * @param ajuste Puntero al buffer que contiene la hora a modificar.
 * @param minutos true para ajustar minutos (índices 2 y 3), false para horas (índices 0 y 1).
 */
static void incrementar_y_decrementar(hora_t ajuste, bool minutos) {
    uint8_t minutos_totales;
    uint8_t horas_totales;

    minutos_totales = ajuste[2] * 10 + ajuste[3];
    horas_totales = ajuste[0] * 10 + ajuste[1];

    if (minutos) {
        if (DigitalInputHasActivated(placa->f3)) {
            if (minutos_totales != 0) {
                minutos_totales--;
            } else {
                minutos_totales = 59;
            }
        }
        if (DigitalInputHasActivated(placa->f4)) {
            if (minutos_totales != 59) {
                minutos_totales++;
            } else {
                minutos_totales = 0;
            }
        }
    } else {
        if (DigitalInputHasActivated(placa->f3)) {
            if (horas_totales != 0) {
                horas_totales--;
            } else {
                horas_totales = 23;
            }
        }
        if (DigitalInputHasActivated(placa->f4)) {
            if (horas_totales != 23) {
                horas_totales++;
            } else {
                horas_totales = 0;
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
        if (actualizar_teclas) {
            actualizar_teclas = false;

            estado = LogicaEstadoSiguiente(estado, placa, reloj);
            if (estado != estado_anterior) {
                idle_time = 0;
                switch (estado) {
                case HORA_SIN_AJUSTAR:
                    DisplayWriteBCD(placa->display, DEFAULT_TIME, 4);
                    DisplayClearDots(placa->display, 0, 3);
                    DisplaySetDots(placa->display, 1, 1);
                    tick_counter = 0;
                    punto_segundo = false;
                    DisplayFlashDigits(placa->display, 0, 3, TICKS_PER_SECOND / 4);
                    break;
                case MOSTRANDO_HORA:
                    DisplayFlashDigits(placa->display, 0, 0, 0);
                    DisplayClearDots(placa->display, 0, 3);
                    DisplaySetDots(placa->display, 1, 1);
                    tick_counter = 0;
                    punto_segundo = false;
                    if (estado_anterior == AJUSTE_HORA_ACTUAL && confirmar_ajuste) {
                        RelojSetupCurrentTime(reloj, hora_ajuste);
                        confirmar_ajuste = false;
                    }
                    if (estado_anterior == AJUSTE_HORA_ALARMA && confirmar_alarma) {
                        RelojSetupAlarm(reloj, hora_ajuste);
                        confirmar_alarma = false;
                    }
                    if (RelojGetAlarm(reloj, NULL)) {
                        DisplaySetDots(placa->display, 3, 3);
                    }
                    break;
                case AJUSTE_MINUTOS_ACTUAL:
                    DisplayFlashDigits(placa->display, 2, 3, TICKS_PER_SECOND / 4);
                    DisplayClearDots(placa->display, 0, 3);
                    DisplaySetDots(placa->display, 1, 1);
                    if (RelojGetCurrentTime(reloj, hora_ajuste) == false) {
                        memcpy(hora_ajuste, DEFAULT_TIME, sizeof(hora_t));
                    }
                    break;
                case AJUSTE_HORA_ACTUAL:
                    DisplayFlashDigits(placa->display, 0, 1, TICKS_PER_SECOND / 4);
                    break;
                case AJUSTE_MINUTOS_ALARMA:
                    DisplayFlashDigits(placa->display, 2, 3, TICKS_PER_SECOND / 4);
                    DisplayClearDots(placa->display, 0, 3);
                    DisplaySetDots(placa->display, 0, 3);
                    if (RelojGetAlarm(reloj, hora_ajuste) == false) {
                        memcpy(hora_ajuste, DEFAULT_TIME, sizeof(hora_t));
                    }
                    break;
                case AJUSTE_HORA_ALARMA:
                    DisplayFlashDigits(placa->display, 0, 1, TICKS_PER_SECOND / 4);
                    break;
                default:
                    break;
                }
                estado_anterior = estado;
            } else {
                switch (estado) {
                case HORA_SIN_AJUSTAR:
                    if (punto_segundo) {
                        DisplayToggleDots(placa->display, 1, 1);
                        punto_segundo = false;
                    }
                    break;
                case MOSTRANDO_HORA:
                    RelojGetCurrentTime(reloj, hora_actual);
                    DisplayWriteBCD(placa->display, hora_actual, 4);
                    if (punto_segundo) {
                        DisplayToggleDots(placa->display, 1, 1);
                        punto_segundo = false;
                    }
                    if (alarma_sonando) {
                        if (DigitalInputHasActivated(placa->cancel)) {
                            alarma_sonando = false;
                            DigitalOutputDeactivate(placa->buzzer);
                        }
                        if (DigitalInputHasActivated(placa->accept)) {
                            alarma_sonando = false;
                            RelojSnoozeAlarm(reloj, 5);
                            DigitalOutputDeactivate(placa->buzzer);
                        }
                    } else {
                        if (DigitalInputHasActivated(placa->cancel) && RelojGetAlarm(reloj, NULL)) {
                            RelojToggleAlarm(reloj);
                            DisplayClearDots(placa->display, 3, 3);
                        }
                        if (DigitalInputHasActivated(placa->accept) && !RelojGetAlarm(reloj, NULL)) {
                            RelojToggleAlarm(reloj);
                            DisplaySetDots(placa->display, 3, 3);
                        }
                    }
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
                    incrementar_y_decrementar(hora_ajuste, true);
                    DisplayWriteBCD(placa->display, hora_ajuste, 4);
                    break;
                case AJUSTE_HORA_ALARMA:
                    incrementar_y_decrementar(hora_ajuste, false);
                    DisplayWriteBCD(placa->display, hora_ajuste, 4);
                    break;
                default:
                    break;
                }
            }
            UpdateAllInputs(placa);
        }
    }
}

void SysTick_Handler(void) {
    DisplayRefresh(placa->display);
    RelojNewTick(reloj);

    static uint32_t debounce_tick = 0;
    debounce_tick++;
    if (debounce_tick >= 50) {
        debounce_tick = 0;
        actualizar_teclas = true;
    }

    tick_counter++;
    if (tick_counter >= TICKS_PER_SECOND / 2) {
        tick_counter = 0;
        punto_segundo = true;
    }

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
}
/* === End of documentation ==================================================================== */