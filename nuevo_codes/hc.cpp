#include "hc.h"
#include "estructuras.h"
#include "restricciones.h"
#include "greedy.h"
#include "f_eval.h"


#include <random>
#include <iostream>

namespace {

int obtenerIndiceSala(const Instancia& instancia, const std::string& salaId) {
    for (int r = 0; r < static_cast<int>(instancia.salas.size()); ++r) {
        if (instancia.salas[r].id == salaId) return r;
    }
    return -1;
}

void evaluarTimeMoves(
    const Instancia& instancia,
    EstadoRapido& estado,
    const Solucion& solActual,
    Movimiento& mejor) {
    for (int c = 0; c < static_cast<int>(solActual.cursos.size()); ++c) {
        for (int l = 0; l < static_cast<int>(solActual.cursos[c].size()); ++l) {
            const auto& asig = solActual.cursos[c][l];
            int salaActualIdx = obtenerIndiceSala(instancia, asig.sala);

            estado.desaplicar(c, asig.dia, asig.periodo, salaActualIdx);

            for (int d = 0; d < instancia.days; ++d) {
                for (int p = 0; p < estado.periodos; ++p) {
                    if (d == asig.dia && p == asig.periodo) continue;

                    for (int r = 0; r < estado.salas; ++r) {
                        if (estado.esValida(instancia, c, d, p, r)) {
                            Movimiento cand;
                            cand.tipo = TipoMovimiento::TIME_MOVE;
                            cand.cursoA = c;
                            cand.lecA = l;
                            cand.nuevoDiaA = d;
                            cand.nuevoPeriodoA = p;
                            cand.nuevaSalaIdxA = r;

                            cand.deltaEval = calcularDeltaMovimiento(instancia, estado, solActual, cand);

                            if (cand.deltaEval < mejor.deltaEval) {
                                mejor = cand;
                            }
                        }
                    }
                }
            }

            estado.aplicar(c, asig.dia, asig.periodo, salaActualIdx);
        }
    }
}

void evaluarRoomMoves(
    const Instancia& instancia,
    EstadoRapido& estado,
    const Solucion& solActual,
    Movimiento& mejor
) {
    for (int c = 0; c < static_cast<int>(solActual.cursos.size()); ++c) {
        for (int l = 0; l < static_cast<int>(solActual.cursos[c].size()); ++l) {
            const auto& asig = solActual.cursos[c][l];
            int salaActualIdx = obtenerIndiceSala(instancia, asig.sala);

            estado.desaplicar(c, asig.dia, asig.periodo, salaActualIdx);

            for (int r = 0; r < estado.salas; ++r) {
                if (r == salaActualIdx) continue;

                if (estado.esValida(instancia, c, asig.dia, asig.periodo, r)) {
                    Movimiento cand;
                    cand.tipo = TipoMovimiento::ROOM_MOVE;
                    cand.cursoA = c;
                    cand.lecA = l;
                    cand.nuevoDiaA = asig.dia;
                    cand.nuevoPeriodoA = asig.periodo;
                    cand.nuevaSalaIdxA = r;

                    cand.deltaEval = calcularDeltaMovimiento(instancia, estado, solActual, cand);

                    if (cand.deltaEval < mejor.deltaEval) {
                        mejor = cand;
                    }
                }
            }

            estado.aplicar(c, asig.dia, asig.periodo, salaActualIdx);
        }
    }
}

void evaluarLectureMoves(
    const Instancia& instancia,
    EstadoRapido& estado,
    const Solucion& solActual,
    Movimiento& mejor
) {
    int numCursos = static_cast<int>(solActual.cursos.size());

    for (int c1 = 0; c1 < numCursos; ++c1) {
        for (int l1 = 0; l1 < static_cast<int>(solActual.cursos[c1].size()); ++l1) {
            const auto& asig1 = solActual.cursos[c1][l1];
            int r1 = obtenerIndiceSala(instancia, asig1.sala);

            for (int c2 = c1; c2 < numCursos; ++c2) {
                int l2_inicio = (c1 == c2) ? l1 + 1 : 0;
                for (int l2 = l2_inicio; l2 < static_cast<int>(solActual.cursos[c2].size()); ++l2) {
                    const auto& asig2 = solActual.cursos[c2][l2];
                    int r2 = obtenerIndiceSala(instancia, asig2.sala);

                    if (asig1.dia == asig2.dia && asig1.periodo == asig2.periodo && r1 == r2) continue;

                    estado.desaplicar(c1, asig1.dia, asig1.periodo, r1);
                    estado.desaplicar(c2, asig2.dia, asig2.periodo, r2);

                    bool v1 = estado.esValida(instancia, c1, asig2.dia, asig2.periodo, r2);
                    estado.aplicar(c1, asig2.dia, asig2.periodo, r2);

                    bool v2 = estado.esValida(instancia, c2, asig1.dia, asig1.periodo, r1);
                    estado.desaplicar(c1, asig2.dia, asig2.periodo, r2);

                    if (v1 && v2) {
                        Movimiento cand;
                        cand.tipo = TipoMovimiento::LECTURE_MOVE;
                        cand.cursoA = c1;
                        cand.lecA = l1;
                        cand.nuevoDiaA = asig2.dia;
                        cand.nuevoPeriodoA = asig2.periodo;
                        cand.nuevaSalaIdxA = r2;

                        cand.cursoB = c2;
                        cand.lecB = l2;
                        cand.nuevoDiaB = asig1.dia;
                        cand.nuevoPeriodoB = asig1.periodo;
                        cand.nuevaSalaIdxB = r1;

                        cand.deltaEval = calcularDeltaMovimiento(instancia, estado, solActual, cand);

                        if (cand.deltaEval < mejor.deltaEval) {
                            mejor = cand;
                        }
                    }

                    estado.aplicar(c1, asig1.dia, asig1.periodo, r1);
                    estado.aplicar(c2, asig2.dia, asig2.periodo, r2);
                }
            }
        }
    }
}

}


