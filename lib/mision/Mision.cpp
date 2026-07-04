#include "Mision.h"

#include <math.h>

#include <Config.h>
#include <Debug.h>

#include <Movimiento.h>
#include <SensorNPK.h>

namespace
{

/***************************************************
 *          MATRIZ DEL TERRENO
 ***************************************************/

/*
 * Matriz dinámica de celdas.
 *
 * Se almacena como un único bloque continuo
 * de memoria.
 */
Mision::Celda* matriz = nullptr;

/*
 * Dimensiones del terreno.
 */
uint16_t totalFilas = 0;

uint16_t totalColumnas = 0;

/***************************************************
 *          ESTADO DE LA MISIÓN
 ***************************************************/

bool inicializado = false;

Mision::Estado estadoActual =
    Mision::Estado::DETENIDA;

/***************************************************
 *          POSICIÓN DEL ROBOT
 ***************************************************/

struct Coordenada
{
    uint16_t x;
    uint16_t y;
};

Coordenada posicionRobot =
{
    0,
    0
};

Coordenada siguienteCelda =
{
    0,
    0
};

Mision::Direccion direccionRobot =
    Mision::Direccion::ESTE;

Mision::Direccion direccionObjetivo =
    Mision::Direccion::ESTE;

/***************************************************
 *          CONTROL DE LA MISIÓN
 ***************************************************/

/*
 * Número de celdas ya muestreadas.
 */
uint32_t celdasCompletadas = 0;

/*
 * Acción pendiente que debe ejecutar
 * Movimiento antes de continuar.
 */
enum class AccionPendiente : uint8_t
{
    NINGUNA,

    GIRAR,

