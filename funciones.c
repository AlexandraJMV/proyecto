#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "hashmap.h"
#include "list.h"
#include "tdas/treemap.h"
#include "funciones.h"

#define MAXCHAR 300
#define MAXCOURSES 100

typedef struct {
    char NomCarrera[MAXCHAR];
    List *Ramos;
}Carrera;

typedef struct {
    char Nombre[MAXCHAR];
    char Contrasena[MAXCHAR];
    char Periodo[MAXCHAR];
    Carrera * Carrera;
    List *Cursos;
}Estudiante;

typedef struct {
    char IDcurso[MAXCHAR]; // considerar eliminar
    char NomCurso[MAXCHAR];
    char Periodo[MAXCHAR];
    char AreaCurso[MAXCHAR];
    char EstadoAprovacion[MAXCHAR];
    float *NotaEximicion; 
    float *NotaAprobacion;
    List *Evaluaciones;
}Curso;
/*
typedef struct {
    char Identificador[MAXCHAR];
    float *valor;
    float *pctjPresentacion;
}Evaluacion;
*/
typedef struct{
    char IDCurso[MAXCHAR];
    Curso * InfoCurso;
    List* Requisitos;
} Node;

/*  funciones */

int toselect(char * str){ /* LISTO */
    for(long i=0 ; str[i] != '\0' ; i++)
    {
        if(str[i]=='\n') break;
        if(str[i]<'1' || str[i]>'9')
            return 0;
    }
    return atoi(str);
}

void clean(void) { /* LISTO */
    char c;
    while((c = getchar()) != '\n' && c != EOF){}
}

