#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "hashmap.h"
#include "list.h"
#include "funciones.h"

typedef struct Carrera{
    char NomCarrera[MAXCHARS];
    HashMap *Ramos;
}Carrera;

typedef struct Curso{
    char IDcurso[MAXCHARS]; 
    char NomCurso[MAXCHARS];
    char Periodo[MAXCHARS];
    char AreaCurso[MAXCHARS];
    char EstadoAprovacion[MAXCHARS];
    float *NotaEximicion; 
    float *NotaAprobacion;
    List *Evaluaciones;
}Curso;

typedef struct Node{
    Curso * curso;
    List* Requisitos;
    List * Subsiguientes;
} Node;

typedef struct Bloque{
    char actividad[MAXCHARS];
    int modificable;
    int ocupado;
}Bloque;

/*  funciones */

/* Convierte una cadena a una seleccion numerica. Devuelve 0 si no son numeros */
int toselect(char * str){ 
    for(long i=0 ; str[i] != '\0' ; i++)
    {
        if(str[i]=='\n') break;
        if(str[i]<'1' || str[i]>'9')
            return 0;
    }
    return atoi(str);
}

/* Limpia input buffer para futuras lecturas */
void clean(void) { 
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

/*  Guarda memoria para un nuevo curso, inicia variables pertinentes y lo retorna */
Curso * createCourse(void){ 
    Curso * course = (Curso*)malloc(sizeof(Curso));
    if(course == NULL){
        perror("Error al reservar memoria para curso ");
        exit(1);
    }
    course->Evaluaciones = createList();
    return course;
}

/*  Guarda memoria para un nuevo nodo, inicia variables pertinentes y lo retorna */
Node * create_graphNode(void){ 
    Node * nodo = (Node*)malloc(sizeof(Node));
    if (nodo == NULL){
        perror("Error al crear nodo para grafos. ");
        exit(1);
    }
    nodo->Subsiguientes = createList();
    nodo->Requisitos = createList();
    nodo->curso = createCourse();
    return nodo;
}

/* Convierte los caracteres de una cadena a minusculas */
void to_minusc(char * str){

    for(long i=0 ; str[i] != '\0' ; i++){
        str[i] = tolower(str[i]);
    }

    return;
}

/* Funcion encargada de guardar los cursos y sus requisitos */
void set_requisitos(const char *requstr, Node * node, HashMap * grafoCursos){ /* LISTO */
    int i = 0;
    Node * nodo;
    while(1){
        // Obtencion de requisitos y control de ciclo.
        const char * campo = get_csv_field((char*)requstr, i);
        if (campo==NULL) break;
        i++;

        // Caso sin requisitos.
        if(strcmp(campo, "Sin Prerrequisitos")==0){
            continue;
        }
        // Agregar cadenas de requisitos en lista.
        else{
            pushBack(node->Requisitos, _strdup(campo));

            // Agregar a  cada curso leido como requisito, el curso subsiguiente.
            // Crear nodo para grafo si no existe. Despues se rellenaran los campos.

            Pair * hashPair = searchMap(grafoCursos,(char*) campo);
            if(hashPair){
                nodo = hashPair->value;
                pushBack(nodo->Subsiguientes, _strdup(node->curso->IDcurso));
            }
            else{
                
                nodo = create_graphNode();
                strcpy(nodo->curso->IDcurso, campo);
                pushBack(nodo->Subsiguientes, node->curso->IDcurso);
                insertMap(grafoCursos, _strdup(campo), nodo);
            }
        }
    }

    return;
}

/* Crea y retorna un grafo con cursos y sus dependencias */
HashMap * import_courses(void){ 
    FILE * entrada = NULL;
    char str[MAXCHARS];

    HashMap * grafo = createMap(MAXCOURSES);

    entrada = fopen("archivos/cursos.csv", "rt");
    if (entrada==NULL){
        perror("No se ha podido abrir el archivo, error fatal! :c\n");
        exit(1);
    }

    while(fgets(str, MAXCHARS, entrada) != NULL){

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
                        course = graphNode->curso;
                    }
                    else{
                        strcpy(course->IDcurso, campo);
                        strcpy(nodo->curso->IDcurso, campo);
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
        nodo->curso = course;
        insertMap(grafo, _strdup(nodo->curso->IDcurso), nodo);
    }


    fclose(entrada);
    return grafo;
}

/* Guarda memoria para una nueva variable carrera, inicia variables pertinentes y la retorna */
Carrera * createCarrera(void){
    Carrera * new = (Carrera *)malloc(sizeof(Carrera));
    if(new == NULL)
    {
        perror("No sepudo reservar memoria para carrera ");
        exit(1);
    }
    new->Ramos = createMap(MAXCOURSES);
    
    return new;
}

/* Devuelve copia de una carrera */
Carrera * copiarCarrera(Carrera * car){
    Carrera * copy = createCarrera();
    strcpy(copy->NomCarrera, car->NomCarrera);
    copy->Ramos = car->Ramos;
    car->Ramos = NULL;

    return copy;
}

/*Inserta los cursos pertenecientes a una carrera al mapa de cursos asociado*/
void insertCourses(char * str, Carrera * car, HashMap * courses){ 
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
                insertMap(car->Ramos, _strdup(nodo->curso->IDcurso), nodo->curso);
             }
        }        
    }
}

