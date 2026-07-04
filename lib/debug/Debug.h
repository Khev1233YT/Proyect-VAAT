#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>
#include <Config.h>

namespace Debug
{

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

/**
 * @brief Inicializa el puerto serie para depuración.
 *
 * Si Config::Debug::HABILITADO es false,
 * esta función no realiza ninguna acción.
 */
void begin();

/***************************************************
 *          IMPRESIÓN GENÉRICA
 ***************************************************/

/**
 * @brief Imprime un dato sin salto de línea.
 */
template<typename T>
void print(const T& dato);

/**
 * @brief Imprime un dato con salto de línea.
 */
template<typename T>
void println(const T& dato);

/***************************************************
 *      NÚMEROS ENTEROS CON BASE
 *
 * Bases soportadas:
 *  DEC
 *  HEX
 *  BIN
 *  OCT
 ***************************************************/

void print(uint8_t dato, int base);
void print(uint16_t dato, int base);
void print(uint32_t dato, int base);

void print(int8_t dato, int base);
void print(int16_t dato, int base);
void print(int32_t dato, int base);

void println(uint8_t dato, int base);
void println(uint16_t dato, int base);
void println(uint32_t dato, int base);

void println(int8_t dato, int base);
void println(int16_t dato, int base);
void println(int32_t dato, int base);

/***************************************************
 *          HEXADECIMAL
 ***************************************************/

/**
 * @brief Imprime un número hexadecimal.
 *
 * Ejemplo:
 *
 * 0x00
 * 0x36
 * 0x68
 * 0xFF
 */
void printHex(uint8_t dato);

/**
 * @brief Imprime un número hexadecimal
 * con salto de línea.
 */
void printlnHex(uint8_t dato);

/***************************************************
 *          NÚMEROS DECIMALES
 ***************************************************/

/**
 * @brief Imprime un número decimal con
 * la cantidad de decimales indicada.
 */
void print(float dato, uint8_t decimales);

/**
 * @brief Imprime un número decimal con
 * salto de línea.
 */
void println(float dato, uint8_t decimales);

/**
 * @brief Versión para double.
 */
void print(double dato, uint8_t decimales);

/**
 * @brief Versión para double con salto
 * de línea.
 */
void println(double dato, uint8_t decimales);

/***************************************************
 *          FORMATO
 ***************************************************/

/**
 * @brief Imprime únicamente un salto
 * de línea.
 */
void println();

/**
 * @brief Imprime una línea separadora.
 *
 * ==================================================
 */
void separador();

/**
 * @brief Imprime un título entre dos
 * separadores.
 *
 * Ejemplo:
 *
 * ==================================================
 *                  ENCODER
 * ==================================================
 */
void titulo(const char* texto);

/***************************************************
 *              PRINTF
 ***************************************************/

/**
 * @brief Imprime texto utilizando el
 * formato tipo printf.
 */
void printf(const char* formato, ...);

} // namespace Debug

/***************************************************
 *      IMPLEMENTACIÓN DE PLANTILLAS
 ***************************************************/

template<typename T>
inline void Debug::print(const T& dato)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.print(dato);
    }
}

template<typename T>
inline void Debug::println(const T& dato)
{
    if (Config::Debug::HABILITADO)
    {
        Serial.println(dato);
    }
}

#endif