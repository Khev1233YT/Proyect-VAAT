#ifndef IMU_H
#define IMU_H

#include <Arduino.h>

namespace IMU
{

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

/**
 * @brief Inicializa el MPU6050.
 *
 * Debe llamarse una única vez durante setup().
 *
 * La función:
 * - Comprueba la presencia del sensor.
 * - Configura sus parámetros internos.
 * - Reinicia la orientación.
 *
 * No realiza la calibración del giroscopio.
 */
void begin();

/***************************************************
 *              ACTUALIZACIÓN
 ***************************************************/

/**
 * @brief Actualiza la orientación del robot.
 *
 * Debe ejecutarse periódicamente desde el
 * scheduler principal.
 *
 * La biblioteca integra automáticamente la
 * velocidad angular del eje Z para mantener
 * actualizado el rumbo del robot.
 */
void update();

/***************************************************
 *              CALIBRACIÓN
 ***************************************************/

/**
 * @brief Calibra el offset del giroscopio.
 *
 * Durante el proceso el robot debe permanecer
 * completamente inmóvil.
 *
 * El offset obtenido será utilizado para
 * compensar la deriva del sensor.
 *
 * Esta función puede ejecutarse siempre que
 * el programa lo considere necesario.
 *
 * @return true si la calibración finalizó
 * correctamente.
 */
bool calibrar();

/***************************************************
 *              ORIENTACIÓN
 ***************************************************/

/**
 * @brief Reinicia el rumbo del robot.
 *
 * Después del reset el ángulo vuelve a 0°.
 */
void reset();

/**
 * @brief Devuelve el rumbo actual del robot.
 *
 * Convención utilizada por VAAT:
 *
 *    0°  -> Frente
 *  +90°  -> Izquierda
 *  -90°  -> Derecha
 */
float getAngulo();

/**
 * @brief Devuelve la velocidad angular
 * instantánea.
 *
 * Corresponde al eje Z del giroscopio.
 *
 * Unidad:
 * grados por segundo.
 */
float getVelocidadAngular();

/***************************************************
 *              DEPURACIÓN
 ***************************************************/

/**
 * @brief Devuelve el offset calculado durante
 * la calibración.
 *
 * Principalmente útil para depuración y
 * diagnóstico del sensor.
 */
float getOffset();

/***************************************************
 *                  ESTADO
 ***************************************************/

/**
 * @brief Indica si el MPU6050 está disponible.
 */
bool disponible();

/**
 * @brief Indica si el giroscopio ya fue
 * calibrado correctamente.
 */
bool calibrado();

} // namespace IMU

#endif