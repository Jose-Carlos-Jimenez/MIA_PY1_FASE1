#ifndef MKDISK_H
#define MKDISK_H
#endif // MKDISK_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>

class MKDISK_
{
private:
    int size;
    char fit_[2];
    std::string unit;
    char path[255];
    bool correct;

public:
    MKDISK_(){};
    void setSize(char*);
    void setFit(char*);
    void setUnit(char*);
    void setPath(char*);
    void setCorrect();
    void printMk();
    std::string convertToString(char*,int);
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
    strcpy(this->path, value);
}

void MKDISK_::setCorrect()
{
    this->correct = false;
    if(this->path[0] == '/' && this->size != 0)
    {
        this->correct = true;
    }
}

void MKDISK_::printMk()
{
    printf("Ruta: %s\n", this->path);
    printf("Fit: %s\n", this->fit_);
    printf("Size: %i\n", this->size);
    printf("Unidad: %s\n", this->unit.c_str());
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
