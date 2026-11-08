#include "lectura.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

bool extraerInstancia(const string& ruta, Instancia& instancia) {

    ifstream archivo(ruta);

    if (!archivo.is_open()) {
        cerr << "Error: no se pudo abrir el archivo." << endl;
        return false;
    }

    string linea;

    // cabecera
    while (getline(archivo, linea)) {

        if (linea.find("Name:") == 0) {
            instancia.nombre = linea.substr(5);
        }
        else if (linea.find("Courses:") == 0) {
            instancia.courses = stoi(linea.substr(8));
        }
        else if (linea.find("Rooms:") == 0) {
            instancia.rooms = stoi(linea.substr(6));
        }
        else if (linea.find("Days:") == 0) {
            instancia.days = stoi(linea.substr(5));
        }
        else if (linea.find("Periods_per_day:") == 0) {
            instancia.periods_per_day = stoi(linea.substr(16));
        }
        else if (linea.find("Curricula:") == 0) {
            instancia.curricula = stoi(linea.substr(10));
        }
        else if (linea.find("Constraints:") == 0) {
            instancia.constraints = stoi(linea.substr(12));
            break;
        }
    }
    //Cursos
    if (true) {
        while (getline(archivo, linea)) {
                if (linea == "COURSES:") {
                    break;
                    }
            }


        for (int i = 0; i < instancia.courses; i++) {

            getline(archivo, linea);

            // Ignorar líneas vacías
            if (linea.empty()) {
                i--;
                continue;
            }

            istringstream ss(linea);
            Curso curso;

            ss >> curso.id
            >> curso.profesor
            >> curso.lectures
            >> curso.minWorkingDays
            >> curso.students;

            if (ss.fail()) {
                cerr << "Error al leer el curso." << endl;
                archivo.close();
                return false;
            }

            instancia.cursos.push_back(curso);
        }
    }
    
    // Salas
    if (true) {
        while (getline(archivo, linea)) {
            if (linea == "ROOMS:") {
                break;
            }
        }


        for (int i = 0; i < instancia.rooms; i++) {

            getline(archivo, linea);

            if (linea.empty()) {
                i--;
                continue;
            }

            istringstream ss(linea);
            Sala sala;

            ss >> sala.id >> sala.capacity;

            if (ss.fail()) {
                cerr << "Error al leer la sala." << endl;
                archivo.close();
                return false;
            }

            instancia.salas.push_back(sala);
        }
    }

    // Currícula
    if (true){
        while (getline(archivo, linea)) {
            if (linea == "CURRICULA:") {
                break;
            }
        }

        for (int i = 0; i < instancia.curricula; i++) {

            getline(archivo, linea);

            if (linea.empty()) {
                i--;
                continue;
            }

            istringstream ss(linea);
            Curricula curr;

            int cantidadCursos;
            ss >> curr.id >> cantidadCursos;

            if (ss.fail()) {
                cerr << "Error al leer la curricula." << endl;
                archivo.close();
                return false;
            }

            for (int j = 0; j < cantidadCursos; j++) {
                string idCurso;
                ss >> idCurso;

                if (ss.fail()) {
                    cerr << "Error al leer los cursos de la curricula." << endl;
                    archivo.close();
                    return false;
                }

                curr.cursos.push_back(idCurso);
            }

            instancia.curriculas.push_back(curr);
        }
    }
    
    // Disponibilidad
    if (true){
        while (getline(archivo, linea)) {
            if (linea == "UNAVAILABILITY_CONSTRAINTS:") {
                break;
            }
        }

        for (int i = 0; i < instancia.constraints; i++) {

            getline(archivo, linea);

            if (linea.empty()) {
                i--;
                continue;
            }

            istringstream ss(linea);
            Disponibilidad restriccion;

            ss >> restriccion.curso
            >> restriccion.dia
            >> restriccion.periodo;

            if (ss.fail()) {
                cerr << "Error al leer la restriccion de disponibilidad." << endl;
                archivo.close();
                return false;
            }

            instancia.restricciones.push_back(restriccion);
        }
    }
    archivo.close();

    return true;
}


void imprimirInstancia(const Instancia& instancia) {


    cout << "\n\nNombre instancia: "<<instancia.nombre;
    // Cabecera
    cout << "\n\nCabecera:\n";
    cout << "  - Courses: " << instancia.courses << endl;
    cout << "  - Rooms: " << instancia.rooms << endl;
    cout << "  - Days: " << instancia.days << endl;
    cout << "  - Periods_per_day: " << instancia.periods_per_day << endl;
    cout << "  - Curricula: " << instancia.curricula << endl;
    cout << "  - Constraints: " << instancia.constraints << endl;


    // Cursos
    cout << "\nCOURSES:\n";
    for (const Curso& curso : instancia.cursos) {
        cout << "  - ID: " << curso.id
             << " | Teacher: " << curso.profesor
             << " | #Lectures: " << curso.lectures
             << " | Min. days: " << curso.minWorkingDays
             << " | #Students: " << curso.students << endl;
    }


    // Salas
    cout << "\nROOMS: \n";
    for (const Sala& sala : instancia.salas) {
        cout << "  - ID: " << sala.id
             << " | Capacity: " << sala.capacity << endl;
    }


    // Curriculas
    cout << "\nCURRICULA: \n";
    for (const Curricula& curr : instancia.curriculas) {
        cout << "  - ID: " << curr.id << " | Courses: ";

        for (const string& curso : curr.cursos) {
            cout << curso << " ";
        }

        cout << endl;
    }


    // Restricciones de disponibilidad
    cout << "\nUNAVAILABILITY_CONSTRAINTS: \n";
    for (const Disponibilidad& restriccion : instancia.restricciones) {
        cout << "  - CourseID: " << restriccion.curso
             << " | Day: " << restriccion.dia
             << " | Day_period: " << restriccion.periodo << endl;
    }

        cout<<"\n";
}