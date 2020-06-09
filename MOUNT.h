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
    void semantic();
    void run();
    int partIndex();
    void mountPartition();
    bool isMounted();
};

extern QList<MOUNT_> *mounted;

void MOUNT_::setId()
{
    this->id = this->getId();
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
    i = mounted->begin();
    i+3;
    for(; i!=mounted->end()-1; i++ )
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

void MOUNT_::mountPartition()
{
    int indexOfPart = partIndex();
    if(indexOfPart != -1)
    {
        //Abrir el archivo
        FILE*file= fopen(this->path.c_str(),"rb");
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
        //Puede ser una partición lógica.

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


