#include "SensorNPK.h"

#include <HardwareSerial.h>

#include <Config.h>
#include <Debug.h>

namespace
{

/***************************************************
 *              UART RS485
 ***************************************************/

/*
 * UART2 utilizada para la comunicación
 * con el módulo XY-K485.
 */
HardwareSerial puertoRS485(2);

/***************************************************
 *          ESTADO INTERNO
 ***************************************************/

bool inicializado = false;

/*
 * Indica si el sensor respondió
 * correctamente al menos una vez.
 */
bool sensorDisponible = false;

/*
 * Indica si existe una muestra válida.
 */
bool hayMuestra = false;

/***************************************************
 *          MÁQUINA DE ESTADOS
 ***************************************************/

SensorNPK::Estado estadoActual =
    SensorNPK::Estado::APAGADO;

/***************************************************
 *          TEMPORIZADORES
 ***************************************************/

uint32_t tiempoEstado = 0;

uint32_t ultimaActualizacion = 0;

/***************************************************
 *          CONTROL MODBUS
 ***************************************************/

uint8_t reintentos = 0;

/***************************************************
 *          ÚLTIMA MUESTRA
 ***************************************************/

SensorNPK::Muestra muestraActual;

/***************************************************
 *          BUFFERS MODBUS
 ***************************************************/

uint8_t tramaTX[8];

uint8_t tramaRX[32];

/***************************************************
 *      FUNCIONES PRIVADAS
 ***************************************************/

void limpiarMuestra();

void cambiarEstado(
    SensorNPK::Estado nuevoEstado);

bool sensorListo();

void activarRele();

void desactivarRele();

bool validarMuestra();

uint16_t calcularCRC(
    const uint8_t* datos,
    uint16_t longitud);

bool validarCRC(
    const uint8_t* datos,
    uint16_t longitud);

void construirTrama();

bool enviarPeticion();

bool recibirRespuesta();

bool procesarRespuesta();

void actualizarFSM();

/***************************************************
 *      NIVEL LÓGICO DEL RELÉ
 ***************************************************/

constexpr uint8_t nivelActivo()
{
    return
        Config::Rele::ACTIVO_ALTO ?
        HIGH :
        LOW;
}

constexpr uint8_t nivelInactivo()
{
    return
        Config::Rele::ACTIVO_ALTO ?
        LOW :
        HIGH;
}

/***************************************************
 *          LIMPIAR MUESTRA
 ***************************************************/

void limpiarMuestra()
{
    muestraActual.nitrogeno = 0.0f;
    muestraActual.fosforo = 0.0f;
    muestraActual.potasio = 0.0f;

    muestraActual.humedad = 0.0f;
    muestraActual.temperatura = 0.0f;
    muestraActual.conductividad = 0.0f;
    muestraActual.ph = 0.0f;

    muestraActual.tiempo = 0;

    hayMuestra = false;
}

/***************************************************
 *          CAMBIO DE ESTADO
 ***************************************************/

void cambiarEstado(
    SensorNPK::Estado nuevoEstado)
{
    estadoActual = nuevoEstado;

    tiempoEstado = millis();

    switch (nuevoEstado)
    {
        case SensorNPK::Estado::APAGADO:
            Debug::println(F("SensorNPK -> APAGADO"));
            break;

        case SensorNPK::Estado::ESPERANDO:
            Debug::println(F("SensorNPK -> ESPERANDO"));
            break;

        case SensorNPK::Estado::LEYENDO:
            Debug::println(F("SensorNPK -> LEYENDO"));
            break;

        case SensorNPK::Estado::LISTO:
            Debug::println(F("SensorNPK -> LISTO"));
            break;

        case SensorNPK::Estado::ERROR:
            Debug::println(F("SensorNPK -> ERROR"));
            break;
    }
}

/***************************************************
 *      SENSOR ESTABILIZADO
 ***************************************************/

bool sensorListo()
{
    return
        millis() - tiempoEstado >=
        Config::Sensor::TIEMPO_MEDICION;
}

/***************************************************
 *          CONTROL DEL RELÉ
 ***************************************************/

void activarRele()
{
    digitalWrite(
        Config::Rele::PIN_ALIMENTACION_SENSOR,
        nivelActivo());
}

void desactivarRele()
{
    digitalWrite(
        Config::Rele::PIN_ALIMENTACION_SENSOR,
        nivelInactivo());
}
/***************************************************
 *          VALIDAR MUESTRA
 ***************************************************/

bool validarMuestra()
{
    if (muestraActual.humedad < 0.0f ||
        muestraActual.humedad > 100.0f)
    {
        return false;
    }

    if (muestraActual.temperatura < -40.0f ||
        muestraActual.temperatura > 80.0f)
    {
        return false;
    }

    if (muestraActual.conductividad < 0.0f ||
        muestraActual.conductividad > 10000.0f)
    {
        return false;
    }

    if (muestraActual.ph < 3.0f ||
        muestraActual.ph > 10.0f)
    {
        return false;
    }

    if (muestraActual.nitrogeno < 0.0f ||
        muestraActual.nitrogeno > 1999.0f)
    {
        return false;
    }

    if (muestraActual.fosforo < 0.0f ||
        muestraActual.fosforo > 1999.0f)
    {
        return false;
    }

    if (muestraActual.potasio < 0.0f ||
        muestraActual.potasio > 1999.0f)
    {
        return false;
    }

    return true;
}

/***************************************************
 *          CRC16 MODBUS
 ***************************************************/

uint16_t calcularCRC(
    const uint8_t* datos,
    uint16_t longitud)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < longitud; i++)
    {
        crc ^= datos[i];

        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return crc;
}

