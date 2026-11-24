#include "greedy2.h"
#include "restricciones.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

int contador_general = 1;

Solucion inicializarSolucion(const Instancia& instancia) {
    Solucion solucion;
    solucion.cursos.resize(instancia.cursos.size());
    return solucion;
}

vector<int> ordenarCursos(const Instancia& instancia) {
    vector<int> orden;

    for (size_t i = 0; i < instancia.cursos.size(); i++) {
        orden.push_back(i);
    }

    sort(orden.begin(), orden.end(),
        [&](int a, int b) {
            int restriccionesA = 0;
            int restriccionesB = 0;

            for (const auto& r : instancia.restricciones) {
                if (r.curso == instancia.cursos[a].id) {
                    restriccionesA++;
                }
                if (r.curso == instancia.cursos[b].id) {
                    restriccionesB++;
                }
            }

            if (restriccionesA != restriccionesB) {
                return restriccionesA > restriccionesB;
            }

            if (instancia.cursos[a].lectures != instancia.cursos[b].lectures) {
                return instancia.cursos[a].lectures > instancia.cursos[b].lectures;
            }

            return instancia.cursos[a].id < instancia.cursos[b].id;
        }
    );

    return orden;
}

namespace {

int contarOpcionesValidas(
    const Instancia& instancia,
    const Solucion& solucion,
    int indiceCurso) {
    int opciones = 0;

    for (int dia = 0; dia < instancia.days; ++dia) {
        for (int periodo = 0; periodo < instancia.periods_per_day; ++periodo) {
            for (const auto& sala : instancia.salas) {
                Asignacion candidata = {dia, periodo, sala.id};
                if (validarAsignacion(instancia, solucion, indiceCurso, candidata)) {
                    ++opciones;
                }
            }
        }
    }

    return opciones;
}

bool asignarPrimeraOpcion(
    const Instancia& instancia,
    Solucion& solucion,
    int indiceCurso) {
    int intentos = 0;

    for (int dia = 0; dia < instancia.days; ++dia) {
        for (int periodo = 0; periodo < instancia.periods_per_day; ++periodo) {
            for (const auto& sala : instancia.salas) {
                ++intentos;
                cout << "[" << contador_general << ":BUSCAR] Intento " << intentos
                     << " | Dia " << dia
                     << " | Periodo " << periodo
                     << " | Sala " << sala.id << endl;
                ++contador_general;

                Asignacion nueva = {dia, periodo, sala.id};
                if (validarAsignacion(instancia, solucion, indiceCurso, nueva)) {
                    solucion.cursos[indiceCurso].push_back(nueva);
                    return true;
                }
            }
        }
    }

    return false;
}

} // namespace

Solucion greedy2(const Instancia& instancia) {
    vector<int> ordenDesempate = ordenarCursos(instancia);
    vector<int> prioridadDesempate(instancia.cursos.size(), 0);
    for (size_t i = 0; i < ordenDesempate.size(); ++i) {
        prioridadDesempate[ordenDesempate[i]] = static_cast<int>(i);
    }

    size_t totalLectures = 0;
    for (const auto& curso : instancia.cursos) {
        totalLectures += static_cast<size_t>(curso.lectures);
    }

    for (size_t intento = 0; intento < instancia.cursos.size(); ++intento) {
        cout << "[GREEDY] Intento " << intento << endl;
        Solucion solucion = inicializarSolucion(instancia);
        bool factible = true;

        for (size_t paso = 0; paso < totalLectures; ++paso) {
            int mejorCurso = -1;
            int menorOpciones = INT_MAX;

            for (size_t c = 0; c < instancia.cursos.size(); ++c) {
                int pendientes = instancia.cursos[c].lectures -
                                 static_cast<int>(solucion.cursos[c].size());
                if (pendientes <= 0) {
                    continue;
                }

                int opciones = contarOpcionesValidas(instancia, solucion, static_cast<int>(c));
                if (opciones == 0) {
                    factible = false;
                    break;
                }

                int prioActual = (prioridadDesempate[c] + static_cast<int>(intento)) %
                                 static_cast<int>(instancia.cursos.size());
                int prioMejor = (mejorCurso == -1)
                                    ? INT_MAX
                                    : (prioridadDesempate[mejorCurso] + static_cast<int>(intento)) %
                                          static_cast<int>(instancia.cursos.size());

                if (opciones < menorOpciones ||
                    (opciones == menorOpciones && prioActual < prioMejor)) {
                    menorOpciones = opciones;
                    mejorCurso = static_cast<int>(c);
                }
            }

            if (!factible || mejorCurso == -1) {
                factible = false;
                break;
            }

            cout << "[GREEDY] Curso " << mejorCurso
                 << " (" << instancia.cursos[mejorCurso].id << ")"
                 << " | Opciones: " << menorOpciones << endl;

            if (!asignarPrimeraOpcion(instancia, solucion, mejorCurso)) {
                factible = false;
                break;
            }
        }

        if (factible) {
            return solucion;
        }
    }

    return inicializarSolucion(instancia);
}