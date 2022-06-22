#include <stdio.h>
#include <stdlib.h>

#include "hashmap.h"
#include "funciones.h"
#include "list.h"
#include "funcionesmain.h"

#define MAX 100

int main (void)
{
    // Importar datos base para funcionamiento del programa.
    HashMap * courses =  import_courses();
    List * careers = import_carreras(courses);
    List * users = import_allUsers(courses);
    
    // Menu
    while (1){
        Estudiante * new_user;

        char strselec[10];
        int numselec;

        // char arch[MAX] = "";  NO HAY necesidad de cadenas en este menu.

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
                menu_usuario(users, courses, careers);
                /*
                system("cls");
                printf("Iniciar Sesion\n\n");

                printf(         >>>>>>>>>>>>>>>>>>>>>>>>>>    Esto debio haber sido eliminado si en menu usuarios ya se pide
                                    >>>>>>>>>>>>>>>>>>>>>>    por la info necesaria. 
                    "1. Rut y Contrasenya"
                );

                scanf("%ld", &selec2);
                clean();*/

                //switch (selec2)  Porque el switch?

                //{
                    //menu_usuario(users); //no estoy segura de que poner en el parametro si se supone que son los datos ya guardados del 
                                        //usuario no unos nuevos. (PROVISORIO)
                    
                    /*
                    Users es una LISTA de usuarios. Usuarios que iniciaron sesion. Hay que COMPROBAR si existe un usuario antes de 
                    importar sus datos.  La funcion de importar YA FUE creada.
                    
                    Esto fue hecho para asegurarse de guardar la informacion modificada de TODOS los usuarios que hayan iniciado sesion.
                    Tambien se puede hacer usuario por usuario pero es lo que se me ocurrio al momento.
                    Se pudo hacer una importacion de todos los usuarios tambien, la funcion YA esta. Se pudo hacer.

                //}*/
                break;
            case 2:
                system("cls");

                if( (new_user = formulario(careers, courses)) ){
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