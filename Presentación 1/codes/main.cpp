#include "lectura.h"
#include "greedy.h"
#include "restricciones.h"
#include "f_eval.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>


namespace fs = std::filesystem;
using namespace std;

void imprimirSolucion(const Solucion& S) {
    for (size_t i = 0; i < S.cursos.size(); i++) {

        cout << "Curso " << i << ":" << endl;

        for (const auto& asignacion : S.cursos[i]) {

            cout << "  Dia: " << asignacion.dia
                 << " | Periodo: " << asignacion.periodo
                 << " | Sala: " << asignacion.sala
                 << endl;
        }

        cout << endl;
    }

}

bool guardarSolucion(const Solucion& S,const string& ruta, const string& nombreArchivo, int eval) {

    string nombre = nombreArchivo;
    bool vacia = true;

    for (const auto& curso : S.cursos) {
        if (!curso.empty()) {
            vacia = false;
            break;
        }
    }

    if (vacia) {
        nombre = "ERROR_" + nombre;
    }

    string rutaCompleta = ruta + "/" + nombre;

    ofstream archivo(rutaCompleta);

    if (!archivo.is_open()) {
        cerr << "Error: no se pudo crear el archivo "
             << rutaCompleta << endl;
        return false;
    }

    for (size_t i = 0; i < S.cursos.size(); ++i) {

        archivo << "Curso " << i << ":" << endl;

        for (const auto& asignacion : S.cursos[i]) {

            archivo << "  Dia: " << asignacion.dia
                    << " | Periodo: " << asignacion.periodo
                    << " | Sala: " << asignacion.sala
                    << endl;
        }

        archivo << endl;
    }

    archivo <<"f_eval(S) = " << eval << endl;

    archivo.close();
    cout<<"Archivo creado en " << rutaCompleta<<"\n";
    return true;
}



int main() {
    Instancia instancia;

    //Lectura de archivo
    if (!extraerInstancia("ejemplo_base/ejemplo.txt", instancia)) {
        cerr << "Error al leer la instancia." << endl;
        return 1;
    }

    imprimirInstancia(instancia);

    //Generación solución incial
    Solucion S = greedy(instancia);

    cout<<"\n\nSolucion encontrada:\n\n";
    imprimirSolucion(S);
    cout<<"Verificacion de restricciones:\n";
    cout << "H1: " << validarH1(instancia, S) << endl;
    cout << "H2: " << validarH2(instancia, S) << endl;
    cout << "H3: " << validarH3(instancia, S) << endl;
    cout << "H4: " << validarH4(instancia, S) << endl;
    cout<<"\n\n";
        
    int evaluacion = f_eval(instancia, S);
    cout<<"f_eval(S) = "<<evaluacion<<"\n";
    guardarSolucion(S, "./OUTPUTS", "ejemplo.txt", evaluacion);
    

    return 0;
}