#ifndef RAID_H
#define RAID_H

#pragma once
#include "arbol.h"
#include "mpi.h"
#include <list>
#include <string>
#include <string.h>
#include <iostream>
#include <sys/stat.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
using namespace std;

typedef enum tipoMensaje
{
    formatear = 0,
    upload,
    download,
    salida,
    comprobar

}tipoMensaje;


class Raid
{
public:
    int numDiscos;


    Raid(int numDiscos);
    bool buscarSectoresLibres(int numeroBloques,nodo_t* nodo);
    void writeFile(string archivo,nodo_t* nodo);
    void readFile(nodo_t* nodo);
    void writeBlock(char* datos, int cantidad, int idBloque);
    void readBlock(char* datos, int cantidad, int idBloque);
    int findDisco(int idBloque,int numeroDiscos);
    int findSectorDelDisco(int idBloque,int numeroDiscos);
    void format(int numeroDiscos,int size);
    void comprobarDisco();
};

#endif // RAID_H
