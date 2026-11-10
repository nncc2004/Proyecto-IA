#include "lectura.h"
#include "greedy.h"
#include "restricciones.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
using namespace std;

const std::string rutaInstancias = "instancias/";

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

vector<string> obtenerArchivos(const string& ruta) {

    vector<string> archivos;

    if (!fs::exists(ruta) || !fs::is_directory(ruta)) {
        cerr << "Error: directorio inexistente o invalido." << endl;
        return archivos;
    }

    for (const auto& entrada : fs::directory_iterator(ruta)) {

        if (entrada.is_regular_file()) {
            archivos.push_back(entrada.path().filename().string());
        }
    }

    return archivos;
}

bool guardarSolucion(const Solucion& S,const string& ruta, const string& nombreArchivo) {

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

    // Construir la ruta completa
    string rutaCompleta = ruta + "/" + nombre;

    ofstream archivo(rutaCompleta);

    if (!archivo.is_open()) {
        cerr << "Error: no se pudo crear el archivo "
             << rutaCompleta << endl;
        return false;
    }

    // Escribir solución
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

    archivo.close();
    cout<<"Archivo creado en " << rutaCompleta<<"\n";
    return true;
}


int main() {
   
    vector<string> archivos = obtenerArchivos(rutaInstancias);
    
    for(size_t i = 0; i< archivos.size(); i++){
        Instancia instancia;
        string arch = rutaInstancias + archivos[i];
        cout<<"Iniciando lectura de:  "<<archivos[i]<<"\n";
        //Lectura de la instancia
        if (!extraerInstancia(arch, instancia)) {
            cerr << "Error al leer la instancia." << endl;
            return 1;
        }

        //imprimirInstancia(instancia);

        //Generación solución incial
        Solucion S = greedy(instancia);

        bool vacia = true;
        for (const auto& curso : S.cursos) {
            if (!curso.empty()) {
                vacia = false;
                break;
            }
        }
        if(vacia){
            cout<<"\n\nSin solución para "<< arch<<"\n";
        }else{
            cout<<"\n\nSolucion encontrada para "<< arch<<"\n";
        }
        guardarSolucion(S, "./OUTPUTS", archivos[i]);
    }
    return 0;
}