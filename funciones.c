#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "hashmap.h"
#include "list.h"
#include "funciones.h"

typedef struct Carrera{
    char NomCarrera[MAXCHAR];
    List *Ramos;
}Carrera;

typedef struct Curso{
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
typedef struct Node{
    char IDCurso[MAXCHAR];
    Curso * InfoCurso;
    List* Requisitos;
} Node;

typedef struct Bloque{
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

/*  Guarda memoria para un nuevo curso, inicia variables pertinentes y lo retorna */
Curso * createCourse(void){ /* COMENTADO */
    Curso * course = (Curso*)malloc(sizeof(Curso));
    if(course == NULL){
        perror("Error al reservar memoria para curso ");
        exit(1);
    }
    course->Evaluaciones = createList();
    return course;
}

/*  Guarda memoria para un nuevo nodo, inicia variables pertinentes y lo retorna */
Node * create_graphNode(void){ /* COMENTADO */
    Node * nodo = (Node*)malloc(sizeof(Node));
    if (nodo == NULL){
        perror("Error al crear nodo para grafos. ");
        exit(1);
    }
    nodo->Requisitos = createList();
    nodo->InfoCurso = createCourse();
    return nodo;
}

/* Convierte los caracteres de una cadena a minusculas */
void to_minusc(char * str){ /* COMENTADO */

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
        else if(strcmp(campo, "Sin Prerrequisitos")==0){
            continue;
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

/* Crea y retorna un grafo con cursos y sus dependencias */
HashMap * import_courses(void){ /* Semi COMENTADO */
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
            if(i==0 && strcmp(campo, "Sin Prerrequisitos")==0) break;
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


    fclose(entrada);
    return grafo;
}

/* Guarda memoria para una nueva variable carrera, inicia variables pertinentes y la retorna */
Carrera * createCarrera(void){/* COMENTADO */
    Carrera * new = (Carrera *)malloc(sizeof(Carrera));
    if(new == NULL)
    {
        perror("No sepudo reservar memoria para carrera ");
        exit(1);
    }
    new->Ramos = createList();
    return new;
}

/* Devuelve copia de una carrera */
Carrera * copiarCarrera(Carrera * car){/* COMENTADO */
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
    fclose(entrada);
    return carreras;
}

void mostrarCurso(HashMap * g){ /* Test */
    int i = 1;
    Pair * par = firstMap(g);
    while(par != NULL){
        Node * nodo = par->value;
        Curso * course = nodo->InfoCurso;

        printf("%d ID curso : %s\n", i, nodo->IDCurso);
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

/* Comprueba que el nombre de usuario ingresado cumpla con las condiciones establecidas */
int is_validuser(char * str){ /* COMENTADO */
    int i;
    for(i=0 ; str[i] != '\0' && str[i] != '\n'; i++){
        if(isalpha(str[i]) == 0 && str[i] != ' ')
            return 0;
    }
    if (i<8)
        return 0;
    return 1;
}

/*  Controla interacciones con el usuario con tal de establecer un nombre de usuario 
    Retorna 0 si se cancela la operacion */
int set_username(char * user){ /* COMENTADO */
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
        char * c = strstr(user_imput, "\n");
        if(c) user_imput[c-user_imput] = '\0';
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

/* Reserva memoria para un tipo de dato estudiante, inicia variables correspondientes y lo retorna */
Estudiante * create_student(){ /* COMENTADO */
    Estudiante * newEst = (Estudiante*)malloc(sizeof(Estudiante));
    if(newEst==NULL){
        perror("No se pudo reservar memoria para nuevo estudiante ");
        exit(1);
    }
    newEst->Cursos = createList();
    return newEst;
}

/* Retorna dato en posicion i dentro de una lista */
void * posList(List * list, int pos){/* COMENTADO */
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

/*  Comtrola interacciones con el usuario con tal de establacer carrera a la cual pertenece 
    Retorna 0 si se cancela la operacion */
int set_career(List * cars, Estudiante * user){/* COMENTADO */
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

/* Busca el mayor periodo posible correspondiente a una carrera */
int comprobarMaxPeriodo(char * str, List * carr){/* COMENTADO */
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

/*  Controla interacciones con el usuario con tal de establecer el periodo al cual pertenece 
    Retorna 0 si se cancela la operacion  */
int set_period(Estudiante * user, List * carreras){ /* COMENTADO */
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

/* Retorna una lista con todos los cursos correspondientes a un periodo especifico, de una carrera especifica */
List * fromPeriod(Estudiante * st, List * careers){ /* COMENTADO */
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
            pushBack(retList, course);
        }
        course=(Curso*)nextList(auxCar->Ramos);
    }
    getchar();
    return retList;
}

int verifyCourse(Curso * course){ /* LISTO */
    char user_imput[MAXCHAR];
    int verif;

    system("cls");
    printf( "ID del curso: %s\n"
            "Nombre del curso: %s\n"
            "Periodo al que pertenece: %s\n\n"
            ,course->IDcurso, course->NomCurso, course->Periodo);
    
    printf("Se encuentra realizando este curso?\nIngrese cualquier numero distinto de 0 si la respuesta es si\n");

    fgets(user_imput, MAXCHAR, stdin);
    verif = toselect(user_imput);

    printf("\nGracias por la confirmacion!\n");

    if(verif == 0){
        clean();
        return 0;
    }

    clean();
    return 1;
}

void insertRequisitos(Curso * curso, List * cursos, HashMap * dependencias){
    //Buscar curso en dependencias
    Pair * hashPair = searchMap(dependencias, curso->IDcurso);
    if(hashPair==NULL){
        printf("No se encontro el curso que debio haberse encontrado.");
        exit(1);
    }

    // Obtencion de lista de nodos requisitos.
    Node * nodo = hashPair->value;
    List * requisitos = nodo->Requisitos;

    Node * rec = (Node*)firstList(requisitos);
    while(rec != NULL){
        if(rec->InfoCurso != NULL){
            pushCurrent(cursos, rec->InfoCurso);
        }
        rec = (Node*)nextList(requisitos);
    }
    return;
}

int modify_courses(Estudiante * student, List * careers, HashMap * dependencias){  /*EN PROGRESO*/
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

    Curso * course = (Curso*)firstList(courses);
    while(course != NULL){
        // CONFIRMAR.AGREGAR A LISTA COURSES LOS REQUISITOS SI
        // NO SE ESTA CURSANDO UN CURSO DEL PERIODO DEL ESTUDIANTE.
        if(verifyCourse(course)==0)
            insertRequisitos(course, courses, dependencias);
        course = (Curso*)nextList(courses);
    }

    // Agregar lista de cursos resultantes al estudiante.
    student->Cursos = courses;
    courses = NULL;
    free(courses);

    clean();
    return 1;
} 

/* Inicializa las variables de un arreglo de bloques */
void set_variablesHorario(Bloque ** arreglo){  /* COMENTADO */
 
    for(int i = 0 ; i<TALLAHORARIO ;i++){
        arreglo[i]->modificable = 1;
        arreglo[i]->ocupado = 0;
        strcpy(arreglo[i]->actividad, "HOLA?");
    }
    return;
}

/*  Reserva memoria para arreglo de punteros para datos tipo bloque. Retorna el arreglo */
Bloque ** reservMemBloques(Bloque ** arreglo){ /* COMENTADO */
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

/* Imprime informacion correspondiente a un dia de la semana 
Metodo:
        Buscar el dia de la semana que se quiere mostrar.
        Recorrer el arreglo correspondiente a dia, imprimimiendo la informacion
        correspondiente.
        Retorna 0 si el dia de la semana ingresado no se encuentra.*/
int printDiaHorario(char * day, HashMap * horario){ /* COMENTADO */
    to_minusc(day);

    Pair * hashPair = searchMap(horario, day);
    if(hashPair){
        day[0] = toupper(day[0]);
        Bloque ** bloques = hashPair->value;

        printf( "+-------------------------------+\n"
                "|       %15s          |\n"
                "+-------------------------------+\n", day);
        
        for(int i=0 ; i<TALLAHORARIO ; i++){
            printf("%-2d- ", i+1);
            if(bloques[i]->ocupado==1)
                printf( "|  %-26s|\n", bloques[i]->actividad);
            else
                printf("|           libre            |\n");
        }
        printf("----------------------------------\n");
    }
    else{
        return 0;
    }
    return 1;
}

/*  Crea y retorna hashmap para horario, inserta dias de la semana e inicia variables correspondinetes */
HashMap * createHorario(){  /* COMENTADO */
    HashMap * horario = createMap(20);
    char dias[MAXCHAR]= "Lunes,Martes,Miercoles,Jueves,Viernes,Sabado,Domingo";

    for(int i = 0 ; i<7 ; i ++)
    {
        Bloque ** bloquesEnDia = reservMemBloques(bloquesEnDia);
        
        set_variablesHorario(bloquesEnDia);

        const char * campo = get_csv_field(dias, i);
        to_minusc((char*)campo);
        insertMap(horario, _strdup(campo), bloquesEnDia);

    }
    return horario;
}

/*  Guarda memoria para un arreglo de punteros de pares y lo retorna */
Pair ** reservMemPares(int talla){  /* COMENTADO */
    Pair ** aux =(Pair**)malloc(sizeof(Pair*)*TALLAHORARIO);
    if (aux==NULL){
        perror("Error reserva arreglo de punteros ");
        exit(1);
    }

    for(int i = 0; i<TALLAHORARIO ; i++){
        aux[i] = (Pair*)malloc(sizeof(Pair));
        if(aux[i]==NULL){
            perror("Error reservar memoria para puntero a par ");
            exit(1);
        }
    } 

    return aux;
}

/* Imprime el horario de una semana entera a partir del hashmap horario.
Metodo: 
        La clave del hashmap es el nombre del dia y cada clave tiene asociado un arreglo de bloques.
        El horario se imprime linea por linea. La primera linea contiene los dias de la semana.
        Los dias de la semana son controlados a traves de una cadena auxiliar con los dias (claves)
        presentes en el hashmap.
        Para simular una impresion por columnas se crea un arreglo de pares, el arreglo se rellena
        de forma ordenada a medida que se leen los dias de la cadena auxiliar.
        Para la impresion del resto de lineas simplemente se accede al bloque que corresponda al dia
        (posicion 0 a 6) y numero de linea (numero del bloque).*/    
void printSemanaHorario(HashMap * horario){ /* COMENTADO */
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

void verificarModificarBloque(char * day, HashMap * horario, int bloque){ /* LISTO */
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
        printf( "\nDesea sobreescribir o eliminar este bloque?\n."
                "Ingrese un numero menor o igual a 10 para sobreescribir\n"
                "Ingrese un numero mayor que 10 para eliminar\n");

        fgets(user_imput, MAXCHAR, stdin);
        clean();
        verif = toselect(user_imput);

        if (verif==0){
            printf("\n\nOperancion cancelada.\nVolviendo a menu anterior....");
            return;
        }
        else if(verif<=10){
            bloques[bloque-1]->ocupado = 0;
            printf("Bloque limpiado de forma exitosa!\nVolviendo a horario semanal...");
            return;
        }
        else{
            system("cls");
            printf("Por favor, ingrese la actividad que quiere insertar en este bloque.\n");

            fgets(user_imput, MAXCHAR, stdin);
            char * c = strstr(user_imput, "\n");
            if(c) user_imput[c-user_imput] = '\0';
            clean();

            strcpy(bloques[bloque-1]->actividad, user_imput);
            printf("Su actividad ha sido insertada de forma exitosa.\nVolviendo a horario semanal...");
            return;
        }
    }
    else{
        printf("Por favor, ingrese la actividad que quiere insertar en este bloque.\n");
        fgets(user_imput, MAXCHAR, stdin);
        char * c = strstr(user_imput, "\n");
        if(c) user_imput[c-user_imput] = '\0';
        clean();

        strcpy(bloques[bloque-1]->actividad, user_imput);
        bloques[bloque-1]->ocupado = 1;
        printf("Su actividad ha sido insertada de forma exitosa.\nVolviendo a horario semanal...");
        return;
    }
}

/* Una vez establecido el horario base, recorre cada bloque del horario para indicar que no son modificacles */
void lock_horario(HashMap * horario){ /* COMENTADO */
    char dias[MAXCHAR]= "lunes,martes,miercoles,jueves,viernes,sabado,domingo";

    for(int i=0 ; i<7 ; i++){
        const char * campo = get_csv_field(dias, i);

        Pair * par = searchMap(horario, (char*)campo);
        Bloque ** bloques = par->value;

        for(int k=0; k<TALLAHORARIO ; k++){
            if(bloques[k]->ocupado == 1)
                bloques[k]->modificable = 0;
        }
    }

    return;
}

/*  Controla mensajes e interaciones con el usuario con tal de establecer un horario. 
    Retorna 0 en caso que se cancele la operacion. */
int set_horario(Estudiante * user){ /* COMENTADO */
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
            lock_horario(horario);
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
    user->Horario = horario;
    getchar();
    return 1;
}

/*  Revisa que una contrasena cumpla con las condiciones especificadas
    Retorna 0 si la contrasena no es valida.*/
int is_validContrasena(char * intento){ /* COMENTADO */
    int len;
    for(int i = 0 ; intento[i]!='\0' ; i++){
        if(isalnum(intento[i]) == 0)
            return 0;
    }

    if( (len = strlen(intento))<4 || len>12 )
        return 0;

    return 1;
}

/*  Controla mensajes e interacciones con el usuario para establecer una contrasena. 
    Retorna 0 en caso de que se cancele la operacion. */
int set_password(Estudiante * user){ /* COMENTADO */
    char user_imput[MAXCHAR];
    int intentos = 0;
    int comprobar;
    while(1){
    system("cls");
    printf("+=================================================================================+\n"
           "| Muy bien! Ahora solo falta establecer una contrasena.                           |\n"
           "|                                                                                 |\n"
           "|                                                                                 |\n"
           "| Su contrasena debe estar compuesta por letras y numeros SOLAMENTE.              |\n"
           "| Ademas debe tener entre 4 a 12 caracteres.                                      |\n"
           "+=================================================================================+\n");

        intentos++;
        if(intentos>2){
            printf("Ha hecho numerosos intentos.\n"
                    "Si prefiere cancelar la operacion, puede ingresar un 0\n\n");
        }

        printf("Ingrese su contrasena:");

        fgets(user_imput, MAXCHAR, stdin);
        char * c = strstr(user_imput, "\n");
        if(c) user_imput[c-user_imput] = '\0';
        clean();

        if(intentos>2){
            if (strcmp(user_imput, "0")==0){
                system("cls");
                printf("Ha cancelado la operacion.\nVolviendoo a menu principal...");
                getchar();
                return 0;
            }
        }

        if(is_validContrasena(user_imput)==0){
            printf("La contrasena ingresada no es valida\nPor favor, intente nuevamente.\n");
            getchar();
        }
        else{
            strcpy(user->Contrasena, user_imput);
            printf("\nSu contrasena ha sido establecida con exito!\n");
            getchar();
            return 1;
        }
    }
    return 0;
}

Estudiante * formulario(List * careers, HashMap * cursos){ /* LISTOOO */
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
        return NULL;
    }
    else{
        // Formulario por partes por orden. retorno de 0 significa que se ha cancelado el registro del usuario y se 
        // vuelve al menu principal
        if(set_username(user_name))
            strcpy(new_user->Nombre, user_name);
        else
            return NULL;
        if(set_career(careers, new_user) == 0)
            return NULL;
        if(set_period(new_user, careers) == 0)
            return NULL;
        if(modify_courses(new_user, careers, cursos) == 0)
            return  NULL;
        if(set_horario(new_user) == 0)
            return NULL;
        if (set_password(new_user) == 0)
            return NULL;

        system("cls");
        printf( "Gracias por crear una cuenta! Por favor, disfrute de las funcionalidades "
                "que le ofrecemos!\n\n");
        clean();
    }
    return new_user;
}

void insert_archUsuarios(Estudiante * user){ /* LISTO */
    FILE * entrada;

    entrada = fopen("archivos/usuarios.csv", "at");
    if(entrada == NULL){
        perror("Error al abrir archivo usuarios ");
        exit(1);
    }

    fprintf(entrada, "%s,%s\n", user->Nombre, user->Contrasena);
    fclose(entrada);
    return;
}

void toFormat(char * str){ /* LISTO */
    int len = strlen(str);
    for(int  i=0 ; i<len ; i++){
        if(str[i]==' ') str[i]='_';
    }
}

void write_Courses(FILE * salida, List * courses){ /* LISTO*/
    int coma=0;

    fprintf(salida, "%s\n", "CURSOS");

    Curso * course = (Curso*)firstList(courses);
    while(course != NULL){
        if(coma > 0)
            fprintf(salida, "%s", ",");

        fprintf(salida,"%s",course->IDcurso);

        coma++;
        course = (Curso*)nextList(courses);
    }

    fprintf(salida, "%s\n", "\nFIN CURSOS");

    return;
}

void write_Horario(FILE *salida, HashMap *  horario){/* LISTO */

    char dias[MAXCHAR] = "lunes,martes,miercoles,jueves,viernes,sabado,domingo";
    fprintf(salida, "%s\n", "HORARIO");

    for(int i= 0; i<7 ; i++){
        const char * campo = get_csv_field(dias, i);

        Pair * hashPair = searchMap(horario, (char*)campo);
        if(hashPair==NULL){
            printf("Error en buscar dia en horario cunado se sabe que existe\n");
            exit(1);
        }

        Bloque ** bloques = hashPair->value;
        int coma = 0;
        fprintf(salida, "%s,\"", campo);
        for(int k=0 ; k<TALLAHORARIO; k++){
            Bloque * b = bloques[k];
            if (coma>0)
                fprintf(salida,"%s",",");
            fprintf(salida, "%s,%d,%d", b->actividad, b->modificable, b->ocupado);
            coma++;
        }
        fprintf(salida,"%s", "\"\n");
    }
    fprintf(salida,"%s\n", "FIN HORARIO");

    return;
}

void write_studentInfo(Estudiante *student, FILE * salida){/* En progreso */
        
        for(int campo = 1; campo<7 ; campo++){
            if(campo==1)
                fprintf(salida, "%s\n", student->Nombre);

            if(campo==2)
                fprintf(salida, "%s\n", student->Contrasena);
            
            if(campo==3)
                fprintf(salida, "%s\n", student->Carrera);
            
            if (campo==4)
                fprintf(salida, "%s\n", student->Periodo);
            
            if(campo==5)
                write_Courses(salida, student->Cursos);

            if(campo==6)
                write_Horario(salida, student->Horario);
        }

        fclose(salida);

        return;
}

void export_infoUsuarios(List * users){ /* LISTO */

    Estudiante * student = (Estudiante*)firstList(users);
    while(student != NULL){
        // Nombre del archivo corresponde al nombre del usuario 
        // con los espacios reemplazados por _.
        char path[MAXCHAR] = "archivos/info_usuarios/";
        char auxnom[MAXCHAR];

        strcpy(auxnom, student->Nombre);
        toFormat(auxnom);
        strcat(path, auxnom);
        strcat(path, ".txt");

        FILE * salida = fopen(path, "wt");
        if(salida == NULL){
            perror("No se pudo abrir o crear el archivo ");
            exit(1);
        }

        write_studentInfo(student, salida);

        fclose(salida);

        student = (Estudiante*)nextList(users);
    }

    return;
}

void readInsert_courses(Estudiante * user,  FILE * imput, HashMap * courses){

    char linea[MAXCHAR*2];
    const char * campo;
    

    while(fgets(linea, MAXCHAR*2 , imput) != NULL){
        char * c = strstr(linea,"\n");
        if(c) linea[c-linea] = '\0';

        if(strcmp(linea, "FIN CURSOS")==0) break;

        campo = get_csv_field(linea, 0);
        for(int i=1; campo != NULL ; i++){
            Pair * hashPair = searchMap(courses,(char*) campo);
            if(hashPair==NULL){
                printf("Error busqueda en readInsert\n");
                exit(1);
            }

            Node * nodo = hashPair->value;
            pushBack(user->Cursos, nodo->InfoCurso);
            
            campo = get_csv_field(linea,i);
        }
    }

    return;
}

void readInsertBloques(Bloque ** arrBloques, char * linea){    
    int i = 0, j = 0;

    const char * campo = get_csv_field(linea, i);
    while(campo != NULL){
        if(i%3 == 0)
            strcpy(arrBloques[j]->actividad, (char*)campo);
        if(i%3 == 1)
            arrBloques[j]->modificable = atoi(campo);
        if(i%3 == 2){
            arrBloques[j]->ocupado = atoi(campo);
            j++;
        }
        i++;

        campo = get_csv_field(linea, i);
    }
}

void read_horario(Estudiante * user, FILE * imput){
    char linea[MAXCHAR*2];
    HashMap * horario = createHorario();
    
    while(fgets(linea, MAXCHAR*2 , imput) != NULL){
        Pair * hashPair;
        char *c = strstr(linea, "\n");
        if(c) linea[c-linea] = '\0';

        if(strcmp(linea, "FIN HORARIO")==0) break;

        for(int i=0 ; i<2 ; i++){
            const char * campo = get_csv_field(linea, i);

            if(i==0){
                to_minusc((char*)campo);
                hashPair = searchMap(horario, (char*)campo);
                if(hashPair == NULL){
                    printf("No se ha encontrado archivo que deberia encontrarse :(??");
                    exit(1);
                }
            }
            if(i==1){ 
                readInsertBloques(hashPair->value, (char*)campo);
            }
        }
    }

    user->Horario = horario;
    horario = NULL;
    free(horario);
    return;
}

Estudiante * import_infoUsuario(char * nombre, HashMap * courses){
    FILE * entrada;
    char path[MAXCHAR] = "archivos/info_usuarios/";
    char linea[MAXCHAR*2];
    int campo = 1;
    
    Estudiante * old_student = create_student();
    
    toFormat(nombre);
    strcat(path, nombre);
    strcat(path, ".txt");             
    
    
    entrada = fopen(path, "rt");
    if (entrada == NULL){
        perror("Error path ");
        exit(1);
    }
    
    while(fgets(linea, MAXCHAR*2 , entrada) != NULL){
        
        char * c = strstr(linea,"\n");
        if(c) linea[c-linea] = '\0';

        if(campo==1)
            strcpy(old_student->Nombre, linea);
        
        if(campo==2)
            strcpy(old_student->Contrasena, linea);
        
        if (campo==3)
            strcpy(old_student->Carrera, linea);
        
        if(campo==4)
            strcpy(old_student->Periodo, linea);
        
        if(campo==5)
            readInsert_courses(old_student, entrada, courses);

        if(campo==6)
            read_horario(old_student, entrada);
        campo++;
    }

    fclose(entrada);

    return old_student;
}

/* funcion calculadora notas ponderadas

    desc:   CASO A(NO POSEE TODAS SUS NOTAS)
            -usuario inserta numero de notas
            -usuario inserta promedio de aprobacion 
            -usuario ingresa sus notas y porcentaje de cada nota n veces 
            -se le da al usuario la nota que debe sacarse para aprobar
            
            CASO B(POSEE TODAS SUS NOTAS)
            -usuario inserta numero de notas
            -usuario inserta promedio de aprobacion 
            -usuario ingresa sus notas y porcentaje de cada nota n veces
            -usuario ingresa porcentaje que vale el examen 
*/
void Calculadora_notas(){
    int NNotas;
    int aprob;
    long selec;


    system("cls");
    printf("+=================================================================================+\n"
           "| Bienvenido a la calculadora de notas ponderadas!                                |\n"
           "|                                                                                 |\n"
           "| Selecciona la opcion que necesites:                                             |\n"
           "|                                                                                 |\n"
           "| 1.Si posees todas tu notas en el curso.                                         |\n"
           "| 2.Si no posees todas tus notas, pero deseas saber la que necesitas para aprobar.|\n"
           "|                                                                                 |\n" 
           "+=================================================================================+\n");
    scanf("%ld", &selec);
    clean();
    /*
    switch(selec){
            case 1:

            case 2:
}

/*fin funciones */
