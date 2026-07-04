#include "Movimiento.h"

#include <Config.h>
#include <Debug.h>

#include <Encoder.h>
#include <IMU.h>
#include <Motores.h>
#include <Muestreador.h>

namespace
{

/***************************************************
 *          ESTADO DE LA BIBLIOTECA
 ***************************************************/

bool inicializado = false;

Movimiento::Estado estadoActual =
    Movimiento::Estado::DETENIDO;

/***************************************************
 *          OBJETIVOS DEL MOVIMIENTO
 ***************************************************/

float distanciaObjetivo = 0.0f;

float anguloObjetivo = 0.0f;

float rumboReferencia = 0.0f;

/***************************************************
 *          AVANCE RECTO
 ***************************************************/

void actualizarAvance()
{
    /*
     * Corrección proporcional del rumbo
     * utilizando la IMU.
     */

    float error =
        rumboReferencia -
        IMU::getAngulo();

    int16_t correccion =
        static_cast<int16_t>(
            error *
            Config::Movimiento::KP_RUMBO);

    correccion = constrain(
        correccion,
        -Config::Movimiento::CORRECCION_MAXIMA,
         Config::Movimiento::CORRECCION_MAXIMA);

    int16_t velocidadIzquierda =
        Config::Motores::VELOCIDAD_AVANCE -
        correccion;

    int16_t velocidadDerecha =
        Config::Motores::VELOCIDAD_AVANCE +
        correccion +
        Config::Movimiento::OFFSET_DIRECCION;

    Motores::setVelocidades(
        velocidadIzquierda,
        velocidadDerecha);

    /*
     * Verificar si ya se alcanzó
     * la distancia objetivo.
     */

    if (Encoder::getDistancia() <
        distanciaObjetivo)
    {
        return;
    }

    Motores::detener();

    estadoActual =
        Movimiento::Estado::ESPERANDO_DETENCION;

    Debug::println(
        F("Distancia alcanzada."));
}

/***************************************************
 *              GIRO
 ***************************************************/

void actualizarGiro()
{
    constexpr float TOLERANCIA = 2.0f;

    float error =
        anguloObjetivo -
        IMU::getAngulo();

    /*
     * Objetivo alcanzado.
     */

    if (fabs(error) <= TOLERANCIA)
    {
        Motores::detener();

        estadoActual =
            Movimiento::Estado::ESPERANDO_DETENCION;

        Debug::println(
            F("Ángulo alcanzado."));

        return;
    }

    int16_t velocidad =
        Config::Motores::VELOCIDAD_GIRO;

    /*
     * Reducir velocidad cuando
     * el error sea pequeño.
     */

    if (fabs(error) < 20.0f)
    {
        velocidad /= 2;
    }

    if (error > 0.0f)
    {
        Motores::setVelocidades(
            -velocidad,
             velocidad);
    }
    else
    {
        Motores::setVelocidades(
             velocidad,
            -velocidad);
    }
}
/***************************************************
 *          ACTUALIZAR MUESTREO
 ***************************************************/

void actualizarMuestreo()
{
    /*
     * Esperar a que el muestreador
     * complete todo su ciclo.
     */

    if (!Muestreador::finalizado())
    {
        return;
    }

    estadoActual =
        Movimiento::Estado::FINALIZADO;

    Debug::println(
        F("Muestreo completado."));
}

/***************************************************
 *      ESPERAR DETENCIÓN DE LOS MOTORES
 ***************************************************/

void actualizarDetencion()
{
    /*
     * Esperar a que la rampa de frenado
     * termine completamente.
     */

    if (!Motores::estaDetenido())
    {
        return;
    }

    estadoActual =
        Movimiento::Estado::FINALIZADO;

    Debug::println(
        F("Movimiento finalizado."));
}

/***************************************************
 *          MÁQUINA DE ESTADOS
 ***************************************************/

void actualizarFSM()
{
    switch (estadoActual)
    {
        case Movimiento::Estado::DETENIDO:
            break;

        case Movimiento::Estado::AVANZANDO:
            actualizarAvance();
            break;

        case Movimiento::Estado::GIRANDO:
            actualizarGiro();
            break;

        case Movimiento::Estado::MUESTREANDO:
            actualizarMuestreo();
            break;

        case Movimiento::Estado::ESPERANDO_DETENCION:
            actualizarDetencion();
            break;

        case Movimiento::Estado::FINALIZADO:
            break;

        case Movimiento::Estado::ERROR:
            break;
    }
}

} // namespace

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

