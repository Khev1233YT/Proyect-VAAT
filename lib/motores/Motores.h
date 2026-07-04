#ifndef MOTORES_H
#define MOTORES_H

#include <Arduino.h>

#include <Config.h>

namespace Motores
{

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

/**
 * @brief Inicializa los motores y el PWM.
 */
void begin();

/**
 * @brief Actualiza el estado interno de los motores.
 *
 * Gestiona las rampas de aceleración y frenado,
 * aplicando progresivamente la velocidad objetivo.
 *
 * Debe ejecutarse periódicamente desde el
 * controlador principal.
 */
void update();

/***************************************************
 *          MOVIMIENTOS BÁSICOS
 ***************************************************/

/**
 * @brief Avanza utilizando la velocidad nominal.
 */
void avanzar();

/**
 * @brief Retrocede utilizando la velocidad nominal.
 */
void retroceder();

/**
 * @brief Gira sobre su eje hacia la izquierda.
 */
void girarIzquierda();

/**
 * @brief Gira sobre su eje hacia la derecha.
 */
void girarDerecha();

/**
 * @brief Detiene ambos motores utilizando la
 * rampa de frenado configurada.
 */
void detener();

/**
 * @brief Detiene inmediatamente ambos motores.
 *
 * No utiliza rampa de frenado.
 */
void frenoEmergencia();

/***************************************************
 *      CONTROL INDIVIDUAL
 ***************************************************/

/**
 * @brief Establece la velocidad objetivo del
 * motor izquierdo.
 *
 * Rango permitido:
 * -PWM_MAX ... PWM_MAX
 */
void setVelocidadIzquierda(int16_t pwm);

/**
 * @brief Establece la velocidad objetivo del
 * motor derecho.
 *
 * Rango permitido:
 * -PWM_MAX ... PWM_MAX
 */
void setVelocidadDerecha(int16_t pwm);

/**
 * @brief Establece simultáneamente la velocidad
 * objetivo de ambos motores.
 */
void setVelocidades(
    int16_t izquierda,
    int16_t derecha);

/***************************************************
 *              CONSULTAS
 ***************************************************/

/**
 * @brief Devuelve la velocidad actual aplicada
 * al motor izquierdo.
 */
int16_t getVelocidadIzquierda();

/**
 * @brief Devuelve la velocidad actual aplicada
 * al motor derecho.
 */
int16_t getVelocidadDerecha();

/**
 * @brief Indica si ambos motores están detenidos.
 *
 * @return true si ambos motores tienen velocidad
 * igual a cero.
 */
bool estaDetenido();

} // namespace Motores

#endif