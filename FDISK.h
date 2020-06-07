#ifndef FDISK_H
#define FDISK_H

#endif // FDISK_H

#include<iostream>
#include<stdio.h>
#include<string.h>
#include<structs.h>

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
     * enum para ver que disco se está escribiendo.
    */
    enum kink{principal, raid};
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

    /*
     * Acceso a los atributos del objeto, se hicieron a conveniencia
     * por lo cual no siguen la típica convención.
    */

    char getFit()
    {
        if(this->fit==0)return'W';
        return toupper(this->fit[0]);
    }

    char getUnit()
    {
        if(this->unit==0)return 'K';
        return toupper(this->unit);
    }

    int getSize()
    {
        char u = getUnit();
        if(u== 'B')
        {
            return this->size;
        }
        else if(u == 'K')
        {
            return size*1024;
        }
        else if(u == 'M')
        {
            return size*1024*1024;
        }
        return 0;
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
    void createPart(FILE *file){
        switch (type) {
        case P:
            break;
        case E:
            extendedPart(file,principal);

            break;
        case L:
            break;
        case empty:
            printf("El tipo de partición no ha podido ser establecido, operación abortada.\n");
            break;
        default:
            break;
        }
    }

    /*
     * Método para crear partición extendida.
    */
    void extendedPart(FILE *file, kink which)
    {
        MBR master;
        bool extendida = false;
        for(int i = 0;i < 4;i++) //Ver si ya hay alguna extendida
        {
            if(master.mbr_partitions[i].part_type == 'E')
            {
                extendida=true;
                break;
            }
        }
        if(!extendida)
        {
            bool partition=false;;
            int partIndex= -1;
            for(int i = 0; i < 4; i++)
            {
                if(master.mbr_partitions[i].part_start == -1 ||(master.mbr_partitions[i].part_status== '1' && master.mbr_partitions[i].part_size >= getSize()))
                {
                    partition=true;
                    partIndex= i;
                }
            }
            if(partition)
            {
                int bytes=0;
                for(int i= 0; i < 4; i++)
                {
                if(master.mbr_partitions[i].part_status=='1')
                {
                    bytes = bytes + master.mbr_partitions[i].part_size;
                }
                }
                if( which==principal)
                {
                    printf("Espacio disponible: %i bytes", (master.mbr_tamano - bytes));
                    printf("Espacio necesario: %i bytes", getSize());
                }
                bool fitsIn = (master.mbr_tamano - bytes) >= getSize();
                if(fitsIn)
                {
                    if(!doesExist(file, this->name))//Despúes de todas las validaciones se puede empezar.
                    {

                    }
                    else
                    {
                        printf("La partición ya existe, por lo que no se puede volver a crear en este disco.\n");
                    }
                }
                else
                {
                    printf("Espacio insuficiente para esta partición en el disco.\n");
                }
            }
            else
            {
                printf("MUCHO TEXTO, ya existen 4 particiones en este disco.");
            }

        }
        else
        {
            printf("Partición extendida ya existe en este equipo.\n");
        }
    }

    /*
     * Método para verificar que la partición no exista ya.
    */
    bool doesExist(FILE *file,char* name)
    {
        MBR master;
        fseek(file,0,SEEK_SET);
        fread(&master, sizeof (MBR), 1, file);
        int extendedIndex = -1;
        for(int i = 0; i<4; i++)
        {
            if(strcmp(master.mbr_partitions[i].part_name, name) == 0)
            {
                return true;
            }
            if(master.mbr_partitions[i].part_type == 'E')
            {
                extendedIndex = i;
            }
        }
        if(extendedIndex != -1)//En caso de que ya exista una partición extendida
        {
            fseek(file, master.mbr_partitions[extendedIndex].part_start, SEEK_SET);//Me coloco al inicio de la partición.
            EBR extended;
            while (((ftell(file)< (master.mbr_partitions[extendedIndex].part_size + master.mbr_partitions[extendedIndex].part_start))
                    && fread(&extended, sizeof (EBR),1,file) !=0)) {
                // Mientras se pueda seguir leyendo y exista espacio para leer
                // y que se haya leido con éxito el struct EBR
                if(extended.part_next == -1)
                {
                    return  false;
                }
                if(extended.part_name == name)
                {
                    return  true;
                }
            }
        }
        return  false;
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
     * Método principal para ejecutar la instrucción.
    */
    void run()
    {
        semantic();
        char path_[200];
        strcpy(path_, this->path);
        FILE *file;
        file = fopen(path, "rb+");
        if(file == NULL)
        {
            printf("El disco al que intentas acceder no existe.\n");
        }
        else
        {
            switch (this->operation) {
            case delete_:
                deletePart();
                break;
            case modify_:
                modifyPart();
                break;
            case create_:
                createPart(file);
                break;
            default:
                printf("No se ha podido establecer que acción desea hacer.\n");
            }
        }

    }
};
