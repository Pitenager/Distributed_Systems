/*AUTORES: PABLO RUIZ Y MIGUEL MAYORAL*/

#include "terminal.h"
#include <stdio.h>
#include "stdlib.h"
#include <string.h>

Terminal::Terminal(int numSlaves) {
    FILE* fichero;
    fichero = fopen("arbol.dat","rb");
    if (fichero==NULL) arbol = new Arbol();
    else cargarArbol(fichero);
    disco = new Raid(numSlaves);
    this->numSlaves=numSlaves-1;

    fichero = fopen("sectoreslibres.dat","r+");
    if (fichero == NULL){
        int size;
        printf("Selecciona size de los discos: ");
        std::cin >> size;
        printf("EquiposMPI: %d Esclavos: %d\n", numSlaves, numSlaves-1);
        disco->format(numSlaves-1,size);
    }
    else fclose(fichero);
}

void Terminal::run() {
    printf("YO \n");
    bool exit = false;
    comando_t comando;
    comando.argumentos = new vector<char*>();
    bool valido = true;
    printf("YO \n");
    while (!exit) {
        valido = true;
        rutaActual(this->arbol->dirActual);
        printf("> ");
        valido = lee_comando(&comando);
        if (valido == false) continue; //Si no es valido lo introducido, volver al principio
        ejecuta_comando(&comando);
        if (comando.tipo == CMD_EXIT) exit = true; //Si es exit se sale del bucle
        comando.argumentos->clear();
    }
}

bool Terminal::lee_comando(comando_t* comando) {
    char* linea = new char[1024];
    char separador[3] = " \n";
    char* token = NULL;    
    //leer terminal
    fgets(linea, 1023, stdin);
    //Subdividir en argumentos
        //string tokenizer
    //TODO: gestion de errores -> user no introdujo comando
    token = strtok(linea, separador); //Devuelve el primer token (comando) -> ej: cd .. devuelve cd en token
    if (token == NULL) { //Si no se introduce comnado
        printf("No se ha introducido ningun comando\n");
        return false;
    }
    comando->tipo = get_tipo_comando(token);
    if (comando->tipo == -1) { //Si el comando introducido no es válido
        printf("Comando no valido\n");
        return false;
    }
    while (token != NULL) { //Para sacar todos los demas argumentos
        token = strtok(NULL, separador);
        comando->argumentos->push_back(token);
    }
    return true; //retornar comando
}

