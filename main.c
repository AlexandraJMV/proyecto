#include <stdio.h>
#include <stdlib.h>

#include "funciones.h"

#define MAX 100

//Estudiante * usuario = create_usuario;
int main (void)
{
    while (1){

        char strselec[10];
        int numselec;
        long selec2;
        long selec3;
        long selec4;
        char arch[MAX] = "";

        system("cls");
        printf("----Navegador Academico----\n\n");

        printf(
            "1. Iniciar Sesion\n"
            "2. Crear Perfil\n"
            "3. Salir\n"
        );

        fgets(strselec, 10, stdin);
        clean();

        numselec = toselect(strselec);

        switch(numselec){
            case 1:

                system("cls");
                printf("Iniciar Sesion\n\n");

                printf(
                    "1. Rut y Contraseña"
                );

                scanf("%ld", &selec2);
                getchar();

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
                            printf("Navegador Academico\n\n");
                            printf(
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
                formulario();
                getchar();
                break;
            case 3:
                system("cls");
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