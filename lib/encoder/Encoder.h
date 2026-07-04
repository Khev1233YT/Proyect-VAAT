#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

namespace Encoder
{

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

/**
 * @brief Inicializa el encoder AS5600.
 *
 * Debe llamarse una sola vez durante setup().
 */
void begin();

/***************************************************
 *              ACTUALIZACIÓN
 ***************************************************/

/**
 * @brief Actualiza la odometría del encoder.
 *
 * Debe ejecutarse periódicamente desde el
 * scheduler principal.
 */
void update();

/***************************************************
 *              ODOMETRÍA
 ***************************************************/

/**
 * @brief Reinicia la odometría.
 *
 * Reinicia:
 * - Ángulo acumulado.
 * - Distancia recorrida.
 *
 * No modifica la posición física del encoder.
 */
void reset();

/***************************************************
 *              CONSULTAS
 ***************************************************/

/**
 * @brief Devuelve el ángulo absoluto del AS5600.
 *
 * Rango:
 * 0° - 360°
 */
float getAngulo();

/**
 * @brief Devuelve el ángulo acumulado desde
 * el último reset.
 */
float getAnguloTotal();

/**
 * @brief Devuelve la distancia recorrida
 * en metros.
 */
float getDistancia();

/**
 * @brief Devuelve el número de vueltas
 * completas recorridas desde el último reset.
 */
int32_t getVueltas();

/**
 * @brief Indica si el encoder está disponible.
 */
bool disponible();

} // namespace Encoder

#endif