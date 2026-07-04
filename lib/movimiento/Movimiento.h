#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H

#include <Arduino.h>

namespace Movimiento
{

/***************************************************
 *                  ESTADOS
 ***************************************************/

enum class Estado : uint8_t
{
    DETENIDO,

    AVANZANDO,

    GIRANDO,

    MUESTREANDO,

    /*
     * Esperando a que los motores terminen
     * de frenar mediante la rampa.
     */
    ESPERANDO_DETENCION,

    FINALIZADO,

    ERROR
};

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

/**
 * @brief Inicializa la biblioteca.
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
 *              COMANDOS
 ***************************************************/

/**
 * @brief Avanza una distancia determinada.
 *
 * @param metros Distancia en metros.
 */
void avanzar(float metros);

/**
 * @brief Gira un ángulo determinado.
 *
 * Convención:
 *
 * + grados -> izquierda.
 * - grados -> derecha.
 */
void girar(float grados);

/**
 * @brief Ejecuta un ciclo completo de muestreo.
 */
void muestrear();

/**
 * @brief Detiene inmediatamente el robot.
 */
void detener();

/**
 * @brief Reinicia la biblioteca al estado inicial.
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
 * @brief Indica si Movimiento está ejecutando
 * una operación.
 */
bool ocupado();

/**
 * @brief Indica si la operación terminó.
 */
bool finalizado();

/**
 * @brief Indica si ocurrió un error.
 */
bool error();

} // namespace Movimiento

#endif