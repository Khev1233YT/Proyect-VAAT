#include "IMU.h"

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <Config.h>
#include <Debug.h>

namespace
{
/***************************************************
 *              SENSOR
 ***************************************************/
Adafruit_MPU6050 mpu;

/***************************************************
 *              ESTADO INTERNO
 ***************************************************/
bool imuDisponible = false;
bool imuCalibrado = false;

/***************************************************
 *              VARIABLES DE GIROSCOPIO
 ***************************************************/
float velocidadZ = 0.0f;      // rad/s
float offsetZ = 0.0f;         // corrección

float angulo = 0.0f;          // grados

uint32_t ultimoTiempo = 0;

/***************************************************
 *              CONVERSIÓN
 ***************************************************/
constexpr float RAD_A_GRADOS = 57.2957795f;

/***************************************************
 *              LECTURA SENSOR
 ***************************************************/
bool leerSensor(float &gz)
{
    sensors_event_t acc;
    sensors_event_t gyro;
    sensors_event_t temp;

    mpu.getEvent(&acc, &gyro, &temp);

    gz = gyro.gyro.z;

    return true;
}

} // namespace


namespace IMU
{

/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/
void begin()
{
    Wire.begin(
        Config::Comunicacion::I2C::SDA,
        Config::Comunicacion::I2C::SCL);

    Wire.setClock(
        Config::Comunicacion::I2C::FRECUENCIA);

    if (!mpu.begin())
    {
        imuDisponible = false;

        Debug::println("ERROR: MPU6050 no detectado");

        return;
    }

    imuDisponible = true;

    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    angulo = 0.0f;
    velocidadZ = 0.0f;
    offsetZ = 0.0f;

    ultimoTiempo = millis();

    Debug::println("IMU inicializada");
}

/***************************************************
 *              ACTUALIZACIÓN
 ***************************************************/
void update()
{
    if (!imuDisponible)
        return;

    float gz;

    leerSensor(gz);

    // Aplicar offset
    gz -= offsetZ;

    // convertir a grados/s
    velocidadZ = gz * RAD_A_GRADOS;

    // tiempo delta
    uint32_t ahora = millis();
    float dt = (ahora - ultimoTiempo) / 1000.0f;
    ultimoTiempo = ahora;

    // integración (Z es el eje del robot)
    angulo += velocidadZ * dt;
}

/***************************************************
 *              CALIBRACIÓN
 ***************************************************/
bool calibrar()
{
    if (!imuDisponible)
        return false;

    Debug::println("Calibrando IMU...");

    const int muestras = 300;

    float suma = 0.0f;

    for (int i = 0; i < muestras; i++)
    {
        sensors_event_t acc;
        sensors_event_t gyro;
        sensors_event_t temp;

        mpu.getEvent(&acc, &gyro, &temp);

        suma += gyro.gyro.z;

        delay(5);
    }

    offsetZ = suma / muestras;

    imuCalibrado = true;

    Debug::println("Calibracion lista");

    return true;
}

/***************************************************
 *              RESET
 ***************************************************/
void reset()
{
    angulo = 0.0f;
}

/***************************************************
 *              GETTERS
 ***************************************************/
float getAngulo()
{
    return angulo;
}

float getVelocidadAngular()
{
    return velocidadZ;
}

float getOffset()
{
    return offsetZ;
}

bool disponible()
{
    return imuDisponible;
}

bool calibrado()
{
    return imuCalibrado;
}

} // namespace IMU