void Terminal::ejecuta_comando(comando_t* comando) {

    char* nombre;
    char * nuevo;
    int size;
    tipoMensaje msg;
    switch (comando->tipo) {
    case CMD_LS:
        listarDirectorio(arbol->dirActual);
        printf("\n");
        break;
    case CMD_CD:
        nombre = comando->argumentos->at(0);
        if (nombre == NULL) {
            printf("Error con los parametros del comando (no se ha insertado directorio al que moverse)\n");
            break;
        }
        cambiarDirectorio(nombre, arbol->dirActual);
        break;
    case CMD_PWD:
        rutaActual(arbol->dirActual);
        printf("\n");
        break;
    case CMD_MKDIR:
        nombre = comando->argumentos->at(0);
        if (nombre == NULL) {
            printf("Error con los parametros del comando (no se ha insertado directorio a crear)\n");
            break;
        }
        makeDirectory(nombre, arbol->dirActual);
        break;
    case CMD_RMDIR:
        nombre = comando->argumentos->at(0);
        if (nombre == NULL) {
            printf("Error con los parametros del comando (no se ha insertado directorio a borrar)\n");
            break;
        }
        borrarDirectorio(nombre, arbol->dirActual);
        break;
    case CMD_RM:
        nombre = comando->argumentos->at(0);
        if (nombre == NULL) {
            printf("Error con los parametros del comando (no se ha insertado fichero a borrar)\n");
            break;
        }
        borrarFichero(nombre, arbol->dirActual);
        break;
    case CMD_UPLOAD:
        nombre = comando->argumentos->at(0);
        if (nombre == NULL) {
            printf("Error con los parametros del comando (no se ha insertado fichero a subir)\n");
            break;
        }
        nuevo = comando->argumentos->at(1);
        if (nuevo == NULL) {
            printf("Error con los parametros del comando (no se ha insertado directorio al que subir)\n");
            break;
        }

        if (strcmp(nuevo,"..")==0) {
            printf("No se permite el destino \"..\" para hacer upload\n");
        }
        upload(nombre,nuevo,arbol->dirActual,this->disco);
        break;
    case CMD_MV:
        nombre = comando->argumentos->at(0);
        if (nombre == NULL) {
            printf("Error con los parametros del comando (no se ha insertado nodo a cambiar)\n");
            break;
        }
        nuevo = comando->argumentos->at(1);
        if (nuevo == NULL) {
            printf("Error con los parametros del comando (no se ha insertado nuevo nombre)\n");
            break;
        }
        cambiarNombre(nombre,nuevo,arbol->dirActual);
        break;
    case CMD_CP:
        nombre = comando->argumentos->at(0);
        if (nombre == NULL) {
            printf("Error con los parametros del comando (no se ha insertado fichero a copiar)\n");
            break;
        }
        nuevo = comando->argumentos->at(1);
        if (nuevo == NULL) {
            printf("Error con los parametros del comando (no se ha insertado directorio al que copiar)\n");
            break;
        }
        if (this->arbol->findChild(this->arbol->dirActual,nuevo) != NULL){
            printf("La carpeta destino ya existe\n");
            break;
        }
        if (strcmp(comando->argumentos->at(1),".")==0){
            printf("Ya existe dicho elemento en el directorio\n");
            break;
        }
        copiar(nombre,nuevo,arbol->dirActual);
        break;
    case CMD_LLS:
        localLS();
        break;
    case CMD_LCD:
        nombre = comando->argumentos->at(0);
        if (nombre == NULL){
            printf("Error, el directorio al que moverse no existe o no se ha introducido\n");
            break;
        }
        localCD(nombre);
        break;
    case CMD_LPWD:
        localPWD();
        break;
    case CMD_EXIT:
        //salvar datos
        printf("Closing file system\n");
        msg = salida;
        enviaMensaje(numSlaves,msg);
        guardarArbol();
        //exit(0);
        break;
    case CMD_DOWNLOAD:
        nombre = comando->argumentos->at(0);
        if (nombre == NULL) {
            printf("Error con los parametros del comando (no se ha insertado nodo a descargar)\n");
            break;
        }
        download(nombre,this->disco);
        break;
    case FORMAT:
        size = atoi(comando->argumentos->at(0));
        if (size == NULL){
            printf("Error, no se ha introducido size del disco\n");
            break;
        }
        else if (size <= 0){
            printf("Error, el size del disco no puede ser <=0\n");
            break;
        }
        msg = formatear;
        enviaMensaje(numSlaves,msg);
        disco->format(numSlaves - 1, size);
        break;
    default:
        printf("Command not found\n");
        break;
    }
}

int Terminal::get_tipo_comando(char* comando) {
    if (comando != NULL) { //Miramos que comando es para asignarlo a la terminal
        if (strcmp("ls", comando) == 0)
            return CMD_LS;
        if (strcmp("cd", comando) == 0)
            return CMD_CD;
        if (strcmp("pwd", comando) == 0)
            return CMD_PWD;
        if (strcmp("mkdir", comando) == 0)
            return CMD_MKDIR;
        if (strcmp("rmdir", comando) == 0)
            return CMD_RMDIR;
        if (strcmp("rm", comando) == 0)
            return CMD_RM;
        if (strcmp("upload", comando) == 0)
            return CMD_UPLOAD;
        if (strcmp("mv", comando) == 0)
            return CMD_MV;
        if (strcmp("cp", comando) == 0)
            return CMD_CP;
        if (strcmp("lls", comando) == 0)
            return CMD_LLS;
        if (strcmp("lcd", comando) == 0)
            return CMD_LCD;
        if (strcmp("lpwd", comando) == 0)
            return CMD_LPWD;
        if (strcmp("exit", comando) == 0)
            return CMD_EXIT;
        if (strcmp("download",comando)==0)
            return CMD_DOWNLOAD;
        if (strcmp("format",comando)==0)
            return FORMAT;
    }
    return -1; //Error

}

