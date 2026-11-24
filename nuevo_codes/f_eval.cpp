#include "f_eval.h"
#include "hc.h"
#include "restricciones.h"
#include <algorithm>

using namespace std;
//S1: Room Capacity
int evaluarS1(const Instancia& instancia, const Solucion& solucion) {
    (void) instancia;
    int penalizacion = 0;

    for (size_t i = 0; i < solucion.cursos.size(); ++i) {
        const Curso& curso = instancia.cursos[i];

        for (const Asignacion& asignacion : solucion.cursos[i]) {
            for (const Sala& sala : instancia.salas) {
                if (sala.id == asignacion.sala) {
                    penalizacion += max(0, curso.students - sala.capacity);
                    break;
                }
            }
        }
    }

    return penalizacion;
}

//S2: Minimum Working Days
int evaluarS2(const Instancia& instancia, const Solucion& solucion) {
    int penalizacion = 0;

    for (size_t i = 0; i < solucion.cursos.size(); ++i) {
        const Curso& curso = instancia.cursos[i];

        vector<bool> diasOcupados(instancia.days, false);

        for (const Asignacion& asignacion : solucion.cursos[i]) {
            diasOcupados[asignacion.dia] = true;
        }

        int diasUtilizados = 0;

        for (bool ocupado : diasOcupados) {
            if (ocupado) {
                ++diasUtilizados;
            }
        }

        penalizacion += max(0, curso.minWorkingDays - diasUtilizados);
    }

    return penalizacion;
}


//S3: Curriculum Compactness (evitar ventanas)
int evaluarS3(const Instancia& instancia, const Solucion& solucion) {
    int penalizacion = 0;

    for (const Curricula& curr : instancia.curriculas) {

        vector<vector<bool>> ocupados(
            instancia.days,
            vector<bool>(instancia.periods_per_day, false)
        );

        // periodos de la curricula
        for (const string& idCurso : curr.cursos) {

            for (size_t i = 0; i < instancia.cursos.size(); ++i) {

                if (instancia.cursos[i].id == idCurso) {

                    for (const Asignacion& asignacion : solucion.cursos[i]) {
                        ocupados[asignacion.dia][asignacion.periodo] = true;
                    }

                    break;
                }
            }
        }

        // clases aisladas
        for (int dia = 0; dia < instancia.days; ++dia) {
            for (int periodo = 0; periodo < instancia.periods_per_day; ++periodo) {

                if (!ocupados[dia][periodo]) {
                    continue;
                }

                bool anterior = (periodo > 0) && ocupados[dia][periodo - 1];
                bool siguiente = (periodo < instancia.periods_per_day - 1)
                                 && ocupados[dia][periodo + 1];

                if (!anterior && !siguiente) {
                    penalizacion++;
                }
            }
        }
    }

    return penalizacion;
}
//S4: Room stability (penalizar por cantidad de salas diferentes para un mismo curso sin contar la primera)
int evaluarS4(const Instancia& instancia, const Solucion& solucion) {
    (void) instancia;
    int penalizacion = 0;

    for (size_t i = 0; i < solucion.cursos.size(); ++i) {

        vector<string> salasUtilizadas;
        //Encotnrar todas las salas diferentes
        for (const Asignacion& asignacion : solucion.cursos[i]) {

            bool encontrada = false;

            for (const string& idSala : salasUtilizadas) {
                if (idSala == asignacion.sala) {
                    encontrada = true;
                    break;
                }
            }

            if (!encontrada) {
                salasUtilizadas.push_back(asignacion.sala);
            }
        }

        // penalización
        if (!salasUtilizadas.empty()) {
            penalizacion += salasUtilizadas.size() - 1; //-1 pq nocuenta la primera
        }
    }

    return penalizacion;
}


int f_eval(const Instancia& instancia, const Solucion& solucion) {
    int S1 = evaluarS1(instancia, solucion);
    int S2 = evaluarS2(instancia, solucion);
    int S3 = evaluarS3(instancia, solucion);
    int S4 = evaluarS4(instancia, solucion);

    return S1 + 5 * S2 + 2 * S3 + S4;
}


/*
int calcularDeltaMovimiento(
    const Instancia& instancia,
    const EstadoRapido& estado,
    const Solucion& solActual,
    const Movimiento& mov) {
    Solucion solCandidata = solActual;

    if (mov.tipo == TipoMovimiento::TIME_MOVE || mov.tipo == TipoMovimiento::ROOM_MOVE) {
        solCandidata.cursos[mov.cursoA][mov.lecA].dia = mov.nuevoDiaA;
        solCandidata.cursos[mov.cursoA][mov.lecA].periodo = mov.nuevoPeriodoA;
        solCandidata.cursos[mov.cursoA][mov.lecA].sala = instancia.salas[mov.nuevaSalaIdxA].id;
    } else if (mov.tipo == TipoMovimiento::LECTURE_MOVE) {
        std::swap(solCandidata.cursos[mov.cursoA][mov.lecA].dia, solCandidata.cursos[mov.cursoB][mov.lecB].dia);
        std::swap(solCandidata.cursos[mov.cursoA][mov.lecA].periodo, solCandidata.cursos[mov.cursoB][mov.lecB].periodo);
        std::swap(solCandidata.cursos[mov.cursoA][mov.lecA].sala, solCandidata.cursos[mov.cursoB][mov.lecB].sala);
    }

    return f_eval(instancia, solCandidata) - f_eval(instancia, solActual);
}
    
*/