const char *get_csv_field (char * tmp, int k) { /* LISTO */
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

Curso * createCourse(void){ /* LISTO */
    Curso * course = (Curso*)malloc(sizeof(Curso));
    if(course == NULL){
        perror("Error al reservar memoria para curso ");
        exit(1);
    }
    course->Evaluaciones = createList();
    return course;
}

Node * create_graphNode(){ /* LISTO */
    Node * nodo = (Node*)malloc(sizeof(Node));
    if (nodo == NULL){
        perror("Error al crear nodo para grafos. ");
        exit(1);
    }
    nodo->Requisitos = createList();
    nodo->InfoCurso = createCourse();
    return nodo;
}

void to_minusc(char * str){ /* LISTO */

    for(long i=0 ; str[i] != '\0' ; i++){
        str[i] = tolower(str[i]);
    }

    return;
}

void set_requisitos(const char *requstr, Node * node, HashMap * grafoCursos){ /* LISTO */
    int i = 0;
    while(1){
        // Obtencion de requisitos y control de ciclo
        const char * campo = get_csv_field((char*)requstr, i);
        if (campo==NULL) break;
        i++;

        // Agregar requisito a lista de requisitos por nodo.
        // Verificar que nodo exista en grafo. Si existe se adjunta el nodo. 
        // Si no, se crea el nodo y se inserta (Solamente contiene el ID)
        Pair * hashPair = searchMap(grafoCursos, (char*)campo);
        if(hashPair){
            pushBack(node->Requisitos, hashPair->value);
        }
        else{
            Curso * course = createCourse();
            Node * newNodo = create_graphNode();

            strcpy(course->IDcurso, campo);
            strcpy(newNodo->IDCurso, campo);
            newNodo->InfoCurso = course;

            pushBack(node->Requisitos, newNodo);
        } 
    }
    return;
}

// importar cursos crea y retorna un grafo con cursos y sus dependencias.
HashMap * import_courses(void){ /* LISTO */
    FILE * entrada = NULL;
    char str[MAXCHAR];

    HashMap * grafo = createMap(MAXCOURSES);

    entrada = fopen("archivos/cursos.csv", "rt");
    if (entrada==NULL){
        perror("No se ha podido abrir el archivo, error fatal! :c\n");
        exit(1);
    }

    while(fgets(str, MAXCHAR, entrada) != NULL){

        Curso * course = createCourse();
        Node * nodo = create_graphNode();

        // Guardar informacion segun campo leido en archivo.
        // Es necesario buscar que el nodo no exista con anterioridad en el grafo.
        // Si no existe, se crea un nuevo nodo.
        // Si existe, se rellenan los campos que no tengan informacion.

        for (int i=0 ; i<5 ; i++){
            Pair * hashPair;
            Node * graphNode;

            const char * campo = get_csv_field(str, i);
            switch(i)
            {
                case 0:
                    hashPair = searchMap(grafo, (char*)campo);
                    if(hashPair){
                        // Se debe acceder al curso contenido en el nodo.
                        graphNode = hashPair->value;
                        course = graphNode->InfoCurso;
                    }
                    else{
                        strcpy(course->IDcurso, campo);
                        strcpy(nodo->IDCurso, campo);
                    }
                    break;
                case 1:
                    strcpy(course->NomCurso, campo);
                    break;
                case 2:
                    strcpy(course->Periodo, campo);
                    break;
                case 3:
                    set_requisitos(campo, nodo, grafo);
                    break;
                case 4:
                    strcpy(course->AreaCurso, campo);
                    break;
            }
        }

        // Insertar nodo en grafo.
        nodo->InfoCurso = course;
        insertMap(grafo, _strdup(nodo->IDCurso), nodo);
    }


    
    return grafo;
}

Carrera * createCarrera(void){/* LISTO */
    Carrera * new = (Carrera *)malloc(sizeof(Carrera));
    if(new == NULL)
    {
        perror("No sepudo reservar memoria para carrera ");
        exit(1);
    }
    new->Ramos = createList();
    return new;
}

Carrera * copiarCarrera(Carrera * car){/* LISTO */
    Carrera * copy = createCarrera();
    strcpy(copy->NomCarrera, car->NomCarrera);
    copy->Ramos = car->Ramos;
    car->Ramos = NULL;

    return copy;
}

void insertCourses(char * str, Carrera * car, HashMap * courses){ /* LISTO */
    int i = 0, aux = 0;
    char * c;

    while(1){
        const char * campo = get_csv_field(str, i);
        if (campo==NULL) break;
        i++;

        char * final = _strdup(campo);
        if( (c = strstr(final, "\n")) )
            final[c-final] = '\0';
            
        Pair * hashPair = searchMap(courses, final);
        if(hashPair){
            Node * nodo = hashPair->value;
             if(nodo){
                pushBack(car->Ramos, nodo->InfoCurso);
             }
        }        
    }
}

void mostrarCarreras(List * list){ /* Test */
    Carrera * car = (Carrera*) firstList(list);
    int cont = 0;
    while(car!=NULL){
        printf("%s\n", car->NomCarrera);

        Curso * cur = (Curso *) firstList(car->Ramos);
        while(cur!=NULL){
            cont++;
            printf("%d %s\n", cont, cur->IDcurso);
            cur =(Curso*)nextList(car->Ramos);
        }
        printf("\n");

        car=(Carrera*)nextList(list);
        
    }
    printf("\n%d\n", cont);
}

List * import_carreras(HashMap * courses){/* LISTO */
    FILE * entrada = NULL;
    List * carreras;
    Carrera * aux_carrera;

    int start = 0, end = 0;
    char str[MAXCHAR];

    entrada = fopen("archivos/carreras.txt", "rt");
    if (entrada==NULL){
        perror("No se ha podido abrir el archivo, error fatal! :c\n");
        exit(1);
    }

    carreras = createList();
    aux_carrera = createCarrera();

    while(fgets(str, MAXCHAR, entrada) != NULL){
        // Lectura linea a linea
        char * c;

        // Copia la linea que contiene el nombre de la carrera, sin el fragmento
        // "CARRERA:" en caso de encontrarse.
        if ( ( c = strstr(str, "CARRERA:")) != NULL ){
            strcpy(aux_carrera->NomCarrera, str+(c-str)+strlen("CARRERA:"));
            printf("leido %s\n", aux_carrera->NomCarrera);
        }        
        if ( (c = strstr(str, "CURSOS:")) != NULL ){
            start = 1;
        }
        if ( (c = strstr(str, "FIN CURSOS")) != NULL ){
            start = 0;
            end = 1;
        }
        if(start == 1){
            insertCourses(str, aux_carrera, courses);
        }
        if (end == 1 && start == 0){
            pushBack(carreras, copiarCarrera(aux_carrera));
            aux_carrera->Ramos = createList();
            end = 0;
        }
    }

    return carreras;
}

void mostrarCurso(HashMap * g){ /* Test */
    int i = 1;
    Pair * par = firstMap(g);
    while(par != NULL){
        Node * nodo = par->value;
        Curso * course = nodo->InfoCurso;

        printf("%d ID curso : %s\n", i, course->IDcurso);
        printf("Nombre curso : %s\n", course->NomCurso);
        printf("Periodo : %s\n", course->Periodo);

        printf("Requisitos: ");

        Node * req = (Node*) firstList(nodo->Requisitos);
        while(req != NULL){
            printf("%s ,", req->IDCurso);
            req = (Node*) nextList(nodo->Requisitos);
        }
        printf("\n\n");
        i++;
        par = nextMap(g);
    }
}

int is_validuser(char * str){ /* LISTO */
    int i;
    for(i=0 ; str[i] != '\0' && str[i] != '\n'; i++){
        if(isalpha(str[i]) == 0 && str[i] != ' ')
            return 0;
    }
    if (i<8)
        return 0;
    return 1;
}

int set_username(char * user){ /* LISTO */
    int intentos = 0;
    do{
        system("cls");
        printf( "====================================================\n"
        "En primer lugar, establezca un nombre de usuario.\n\n"
        
        "Tenga en mente que su nombre de usuario SOLO puede tener contener letras, "
        "por lo que caracteres especiales y numeros no seran aceptados.\n\n"

        "Tambien necesita un minimo de 8 caracteres para ser valido.\n"
        "====================================================\n"
        "\n\nNombre de usuario: ");

        char user_imput[MAXCHAR];

        fgets(user_imput, MAXCHAR, stdin);
        clean();

        if (intentos > 2){
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
                printf("Wow! Ha realizado varios intentos!\n"
                       "Si prefiere volver al menu principal puede ingresar cualquier numero en su siguiente intento.\n\n");
            }

            printf("Este nombre de usuario no es valido! :(\n"
                   "Por favor, intentelo nuevamente.\n\n");
            getchar();
        }
    }while(1);

    return 0;
}

