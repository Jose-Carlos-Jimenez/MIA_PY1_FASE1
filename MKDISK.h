#ifndef MKDISK_H
#define MKDISK_H
#endif /// MKDISK_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <libgen.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include <structs.h>

using namespace std;

class MKDISK_
{
private:
    int size;///Atributo size
    char fit_[2];/// Arreglo para guardar fit
    string unit;/// Atributo unit
    char path[255];/// Carpeta en donde se almacenará el disco
    bool correct;/// Indicador de ejecución.
    MBR master;/// MBR auxiliar para escribir en disco.

public:
    /**
     * Constructor
    */
    MKDISK_(){};

    /**
     * Configura el atributo size
     * @param size: Indica la cantidad de unidades que ocupará el disco.
    */
    void setSize(char* size);

    /**
     * Configura el parámetro fit
     * @param fit: Indica el tipo de ajuste con el que se agregarán las particiones en el disco.
    */
    void setFit(char* fit);

    /**
     * Configura las unidades del disco.
     * @param unit: indica si son megabytes, kilobytes ó bytes.
    */
    void setUnit(char* unit);

    /**
     * Indica la ruta del archivo
     * @param path: indica en donde se debe de crear el disco.
    */
    void setPath(char* path);

    /**
     * Verifica si la semántica del caso es adecuada.
    */
    void setCorrect();

    /**
     * Acciona el comando para crear disco.
    */
    void printMk();

    /**
     * Crea un disco con los parámetros de la clase si todo está ok.
    */
    int createDisk();

    /**
     * Devuelve el tamaño del disco en bytes.
    */
    int getSize();

    /**
     * Devuelve un múltiplo para cuantificar las unidades establecidas.
    */
    int getUnit();

    /**
     * Devuelve un string con todos los cáracteres contenidos en el parámetro en minúscula.
     * @param cadena: Cadena que se desea en minúsculas.
    */
    std::string toLowerString(string cadena);

    /**
     * Configura el MBR cuando todos los parámetros están ok,
     * este utiliza los parámetros de la clase misma por lo cual no
     * recibe parámetros.
    */
    void configureMaster();

    /**
     * Obtiene la letra representativa del tipo de ajuste que se tiene.
    */
    char getFit();
};

void MKDISK_::setSize(char *value)
{
    this->size =atoi(value);
}

void MKDISK_::setFit(char *value)
{
    strcpy(this->fit_, value);
    this->fit_[0] = tolower(fit_[0]);
    this->fit_[1] = tolower(fit_[1]);
}

void MKDISK_::setUnit(char *value)
{
    this->unit=value;
}

void MKDISK_::setPath(char *value)
{
    if(value[0]=='\"')
    {
        string aux = value;
        aux = aux.substr(1,aux.length()-2);
        const char* nValue = aux.c_str();
        strcpy(this->path, nValue);
    }
    else
    {
        strcpy(this->path, value);
    }
}

void MKDISK_::setCorrect()
{
    this->correct = false;
    if(this->path[0] == '/' && this->size != 0)
    {
        if(std::string(this->path) != "/" && size > 0)
        {
            this->correct = true;
        }
        else
        {
            cout << "Algunos de los parametros no cumplen con los requerimentos." << endl;
        }
    }
    else
    {
        cout << "Algunos de los parametros no cumplen con los requerimentos." << endl;
    }
}

void MKDISK_::printMk()
{
    createDisk();
}

int MKDISK_::createDisk()
{
    setCorrect();
    if(this->correct)
    {
        ///Creando el disco principal
        string pathClone = this->path;
        string pathTwice = this->path;
        char* ruta_aux = this->path;

        const size_t last_slash_idx = pathClone.find_last_of("/");
        if (std::string::npos != last_slash_idx)
        {
            pathClone = pathClone.substr(0, last_slash_idx);
        }
        string comando = "sudo mkdir -p \'";
        comando+= dirname(path);
        comando+= '\'';
        system(comando.c_str());

        /// Comando para permisos.
        string comandow = "sudo chmod -R 777 \'";
        comandow+= dirname(ruta_aux);
        comandow += '\'';
        system(comandow.c_str());
        FILE *f;
        f = fopen(pathTwice.c_str(),"wb");
        if(f == NULL)
        {
            return 0;
        }
        int sizeOfArray = getSize();
        for(int i = 0; i < sizeOfArray; ++i)
        {
            char space[1024];
            if(i==0)
            {
                space[0] = '0';
            }
            fwrite(&space, sizeof(space), 1, f);
        }
        configureMaster();
        fseek(f,0,SEEK_SET);
        fwrite(&this->master, sizeof (MBR),1,f);
        fseek(f,0,SEEK_SET);
        fread(&master,sizeof (MBR),1,f);
        fflush(f);
        fclose(f);

        /// Creando RAID
        string pathRaid = pathTwice.substr(0, pathTwice.size()-5) + "_raid.disk";
        FILE* fr = fopen(pathRaid.c_str(),"wb");
        for(int i = 0; i < sizeOfArray; ++i)
        {
            char space[1024];
            if(i==0)
            {
                space[0] = '0';
            }
            fwrite(&space, sizeof(space), 1, fr);
        }
        fseek(fr,0,SEEK_SET);
        fwrite(&this->master, sizeof (MBR),1,fr);
        fflush(fr);
        fclose(fr);

        ///Mensaje para notificar.
        cout << "Disco creado con éxito." << endl;
        return 1;
    }
    return 0;
}

int MKDISK_::getSize()
{
    int tam = this->size*this->getUnit();
    return tam;
}

int MKDISK_::getUnit()
{
    this->unit = this->toLowerString(this->unit);
    if(this->unit=="m")
    {
        return 1024;
    }
    return 1;
}

string MKDISK_::toLowerString(string input)
{
    std::for_each(input.begin(), input.end(), [](char & c){
        c = ::tolower(c);
    });
    return input;
}

void MKDISK_::configureMaster()
{
    /// Seteando tamaño
    master.mbr_tamano = getSize()*1024;

    /// Seteando fecha y hora
    time_t     now = time(0);
    master.mbr_fecha_creacion = now;

    /// Firma del disco
    srand(time(NULL));
    int num = rand();
    num=1+rand()%(100000-1);
    master.mbr_disk_signature = num;

    /// Guardando tipo de fit
    master.disk_fit = getFit();
}

char MKDISK_::getFit()
{
    if(tolower(fit_[0]) != 'b' || tolower(fit_[0]) != 'f' || tolower(fit_[0]) != 'w')
    {
        return 'F';
    }
    return toupper(this->fit_[0]);
}

std::string convertToString(char* a, int size)
{
    int i;
    std::string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}
