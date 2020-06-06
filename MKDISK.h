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

using namespace std;

class MKDISK_
{
private:
    int size;
    char fit_[2];
    std::string unit;
    char path[255];
    bool correct;
    struct kbyte
    {
        char space[1024] = "0";
    };

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
};

void MKDISK_::setSize(char *value)
{
    this->size =atoi(value);
}

void MKDISK_::setFit(char *value)
{
    strcpy(this->fit_, value);
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
        cout << this->path << endl;
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
    printf("Ruta: %s\n", this->path);
    printf("Fit: %s\n", this->fit_);
    printf("Size: %i\n", this->size);
    printf("Unidad: %s\n", this->unit.c_str());
    createDisk();
}

int MKDISK_::createDisk()
{
    setCorrect();
    if(this->correct)
    {
        string pathClone = this->path;
        string pathTwice = this->path;
        const size_t last_slash_idx = pathClone.find_last_of("/");
        if (std::string::npos != last_slash_idx)
        {
            pathClone = pathClone.substr(0, last_slash_idx);
        }
        string comando = "mkdir -p \'";
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
        fflush(f);
        fclose(f);
        ifstream  original_disk(pathTwice, std::ios::binary);
        string name_without_ext = pathTwice.substr(0, pathTwice.size()-5);
        ofstream  dst(name_without_ext+"[RAID].disk",   std::ios::binary);
        dst << original_disk.rdbuf();

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

std::string convertToString(char* a, int size)
{
    int i;
    std::string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}
