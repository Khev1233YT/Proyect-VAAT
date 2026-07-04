#ifndef I2C_H
#define I2C_H

#include <Arduino.h>

namespace I2C
{

/***************************************************
 *          DISPOSITIVOS REGISTRADOS
 ***************************************************/

/**
 * @brief Información de un dispositivo I2C.
 *
 * El I2C Manager mantiene un registro interno
 * de todos los dispositivos conocidos del robot.
 */
struct Dispositivo
{
    const char* nombre;

    uint8_t direccion;

    bool disponible;
};

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

/**
 * @brief Inicializa el bus I2C.
 *
 * Configura el bus utilizando los parámetros
 * definidos en Config.h.
 *
 * La inicialización solamente se realiza una vez,
 * independientemente del número de llamadas.
 *
 * Además registra todos los dispositivos
 * conocidos del robot.
 *
 * @return true si el bus quedó correctamente
 *         inicializado.
 */
bool begin();

/***************************************************
 *          ACTUALIZACIÓN DEL BUS
 ***************************************************/

/**
 * @brief Actualiza el estado de todos los
 * dispositivos registrados.
 *
 * Consulta nuevamente cada dirección I2C y
 * actualiza su disponibilidad.
 *
 * Puede utilizarse si se conecta o desconecta
 * un dispositivo durante la ejecución.
 */
void actualizar();

/***************************************************
 *              CONSULTAS
 ***************************************************/

/**
 * @brief Indica si el bus ya fue inicializado.
 *
 * @return true si begin() fue ejecutado.
 */
bool ready();

/**
 * @brief Consulta si un dispositivo está
 * disponible.
 *
 * La información proviene del registro interno,
 * por lo que NO genera tráfico adicional
 * sobre el bus I2C.
 *
 * @param direccion Dirección I2C.
 *
 * @return true si el dispositivo respondió
 * durante la última actualización.
 */
bool disponible(uint8_t direccion);

/**
 * @brief Devuelve la información registrada
 * de un dispositivo.
 *
 * @param direccion Dirección I2C.
 *
 * @return Puntero al registro o nullptr si no
 * existe.
 */
const Dispositivo* obtener(uint8_t direccion);

/***************************************************
 *              DEPURACIÓN
 ***************************************************/

/**
 * @brief Imprime el estado del bus I2C.
 *
 * Muestra todos los dispositivos registrados
 * indicando si están disponibles.
 */
void scan();

} // namespace I2C

#endif