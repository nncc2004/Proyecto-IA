#include "lectura.h"
#include "greedy.h"
#include "restricciones.h"
#include "f_eval.h"
#include "hc.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <ctime>

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


bool guardarSolucion(const Solucion& S,const string& ruta, 
                    const string& nombreArchivo, int eval, 
                    int eval_inicial, Instancia instancia,
                    auto duracion, int MAX_RESTARTS, int MAX_ITERACIONES_POR_HC) {

    string nombre = to_string(MAX_ITERACIONES_POR_HC)+"_iters_"+nombreArchivo; 
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
    
    archivo << "H1: " << validarH1(instancia, S) << endl;
    archivo << "H2: " << validarH2(instancia, S) << endl;
    archivo << "H3: " << validarH3(instancia, S) << endl;
    archivo << "H4: " << validarH4(instancia, S) << endl;
    archivo <<"f_eval(S) (inicial) = " << eval_inicial << endl;
    archivo <<"f_eval(S) = " << eval << endl;
    archivo <<"Duracion en segundos: " << duracion<<endl;
    archivo <<"MAX_RESTARTS:" << MAX_RESTARTS<<endl;
    archivo <<"MAX_ITERACIONES_POR_HC: " << MAX_ITERACIONES_POR_HC<<endl;


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

    const int MAX_RESTARTS = 10;
    const int MAX_ITERACIONES_POR_HC = 10;
    const unsigned int SEMILLA_BASE = 42u;

    for (size_t i = 0; i < archivos.size(); i++) {

        string nombre = archivos[i];
        Instancia instancia;
        string arch = rutaInstancias + nombre;

        cout << "\nIniciando lectura de: " << nombre << "\n";

        // Lectura de la instancia
        if (!extraerInstancia(arch, instancia)) {
            cerr << "Error al leer la instancia." << endl;
            continue;
        }

        cout << "Instancia extraida\n";

        // Generación de solución inicial
        Solucion S = greedy(instancia);

        bool vacia = true;
        for (const auto& curso : S.cursos) {
            if (!curso.empty()) {
                vacia = false;
                break;
            }
        }

        if (vacia) {
            cout << "\n\nSin solución inicial para " << arch << "\n";
            continue;
        }

        cout << "\n\nSolucion encontrada para " << arch << "\n";

        int eval_inicial = f_eval(instancia, S);

        // Configuración de HC
        EstadoRapido estado = construirEstadoDesdeSolucion(instancia, S);

        cout << "DESDE AQUI HC\n" << flush;

        auto antes = chrono::steady_clock::now();

        Solucion mejorSol = orquestarHCMMConRestart(
            instancia, S, estado,
            MAX_RESTARTS, MAX_ITERACIONES_POR_HC, SEMILLA_BASE
        );

        auto despues = chrono::steady_clock::now();
        auto duracion = chrono::duration<double>(despues - antes);

        int eval = f_eval(instancia, mejorSol);

        // Guardar resultado
        guardarSolucion(
            mejorSol, "./OUTPUTS", nombre,
            eval, eval_inicial, instancia, duracion.count(),
            MAX_RESTARTS, MAX_ITERACIONES_POR_HC
        );

    }

    cout << "\nFin\n";

    return 0;
}



/*
int main() {
    string nombre = "comp06.ctt";

    Instancia instancia;
    string arch = rutaInstancias + nombre;
    cout<<"Iniciando lectura de:  "<<nombre<<"\n";
    //Lectura de la instancia
    if (!extraerInstancia(arch, instancia)) {
        cerr << "Error al leer la instancia." << endl;
        return 1;
    }
    cout<<"Instancia extraida\n";
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
        cout<<"\n\nSin solución inicial para "<< arch<<"\n";
        return -1;
    }

    cout<<"\n\nSolucion encontrada para "<< arch<<"\n";
    int eval_inicial = f_eval(instancia, S);
    
    // Desde aquí es lógica de HC
    EstadoRapido estado = construirEstadoDesdeSolucion(instancia, S);
    const int MAX_RESTARTS = 10;
    const int MAX_ITERACIONES_POR_HC = 100;
    const unsigned int SEMILLA_BASE = 42u;
    cout << "DESDE AQUI HC\n" << flush;


    auto antes = chrono::steady_clock::now();
    Solucion mejorSol = orquestarHCMMConRestart(
        instancia, S, estado, MAX_RESTARTS, MAX_ITERACIONES_POR_HC, SEMILLA_BASE
    );
    auto despues = chrono::steady_clock::now();
    auto duracion = chrono::duration<double>(despues - antes);

    int eval = f_eval(instancia, mejorSol);

    guardarSolucion(S, "./OUTPUTS", nombre, eval, eval_inicial, instancia, duracion.count());
    
    cout <<"\n\nFin\n\n";
    return 0;
}


    
*/