Estudiante * create_student(){ /* LISTO */
    Estudiante * newEst = (Estudiante*)malloc(sizeof(Estudiante));
    if(newEst==NULL){
        perror("No se pudo reservar memoria para nuevo estudiante ");
        exit(1);
    }
    newEst->Cursos = createList();
    return newEst;
}

int set_career(List * cars, Estudiante * user){
    int cont = 0, select;
    char user_input[MAXCHAR];

    printf( "==========================================\n"
            "A continuacion se le presentaran las carreras disponibles en \n"
            "dentro del navegador. Por favor, elija aquella a la que pertenezca.\n"

            "\nPuede ingresar cualquier numero no valido para cancelar la operacion \n"
            "y volver al menu principal.\n"
            "==========================================\n\n"

            "++++++++++++++++ Carreras disponibles ++++++++++++++++\n");
    

    Carrera * recCar = (Carrera*) firstList(cars);
    while(recCar!=NULL){
        cont++;
        printf( "------------------------------------------------------+\n"
                "|%-2d-.|%-20s|\n", cont, recCar->NomCarrera);
        
        recCar = (Carrera*) nextList(cars);
    }
    printf("------------------------------------------------------+\n");
    
    fgets(user_input, MAXCHAR, stdin);
    select = toselect(user_input);

    if(select > cont || select<cont){
        system("cls");
        printf("\n Volviendo al menu principal...");
        clean();
        return 0;
    }
    else{
        printf("^_^)b\n");
    }

    clean();
    return 1;
}

void formulario(List * careers){

    char selec[10];
    char user_name[MAXCHAR], user_career[MAXCHAR]/*, user_period[10]*/;
    Estudiante * new_user;

    new_user = create_student();

    system("cls");
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
           "Bienvenido estudiante!\n"
           "Muchas gracias por preferir nuestro navegador academico.\n\n"

           "A continuacion se le realizaran una serie de preguntas con tal de establecer\n"
           "su nueva cuenta en el sistema. Se le solicita que siga las instrucciones de forma\n"
           "responsable.\n\nDesea continuar? Escriba y para confirmar.\n" 
           "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

    fgets(selec, 10, stdin);
    to_minusc(selec);
    clean();

    system("cls");

    if (strcmp(selec, "y\n") != 0){
        printf("%s", selec);
        printf( "Es una pena! :(\n"
                "Volviendo al menu principal...");
    }
    else{
        // Formulario por partes por orden. retorno de 0 significa que se ha cancelado el registro del usuario y se 
        // vuelve al menu principal
        if(set_username(user_name))
            strcpy(new_user->Nombre, user_name);
        else
            return;
        if(set_career(careers, new_user) == 0)
            return;

        /*
        if(set_career(user_career) == 0)
            return;

        if(set_period(user_period) == 0)
            return;
        
        if(modify_courses(user_career) == 0)
            return;
        
        if(set_horario(user_career) == 0)
            return;
        
        set_password();
        */

        system("cls");
        printf( "Gracias por crear una cuenta! Por favor, disfrute de las funcionalidades "
                "que le ofrecemos!\n\n");
        clean();
    }
    return;
}

/* fin funciones */