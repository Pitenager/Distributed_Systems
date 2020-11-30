#include "raid.h"
#include "arbol.h"

// 1000 BYTES cada bloque
#define BLOQUE 1000

Raid::Raid(int numDiscos)
{
    this->numDiscos = numDiscos;
}


bool Raid::buscarSectoresLibres(int numeroBloquesNecesarios,nodo_t* nodo) {

    bool bloqueValido;
    bool ocupado = false;
    int contador = 0;
    FILE* sectoresLibres = fopen("sectoreslibres.dat","r+");
    //tamano correcto de sectoresLibres
    for(int i=0;i<(32000/BLOQUE)*numDiscos;i++){

        fread(&bloqueValido,sizeof(bool),1,sectoresLibres);
        if(bloqueValido){ //Si es válido coge el bloque y lo reserva
            nodo->bloquesUsados->push_back(i);
            fseek(sectoresLibres,sizeof(bool)*i,SEEK_SET);
            fwrite(&ocupado,sizeof(bool),1,sectoresLibres);
            contador++;
            if(contador==numeroBloquesNecesarios){ //Ya estan todos los bloques reservados
                fclose(sectoresLibres);
                return true;
            }
        }
    }
    printf("No se han podido reservar los bloques necesarios\n");
    int elemento;
    bool libre = true;

    for (contador;contador>=1;contador--){
        elemento = nodo->bloquesUsados->back();
        nodo->bloquesUsados->pop_back();
        fseek(sectoresLibres,sizeof(bool)*elemento,SEEK_SET);
        fwrite(&libre,sizeof(bool),1,sectoresLibres);
    }
    fclose(sectoresLibres);
    return false;
}


void Raid::writeFile(string archivo, nodo_t* nodo) {

    char* buffer = (char*) calloc(BLOQUE, sizeof(char));

    //cout << archivo << endl;

    FILE* miArchivo;
    int cantidad = 0, contador = 0;

    for(list<int>::iterator i = nodo->bloquesUsados->begin(); i != nodo->bloquesUsados->end(); i++, contador++){

        miArchivo = fopen(archivo.c_str(),"r+");
        fseek(miArchivo,contador*BLOQUE, SEEK_SET);
        fread(buffer,sizeof(char),BLOQUE,miArchivo);
        fclose(miArchivo);

        //El archivo ocupa un bloque o menos
        if(nodo->sizeNodo <= BLOQUE){
            cantidad = nodo->sizeNodo;
            writeBlock(buffer,cantidad,(*i));
        }
        //El archivo ocupa mas de un bloque
        else if(nodo->sizeNodo - (BLOQUE * contador) > BLOQUE){
            writeBlock(buffer,BLOQUE,(*i));
        }
        else { //Cuando lo que resta de archivo es menor que un bloque
            cantidad = nodo->sizeNodo - (BLOQUE * contador);
            writeBlock(buffer,cantidad,(*i));
        }

    }
    free(buffer);
}


void Raid::writeBlock(char* datos, int cantidad, int idBloque) {

    int numeroDisco = findDisco(idBloque,numDiscos);
    int numeroBloque = findSectorDelDisco(idBloque,numDiscos);
    string nombre = "disco"+to_string(numeroDisco)+".dat";

    // Envio un msg para que el disco que necesito espere los datos

    tipoMensaje msg = upload;
    MPI_Send(&msg, sizeof(tipoMensaje), MPI_BYTE , numeroDisco +1, 0, MPI_COMM_WORLD);

    // Envio numeroDisco, cantidad, numeroBloque, datos
    MPI_Send(&numeroDisco,  sizeof(int) , MPI_BYTE , numeroDisco +1, 0, MPI_COMM_WORLD);
    MPI_Send(&cantidad, sizeof(int), MPI_BYTE , numeroDisco +1, 0, MPI_COMM_WORLD);
    MPI_Send(&numeroBloque, sizeof(int), MPI_BYTE , numeroDisco +1, 0, MPI_COMM_WORLD);
    MPI_Send(datos, sizeof(char) * BLOQUE, MPI_BYTE , numeroDisco +1, 0, MPI_COMM_WORLD);



}