/***************************************************
 *          VALIDAR CRC
 ***************************************************/

bool validarCRC(
    const uint8_t* datos,
    uint16_t longitud)
{
    if (longitud < 2)
    {
        return false;
    }

    uint16_t crcCalculado =
        calcularCRC(
            datos,
            longitud - 2);

    uint16_t crcRecibido =
        datos[longitud - 2] |
        (datos[longitud - 1] << 8);

    return
        crcCalculado == crcRecibido;
}

/***************************************************
 *      CONSTRUIR TRAMA MODBUS
 ***************************************************/

void construirTrama()
{
    tramaTX[0] =
        Config::Comunicacion::Modbus::DIRECCION_SENSOR;

    /*
     * Función 03:
     * Leer Holding Registers.
     */
    tramaTX[1] = 0x03;

    /*
     * Registro inicial = 0x0000
     */
    tramaTX[2] = 0x00;
    tramaTX[3] = 0x00;

    /*
     * Leer 7 registros.
     */
    tramaTX[4] = 0x00;
    tramaTX[5] = 0x07;

    uint16_t crc =
        calcularCRC(
            tramaTX,
            6);

    tramaTX[6] = lowByte(crc);
    tramaTX[7] = highByte(crc);
}

/***************************************************
 *          ENVIAR PETICIÓN
 ***************************************************/

bool enviarPeticion()
{
    while (puertoRS485.available())
    {
        puertoRS485.read();
    }

    puertoRS485.write(
        tramaTX,
        sizeof(tramaTX));

    puertoRS485.flush();

    return true;
}

/***************************************************
 *          RECIBIR RESPUESTA
 ***************************************************/

bool recibirRespuesta()
{
    constexpr uint8_t LONGITUD_RESPUESTA = 19;

    uint32_t inicio = millis();

    uint8_t indice = 0;

    while (millis() - inicio <
           Config::Timeout::SENSOR)
    {
        while (puertoRS485.available())
        {
            if (indice >= sizeof(tramaRX))
            {
                return false;
            }

            tramaRX[indice++] =
                puertoRS485.read();
        }

        if (indice >= LONGITUD_RESPUESTA)
        {
            return true;
        }

        delay(1);
    }

    return false;
}

/***************************************************
 *      PROCESAR RESPUESTA
 ***************************************************/

bool procesarRespuesta()
{
    constexpr uint8_t LONGITUD_RESPUESTA = 19;

    if (!validarCRC(
            tramaRX,
            LONGITUD_RESPUESTA))
    {
        return false;
    }

    muestraActual.humedad =
        (((uint16_t)tramaRX[3] << 8) |
         tramaRX[4]) / 10.0f;

    int16_t temperatura =
        (((int16_t)tramaRX[5] << 8) |
         tramaRX[6]);

    muestraActual.temperatura =
        temperatura / 10.0f;

    muestraActual.conductividad =
        ((uint16_t)tramaRX[7] << 8) |
         tramaRX[8];

    muestraActual.ph =
        (((uint16_t)tramaRX[9] << 8) |
         tramaRX[10]) / 10.0f;

    muestraActual.nitrogeno =
        ((uint16_t)tramaRX[11] << 8) |
         tramaRX[12];

    muestraActual.fosforo =
        ((uint16_t)tramaRX[13] << 8) |
         tramaRX[14];

    muestraActual.potasio =
        ((uint16_t)tramaRX[15] << 8) |
         tramaRX[16];

    muestraActual.tiempo =
        millis();

    if (!validarMuestra())
    {
        return false;
    }

    hayMuestra = true;

    sensorDisponible = true;

    ultimaActualizacion =
        muestraActual.tiempo;

    return true;
}
/***************************************************
 *          MÁQUINA DE ESTADOS
 ***************************************************/

