#ifndef MUESTREADOR_H
#define MUESTREADOR_H

#include <Arduino.h>

namespace Muestreador
{

/***************************************************
 *                  ESTADOS
 ***************************************************/

enum class Estado : uint8_t
{
    DETENIDO,

    BAJANDO,
    FRENANDO_INFERIOR,
    ESPERANDO_MEDICION,
    LISTO,

    SUBIENDO,
    FRENANDO_SUPERIOR,

    FINALIZADO,

    ERROR_TIMEOUT
};

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

/**
 * @brief Inicializa el hardware del muestreador.
 *
 * Configura:
 *
 * - Servo
 * - Final superior
 * - Final inferior
 *
 * No mueve el mecanismo.
 */
bool begin();

/***************************************************
 *              ACTUALIZACIÓN
 ***************************************************/

/**
 * @brief Actualiza la máquina de estados.
 *
 * Debe llamarse continuamente desde loop().
 */
void update();

/***************************************************
 *              CONTROL
 ***************************************************/

/**
 * @brief Inicia la bajada del muestreador.
 *
 * Solo tiene efecto si el estado es DETENIDO.
 */
void bajar();

/**
 * @brief Inicia la subida del muestreador.
 *
 * Solo tiene efecto si el estado es LISTO.
 */
void subir();

/**
 * @brief Detiene el servo.
 */
void detener();

/**
 * @brief Lleva el mecanismo a la posición inicial.
 *
 * Se utiliza al arrancar el robot para garantizar
 * que el muestreador comienza desde el final de
 * carrera superior.
 */
void reset();

/***************************************************
 *              CONSULTAS
 ***************************************************/

/**
 * @brief Devuelve el estado actual.
 */
Estado estado();

/**
 * @brief Indica si el muestreador está ejecutando
 * una operación.
 */
bool ocupado();

/**
 * @brief Indica que el mecanismo ya está abajo y
 * estabilizado para comenzar la medición.
 */
bool listo();

/**
 * @brief Indica que el ciclo completo terminó.
 */
bool finalizado();

/**
 * @brief Indica si ocurrió un timeout.
 */
bool error();

/**
 * @brief Estado del final superior.
 */
bool finalSuperior();

/**
 * @brief Estado del final inferior.
 */
bool finalInferior();

} // namespace Muestreador

#endif