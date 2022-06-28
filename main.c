#include <stdio.h>
#include <stdlib.h>

#include "hashmap.h"
#include "funciones.h"
#include "list.h"

#define MAX 100

int main (void)
{
    // Importar datos base para funcionamiento del programa.
    system("COLOR 80");
    
    HashMap * courses =  import_courses();
    List * careers = import_carreras(courses);
    List * users = import_allUsers(courses);
    
    // Menu
    while (1){
        Estudiante * new_user;

        char strselec[10];
        int numselec;

        system("cls");
        printf("----Navegador Academico----\n\n"

                "1. Iniciar Sesion\n"
                "2. Crear Perfil\n"
                "3. Salir\n");
        

        fgets(strselec, 10, stdin);
        clean();

        numselec = toselect(strselec);

        switch(numselec){
            case 1:
                iniciar_sesion(users, courses, careers);
                break;
            case 2:
                system("cls");

                if( (new_user = formulario(careers, courses, users)) ){
                    pushBack(users, new_user);
                    insert_archUsuarios(new_user);
                }

                getchar();
                break;
            case 3:
                system("cls");
                
                puts("Saliendo del programa...");
                export_infoUsuarios(users);
                
                getchar();
                return 0;

            default:
                system("cls");

                puts( "Opcion no valida!\n"
                      "Volviendo al menu principal...");

                getchar();
                break;
        }
    }
}