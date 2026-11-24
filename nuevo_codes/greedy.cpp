#include "greedy.h"
#include "restricciones.h"
#include <algorithm>
#include <ostream>
#include <iostream>
#include <climits>
#include <random>
#include <vector>

Solucion inicializarSolucion(const Instancia& instancia) {
    Solucion solucion;
    solucion.cursos.resize(instancia.cursos.size());
    return solucion;
}

vector<int> ordenarCursos(const Instancia& instancia) {
    vector<int> orden(instancia.cursos.size());
    for (size_t i = 0; i < instancia.cursos.size(); ++i) {
        orden[i] = static_cast<int>(i);
    }

    vector<int> indisponibilidades(instancia.cursos.size(), 0);
    for (const auto& r : instancia.restricciones) {
        for (size_t c = 0; c < instancia.cursos.size(); ++c) {
            if (instancia.cursos[c].id == r.curso) {
                ++indisponibilidades[c];
            }
        }
    }

    sort(orden.begin(), orden.end(), [&](int a, int b) {
        if (indisponibilidades[a] != indisponibilidades[b]) {
            return indisponibilidades[a] > indisponibilidades[b];
        }
        if (instancia.cursos[a].lectures != instancia.cursos[b].lectures) {
            return instancia.cursos[a].lectures > instancia.cursos[b].lectures;
        }
        if (instancia.cursos[a].minWorkingDays != instancia.cursos[b].minWorkingDays) {
            return instancia.cursos[a].minWorkingDays > instancia.cursos[b].minWorkingDays;
        }
        return instancia.cursos[a].id < instancia.cursos[b].id;
    });

    return orden;
}

namespace {

int puntuarCandidato(
    const Instancia& instancia,
    const EstadoRapido& estado,
    int cursoIdx,
    int dia,
    int periodo,
    int salaIdx,
    int clasesDiaCurso,
    int totalDiasCurso
) {
    int score = 0;

    // 1. Preferir días distintos para repartir lectures (y cumplir minWorkingDays)
    int diasFaltantes = instancia.cursos[cursoIdx].minWorkingDays - totalDiasCurso;
    if (diasFaltantes > 0 && clasesDiaCurso == 0) {
        score -= 1000;
    } else if (clasesDiaCurso == 0) {
        score -= 200;
    } else {
        score += clasesDiaCurso * 300;
    }

    // 2. Evitar saturar periodos/currículas ya cargadas
    int ocupacionBloque = 0;
    for (int r = 0; r < estado.salas; ++r) {
        if (estado.ocupadoSala[dia][periodo][r]) {
            ++ocupacionBloque;
        }
    }
    score += ocupacionBloque * 15;

    for (int q : estado.curriculasDeCurso[cursoIdx]) {
        int ocupCurrDia = 0;
        for (int p = 0; p < estado.periodos; ++p) {
            if (estado.ocupadoCurricula[q][dia][p]) {
                ++ocupCurrDia;
            }
        }
        score += ocupCurrDia * 10;
    }

    // 3. Elegir sala con capacidad suficiente sin desperdiciar demasiado
    int alumnos = instancia.cursos[cursoIdx].students;
    int capacidad = instancia.salas[salaIdx].capacity;
    if (capacidad >= alumnos) {
        score += (capacidad - alumnos);
    } else {
        score += 5000 + (alumnos - capacidad);
    }

    return score;
}

bool construirIntento(
    const Instancia& instancia,
    const vector<int>& ordenCursos,
    Solucion& solucion,
    mt19937& rng) {
    solucion = inicializarSolucion(instancia);
    EstadoRapido estado;
    estado.inicializar(instancia);

    vector<int> clasesPorDia(instancia.cursos.size() * instancia.days, 0);
    auto idxDia = [&](int c, int d) -> int {
        return c * instancia.days + d;
    };

    uniform_int_distribution<int> ruido(0, 4);

    for (int cursoIdx : ordenCursos) {
        const Curso& curso = instancia.cursos[cursoIdx];

        for (int lec = 0; lec < curso.lectures; ++lec) {
            int totalDiasUsados = 0;
            for (int d = 0; d < instancia.days; ++d) {
                if (clasesPorDia[idxDia(cursoIdx, d)] > 0) {
                    ++totalDiasUsados;
                }
            }

            int mejorScore = INT_MAX;
            int mejorDia = -1;
            int mejorPeriodo = -1;
            int mejorSala = -1;

            for (int d = 0; d < instancia.days; ++d) {
                for (int p = 0; p < estado.periodos; ++p) {
                    for (int r = 0; r < estado.salas; ++r) {
                        if (!estado.esValida(instancia, cursoIdx, d, p, r)) {
                            continue;
                        }

                        int score = puntuarCandidato(
                            instancia,
                            estado,
                            cursoIdx,
                            d,
                            p,
                            r,
                            clasesPorDia[idxDia(cursoIdx, d)],
                            totalDiasUsados
                        ) + ruido(rng);

                        if (score < mejorScore) {
                            mejorScore = score;
                            mejorDia = d;
                            mejorPeriodo = p;
                            mejorSala = r;
                        }
                    }
                }
            }

            if (mejorDia == -1) {
                return false;
            }

            Asignacion nueva{mejorDia, mejorPeriodo, instancia.salas[mejorSala].id};
            solucion.cursos[cursoIdx].push_back(nueva);
            estado.aplicar(cursoIdx, mejorDia, mejorPeriodo, mejorSala);
            ++clasesPorDia[idxDia(cursoIdx, mejorDia)];
        }
    }

    return true;
}

} // namespace