void aplicarMovimiento(
    const Instancia& instancia,
    EstadoRapido& estado,
    Solucion& sol,
    const Movimiento& mov) {

    if (mov.tipo == TipoMovimiento::NINGUNO) return;

    if (mov.tipo == TipoMovimiento::TIME_MOVE || mov.tipo == TipoMovimiento::ROOM_MOVE) {
        auto& asig = sol.cursos[mov.cursoA][mov.lecA];
        int salaAntiguaIdx = obtenerIndiceSala(instancia, asig.sala);

        estado.desaplicar(mov.cursoA, asig.dia, asig.periodo, salaAntiguaIdx);

        asig.dia = mov.nuevoDiaA;
        asig.periodo = mov.nuevoPeriodoA;
        asig.sala = instancia.salas[mov.nuevaSalaIdxA].id;

        estado.aplicar(mov.cursoA, mov.nuevoDiaA, mov.nuevoPeriodoA, mov.nuevaSalaIdxA);
    } else if (mov.tipo == TipoMovimiento::LECTURE_MOVE) {
        auto& asigA = sol.cursos[mov.cursoA][mov.lecA];
        auto& asigB = sol.cursos[mov.cursoB][mov.lecB];

        int salaIdxA = obtenerIndiceSala(instancia, asigA.sala);
        int salaIdxB = obtenerIndiceSala(instancia, asigB.sala);

        estado.desaplicar(mov.cursoA, asigA.dia, asigA.periodo, salaIdxA);
        estado.desaplicar(mov.cursoB, asigB.dia, asigB.periodo, salaIdxB);

        std::swap(asigA.dia, asigB.dia);
        std::swap(asigA.periodo, asigB.periodo);
        std::swap(asigA.sala, asigB.sala);

        estado.aplicar(mov.cursoA, mov.nuevoDiaA, mov.nuevoPeriodoA, mov.nuevaSalaIdxA);
        estado.aplicar(mov.cursoB, mov.nuevoDiaB, mov.nuevoPeriodoB, mov.nuevaSalaIdxB);
    }
}