    AVANZAR
};

AccionPendiente accionPendiente =
    AccionPendiente::NINGUNA;

/***************************************************
 *          FUNCIONES PRIVADAS
 ***************************************************/

/*
 * Inicializa completamente una celda.
 */
void inicializarCelda(
    Mision::Celda& celda);

/*
 * Reserva memoria e inicializa la matriz.
 */
void inicializarMatriz();

/*
 * Libera la memoria dinámica.
 */
void liberarMatriz();

/*
 * Devuelve una celda de la matriz.
 */
Mision::Celda* obtenerCeldaInterna(
    uint16_t x,
    uint16_t y);

/*
 * Calcula la siguiente celda siguiendo
 * el recorrido tipo serpiente.
 */
void calcularSiguienteCelda();

/*
 * Calcula la dirección necesaria para
 * desplazarse entre dos celdas.
 */
Mision::Direccion calcularDireccion(
    const Coordenada& origen,
    const Coordenada& destino);

/*
 * Calcula el giro requerido.
 *
 * Retorna:
 *  90
 * -90
 * 180
 *   0
 */
float calcularGiro(
    Mision::Direccion origen,
    Mision::Direccion destino);

/*
 * Ordena al robot desplazarse hacia la
 * siguiente celda.
 */
void moverASiguienteCelda();

/*
 * Guarda la última muestra del sensor
 * dentro de la celda actual.
 */
void guardarMuestra();

/*
 * Máquina principal de estados.
 */
void actualizarFSM();
/***************************************************
 *      INICIALIZAR UNA CELDA
 ***************************************************/

void inicializarCelda(
    Mision::Celda& celda)
{
    celda.fila = 0;
    celda.columna = 0;

    celda.estado =
        Mision::EstadoCelda::PENDIENTE;

    celda.muestra.nitrogeno = 0.0f;
    celda.muestra.fosforo = 0.0f;
    celda.muestra.potasio = 0.0f;

    celda.muestra.humedad = 0.0f;
    celda.muestra.temperatura = 0.0f;
    celda.muestra.conductividad = 0.0f;
    celda.muestra.ph = 0.0f;

    celda.muestra.tiempo = 0;
}

/***************************************************
 *      LIBERAR MATRIZ
 ***************************************************/

void liberarMatriz()
{
    if (matriz != nullptr)
    {
        delete[] matriz;
        matriz = nullptr;
    }

    totalFilas = 0;
    totalColumnas = 0;

    celdasCompletadas = 0;
}

/***************************************************
 *      INICIALIZAR MATRIZ
 ***************************************************/

void inicializarMatriz()
{
    liberarMatriz();

    totalColumnas =
        static_cast<uint16_t>(
            ceil(Config::Mision::terreno.ancho));

    totalFilas =
        static_cast<uint16_t>(
            ceil(Config::Mision::terreno.largo));

    if (totalColumnas == 0)
    {
        totalColumnas = 1;
    }

    if (totalFilas == 0)
    {
        totalFilas = 1;
    }

    const uint32_t total =
        static_cast<uint32_t>(totalFilas) *
        static_cast<uint32_t>(totalColumnas);

    matriz = new Mision::Celda[total];

    for (uint16_t fila = 0;
         fila < totalFilas;
         fila++)
    {
        for (uint16_t columna = 0;
             columna < totalColumnas;
             columna++)
        {
            uint32_t indice =
                static_cast<uint32_t>(fila) *
                static_cast<uint32_t>(totalColumnas) +
                columna;

            inicializarCelda(
                matriz[indice]);

            matriz[indice].fila = fila;
            matriz[indice].columna = columna;
        }
    }

    posicionRobot =
    {
        0,
        0
    };

    siguienteCelda =
    {
        0,
        0
    };

    direccionRobot =
        Mision::Direccion::ESTE;

    direccionObjetivo =
        Mision::Direccion::ESTE;

    accionPendiente =
        AccionPendiente::NINGUNA;

    Debug::printf(
        "Terreno creado: %u x %u (%lu celdas)\n",
        totalColumnas,
        totalFilas,
        static_cast<unsigned long>(total));
}

/***************************************************
 *      ACCESO A UNA CELDA
 ***************************************************/

Mision::Celda* obtenerCeldaInterna(
    uint16_t x,
    uint16_t y)
{
    if (matriz == nullptr)
    {
        return nullptr;
    }

    if (x >= totalColumnas)
    {
        return nullptr;
    }

    if (y >= totalFilas)
    {
        return nullptr;
    }

    uint32_t indice =
        static_cast<uint32_t>(y) *
        static_cast<uint32_t>(totalColumnas) +
        x;

    return &matriz[indice];
}
/***************************************************
 *      CALCULAR DIRECCIÓN
 ***************************************************/

Mision::Direccion calcularDireccion(
    const Coordenada& origen,
    const Coordenada& destino)
{
    if (destino.x > origen.x)
    {
        return Mision::Direccion::ESTE;
    }

    if (destino.x < origen.x)
    {
        return Mision::Direccion::OESTE;
    }

    if (destino.y > origen.y)
    {
        return Mision::Direccion::SUR;
    }

    return Mision::Direccion::NORTE;
}

/***************************************************
 *      CALCULAR GIRO
 ***************************************************/

float calcularGiro(
    Mision::Direccion origen,
    Mision::Direccion destino)
{
    if (origen == destino)
    {
        return 0.0f;
    }

    switch (origen)
    {
        case Mision::Direccion::NORTE:

            switch (destino)
            {
                case Mision::Direccion::ESTE:
                    return -90.0f;

                case Mision::Direccion::OESTE:
                    return 90.0f;

                case Mision::Direccion::SUR:
                    return 180.0f;

                default:
                    return 0.0f;
            }

        case Mision::Direccion::ESTE:

            switch (destino)
            {
                case Mision::Direccion::SUR:
                    return -90.0f;

                case Mision::Direccion::NORTE:
                    return 90.0f;

                case Mision::Direccion::OESTE:
                    return 180.0f;

                default:
                    return 0.0f;
            }

        case Mision::Direccion::SUR:

            switch (destino)
            {
                case Mision::Direccion::OESTE:
                    return -90.0f;

                case Mision::Direccion::ESTE:
                    return 90.0f;

                case Mision::Direccion::NORTE:
                    return 180.0f;

                default:
                    return 0.0f;
            }

        case Mision::Direccion::OESTE:

            switch (destino)
            {
                case Mision::Direccion::NORTE:
                    return -90.0f;

                case Mision::Direccion::SUR:
                    return 90.0f;

                case Mision::Direccion::ESTE:
                    return 180.0f;

                default:
                    return 0.0f;
            }
    }

    return 0.0f;
}

/***************************************************
 *      CALCULAR SIGUIENTE CELDA
 ***************************************************/

void calcularSiguienteCelda()
{
    siguienteCelda = posicionRobot;

    /*
     * Filas pares:
     * recorrido hacia la derecha.
     */
    if ((posicionRobot.y % 2) == 0)
    {
        if (posicionRobot.x + 1 < totalColumnas)
        {
            siguienteCelda.x++;
        }
        else if (posicionRobot.y + 1 < totalFilas)
        {
            siguienteCelda.y++;
        }
    }
    /*
     * Filas impares:
     * recorrido hacia la izquierda.
     */
    else
    {
        if (posicionRobot.x > 0)
        {
            siguienteCelda.x--;
        }
        else if (posicionRobot.y + 1 < totalFilas)
        {
            siguienteCelda.y++;
        }
    }
}

/***************************************************
 *      MOVER A LA SIGUIENTE CELDA
 ***************************************************/

void moverASiguienteCelda()
{
    calcularSiguienteCelda();

    /*
     * No existe una siguiente celda.
     */
    if ((siguienteCelda.x == posicionRobot.x) &&
        (siguienteCelda.y == posicionRobot.y))
    {
        estadoActual =
            Mision::Estado::FINALIZADA;

        Debug::println(
            F("Mision finalizada."));

        return;
    }

    direccionObjetivo =
        calcularDireccion(
            posicionRobot,
            siguienteCelda);

    float giro =
        calcularGiro(
            direccionRobot,
            direccionObjetivo);

    if (fabs(giro) > 0.1f)
    {
        Movimiento::girar(giro);

        accionPendiente =
            AccionPendiente::GIRAR;
    }
    else
    {
        Movimiento::avanzar(
            Config::Mision::DISTANCIA_ENTRE_MUESTRAS);

        accionPendiente =
            AccionPendiente::AVANZAR;
    }

    estadoActual =
        Mision::Estado::MOVIENDO;
}
/***************************************************
 *      GUARDAR MUESTRA
 ***************************************************/

void guardarMuestra()
{
    Mision::Celda* celda =
        obtenerCeldaInterna(
            posicionRobot.x,
            posicionRobot.y);

    if (celda == nullptr)
    {
        estadoActual =
            Mision::Estado::ERROR;

        return;
    }

    celda->muestra =
        SensorNPK::obtenerMuestra();

    celda->estado =
        Mision::EstadoCelda::COMPLETADA;

    celdasCompletadas++;
}

/***************************************************
 *      MÁQUINA DE ESTADOS
 ***************************************************/

void actualizarFSM()
{
    if (Movimiento::error())
    {
        estadoActual =
            Mision::Estado::ERROR;

        return;
    }

    if (SensorNPK::error())
    {
        estadoActual =
            Mision::Estado::ERROR;

        return;
    }

    switch (estadoActual)
    {
        /*******************************************
         * DETENIDA
         *******************************************/
        case Mision::Estado::DETENIDA:
            return;

        /*******************************************
         * PLANIFICANDO
         *******************************************/
        case Mision::Estado::PLANIFICANDO:

            estadoActual =
                Mision::Estado::MUESTREANDO;

            SensorNPK::iniciarMuestreo();

            return;

        /*******************************************
         * MUESTREANDO
         *******************************************/
        case Mision::Estado::MUESTREANDO:

            if (!SensorNPK::finalizado())
            {
                return;
            }

            guardarMuestra();

            moverASiguienteCelda();

            return;

        /*******************************************
         * MOVIENDO
         *******************************************/
        case Mision::Estado::MOVIENDO:

            if (!Movimiento::finalizado())
            {
                return;
            }

            if (accionPendiente ==
                AccionPendiente::GIRAR)
            {
                direccionRobot =
                    direccionObjetivo;

                Movimiento::avanzar(
                    Config::Mision::DISTANCIA_ENTRE_MUESTRAS);

                accionPendiente =
                    AccionPendiente::AVANZAR;

                return;
            }

            posicionRobot =
                siguienteCelda;

            accionPendiente =
                AccionPendiente::NINGUNA;

            estadoActual =
                Mision::Estado::MUESTREANDO;

            SensorNPK::iniciarMuestreo();

            return;

        /*******************************************
         * FINALIZADA
         *******************************************/
        case Mision::Estado::FINALIZADA:
            return;

        /*******************************************
         * ERROR
         *******************************************/
        case Mision::Estado::ERROR:

            Movimiento::detener();

            return;
    }
}

/***************************************************
 *      FIN DEL NAMESPACE PRIVADO
 ***************************************************/

} // namespace
/***************************************************
 *              INICIALIZACIÓN
 ***************************************************/