/*A partir delarchivo carreras.txt, importa las carreras disponibles dentro de la aplicacion*/
List * import_carreras(HashMap * courses){
    FILE * entrada = NULL;
    List * carreras;
    Carrera * aux_carrera;

    int start = 0, end = 0;
    char str[MAXCHARS];

    entrada = fopen("archivos/carreras.txt", "rt");
    if (entrada==NULL){
        perror("No se ha podido abrir el archivo, error fatal! :c\n");
        exit(1);
    }

    carreras = createList();
    aux_carrera = createCarrera();

    while(fgets(str, MAXCHARS, entrada) != NULL){
        // Lectura linea a linea
        char * c;
        char * p = strstr(str, "\n");
        if (p) str[p-str] = '\0';

        // Copia la linea que contiene el nombre de la carrera, sin el fragmento
        // "CARRERA:" en caso de encontrarse.
        if ( ( c = strstr(str, "CARRERA:")) != NULL ){
            strcpy(aux_carrera->NomCarrera, str+(c-str)+strlen("CARRERA:"));
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
            aux_carrera->Ramos = createMap(MAXCOURSES);
            end = 0;
        }
    }
    fclose(entrada);
    return carreras;
}

/* Comprueba que el nombre de usuario ingresado cumpla con las condiciones establecidas */
int is_validuser(char * str){ 
    int i;
    for(i=0 ; str[i] != '\0' && str[i] != '\n'; i++){
        if(isalpha(str[i]) == 0 && str[i] != ' ')
            return 0;
    }
    if (i<8)
        return 0;
    return 1;
}

/*Verifica que un nombre de usuario no se encuentre en uso verificando el nombre de todos los usuarios*/
int is_newUsername(char * userName, List * users){

    char copytry[MAXCHARS];
    char copyUserName[MAXCHARS];

    Estudiante * student = (Estudiante*) firstList(users);
    while(student!=NULL){
        strcpy(copytry, userName);
        strcpy(copyUserName, student->Nombre);

        to_minusc(copytry);
        to_minusc(copyUserName);

        if(strcmp(copyUserName, copytry)==0)
            return 0;
        
        student=(Estudiante*)nextList(users);
    }

    return 1;
}

/*  Controla interacciones con el usuario con tal de establecer un nombre de usuario 
    Retorna 0 si se cancela la operacion */