bool Movimiento::begin()
{
    if (inicializado)
    {
        return true;
    }

    Debug::titulo("MOVIMIENTO");

    estadoActual =
        Estado::DETENIDO;

    distanciaObjetivo = 0.0f;

    anguloObjetivo = 0.0f;

    rumboReferencia = 0.0f;

    inicializado = true;

    Debug::println(
        F("Movimiento inicializado."));

    return true;
}

/***************************************************
 *              ACTUALIZACIÓN
 ***************************************************/

void Movimiento::update()
{
    if (!inicializado)
    {
        return;
    }

    /*
     * Actualizar sensores.
     */

    Encoder::update();

    IMU::update();

    /*
     * Ejecutar la FSM.
     */

    actualizarFSM();

    /*
     * Actualizar actuadores.
     */

    Motores::update();

    Muestreador::update();
}
/***************************************************
 *                  AVANZAR
 ***************************************************/

void Movimiento::avanzar(float metros)
{
    if (!inicializado)
    {
        return;
    }

    if (ocupado())
    {
        return;
    }

    Encoder::reset();

    rumboReferencia =
        IMU::getAngulo();

    distanciaObjetivo = metros;

    estadoActual =
        Estado::AVANZANDO;

    Debug::printf(
        "Movimiento: Avanzar %.2f m\n",
        metros);
}

/***************************************************
 *                  GIRAR
 ***************************************************/

void Movimiento::girar(float grados)
{
    if (!inicializado)
    {
        return;
    }

    if (ocupado())
    {
        return;
    }

    anguloObjetivo =
        IMU::getAngulo() +
        grados;

    estadoActual =
        Estado::GIRANDO;

    Debug::printf(
        "Movimiento: Girar %.1f°\n",
        grados);
}

/***************************************************
 *                  MUESTREAR
 ***************************************************/

void Movimiento::muestrear()
{
    if (!inicializado)
    {
        return;
    }

    if (ocupado())
    {
        return;
    }

    Muestreador::bajar();

    estadoActual =
        Estado::MUESTREANDO;

    Debug::println(
        F("Movimiento: Iniciando muestreo."));
}

/***************************************************
 *                  DETENER
 ***************************************************/

void Movimiento::detener()
{
    Motores::detener();

    estadoActual =
        Estado::DETENIDO;

    Debug::println(
        F("Movimiento detenido."));
}

/***************************************************
 *                  RESET
 ***************************************************/

void Movimiento::reset()
{
    Motores::frenoEmergencia();

    Encoder::reset();

    IMU::reset();

    Muestreador::reset();

    distanciaObjetivo = 0.0f;

    anguloObjetivo = 0.0f;

    rumboReferencia = 0.0f;

    estadoActual =
        Estado::DETENIDO;

    Debug::println(
        F("Movimiento reiniciado."));
}

/***************************************************
 *              CONSULTAS
 ***************************************************/

Movimiento::Estado Movimiento::estado()
{
    return estadoActual;
}

/***************************************************
 *                  OCUPADO
 ***************************************************/

bool Movimiento::ocupado()
{
    switch (estadoActual)
    {
        case Estado::DETENIDO:
        case Estado::FINALIZADO:
        case Estado::ERROR:
            return false;

        default:
            return true;
    }
}

/***************************************************
 *              FINALIZADO
 ***************************************************/

bool Movimiento::finalizado()
{
    return
        estadoActual ==
        Estado::FINALIZADO;
}

/***************************************************
 *                  ERROR
 ***************************************************/

bool Movimiento::error()
{
    return
        estadoActual ==
        Estado::ERROR;
}