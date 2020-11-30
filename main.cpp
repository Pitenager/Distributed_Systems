/*AUTORES: PABLO RUIZ Y MIGUEL MAYORAL*/
/*
#include <iostream>
#include "terminal.h"
#include "arbol.h"

using namespace std;

int main()
{
    printf("Practica realizada por Pablo Ruiz y Miguel Mayoral\n");
    printf("Iniciando la consola personal :)\n");
    Terminal* term = new Terminal();
    term->run();
    getchar();
    return 0;
}
*/

#include "arbol.h"
#include "terminal.h"
#include "mpi.h"
#include "slave.h"
#include "raid.h"
#include <list>
#include <string>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
using namespace std;

#define BLOQUE 1000


void master(int numSlaves)
{
    printf("Practica realizada por Pablo Ruiz y Miguel Mayoral\n");
    printf("Iniciando la consola personal :)\n");
    Terminal* term = new Terminal(numSlaves);
    term->run();
    //getchar();
    //return 0;
    //delete nuevoArbol;
    //delete disco;
    //delete terminal;
}

void slave()
{
    Slave* slave = new Slave();

    char* datos = (char*) calloc(BLOQUE, sizeof(char));
    MPI_Status status;
    bool salir = false;
    int tam, bloque, numeroDisco, existe;

    while(!salir)
    {
        tipoMensaje msg;
        MPI_Recv(&msg, sizeof(tipoMensaje), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);

        switch(msg)
        {
            case formatear:
                MPI_Recv(&tam, sizeof(int), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                MPI_Recv(&numeroDisco, sizeof(int), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                slave->format(numeroDisco,tam);
            break;

            case download:
                printf("Recibido download");
                MPI_Recv(&numeroDisco, sizeof(int), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                MPI_Recv(&tam, sizeof(int), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                MPI_Recv(&bloque, sizeof(int), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                slave->readBlock(numeroDisco,tam,bloque);
            break;

            case upload:
                MPI_Recv(&numeroDisco, sizeof(int), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                MPI_Recv(&tam, sizeof(int), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                MPI_Recv(&bloque, sizeof(int), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                MPI_Recv(datos, sizeof(char) * BLOQUE, MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);

                slave->writeBlock(numeroDisco,tam,bloque,datos);
            break;

            case comprobar:
                existe = 0;
                MPI_Recv(&numeroDisco, sizeof(int), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                existe = slave->comprobarDisco(numeroDisco);
                MPI_Send(&existe,sizeof(int),MPI_BYTE,0,0,MPI_COMM_WORLD);
            break;

            case salida:
                salir = true;
            break;                

            default:
                printf("Error\n");
            break;
        };

    }

    free(datos);
}


int main(int argc, char** argv){

    int commSize;
    int rank;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0)		// Gather scater, posible alternativa eficiente para esto, pero es mas avanzado
    {
        master(commSize);
    }
    else
    {
        slave();
    }

    MPI_Finalize();
    return 0;
}
