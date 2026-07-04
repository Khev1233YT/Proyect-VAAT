#include "Debug.h"

#include <Config.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

namespace Debug
{

namespace
{

bool inicializado = false;

/***************************************************
 *      SEPARADOR INTERNO
 ***************************************************/

constexpr char LINEA[] =
"================================================";

}

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

void begin()
{
    if (!Config::Debug::HABILITADO)
    {
        return;
    }

    if (inicializado)
    {
        return;
    }

    Serial.begin(Config::Debug::BAUDRATE);

    inicializado = true;
}

/***************************************************
 *      NÚMEROS ENTEROS CON BASE
 ***************************************************/

void print(uint8_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.print(dato, base);
    }
}

void print(uint16_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.print(dato, base);
    }
}

void print(uint32_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.print(dato, base);
    }
}

void print(int8_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.print(dato, base);
    }
}

void print(int16_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.print(dato, base);
    }
}

void print(int32_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.print(dato, base);
    }
}

void println(uint8_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.println(dato, base);
    }
}

void println(uint16_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.println(dato, base);
    }
}

void println(uint32_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.println(dato, base);
    }
}

void println(int8_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.println(dato, base);
    }
}

void println(int16_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.println(dato, base);
    }
}

void println(int32_t dato, int base)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.println(dato, base);
    }
}

/***************************************************
 *          HEXADECIMAL
 ***************************************************/

void printHex(uint8_t dato)
{
    if (!Config::Debug::HABILITADO)
    {
        return;
    }

    Serial.print(F("0x"));

    if (dato < 0x10)
    {
        Serial.print('0');
    }

    Serial.print(dato, HEX);
}

void printlnHex(uint8_t dato)
{
    if (!Config::Debug::HABILITADO)
    {
        return;
    }

    printHex(dato);

    Serial.println();
}

/***************************************************
 *          NÚMEROS DECIMALES
 ***************************************************/

void print(float dato, uint8_t decimales)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.print(dato, decimales);
    }
}

void println(float dato, uint8_t decimales)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.println(dato, decimales);
    }
}

void print(double dato, uint8_t decimales)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.print(dato, decimales);
    }
}

void println(double dato, uint8_t decimales)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.println(dato, decimales);
    }
}

/***************************************************
 *              FORMATO
 ***************************************************/

void println()
{
    if (Config::Debug::HABILITADO)
    {
        Serial.println();
    }
}

void separador()
{
    if (!Config::Debug::HABILITADO)
    {
        return;
    }

    Serial.println(F(LINEA));
}

void titulo(const char* texto)
{
    if (!Config::Debug::HABILITADO)
    {
        return;
    }

    separador();

    constexpr uint8_t ANCHO = sizeof(LINEA) - 1;

    uint8_t longitud = strlen(texto);

    if (longitud >= ANCHO)
    {
        Serial.println(texto);

        separador();

        return;
    }

    uint8_t espacios = (ANCHO - longitud) / 2;

    for (uint8_t i = 0; i < espacios; i++)
    {
        Serial.print(' ');
    }

    Serial.println(texto);

    separador();
}

/***************************************************
 *                  PRINTF
 ***************************************************/

void printf(const char* formato, ...)
{
    if (!Config::Debug::HABILITADO)
    {
        return;
    }

    char buffer[256];

    va_list args;

    va_start(args, formato);

    vsnprintf(
        buffer,
        sizeof(buffer),
        formato,
        args);

    va_end(args);

    Serial.print(buffer);
}

} // namespace Debug