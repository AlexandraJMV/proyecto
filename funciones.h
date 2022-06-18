#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED
#include "hashmap.h"
#include "list.h"

//typedef struct Estudiante;

//typedef struct Carrera Carrera;

//typedef struct Curso;

//typedef struct Evaluacion;

void formulario(List * careers);
void clean(void);
int toselect(char * str);
HashMap * import_courses(void);
List * import_carreras(HashMap * courses);

#endif