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

#ifndef BSP_H_
#define BSP_H_

/** @file bsp.h
 ** @brief Definición de la placa de desarrollo y sus componentes
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digital.h"
#include "screen.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/** @brief Tipo para representar la placa PONCHO */
typedef struct board_s {

    digital_output_t buzzer; /**< Salida digital para el buzzer */
    digital_input_t f1;      /**< Entrada digital para la tecla f1 */
    digital_input_t f2;      /**< Entrada digital para la tecla f2 */
    digital_input_t f3;      /**< Entrada digital para la tecla f3 */
    digital_input_t f4;      /**< Entrada digital para la tecla f4 */
    digital_input_t accept;  /**< Entrada digital para la tecla accept */
    digital_input_t cancel;  /**< Entrada digital para la tecla cancel */
    display_t display;       /**< Descriptor para el manejo de la pantalla de 7 segmentos */

} const * const board_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea y configura la placa de desarrollo, inicializando sus componentes
 *
 * @return Puntero a la estructura que representa la placa de desarrollo
 */
board_t BoardCreate(void);

/**
 * @brief Actualiza el estado de todas las entradas de la placa
 *
 * @param self Puntero a la estructura que representa la placa de desarrollo
 */
void UpdateAllInputs(board_t self);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* BSP_H_ */