// Obtiene la asignacion que tendria una lecture despues del movimiento.
static Asignacion obtenerAsignacionPost(
    const Solucion& sol,
    const Movimiento& mov,
    int curso,
    int lecture
) {
    Asignacion resultado = sol.cursos[curso][lecture];

    if (mov.tipo == TipoMovimiento::TIME_MOVE ||
        mov.tipo == TipoMovimiento::ROOM_MOVE) {

        if (curso == mov.cursoA && lecture == mov.lecA) {
            resultado.dia = mov.nuevoDiaA;
            resultado.periodo = mov.nuevoPeriodoA;
            // La sala se obtiene posteriormente desde la instancia.
        }
    }
    else if (mov.tipo == TipoMovimiento::LECTURE_MOVE) {

        if (curso == mov.cursoA && lecture == mov.lecA) {
            resultado.dia = sol.cursos[mov.cursoB][mov.lecB].dia;
            resultado.periodo = sol.cursos[mov.cursoB][mov.lecB].periodo;
            resultado.sala = sol.cursos[mov.cursoB][mov.lecB].sala;
        }
        else if (curso == mov.cursoB && lecture == mov.lecB) {
            resultado.dia = sol.cursos[mov.cursoA][mov.lecA].dia;
            resultado.periodo = sol.cursos[mov.cursoA][mov.lecA].periodo;
            resultado.sala = sol.cursos[mov.cursoA][mov.lecA].sala;
        }
    }

    return resultado;
}

static string obtenerSalaPost(
    const Instancia& instancia,
    const Solucion& sol,
    const Movimiento& mov,
    int curso,
    int lecture
) {
    if ((mov.tipo == TipoMovimiento::TIME_MOVE ||
         mov.tipo == TipoMovimiento::ROOM_MOVE) &&
        curso == mov.cursoA && lecture == mov.lecA) {

        return instancia.salas[mov.nuevaSalaIdxA].id;
    }

    return obtenerAsignacionPost(sol, mov, curso, lecture).sala;
}


// S1
static int deltaS1Incremental(
    const Instancia& instancia,
    const Solucion& sol,
    const Movimiento& mov
) {
    auto penalizacion = [&](int curso, const string& salaId) {
        const Curso& c = instancia.cursos[curso];

        for (const Sala& sala : instancia.salas) {
            if (sala.id == salaId) {
                return max(0, c.students - sala.capacity);
            }
        }

        return 0;
    };

    int delta = 0;

    int cA = mov.cursoA;
    int lA = mov.lecA;

    delta -= penalizacion(cA, sol.cursos[cA][lA].sala);
    delta += penalizacion(
        cA,
        obtenerSalaPost(instancia, sol, mov, cA, lA)
    );

    if (mov.tipo == TipoMovimiento::LECTURE_MOVE) {
        int cB = mov.cursoB;
        int lB = mov.lecB;

        delta -= penalizacion(cB, sol.cursos[cB][lB].sala);
        delta += penalizacion(
            cB,
            obtenerSalaPost(instancia, sol, mov, cB, lB)
        );
    }

    return delta;
}


// S2
static int penalizacionDiasCurso(
    const Instancia& instancia,
    const Solucion& sol,
    const Movimiento& mov,
    int curso,
    bool posterior
) {
    vector<bool> dias(instancia.days, false);

    for (int l = 0; l < static_cast<int>(sol.cursos[curso].size()); ++l) {
        Asignacion a = posterior
            ? obtenerAsignacionPost(sol, mov, curso, l)
            : sol.cursos[curso][l];

        dias[a.dia] = true;
    }

    int utilizados = 0;

    for (bool ocupado : dias) {
        if (ocupado) ++utilizados;
    }

    return max(0, instancia.cursos[curso].minWorkingDays - utilizados);
}

static int deltaS2Incremental(
    const Instancia& instancia,
    const Solucion& sol,
    const Movimiento& mov
) {
    int delta = 0;

    delta -= penalizacionDiasCurso(
        instancia, sol, mov, mov.cursoA, false
    );

    delta += penalizacionDiasCurso(
        instancia, sol, mov, mov.cursoA, true
    );

    if (mov.tipo == TipoMovimiento::LECTURE_MOVE &&
        mov.cursoB != mov.cursoA) {

        delta -= penalizacionDiasCurso(
            instancia, sol, mov, mov.cursoB, false
        );

        delta += penalizacionDiasCurso(
            instancia, sol, mov, mov.cursoB, true
        );
    }

    return delta;
}


