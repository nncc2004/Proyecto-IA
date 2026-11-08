#include "estructuras.h"

/**  Hard Constraints **/

//H1
bool validarH1(const Instancia& instancia, const Solucion& solucion) {
    (void)instancia;
    for (const auto& asignaciones : solucion.cursos) {

        for (size_t i = 0; i < asignaciones.size(); i++) {
            for (size_t j = i + 1; j < asignaciones.size(); j++) {
                // Si dia, periodo y curso coinciden. False
                if (asignaciones[i].dia == asignaciones[j].dia &&
                    asignaciones[i].periodo == asignaciones[j].periodo) {
                    return false;
                }
            }
        }
    }

    return true;
}


//H2
bool validarH2(const Instancia& instancia, const Solucion& solucion) {
    (void)instancia;
    for (size_t i = 0; i < solucion.cursos.size(); i++) {
        for (const auto& asignacion1 : solucion.cursos[i]) {

            for (size_t j = i + 1; j < solucion.cursos.size(); j++) {
                for (const auto& asignacion2 : solucion.cursos[j]) {
                    // Si dia, periodo y sala coindicen, False
                    if (asignacion1.dia == asignacion2.dia &&
                        asignacion1.periodo == asignacion2.periodo &&
                        asignacion1.sala == asignacion2.sala) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

//H3
bool validarConflictoProfesor(const Instancia& instancia, const Solucion& solucion) {

    for (size_t i = 0; i < solucion.cursos.size(); i++) {

        for (size_t j = i + 1; j < solucion.cursos.size(); j++) {

            // Si es diferente profesor, va al siguiente
            if (instancia.cursos[i].profesor != instancia.cursos[j].profesor) {
                continue;
            }

            // Aquí ya sé que curso i y curso j comparten profesor. 
            for (const auto& a1 : solucion.cursos[i]) {
                for (const auto& a2 : solucion.cursos[j]) {
                    //Si tienen el mismo profesor, no pueden compartir día y período. 
                    if (a1.dia == a2.dia &&
                        a1.periodo == a2.periodo) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool validarConflictoCurricula(const Instancia& instancia, const Solucion& solucion) {

    for (size_t i = 0; i < solucion.cursos.size(); i++) {

        for (size_t j = i + 1; j < solucion.cursos.size(); j++) {

            bool compartenCurricula = false;

            // Buscar alguna curricula que contenga ambos cursos
            for (const auto& curricula : instancia.curriculas) {

                bool contieneI = false;
                bool contieneJ = false;

                for (const auto& curso : curricula.cursos) {

                    if (curso == instancia.cursos[i].id) {
                        contieneI = true;
                    }

                    if (curso == instancia.cursos[j].id) {
                        contieneJ = true;
                    }
                }

                if (contieneI && contieneJ) {
                    compartenCurricula = true;
                    break;
                }
            }
            //Si no comparten ninguna curricula, vamos a la siguiente
            if (!compartenCurricula) {
                continue;
            }

            // Aquí ya sé que comparten curricula curso i y curso j
            for (const auto& a1 : solucion.cursos[i]) {
                for (const auto& a2 : solucion.cursos[j]) {
                    // Dos mallas de la misma curricula no pueden compartir día y periodo (tope)
                    if (a1.dia == a2.dia &&
                        a1.periodo == a2.periodo) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}



bool validarH3(const Instancia& instancia, const Solucion& solucion) {

    return validarConflictoProfesor(instancia, solucion) &&
           validarConflictoCurricula(instancia, solucion);
}

//H4
bool validarH4(const Instancia& instancia, const Solucion& solucion) {

    for (size_t i = 0; i < solucion.cursos.size(); i++) {

        const string& idCurso = instancia.cursos[i].id;

        for (const auto& asignacion : solucion.cursos[i]) {

            for (const auto& restriccion : instancia.restricciones) {
                //Dada un curso, revisa que cumpla con la restriccion de disponibilidad. 
                if (restriccion.curso == idCurso &&
                    restriccion.dia == asignacion.dia &&
                    restriccion.periodo == asignacion.periodo) {

                    return false;
                }
            }
        }
    }

    return true;
}


bool validarTodas(const Instancia& instancia, const Solucion& solucion){

    if(!validarH1(instancia, solucion)) return false;
    if(!validarH2(instancia, solucion)) return false;
    if(!validarH3(instancia, solucion)) return false;
    if(!validarH4(instancia, solucion)) return false;

    return true;
}