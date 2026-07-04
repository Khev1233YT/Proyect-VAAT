#include "Encoder.h"

#include <AS5600.h>

#include <Config.h>
#include <Debug.h>
#include <I2C.h>

namespace
{

/***************************************************
 *              OBJETO AS5600
 ***************************************************/
AS5600 encoder;

/***************************************************
 *          VARIABLES PRIVADAS
 ***************************************************/

// Indica si el encoder fue detectado correctamente.
bool encoderDisponible = false;

// Ángulo absoluto leído del AS5600 (0° - 360°)
float anguloActual = 0.0f;

// Ángulo de la lectura anterior.
float anguloAnterior = 0.0f;

// Ángulo acumulado desde el último reset.
float anguloTotal = 0.0f;

// Distancia recorrida en metros.
float distancia = 0.0f;

/***************************************************
 *          FUNCIONES PRIVADAS
 ***************************************************/

/**
 * @brief Lee el ángulo absoluto del AS5600.
 *
 * @param angulo Variable donde se almacena
 * la lectura.
 *
 * @return true si la lectura fue correcta.
 */
bool leerSensor(float& angulo)
{
    if (!encoderDisponible)
    {
        return false;
    }

    uint16_t lectura = encoder.readAngle();

    angulo =
        (static_cast<float>(lectura) * 360.0f) /
        Config::Encoder::RESOLUCION;

    return true;
}

} // namespace

namespace Encoder
{

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

void begin()
{
    if (!I2C::ready())
    {
        Debug::println(
            F("ERROR: El bus I2C no ha sido inicializado."));

        encoderDisponible = false;

        return;
    }

    I2C::actualizar();

    if (!I2C::disponible(
            Config::Comunicacion::I2C::Direcciones::AS5600))
    {
        encoderDisponible = false;

        Debug::println(
            F("ERROR: AS5600 no encontrado."));

        return;
    }

    encoder.begin();

    encoderDisponible = true;

    anguloActual = 0.0f;
    anguloAnterior = 0.0f;
    anguloTotal = 0.0f;
    distancia = 0.0f;

    // Primera lectura para sincronizar
    // la odometría.
    leerSensor(anguloActual);

    anguloAnterior = anguloActual;

    Debug::println(
        F("Encoder AS5600 inicializado."));
}
/***************************************************
 *              ACTUALIZACIÓN
 ***************************************************/

void update()
{
    float nuevoAngulo;

    // Leer el encoder.
    if (!leerSensor(nuevoAngulo))
    {
        return;
    }

    /***************************************************
     *      CALCULAR DESPLAZAMIENTO ANGULAR
     ***************************************************/

    float delta = nuevoAngulo - anguloAnterior;

    // Corregir el salto de 359° -> 0°
    if (delta < -180.0f)
    {
        delta += 360.0f;
    }
    // Corregir el salto de 0° -> 359°
    else if (delta > 180.0f)
    {
        delta -= 360.0f;
    }

    /*
     * Ignorar pequeñas variaciones producidas
     * por el ruido del sensor.
     */
    if (fabs(delta) < Config::Encoder::ANGULO_MINIMO)
    {
        delta = 0.0f;
    }

    /***************************************************
     *      ACTUALIZAR ODOMETRÍA
     ***************************************************/

    anguloAnterior = nuevoAngulo;
    anguloActual = nuevoAngulo;

    anguloTotal += delta;

    distancia =
        (anguloTotal / 360.0f) *
        Config::Encoder::PERIMETRO_RUEDA;
}

/***************************************************
 *                  RESET
 ***************************************************/

void reset()
{
    anguloTotal = 0.0f;

    distancia = 0.0f;

    /*
     * Sincronizar la referencia con la posición
     * actual del encoder para evitar un salto
     * en la siguiente actualización.
     */
    if (leerSensor(anguloActual))
    {
        anguloAnterior = anguloActual;
    }
}
/***************************************************
 *                  CONSULTAS
 ***************************************************/

/**
 * @brief Devuelve el ángulo absoluto del AS5600.
 *
 * Rango:
 * 0° - 360°
 */
float getAngulo()
{
    return anguloActual;
}

/**
 * @brief Devuelve el ángulo acumulado desde
 * el último reset.
 */
float getAnguloTotal()
{
    return anguloTotal;
}

/**
 * @brief Devuelve la distancia recorrida
 * en metros.
 */
float getDistancia()
{
    return distancia;
}

/**
 * @brief Devuelve el número de vueltas
 * calculadas a partir del ángulo acumulado.
 */
int32_t getVueltas()
{
    return static_cast<int32_t>(
        anguloTotal / 360.0f);
}

/**
 * @brief Indica si el AS5600 está disponible.
 */
bool disponible()
{
    return encoderDisponible;
}

} // namespace Encoder