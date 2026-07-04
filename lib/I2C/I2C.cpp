#include "I2C.h"

#include <Wire.h>

#include <Config.h>
#include <Debug.h>

namespace I2C
{

namespace
{

/***************************************************
 *          ESTADO INTERNO
 ***************************************************/

bool inicializado = false;

/***************************************************
 *      REGISTRO DE DISPOSITIVOS
 ***************************************************/

Dispositivo dispositivos[] =
{
    {
        "AS5600",
        Config::Comunicacion::I2C::Direcciones::AS5600,
        false
    },

    {
        "MPU6050",
        Config::Comunicacion::I2C::Direcciones::MPU6050,
        false
    },

    {
        "INA219",
        Config::Comunicacion::I2C::Direcciones::INA219,
        false
    }

};

constexpr uint8_t NUM_DISPOSITIVOS =
    sizeof(dispositivos) /
    sizeof(dispositivos[0]);

/***************************************************
 *      BÚSQUEDA DE DISPOSITIVOS
 ***************************************************/

Dispositivo* buscar(uint8_t direccion)
{
    for (uint8_t i = 0;
         i < NUM_DISPOSITIVOS;
         i++)
    {
        if (dispositivos[i].direccion == direccion)
        {
            return &dispositivos[i];
        }
    }

    return nullptr;
}

} // namespace

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

bool begin()
{
    if (inicializado)
    {
        return true;
    }

    Wire.begin(
        Config::Comunicacion::I2C::SDA,
        Config::Comunicacion::I2C::SCL);

    Wire.setClock(
        Config::Comunicacion::I2C::FRECUENCIA);

#if defined(ESP32)

    Wire.setTimeOut(
        Config::Comunicacion::I2C::TIMEOUT);

#endif

    inicializado = true;

    actualizar();

    Debug::println(
        F("Bus I2C inicializado."));

    return true;
}

/***************************************************
 *      ACTUALIZACIÓN DEL REGISTRO
 ***************************************************/

void actualizar()
{
    if (!inicializado)
    {
        return;
    }

    for (uint8_t i = 0;
         i < NUM_DISPOSITIVOS;
         i++)
    {
        Wire.beginTransmission(
            dispositivos[i].direccion);

        dispositivos[i].disponible =
            (Wire.endTransmission() == 0);
    }
}

/***************************************************
 *              CONSULTAS
 ***************************************************/

bool ready()
{
    return inicializado;
}

bool disponible(uint8_t direccion)
{
    Dispositivo* dispositivo =
        buscar(direccion);

    if (dispositivo == nullptr)
    {
        return false;
    }

    return dispositivo->disponible;
}

const Dispositivo* obtener(uint8_t direccion)
{
    return buscar(direccion);
}

/***************************************************
 *              DEPURACIÓN
 ***************************************************/

void scan()
{
    Debug::println();

    Debug::println(
        F("Estado del bus I2C"));

    Debug::println(
        F("------------------------------"));

    for (uint8_t i = 0;
         i < NUM_DISPOSITIVOS;
         i++)
    {
        Debug::print(dispositivos[i].nombre);

        if (strlen(dispositivos[i].nombre) < 8)
        {
            Debug::print("\t");
        }

        Debug::print("\t0x");

        if (dispositivos[i].direccion < 16)
        {
            Debug::print("0");
        }

        Debug::print(
            String(
                dispositivos[i].direccion,
                HEX));

        Debug::print("   ");

        if (dispositivos[i].disponible)
        {
            Debug::println("OK");
        }
        else
        {
            Debug::println("ERROR");
        }
    }

    Debug::println();
}

} // namespace I2C