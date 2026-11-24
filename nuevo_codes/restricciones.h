
#ifndef RESTRICCIONES_H
#define RESTRICCIONES_H

#include "estructuras.h"

// H1: Lectures
bool validarH1(const Instancia& instancia, const Solucion& solucion);

// H2: Room Occupancy
bool validarH2(const Instancia& instancia, const Solucion& solucion);

// H3: Conflict
bool validarH3(const Instancia& instancia, const Solucion& solucion);

// H4: Availability
bool validarH4(const Instancia& instancia, const Solucion& solucion);

//Validacion de todas
bool validarTodas(const Instancia& instancia, const Solucion& solucion);


// Validación local de una nueva asignación
bool validarAsignacion(const Instancia& instancia, const Solucion& solucion, int indiceCurso,const Asignacion& nueva);

// Estado rápido para acelerar búsquedas en O(1)
struct EstadoRapido {
    int dias = 0;
    int periodos = 0;
    int salas = 0;
    int cursos = 0;
    int curriculas = 0;

    std::vector<int> indiceProfesorCurso;
    std::vector<std::vector<int>> curriculasDeCurso;
    std::vector<std::vector<std::vector<bool>>> bloqueadoCurso;

    std::vector<std::vector<std::vector<bool>>> ocupadoCurso;
    std::vector<std::vector<std::vector<bool>>> ocupadoSala;
    std::vector<std::vector<std::vector<bool>>> ocupadoProfesor;
    std::vector<std::vector<std::vector<bool>>> ocupadoCurricula;

    void inicializar(const Instancia& instancia);
    bool esValida(const Instancia& instancia, int indiceCurso, int dia, int periodo, int indiceSala) const;
    void aplicar(int indiceCurso, int dia, int periodo, int indiceSala);
    void desaplicar(int indiceCurso, int dia, int periodo, int indiceSala);
};

EstadoRapido construirEstadoDesdeSolucion(const Instancia& instancia, const Solucion& solucion);

#endif