void Terminal::listarDirectorio(nodo_t* nodoActual) {
    int size = nodoActual->hijos->size();
    int contador = 0;
    if (size == 0) printf("No hay elementos que listar");
    else {
        for (int i = 0; i < size; i++) {
            if (nodoActual->hijos->at(i)->esDirectorio) {
                printf("DIR ");// %s %d %s", nodoActual->hijos->at(i)->nombre, nodoActual->hijos->at(i)->tamano, asctime(localtime(&nodoActual->hijos->at(i)->fechaModificacion)));
            }
            else printf("FILE "); //FALTA IMPRIMIR FECHA
            printf("%s %d %s", nodoActual->hijos->at(i)->nombre, nodoActual->hijos->at(i)->tamano,asctime(localtime(&nodoActual->hijos->at(i)->fechaModificacion)));
            contador = contador + nodoActual->hijos->at(i)->tamano;
        }
        printf("%d elemento(s) en el directorio remoto \"%s\" ocupando un total de %d bytes", size, nodoActual->nombre, contador);
    }
}

void Terminal::rutaActual(nodo_t *nodoActual) {
    vector<char*>*nodos = new vector<char*>();
    int profundidad = nodoActual->profundidad;
    for (int i = 0; i <= profundidad; i++) { //Meto todos los nombres al principio para que esten en orden
        nodos->insert(nodos->begin(), nodoActual->nombre);
        nodoActual = nodoActual->nodoPadre;
    }
    for (int i = 0; i <= profundidad; i++) //Los imprimo
        printf("/%s", nodos->at(i));
}

void Terminal::makeDirectory(char* nombre, nodo_t*nodoActual) {
    if (strcmp(nombre,".")==0 || strcmp(nombre,"..")==0) printf("Ya existe el directorio %s\n",nombre);
    else{
        nodo_t* nuevoNodo;
        nuevoNodo = nodoActual->elArbol->findChild(nodoActual, nombre);
        if (nuevoNodo != NULL) printf("Error, ese directorio ya existe\n");
        else {
            nuevoNodo = nodoActual->elArbol->addChild(nodoActual, nombre,true);
            if (nuevoNodo == NULL) printf("Error creando el nodo\n");
        }
    }
}

void Terminal::borrarFichero(char* nombre, nodo_t* nodoActual) {
    nodo_t* nodoRemoved;
    nodoRemoved = nodoActual->elArbol->findChild(nodoActual, nombre);
    if (nodoRemoved != NULL) nodoActual->elArbol->removeChild(nodoRemoved);
    else printf("El fichero indicado no existe\n");
}

void Terminal::borrarDirectorio(char* nombre, nodo_t* nodoActual) {
    if (strcmp(nombre,"..")==0) printf("No se puede borrar el directorio padre desde el hijo\n");
    else if (strcmp(nombre,".")==0) printf("No se puede borrar un directorio estando en el\n");
    else {
        nodo_t* nodoRemoved = nodoActual->elArbol->findChild(nodoActual, nombre);
        if (nodoRemoved == NULL) printf("El directorio indicado no existe\n");
        else {
            if (nodoRemoved->hijos->size() != 0) printf("El directorio indicado no se puede borrar, tiene hijos\n");
            else {
                nodoActual->elArbol->removeChild(nodoRemoved);
            }
        }
    }
}

void Terminal::cambiarDirectorio(char* nombre, nodo_t* nodoActual) {
    if (strcmp(nombre, "..") == 0)
        nodoActual->elArbol->dirActual = nodoActual->nodoPadre;
    else if (strcmp(nombre, "/") == 0)
        nodoActual->elArbol->dirActual = nodoActual->elArbol->nodoRaiz;
    else if (strcmp(nombre,".")==0) printf("Ya estas en dicho directorio\n");
    else {
        nodo_t* nodoACambiar = nodoActual->elArbol->findChild(nodoActual, nombre);
        if (nodoACambiar == NULL) printf("No existe el directorio especificado\n");
        else {
            if (nodoACambiar->esDirectorio == false) printf("No se puede cambiar a un fichero\n");
            else if (nodoACambiar != NULL) {
                nodoActual->elArbol->dirActual = nodoACambiar;
            }
        }
    }
}

