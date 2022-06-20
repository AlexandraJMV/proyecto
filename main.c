#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "funciones.h"
#include "list.h"

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
        long selec3;
        long selec4;
        char arch[MAXCHAR] = "";

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
                    case 1:

                        system("cls");
                        puts("Ingrese Rut");
                        scanf("%s", arch);
                        clean();
                        //buscar_Us(arch,usuario)
                        clean();
                        if (/*buscar_Us(arch,usuario == 1*/NULL){
                            system("cls");
                            printf("Navegador Academico\n\n"

                                "1. Mostrar datos Usuario\n"
                                "2. Calculador de Notas\n"
                                "3. Aprovacion y cursos\n"
                                "4. Visualizacion  y Modificacion de Notas\n"
                                "5. Organizador Horario\n"
                                "6. Valoracion del Curso\n"
                                "7. Cerrar sesion\n"

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
                                    puts("Nota y Porcentaje");
                                    scanf("%s", arch);
                                    getchar();
                                    //Calculador(arch,usuario)
                                    clean();
                                break;

                                case 3:
                                    system("cls");
                                    puts("Formas de aprovacion o reprovacion");
                                    //Mostrar_ap(arch,usuario)
                                    clean();
                                break;

                                case 4:

                                    system("cls");
                                    puts("Estas son sus notas");
                                    //Mostrar_notas(arch,usuario)
                                    clean();
                                break;

                                case 5:

                                    system("cls");
                                    puts("Este es tu horario");
                                    //Mostrar_horario(arch,usuario)
                                    clean();
                                break; 

                                case 6:

                                    system("cls");
                                    puts("Ingrese valoracion del curso");
                                    scanf("%s", arch);
                                    clean();
                                    //Valoracion(arch,usuario)
                                    clean();
                                break;  

                            }   
                        }
                    }
                break;
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