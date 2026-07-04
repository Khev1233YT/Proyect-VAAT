#include "Muestreador.h"

#include <Config.h>
#include <Debug.h>

#include <ESP32Servo.h>

namespace
{

/***************************************************
 *          MOVIMIENTO DEL SERVO
 ***************************************************/

enum class MovimientoServo : uint8_t
{
    ACELERAR,
    FRENAR
};

/***************************************************
 *                  HARDWARE
 ***************************************************/

Servo servo;

/***************************************************
 *          ESTADO DE LA BIBLIOTECA
 ***************************************************/

bool inicializado = false;

Muestreador::Estado estadoActual =
    Muestreador::Estado::DETENIDO;

/***************************************************
 *          CONTROL DEL SERVO
 ***************************************************/

uint8_t pwmActual =
    Config::Servo::DETENER;

uint8_t pwmObjetivo =
    Config::Servo::DETENER;

MovimientoServo movimientoServo =
    MovimientoServo::ACELERAR;

/***************************************************
 *              SENSORES
 ***************************************************/

bool finalSuperior = false;

bool finalInferior = false;

/***************************************************
 *          TEMPORIZADORES
 ***************************************************/

uint32_t tiempoEstado = 0;

uint32_t ultimoServicio = 0;

/***************************************************
 *          ESCRIBIR SERVO
 ***************************************************/

void escribirServo()
{
    servo.write(static_cast<int>(pwmActual));
}

/***************************************************
 *      ¿SERVO COMPLETAMENTE DETENIDO?
 ***************************************************/

bool servoDetenido()
{
    return pwmActual == pwmObjetivo;
}

/***************************************************
 *          LEER FINALES
 ***************************************************/

void leerFinales()
{
    finalSuperior =
        digitalRead(Config::Servo::PIN_FINAL_SUPERIOR);

    finalInferior =
        digitalRead(Config::Servo::PIN_FINAL_INFERIOR);

    if (Config::Servo::FINAL_ACTIVO_BAJO)
    {
        finalSuperior = !finalSuperior;
        finalInferior = !finalInferior;
    }
}

/***************************************************
 *          ACTUALIZAR SERVO
 ***************************************************/

void actualizarServo()
{
    if (servoDetenido())
    {
        return;
    }

    const uint8_t incremento =
        (movimientoServo == MovimientoServo::ACELERAR)
        ? Config::Servo::RAMPA_ACELERACION
        : Config::Servo::RAMPA_FRENADO;

    int16_t siguiente =
        static_cast<int16_t>(pwmActual);

    if (pwmActual < pwmObjetivo)
    {
        siguiente += incremento;

        if (siguiente > pwmObjetivo)
        {
            siguiente = pwmObjetivo;
        }
    }
    else
    {
        siguiente -= incremento;

        if (siguiente < pwmObjetivo)
        {
            siguiente = pwmObjetivo;
        }
    }

    pwmActual = static_cast<uint8_t>(siguiente);

    escribirServo();
}

} // namespace
/***************************************************
 *          MÁQUINA DE ESTADOS
 ***************************************************/

void actualizarFSM()
{
    switch (estadoActual)
    {
        case Muestreador::Estado::DETENIDO:
            break;

        /*******************************************/

        case Muestreador::Estado::BAJANDO:

            if (millis() - tiempoEstado >=
                Config::Timeout::SERVO)
            {
                pwmObjetivo = Config::Servo::DETENER;

                movimientoServo =
                    MovimientoServo::FRENAR;

                estadoActual =
                    Muestreador::Estado::ERROR_TIMEOUT;

                Debug::println(
                    F("ERROR: Timeout bajando."));

                break;
            }

            if (finalInferior)
            {
                pwmObjetivo =
                    Config::Servo::DETENER;

                movimientoServo =
                    MovimientoServo::FRENAR;

                estadoActual =
                    Muestreador::Estado::FRENANDO_INFERIOR;

                Debug::println(
                    F("Final inferior detectado."));
            }

            break;

        /*******************************************/

        case Muestreador::Estado::FRENANDO_INFERIOR:

            if (servoDetenido())
            {
                tiempoEstado = millis();

                estadoActual =
                    Muestreador::Estado::ESPERANDO_MEDICION;

                Debug::println(
                    F("Servo detenido."));
            }

            break;

        /*******************************************/

        case Muestreador::Estado::ESPERANDO_MEDICION:

            if (millis() - tiempoEstado >=
                Config::Sensor::TIEMPO_MEDICION)
            {
                estadoActual =
                    Muestreador::Estado::LISTO;

                Debug::println(
                    F("Tiempo de estabilizacion finalizado."));
            }

            break;

        /*******************************************/

        case Muestreador::Estado::LISTO:
            break;

        /*******************************************/

        case Muestreador::Estado::SUBIENDO:

            if (millis() - tiempoEstado >=
                Config::Timeout::SERVO)
            {
                pwmObjetivo =
                    Config::Servo::DETENER;

                movimientoServo =
                    MovimientoServo::FRENAR;

                estadoActual =
                    Muestreador::Estado::ERROR_TIMEOUT;

                Debug::println(
                    F("ERROR: Timeout subiendo."));

                break;
            }

            if (finalSuperior)
            {
                pwmObjetivo =
                    Config::Servo::DETENER;

                movimientoServo =
                    MovimientoServo::FRENAR;

                estadoActual =
                    Muestreador::Estado::FRENANDO_SUPERIOR;

                Debug::println(
                    F("Final superior detectado."));
            }

            break;

        /*******************************************/

        case Muestreador::Estado::FRENANDO_SUPERIOR:

            if (servoDetenido())
            {
                estadoActual =
                    Muestreador::Estado::FINALIZADO;

                Debug::println(
                    F("Servo detenido."));
            }

            break;

        /*******************************************/

        case Muestreador::Estado::FINALIZADO:
            break;

        /*******************************************/

        case Muestreador::Estado::ERROR_TIMEOUT:
            break;
    }
}

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

