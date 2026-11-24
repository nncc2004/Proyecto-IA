#include "estructuras.h"
#include <iostream>
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

bool validarTodas(const Instancia& instancia, const Solucion& solucion) {

    cout << "[VALIDAR] H1" << endl;
    if (!validarH1(instancia, solucion)) return false;

    cout << "[VALIDAR] H2" << endl;
    if (!validarH2(instancia, solucion)) return false;

    cout << "[VALIDAR] H3" << endl;
    if (!validarH3(instancia, solucion)) return false;

    cout << "[VALIDAR] H4" << endl;
    if (!validarH4(instancia, solucion)) return false;

    return true;
}



bool validarAsignacion(
    const Instancia& instancia,
    const Solucion& solucion,
    int indiceCurso,
    const Asignacion& nueva
) {
    const Curso& cursoNuevo = instancia.cursos[indiceCurso];

    // H1: El mismo curso no puede tener dos lectures
    // en el mismo día y período.
    for (const auto& asignacion : solucion.cursos[indiceCurso]) {
        if (asignacion.dia == nueva.dia &&
            asignacion.periodo == nueva.periodo) {
            return false;
        }
    }

    // Revisar los demás cursos.
    for (size_t i = 0; i < solucion.cursos.size(); ++i) {

        if (i == static_cast<size_t>(indiceCurso)) {
            continue;
        }

        const Curso& otroCurso = instancia.cursos[i];

        for (const auto& asignacion : solucion.cursos[i]) {

            bool mismoPeriodo =
                asignacion.dia == nueva.dia &&
                asignacion.periodo == nueva.periodo;

            if (!mismoPeriodo) {
                continue;
            }

            // H2: Una sala no puede estar ocupada dos veces.
            if (asignacion.sala == nueva.sala) {
                return false;
            }

            // H3: Dos cursos con el mismo profesor
            // no pueden coincidir.
            if (cursoNuevo.profesor == otroCurso.profesor) {
                return false;
            }

            // H3: Dos cursos de una misma currícula
            // no pueden coincidir.
            for (const auto& curricula : instancia.curriculas) {

                bool contieneNuevo = false;
                bool contieneOtro = false;

                for (const auto& idCurso : curricula.cursos) {
                    if (idCurso == cursoNuevo.id) {
                        contieneNuevo = true;
                    }

                    if (idCurso == otroCurso.id) {
                        contieneOtro = true;
                    }
                }

                if (contieneNuevo && contieneOtro) {
                    return false;
                }
            }
        }
    }

    // H4: El curso no puede tener bloqueado ese período.
    for (const auto& restriccion : instancia.restricciones) {

        if (restriccion.curso == cursoNuevo.id &&
            restriccion.dia == nueva.dia &&
            restriccion.periodo == nueva.periodo) {
            return false;
        }
    }

    return true;
}
