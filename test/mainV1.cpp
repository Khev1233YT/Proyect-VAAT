#include <Arduino.h>

#include <Debug.h>
#include <I2C.h>

#include <Motores.h>
#include <Encoder.h>
#include <IMU.h>

#include <Muestreador.h>
#include <SensorNPK.h>

#include <Movimiento.h>
#include <Mision.h>

void setup()
{
    /***************************************************
     *              DEPURACIÓN
     ***************************************************/
    Debug::begin();

    Debug::titulo("VAAT");

    /***************************************************
     *              BUSES
     ***************************************************/
    I2C::begin();

    /***************************************************
     *              ACTUADORES
     ***************************************************/
    Motores::begin();

    Muestreador::begin();

    /***************************************************
     *              SENSORES
     ***************************************************/
    Encoder::begin();

    IMU::begin();

    SensorNPK::begin();

    /***************************************************
     *          CALIBRACIÓN INICIAL
     ***************************************************/
    if (IMU::disponible())
    {
        IMU::calibrar();
    }

    /***************************************************
     *      POSICIÓN INICIAL DEL MUESTREADOR
     ***************************************************/
    Muestreador::reset();

    while (Muestreador::ocupado())
    {
        Muestreador::update();
    }

    /***************************************************
     *          MÓDULOS DE CONTROL
     ***************************************************/
    Movimiento::begin();

    Mision::begin();

    /***************************************************
     *          INICIAR MISIÓN
     ***************************************************/
    Mision::iniciar();

    Debug::titulo("SISTEMA LISTO");
}

void loop()
{
    /***************************************************
     *      SCHEDULER PRINCIPAL
     ***************************************************/
    Movimiento::update();

    Mision::update();

    /***************************************************
     *      FIN DE MISIÓN
     ***************************************************/
    if (Mision::finalizada())
    {
        Movimiento::detener();

        while (true)
        {
            delay(1000);
        }
    }

    /***************************************************
     *      ERROR GENERAL
     ***************************************************/
    if (Mision::error())
    {
        Movimiento::reset();

        while (true)
        {
            delay(1000);
        }
    }
}