bool Muestreador::begin()
{
    if (inicializado)
    {
        return true;
    }

    Debug::titulo("MUESTREADOR");

    pinMode(
        Config::Servo::PIN_FINAL_SUPERIOR,
        INPUT_PULLUP);

    pinMode(
        Config::Servo::PIN_FINAL_INFERIOR,
        INPUT_PULLUP);

    servo.setPeriodHertz(50);

    servo.attach(
        Config::Servo::PIN);

    /***************************************************
     *      ESTADO INICIAL DEL SERVO
     ***************************************************/

    pwmActual =
        Config::Servo::DETENER;

    pwmObjetivo =
        Config::Servo::DETENER;

    escribirServo();

    /***************************************************
     *      ESTADO INICIAL DE LA FSM
     ***************************************************/

    leerFinales();

    estadoActual =
        Estado::DETENIDO;

    inicializado = true;

    Debug::println(
        F("Inicializado correctamente."));

    return true;
}

/***************************************************
 *              ACTUALIZACIÓN
 ***************************************************/

void Muestreador::update()
{
    if (!inicializado)
    {
        return;
    }

    if (millis() - ultimoServicio <
        Config::Update::SERVO)
    {
        return;
    }

    ultimoServicio = millis();

    leerFinales();

    actualizarFSM();

    actualizarServo();
}
/***************************************************
 *              BAJAR
 ***************************************************/

void Muestreador::bajar()
{
    if (!inicializado)
    {
        return;
    }

    if (estadoActual != Estado::DETENIDO)
    {
        return;
    }

    pwmObjetivo = Config::Servo::BAJAR;

    movimientoServo = MovimientoServo::ACELERAR;

    tiempoEstado = millis();

    estadoActual = Estado::BAJANDO;

    Debug::println(F("Bajando muestreador."));
}

/***************************************************
 *              SUBIR
 ***************************************************/

void Muestreador::subir()
{
    if (!inicializado)
    {
        return;
    }

    if (estadoActual != Estado::LISTO)
    {
        return;
    }

    pwmObjetivo = Config::Servo::SUBIR;

    movimientoServo = MovimientoServo::ACELERAR;

    tiempoEstado = millis();

    estadoActual = Estado::SUBIENDO;

    Debug::println(F("Subiendo muestreador."));
}

/***************************************************
 *              DETENER
 ***************************************************/

void Muestreador::detener()
{
    pwmObjetivo = Config::Servo::DETENER;

    movimientoServo = MovimientoServo::FRENAR;

    estadoActual = Estado::DETENIDO;

    Debug::println(F("Muestreador detenido."));
}

/***************************************************
 *              RESET
 ***************************************************/

void Muestreador::reset()
{
    if (!inicializado)
    {
        return;
    }

    leerFinales();

    if (finalSuperior)
    {
        detener();

        return;
    }

    pwmObjetivo = Config::Servo::SUBIR;

    movimientoServo = MovimientoServo::ACELERAR;

    tiempoEstado = millis();

    estadoActual = Estado::SUBIENDO;

    Debug::println(F("Buscando posicion inicial."));
}

/***************************************************
 *              CONSULTAS
 ***************************************************/

Muestreador::Estado Muestreador::estado()
{
    return estadoActual;
}

bool Muestreador::ocupado()
{
    switch (estadoActual)
    {
        case Estado::BAJANDO:
        case Estado::FRENANDO_INFERIOR:
        case Estado::ESPERANDO_MEDICION:
        case Estado::SUBIENDO:
        case Estado::FRENANDO_SUPERIOR:
            return true;

        default:
            return false;
    }
}

bool Muestreador::listo()
{
    return estadoActual == Estado::LISTO;
}

bool Muestreador::finalizado()
{
    return estadoActual == Estado::FINALIZADO;
}

bool Muestreador::error()
{
    return estadoActual == Estado::ERROR_TIMEOUT;
}

bool Muestreador::finalSuperior()
{
    return ::finalSuperior;
}

bool Muestreador::finalInferior()
{
    return ::finalInferior;
}