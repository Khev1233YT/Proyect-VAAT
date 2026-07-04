#include "Config.h"

namespace Config
{

namespace Mision
{

Terreno terreno =
{
    2.0f,   // ancho
    2.0f    // largo
};

void cargarValoresPorDefecto()
{
    terreno.ancho = 1.0f;
    terreno.largo = 1.0f;
}

} // namespace Mision

} // namespace Config