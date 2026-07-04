#ifndef SENSOR_NPK_H
#define SENSOR_NPK_H

#include <Arduino.h>

namespace SensorNPK
{

/***************************************************
 *              MUESTRA NPK
 ***************************************************/

/*
 * Información correspondiente a una
 * medición realizada por el sensor.
 */
struct Muestra
{
    /*
     * Nutrientes (mg/kg)
     */
    float nitrogeno;

    float fosforo;

    float potasio;

    /*
     * Variables del suelo.
     */
    float humedad;

    float temperatura;

    float conductividad;

    float ph;

    /*
     * Instante en que fue obtenida
     * la medición.
     */
    uint32_t tiempo;
};

/***************************************************
 *              ESTADOS
 ***************************************************/

/*
 * Estados internos de la máquina
 * de estados del sensor.
 */
enum class Estado : uint8_t
{
    /*
     * Sensor apagado.
     */
    APAGADO,

    /*
     * Esperando el tiempo de
     * estabilización después de
     * energizar el sensor.
     */
    ESPERANDO,

    /*
     * Comunicación Modbus en curso.
     */
    LEYENDO,

    /*
     * Lectura finalizada correctamente.
     */
    LISTO,

    /*
     * Error durante el proceso.
     */
    ERROR
};

/***************************************************
 *          INICIALIZACIÓN
 ***************************************************/

/*
 * Inicializa la biblioteca.
 */
bool begin();

/***************************************************
 *          ACTUALIZACIÓN
 ***************************************************/

/*
 * Debe ejecutarse periódicamente
 * desde loop().
 */
void update();

/***************************************************
 *          CONTROL DEL SENSOR
 ***************************************************/

/*
 * Inicia una nueva adquisición.
 *
 * El proceso completo es
 * completamente no bloqueante.
 *
 * Devuelve false si el sensor ya
 * está realizando otra lectura.
 */
bool iniciarMuestreo();

/*
 * Cancela el muestreo actual.
 */
void cancelar();

/*
 * Reinicia completamente la
 * biblioteca.
 */
void reset();

/***************************************************
 *              CONSULTAS
 ***************************************************/

/*
 * Indica si existe una muestra
 * válida disponible.
 */
bool muestraDisponible();

/*
 * Devuelve la última muestra
 * obtenida.
 */
const Muestra& obtenerMuestra();

/*
 * Estado actual del sensor.
 */
Estado estado();

/*
 * Indica si el sensor se encuentra
 * realizando una medición.
 */
bool ocupado();

/*
 * Indica si la última medición
 * terminó correctamente.
 */
bool finalizado();

/*
 * Indica si el sensor ha respondido
 * correctamente al menos una vez
 * desde la inicialización.
 */
bool disponible();

/*
 * Indica si ocurrió un error.
 */
bool error();

} // namespace SensorNPK

#endif