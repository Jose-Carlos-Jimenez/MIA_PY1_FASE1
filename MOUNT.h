#pragma once
#ifndef MOUNT_H
#define MOUNT_H

#endif // MOUNT_H

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
    string path;
    string name;
    string id;
    char letter;
    int number;
    bool correct;
public:
    MOUNT_():path(""), name(""), correct(false){}
    void setPath(char* c)
    {
        path = c;
        if(path[0]=='\"')
        {
            path = path.substr(1, path.size()-2);
        }
    }
    void setName(char *c)
    {
        name = c;
        if(name[0] == '\"')
        {
            name = name.substr(1, name.size()-2);
        }
    }
    void setId();
    void setLetter(char c);
    string getName(){return this->name;};
    string getPath(){return this->path;};
    char getLetter();
    int getNumber();
    string getId();
    string confId();
    void semantic();
    void run();
    int partIndex();
    int LogicPartIndex();
    void mountPartition();
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
    char a = 'a' - 1;
    if(mounted->size() == 0){
        setLetter('a');
        return 'a';
    }
    QList<MOUNT_>::iterator i;
    i+3;
    for(i=mounted->begin(); i!=mounted->end(); i++)
    {
        if(i->path != this->path)
        {
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
    int a = 0;
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
        // Buscar la partición extendida del disco
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
            // Buscar si existe una lógica en el disco
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
        //Abrir el archivo
        FILE*file= fopen(this->path.c_str(),"r+b");
        MBR master;
        fseek(file,0,SEEK_SET);
        fread(&master,sizeof (MBR),1,file);

        //Cambiar su status
        master.mbr_partitions[indexOfPart].part_status = '2';

        //Reescribirlo
        fseek(file,0, SEEK_SET);
        fwrite(&master,sizeof (MBR),1,file);
        fseek(file,0,SEEK_SET);
        fread(&master, sizeof (MBR),1,file);
        fclose(file);

        //Verificar si está montada
        if(!isMounted())
        {
            //Insertar a la lista.
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
        int indexLog = this->LogicPartIndex();
        if(indexLog != -1)
        {
            FILE * aux;
            if((aux = fopen(getPath().c_str(),"r+b")))
            {
                //Leer el ebr
                EBR ebr;
                fseek(aux, indexLog, SEEK_SET);
                fread(&ebr, sizeof(EBR),1,aux);

                //Cambiar su estado a montado.
                ebr.part_status = '2';
                fseek(aux,indexLog,SEEK_SET);
                fwrite(&ebr,sizeof(EBR),1, aux);
                fclose(aux);
                if(!isMounted())
                {
                    //Insertar a la lista.
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