void Raid::readFile(nodo_t* nodo){

    FILE* nuevoArchivo = fopen(nodo->nombre,"w");
    fclose(nuevoArchivo);
    char* datos = (char*) calloc(BLOQUE, sizeof(char));
    int cantidad = 0, contador = 0;

    for(std::list<int>::iterator i = nodo->bloquesUsados->begin(); i != nodo->bloquesUsados->end(); i++,contador++){

        //El archivo ocupa un bloque o menos
        if(nodo->sizeNodo <= BLOQUE){
            cantidad = nodo->sizeNodo;
            readBlock(datos, cantidad, (*i));
            nuevoArchivo = fopen(nodo->nombre,"r+");
            fseek(nuevoArchivo,contador*BLOQUE, SEEK_SET);
            fwrite(datos,sizeof(char),cantidad,nuevoArchivo);
        }
        //El archivo ocupa mas de un bloque
        else if(nodo->sizeNodo - (BLOQUE * contador) > BLOQUE){
            readBlock(datos,BLOQUE,(*i));
            nuevoArchivo = fopen(nodo->nombre,"r+");
            fseek(nuevoArchivo,contador*BLOQUE, SEEK_SET);
            fwrite(datos,sizeof(char),BLOQUE,nuevoArchivo);
        }
        else {
            cantidad = nodo->sizeNodo - (BLOQUE * contador);
            readBlock(datos, cantidad, (*i));
            nuevoArchivo = fopen(nodo->nombre,"r+");
            fseek(nuevoArchivo,contador*BLOQUE, SEEK_SET);
            fwrite(datos,sizeof(char),cantidad,nuevoArchivo);
        }
        fclose(nuevoArchivo);
    }

    free(datos);
}

/**
 * @brief Raid::readBlock
 * @param datos
 * @param cantidad
 * @param idBloque
 */
void Raid::readBlock(char* datos,int cantidad,int idBloque){

    int numeroDisco = findDisco(idBloque,numDiscos);
    int numeroBloque = findSectorDelDisco(idBloque,numDiscos);

    // Envio un msg para que el disco que necesito espere los datos

    tipoMensaje msg = download;
    MPI_Send(&msg, sizeof(tipoMensaje), MPI_BYTE , numeroDisco +1, 0, MPI_COMM_WORLD);

    // Envio numeroDisco, cantidad, numeroBloque

    MPI_Send(&numeroDisco, sizeof(int), MPI_BYTE , numeroDisco +1, 0, MPI_COMM_WORLD);
    MPI_Send(&cantidad, sizeof(int), MPI_BYTE , numeroDisco +1, 0, MPI_COMM_WORLD);
    MPI_Send(&numeroBloque, sizeof(int), MPI_BYTE , numeroDisco +1, 0, MPI_COMM_WORLD);

    // Recibo datos

    MPI_Recv(datos, sizeof(char) * BLOQUE, MPI_BYTE, numeroDisco +1, MPI_ANY_TAG, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

}


int Raid::findDisco(int idBloque,int numeroDiscos){

    return idBloque%numeroDiscos;

}

int Raid::findSectorDelDisco(int idBloque,int numeroDiscos){

    return idBloque/numeroDiscos;
}


void Raid::format(int numeroDiscos,int size){

    printf("Formateo de los discos\n");

    this->numDiscos = numeroDiscos;

    size = size*1000-1;
    tipoMensaje msg = formatear;

    for(int i=0;i<numeroDiscos;i++){

        MPI_Send(&msg, sizeof(int), MPI_BYTE , i+1, 0, MPI_COMM_WORLD);
        MPI_Send(&size, sizeof(int), MPI_BYTE , i+1, 0, MPI_COMM_WORLD);
        MPI_Send(&i, sizeof(int), MPI_BYTE , i+1, 0, MPI_COMM_WORLD);

    }

    FILE* sectoresLibres = fopen("sectoreslibres.dat","w");
    bool booleano = true;
    for(int i=0;i<=(size/BLOQUE)*numeroDiscos;i++) {
        fwrite(&booleano, sizeof(bool), 1, sectoresLibres);
    }
    fclose(sectoresLibres);
}

void Raid::comprobarDisco(){
    printf("Comprobando existencia de todos los discos\n");
    MPI_Status status;
    int existe;
    tipoMensaje msg=comprobar;

    for(int i=0;i<this->numDiscos;i++)
    {
        MPI_Send(&msg, sizeof(int), MPI_BYTE , i+1, 0, MPI_COMM_WORLD);
        MPI_Send(&i, sizeof(int), MPI_BYTE , i+1, 0, MPI_COMM_WORLD);
        MPI_Recv(&existe, sizeof(int), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
        if (existe==0)
        {
            int size=0;
            printf("Se ha perdido el disco %d. Se procede a resetear el sistema\n",i);
            printf("Selecciona size de los discos: ");
            std::cin>>size;
            this->format(this->numDiscos,size);
            FILE* fichero = fopen("arbol.dat","w+");
            fclose(fichero);
            break;
        }
    }
}
