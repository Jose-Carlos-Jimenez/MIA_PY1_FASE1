#ifndef RMDISK_H
#define RMDISK_H

#endif // RMDISK_H

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <cctype>
#include <libgen.h>

using namespace std;

class RMDISK_
{
private:
    string path;
    bool correct;

public:
    RMDISK_():path(""){};
    void borrarDisco();
    void setObligatoriedad();
    void setPath(char*);
};

void RMDISK_::borrarDisco()
{
    //Verificando semantica
    setObligatoriedad();

    if(this->correct)
    {
        //Creando el disco principal
        string pathClone = this->path;
        FILE *f;
        if((f=fopen(this->path.c_str(),"r")))
        {
            string command = "rm '" + string(this->path) + "'";
            system(command.c_str());
            cout << "Se ha eliminado el disco con éxito." << endl;
        }
        else
        {
            cout << "El disco que desea borrar no existe, por favor verifique la ruta." << endl;
        }
    }
    else
    {
        cout << "El parametro -path es obligatorio, por favor verifique." << endl;
    }
}

void RMDISK_::setObligatoriedad()
{
    this->correct = false;
    string proob = string(path);
    if(proob.length() > 1)
    {
        this->correct = true;
        return;
    }
}

void RMDISK_::setPath(char * value)
{
    if(value[0]=='\"')
    {
        this->path = value;
        path = path.substr(1, path.length()-2);
    }
    else
    {
        this->path = value;
    }
}
