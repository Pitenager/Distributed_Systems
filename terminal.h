/*AUTORES: PABLO RUIZ Y MIGUEL MAYORAL*/

#ifndef TERMINAL_H
#define TERMINAL_H
#include "arbol.h"
#include "raid.h"
#include <vector>
#include <math.h>

using namespace std;

#define CMD_LS 1
#define CMD_CD 2
#define CMD_PWD 3
#define CMD_MKDIR 4
#define CMD_RMDIR 5
#define CMD_RM 6
#define CMD_UPLOAD 7
#define CMD_MV 8
#define CMD_CP 9
#define CMD_LLS 10
#define CMD_LCD 11
#define CMD_LPWD 12
#define CMD_EXIT 13
#define CMD_DOWNLOAD 14
#define FORMAT 15

typedef struct comando_t {
    int tipo;
    vector<char*>* argumentos;
}comando_t;

class Terminal {

    const char* comandos[13] = {
        "ls",
        "cd",
        "pwd",
        "mkdir",
        "rmdir",
        "rm",
        "upload",
        "mv",
        "cp",
        "lls",
        "lcd",
        "lpwd",
        "exit"
    };

public:
    Arbol* arbol;
    Raid* disco;
    int numSlaves;

    Terminal(int numSlaves);
    void run();
    bool lee_comando(comando_t* comando);
    void ejecuta_comando(comando_t* comando);
    int get_tipo_comando(char* comando);
    void listarDirectorio(nodo_t* nodoActual);
    void cambiarDirectorio(char* nombre, nodo_t* nodoActual);
    void rutaActual(nodo_t*nodoActual);
    void makeDirectory(char*nombre, nodo_t* nodoActual);
    void borrarFichero(char* nombre, nodo_t* nodoActual);
    void borrarDirectorio(char* nombre, nodo_t* nodoActual);
    void upload(char* nombre,char* destino, nodo_t* nodoActual,Raid* raid);
    void uploadRecursivo(nodo_t* nodoPadre,char* nombre, Raid* raid);
    void cambiarNombre(char* nombreAntiguo,char*nombreNuevo,nodo_t*nodoPadre);
    void copiar(char* nombre,char* destino, nodo_t* nodoActual);
    void copiarRecursivo(nodo_t* nodoPadre,nodo_t* original);
    void localLS();
    void localCD(char* destino);
    void localPWD();
    void cargarArbol(FILE* fichero);
    void guardarArbol();
    int guardarArbolRecursivo(nodo_t* nodo, FILE* fichero);
    void enviaMensaje(int numslaves, tipoMensaje msg);
    void download(char* archivo, Raid* raid);
};

#endif // TERMINAL_H