int set_username(char * user, List * users){ 
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

        char user_input[MAXCHARS];

        fgets(user_input, MAXCHARS, stdin);
        char * c = strstr(user_input, "\n");
        if(c) user_input[c-user_input] = '\0';
        clean();

        if (intentos > 2){
            if(toselect(user_input) != 0)
               break;
        }

        if (is_validuser(user_input)){

            if(is_newUsername(user_input, users)){
                strcpy(user, user_input);
                return 1;   
            }
            else{
                printf("\n Este nombre de usuario ya esta en uso!\n");
                intentos++;
                getchar();
            }
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
Estudiante * create_student(){ 
    Estudiante * newEst = (Estudiante*)malloc(sizeof(Estudiante));
    if(newEst==NULL){
        perror("No se pudo reservar memoria para nuevo estudiante ");
        exit(1);
    }
    newEst->Cursos = createList();
    return newEst;
}

/* Retorna dato en posicion i dentro de una lista */
void * posList(List * list, int pos){
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
int set_career(List * cars, Estudiante * user){
    int cont = 0, select;
    char user_input[MAXCHARS];

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
    
    fgets(user_input, MAXCHARS, stdin);
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
int comprobarMaxPeriodo(char * str, List * carr){
    int max = 0, n;
    Carrera * auxCar = (Carrera*)firstList(carr);
    while (auxCar!=NULL)
    {
        if(strstr(auxCar->NomCarrera, str))
            break;
        auxCar = (Carrera*)nextList(carr);
    }

    if (auxCar == NULL) return 0;
    
    Pair * hashPair = firstMap(auxCar->Ramos);
    Curso * course = hashPair->value;
    
    while(course!=NULL){
        if((n = toselect(course->Periodo))>max)
            max = n;

        hashPair = nextMap(auxCar->Ramos);

        if(hashPair==NULL) break;
        course=hashPair->value;
    }

    return max;
}

/*  Controla interacciones con el usuario con tal de establecer el periodo al cual pertenece 
    Retorna 0 si se cancela la operacion  */
int set_period(Estudiante * user, List * carreras){ 
    char user_input[MAXCHARS];
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
    
    fgets(user_input, MAXCHARS, stdin);
    p = strstr(user_input, "\n");
    if(p)
        user_input[p-user_input] = '\0';
    
    comprobar_num = toselect(user_input);
    max_period = comprobarMaxPeriodo(user->Carrera, carreras);

    if (comprobar_num>max_period && max_period != 0){
        system("cls");
        printf( "El periodio que ha ingresado no es valido!\n"
                "Volviendo al menu principal...");
        clean();
        return 0;
    } 
    else if(comprobar_num != 0)
        strcpy(user->Periodo, user_input);
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
List * fromPeriod(Estudiante * st, List * careers){
    List * retList = createList();

    Carrera * auxCar = (Carrera*)firstList(careers);
    while (auxCar!=NULL)
    {
        if(strstr(auxCar->NomCarrera, st->Carrera))
            break;
        auxCar = (Carrera*)nextList(careers);
    }

    if(auxCar==NULL){
        printf("PROBLEMA AL BUSCAR CARRERA EN FROM PERIOD\n");
        exit(1);
    }

    Pair * par = firstMap(auxCar->Ramos);
    Curso * course = par->value;
    while(course!=NULL){
        if(atoi(course->Periodo) == atoi(st->Periodo)){
            pushBack(retList, course);
        }

        par = nextMap(auxCar->Ramos);

        if(par==NULL) break;
        course = par->value;
    }
    getchar();
    return retList;
}

/*Controla interaccion con el usuario con el fin de establecer que cursos se encuentra cursando. Retorna 0 si la respuesta es no*/
int verifyCourse(Curso * course){ /* LISTO */
    char user_input[MAXCHARS];
    int verif;

    system("cls");
    printf( "ID del curso: %s\n"
            "Nombre del curso: %s\n"
            "Periodo al que pertenece: %s\n\n"
            ,course->IDcurso, course->NomCurso, course->Periodo);
    
    printf("Se encuentra realizando este curso?\nIngrese cualquier numero distinto de 0 si la respuesta es si\n");

    fgets(user_input, MAXCHARS, stdin);
    verif = toselect(user_input);

    printf("\nGracias por la confirmacion!\n");

    if(verif == 0){
        clean();
        return 0;
    }

    clean();
    return 1;
}

/*Inserta las dependencias de un curso dentro de una listapara su posterior uso*/
void insertRequisitos(Curso * curso, List * cursos, HashMap * dependencias){
    // Buscar curso en dependencias
    Pair * hashPair = searchMap(dependencias, curso->IDcurso);
    if(hashPair==NULL){
        printf("No se encontro el curso que debio haberse encontrado.");
        exit(1);
    }

    // Obtencion de lista de nodos requisitos.
    Node * nodo = hashPair->value;
    List * requisitos = nodo->Requisitos;

    // Se recorre lista de cadenas correspondientes a el codigo del curso.
    char * rec = (char*)firstList(requisitos);
    while(rec != NULL){
        // Se busca el codigo en dependencias para extraer el curso.
        hashPair = searchMap(dependencias, rec);
        Node * recNode = hashPair->value;
        if(recNode==NULL){
            printf("No se encontro recNode que deberia existir");
            exit(1);
        }

        if(recNode->curso != NULL){
            pushCurrent(cursos, recNode->curso);
        }
        rec = (char*)nextList(requisitos);
    }
    return;
}

/*Maneja interacciones con el usuario con el objetivo de confirmar  que cursos se encuentra cursando
Si retorna 0, se ha cancelado la operacion*/
int modify_courses(Estudiante * student, List * careers, HashMap * dependencias){ 
    char user_input[MAXCHARS];
    int num_input;

    system("cls");
    printf( "+==============================================================+\n"
            "| A continuacion, se le preguntara si esta cursando los cursos |\n"
            "| pertenecientes a su periodo.                                 |\n"
            "|                                                              |\n"
            "| Para aquellos cursos que no este cursando, se le preguntara  |\n"
            "| por aquellos cursos que le preceden.                         |\n"
            "|                                                              |\n"
            "| > Ingrese cualquier numero distinto de 0 para continuar.     |\n"
            "+==============================================================+\n");
    
    fgets(user_input, MAXCHARS, stdin);
    num_input = toselect(user_input);

    if(num_input == 0){
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
void set_variablesHorario(Bloque ** arreglo){
 
    for(int i = 0 ; i<TALLAHORARIO ;i++){
        arreglo[i]->modificable = 1;
        arreglo[i]->ocupado = 0;
        strcpy(arreglo[i]->actividad, "HOLA?");
    }
    return;
}

/*  Reserva memoria para arreglo de punteros para datos tipo bloque. Retorna el arreglo */
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

/* Imprime informacion correspondiente a un dia de la semana 
Metodo:
        Buscar el dia de la semana que se quiere mostrar.
        Recorrer el arreglo correspondiente a dia, imprimimiendo la informacion
        correspondiente.
        Retorna 0 si el dia de la semana ingresado no se encuentra.*/
int printDiaHorario(char * day, HashMap * horario){ 
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
HashMap * createHorario(){
    HashMap * horario = createMap(20);
    char dias[MAXCHARS]= "Lunes,Martes,Miercoles,Jueves,Viernes,Sabado,Domingo";

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
Pair ** reservMemPares(int talla){  
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
void printSemanaHorario(HashMap * horario){ 
    char dias[MAXCHARS]= "Lunes,Martes,Miercoles,Jueves,Viernes,Sabado,Domingo";
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

/* Controla la interacci??n con el usuario con tal de modificar un bloque horario */
void verificarModificarBloque(char * day, HashMap * horario, int bloque){
    char user_input[MAXCHARS];
    int verif;

    system("cls");
    to_minusc(day);
    Pair * hashPair = searchMap(horario, day);
    if(hashPair==NULL){
        printf("Error en verificar bloque\n");
        exit(1);
    }

    Bloque ** bloques = hashPair->value;

    if(bloques[bloque-1]->modificable == 0){
        printf( "Este bloque no es modificable!\n"
                "Por favor, seleccione otro la proxima ocasion!\n");
        getchar();
        return;
    }

    if(bloques[bloque-1]->ocupado == 1){
        printf( "El bloque que usted ingreso se encuentra ocupado por la actividad:\n"
                "%s.\n", bloques[bloque-1]->actividad);
        printf( "\nDesea sobreescribir o eliminar este bloque?\n"
                "Ingrese un numero mayor a 10 para sobreescribir\n"
                "Ingrese un numero menor o igual que 10 para eliminar\n");

        fgets(user_input, MAXCHARS, stdin);
        clean();
        verif = toselect(user_input);

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

            fgets(user_input, MAXCHARS, stdin);
            char * c = strstr(user_input, "\n");
            if(c) user_input[c-user_input] = '\0';
            clean();

            strcpy(bloques[bloque-1]->actividad, user_input);
            printf("Su actividad ha sido insertada de forma exitosa.\nVolviendo a horario semanal...");
            return;
        }
    }
    else{
        printf("Por favor, ingrese la actividad que quiere insertar en este bloque.\n");
        fgets(user_input, MAXCHARS, stdin);
        char * c = strstr(user_input, "\n");
        if(c) user_input[c-user_input] = '\0';
        clean();

        strcpy(bloques[bloque-1]->actividad, user_input);
        bloques[bloque-1]->ocupado = 1;
        printf("Su actividad ha sido insertada de forma exitosa.\nVolviendo a horario semanal...");
        return;
    }
}

/* Una vez establecido el horario base, recorre cada bloque del horario para indicar que no son modificacles */
void lock_horario(HashMap * horario){ 
    char dias[MAXCHARS]= "lunes,martes,miercoles,jueves,viernes,sabado,domingo";

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
int set_horario(Estudiante * user){ 
    char user_input[MAXCHARS];
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

    fgets(user_input, MAXCHARS, stdin);
    char * p = strstr(user_input, "\n");
    if(p) user_input[p-user_input] = '\0';
    clean();

    if(toselect(user_input)==0){
        printf("Operacion cancelada.\nVolviendo al menu principal...");
        clean();
        return 0;
    }
    
    while (1){
        system("cls");
        printSemanaHorario(horario);

        printf("Ingrese el nombre de un dia para modificar ese fragento de horario :");
        fgets(user_input, MAXCHARS, stdin);
        char * c = strstr(user_input, "\n");
        if(c) user_input[c-user_input] = '\0';
        clean();

        if(strcmp(user_input, "fin")==0)
        {
            printf("Ha terminado con su horario!");
            lock_horario(horario);
            break;
        }
        else if(strcmp(user_input, "0")==0)
        {
            printf("Se ha cancelado la operacion.\nVolviendo al menu principal...\n");
            getchar();
            return 0;
        }
        else{
            int bloque;
            system("cls");
            if (printDiaHorario(user_input, horario)==0){
                printf("No se econtro el dia en la impresion de dias!\n");
                getchar();
            }
            else{
                char copyDia[50];
                strcpy(copyDia, user_input);

                printf("Seleccione algun bloque para ingresar/borrar alguna actividad: ");

                fgets(user_input, MAXCHARS, stdin);
                char * c = strstr(user_input, "\n");
                if(c) user_input[c-user_input] = '\0';

                bloque = toselect(user_input);
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
int is_validContrasena(char * intento){
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
int set_password(Estudiante * user){
    char user_input[MAXCHARS];
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

        fgets(user_input, MAXCHARS, stdin);
        char * c = strstr(user_input, "\n");
        if(c) user_input[c-user_input] = '\0';
        clean();

        if(intentos>2){
            if (strcmp(user_input, "0")==0){
                system("cls");
                printf("Ha cancelado la operacion.\nVolviendoo a menu principal...");
                getchar();
                return 0;
            }
        }

        if(is_validContrasena(user_input)==0){
            printf("La contrasena ingresada no es valida\nPor favor, intente nuevamente.\n");
            getchar();
        }
        else{
            strcpy(user->Contrasena, user_input);
            printf("\nSu contrasena ha sido establecida con exito!\n");
            getchar();
            return 1;
        }
    }
    return 0;
}

/*Funci??n que se encarga de manejar las interacciones con el usuario para crear un nuevo perfil de estudiante
  Retorna un dato del  tipo estudiante si se complet?? la operaci??n con ??xito */
Estudiante * formulario(List * careers, HashMap * cursos, List * users){
    Estudiante * new_user;
    char selec[10];
    char user_name[MAXCHARS];
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
        if(set_username(user_name, users))
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

/* Inserta en archivo usuarios.csv el nombre de usuario y contrase??a de un nuevo usuario luego de su creaci??n */
void insert_archUsuarios(Estudiante * user){ /* LISTO */
    FILE * entrada;

    entrada = fopen("archivos/usuarios.csv", "at");
    if(entrada == NULL){
        perror("Error al abrir archivo usuarios ");
        exit(1);
    }

    fprintf(entrada, "\n%s,%s", user->Nombre, user->Contrasena);
    fclose(entrada);
    return;
}

/* Modifica una cadena, volviendo sus espacios a guiones bajos */
void toFormat(char * str){
    int len = strlen(str);
    for(int  i=0 ; i<len ; i++){
        if(str[i]==' ') str[i]='_';
    }
}

/* Escribe los cursos de un alumno en su archivo correspondiente */
void write_Courses(FILE * salida, List * courses){
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

/* Escribe el horario de un alumno en su archivo correspondiente */
void write_Horario(FILE *salida, HashMap *  horario){/* LISTO */

    char dias[MAXCHARS] = "lunes,martes,miercoles,jueves,viernes,sabado,domingo";
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

/* Maneja la exportacion de informaci??n de un alumno en su archivo correspondiente */
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

/* Maneja la exportacion de infromaci??n de todos los alumnos */
void export_infoUsuarios(List * users){ /* LISTO */

    Estudiante * student = (Estudiante*)firstList(users);
    while(student != NULL){
        // Nombre del archivo corresponde al nombre del usuario 
        // con los espacios reemplazados por _.
        char path[MAXCHARS] = "archivos/info_usuarios/";
        char auxnom[MAXCHARS];

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

/*Lee cursos de un usuario desde un archivo y los inserta en la lista de cursos asociada al estudiante */
void readInsert_courses(Estudiante * user,  FILE * input, HashMap * courses){

    char linea[MAXCHARS*2];
    const char * campo;
    

    while(fgets(linea, MAXCHARS*2 , input) != NULL){
        char * c = strstr(linea,"\n");
        if(c) linea[c-linea] = '\0';

        if(strcmp(linea, "FIN CURSOS")==0) break;

        campo = get_csv_field(linea, 0);
        for(int i=1; campo != NULL ; i++){
            Pair * hashPair = searchMap(courses,(char*) campo);
            if(hashPair==NULL){
                perror("Error busqueda en readInsert\n");
                exit(1);
            }

            Node * nodo = hashPair->value;
            pushBack(user->Cursos, nodo->curso);
            campo = get_csv_field(linea,i);
        }
    }

    return;
}

/*Lee los bloques de un usuario desde un archivo y los inserta en el arreglo de bloques  */
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

/*Lee la informaci??n de un horario desde un archivo y lo al estudiante */
void read_horario(Estudiante * user, FILE * input){
    char linea[MAXCHARS*2];
    HashMap * horario = createHorario();
    
    while(fgets(linea, MAXCHARS*2 , input) != NULL){
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

/* Se encarga de leer la infromaci??n de un usuario a partir de su archivo asociaado */
Estudiante * import_infoUsuario(char * nombre, HashMap * courses){

    FILE * entrada;
    char path[MAXCHARS] = "archivos/info_usuarios/";
    char linea[MAXCHARS*2];
    int campo = 1;
    
    Estudiante * old_student = create_student();
    
    toFormat(nombre);
    strcat(path, nombre);
    strcat(path, ".txt");             
    
    
    
    entrada = fopen(path, "rt");
    if (entrada == NULL){
        printf("nombre es %s\n", nombre);
        perror("Error path ");
        exit(1);
    }
    
    while(fgets(linea, MAXCHARS*2 , entrada) != NULL){
        
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

/*Se encarga de importar la informacion de todos los usuarios registrados en la aplicaci??n*/
List * import_allUsers(HashMap * courses){
    char path[MAXCHARS] = "archivos/usuarios.csv";
    List * usuarios = createList();
    Estudiante * usuarioLeido;
    char linea[MAXCHARS];
    FILE * entrada;
    int i = 0;

    entrada = fopen(path,"rt");
    if(entrada==NULL){
        printf("Error path usuarios.");
        exit(1);
    }

    while(fgets(linea, MAXCHARS, entrada) != NULL){
        if(strlen(linea)<3) break;
        const char * campo = get_csv_field(linea, i);

        usuarioLeido = import_infoUsuario((char*) campo, courses);
        if(usuarioLeido) {
            pushBack(usuarios, usuarioLeido);
        }
    }

    fclose(entrada);
    return usuarios;
}

/* Realiza la comprobaci??n de un usuario y su contrase??a */
Estudiante *  comprobar_user(List * usuarios, char * user_input){

    Estudiante * estLeido = (Estudiante*) firstList(usuarios);
    while (estLeido != NULL)
    {
        const char * user = get_csv_field(user_input,0);
        const char * contr = get_csv_field(user_input,1);

        if(strcmp(user , estLeido->Nombre) == 0 
        && strcmp(contr, estLeido->Contrasena) == 0)
            return estLeido;

        estLeido = (Estudiante*) nextList(usuarios);
    }

    return NULL;

 }

int comprobar_Formato(char * input){
    char * tieneComa = strstr(input,",");
    if(tieneComa) return 1;
    else return 0;
}

void analizar_consecuencias(Node * nodo, HashMap * cursos, Estudiante * student, List * Carreras){

    // Obtener carrera del usuario
    Carrera * carrera_est = (Carrera*) firstList(Carreras);
    while(carrera_est!=NULL){

        if(strcmp(carrera_est->NomCarrera, student->Carrera)==0) break;
        carrera_est = (Carrera*) nextList(Carreras);
    }

    // Impresion requisitos del curso a revisar
    printf( "Curso a revisar: %s\n"
            "El nombre es : %s \n\n",nodo->curso->IDcurso, nodo->curso->NomCurso);

    printf( "==============================================\n"
            "|          Requisitos del curso              |\n"
            "==============================================\n\n");


    char * requisito = (char*)firstList(nodo->Requisitos);
    while(requisito!=NULL){
    
        Pair * reqPair = searchMap(cursos, requisito);
        if(reqPair==NULL) break;

        Node * reqNode = reqPair->value;

        printf("%s -> %s\n", reqNode->curso->IDcurso, reqNode->curso->NomCurso);

        requisito = (char*)nextList(nodo->Requisitos);
    }
    printf("\n");

    // Impresion y Analisis de cursos subsiguientes
    printf( "==============================================\n"
            "| Cursos que tienen como requisito %-9s |\n"
            "==============================================\n\n", nodo->curso->IDcurso);
    char * subsiguiente = (char*)firstList(nodo->Subsiguientes);
    while(subsiguiente!=NULL){
    
        Pair * subPair = searchMap(cursos, subsiguiente);
        if(subPair==NULL) break;

        // Revisar que el curso extraido se encuentre en la carrera del estudiante.
        Node * subNode = subPair->value;
        Pair * inCarrera = searchMap(carrera_est->Ramos, subNode->curso->IDcurso);
        if(inCarrera){

            printf("%s -> %s\n\n", subNode->curso->IDcurso, subNode->curso->NomCurso);
            printf("Su periodo es %s, el periodo del curso es %s.\n", student->Periodo, subNode->curso->Periodo);

            if((atoi(student->Periodo)+1)!=atoi(subNode->curso->Periodo)){
                printf( "El reprobar este curso no afectara directamente algun curso del proximo semestre\n"
                        "Sin embargo, esto no significa que este a salvo.\n\n");
            }
            else{
                printf("El reprobar este curso hace que no pueda dar este su proximo semestre!\n\n");
            }
                
        }

        subsiguiente = (char*)nextList(nodo->Subsiguientes);
    }
}


void consecuenciasReprobacion(Estudiante * st, HashMap * cursos, List * carreras){
    char user_input[MAXCHARS];
    const char * campo;
    int i = 0;

    system("cls");
    printf("+=================================================================================+\n"
           "| Aqui puede verificar como le afectara el reprobar un curso.                     |\n"
           "|                                                                                 |\n"
           "| Por favor, ingrese el codigo del curso que desea verificar. Puede ingresar uno  |\n" 
           "| o mas cursos, asi como se especifica en el siguiente ejemplo:                   |\n"
           "|                                                                                 |\n"
           "| 'ABC 123' para un solo curso                                                    |\n"
           "| 'ABC 123,DEF 456,GHI 789' para una serie de cursos.                             |\n"
           "+=================================================================================+\n");

    fgets(user_input, MAXCHARS, stdin);
    char * p = strstr(user_input, "\n");
    if(p) user_input[p-user_input] = '\0';
    clean();

    do{
        system("cls");
        campo = get_csv_field(user_input, i);
        if(campo==NULL) break;

        Pair * hashPair = searchMap(cursos, (char*)campo);
        if(hashPair == NULL){
            printf("El curso %s no existe\nCancelando toda la operacion...");
            getchar();
            break;
        }

        Node * nodo = hashPair->value;

        analizar_consecuencias(nodo, cursos, st, carreras);
        getchar();
        i++;
    }while(1);

}

/* Maneja interacciones con el usuario con tal de modificar su horario */
void utilidad_horario(Estudiante * user){
    while(1){
        char user_input[MAXCHARS];
        int bloque;

        system("cls");
        printf("Bienvenido a su horario\n\n");

        printSemanaHorario(user->Horario);

        printf( "Puede entrar a algun dia para ver o modificar su horario!\n"
                "Si desea volver al menu anterior, ingrese la cadena \"fin\"\n\n"
                "Ingrese el nombre de un dia para modificar ese fragento: ");

        fgets(user_input, MAXCHARS, stdin);
        char * c = strstr(user_input, "\n");
        if(c) user_input[c-user_input] = '\0';
        clean();


        if(strcmp(user_input, "fin")==0)
        {
            system("cls");
            printf("Volviendo al menu principal...");
            break;
        }

        system("cls");

        if (printDiaHorario(user_input, user->Horario)==0){
            printf("No se econtro el dia en la impresion de dias!\n");
            getchar();
        }
        else{
            char copyDia[50];
            strcpy(copyDia, user_input);

            printf("Seleccione algun bloque para ingresar/borrar alguna actividad: ");

            fgets(user_input, MAXCHARS, stdin);
            char * c = strstr(user_input, "\n");
            if(c) user_input[c-user_input] = '\0';

            bloque = toselect(user_input);
            if (bloque==0 || bloque > TALLAHORARIO){
                printf("\nEl bloque ingresado no es valido.\nVolviendo a horario semanal...");
            }
            else 
                verificarModificarBloque(copyDia, user->Horario, bloque);
            clean();
            }

    }
    return;
}


void utilidad_ap(Estudiante *usuario, HashMap * cursos, List * Carreras){

    while(1){
        long selec3;
        long selec4;
        char arch[MAXCHARS];
    
        system("cls");
        printf("Navegador Academico\n\n"

            "1. Mostrar datos Usuario\n"
            "2. Calculadora de Notas\n"
            "3. Aprobacion y cursos\n"
            "4. Organizador Horario\n"
            "5. Valoracion del Curso\n"
            "6. Cerrar sesion\n"

            );

        scanf("%ld", &selec3);
        clean();

        switch (selec3){

            case 1:
                system("cls");
                puts("Estos son sus Datos");
                //Mostrar_datos(arch,usuario)
                clean();
            break;

            case 2:
                system("cls");
                Calculadora_notas();
                clean();
            break;

            case 3:
            consecuenciasReprobacion(usuario, cursos, Carreras);
            
            break;

            case 4:
                system("cls");
                utilidad_horario(usuario);
                clean();
            break; 

            case 5:
                system("cls");
                puts("Ingrese valoracion del curso");
                scanf("%s", arch);
                clean();
                //Valoracion(arch,usuario)
                clean();

            case 6:
                system("cls");
                printf("Ha decidido cerrar sesion!\nVolviendo almenu principal...");
                getchar(); 
                return;

            default:
                printf("La opcion que ha ingresado no es valida!\nVolviendo al menu anterior...");
                getchar();
            break;  
        }
    }
}


/* Se encarga de verificar el inicio de sesion  */
void iniciar_sesion(List * usuarios, HashMap * cursos, List * carreras){
    while(1){
        system("cls");
        puts("Ingrese usuario y contrasenya separados por una coma, sin espacio luego de la coma\n"
            "Ejemplo: Nombre de usuario,contrasena");
        
   
        char user_input[MAXCHARS];
        Estudiante * estudianteIngresado;

        fgets(user_input, MAXCHARS, stdin);
        char * c = strstr(user_input, "\n");
        if(c) user_input[c-user_input] = '\0';
        clean();

        if(comprobar_Formato(user_input)==0){
            printf( "No ha ingresado una cadena con el formato pedido\n"
                    "Vuelva a intentarlo!");
            getchar();
        }
        else{
            if ( (estudianteIngresado = comprobar_user(usuarios,user_input)) ){
                utilidad_ap(estudianteIngresado, cursos, carreras);
                return;
            }
            else
            {
                system("cls");
                printf( "Al parecer este nombre de usuario o contrasena no existen.\n"
                        "Ingrese cualquier numero mayor a 0 para intentar nuevamente: ");

                fgets(user_input, MAXCHARS, stdin);

                if(toselect(user_input) == 0){
                    printf("\n\nNo ha ingresado un numero mayor a 0.\nVolviendo al menu principal...");
                    clean();

                    return;
                }
                clean();
            } 
        }
    }
}

/* funcion calculadora notas ponderadas
    desc:   
            CASO A(POSEE TODAS SUS NOTAS)
            -usuario inserta numero de notas
            -usuario inserta promedio de aprobacion 
            -usuario ingresa sus notas y porcentaje de cada nota n veces
            -usuario ingresa porcentaje que vale el examen 
    
            
            CASO b(NO POSEE TODAS SUS NOTAS)
            -usuario inserta numero de notas
            -usuario inserta promedio de aprobacion 
            -usuario ingresa sus notas y porcentaje de cada nota n veces 
            -se le da al usuario la nota que debe sacarse para aprobar
            
            
*/
int calculoProm(int n,int tabla[n][2]){
    int suma=0;
    int x,y;
    int res;

    for (int i=0;i<n;i++){
        x=tabla[i][1];
        y=tabla[i][2];
        res=x*y;
        suma=suma+res;
    }

    return suma;
}


int ingresoNotas(int n,int aprob){
    int tabla[n][2];
    int nota,porc;
    int cont=1;

    for (int i=0;i<n;i++){
        printf("+=================================================================================+\n"
               "|          Porfavor ingresa tu nota #%d en el siguiente formato:                  |\n"
               "|                          <6,20>                                                 |\n"
               "+=================================================================================+\n",cont);
        scanf("%d , %d",&tabla[i][1],&tabla[i][2]);
        cont++;
        system("cls");
    }

    clean();
    system("cls");
    int promedio=calculoProm(n,tabla);
    printf("+=================================================================================+\n"
           "| Tu tabla de notas y tu promedio es el siguiente:                                |\n"
           "|                                                                                 |\n"
           "|                 notas--->%%porcentajes                                          |\n");
    printf("|                                                                                 |\n");
                       
    for (int j=0;j<n;j++){
        
    printf("|                   |%d--->%d%%|                                                    |\n",tabla[j][1],tabla[j][2]);
    }
    printf("|                                                                                 |\n");
    printf("|Promedio: %d                                                                      |\n",promedio);
    printf("+=================================================================================+\n");
    
}

int CalcularPorcentRes(int n,int tabla[n][2]){
    int tot=100;

    for (int i=0;i<n-1;i++){
        tot=tot-tabla[i][2];
    }

    return tot;
}

int notaNecesaria(int aprob,int n,int tabla[n][2],int porc){
    int tot=0;
    for (int i=0;i<n-1;i++){
        tot=tot-tabla[i][2];
    }

    return tot;
}

int ingresoNfal(int n,int aprob){
    int tabla[n][2];
    int nota,porc;
    int cont=1;
    
    
    for (int i=0;i<n-1;i++){
        printf("+=================================================================================+\n"
               "|          Porfavor ingresa tu nota #%d en el siguiente formato:                  |\n"
               "|                          <6,20>                                                 |\n"
               "+=================================================================================+\n",cont);
        scanf("%d , %d",&tabla[i][1],&tabla[i][2]);
        cont++;
        system("cls");
    }

    int porcent=CalcularPorcentRes(n,tabla);
    int notafal=notaNecesaria(aprob,n,tabla,porcent);
    clean();
    system("cls");
    printf("+=================================================================================+\n"
            "| Tu tabla de notas son la siguiente y necesitas un %d para aprobar el curso:    |\n"
            "|                                                                                |\n"
            "|                 notas--->%%porcentajes                                         |\n",notafal);
                       
    for (int j=0;j<n-1;j++){
    printf( "|                     %d--->%d%%                                                 |\n",tabla[j][1],tabla[j][2]);
    }
    printf("|          Necesitas un %d que equivale a el %d%% de tu promedio de aprobacion    |\n",notafal,porcent);
    printf("+=================================================================================+\n");
    
}

void Calculadora_notas(){
    long NNotas; 
    long aprob;
    long selec;


    system("cls");
    printf("+=================================================================================+\n"
           "| Bienvenido a la calculadora de notas ponderadas!                                |\n"
           "|                                                                                 |\n"
           "| Selecciona la opcion que necesites:                                             |\n"
           "|                                                                                 |\n"
           "| 1.Si posees todas tu notas en el curso.                                         |\n"
           "| 2.Si no posees todas tus notas, pero deseas saber la nota que necesitas para    |\n"
           "|   aprobar.                                                                      |\n" 
           "+=================================================================================+\n");
    scanf("%ld", &selec);
    clean();
    switch(selec){
            case 1:
                system("cls");
                printf("+=================================================================================+\n"
                       "| Porfavor ingresa tu numero de notas y el promedio de aprobacion del curso :     |\n"
                       "|                                                                                 |\n"
                       "|   ejemplo: ( 6 , 45 )                                                           |\n"
                       "+=================================================================================+\n");
                scanf("%ld,%ld",&NNotas,&aprob);
                system("cls");
                ingresoNotas(NNotas,aprob);
                clean();
            break;

            case 2:
                system("cls");
                printf("+=================================================================================+\n"
                       "| Porfavor ingresa tu numero de notas y el promedio de aprobacion del curso :     |\n"
                       "|                                                                                 |\n"
                       "|   ejemplo: ( 6 , 45 )                                                           |\n"
                       "+=================================================================================+\n");
                scanf("%ld,%ld",&NNotas,&aprob);
                system("cls");
                ingresoNfal(NNotas,aprob);
                clean();
            break;
            default:
                printf("La opcion que ha ingresado no es valida!\nVolviendo al menu anterior...");
                getchar();
            break;
    }
}
/*fin funciones */
