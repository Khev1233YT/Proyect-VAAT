#include <Arduino.h>

#include <Debug.h>
#include <Muestreador.h>

void setup()
{
    Debug::begin();

    Debug::titulo("PRUEBA MUESTREADOR");

    if (!Muestreador::begin())
    {
        Debug::println("Error inicializando Muestreador.");

        while (true)
        {
        }
    }

    Debug::println("Iniciando ciclo...");

    Muestreador::bajar();
}

void loop()
{
    Muestreador::update();

    /***************************************************
     *      Mostrar estado cada 200 ms
     ***************************************************/

    static uint32_t ultimoDebug = 0;

    if (millis() - ultimoDebug >= 200)
    {
        ultimoDebug = millis();

        Debug::printf(
            "Estado:%d  Superior:%d  Inferior:%d\n",
            static_cast<int>(Muestreador::estado()),
            Muestreador::finalSuperior(),
            Muestreador::finalInferior());
    }

    /***************************************************
     *      Cuando termina el tiempo de muestreo
     ***************************************************/

    static bool subidaIniciada = false;

    if (!subidaIniciada &&
        Muestreador::listo())
    {
        subidaIniciada = true;

        Debug::println("Subiendo...");

        Muestreador::subir();
    }

    /***************************************************
     *      Fin del ciclo
     ***************************************************/

    static bool terminado = false;

    if (!terminado &&
        Muestreador::finalizado())
    {
        terminado = true;

        Debug::titulo("MUESTREO FINALIZADO");
    }

    /***************************************************
     *      Error
     ***************************************************/

    if (Muestreador::error())
    {
        Debug::titulo("ERROR TIMEOUT");

        while (true)
        {
        }
    }
}