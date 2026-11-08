
#ifndef RESTRICCIONES_H
#define RESTRICCIONES_H

#include "estructuras.h"

// H1: Lectures
bool validarH1(const Instancia& instancia, const Solucion& solucion);

// H2: Room Occupancy
bool validarH2(const Instancia& instancia, const Solucion& solucion);

// H3: Conflict
bool validarH3(const Instancia& instancia, const Solucion& solucion);

// H4: Availability
bool validarH4(const Instancia& instancia, const Solucion& solucion);

//Validacion de todas
bool validarTodas(const Instancia& instancia, const Solucion& solucion);

#endif
