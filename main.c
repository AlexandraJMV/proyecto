#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX 100


void formulario(void){

    char selec[10];

    system("color BC");

    printf("Bienvenido estudiante!\n"
           "Muchas gracias por preferir nuestro navegador academico.\n\n"

           "A continuacion se le realizaran una serie de preguntas con tal de establecer\n"
           "su nueva cuenta en el sistema. Se le solicita que siga las instrucciones de forma\n"
           "responsable.\n ¿Desea continuar? y/n" );

    fgets(selec, 10, stdin);

    if (strcmp(selec, "y") != 0){
        system("cls");
        system("color 08");

        printf( "Es una pena! :(\n"
                "Volviendo al menu principal...\n");
    }
    else{
        printf("lol");
    }
    
    return;
}

//Estudiante * usuario = create_usuario;
int main (void)
{
    /*
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
    }*/
    formulario();
    getchar();
    return 0;
}