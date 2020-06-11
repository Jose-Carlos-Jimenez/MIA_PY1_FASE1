#pragma once
#ifndef MOUNT_H
#define MOUNT_H

#endif /// MOUNT_H

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <QList>
#include <structs.h>
#include "parser.h"

using namespace std;

class MOUNT_
{
private:
    string path;/// Ruta del disco de la partición.
    string name;/// Nombre de la partición.
    string id;/// Id en la lista de particiones montadas.
    char letter;/// Caracter que indica a que disco pertenece.
    int number;/// Número de partición que es montada de algún disco.
    bool correct;/// Semántica correcta de la función.
public:
    /**
     * Constructor de MOUNT
    */
    MOUNT_():path(""), name(""), correct(false){}

    /**
     * Guarda el valor de la ruta
     * @param c: indica la ruta del disco de la partición a montar.
    */
    void setPath(char* c)
    {
        path = c;
        if(path[0]=='\"')
        {
            path = path.substr(1, path.size()-2);
        }
    }
    /**
     * Guarda el nombre de la partición a montar.
     * @param c: partición a montar.
    */
    void setName(char *c)
    {
        name = c;
        if(name[0] == '\"')
        {
            name = name.substr(1, name.size()-2);
        }
    }

    /**
     * Guarda el id que se le asigno a la partición
     * en la lista de montadas.
    */
    void setId();

    /**
     * Guarda la letra del disco a la que pertenece esta partición montada.
     * @param c: letra asignada por el sistema de montaje a su disco.
    */
    void setLetter(char c);

    /**
     * Obtiene el nombre de la partición
    */
    string getName(){return this->name;};

    /**
     * Retorna la ruta del disco de esta partición.
    */
    string getPath(){return this->path;};

    /**
     * Calcula la letra del disco a la cual pertenece la partición
    */
    char getLetter();

    /**
     * Obtiene el número de partición montada de un mismo disco.
    */
    int getNumber();

    /**
     * Calcula el id mediante la concatenacion de
     * VD + letra del disco + número de partición de ese disco.
    */
    string getId();

    /*
     * Configura el identificador de la partición montada.
    */
    string confId();

    /**
     * Verifica la semántica de la operación ingresada en consola.
    */
    void semantic();

    /**
     * Ejecuta la montura.
    */
    void run();

    /**
     * Obtiene el indice de partición que tiene la deseada en el disco.
    */
    int partIndex();

    /**
     * Obtiene el indice de partición en donde está almacenada la lógica.
    */
    int LogicPartIndex();

    /*
     * Monta la partición.
    */
    void mountPartition();

    /*
     * Verifica si la partición que se desea montar ya ha sido montada.
    */
    bool isMounted();
};

extern QList<MOUNT_> *mounted;

void MOUNT_::setId()
{
    this->id = this->confId();
}

void MOUNT_::setLetter(char c)
{
    this->letter = c;
}

char MOUNT_::getLetter()
{
    char a = 'a';
    if(mounted->size() == 0){
        setLetter('a');
        return 'a';
    }
    QList<MOUNT_>::iterator i;
    QList<string> *marcadas = new QList<string>();
    for(i=mounted->begin(); i!=mounted->end(); i++)
    {
        if(i->path != this->path && !marcadas->contains(i->path))
        {
            marcadas->append(i->path);
            a++;
        }
        if(i->path == this->path)
        {
            setLetter(i->letter);
            return  i->letter;
        }
    }
    setLetter(a);
    return a;
}

int MOUNT_::getNumber()
{
    int a = 1;
    QList<MOUNT_>::iterator i;
    i=i+1;
    for(i = mounted->begin(); i!=mounted->end(); i++ )
    {
        if(i->path == this->path)
        {
            a++;
        }
    }
    this->number = a;
    return a;
}

string MOUNT_::getId()
{
    return this->id;
}

string MOUNT_::confId()
{
    string l(1,getLetter());
    return "vd"+ l+ to_string(this->getNumber());
}

void MOUNT_::semantic()
{
    if(this->path !="" && this->name != "")
    {
        correct = true;
    }
    if(this->path == "")
    {
        cout << "El parámetro -path es obligatorio." << endl;
    }
    if(this->name == "")
    {
        cout << "El parámetro -name es obligatorio." << endl;
    }
}

