#ifndef HC_H
#define HC_H

#include "estructuras.h"
#include "restricciones.h"
#include <random>

enum class TipoMovimiento {
    NINGUNO,
    TIME_MOVE,
    ROOM_MOVE,
    LECTURE_MOVE
};

struct Movimiento {
    TipoMovimiento tipo = TipoMovimiento::NINGUNO;

    int cursoA = -1;
    int lecA = -1;
    int cursoB = -1;
    int lecB = -1;

    int nuevoDiaA = -1;
    int nuevoPeriodoA = -1;
    int nuevaSalaIdxA = -1;

    int nuevoDiaB = -1;
    int nuevoPeriodoB = -1;
    int nuevaSalaIdxB = -1;

    int deltaEval = 0;
};

void aplicarMovimiento(
    const Instancia& instancia,
    EstadoRapido& estado,
    Solucion& sol,
    const Movimiento& mov
);

Movimiento buscarMejorVecino(
    const Instancia& instancia,
    EstadoRapido& estado,
    const Solucion& solActual
);

Solucion ejecutarHillClimbingMM(
    const Instancia& instancia,
    Solucion solActual,
    EstadoRapido& estado,
    int maxIteraciones
);

Solucion aplicarRestart(
    const Instancia& instancia,
    EstadoRapido& estado,
    const Solucion& mejorGlobal,
    const Solucion& solActual,
    std::mt19937& rng,
    int indiceRestart
);

Solucion orquestarHCMMConRestart(
    const Instancia& instancia,
    Solucion solucionInicial,
    EstadoRapido& estado,
    int maxRestarts,
    int maxIteracionesPorHC,
    unsigned int semillaBase
);

#endif