#include <stdio.h>
#include <stdlib.h>

#include "hashmap.h"
#include "funciones.h"
#include "list.h"

#define MAX 100

void menu_usuario(Estudiante *usuario){

    char arch[MAX] = "";
 

    system("cls");
    puts("Ingrese Rut y contrasenya separados por un espacio.");
    scanf("%s", arch);

    clean();
    //buscar_Us(arch,usuario)
    clean();

    if (/*buscar_Us(arch,usuario == 1*/NULL){
        utilidad_ap(usuario);
    }
}


void utilidad_ap(Estudiante *usuario){

    char arch[MAX] = "";
    long selec3;
    long selec4;
 
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
            printf("Formas de aprovacion o reprovacion\n\n"

                "1. Aprovacion\n"
                "2. Reprovacion\n"
            );

            scanf("%ld", &selec4);
            clean();
            switch(selec4){
                case 1:
                    system("cls");
                    puts("Notas necesarias para Aprovacion");
                    //Mostrar_Nap(arch,usuario)
                    clean();
                break;
                case 2:
                    system("cls");
                    puts("Notas necesarias para Reprovar");
                    //Mostrar_Nre(arch,usuario)
                    clean();
                break;
            }
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