#ifndef F_EVAL_H
#define F_EVAL_H

#include "estructuras.h"

int evaluarS1(const Instancia& instancia, const Solucion& solucion);
int evaluarS2(const Instancia& instancia, const Solucion& solucion);
int evaluarS3(const Instancia& instancia, const Solucion& solucion);
int evaluarS4(const Instancia& instancia, const Solucion& solucion);

int f_eval(const Instancia& instancia, const Solucion& solucion);

#endif