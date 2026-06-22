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

#ifndef RELOJ_H_
#define RELOJ_H_

/** @file reloj.h
 ** @brief Declaraciones de tipos de datos y funciones de reloj.
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/**
 * @brief Tipo de dato opaco que representa la instancia del reloj.
 */
typedef struct clock_s * clock_t;

/**
 * @brief Arreglo para representar la hora en formato BCD.
 * * El formato esperado es [H_decenas, H_unidades, M_decenas, M_unidades, S_decenas, S_unidades].
 * Ejemplo para las 14:30:05 -> {1, 4, 3, 0, 0, 5}
 */
typedef uint8_t hora_t[6];

/**
 * @brief Puntero a función (callback) para el evento de disparo de la alarma.
 * * @param reloj Instancia del reloj que disparó el evento.
 */
typedef void (*alarm_handler_t)(clock_t reloj);

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea e inicializa una instancia del reloj.
 * * @param ticks_per_second Cantidad de llamadas a RelojNewTick que equivalen a 1 segundo.
 * @param alarm_handler Función que se ejecutará cuando la hora coincida con la alarma habilitada.
 * @return clock_t Puntero a la instancia del reloj creada.
 */
clock_t RelojCreate(unsigned int ticks_per_second, alarm_handler_t alarm_handler);

/**
 * @brief Obtiene la hora actual del reloj en formato BCD.
 *
 * @param reloj Instancia del reloj.
 * @param current_time Arreglo donde se guardará la hora leída.
 * @return true Si la hora devuelta es válida (el reloj fue configurado previamente).
 * @return false Si el reloj nunca fue puesto en hora.
 * @note Si el reloj no fue configurado, current_time se rellena con 00:00:00.
 */
bool RelojGetCurrentTime(clock_t reloj, hora_t current_time);

/**
 * @brief Configura la hora actual del reloj.
 *
 * @param reloj Instancia del reloj.
 * @param current_time Arreglo con la nueva hora en formato BCD.
 * @return true Si los valores son válidos y la hora fue actualizada exitosamente.
 * @return false Si los valores están fuera de rango (horas > 23, minutos > 59 o segundos > 59).
 */
bool RelojSetupCurrentTime(clock_t reloj, const hora_t current_time);

/**
 * @brief Informa al reloj que ha transcurrido un ciclo de reloj (tick) del sistema.
 * * Esta función debe llamarse periódicamente desde un timer.
 * Se encarga de avanzar el tiempo y disparar la alarma si corresponde.
 * * @param reloj Instancia del reloj.
 */
void RelojNewTick(clock_t reloj);

/**
 * @brief Configura la hora de la alarma y la habilita automáticamente.
 * * Al configurar una nueva alarma, cualquier estado de "snooze" previo se cancela.
 * * @param reloj Instancia del reloj.
 * @param alarm_time Arreglo con la hora de la alarma en formato BCD.
 * @return true Si los valores son válidos y la alarma fue configurada.
 * @return false Si los valores de la hora proporcionada están fuera de rango.
 */
bool RelojSetupAlarm(clock_t reloj, const hora_t alarm_time);

/**
 * @brief Obtiene la hora programada de la alarma en formato BCD y su estado actual.
 * * @param reloj Instancia del reloj.
 * @param alarm_time Arreglo donde se guardará la hora programada de la alarma.
 * @return true Si la alarma se encuentra actualmente habilitada.
 * @return false Si la alarma se encuentra deshabilitada.
 */
bool RelojGetAlarm(clock_t reloj, hora_t alarm_time);

/**
 * @brief Alterna el estado de activación de la alarma (Encendido/Apagado).
 * * Si la alarma se desactiva mediante esta función, se cancelará automáticamente
 * cualquier evento de "posponer" (snooze) que estuviera en curso.
 * * @param reloj Instancia del reloj.
 */
void RelojToggleAlarm(clock_t reloj);

/**
 * @brief Pospone temporalmente la alarma.
 * * Si ya existe un pospuesto (snooze) activo, esta función lo sobrescribe con el nuevo
 * valor calculado a partir de la hora de la alarma original. Generalmente se llama
 * mientras la alarma está sonando.
 * * @param reloj Instancia del reloj.
 * @param snooze_time Cantidad de minutos a posponer.
 */
void RelojSnoozeAlarm(clock_t reloj, const unsigned int snooze_time);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* RELOJ_H_ */