void MOUNT_::run()
{
    semantic();
    if(this->correct)
    {
        setId();
        cout <<"Intentando montar: "<< this->getId() << endl;
        mountPartition();
    }
    else
    {
        cout << "Los parámetros path y name son obligatorios para montar un disco."<< endl;
    }
}

int MOUNT_::partIndex()
{
    FILE *fp;
    if((fp = fopen(path.c_str(),"rb+"))){
        MBR masterboot;
        fseek(fp,0,SEEK_SET);
        fread(&masterboot,sizeof(MBR),1,fp);
        for(int i = 0; i < 4; i++){
            if(masterboot.mbr_partitions[i].part_status != '1'){
                if(strcmp(masterboot.mbr_partitions[i].part_name,name.c_str()) == 0){
                    fclose(fp);
                    return i;
                }
            }
        }
    }
    return -1;
}

int MOUNT_::LogicPartIndex()
{
    FILE *file;
    if((file = fopen(this->path.c_str(),"r+b"))){
        /// Buscar la partición extendida del disco
        int extIndex = -1;
        MBR master;
        fseek(file,0,SEEK_SET);
        fread(&master,sizeof(MBR),1,file);
        for(int i = 0; i < 4; i++){
            if(master.mbr_partitions[i].part_type == 'E'){
                extIndex = i;
                break;
            }
        }
        if(extIndex != -1){
            /// Buscar si existe una lógica en el disco
            EBR ebr;
            fseek(file, master.mbr_partitions[extIndex].part_start,SEEK_SET);
            fread(&ebr,sizeof (EBR),1,file);
            fseek(file, master.mbr_partitions[extIndex].part_start,SEEK_SET);
            while(fread(&ebr,sizeof(EBR),1,file)!=0 && (ftell(file) < master.mbr_partitions[extIndex].part_start + master.mbr_partitions[extIndex].part_size)){
                if(strcmp(ebr.part_name, this->getName().c_str()) == 0){
                    return (ftell(file) - sizeof(EBR));
                }
                if(ebr.part_next == -1)break;
                else fseek(file,ebr.part_next,SEEK_SET);
            }
        }
        fclose(file);
    }
    return -1;
}

void MOUNT_::mountPartition()
{
    int indexOfPart = partIndex();
    if(indexOfPart != -1)
    {
        ///Abrir el archivo
        FILE*file= fopen(this->path.c_str(),"r+b");
        MBR master;
        fseek(file,0,SEEK_SET);
        fread(&master,sizeof (MBR),1,file);

        ///Cambiar su status
        master.mbr_partitions[indexOfPart].part_status = '2';

        ///Reescribirlo
        fseek(file,0, SEEK_SET);
        fwrite(&master,sizeof (MBR),1,file);
        fseek(file,0,SEEK_SET);
        fread(&master, sizeof (MBR),1,file);
        fclose(file);

        ///Verificar si está montada
        if(!isMounted())
        {
            ///Insertar a la lista.
            mounted->append(*this);
            cout << "La partición " << this->id << " ha sido montada con éxito." << endl;
        }
        else
        {
            cout << "La partición ya ha sido montada." << endl;
        }

    }
    else
    {
        int indexLog = this->LogicPartIndex();
        if(indexLog != -1)
        {
            FILE * aux;
            if((aux = fopen(getPath().c_str(),"r+b")))
            {
                ///Leer el ebr
                EBR ebr;
                fseek(aux, indexLog, SEEK_SET);
                fread(&ebr, sizeof(EBR),1,aux);

                ///Cambiar su estado a montado.
                ebr.part_status = '2';
                fseek(aux,indexLog,SEEK_SET);
                fwrite(&ebr,sizeof(EBR),1, aux);
                fclose(aux);
                if(!isMounted())
                {
                    ///Insertar a la lista.
                    setId();
                    mounted->append(*this);
                    cout << "La partición " << this->id << " ha sido montada con éxito." << endl;
                }
                else
                {
                    cout << "La partición ya ha sido montada." << endl;
                }

            }
            else
            {
                cout << "El disco de la partición no existe." << endl;
            }
        }
        else
        {
            cout << "El disco que desea montar no existe."  << endl;
        }

    }
}

bool MOUNT_::isMounted()
{
    QList<MOUNT_>::iterator i;
    for(i = mounted->begin();i != mounted->end();i++)
    {
        if(i->path == this->path && i->name== this->name)
        {
            return  true;
        }
    }
    return  false;
}



