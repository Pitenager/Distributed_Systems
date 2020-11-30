/*AUTORES: PABLO RUIZ Y MIGUEL MAYORAL*/

#ifndef ARBOL_H
#define ARBOL_H
#include <vector>
#include <stdio.h>
#include "stdlib.h"
#include <string.h>
#include<sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <list>

using namespace std;
class Arbol;
typedef struct nodo_t {
    bool esDirectorio;
    int id;
    nodo_t* nodoPadre;
    off_t tamano;
    time_t fechaModificacion;
    vector<nodo_t*>* hijos;
    char nombre[25];
    Arbol* elArbol;
    int profundidad;
    int numeroHijos;

    list<int> *bloquesUsados;
    int sizeNodo;
}nodo_t;

class Arbol {
public:
    nodo_t* nodoRaiz;
    nodo_t* dirActual;
    int totalNodos;
    int ultimoID;

    Arbol();
    nodo_t* sacarPadre(nodo_t* nodoActual);
    vector<nodo_t>* sacarHijos(nodo_t* nodoActual);
    nodo_t* sacarUnHijo(nodo_t* nodoActual, char* nombre);
    int getId(nodo_t* nodoActual);
    bool getTipo(nodo_t* nodoActual);
    time_t getFechaModificacion(nodo_t* nodoActual);
    off_t getTamano(nodo_t* nodoActual);
    nodo_t* addChild(nodo_t* nodoPadre, char* nombre,bool tipo);
    nodo_t* findChild(nodo_t* nodoPadre, char* nombre);
    nodo_t* findChildByID(nodo_t* nodoABuscar,int id);
    void removeChild(nodo_t*nodo);
    nodo_t* updateChild(char* nombre, time_t time, off_t tamano, nodo_t* nodo);
    char* getNombre(nodo_t* nodoActual);
};

#endif // ARBOL_H