/*
Solucion greedy(const Instancia& instancia) {
    vector<int> baseOrden = ordenarCursos(instancia);
    mt19937 rng(42);
    const int MAX_REINTENTOS = 1500;
    string nombreInstancia = instancia.nombre;
    cout<<"[GREEDY] Iniciando greedy de: " <<nombreInstancia<<endl;
    for (int intento = 0; intento < MAX_REINTENTOS; ++intento) {
        cout<<"[GREEDY] Intento: " << intento <<" ("<<nombreInstancia<<")"<< endl;
        vector<int> ordenActual = baseOrden;

        // A partir del segundo intento, mezcla ligeramente cursos con igual dificultad
        if (intento > 0 && ordenActual.size() > 1) {
            shuffle(ordenActual.begin(), ordenActual.end(), rng);
        }

        Solucion candidata;
        if (construirIntento(instancia, ordenActual, candidata, rng)) {
            return candidata;
        }
    }

    return inicializarSolucion(instancia);
}
*/
namespace {

Solucion ejecutarGreedy(const Instancia& instancia, bool forzarShuffle, unsigned int semilla) {
    vector<int> baseOrden = ordenarCursos(instancia);
    mt19937 rng(forzarShuffle ? semilla : 42u);
    const int MAX_REINTENTOS = 1500;
    string nombreInstancia = instancia.nombre;

    cout << "[GREEDY] Iniciando greedy de: " << nombreInstancia << endl;
    for (int intento = 0; intento < MAX_REINTENTOS; ++intento) {
        cout << "[GREEDY] Intento: " << intento << " (" << nombreInstancia << ")" << endl;
        vector<int> ordenActual = baseOrden;

        if ((forzarShuffle || intento > 0) && ordenActual.size() > 1) {
            shuffle(ordenActual.begin(), ordenActual.end(), rng);
        }

        Solucion candidata;
        if (construirIntento(instancia, ordenActual, candidata, rng)) {
            return candidata;
        }
    }

    return inicializarSolucion(instancia);
}

} // namespace

Solucion greedy(const Instancia& instancia) {
    return ejecutarGreedy(instancia, false, 42u);
}

Solucion greedy(const Instancia& instancia, unsigned int semilla) {
    return ejecutarGreedy(instancia, true, semilla);
}