#ifndef SLAVE_H
#define SLAVE_H

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


class Slave {
public:

    void writeBlock(int numeroDisco, int cantidad,int numeroBloque, char* datos);
    void readBlock(int numeroDisco, int cantidad,int numeroBloque);
    void format(int numeroDisco,int size);
    int comprobarDisco(int numeroDisco);

};

#endif // SLAVE_H