void actualizarFSM()
{
    switch (estadoActual)
    {
        /*******************************************
         * APAGADO
         *******************************************/
        case SensorNPK::Estado::APAGADO:
        {
            break;
        }

        /*******************************************
         * ESPERANDO
         *******************************************/
        case SensorNPK::Estado::ESPERANDO:
        {
            if (!sensorListo())
            {
                break;
            }

            construirTrama();

            cambiarEstado(
                SensorNPK::Estado::LEYENDO);

            break;
        }

        /*******************************************
         * LEYENDO
         *******************************************/
        case SensorNPK::Estado::LEYENDO:
        {
            if (!enviarPeticion())
            {
                reintentos++;

                if (reintentos >=
                    Config::Sensor::REINTENTOS)
                {
                    desactivarRele();

                    cambiarEstado(
                        SensorNPK::Estado::ERROR);
                }

                break;
            }

            if (!recibirRespuesta())
            {
                reintentos++;

                if (reintentos >=
                    Config::Sensor::REINTENTOS)
                {
                    desactivarRele();

                    cambiarEstado(
                        SensorNPK::Estado::ERROR);
                }

                break;
            }

            if (!procesarRespuesta())
            {
                reintentos++;

                if (reintentos >=
                    Config::Sensor::REINTENTOS)
                {
                    desactivarRele();

                    cambiarEstado(
                        SensorNPK::Estado::ERROR);
                }

                break;
            }

            desactivarRele();

            cambiarEstado(
                SensorNPK::Estado::LISTO);

            break;
        }

        /*******************************************
         * LISTO
         *******************************************/
        case SensorNPK::Estado::LISTO:
        {
            break;
        }

        /*******************************************
         * ERROR
         *******************************************/
        case SensorNPK::Estado::ERROR:
        {
            desactivarRele();

            break;
        }
    }
}
} // namespace anónimo

namespace SensorNPK
{

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

bool begin()
{
    pinMode(
        Config::Rele::PIN_ALIMENTACION_SENSOR,
        OUTPUT);

    desactivarRele();

    puertoRS485.begin(
        Config::Comunicacion::Modbus::BAUDRATE,
        SERIAL_8N1,
        Config::Comunicacion::Modbus::PIN_RX,
        Config::Comunicacion::Modbus::PIN_TX);

    limpiarMuestra();

    reintentos = 0;

    sensorDisponible = false;

    hayMuestra = false;

    estadoActual = Estado::APAGADO;

    inicializado = true;

    return true;
}

/***************************************************
 *              ACTUALIZACIÓN
 ***************************************************/

void update()
{
    if (!inicializado)
    {
        return;
    }

    actualizarFSM();
}

/***************************************************
 *          INICIAR MUESTREO
 ***************************************************/

bool iniciarMuestreo()
{
    if (!inicializado)
    {
        return false;
    }

    if (ocupado())
    {
        return false;
    }

    limpiarMuestra();

    reintentos = 0;

    activarRele();

    cambiarEstado(
        Estado::ESPERANDO);

    return true;
}

/***************************************************
 *              CANCELAR
 ***************************************************/

void cancelar()
{
    desactivarRele();

    limpiarMuestra();

    cambiarEstado(
        Estado::APAGADO);
}

/***************************************************
 *                  RESET
 ***************************************************/

void reset()
{
    cancelar();

    sensorDisponible = false;

    reintentos = 0;

    ultimaActualizacion = 0;
}
/***************************************************
 *              CONSULTAS
 ***************************************************/

bool muestraDisponible()
{
    return hayMuestra;
}

const Muestra& obtenerMuestra()
{
    return muestraActual;
}

Estado estado()
{
    return estadoActual;
}

bool ocupado()
{
    return
        estadoActual == Estado::ESPERANDO ||
        estadoActual == Estado::LEYENDO;
}

bool finalizado()
{
    return
        estadoActual == Estado::LISTO;
}

bool disponible()
{
    return sensorDisponible;
}

bool error()
{
    return
        estadoActual == Estado::ERROR;
}

} // namespace SensorNPK