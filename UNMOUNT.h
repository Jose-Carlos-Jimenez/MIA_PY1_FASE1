#ifndef UNMOUNT_H
#define UNMOUNT_H

#endif // UNMOUNT_H

#include <QList>
#include <structs.h>
#include "parser.h"
#include <MOUNT.h>

class UNMOUNT_
{
private:
    string id;
    bool correct;
public:
    UNMOUNT_():id(""), correct(false){}
    void setId(char*);
    string getId();
    void semantic();
    void unmountPart();
    void run();
};

extern QList<MOUNT_> *mounted;

void UNMOUNT_::setId(char * id)
{
    this->id = id;
    if(this->id[0] == '\"')
    {
        this->id = this->id.substr(1, this->id.size() -2);
    }
}

string UNMOUNT_::getId()
{
    return this->id;
}

void UNMOUNT_::semantic()
{
    if(this->getId() != "")
    {
        correct = true;
    }
}

void UNMOUNT_::unmountPart()
{

    int j=-1;
    QList<MOUNT_>::iterator i;
    cout << "MONTADAS: " << endl;
    for(i = mounted->begin(); i!= mounted->end();++i)
    {
        cout << i->getPath() << endl;
    }
    cout << "Desmontando..." << endl;
    for(i = mounted->begin(); i != mounted->end();++i)
    {
        cout << i->getId()<< endl;
        j++;
        if(i->getId() == this->id)
        {
            cout << "Se encontró: " << this->id << endl;
            break;
        }
    }
    if(j!=-1)
    {
        mounted->removeAt(j);
    }
    else
    {
        cout << "La partición que deseas montar no se encuentra montada."<< endl;
    }
    cout << "Lista actualizada: " << endl;
    for(i = mounted->begin(); i!= mounted->end();++i)
    {
        cout << i->getId() << endl;
    }
}

void UNMOUNT_::run()
{
    semantic();
    if(this->correct)
    {
        unmountPart();
    }
    else
    {
        cout << "El parámetro id es obligatorio para este comando." << endl;
    }
}
