#include "estructuras.h"
#include "restricciones.h"
#include <iostream>
#include <unordered_map>
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
    const Asignacion& nueva) {
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



void EstadoRapido::inicializar(const Instancia& instancia) {
    dias = instancia.days;
    periodos = instancia.periods_per_day;
    salas = static_cast<int>(instancia.salas.size());
    cursos = static_cast<int>(instancia.cursos.size());
    curriculas = static_cast<int>(instancia.curriculas.size());

    unordered_map<string, int> idxCurso;
    idxCurso.reserve(cursos * 2);
    unordered_map<string, int> idxProfe;
    idxProfe.reserve(cursos * 2);

    indiceProfesorCurso.assign(cursos, -1);
    for (int c = 0; c < cursos; ++c) {
        idxCurso[instancia.cursos[c].id] = c;
        const string& prof = instancia.cursos[c].profesor;
        auto it = idxProfe.find(prof);
        if (it == idxProfe.end()) {
            int nuevoId = static_cast<int>(idxProfe.size());
            idxProfe[prof] = nuevoId;
            indiceProfesorCurso[c] = nuevoId;
        } else {
            indiceProfesorCurso[c] = it->second;
        }
    }

    curriculasDeCurso.assign(cursos, {});
    for (int q = 0; q < curriculas; ++q) {
        for (const auto& idC : instancia.curriculas[q].cursos) {
            auto it = idxCurso.find(idC);
            if (it != idxCurso.end()) {
                curriculasDeCurso[it->second].push_back(q);
            }
        }
    }

    bloqueadoCurso.assign(cursos,
        std::vector<std::vector<bool>>(dias, std::vector<bool>(periodos, false)));

    for (const auto& r : instancia.restricciones) {
        auto it = idxCurso.find(r.curso);
        if (it != idxCurso.end() &&
            r.dia >= 0 && r.dia < dias &&
            r.periodo >= 0 && r.periodo < periodos) {
            bloqueadoCurso[it->second][r.dia][r.periodo] = true;
        }
    }

    ocupadoCurso.assign(cursos,
        std::vector<std::vector<bool>>(dias, std::vector<bool>(periodos, false)));
    ocupadoSala.assign(dias,
        std::vector<std::vector<bool>>(periodos, std::vector<bool>(salas, false)));
    ocupadoProfesor.assign(idxProfe.size(),
        std::vector<std::vector<bool>>(dias, std::vector<bool>(periodos, false)));
    ocupadoCurricula.assign(curriculas,
        std::vector<std::vector<bool>>(dias, std::vector<bool>(periodos, false)));
}

bool EstadoRapido::esValida(
    const Instancia& instancia,
    int indiceCurso,
    int dia,
    int periodo,
    int indiceSala
) const {
    (void)instancia;
    if (dia < 0 || dia >= dias || periodo < 0 || periodo >= periodos ||
        indiceSala < 0 || indiceSala >= salas) {
        return false;
    }
    if (bloqueadoCurso[indiceCurso][dia][periodo]) return false; //H4
    if (ocupadoCurso[indiceCurso][dia][periodo]) return false; //H1
    if (ocupadoSala[dia][periodo][indiceSala]) return false; //H2

    int prof = indiceProfesorCurso[indiceCurso];
    if (prof >= 0 && ocupadoProfesor[prof][dia][periodo]) return false; //H3 (profesor)

    for (int q : curriculasDeCurso[indiceCurso]) {
        if (ocupadoCurricula[q][dia][periodo]) return false; //H3 (curricula)
    }

    return true;
}

void EstadoRapido::aplicar(int indiceCurso, int dia, int periodo, int indiceSala) {
    ocupadoCurso[indiceCurso][dia][periodo] = true;
    ocupadoSala[dia][periodo][indiceSala] = true;

    int prof = indiceProfesorCurso[indiceCurso];
    if (prof >= 0) {
        ocupadoProfesor[prof][dia][periodo] = true;
    }

    for (int q : curriculasDeCurso[indiceCurso]) {
        ocupadoCurricula[q][dia][periodo] = true;
    }
}

void EstadoRapido::desaplicar(int indiceCurso, int dia, int periodo, int indiceSala) {
    ocupadoCurso[indiceCurso][dia][periodo] = false;
    ocupadoSala[dia][periodo][indiceSala] = false;

    int prof = indiceProfesorCurso[indiceCurso];
    if (prof >= 0) {
        ocupadoProfesor[prof][dia][periodo] = false;
    }

    for (int q : curriculasDeCurso[indiceCurso]) {
        ocupadoCurricula[q][dia][periodo] = false;
    }
}



EstadoRapido construirEstadoDesdeSolucion(const Instancia& instancia, const Solucion& solucion) {
    EstadoRapido estado;
    estado.inicializar(instancia);

    for (int cursoIdx = 0; cursoIdx < static_cast<int>(solucion.cursos.size()); ++cursoIdx) {
        for (const auto& asig : solucion.cursos[cursoIdx]) {
            // Buscamos el índice de la sala a partir de su ID
            int salaIdx = -1;
            for (int r = 0; r < static_cast<int>(instancia.salas.size()); ++r) {
                if (instancia.salas[r].id == asig.sala) {
                    salaIdx = r;
                    break;
                }
            }
            if (salaIdx != -1) {
                estado.aplicar(cursoIdx, asig.dia, asig.periodo, salaIdx);
            }
        }
    }
    return estado;
}