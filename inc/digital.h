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

#ifndef DIGITAL_H_
#define DIGITAL_H_

/** @file digital.h
 ** @brief Declaraciones para el manejo de salidas digitales mediante tipo abstracto de datos
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

/** @brief Tipo opaco que representa una salida digital */
typedef struct digital_output_s * digital_output_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */


/**
 * @brief Crea y configura una salida digital
 *
 * @param port  Puerto GPIO al que pertenece el pin
 * @param pin   Número de bit dentro del puerto GPIO
 * @param inverted  Si es true, la lógica de activación se invierte
 * @return digital_output_t  Puntero al objeto creado, o NULL si falla la asignación
 */
digital_output_t DigitalOutputCreate(int32_t port, int8_t pin, bool inverted);

/**
 * @brief Activa una salida digital (pone el pin en estado activo)
 *
 * @param output  Salida digital a activar
 */
void DigitalOutputActivate(digital_output_t output);

/**
 * @brief Desactiva una salida digital (pone el pin en estado inactivo)
 *
 * @param output  Salida digital a desactivar
 */
void DigitalOutputDeactivate(digital_output_t output);

/**
 * @brief Alterna el estado de una salida digital
 *
 * @param output  Salida digital a alternar
 */
void DigitalOutputToggle(digital_output_t output);

/**
 * @brief Obtiene el estado de una salida digital
 *
 * @param output  Salida digital de la cual obtener el estado
 * @return true si la salida está activa, false en caso contrario
 */
bool DigitalOutputGetState(digital_output_t output);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITAL_H_ */