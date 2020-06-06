#ifndef FDISK_H
#define FDISK_H

#endif // FDISK_H

/***********************
 *   Librerias de C++  *
 ***********************/
#include<iostream>
using namespace  std;

class FDISK_
{
private:
    /*
     * Tamaño de la particion
     */
    int size;
    /*
     * Unidad del tamaño de la particion
     */
    char unit;
    /*
     * Directorio en donde se creara la particio
     */
    char *path;
    /*
     * Ajuste de la particion
     */
    char *fit;
    /*
     * Forma de eliminar
     */
    char *_delete;
    /*
     * Nombre de la particion
     */
    char *name;
    /*
     * Añade o remueve segun sea el signo del numero, espacio en la particion
     */
    int add;
    /*
     * Tipo de partición que se creara
    */
    enum type{P, E, L, empty};
    type type;
    /*
     * Tipo de operacion
     */
    enum operation{create_, delete_, modify_};
    operation operation;
    /*
     * Identificara si la instruccion FDISK es valida
     */
    bool correct;


public:
    /*
     * Constructor vacio del objeto
     */
    FDISK_():size(0),add(0),type(empty),operation(create_){};

    /*
     * Modificadores de los atributos del objeto
     */
    void setSize(char* size)
    {
        this->size = stoi(size);
    }

    void setUnit(char* unit)
    {
        this->unit = unit[0];
    }

    void setPath(char *path)
    {
        this->path = path;
    }

    void setFit(char* fit)
    {
        this->fit = fit;
    }

    void setDelete(char* _delete)
    {
        this->_delete = _delete;
        this->operation = delete_;
    }

    void setName(char *name)
    {
        this->name = name;
    }

    void setAdd(char* add)
    {
        this->add = atoi(add);
        this->operation = modify_;
    }

    void setCorrect(bool correct)
    {
        this->correct = correct;
    }

    void setType(char *type)
    {
        switch (tolower(type[0])) {
        case 'p':
            this->type = P;
            break;
        case 'e':
            this->type = E;
            break;
        case 'l':
            this->type = L;
            break;
        default:
            cout << "Error, tipo de partición no definida." << endl;
            this->correct = false;
        }
    }

    signed char getType()
    {
        switch (type) {
        case P:
            return('P');
            break;
        case E:
            return('E');
            break;
        case L:
            return('L');
            break;
        default:
            return('P');
        }
    }

    /*
     * Metodo para crear particion
     */
    void createPart(){
        //print();
        semantic();
        return;
    }

    /*
     * Metodo para modificar particion
     */
    void modifyPart(){
        return;
    }

    /*
     * Metodo para elimiar particion
     */
    void deletePart(){
        return;
    }

    /*
     * Verifica que los atributos de la instruccion esten correctos
     */
    bool checkAtt(){
        return false;
    }

    /*
     * Verifica que los atributos de la instruccion esten correctos semanticamente
     */
    void semantic(){

        if(operation == create_)
        {

        }
        else if(operation == delete_)
        {
            //Solo deben de estar los parametros delete, name y path.
            if(_delete != 0 && path !=0 && name != 0)
            {
                if(size == 0 && unit == 0 && type==empty && fit == 0 && add == 0 )
                {
                }
                else
                {
                    correct = false;
                    cout <<  "El comando delete solamente puedes utilizar path y name." << endl;
                }
            }
            else
            {
                correct = false;
                cout << "El comando -delete exige -path y -name obligatorios." << endl;
            }
        }
        else if(operation == modify_)
        {
            if(add != 0 && size != 0 && path!=0 && name != 0)
            {
                if(type== empty && fit == 0 && _delete ==0 )
                {

                }
                else
                {
                    correct = false;
                    cout << "El parametro -add no es compatible con los parámetros -type, -fit ni -delete." << endl;
                }
            }
            else
            {
                correct = false;
                cout << "El comando -add requiere -size, -path y name obligatorios." << endl;
            }
        }
        else
        {
            correct = false;
            cout << "Existe un error en la semántica de la operación" << endl;
            cout << "Verifique no combinar add con delete." << endl;
        }
        return;
    }

    /*
     * Destructor del objeto
     */
    ~FDISK_();
};