Movimiento buscarMejorVecino(
    const Instancia& instancia,
    EstadoRapido& estado,
    const Solucion& solActual) {


    Movimiento mejor;
    mejor.tipo = TipoMovimiento::NINGUNO;
    mejor.deltaEval = 0;
    evaluarTimeMoves(instancia, estado, solActual, mejor);
    evaluarRoomMoves(instancia, estado, solActual, mejor);
    //evaluarLectureMoves(instancia, estado, solActual, mejor);


    return mejor;
}

Solucion ejecutarHillClimbingMM(
    const Instancia& instancia,
    Solucion solActual,
    EstadoRapido& estado,
    int maxIteraciones) {
    
    for (int iter = 0; iter < maxIteraciones; ++iter) {
        cout<<"[HC + MM] llamando a buscarMejorVecino() N: " << iter<<endl;
        Movimiento mejor = buscarMejorVecino(instancia, estado, solActual);
        // Si no hay mejora estricta, estamos en óptimo local
        if (mejor.deltaEval >= 0) {
            break;
        }

        aplicarMovimiento(instancia, estado, solActual, mejor);
    }

    return solActual;
}

bool solucionVacia(const Solucion& sol) {
    for (const auto& curso : sol.cursos) {
        if (!curso.empty()) {
            return false;
        }
    }
    return true;
}

Solucion aplicarRestart(
    const Instancia& instancia,
    EstadoRapido& estado,
    const Solucion& /* mejorGlobal */,
    const Solucion& /* solActual */,
    std::mt19937& rng,
    int indiceRestart) {

    unsigned int semillaRestart = rng() ^ (static_cast<unsigned int>(indiceRestart + 1) * 0x9e3779b9u);

    Solucion nuevaSol = greedy(instancia, semillaRestart);

    // Si greedy falla, no reconstruimos el estado.
    if (solucionVacia(nuevaSol)) {
        return nuevaSol;
    }

    estado = construirEstadoDesdeSolucion(instancia, nuevaSol);

    return nuevaSol;
}
Solucion orquestarHCMMConRestart(
    const Instancia& instancia,
    Solucion solucionInicial,
    EstadoRapido& estado,
    int maxRestarts,
    int maxIteracionesPorHC,
    unsigned int semillaBase) {
    std::mt19937 rng(semillaBase);

    Solucion mejorSolucion = solucionInicial;
    int mejorCosto = f_eval(instancia, mejorSolucion);
        cout<<"[HC + MM] Haciendo primer llamado a ejecutarHillClimbingMM()"<<endl;
    // 1. Primera corrida de HC + MM desde la solución inicial entregada
    Solucion solActual = ejecutarHillClimbingMM(
        instancia, solucionInicial, estado, maxIteracionesPorHC
    );
    int costoCandidato = f_eval(instancia, solActual);

    if (costoCandidato < mejorCosto) {
        mejorCosto = costoCandidato;
        mejorSolucion = solActual;
    }
    solActual = mejorSolucion;

    // 2. Ciclos de restart + nueva corrida de HC + MM con límite de iteraciones
    for (int r = 0; r < maxRestarts; ++r) {

        cout<<"[HC + MM] Reestart de " << instancia.nombre << endl;
        solActual = aplicarRestart(instancia, estado, mejorSolucion, solActual, rng, r);
        if (solucionVacia(solActual)) {
            cout << "[HC + MM] Greedy no encontró solución. Saltando restart." << endl;
            continue;
        }
        cout<<"[HC + MM] ejecutarHillClimbingMM() numero: " << r<<endl;
        solActual = ejecutarHillClimbingMM(
            instancia, solActual, estado, maxIteracionesPorHC
        );

        costoCandidato = f_eval(instancia, solActual);
        if (costoCandidato < mejorCosto) {
            mejorCosto = costoCandidato;
            mejorSolucion = solActual;
        }
    }

    return mejorSolucion;
}