void Terminal::localLS() {
    system("ls");
}

void Terminal::localCD(char* destino) {
    chdir(destino);
}

void Terminal::localPWD() {
    system("pwd");
}

void Terminal::cambiarNombre(char* nombreAntiguo, char*nombreNuevo, nodo_t*nodoPadre) {
    nodo_t* nodoACambiar = nodoPadre->elArbol->findChild(nodoPadre, nombreAntiguo);
    if (nodoACambiar == NULL) printf("No existe el nodo que se quiere modificar\n");
    else {
        if (nodoACambiar->esDirectorio && (strcmp(nombreNuevo,"..")==0 || strcmp(nombreNuevo,".")==0))
            printf ("No se puede poner dicho nombre a un directorio\n");
        else nodoPadre->elArbol->updateChild(nombreNuevo, NULL, NULL, nodoACambiar);
    }
}


void Terminal::upload(char* nombre,char* destino, nodo_t* nodoActual, Raid* raid) {
    nodo_t* nodoNuevo;
    nodoNuevo = this->arbol->findChild(this->arbol->dirActual,destino);
    int size=0;
    int bloquesNecesarios=0;
    bool haySectores = false;
    FILE* nuevoArchivo=fopen(nombre,"r+");

    if (strcmp(destino,".")!=0 && nodoNuevo==NULL)
        nodoNuevo = nodoActual->elArbol->addChild(nodoActual,destino,true);
    else if (nodoNuevo != NULL)
        nodoActual = nodoNuevo;
    else nodoNuevo = nodoActual;

    struct stat fileInfo;
    stat(nombre, &fileInfo);
    if (S_ISDIR (fileInfo.st_mode) == 0){
        nodoNuevo = nodoActual->elArbol->addChild(nodoNuevo,nombre,false);
        nodoActual->elArbol->updateChild(NULL,fileInfo.st_mtime,fileInfo.st_size,nodoNuevo);

        fseek(nuevoArchivo,0,SEEK_END);
        size=ftell(nuevoArchivo);
        fclose(nuevoArchivo);
        nodoNuevo->sizeNodo=size;
        if (size>=1000){
            bloquesNecesarios=ceil(size/1000.0f);
        }
        else
            bloquesNecesarios=1;
        printf("...Buscando sectores libres para el archivo...\n");
        haySectores=raid->buscarSectoresLibres(bloquesNecesarios,nodoNuevo);
        if (haySectores){
            printf("...Escribiendo en disco...\n");
            raid->writeFile(nombre,nodoNuevo);
        }
    }
    else {
        nodoNuevo = nodoActual->elArbol->addChild(nodoNuevo,nombre,true);
        uploadRecursivo(nodoNuevo,nodoNuevo->nombre,raid);
    }
}

void Terminal::uploadRecursivo(nodo_t* nodoPadre, char* nombre,Raid* raid){
    int size=0;
    int bloquesNecesarios=0;
    bool haySectores = false;
    FILE* nuevoArchivo=fopen(nombre,"r+");

    DIR * dir;
    struct dirent  *elemento;
    if ((dir = opendir (nombre))!=NULL){
        chdir(nombre);
        nodo_t* nodoNuevo;
        struct stat fileInfo;
        while((elemento = readdir(dir)) != NULL){
            if (strcmp(elemento->d_name,"..")!=0 & strcmp(elemento->d_name,".")!=0){
                stat(elemento->d_name, &fileInfo);
                if (S_IFDIR &(fileInfo.st_mode)){
                    nodoNuevo = nodoPadre->elArbol->addChild(nodoPadre,elemento->d_name,true);
                    nodoPadre->elArbol->updateChild(NULL,fileInfo.st_mtime,fileInfo.st_size,nodoNuevo);
                    uploadRecursivo(nodoNuevo,nodoNuevo->nombre,raid);
                    chdir("..");
                }
                else {
                    nodoNuevo = nodoPadre->elArbol->addChild(nodoPadre,elemento->d_name,false);
                    nodoPadre->elArbol->updateChild(NULL,fileInfo.st_mtime,fileInfo.st_size,nodoNuevo);

                    fseek(nuevoArchivo,0,SEEK_END);
                    size=ftell(nuevoArchivo);
                    fclose(nuevoArchivo);
                    nodoNuevo->sizeNodo=size;
                    if (size>=1000){
                        bloquesNecesarios=ceil(size/1000.0f);
                    }
                    else
                        bloquesNecesarios=1;
                    printf("...Buscando sectores libres para el archivo...\n");
                    haySectores = raid->buscarSectoresLibres(bloquesNecesarios,nodoNuevo);
                    if (haySectores){
                        printf("...Escribiendo en disco...\n");
                        raid->writeFile(nombre,nodoNuevo);
                    }
                }
            }
        }
        closedir(dir);
    }
    else printf("Ha habido un problema abriendo el archivo\n");
}

