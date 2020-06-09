#ifndef MKDISK_H
#define MKDISK_H
#endif // MKDISK_H

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
    int size;
    char fit_[2];
    string unit;
    char path[255];
    bool correct;
    MBR master;

public:
    MKDISK_(){};
    void setSize(char*);
    void setFit(char*);
    void setUnit(char*);
    void setPath(char*);
    void setCorrect();
    void printMk();
    std::string convertToString(char*,int);
    int createDisk();
    int getSize();
    int getUnit();
    std::string toLowerString(string);
    void configureMaster();
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
        //Creando el disco principal
        string pathClone = this->path;
        string pathTwice = this->path;
        const size_t last_slash_idx = pathClone.find_last_of("/");
        if (std::string::npos != last_slash_idx)
        {
            pathClone = pathClone.substr(0, last_slash_idx);
        }
        string comando = "sudo mkdir -p \'";
        comando+= dirname(path);
        comando+= '\'';
        system(comando.c_str());
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
        fflush(f);
        fclose(f);

        // Creando RAID
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

        //Mensaje para notificar.
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
    // Seteando tamaño
    master.mbr_tamano = getSize()*1024;

    // Seteando fecha y hora
    time_t     now = time(0);
    master.mbr_fecha_creacion = now;
    /* No se utilizó pero es para imprimir time_t
     *
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    char s = strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    cout << buf << endl;
    */

    // Firma del disco
    srand(time(NULL));
    int num = rand();
    num=1+rand()%(100000-1);
    master.mbr_disk_signature = num;

    // Guardando tipo de fit
    master.disk_fit = getFit();
}

char MKDISK_::getFit()
{
    if(fit_[0]== '\0')
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
