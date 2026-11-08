#include "lectura.h"
#include "greedy.h"
#include "restricciones.h"
#include <iostream>

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


int main() {
    Instancia instancia;

    //Lectura de la instancia
    if (!extraerInstancia("instancias/ejemplo.txt", instancia)) {
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
    
    return 0;
}