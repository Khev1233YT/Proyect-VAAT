#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace Config
{

/***************************************************
 *                  COMUNICACIÓN
 ***************************************************/
namespace Comunicacion
{

    /************** I2C **************/
    namespace I2C
    {
        /*
         * Pines del bus I2C.
         */
        constexpr uint8_t SDA = 21;

        constexpr uint8_t SCL = 22;

        /*
         * Frecuencia de funcionamiento.
         */
        constexpr uint32_t FRECUENCIA = 400000;

        /*
         * Tiempo máximo de espera para una
         * comunicación I2C.
         */
        constexpr uint32_t TIMEOUT = 100;

        /*******************************************
         *      Direcciones de dispositivos I2C
         *******************************************/
        namespace Direcciones
        {
            constexpr uint8_t AS5600  = 0x36;

            constexpr uint8_t MPU6050 = 0x68;

            constexpr uint8_t INA219  = 0x40;

        } // namespace Direcciones

    } // namespace I2C

    /************** MODBUS **************/
    namespace Modbus
{
        constexpr uint32_t BAUDRATE = 9600;

        constexpr uint8_t DIRECCION_SENSOR = 0x01;

        constexpr uint8_t PIN_RX = 16;

        constexpr uint8_t PIN_TX = 17;

        /*
         * Tiempo máximo de espera de una respuesta.
         */
        constexpr uint16_t TIMEOUT = 1000;

        /*
         * Función Modbus utilizada por el sensor.
         */
        constexpr uint8_t FUNCION_LEER = 0x03;

        /*
         * Primer registro del sensor.
         */
        constexpr uint16_t REGISTRO_INICIAL = 0x0000;

        /*
        * Número de registros consecutivos.
        */
        constexpr uint16_t NUMERO_REGISTROS = 7;
} // namespace Modbus

} // namespace Comunicacion

/***************************************************
 *                  DEBUG
 ***************************************************/
namespace Debug
{
    /*
     * Activa o desactiva completamente el sistema
     * de depuración.
     *
     * Cuando está en false, el compilador elimina
     * automáticamente todo el código Debug.
     */
    constexpr bool HABILITADO = true;

    /*
     * Velocidad del puerto serie.
     */
    constexpr uint32_t BAUDRATE = 115200;

} // namespace Debug
/***************************************************
 *                  MOTORES
 ***************************************************/
namespace Motores
{

    /************** Pines **************/
    constexpr uint8_t PIN_IN1 = 25;

    constexpr uint8_t PIN_IN2 = 33;

    constexpr uint8_t PIN_PWM_IZQUIERDO = 26;

    constexpr uint8_t PIN_IN3 = 32;

    constexpr uint8_t PIN_IN4 = 14;

    constexpr uint8_t PIN_PWM_DERECHO = 13;

    /************** PWM **************/
    constexpr uint16_t PWM_FRECUENCIA = 1000;

    constexpr uint8_t PWM_RESOLUCION = 8;

    constexpr uint8_t PWM_MAX = 255;

    /*
     * PWM mínimo utilizado para iniciar el
     * movimiento de cada motor.
     *
     * Cada motor puede requerir un valor diferente
     * dependiendo del sentido de giro debido a
     * diferencias mecánicas, reductoras y a la
     * caída de tensión del puente H.
     *
     * Estos valores fueron obtenidos mediante
     * calibración experimental.
     */
    constexpr uint8_t PWM_MINIMO_IZQUIERDO_ADELANTE = 0;

    constexpr uint8_t PWM_MINIMO_IZQUIERDO_ATRAS = 0;

    constexpr uint8_t PWM_MINIMO_DERECHO_ADELANTE = 220;

    constexpr uint8_t PWM_MINIMO_DERECHO_ATRAS = 220;

    /************** Velocidades **************/

    /*
     * Velocidades nominales del robot.
     *
     * La biblioteca Motores se encargará de
     * compensar automáticamente las diferencias
     * entre ambos motores.
     */
    constexpr uint8_t VELOCIDAD_AVANCE = 240;

    constexpr uint8_t VELOCIDAD_GIRO = 240;

    /************** Rampas **************/

    /*
     * Parámetros de aceleración y frenado.
     *
     * Una rampa suave reduce los picos de corriente,
     * protege la transmisión y mejora la estabilidad
     * del robot.
     */
    constexpr uint16_t PERIODO_RAMPA = 15;

    constexpr uint8_t RAMPA_ACELERACION = 4;

    constexpr uint8_t RAMPA_FRENADO = 8;

} // namespace Motores

/***************************************************
 *              ENCODER AS5600
 ***************************************************/
namespace Encoder
{

    /*
     * Resolución del AS5600.
     */
    constexpr uint16_t RESOLUCION = 4096;

    /*
     * Diámetro efectivo de la rueda motriz con
     * la oruga instalada.
     *
     * Valor medido físicamente sobre el robot.
     */
    constexpr float DIAMETRO_RUEDA = 0.055f;

    /*
     * Perímetro efectivo utilizado para convertir
     * el desplazamiento angular en distancia lineal.
     */
    constexpr float PERIMETRO_RUEDA =
        PI * DIAMETRO_RUEDA;

    /*
     * Cambio mínimo para considerar que la rueda
     * realmente se movió.
     *
     * Permite eliminar pequeñas variaciones
     * producidas por el ruido del sensor.
     */
    constexpr float ANGULO_MINIMO = 0.20f;

    /*
     * Tamaño del filtro de media móvil utilizado
     * para suavizar las mediciones.
     */
    constexpr uint8_t MEDIA_MOVIL = 5;

} // namespace Encoder
/***************************************************
 *              ESTADOS DEL SISTEMA
 ***************************************************/

/*
 * Estados posibles del sistema de energía.
 *
 * Esta enumeración es utilizada por las bibliotecas:
 *
 * - Energia
 * - Movimiento
 * - Mision
 * - Debug
 *
 * De esta forma todas interpretan exactamente
 * el mismo estado del robot.
 */
enum class EstadoEnergia : uint8_t
{
    NORMAL,

    BATERIA_BAJA,

    BATERIA_CRITICA,

    ESPERANDO_PARADA,

    CARGANDO,

    CARGA_COMPLETA,

    ERROR_SENSOR

};

/***************************************************
 *                  IMU MPU6050
 ***************************************************/
namespace IMU
{

    /*
     * Error máximo permitido para considerar
     * que el robot alcanzó el ángulo objetivo.
     */
    constexpr float TOLERANCIA_GIRO = 2.0f;

    /*
     * Error máximo permitido para mantener
     * el rumbo durante un desplazamiento.
     */
    constexpr float TOLERANCIA_RUMBO = 1.5f;

    /*
     * Número de muestras utilizadas para
     * calcular el offset del giroscopio.
     */
    constexpr uint16_t MUESTRAS_CALIBRACION = 500;

    /*
     * Desviación estándar máxima permitida
     * durante la calibración.
     */
    constexpr float DESVIACION_MAXIMA = 0.02f;

} // namespace IMU

/***************************************************
 *                  ENERGÍA
 *
 * Configuración del sistema de gestión energética
 * del robot.
 *
 * La biblioteca Energia será responsable de:
 *
 * - Lectura del INA219
 * - Voltaje
 * - Corriente
 * - Potencia
 * - Estado de la batería
 * - Alarmas
 * - Protección
 ***************************************************/
namespace Energia
{

    /************** INA219 **************/

    /*
     * Voltaje máximo soportado por el INA219.
     */
    constexpr float TENSION_MAXIMA_MEDIBLE = 26.0f;

    /*
     * Corriente máxima esperada del robot.
     */
    constexpr float CORRIENTE_MAXIMA = 5.0f;

    /************** Batería **************/

    /*
     * Valores característicos de la batería
     * Li-Ion 3S utilizada por el VAAT.
     */
    constexpr float BATERIA_LLENA = 12.60f;

    constexpr float BATERIA_NOMINAL = 11.10f;

    constexpr float BATERIA_BAJA = 10.20f;

    constexpr float BATERIA_CRITICA = 9.60f;

    constexpr float BATERIA_MINIMA = 9.00f;

    /************** Filtro **************/

    /*
     * Activa el filtro de media móvil.
     */
    constexpr bool USAR_FILTRO = true;

    /*
     * Número de muestras utilizadas por
     * la media móvil.
     */
    constexpr uint8_t MEDIA_MOVIL = 10;

    /*
     * Variación mínima para considerar
     * válida una nueva medición.
     */
    constexpr float ERROR_MEDICION = 0.05f;

    /************** Protección **************/

    /*
     * Histéresis aplicada a las transiciones
     * entre estados de batería.
     */
    constexpr float HISTERESIS = 0.20f;

    /*
     * Tiempo durante el cual la batería debe
     * permanecer en estado crítico antes de
     * solicitar una parada segura.
     */
    constexpr uint32_t RETARDO_BATERIA_CRITICA = 5000;

} // namespace Energia

/***************************************************
 *                  SERVO
 ***************************************************/
namespace Servo
{

    /*
     * Servo de rotación continua encargado de
     * elevar y descender el mecanismo de muestreo.
     */
    constexpr uint8_t PIN = 23;

    /*
     * Finales de carrera.
     */
    constexpr uint8_t PIN_FINAL_SUPERIOR = 18;

    constexpr uint8_t PIN_FINAL_INFERIOR = 19;

    /*
     * Indica si los finales de carrera son
     * activos en nivel bajo.
     */
    constexpr bool FINAL_ACTIVO_BAJO = true;

    /************** Control del servo **************/

    /*
     * Valor PWM que mantiene el servo detenido.
     */
    constexpr uint8_t DETENER = 90;

    /*
     * Velocidad máxima para elevar el mecanismo.
     *
     * Valores menores a DETENER producen el
     * movimiento de subida.
     */
    constexpr uint8_t SUBIR = 180;

    /*
     * Velocidad máxima para descender el mecanismo.
     *
     * Valores mayores a DETENER producen el
     * movimiento de bajada.
     */
    constexpr uint8_t BAJAR = 0;

    /************** Rampa **************/

    /*
     * Incremento aplicado en cada actualización
     * para acelerar o desacelerar el servo.
     */
    constexpr uint8_t RAMPA_ACELERACION = 3;

    constexpr uint8_t RAMPA_FRENADO = 5;

} // namespace Servo

/***************************************************
 *                  SENSOR NPK
 ***************************************************/
namespace Sensor
{

    /*
     * Tiempo necesario para obtener una
     * lectura estable después de energizar
     * el sensor.
     */
    constexpr uint32_t TIEMPO_MEDICION = 15000;

    constexpr uint8_t REINTENTOS = 3;

} // namespace Sensor
/***************************************************
 *                  MOVIMIENTO
 ***************************************************/
namespace Movimiento
{

    /*
     * Distancia entre el centro de ambas orugas.
     *
     * Valor medido físicamente sobre el robot.
     * Se utiliza para calcular la cinemática
     * de los giros.
     */
    constexpr float DISTANCIA_ENTRE_ORUGAS = 0.202f;

    /*
     * Compensación fija entre ambos motores.
     *
     * Valor positivo:
     *     aumenta la velocidad del motor derecho.
     *
     * Valor negativo:
     *     aumenta la velocidad del motor izquierdo.
     *
     * Se obtiene mediante calibración
     * experimental.
     */
    constexpr int16_t OFFSET_DIRECCION = 0;

    /*
     * Ganancia proporcional utilizada para
     * corregir el rumbo mediante la IMU.
     */
    constexpr float KP_RUMBO = 2.0f;

    /*
     * Corrección máxima permitida sobre el PWM.
     */
    constexpr uint8_t CORRECCION_MAXIMA = 30;

} // namespace Movimiento

/***************************************************
 *              PERIODOS DE ACTUALIZACIÓN
 ***************************************************/
namespace Update
{

    /*
     * Periodo base del sistema.
     *
     * La mayoría de las bibliotecas se ejecutan
     * tomando este valor como referencia.
     */
    constexpr uint16_t GENERAL = 10;

    constexpr uint16_t MOTORES = 10;

    constexpr uint16_t ENCODER = 10;

    constexpr uint16_t IMU = 10;

    constexpr uint16_t ENERGIA = 100;

    constexpr uint16_t MOVIMIENTO = 10;

    constexpr uint16_t SERVO = 10;

    /*
     * El sensor NPK necesita un periodo mucho
     * mayor debido al tiempo de estabilización
     * de las mediciones.
     */
    constexpr uint16_t SENSOR = 100;

} // namespace Update

/***************************************************
 *                  ROBOT
 ***************************************************/
namespace Robot
{

    /*
     * Nombre del proyecto.
     */
    constexpr char NOMBRE[] = "VAAT";

    /*
     * Versión actual del software.
     */
    constexpr char VERSION[] = "3.0.0";

    /*
     * Periodo principal del controlador.
     */
    constexpr uint16_t PERIODO_CONTROL = 10;

} // namespace Robot

/***************************************************
 *                  TIMEOUTS
 ***************************************************/
namespace Timeout
{

    /*
     * Tiempo máximo permitido para recorrer
     * un tramo de avance.
     */
    constexpr uint32_t AVANCE = 10000;

    /*
     * Tiempo máximo permitido para realizar
     * un giro.
     */
    constexpr uint32_t GIRO = 5000;

    /*
     * Tiempo máximo permitido para que el
     * servo alcance su posición objetivo.
     */
    constexpr uint32_t SERVO = 5000;

    constexpr uint32_t SENSOR = 3000;

} // namespace Timeout
/***************************************************
 *                  MISIÓN
 *
 * El operador únicamente introduce el ancho
 * y el largo del terreno.
 *
 * El recorrido completo será calculado
 * automáticamente por el sistema.
 ***************************************************/
namespace Mision
{

    /*
     * Dimensiones del terreno.
     */
    struct Terreno
    {
        float ancho;

        float largo;
    };

    /*
     * Información del terreno actualmente
     * cargado.
     */
    extern Terreno terreno;

    /*
     * Separación fija entre puntos de muestreo.
     */
    constexpr float DISTANCIA_ENTRE_MUESTRAS = 1.0f;

    /*
     * Carga los valores por defecto del terreno.
     */
    void cargarValoresPorDefecto();

} // namespace Mision

/***************************************************
 *                    RELÉS
 ***************************************************/
namespace Rele
{

    /*
     * Relé principal de seguridad.
     *
     * Cuando la batería alcanza un nivel crítico,
     * este relé desconecta la alimentación de:
     *
     * - Motores
     * - Sensor NPK
     *
     * Protege la batería contra una descarga
     * excesiva.
     */
    constexpr uint8_t PIN_CORTE_BATERIA = 27;

    /*
     * Relé de alimentación del sensor NPK.
     *
     * Permite energizar el sensor únicamente
     * durante la toma de muestras para reducir
     * el consumo energético.
     */
    constexpr uint8_t PIN_ALIMENTACION_SENSOR = 4;

    /*
     * Estado lógico que activa los relés.
     *
     * Permite utilizar módulos activos en HIGH
     * o activos en LOW sin modificar la
     * biblioteca.
     */
    constexpr bool ACTIVO_ALTO = false;

} // namespace Rele

} // namespace Config

#endif // CONFIG_H