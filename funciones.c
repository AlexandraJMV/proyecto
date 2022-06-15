#include <stdlib.h>
#include <stdio.h>

#include "tdas/list.h"
#include "tdas/treemap.h"
#include "funciones.h"

typedef struct {
    char *Nombre;
    char *Contrasena;
    char *Periodo;
    TreeMap *Cursos;
}Estudiante;

typedef struct {
    char *NomCarrera;
    TreeMap *Ramos;
}Carrera;

typedef struct {
    char *NomCurso;
    char *Periodo;
    char *EstadoAprovacion;
    int *NotaEximicion ; 
    int *NotaAprobación;
    List *Evaluaciones;
    List *Requisitos;
}Curso;

typedef struct {
    char *Identificador;
    int *valor;
    int *pctjPresentacion;
}Evaluacion;


/*  funciones */

    // .... //

/* fin funciones */