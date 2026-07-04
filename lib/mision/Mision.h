#ifndef MISION_H
#define MISION_H

#include <Arduino.h>

#include <SensorNPK.h>

namespace Mision
{

/***************************************************
 *          ESTADO DE UNA CELDA
 ***************************************************/

enum class EstadoCelda : uint8_t
{
    PENDIENTE,

    COMPLETADA,

    ERROR
};

/***************************************************
 *          INFORMACIÓN DE UNA CELDA
 ***************************************************/

struct Celda
{
    uint16_t fila;

    uint16_t columna;

    SensorNPK::Muestra muestra;

    EstadoCelda estado;
};

/***************************************************
 *          DIRECCIÓN DEL ROBOT
 ***************************************************/

enum class Direccion : uint8_t
{
    NORTE,

    ESTE,

    SUR,

    OESTE
};

/***************************************************
 *          ESTADOS DE LA MISIÓN
 ***************************************************/

enum class Estado : uint8_t
{
    DETENIDA,

    PLANIFICANDO,

    SIGUIENTE_CELDA,

    MOVIENDO,

    MUESTREANDO,

    FINALIZADA,

    ERROR
};

/***************************************************
 *          INICIALIZACIÓN
 ***************************************************/

/**
 * Inicializa la biblioteca.
 *
 * Reserva la memoria para la matriz
 * del terreno utilizando el tamaño
 * configurado en Config.
 */
bool begin();

/***************************************************
 *          ACTUALIZACIÓN
 ***************************************************/

/**
 * Debe ejecutarse continuamente desde loop().
 */
void update();

/***************************************************
 *          CONTROL DE LA MISIÓN
 ***************************************************/

/**
 * Inicia una misión completa.
 *
 * Retorna false si ya existe una
 * misión en ejecución.
 */
bool iniciar();

/**
 * Detiene inmediatamente la misión.
 */
void detener();

/**
 * Reinicia completamente el estado
 * interno de la biblioteca.
 */
void reset();

/***************************************************
 *          CONSULTAS
 ***************************************************/

Estado estado();

/**
 * Indica si la misión está ejecutándose.
 */
bool ocupada();

/**
 * Indica si la misión terminó.
 */
bool finalizada();

/**
 * Indica si ocurrió un error.
 */
bool error();

/***************************************************
 *          INFORMACIÓN DEL TERRENO
 ***************************************************/

uint16_t filas();

uint16_t columnas();

/**
 * Número total de celdas.
 */
uint32_t totalCeldas();

/**
 * Número de celdas ya muestreadas.
 */
uint32_t celdasRecorridas();

/***************************************************
 *          MATRIZ
 ***************************************************/

/**
 * Comprueba si una coordenada
 * pertenece al terreno.
 */
bool coordenadaValida(
    uint16_t x,
    uint16_t y);

/**
 * Devuelve la celda solicitada.
 *
 * nullptr si las coordenadas
 * son inválidas.
 */
Celda* obtenerCelda(
    uint16_t x,
    uint16_t y);

/***************************************************
 *          POSICIÓN DEL ROBOT
 ***************************************************/

uint16_t posicionX();

uint16_t posicionY();

Direccion direccion();

/***************************************************
 *          MEMORIA
 ***************************************************/

/**
 * Libera la memoria utilizada por
 * la matriz.
 *
 * Normalmente sólo se ejecuta si
 * se desea cambiar el tamaño del
 * terreno.
 */
void liberarMemoria();

} // namespace Mision

#endif