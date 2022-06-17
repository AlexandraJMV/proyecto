#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "tdas/hashmap.h"
#include "tdas/list.h"
#include "tdas/treemap.h"
#include "funciones.h"

#define MAXCHAR 150
#define MAXCOURSES 100

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

typedef struct{
    char *IDCurso;
    List* Requisitos;
} Node;

/*  funciones */

int toselect(char * str){
    for(int i=0 ; str[i] != '\0' ; i++)
    {
        if(str[i]=='\n') break;
        if(str[i]<'1' || str[i]>'9')
            return 0;
    }
    return atoi(str);
}

void clean(void)
{
    char c;
    while((c = getchar()) != '\n' && c != EOF){}
}

const char *get_csv_field (char * tmp, int k) {
    int open_mark = 0;
    char* ret=(char*) malloc (150*sizeof(char));
    int ini_i=0, i=0;
    int j=0;
    while(tmp[i+1]!='\0'){

        if(tmp[i]== '\"'){
            open_mark = 1-open_mark;
            if(open_mark) ini_i = i+1;
            i++;
            continue;
        }

        if(open_mark || tmp[i]!= ','){
            if(k==j) ret[i-ini_i] = tmp[i];
            i++;
            continue;
        }

        if(tmp[i]== ','){
            if(k==j) {
               ret[i-ini_i] = 0;
               return ret;
            }
            j++; ini_i = i+1;
        }

        i++;
    }

    if(k==j) {

        i = i-1;
       while(tmp[i]!=  '\0')
        {
            ret[i-ini_i] =  tmp[i];
            i++;
        }
       ret[i-ini_i] = 0;
       return ret;
    }

    return NULL;
}


Curso * read_course_line(char * str){
    return NULL;
}

HashMap * import_courses(void){
    FILE * entrada = NULL;
    char str[MAXCHAR];

    HashMap * grafo = createMap(MAXCOURSES);

    entrada = fopen("archivos/cursos", "rt");
    if (entrada==NULL){
        perror("No se ha podido abrirel archivo, error fatal! :c\n");
        exit(1);
    }

    while(fgets(str, MAXCHAR, entrada) != NULL){
        Curso * course = read_course_line(str);
        
        if(course){
            createNode(course);
            insert(grafo, course);
        }
    }

    return grafo;
}

void to_minusc(char * str){

    for(long i=0 ; str[i] != '\0' ; i++){
        str[i] = tolower(str[i]);
    }

    return;
}

int set_username(char * user){

    int intentos = 0;

    printf( "Perfecto! ^-^\n"
            "En primer lugar, establezca un nombre de usuario:\n");
    
    do{
        char user_imput[MAXCHAR];

        fgets(user_imput, MAXCHAR, stdin);
        clean();

        if (intentos > 3){
            if(toselect(user_imput) != 0)
                break;
        }

        if (is_validuser(user_imput)){
            
            strcpy(user, user_imput);
            return 1;
        }
        else{
            intentos++;

            if(intentos > 2){
                printf("Wow! Ha intentado demasiados intentos!\n"
                       "Si prefiere volver al menu principal puede ingresar cualquier numero.\n\n");
            }

            printf("Este nombre de usuario no es valido! :(\n"
                   "Por favor, intentelo nuevamente.\n\n");
        }
    }while(1);

    return 0;
}

void formulario(void){

    char selec[10];
    char user_name[MAXCHAR];

    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
           "Bienvenido estudiante!\n"
           "Muchas gracias por preferir nuestro navegador academico.\n\n"

           "A continuacion se le realizaran una serie de preguntas con tal de establecer\n"
           "su nueva cuenta en el sistema. Se le solicita que siga las instrucciones de forma\n"
           "responsable.\n\n ¿Desea continuar? y/n\n" 
           "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    fgets(selec, 10, stdin);
    to_minusc(selec);
    clean();

    system("cls");

    if (strcmp(selec, "y") != 0){
        
        printf( "Es una pena! :(\n"
                "Volviendo al menu principal...\n");
    }
    else{
        // Formulario por partes por orden. retorno de 0 significa que se ha cancelado el registro del usuario y se 
        // vuelve al menu principal

        if(set_username(user_name) == 0)
            return;
        
        if(set_career(user_carrer) == 0)
            return;

        if(set_period(user_period) == 0)
            return;
        
        if(modify_courses(user_career) == 0)
            return;
        
        if(set_horario(user_carrer) == 0)
            return;
        
        set_password();

        system("cls");
        printf( "Gracias por crear una cuenta! Por favor, disfrute de las funcionalidades "
                "que le ofrecemos! ^u^ )/\n\n");
        clean();
    }

    return;
}

/* fin funciones */