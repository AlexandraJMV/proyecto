#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED
#include "hashmap.h"
#include "list.h"

#define MAXCHAR 300
#define MAXCOURSES 100
#define TALLAHORARIO 14

typedef struct Estudiante{
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
Estudiante * import_infoUsuario(char * nombre, HashMap * courses);

void insert_archUsuarios(Estudiante * user);

void export_infoUsuarios(List * users);

Estudiante * formulario(List * careers, HashMap * cursos, List * users);

void clean(void);

int toselect(char * str);

HashMap * import_courses(void);

List * import_carreras(HashMap * courses);

void mostrarCurso(HashMap * g);

void Buscar_Us(char *t, Estudiante * user);

void Mostrar_datos(Estudiante * user);

List * import_allUsers(HashMap * courses);

Estudiante *  comprobar_user(List * usuarios, char * user_imput);

int comprobar_Formato(char * imput);

void iniciar_sesion(List * usuarios, HashMap * cursos, List * carreras);

void utilidad_horario(Estudiante * user);

#endif