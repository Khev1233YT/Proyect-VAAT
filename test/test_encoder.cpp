#include <Arduino.h>

#include <Debug.h>
#include <Motores.h>
#include <Encoder.h>

/***************************************************
 *              CONFIGURACIÓN DE LA PRUEBA
 ***************************************************/

// Distancia que debe recorrer el robot.
constexpr float DISTANCIA_OBJETIVO = 1.0f;

// Tolerancia permitida.
// El robot comenzará a detenerse un poco antes
// para compensar la inercia.
constexpr float TOLERANCIA = 0.01f;

// Punto donde se inicia el frenado.
constexpr float DISTANCIA_PARADA =
    DISTANCIA_OBJETIVO - TOLERANCIA;

bool movimientoFinalizado = false;

/***************************************************
 *                  SETUP
 ***************************************************/
void setup()
{
    Debug::begin();

    Debug::println();
    Debug::println("====================================");
    Debug::println(" PRUEBA DE AVANCE POR DISTANCIA");
    Debug::println("====================================");

    Motores::begin();
    Encoder::begin();

    if (!Encoder::disponible())
    {
        Debug::println("ERROR: Encoder no detectado.");

        while (true)
        {
            delay(1000);
        }
    }

    Encoder::reset();

    Debug::println("Encoder reiniciado.");
    Debug::println("Iniciando avance...");
    Debug::println();

    Motores::avanzar();
}

/***************************************************
 *                  LOOP
 ***************************************************/
void loop()
{
    Motores::update();
    Encoder::update();

    /***************************************************
     *      INFORMACIÓN DE DEPURACIÓN
     ***************************************************/
    static uint32_t ultimoDebug = 0;

    if (millis() - ultimoDebug >= 200)
    {
        ultimoDebug = millis();

        Debug::print("Angulo: ");
        Debug::print(Encoder::getAngulo(), 2);

        Debug::print("°   Total: ");
        Debug::print(Encoder::getAnguloTotal(), 2);

        Debug::print("°   Vueltas: ");
        Debug::print(Encoder::getVueltas());

        Debug::print("   Distancia: ");
        Debug::print(Encoder::getDistancia(), 3);

        Debug::println(" m");
    }

    /***************************************************
     *      DETENER AL ALCANZAR LA DISTANCIA
     ***************************************************/
    if (!movimientoFinalizado &&
        Encoder::getDistancia() >= DISTANCIA_PARADA)
    {
        movimientoFinalizado = true;

        Motores::detener();

        Debug::println();
        Debug::println("====================================");
        Debug::println(" DISTANCIA OBJETIVO ALCANZADA");
        Debug::println("====================================");

        Debug::print("Distancia final: ");
        Debug::print(Encoder::getDistancia(), 3);
        Debug::println(" m");

        Debug::println("Robot detenido.");
    }
}