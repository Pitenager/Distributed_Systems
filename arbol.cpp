/*AUTORES: PABLO RUIZ Y MIGUEL MAYORAL*/

#include "terminal.h"
#include "arbol.h"

Arbol::Arbol() { //Crear el arbol remoto (directorio raiz)
    nodo_t* raiz=new nodo_t();
    raiz->esDirectorio = 1;
    raiz->id = 0;
    raiz->nodoPadre = NULL;
    raiz->tamano = 4096;
    raiz->fechaModificacion = time(0);
    raiz->hijos = new vector<nodo_t*>();
    strcpy(raiz->nombre, "root");
    raiz->elArbol = this;
    raiz->profundidad = 0;
    raiz->numeroHijos = 0;
    raiz->bloquesUsados = new list<int>();
    raiz->sizeNodo = 0;

    this->nodoRaiz = raiz;
    this->dirActual = raiz;
    this->totalNodos = 1;
    this->ultimoID = 0;
}

int Arbol::getId(nodo_t* nodoActual) {
    return nodoActual->id;
}

bool Arbol::getTipo(nodo_t* nodoActual) {
    return nodoActual->esDirectorio;
}

char* Arbol::getNombre(nodo_t* nodoActual) {
    return nodoActual->nombre;
}

off_t Arbol::getTamano(nodo_t *nodoActual) {
    return nodoActual->tamano;
}

time_t Arbol::getFechaModificacion(nodo_t *nodoActual) {
    return nodoActual->fechaModificacion;
}

nodo_t* Arbol::addChild(nodo_t *nodoPadre, char *nombre,bool tipo) {
    nodo_t* nuevoNodo=new nodo_t();
    if (nodoPadre->esDirectorio == true) {
        this->ultimoID++;
        this->totalNodos++;
        nuevoNodo->elArbol = nodoPadre->elArbol;
        nuevoNodo->esDirectorio = tipo;
        nuevoNodo->fechaModificacion = time(0);
        nuevoNodo->hijos = new vector<nodo_t*>();
        nuevoNodo->bloquesUsados = new list<int>();
        nuevoNodo->sizeNodo = 0;
        nuevoNodo->id = this->ultimoID;
        nuevoNodo->nodoPadre = nodoPadre;
        strcpy(nuevoNodo->nombre, nombre);
        nuevoNodo->numeroHijos = 0;
        nuevoNodo->profundidad = nodoPadre->profundidad + 1;
        nuevoNodo->tamano = 4096;
        nodoPadre->hijos->push_back(nuevoNodo);
        return nuevoNodo;
    }

    return NULL;
}

nodo_t* Arbol::findChild(nodo_t* nodoPadre, char* nombre) {
    int size = nodoPadre->hijos->size();
    for (int i = 0; i < size; i++) {
        if (strcmp(nodoPadre->hijos->at(i)->nombre, nombre) == 0)
            return nodoPadre->hijos->at(i);
    }
    return NULL;
}

nodo_t* Arbol::findChildByID(nodo_t* nodoABuscar,int id) {
    if (nodoABuscar->id == id) return nodoABuscar;
    else {
        int size = nodoABuscar->hijos->size();
        if (size==0) return NULL;
        else {
            nodo_t* nodo;
            for (int i=0;i<size;i++){
                nodo = findChildByID(nodoABuscar->hijos->at(i),id);
                if (nodo != NULL) return nodo;
            }
        }
    }

    return NULL;
}

nodo_t* Arbol::updateChild(char* nombre, time_t time, off_t tamano, nodo_t* nodo) {
    if (nombre != NULL) {
        strcpy(nodo->nombre, nombre);
    }
    if (time != NULL) {
        nodo->fechaModificacion = time;
    }
    if (tamano != NULL) {
        nodo->tamano = tamano;
    }
    return nodo;
}

void Arbol::removeChild(nodo_t *nodo) {
    int size = nodo->nodoPadre->hijos->size();
    int id = nodo->id;
    for (int i = 0; i < size; i++) {
        if (nodo->nodoPadre->hijos->at(i)->id == id) {
            nodo->nodoPadre->hijos->erase(nodo->nodoPadre->hijos->begin() + i);
            nodo->nodoPadre = NULL;
            break;
        }
    }
    this->totalNodos--;
}
