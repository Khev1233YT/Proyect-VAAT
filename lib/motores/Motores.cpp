#include "Motores.h"

#include <Debug.h>

namespace
{

/***************************************************
 *              CANALES PWM
 ***************************************************/
constexpr uint8_t CANAL_PWM_IZQUIERDO = 0;

constexpr uint8_t CANAL_PWM_DERECHO = 1;

/***************************************************
 *      VARIABLES DE CONTROL DE MOTORES
 ***************************************************/

// Velocidad solicitada por el programa.
int16_t velocidadObjetivoIzquierda = 0;
int16_t velocidadObjetivoDerecha = 0;

// Velocidad realmente aplicada al motor.
int16_t velocidadActualIzquierda = 0;
int16_t velocidadActualDerecha = 0;

// Control del periodo de actualización de la rampa.
uint32_t ultimoTiempoRampa = 0;

/***************************************************
 *          FUNCIONES AUXILIARES
 ***************************************************/

/**
 * @brief Limita un valor PWM al rango permitido.
 */
inline int16_t limitarPWM(int16_t pwm)
{
    return constrain(
        pwm,
        -Config::Motores::PWM_MAX,
         Config::Motores::PWM_MAX);
}

/***************************************************
 *      CALIBRACIÓN DE PWM DE ARRANQUE
 ***************************************************/

/**
 * @brief Garantiza el PWM mínimo necesario para
 * iniciar el movimiento de un motor.
 *
 * El PWM mínimo únicamente se aplica cuando el
 * motor arranca desde cero.
 */
int16_t corregirPWMArranque(
    int16_t pwm,
    int16_t pwmActual,
    uint8_t pwmAdelante,
    uint8_t pwmAtras)
{
    if (pwm == 0)
    {
        return 0;
    }

    // Si el motor ya está girando no se modifica.
    if (pwmActual != 0)
    {
        return pwm;
    }

    if (pwm > 0)
    {
        if (pwm < pwmAdelante)
        {
            pwm = pwmAdelante;
        }
    }
    else
    {
        if (-pwm < pwmAtras)
        {
            pwm = -pwmAtras;
        }
    }

    return pwm;
}

/***************************************************
 *          FUNCIONES PRIVADAS
 ***************************************************/

/**
 * @brief Aplica directamente el PWM al motor izquierdo.
 *
 * Esta función NO utiliza rampas.
 * Únicamente escribe sobre el L298N.
 */
void aplicarMotorIzquierdo(int16_t pwm)
{
    pwm = corregirPWMArranque(
        pwm,
        velocidadActualIzquierda,
        Config::Motores::PWM_MINIMO_IZQUIERDO_ADELANTE,
        Config::Motores::PWM_MINIMO_IZQUIERDO_ATRAS);

    pwm = limitarPWM(pwm);

    if (pwm > 0)
    {
        digitalWrite(Config::Motores::PIN_IN1, HIGH);
        digitalWrite(Config::Motores::PIN_IN2, LOW);

        ledcWrite(
            CANAL_PWM_IZQUIERDO,
            pwm);
    }
    else if (pwm < 0)
    {
        digitalWrite(Config::Motores::PIN_IN1, LOW);
        digitalWrite(Config::Motores::PIN_IN2, HIGH);

        ledcWrite(
            CANAL_PWM_IZQUIERDO,
            -pwm);
    }
    else
    {
        digitalWrite(Config::Motores::PIN_IN1, LOW);
        digitalWrite(Config::Motores::PIN_IN2, LOW);

        ledcWrite(
            CANAL_PWM_IZQUIERDO,
            0);
    }
}

/**
 * @brief Aplica directamente el PWM al motor derecho.
 *
 * Esta función NO utiliza rampas.
 * Únicamente escribe sobre el L298N.
 */
void aplicarMotorDerecho(int16_t pwm)
{
    pwm = corregirPWMArranque(
        pwm,
        velocidadActualDerecha,
        Config::Motores::PWM_MINIMO_DERECHO_ADELANTE,
        Config::Motores::PWM_MINIMO_DERECHO_ATRAS);

    pwm = limitarPWM(pwm);

    if (pwm > 0)
    {
        digitalWrite(Config::Motores::PIN_IN3, HIGH);
        digitalWrite(Config::Motores::PIN_IN4, LOW);

        ledcWrite(
            CANAL_PWM_DERECHO,
            pwm);
    }
    else if (pwm < 0)
    {
        digitalWrite(Config::Motores::PIN_IN3, LOW);
        digitalWrite(Config::Motores::PIN_IN4, HIGH);

        ledcWrite(
            CANAL_PWM_DERECHO,
            -pwm);
    }
    else
    {
        digitalWrite(Config::Motores::PIN_IN3, LOW);
        digitalWrite(Config::Motores::PIN_IN4, LOW);

        ledcWrite(
            CANAL_PWM_DERECHO,
            0);
    }
}

/***************************************************
 *          CONTROL DE RAMPA
 ***************************************************/

/**
 * @brief Actualiza la velocidad aplicada hasta
 * alcanzar la velocidad objetivo.
 */
void actualizarRampa(
    int16_t& velocidadActual,
    int16_t velocidadObjetivo)
{
    if (velocidadActual < velocidadObjetivo)
    {
        velocidadActual +=
            Config::Motores::RAMPA_ACELERACION;

        if (velocidadActual > velocidadObjetivo)
        {
            velocidadActual = velocidadObjetivo;
        }
    }
    else if (velocidadActual > velocidadObjetivo)
    {
        velocidadActual -=
            Config::Motores::RAMPA_FRENADO;

        if (velocidadActual < velocidadObjetivo)
        {
            velocidadActual = velocidadObjetivo;
        }
    }
}

} // namespace
namespace Motores
{

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/
void begin()
{
    pinMode(Config::Motores::PIN_IN1, OUTPUT);
    pinMode(Config::Motores::PIN_IN2, OUTPUT);

    pinMode(Config::Motores::PIN_IN3, OUTPUT);
    pinMode(Config::Motores::PIN_IN4, OUTPUT);

    ledcSetup(
        CANAL_PWM_IZQUIERDO,
        Config::Motores::PWM_FRECUENCIA,
        Config::Motores::PWM_RESOLUCION);

    ledcAttachPin(
        Config::Motores::PIN_PWM_IZQUIERDO,
        CANAL_PWM_IZQUIERDO);

    ledcSetup(
        CANAL_PWM_DERECHO,
        Config::Motores::PWM_FRECUENCIA,
        Config::Motores::PWM_RESOLUCION);

    ledcAttachPin(
        Config::Motores::PIN_PWM_DERECHO,
        CANAL_PWM_DERECHO);

    velocidadObjetivoIzquierda = 0;
    velocidadObjetivoDerecha = 0;

    velocidadActualIzquierda = 0;
    velocidadActualDerecha = 0;

    ultimoTiempoRampa = millis();

    aplicarMotorIzquierdo(0);
    aplicarMotorDerecho(0);

    Debug::println(F("Motores inicializados."));
}

/***************************************************
 *              ACTUALIZACIÓN
 ***************************************************/
void update()
{
    static uint32_t ultimoDebug = 0;

if (millis() - ultimoDebug >= 250)
{
    ultimoDebug = millis();

    Debug::println();
    Debug::println("===== ESTADO MOTORES =====");

    Debug::print("Objetivo Izq : ");
    Debug::println(velocidadObjetivoIzquierda);

    Debug::print("Actual   Izq : ");
    Debug::println(velocidadActualIzquierda);

    Debug::print("Objetivo Der : ");
    Debug::println(velocidadObjetivoDerecha);

    Debug::print("Actual   Der : ");
    Debug::println(velocidadActualDerecha);

    Debug::println("==========================");
}
    if (millis() - ultimoTiempoRampa <
        Config::Motores::PERIODO_RAMPA)
    {
        return;
    }

    ultimoTiempoRampa = millis();

    actualizarRampa(
        velocidadActualIzquierda,
        velocidadObjetivoIzquierda);

    actualizarRampa(
        velocidadActualDerecha,
        velocidadObjetivoDerecha);

    aplicarMotorIzquierdo(
        velocidadActualIzquierda);

    aplicarMotorDerecho(
        velocidadActualDerecha);
}

/***************************************************
 *          MOVIMIENTOS BÁSICOS
 ***************************************************/
void avanzar()
{
    setVelocidades(
        Config::Motores::VELOCIDAD_AVANCE,
        Config::Motores::VELOCIDAD_AVANCE);
}

void retroceder()
{
    setVelocidades(
        -Config::Motores::VELOCIDAD_AVANCE,
        -Config::Motores::VELOCIDAD_AVANCE);
}

void girarIzquierda()
{
    setVelocidades(
        -Config::Motores::VELOCIDAD_GIRO,
         Config::Motores::VELOCIDAD_GIRO);
}

void girarDerecha()
{
    setVelocidades(
         Config::Motores::VELOCIDAD_GIRO,
        -Config::Motores::VELOCIDAD_GIRO);
}

/***************************************************
 *                  DETENER
 ***************************************************/
void detener()
{
    setVelocidades(0, 0);
}

/***************************************************
 *          FRENO DE EMERGENCIA
 ***************************************************/
void frenoEmergencia()
{
    velocidadObjetivoIzquierda = 0;
    velocidadObjetivoDerecha = 0;

    velocidadActualIzquierda = 0;
    velocidadActualDerecha = 0;

    ultimoTiempoRampa = millis();

    aplicarMotorIzquierdo(0);
    aplicarMotorDerecho(0);

    Debug::println(F("Freno de emergencia."));
}
/***************************************************
 *      CONTROL MANUAL DE VELOCIDADES
 ***************************************************/

void setVelocidadIzquierda(int16_t pwm)
{
    velocidadObjetivoIzquierda = limitarPWM(pwm);
}

void setVelocidadDerecha(int16_t pwm)
{
    velocidadObjetivoDerecha = limitarPWM(pwm);
}

void setVelocidades(
    int16_t izquierda,
    int16_t derecha)
{
    setVelocidadIzquierda(izquierda);

    setVelocidadDerecha(derecha);
}

/***************************************************
 *              CONSULTAS
 ***************************************************/

int16_t getVelocidadIzquierda()
{
    return velocidadActualIzquierda;
}

int16_t getVelocidadDerecha()
{
    return velocidadActualDerecha;
}

bool estaDetenido()
{
    return
    (
        velocidadActualIzquierda == 0 &&
        velocidadActualDerecha == 0
    );
}

} // namespace Motores