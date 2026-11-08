#include "greedy.h"
#include "restricciones.h"

#include <vector>
#include <algorithm>

Solucion inicializarSolucion(const Instancia& instancia) {

    Solucion solucion;
    solucion.cursos.resize(instancia.cursos.size());

    return solucion;
}



vector<int> ordenarCursos(const Instancia& instancia) {

    vector<int> orden;

    for (size_t  i = 0; i < instancia.cursos.size(); i++) {
        orden.push_back(i);
    }

    // Ordena segun la disponibilidad (en empate por n de lectures y alfabetico)
    sort(orden.begin(), orden.end(),
        [&](int a, int b) {

            // disponibilidad
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

            //lectures
            if (instancia.cursos[a].lectures != instancia.cursos[b].lectures) {
                return instancia.cursos[a].lectures >
                       instancia.cursos[b].lectures;
            }

            // alafabetico
            return instancia.cursos[a].id < instancia.cursos[b].id;
        }
    );

    return orden;
}


bool buscarAsignacion(const Instancia& instancia,  Solucion& solucion, int indiceCurso) {
    // Busca una asignacion dia/priodo/sala para una lecture de un curso

    //Dias
    for (int dia = 0; dia < instancia.days; ++dia) {

        //periodos
        for (int periodo = 0; periodo < instancia.periods_per_day; ++periodo) {

            // salas
            for (const auto& sala : instancia.salas) {

                Asignacion nueva = {dia, periodo, sala.id};
                solucion.cursos[indiceCurso].push_back(nueva);

                // Compurueba todas las restricciones
                if (validarTodas(instancia, solucion))  return true;

                // SI no es factivle, la elimina y prueba la isguiente
                solucion.cursos[indiceCurso].pop_back();
            }
        }
    }
    //Si no hay factible
    return false;
}

bool asignarCurso(const Instancia& instancia, Solucion& solucion, int indiceCurso) {

    // Asignar todas las lectures del curso
    for (int i = 0; i < instancia.cursos[indiceCurso].lectures; ++i) {

        if (!buscarAsignacion(instancia, solucion, indiceCurso)) {
            return false;
        }
    }

    return true;
}



Solucion greedy(const Instancia& instancia) {

    // Obtener el orden de prioridad de los cursos
    vector<int> orden = ordenarCursos(instancia);

    // Probar distintas rotaciones del orden
    for (size_t  intento = 0; intento < orden.size(); ++intento) {

        // Inicializar una solución vacía
        Solucion solucion = inicializarSolucion(instancia);

        bool factible = true;

        // Recorrer los cursos en orden circular
        for (size_t  i = 0; i < orden.size(); ++i) {

            int indiceCurso = orden[(intento + i) % orden.size()];

            // Intentar asignar todas las lectures del curso
            if (!asignarCurso(instancia, solucion, indiceCurso)) {
                factible = false;
                break;
            }
        }

        // Si todos los cursos fueron asignados, retornar solución
        if (factible) {
            return solucion;
        }
    }

    // No se encontró una solución factible
    return inicializarSolucion(instancia);
}


// Solucion greedy(const Instancia& instancia){
//     Solucion S = inicializarSolucion(instancia);
//     int reintentos = 0;
    
//     vector<int> orden_cursos = ordenarCursos(instancia);
//     int pos_inicial = reintento % orden_cursos.size();



//     return S;
// }