void Terminal::copiar(char* nombre,char* destino, nodo_t* nodoActual) {
    nodo_t* nodoNuevo;
    if (destino == "..") nodoActual = nodoActual->nodoPadre;
    else if (destino == "/") nodoActual = nodoActual->elArbol->nodoRaiz;
    nodoNuevo = nodoActual->elArbol->findChild(nodoActual,destino);
    if (nodoNuevo == NULL) printf("Error, la carpeta destino no existe\n");
    else {
        nodoNuevo = nodoActual->elArbol->addChild(nodoActual,destino,true);
         if (nodoNuevo->esDirectorio == false){
             nodoNuevo = nodoActual->elArbol->addChild(nodoNuevo,nombre,false);
               //nodoActual->elArbol->updateChild(NULL,nodoNuevo->fechaModificacion,nodoNuevo->tamano,nodoNuevo);
         }
         else {
             nodoNuevo = nodoActual->elArbol->addChild(nodoNuevo,nombre,true);
             copiarRecursivo(nodoNuevo,nodoActual->elArbol->findChild(nodoActual,nombre));
         }
    }
}

void Terminal::copiarRecursivo(nodo_t* nodoPadre, nodo_t* original){
    int size = original->hijos->size();
    if (size!=0){
    nodo_t* nodoNuevo;
    for (int i=0;i<size;i++){
            if (original->hijos->at(i)->esDirectorio){
                nodoNuevo = nodoPadre->elArbol->addChild(nodoPadre,original->hijos->at(i)->nombre,true);
                nodoPadre->elArbol->updateChild(original->hijos->at(i)->nombre,original->hijos->at(i)->fechaModificacion,original->hijos->at(i)->tamano,nodoNuevo);
                copiarRecursivo(nodoNuevo,original->elArbol->findChild(original,original->hijos->at(i)->nombre));
            }
            else {
                nodoNuevo = nodoPadre->elArbol->addChild(nodoPadre,original->hijos->at(i)->nombre,false);
                nodoPadre->elArbol->updateChild(original->hijos->at(i)->nombre,original->hijos->at(i)->fechaModificacion,original->hijos->at(i)->tamano,nodoNuevo);
            }
        }
    }
    else printf("Ha habido un problema abriendo el archivo\n");
}


void Terminal::cargarArbol(FILE* fichero){
    int numeroNodos[1];
    fread(numeroNodos,sizeof(int),1,fichero);
    int ultimoID[1];
    fread(ultimoID,sizeof(int),1,fichero);
    bool esDirectorio [1];
    int id[1];
    off_t tamano[1];
    time_t fechaModificacion[1];
    char nombre [25];
    int profundidad[1];
    int numeroHijos[1];
    int idPadre[1];

    arbol = new Arbol();

    nodo_t* nodoPadre;
    nodo_t* nodoACrear;

    for (int i=0;i<numeroNodos[0];i++){
        fread(esDirectorio,sizeof(bool),1,fichero);
        fread(id,sizeof(int),1,fichero);
        fread(tamano,sizeof(off_t),1,fichero);
        fread(fechaModificacion,sizeof(time_t),1,fichero);
        fread(nombre, sizeof(char),25,fichero);
        fread(profundidad,sizeof(int),1,fichero);
        fread(numeroHijos,sizeof(int),1,fichero);
        fread(idPadre,sizeof(int),1,fichero);

        if (id[0] == 0){
            arbol->nodoRaiz->esDirectorio = esDirectorio[0];
            arbol->nodoRaiz->id = id[0];
            arbol->nodoRaiz->tamano = tamano[0];
            arbol->nodoRaiz->fechaModificacion = fechaModificacion[0];
            strcpy(arbol->nodoRaiz->nombre,nombre);
            arbol->nodoRaiz->profundidad = profundidad[0];
            arbol->nodoRaiz->numeroHijos = numeroHijos[0];

        }

        else {
            nodoPadre = arbol->findChildByID(arbol->nodoRaiz,idPadre[0]);
            nodoACrear = arbol->addChild(nodoPadre,nombre,esDirectorio[0]);
            nodoACrear->fechaModificacion = fechaModificacion[0];
            nodoACrear->id = id[0];
            nodoACrear->numeroHijos = numeroHijos[0];
            nodoACrear->profundidad = profundidad[0];
            nodoACrear->tamano = tamano[0];
        }
    }
    
    fclose(fichero);
}



