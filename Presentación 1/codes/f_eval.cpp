#include "f_eval.h"
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