#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED
#include "hashmap.h"
#include "list.h"

#define MAXCHAR 300
#define MAXCOURSES 100
#define TALLAHORARIO 15

typedef struct {
    char Nombre[MAXCHAR];
    char Contrasena[MAXCHAR];
    char Periodo[MAXCHAR];
    char Carrera[MAXCHAR];
    List *Cursos;
    HashMap * Horario;
}Estudiante;

//typedef struct Carrera Carrera;

//typedef struct Curso;

//typedef struct Evaluacion;

Estudiante * formulario(List * careers, HashMap * cursos);
/*----------------------------------------------------------*/
void clean(void);
/*----------------------------------------------------------*/
int toselect(char * str);
/*----------------------------------------------------------*/
HashMap * import_courses(void);
/*----------------------------------------------------------*/
List * import_carreras(HashMap * courses);
/*----------------------------------------------------------*/
void mostrarCurso(HashMap * g);
/*----------------------------------------------------------*/
void Buscar_Us(char *t, Estudiante * user);
/*----------------------------------------------------------*/
void Mostrar_datos(Estudiante * user);
/*----------------------------------------------------------*/
#endif