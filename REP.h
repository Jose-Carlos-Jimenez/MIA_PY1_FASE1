#ifndef REP_H
#define REP_H

#endif // REP_H

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <algorithm>

using namespace std;

class REP_
{
private:
    char *id;
    char *ruta;
    char *name;
    bool correct;
public:
    /*
     * Constructor
    */
    REP_():id(0),ruta(0),name(0),correct(false){}
    /*
     * Getters & Setters
    */
    void setId(char* id)
    {
        this->id = id;
    }
    void setRuta(char* ruta)
    {
        this->ruta = ruta;
    }
    void setName(char* name)
    {
        this->name = name;
    }
    char* getId()
    {
        return this->id;
    }
    char* getRuta()
    {
        return  this->id;
    }
    char* getName()
    {
        return  this->id;
    }

    /*
     * Método para verificar semántica de la operación
    */
    void semantic()
    {
        if(this->getId() !=0 && this->getName() != 0 && this->getRuta()!=0)
        {
            this->correct = true;
        }
    }
    /*
     * Método para ejecutar
    */
    void run()
    {
        semantic();
        if(this->correct)
        {
            string nameS(this->name);
            if(toLowerString(nameS) == "mbr")
            {
                //reportMBR();
            }
            else if(toLowerString(nameS) == "disk")
            {
                //reportDisk();
            }
        }
        else
        {
            cout << "Faltan parámetros para generar el reporte." << endl;
        }
    }
    string toLowerString(string input)
    {
        std::for_each(input.begin(), input.end(), [](char & c){
            c = ::tolower(c);
        });
        return input;
    }

    /*
     * Método para generar el reporte de MBR
    */
    void reportDisk()
    {
        FILE *f;
        if((f = fopen(this->getRuta(),"w")))
        {

        }
        else
        {
            cout << "El disco que intentas generar no existe." << endl;
        }

    }
};