// S3
static int penalizacionCurricula(
    const Instancia& instancia,
    const Solucion& sol,
    const Movimiento& mov,
    int indiceCurricula,
    bool posterior
) {
    vector<vector<bool>> ocupados(
        instancia.days,
        vector<bool>(instancia.periods_per_day, false)
    );

    const Curricula& curr = instancia.curriculas[indiceCurricula];

    for (const string& idCurso : curr.cursos) {

        for (int c = 0; c < static_cast<int>(instancia.cursos.size()); ++c) {

            if (instancia.cursos[c].id != idCurso) continue;

            for (int l = 0; l < static_cast<int>(sol.cursos[c].size()); ++l) {

                Asignacion a = posterior
                    ? obtenerAsignacionPost(sol, mov, c, l)
                    : sol.cursos[c][l];

                ocupados[a.dia][a.periodo] = true;
            }

            break;
        }
    }

    int penalizacion = 0;

    for (int d = 0; d < instancia.days; ++d) {
        for (int p = 0; p < instancia.periods_per_day; ++p) {

            if (!ocupados[d][p]) continue;

            bool anterior = p > 0 && ocupados[d][p - 1];
            bool siguiente =
                p < instancia.periods_per_day - 1 &&
                ocupados[d][p + 1];

            if (!anterior && !siguiente) {
                ++penalizacion;
            }
        }
    }

    return penalizacion;
}

static int deltaS3Incremental(
    const Instancia& instancia,
    const Solucion& sol,
    const Movimiento& mov
) {
    vector<int> afectadas;

    auto agregarCurriculas = [&](int curso) {
        const string& id = instancia.cursos[curso].id;

        for (int q = 0; q < static_cast<int>(instancia.curriculas.size()); ++q) {
            for (const string& idCurso : instancia.curriculas[q].cursos) {

                if (idCurso == id) {
                    if (find(afectadas.begin(), afectadas.end(), q)
                        == afectadas.end()) {
                        afectadas.push_back(q);
                    }
                    break;
                }
            }
        }
    };

    agregarCurriculas(mov.cursoA);

    if (mov.tipo == TipoMovimiento::LECTURE_MOVE) {
        agregarCurriculas(mov.cursoB);
    }

    int delta = 0;

    for (int q : afectadas) {
        delta -= penalizacionCurricula(
            instancia, sol, mov, q, false
        );

        delta += penalizacionCurricula(
            instancia, sol, mov, q, true
        );
    }

    return delta;
}


// S4
static int penalizacionSalasCurso(
    const Instancia& instancia,
    const Solucion& solucion,
    const Movimiento& mov,
    int curso,
    bool post
) {
    vector<string> salas;

    for (int l = 0; l < static_cast<int>(solucion.cursos[curso].size()); ++l) {

        string sala = post
            ? obtenerSalaPost(instancia, solucion, mov, curso, l)
            : solucion.cursos[curso][l].sala;

        if (find(salas.begin(), salas.end(), sala) == salas.end()) {
            salas.push_back(sala);
        }
    }

    return salas.empty() ? 0 : static_cast<int>(salas.size()) - 1;
}

static int deltaS4Incremental(
    const Instancia& instancia,
    const Solucion& sol,
    const Movimiento& mov
) {
    (void)instancia;

    int delta = 0;

    delta -= penalizacionSalasCurso(
        instancia, sol, mov, mov.cursoA, false
    );

    delta += penalizacionSalasCurso(
        instancia, sol, mov, mov.cursoA, true
    );

    if (mov.tipo == TipoMovimiento::LECTURE_MOVE &&
        mov.cursoB != mov.cursoA) {

        delta -= penalizacionSalasCurso(
            instancia, sol, mov, mov.cursoB, false
        );

        delta += penalizacionSalasCurso(
            instancia, sol, mov, mov.cursoB, true
        );
    }

    return delta;
}

int calcularDeltaMovimiento(
    const Instancia& instancia,
    const EstadoRapido& estado,
    const Solucion& solActual,
    const Movimiento& mov
) {
    (void)estado;

    if (mov.tipo == TipoMovimiento::NINGUNO) return 0;

    int dS1 = deltaS1Incremental(instancia, solActual, mov);
    int dS2 = deltaS2Incremental(instancia, solActual, mov);
    int dS3 = deltaS3Incremental(instancia, solActual, mov);
    int dS4 = deltaS4Incremental(instancia, solActual, mov);

    return dS1 + 5 * dS2 + 2 * dS3 + dS4;
}