void Terminal::guardarArbol(){
    // crear fichero y abrir fichero modo lectura
    FILE* fichero;
    fichero = fopen("arbol.dat", "wb");

    if(fichero == NULL)
        printf("Error al abrir el fichero en la escritura de datos");

    else
    {
        // guardar num nodos y ultimo id
        int numeroNodos[1];
        numeroNodos[0] = arbol->totalNodos;
        fwrite(numeroNodos, sizeof(int), 1, fichero);

        int ultimoIdentificador[1];
        ultimoIdentificador[0] = arbol->ultimoID;
        fwrite(ultimoIdentificador, sizeof(int), 1, fichero);

        guardarArbolRecursivo(arbol->nodoRaiz, fichero);

        fclose(fichero);
    }


}

int Terminal::guardarArbolRecursivo(nodo_t* nodo, FILE* fichero){
    bool indicadorDirectorio[1];
    indicadorDirectorio[0] = nodo->esDirectorio;
    fwrite(indicadorDirectorio, sizeof(bool), 1, fichero);

    int identificador[1];
    identificador[0] = nodo->id;
    fwrite(identificador, sizeof(int), 1, fichero);

    off_t tamanio[1];
    tamanio[0] = nodo->tamano;
    fwrite(tamanio, sizeof(off_t), 1, fichero);

    time_t fecha[1];
    fecha[0] = nodo->fechaModificacion;
    fwrite(fecha, sizeof(time_t), 1, fichero);

    char nombreNodo[25];
    strcpy(nombreNodo,nodo->nombre);
    fwrite(nombreNodo, sizeof(char), 25, fichero);

    int profundidadNodo[1];
    profundidadNodo[0] = nodo->profundidad;
    fwrite(profundidadNodo, sizeof(int), 1, fichero);

    int hijos[1];
    hijos[0] = nodo->hijos->size();
    fwrite(hijos, sizeof(int), 1, fichero);

    int idPadre[1];
    if(nodo->nodoPadre == NULL)
        idPadre[0] = -1;
    else
        idPadre[0] = nodo->nodoPadre->id;
    fwrite(idPadre, sizeof(int), 1, fichero);
    int numeroDeHijos = nodo->hijos->size();

    int basura;
    if (numeroDeHijos==0) return 0;
    else {
        for(int i = 0; i < numeroDeHijos; i++)
        {
            basura = guardarArbolRecursivo(nodo->hijos->at(i), fichero);
        }

        return 0;
    }
}

void Terminal::enviaMensaje(int numslaves, tipoMensaje msg)
{
    printf("Esclavos: %d\n", numslaves);
    for(int i=0; i < numslaves; i++)
    {
        //MPI_Send(const void *buf, int count, MPI_Datatype datatype, int des, int tag, MPI_Comm comm);
        printf("Mensaje de salida al slave %d\n", i+1);
        MPI_Send(&msg, sizeof(tipoMensaje), MPI_BYTE , i+1, 0, MPI_COMM_WORLD);
    }
}

void Terminal::download(char* archivo, Raid *raid){
    nodo_t * downloaded = this->arbol->findChild(this->arbol->dirActual,archivo);
    if (downloaded != NULL)
        raid->readFile(downloaded);
    else printf("El archivo a descargar no se ha encontrado\n");

}
