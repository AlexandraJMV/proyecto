#include <stdio.h>
#include <stdlib.h>

#include "funciones.h"

#define MAX 100

//Estudiante * usuario = create_usuario;
int main (void)
{
    while (1){

        long selec;
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

        scanf("%ld", &selec2);
        getchar();

        switch(selec){
            case 1:

                system("cls");
                printf("Iniciar Sesion\n\n");

                printf(
                    "1. Rut\n"
                    "2. Contraseña\n"
                    "3. Comprovacion\n"
                );

                scanf("%ld", &selec2);
                getchar();

                switch (selec2)
                {
                    case 1:

                        system("cls");
                        puts("Ingrese Rut");
                        scanf("%s", arch);
                        getchar();
                        //buscar_Rut(arch,usuario)
                        getchar();
                    break;

                    case 2:

                        system("cls");
                        puts("Ingrese Contraseña");
                        scanf("%s", arch);
                        getchar();
                        //buscar_Contraseña(arch,usuario)
                        getchar();
                    break;

                    case 3:

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
                        getchar();

                        switch (selec3){

                            case 1:

                                system("cls");
                                puts("Estos son sus Datos");
                                //Mostrar_datos(arch,usuario)
                                getchar();
                            break;

                            case 2:
                                system("cls");
                                puts("Nota y Porcentaje");
                                scanf("%s", arch);
                                getchar();
                                //Calculador(arch,usuario)
                                getchar();
                            break;

                            case 3:

                                system("cls");
                                puts("Formas de aprovacion o reprovacion");
                                //Mostrar_ap(arch,usuario)
                                getchar();
                            break;

                            case 4:

                                system("cls");
                                puts("Estas son sus notas");
                                //Mostrar_notas(arch,usuario)
                                getchar();
                            break;

                            case 5:

                                system("cls");
                                puts("Este es tu horario");
                                //Mostrar_horario(arch,usuario)
                                getchar();
                            break; 

                            case 6:

                                system("cls");
                                puts("Ingrese valoracion del curso");
                                scanf("%s", arch);
                                getchar();
                                //Valoracion(arch,usuario)
                                getchar();
                            break;  

                        }   
                    break;
            }
            break;

            case 2:

                system("cls");
                printf("Crear Perfil\n\n");

                printf(
                    "1. Nombre del Estudiante\n"
                    "2. Carrera del Estudiante\n"
                    "3. Periodo del Estudiante\n"
                    "4. Modificacion y verificacion de cursos\n"
                    "5. Horario Base\n"
                    "6. Establecer Contraseña\n"
                );

                scanf("%ld", &selec2);
                getchar(); 

                switch(selec4){

                    case 1:

                        system("cls");
                        puts("Ingrese Nombre Estudiante");
                        scanf("%s", arch);
                        getchar();
                        //Agregar_Nom(arch,usuario)
                        getchar();
                    break;

                    case 2:

                        system("cls");
                        puts("Ingrese Carrera");
                        scanf("%s", arch);
                        getchar();
                        //Agregar_carr(arch,usuario)
                        getchar();
                    break;

                    case 3:

                        system("cls");
                        puts("Ingrese Periodo Estudiante");
                        scanf("%s", arch);
                        getchar();
                        //Agregar_Per(arch,usuario)
                        getchar();
                    break;

                    case 4:

                        system("cls");
                        puts("Ingrese Modificacion del curso");
                        scanf("%s", arch);
                        getchar();
                        //Modificacion(arch,usuario)
                        getchar();
                    break;

                    case 5:

                        system("cls");
                        puts("Ingrese Horario Base");
                        scanf("%s", arch);
                        getchar();
                        //Agregar_Ho(arch,usuario)
                        getchar();
                    break;

                    case 6:

                        system("cls");
                        puts("Ingrese Nueva Contraseña");
                        scanf("%s", arch);
                        getchar();
                        //Agregar_Con(arch,usuario)
                        getchar();
                    break;
                    
                }
            break;
        }
        break;
    }
}