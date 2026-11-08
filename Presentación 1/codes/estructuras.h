
#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <string>
#include <vector>

using namespace std;


//Auxiliares
struct Curso {
    string id;
    string profesor;
    int lectures;
    int minWorkingDays;
    int students;
};


struct Sala {
    string id;
    int capacity;
};


struct Curricula {
    string id;
    vector<string> cursos;
};


struct Disponibilidad {
    string curso;
    int dia;
    int periodo;
};




//Instancia misma
struct Instancia {
    string nombre;
    int courses;
    int rooms;
    int days;
    int periods_per_day;
    int curricula;
    int constraints;


    vector<Curso> cursos;
    vector<Sala> salas;
    vector<Curricula> curriculas;
    vector<Disponibilidad> restricciones;
};



//Para la solución
struct Asignacion {
    int dia;
    int periodo;
    string sala;
};

struct Solucion {
    vector<vector<Asignacion>> cursos;
};


#endif 
