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
        char arch[MAX] = "";

        system("cls");
        printf("Iniciar sesion\n\n");

        printf(
            "1. Rut\n"
            "2. contraseña\n"
        );

        scanf("%ld", &selec);
        getchar();

        switch (selec)
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

            
        }
    }
}