bool Mision::begin()
{
    if (inicializado)
    {
        return true;
    }

    Debug::titulo("MISION");

    inicializarMatriz();

    estadoActual =
        Estado::DETENIDA;

    inicializado = true;

    Debug::println(
        F("Mision inicializada."));

    return true;
}

/***************************************************
 *              ACTUALIZACIÓN
 ***************************************************/

void Mision::update()
{
    if (!inicializado)
    {
        return;
    }

    SensorNPK::update();

    actualizarFSM();
}

/***************************************************
 *          CONTROL DE LA MISIÓN
 ***************************************************/

bool Mision::iniciar()
{
    if (!inicializado)
    {
        return false;
    }

    if (ocupada())
    {
        return false;
    }

    posicionRobot =
    {
        0,
        0
    };

    siguienteCelda =
    {
        0,
        0
    };

    direccionRobot =
        Direccion::ESTE;

    direccionObjetivo =
        Direccion::ESTE;

    accionPendiente =
        AccionPendiente::NINGUNA;

    celdasCompletadas = 0;

    estadoActual =
        Estado::PLANIFICANDO;

    Debug::println(
        F("Mision iniciada."));

    return true;
}

void Mision::detener()
{
    Movimiento::detener();

    estadoActual =
        Estado::DETENIDA;
}

