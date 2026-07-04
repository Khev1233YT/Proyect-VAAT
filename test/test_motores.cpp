#include <Arduino.h>

#include <Debug.h>
#include <Motores.h>

void setup()
{
    Debug::begin();

    Debug::println();
    Debug::println("==========");
    Debug::println("PRUEBA MOTORES");
    Debug::println("==========");

    Motores::begin();
}

void loop()
{
    Debug::println("Avanzar");
    Motores::avanzar();
    delay(3000);

    Debug::println("Detener");
    Motores::detener();
    delay(2000);

    Debug::println("Retroceder");
    Motores::retroceder();
    delay(3000);

    Debug::println("Detener");
    Motores::detener();
    delay(2000);

    Debug::println("Giro izquierda");
    Motores::girarIzquierda();
    delay(2000);

    Debug::println("Detener");
    Motores::detener();
    delay(2000);

    Debug::println("Giro derecha");
    Motores::girarDerecha();
    delay(2000);

    Debug::println("Detener");
    Motores::detener();
    delay(3000);
}