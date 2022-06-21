#include <stdio.h>
#include <stdlib.h>

#include "hashmap.h"
#include "funciones.h"
#include "list.h"
#include "funcionesmain.c"

#define MAX 100

int main (void)
{
    // Importar datos base para funcionamiento del programa.
    List * users = createList();
    HashMap * courses =  import_courses(); //CONTIENE CURSOS Y SUS DEPENDENCIAS EN NODOS.
    List * careers = import_carreras(courses);
    

    while (1){
        Estudiante * new_user;

        char strselec[10];
        int numselec;
        long selec2;
        char arch[MAX] = "";

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

                system("cls");
                printf("Iniciar Sesion\n\n");

                printf(
                    "1. Rut y Contrasenya"
                );
                // Separar ingreso rut y contrasena o  especificar un formato de ingreso
                // ej: RUT//CONTRASENA.
                scanf("%ld", &selec2);
                getchar();

                // la confirmacion de usuario deberia estar fuera del siguiente sub-menu.
                // sub menus se recomiendan esten en funcion aparte
                // ejemplo: menu_usuario(usuario) en funciones.c o funciones main.c
                switch (selec2)
                {
                    menu_usuario(users); //no estoy segura de que poner en el parametro si se supone que son los datos ya guardados del 
                                        //usuario no unos nuevos. (PROVISORIO)
                }
            case 2:
                system("cls");
                if((new_user = formulario(careers, courses))){
                    pushBack(users, new_user);
                    insert_archUsuarios(new_user);
                }
                
                getchar();
                break;
            case 3:
                system("cls");
                export_infoUsuarios(users);
                puts("Saliendo del programa...");
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