void Mision::reset()
{
    detener();

    inicializarMatriz();

    estadoActual =
        Estado::DETENIDA;
}

/***************************************************
 *              CONSULTAS
 ***************************************************/

Mision::Estado Mision::estado()
{
    return estadoActual;
}

bool Mision::ocupada()
{
    switch (estadoActual)
    {
        case Estado::DETENIDA:
        case Estado::FINALIZADA:
        case Estado::ERROR:
            return false;

        default:
            return true;
    }
}

bool Mision::finalizada()
{
    return
        estadoActual ==
        Estado::FINALIZADA;
}

bool Mision::error()
{
    return
        estadoActual ==
        Estado::ERROR;
}

/***************************************************
 *      INFORMACIÓN DEL TERRENO
 ***************************************************/

uint16_t Mision::filas()
{
    return totalFilas;
}

uint16_t Mision::columnas()
{
    return totalColumnas;
}

uint32_t Mision::totalCeldas()
{
    return
        static_cast<uint32_t>(totalFilas) *
        static_cast<uint32_t>(totalColumnas);
}

uint32_t Mision::celdasRecorridas()
{
    return celdasCompletadas;
}

/***************************************************
 *              MATRIZ
 ***************************************************/

bool Mision::coordenadaValida(
    uint16_t x,
    uint16_t y)
{
    return
        (x < totalColumnas) &&
        (y < totalFilas);
}

Mision::Celda* Mision::obtenerCelda(
    uint16_t x,
    uint16_t y)
{
    return obtenerCeldaInterna(
        x,
        y);
}

/***************************************************
 *          POSICIÓN DEL ROBOT
 ***************************************************/

uint16_t Mision::posicionX()
{
    return posicionRobot.x;
}

uint16_t Mision::posicionY()
{
    return posicionRobot.y;
}

Mision::Direccion Mision::direccion()
{
    return direccionRobot;
}

/***************************************************
 *              MEMORIA
 ***************************************************/

void Mision::liberarMemoria()
{
    liberarMatriz();
}