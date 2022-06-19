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
#define TALLAHORARIO 15

typedef struct {
    char NomCarrera[MAXCHAR];
    List *Ramos;
}Carrera;

typedef struct {
    char Nombre[MAXCHAR];
    char Contrasena[MAXCHAR];
    char Periodo[MAXCHAR];
    char Carrera[MAXCHAR];
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

typedef struct{
    char actividad[MAXCHAR];
    int modificable;
    int ocupado;
}Bloque;

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
        char * p = strstr(str, "\n");
        if (p) str[p-str] = '\0';

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
        printf( "+=================================================================================+\n"
                "| En primer lugar, establezca un nombre de usuario.                               |\n"
                "|                                                                                 |\n"
                "| Tenga en mente que su nombre de usuario SOLO puede tener contener letras, por   |\n"
                "| lo que caracteres especiales y numeros no seran aceptados.                      |\n"
                "|                                                                                 |\n"
                "| Tambien necesita un minimo de 8 caracteres para ser valido.                     |\n"
                "+=================================================================================+\n\n"
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

void * posList(List * list, int pos){/* LISTO */
    int cont=0;
    void* data = firstList(list);
    while (data!=NULL)
    {
        cont++;
        if(cont==pos){
            return data;
        }
        data = nextList(list);
    }
    return NULL;
}

int set_career(List * cars, Estudiante * user){/* LISTO */
    int cont = 0, select;
    char user_input[MAXCHAR];

    system("cls");
    printf( "+=================================================================================+\n"
            "| A continuacion se le presentaran las carreras disponibles dentro del navegador. |\n"
            "| Por favor, elija aquella a la que pertenezca.                                   |\n"
            "|                                                                                 |\n"
            "| Puede ingresar cualquier numero no valido para cancelar la operacion            |\n"
            "| y volver al menu principal.                                                     |\n"
            "+=================================================================================+\n\n\n"

            "-----------------------< Carreras disponibles >-----------------------\n");
    

    Carrera * recCar = (Carrera*) firstList(cars);
    while(recCar!=NULL){
        cont++;
        printf("|                                                                    |\n");
        printf("|%-2d-. %62s |\n", cont, recCar->NomCarrera);
        
        recCar = (Carrera*) nextList(cars);
    }
    printf("----------------------------------------------------------------------\n");
    
    fgets(user_input, MAXCHAR, stdin);
    select = toselect(user_input);

    if(select > cont || select==0){
        system("cls");
        printf("\n Volviendo al menu principal...");
        clean();
        return 0;
    }
    else{
        Carrera * auxcar = (Carrera*) posList(cars, select);
        if(auxcar)
            strcpy(user->Carrera, auxcar->NomCarrera);
    }
    clean();
    return 1;
}

int comprobarMaxPeriodo(char * str, List * carr){/* LISTO */
    int max = 0, n;
    Carrera * auxCar = (Carrera*)firstList(carr);
    while (auxCar!=NULL)
    {
        if(strstr(auxCar->NomCarrera, str))
            break;
        auxCar = (Carrera*)nextList(carr);
    }

    if (auxCar == NULL) return 0;

    Curso * course = (Curso*)firstList(auxCar->Ramos);
    while(course!=NULL){
        if((n = toselect(course->Periodo))>max)
            max = n;
        course=(Curso*)nextList(auxCar->Ramos);
    }

    return max;
}

int set_period(Estudiante * user, List * carreras){ /* LISTO */
    char user_imput[MAXCHAR];
    int comprobar_num, max_period;
    char * p;
    system("cls");
    printf( "===================================================\n"
            "A continuacion, seleccione el periodo (semestre) al cual pertenece.\n"
            "Por favor, respete este formato:\n"
            
            "\n            <numero periodo>\n\n"

            "En caso de ingresarse un numero no valido, se le mandara al menu de inicio.\n"
            "Nota: no ingrese los simbolos < o >\n"
            "===================================================\n");
    
    fgets(user_imput, MAXCHAR, stdin);
    p = strstr(user_imput, "\n");
    if(p)
        user_imput[p-user_imput] = '\0';
    
    comprobar_num = toselect(user_imput);
    max_period = comprobarMaxPeriodo(user->Carrera, carreras);

    if (comprobar_num>max_period && max_period != 0){
        system("cls");
        printf( "El periodio que ha ingresado no es valido!\n"
                "Volviendo al menu principal...");
        clean();
        return 0;
    } 
    else if(comprobar_num != 0)
        strcpy(user->Periodo, user_imput);
    else{
        system("cls");
        printf("Volviendo al menu principal...");
        clean();
        return 0;
    }
    
    clean();
    return 1;
}

List * fromPeriod(Estudiante * st, List * careers){ /* LISTO */
    List * retList = createList();

    Carrera * auxCar = (Carrera*)firstList(careers);
    while (auxCar!=NULL)
    {
        if(strstr(auxCar->NomCarrera, st->Carrera))
            break;
        auxCar = (Carrera*)nextList(careers);
    }

    if(auxCar==NULL){
        printf("PROBLEMA AL BUSCAR CARRERA EN FROMPERIOD\n");
        exit(1);
    }

    Curso * course = (Curso*)firstList(auxCar->Ramos);
    while(course!=NULL){
        if(strstr(course->Periodo, st->Periodo)){
            printf("%s\n", course->IDcurso);
            pushBack(retList, course);
        }
        course=(Curso*)nextList(auxCar->Ramos);
    }
    getchar();
    return retList;
}
/*
int modify_courses(Estudiante * student, List * careers){  EN PROGRESO
    char user_input[MAXCHAR];
    int num_imput;

    system("cls");
    printf( "+==============================================================+\n"
            "| A continuacion, se le preguntara si esta cursando los cursos |\n"
            "| pertenecientes a su periodo.                                 |\n"
            "|                                                              |\n"
            "| Para aquellos cursos que no este cursando, se le preguntara  |\n"
            "| por aquellos cursos que le preceden.                         |\n"
            "|                                                              |\n"
            "| > Ingrese cualquier numero distinto de 0 para continuar.     |\n"
            "+==============================================================+\n\n\n");
    
    fgets(user_input, MAXCHAR, stdin);
    num_imput = toselect(user_input);

    if(num_imput == 0){
        printf( "No ha ingresado un numero!\n"
                "Se le devolvera al menu principal.");
        clean();
        return 0;
    }

    int cont = 1;

    List * courses  = fromPeriod(student, careers);

    Curso * course = firstList(courses);
    while(course != NULL){
        // CONFIRMAR.AGREGAR A LISTA COURSES LOS REQUISITOS SI
        // NO SE ESTA CURSANDO UN CURSO DEL PERIODO DEL ESTUDIANTE.
        if(verifyCourse(course))
            continue;
        else
            insertRequisitos(course, courses);
        course = (Curso*)nextList(courses);
    }

    // Agregar lista de cursos resultantes al estudiante.
    student->Cursos = courses;
    courses = NULL;
    free(courses);

    clean();
    return 1;
} */

void set_variablesHorario(Bloque ** arreglo){  /* LISTO */
 
    for(int i = 0 ; i<TALLAHORARIO ;i++){
        arreglo[i]->modificable = 1;
        arreglo[i]->ocupado = 0;
        strcpy(arreglo[i]->actividad, "HOLA?");
    }
    return;
}

Bloque ** reservMemBloques(Bloque ** arreglo){
    Bloque ** aux =(Bloque**)malloc(sizeof(Bloque*)*TALLAHORARIO);
    if(aux==NULL) {
        printf("Error\n");
        exit(1);
    }
    for(int i = 0; i<TALLAHORARIO ; i++){
        aux[i] = (Bloque*)malloc(sizeof(Bloque));
        if(aux[i]==NULL){
            printf("error\n");
            exit(1);
        } 
    } 

    return aux;
}

int printDiaHorario(char * day, HashMap * horario){ /* LISTO ??????????????????*/
    to_minusc(day);

    Pair * hashPair = searchMap(horario, day);
    if(hashPair){
        day[0] = toupper(day[0]);
        Bloque ** bloques = hashPair->value;

        printf( "+--------------------+\n"
                "|%20s|\n"
                "+--------------------+\n", day);
        
        for(int i=0 ; i<TALLAHORARIO ; i++){
            printf("%-2d- ", i+1);
            if(bloques[i]->ocupado==1)
                printf( "|%-20s|\n", bloques[i]->actividad);
            else
                printf("|     libre     |\n");
        }
        printf("----------------------\n");
    }
    else{
        return 0;
    }
    return 1;
}

HashMap * createHorario(){  /* LISTO */
    HashMap * horario = createMap(20);
    char dias[MAXCHAR]= "Lunes,Martes,Miercoles,Jueves,Viernes,Sabado,Domingo";

    for(int i = 0 ; i<7 ; i ++)
    {
        Bloque ** bloquesEnDia = reservMemBloques(bloquesEnDia);
        
        set_variablesHorario(bloquesEnDia);

        const char * campo = get_csv_field(dias, i);
        to_minusc((char*)campo);
        insertMap(horario, _strdup(campo), bloquesEnDia);

        printDiaHorario((char*)campo, horario);
    }
    return horario;
}

Pair ** reservMemPares(int talla){  /* LISTO */
    Pair ** aux =(Pair**)malloc(sizeof(Pair*)*TALLAHORARIO);

    for(int i = 0; i<TALLAHORARIO ; i++){
        aux[i] = (Pair*)malloc(sizeof(Pair));
    } 

    return aux;
}

void printSemanaHorario(HashMap * horario){ /* LISTO */
    char dias[MAXCHAR]= "Lunes,Martes,Miercoles,Jueves,Viernes,Sabado,Domingo";
    Pair ** pares = reservMemPares(7);

    for(int i = 0; i<TALLAHORARIO+1 ; i++)
    {
        if (i==0){
            printf("dias");
            for(int k=0 ; k<7 ; k++){
                const char * campo = get_csv_field(dias, k);
                printf("|%9s", campo);

                to_minusc((char*)campo);
                Pair * par = searchMap(horario, (char*)campo);
                if(par == NULL){
                    printf("centinela\n");
                    exit(1);
                }
                pares[k] = par;
            }
            printf("|\n");           
        }
        else{
            printf("%2d-.", i);
            for(int j=0 ; j<7 ; j++){
                
                Pair * aux = pares[j];
                Bloque ** bloq = aux->value;
                if(bloq[i-1]->ocupado == 1)
                    printf("| ocupado ");
                else
                    printf("|  libre  ");
            }
            printf("|\n");
        }
    }
    printf("\n");
}

void verificarModificarBloque(char * day, HashMap * horario, int bloque){
    char user_imput[MAXCHAR];
    int verif;

    system("cls");
    to_minusc(day);
    Pair * hashPair = searchMap(horario, day);
    if(hashPair==NULL){
        printf("Error enn verificar bloque\n");
        exit(1);
    }

    Bloque ** bloques = hashPair->value;

    if(bloques[bloque-1]->ocupado == 1){
        printf( "El bloque que usted ingreso se encuentra ocupado por la actividad: \n"
                "%s.\n", bloques[bloque-1]->actividad);
        printf("\nDesea sobreescribir este bloque? Ingrese cualquier numero para confirmar. ");

        fgets(user_imput, MAXCHAR, stdin);
        clean();
        verif = toselect(user_imput);

        if (verif==0){
            printf("\n\nOperancion cancelada.\nVolviendo a menu anterior....");
            return;
        }
        else{
            system("cls");
            printf("Por favor, ingrese la actividad que quiere isertar en este bloque.");

            fgets(user_imput, MAXCHAR, stdin);
            char * c = strstr(user_imput, "\n");
            if(c) user_imput[c-user_imput] = '\0';
            clean();

            strcpy(bloques[bloque-1]->actividad, user_imput);
            printf("Su actividad ha sido insertada de forma exitosa.\nVolviendo a menu principal...");
            return;
        }
    }
    else{
        printf("Por favor, ingrese la actividad que quiere isertar en este bloque.");
        fgets(user_imput, MAXCHAR, stdin);
        char * c = strstr(user_imput, "\n");
        if(c) user_imput[c-user_imput] = '\0';
        clean();

        strcpy(bloques[bloque-1]->actividad, user_imput);
        bloques[bloque-1]->ocupado = 1;
        printf("Su actividad ha sido insertada de forma exitosa.\nVolviendo a menu principal...");
        return;
    }
}
// El horario sera un hashmap cuyas claves son los DIAS
// el valor asociado a la clave puede ser un arreglo de un nuevo tipo de dato llamado
// BLOQUE. que corresponde aun bloque horario y posee una actividad y un bool que indica
// si ese bloque se encuentra utilizado, y otro para indicar si no es modificable.
int set_horario(Estudiante * user){
    char user_imput[MAXCHAR];
    HashMap * horario = createHorario();

    system("cls");
    printf("+=================================================================================+\n"
           "| Ya casi ha finalizado!                                                          |\n"
           "| Ahora solo necesita establecer su horario para que empiece a utilizar el        |\n"
           "| navegador academico.                                                            |\n"
           "|                                                                                 |\n"
           "| Las instrucciones son las siguientes:                                           |\n"
           "| Se le va a mostrar un calendario que respresenta la semana. Usted debe          |\n"
           "| seleccionar un dia y un bloque horario para insertar un curso.                  |\n"
           "|                                                                                 |\n"
           "| Para finalizar el establecimiento del horario, ingrese la cadena \"fin\"          |\n"
           "| Si desea cancelar la operacion en cualquier instancia, ingrese un 0.            |\n"
           "|                                                                                 |\n"
           "|                                      > Ingrese cualquier numero para continuar. |\n"
           "+=================================================================================+\n");

    fgets(user_imput, MAXCHAR, stdin);
    char * p = strstr(user_imput, "\n");
    if(p) user_imput[p-user_imput] = '\0';
    clean();

    if(toselect(user_imput)==0){
        printf("Operacion cancelada.\nVolviendo al menu principal...");
        clean();
        return 0;
    }
    
    while (1){
        system("cls");
        printSemanaHorario(horario);

        printf("Ingrese el nombre de un dia para modificar ese fragento de horario :");
        fgets(user_imput, MAXCHAR, stdin);
        char * c = strstr(user_imput, "\n");
        if(c) user_imput[c-user_imput] = '\0';
        clean();

        if(strcmp(user_imput, "fin")==0)
        {
            printf("Ha terminado con su horario!");
            break;
        }
        else if(strcmp(user_imput, "0")==0)
        {
            printf("Se ha cancelado la operacion.\nVolviendo al menu principal...\n");
            getchar();
            return 0;
        }
        else{
            int bloque;
            system("cls");
            if (printDiaHorario(user_imput, horario)==0){
                printf("No se econtro el dia en la impresion de dias!\n");
                getchar();
            }
            else{
                char copyDia[50];
                strcpy(copyDia, user_imput);

                printf("Seleccione algun bloque para ingresar/borrar alguna actividad: ");

                fgets(user_imput, MAXCHAR, stdin);
                char * c = strstr(user_imput, "\n");
                if(c) user_imput[c-user_imput] = '\0';

                bloque = toselect(user_imput);
                if (bloque==0 || bloque > TALLAHORARIO){
                    printf("\nEl bloque ingresado no es valido.\nVolviendo a horario semanal...");
                }
                else 
                    verificarModificarBloque(copyDia, horario, bloque);
                clean();
            }
        }
    }
    getchar();
    return 1;
}

void formulario(List * careers){ /* EN PROGRESO */
    Estudiante * new_user;
    char selec[10];
    char user_name[MAXCHAR];
    int confirm;
    
    new_user = create_student();

    system("cls");
    printf("+=================================================================================+\n"
           "| Bienvenido estudiante!                                                          |\n"
           "| Muchas gracias por preferir nuestro navegador academico.                        |\n"
           "|                                                                                 |\n"
           "| A continuacion se le realizaran una serie de preguntas con tal de establecer    |\n"
           "| su nueva cuenta en el sistema. Se le solicita que siga las instrucciones de     |\n"
           "| forma responsable.                                                              |\n"
           "|                                                                                 |\n"
           "| Desea continuar? Escriba cualquier numero para confirmar.                       |\n" 
           "+=================================================================================+\n");

    fgets(selec, 10, stdin);
    confirm = toselect(selec);
    clean();

    system("cls");

    if (confirm == 0){
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
        if(set_period(new_user, careers) == 0)
            return;
       /* if(modify_courses(new_user, careers) == 0)
            return;*/
        if(set_horario(new_user) == 0)
            return;
        
        //set_password();
        

        system("cls");
        printf( "Gracias por crear una cuenta! Por favor, disfrute de las funcionalidades "
                "que le ofrecemos!\n\n");
        clean();
    }
    return;
}

/* fin funciones */