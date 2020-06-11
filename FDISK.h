#ifndef FDISK_H
#define FDISK_H

#endif // FDISK_H

#include "MOUNT.h"

#include <QList>
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<structs.h>

using namespace  std;
extern QList<MOUNT_> *mounted;

/**
  @brief
 * Clase en la que se programaron el análisis semantico de la función FDISK
 * además se implemento su funcionamiento y validación de parámetros.
 *
*/
class FDISK_
{
private:
    /**
     * Tamaño de la particion
     */
    int size;
    /**
     * Unidad del tamaño de la particion
     */
    char unit;
    /**
     * Directorio en donde se creara la particion
     */
    char *path;
    /**
     * Directorio para actualizar el raid
    */
    string raid_path;
    /**
     * Ajuste de la particion
     */
    char *fit;
    /**
     * Forma de eliminar
     */
    char *_delete;
    /**
     * Nombre de la particion
     */
    char *name;
    /**
     * Añade o remueve segun sea el signo del numero, espacio en la particion
     */
    int add;
    /**
     * Tipo de partición que se creara
    */
    enum type{P, E, L, empty};
    type type;
    /**
     * Tipo de operacion
     */
    enum operation{create_, delete_, modify_};
    operation operation;
    /**
     * enum para ver que disco se está escribiendo.
    */
    enum kink{principal, raid};
    /**
     * Identificara si la instruccion FDISK es valida
     */
    bool correct;


public:
    /**
     * Constructor vacio del objeto
     */
    FDISK_():size(0),fit(0),_delete(0),add(0),type(empty),operation(create_){};

    /**
     * Modificador del atributo size
     * @param size: indica el tamaño de la partición que se desea realizar.
     */
    void setSize(char* size)
    {
        this->size = stoi(size);
    }

    /**
     * Modificador del atributo unit
     * @param unit: indica la unidad de medida del parametro size.
     * Megabytes = M|m
     * Kilobytes = K|k
     * Bytes = B|b
    */
    void setUnit(char* unit)
    {
        this->unit = unit[0];
    }

    /**
     * Modificador del atributo path
     * @param path: indica en donde se guardará el disco.
    */
    void setPath(char *path)
    {
        this->path = path;
        string aux = path;
        if(aux[0] == '\"')
        {
            aux = aux.substr(1,aux.size()-2);
            strcpy(this->path, aux.c_str());
        }
        raid_path = path;
        this->raid_path = raid_path.substr(0, raid_path.size()-5) + "_raid.disk";

    }

    /**
     * Modificador del tipo de ajuste
     * @param fit: indica el ajuste con el que las particiones se insertarán al disco.
     *
    */
    void setFit(char* fit)
    {
        this->fit = fit;
    }

    /**
     * Modificador del atributo delete
     * @param delete: indica que tipo de borrado se aplicará para una partición.
    */
    void setDelete(char* _delete)
    {
        this->_delete = _delete;
        this->operation = delete_;
    }

    /**
     * Modificador del atributo name.
     * @param name: indica el nombre de la partición.
    */
    void setName(char *name)
    {
        this->name = name;
    }

    /**
     * Modificador del atributo add.
     * @param add: Indica la cantidad de espacio que se agregará o disminuirá la partición.
    */
    void setAdd(char* add)
    {
        this->add = atoi(add);
        this->operation = modify_;
    }

    /**
     * Modificador del atributo correct.
     * @param correct: identifica si la semántica del programa es correcta.
    */
    void setCorrect(bool correct)
    {
        this->correct = correct;
    }

    /**
     * Modificador de tipo
     * @param type: Indica que tipo de operación hará el comando.
    */
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

    /**
     * Acceso a los atributos del objeto, se hicieron a conveniencia
     * por lo cual no siguen la típica convención.
    */
    char getFit()
    {
        if(this->fit==0)return'W';
        char aux [2];
        strcpy(aux,fit);
        return toupper(aux[0]);
    }

    /**
     * Acceso a la unidad, la letra de la unidad que se estableció
     * en formato estándar según el enunciado.
    */
    char getUnit()
    {
        if(this->unit==0)return 'K';
        return toupper(this->unit);
    }

    /**
     * Permite obtener el tamaño de la partición a realizar
     * según la unidad que fue indicada.
    */
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

    /**
     * Nos indica el tipo de partición sobre la cual se esta trabajando.
    */
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

    /**
     * Metodo para crear particion
     */
    void createPart(){
        switch (type) {
        case P:
            primaryPart(principal);
            primaryPart(raid);
            break;
        case E:
            extendedPart(principal);
            extendedPart(raid);
            break;
        case L:
            logicPart(principal);
            logicPart(raid);
            break;
        case empty:
            printf("El tipo de partición no ha podido ser establecido, operación abortada.\n");
            break;
        default:
            break;
        }
    }

    /**
     * Método para crear partición primaria.
    */
    void primaryPart(kink which)
    {
        // Abrir el MBR del disco para validar
        FILE *file;
        if(which == principal)
        {
            file = fopen(path,"rb+");
        }
        else
        {
            file = fopen(raid_path.c_str(),"rb+");
        }
        MBR master;
        fseek(file,0,SEEK_SET);
        fread(&master,sizeof(MBR),1,file);

        // ¿Existe una partición libre?
        bool partition=false;;
        int partIndex= -1;
        for(int i = 0; i < 4; i++)
        {
            if(master.mbr_partitions[i].part_start == -1 ||(master.mbr_partitions[i].part_status== '1' && master.mbr_partitions[i].part_size >= getSize()))
            {
                partition=true;
                partIndex= i;
                break;
            }
        }
        if(partition)
        {
            // ¿Es suficiente el espacio en el disco para la nueva partición?
            int bytes=0;
            for(int i= 0; i < 4; i++)
            {
                if(master.mbr_partitions[i].part_status!='1')
                {
                    bytes += master.mbr_partitions[i].part_size;
                }
            }
            if( which==principal)
            {
                printf("Espacio disponible: %i bytes\n", (master.mbr_tamano - bytes));
                printf("Espacio necesario: %i bytes\n", getSize());
            }
            bool fitsIn = (master.mbr_tamano - bytes) >= getSize();
            if(fitsIn)
            {
                //¿Aún no existe la partición?
                if(!doesExist(this->name, which))
                {
                    if(master.disk_fit=='F')
                    {
                        // Seteando la metadata de la partición.
                        if(partIndex ==0)
                        {
                            master.mbr_partitions[partIndex].part_start = sizeof (MBR);
                        }
                        else
                        {
                            master.mbr_partitions[partIndex].part_start = master.mbr_partitions[partIndex-1].part_start + master.mbr_partitions[partIndex-1].part_size;
                        }
                        master.mbr_partitions[partIndex].part_size = getSize();
                        master.mbr_partitions[partIndex].part_status = '0';
                        master.mbr_partitions[partIndex].part_type = 'P';
                        master.mbr_partitions[partIndex].part_fit = getFit();
                        strcpy(master.mbr_partitions[partIndex].part_name, this->name);

                        // Guardamos el MBR actualizado
                        fseek(file,0, SEEK_SET);
                        fwrite(&master,sizeof (MBR),1,file);

                        // Llenando el espacio reservado
                        char buff = '1';//Para llenar byte a byte
                        fseek(file,master.mbr_partitions[partIndex].part_start,SEEK_SET);
                        for(int i = 0; i < getSize(); i++){
                            fwrite(&buff,1,1,file);
                        }
                        printEnd(which);
                    }
                    else if(master.disk_fit == 'B')
                    {
                        int best = partIndex;
                        for(int i = 0; i < 4; i++){//Calculo de la mejor posición
                            if(master.mbr_partitions[i].part_start == -1 || (master.mbr_partitions[i].part_status == '1' && master.mbr_partitions[i].part_size>=getSize())){
                                if(i != partIndex){
                                    if(master.mbr_partitions[i].part_size < master.mbr_partitions[best].part_size ){
                                        best = i;
                                        continue;
                                    }
                                }
                            }
                        }
                        // Seteando la metadata de la partición.
                        if(best ==0)
                        {
                            master.mbr_partitions[best].part_start = sizeof (MBR);
                        }
                        else
                        {
                            master.mbr_partitions[best].part_start = master.mbr_partitions[best-1].part_start + master.mbr_partitions[best-1].part_size;
                        }
                        master.mbr_partitions[best].part_size = getSize();
                        master.mbr_partitions[best].part_status = '0';
                        master.mbr_partitions[best].part_type = 'P';
                        master.mbr_partitions[best].part_fit = getFit();
                        strcpy(master.mbr_partitions[best].part_name, this->name);

                        // Guardamos el MBR actualizado
                        fseek(file,0, SEEK_SET);
                        fwrite(&master,sizeof (MBR),1,file);

                        // Llenando el espacio reservado
                        char buff = '1';//Para llenar byte a byte
                        fseek(file,master.mbr_partitions[best].part_start,SEEK_SET);
                        for(int i = 0; i < bytes; i++){
                            fwrite(&buff,1,1,file);
                        }
                        printEnd(which);
                    }
                    else if(master.disk_fit == 'W')
                    {
                        int  worst= partIndex;
                        for(int i = 0; i < 4; i++){
                            if(master.mbr_partitions[i].part_start == -1 || (master.mbr_partitions[i].part_status == '1' && master.mbr_partitions[i].part_size>=getSize())){
                                if(i != partIndex){
                                    if(master.mbr_partitions[worst].part_size < master.mbr_partitions[i].part_size){
                                        worst= i;
                                        break;
                                    }
                                }
                            }
                        }
                        // Seteando la metadata de la partición.
                        if(worst ==0)
                        {
                            master.mbr_partitions[worst].part_start = sizeof (MBR);
                        }
                        else
                        {
                            master.mbr_partitions[worst].part_start = master.mbr_partitions[worst-1].part_start + master.mbr_partitions[worst-1].part_size;
                        }
                        master.mbr_partitions[worst].part_size = getSize();
                        master.mbr_partitions[worst].part_status = '0';
                        master.mbr_partitions[worst].part_type = 'P';
                        master.mbr_partitions[worst].part_fit = getFit();
                        strcpy(master.mbr_partitions[worst].part_name, this->name);

                        // Guardamos el MBR actualizado
                        fseek(file,0, SEEK_SET);
                        fwrite(&master,sizeof (MBR),1,file);

                        // Llenando el espacio reservado
                        char buff = '1';//Para llenar byte a byte
                        fseek(file,master.mbr_partitions[worst].part_start,SEEK_SET);
                        for(int i = 0; i < bytes; i++){
                            fwrite(&buff,1,1,file);
                        }
                        printEnd(which);
                    }
                }
                else
                {
                    printf("La partición  %s ya existe, por lo que no se puede volver a crear en este disco.\n", this->name);
                }
            }
            else
            {
                printf("Espacio insuficiente para %s en el disco.\n", this->name);
            }

        }
        else
        {
            printf("Espacio insuficiente para %s en el disco.\n",this->name);
        }
        fclose(file);



    }

    /**
     * Método para crear partición extendida.
    */
    void extendedPart(kink which)
    {
        FILE *file;
        if(which == principal){file = fopen(path,"rb+");}
        else {
            file = fopen(raid_path.c_str(),"rb+");
        }
        MBR master;
        fseek(file,0,SEEK_SET);
        fread(&master,sizeof(MBR),1,file);
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
                    break;
                }
            }
            if(partition)
            {
                int bytes=0;
                for(int i= 0; i < 4; i++)
                {
                    if(master.mbr_partitions[i].part_status!='1')
                    {
                        bytes = bytes + master.mbr_partitions[i].part_size;
                    }
                }
                if( which==principal)
                {
                    printf("Espacio disponible: %i bytes\n", (master.mbr_tamano - bytes));
                    printf("Espacio necesario: %i bytes\n", getSize());
                }
                bool fitsIn = (master.mbr_tamano - bytes) >= getSize();
                if(fitsIn)
                {
                    if(!doesExist(this->name,which))//Despúes de todas las validaciones se puede empezar.
                    {
                        if(master.disk_fit == 'F')
                        {
                            // Seteando la metadata de la partición.
                            if(partIndex ==0)
                            {
                                master.mbr_partitions[partIndex].part_start = sizeof (MBR);
                            }
                            else
                            {
                                master.mbr_partitions[partIndex].part_start = master.mbr_partitions[partIndex-1].part_start + master.mbr_partitions[partIndex-1].part_size;
                            }
                            master.mbr_partitions[partIndex].part_size = getSize();
                            master.mbr_partitions[partIndex].part_status = '0';
                            master.mbr_partitions[partIndex].part_type = 'E';
                            master.mbr_partitions[partIndex].part_fit = getFit();
                            strcpy(master.mbr_partitions[partIndex].part_name, this->name);

                            // Guardamos el MBR actualizado
                            fseek(file,0, SEEK_SET);
                            fwrite(&master,sizeof (MBR),1,file);

                            // Guardamos la partición extendida
                            fseek(file, master.mbr_partitions[partIndex].part_start, SEEK_SET);
                            EBR eboot;
                            eboot.part_fit = getFit();
                            strcpy(eboot.part_name,this->name);
                            eboot.part_next = -1;
                            eboot.part_size = 0;
                            eboot.part_start = master.mbr_partitions[partIndex].part_start;
                            eboot.part_status = '0';
                            fwrite(&eboot, sizeof (EBR),1,file);
                            int ebrSize = (int)sizeof (EBR);
                            int top = getSize() - ebrSize;
                            char buff = '1';//Para llenar byte a byte
                            for(int i = top; i > 0 ; i--)
                            {
                                fwrite(&buff,1,1,file);
                            }
                            printEnd(which);
                        }
                        else if(master.disk_fit == 'B')
                        {
                            int best = partIndex;
                            for(int i = 0; i < 4; i++){//Calculo de la mejor posición
                                if(master.mbr_partitions[i].part_start == -1 || (master.mbr_partitions[i].part_status == '1' && master.mbr_partitions[i].part_size>=getSize())){
                                    if(i != partIndex){
                                        if(master.mbr_partitions[i].part_size < master.mbr_partitions[best].part_size ){
                                            best = i;
                                            continue;
                                        }
                                    }
                                }
                            }

                            // Seteando la metadata de la partición.
                            if(best ==0)
                            {
                                master.mbr_partitions[best].part_start = sizeof (MBR);
                            }
                            else
                            {
                                master.mbr_partitions[best].part_start = master.mbr_partitions[best-1].part_start + master.mbr_partitions[best-1].part_size;
                            }
                            master.mbr_partitions[best].part_size = getSize();
                            master.mbr_partitions[best].part_status = '0';
                            master.mbr_partitions[best].part_type = 'E';
                            master.mbr_partitions[best].part_fit = getFit();
                            strcpy(master.mbr_partitions[best].part_name, this->name);

                            // Guardamos el MBR actualizado
                            fseek(file,0, SEEK_SET);
                            fwrite(&master,sizeof (MBR),1,file);

                            // Guardamos la partición extendida
                            fseek(file, master.mbr_partitions[best].part_start, SEEK_SET);
                            EBR eboot;
                            eboot.part_fit = getFit();
                            strcpy(eboot.part_name,this->name);
                            eboot.part_next = -1;
                            eboot.part_size = 0;
                            eboot.part_start = master.mbr_partitions[best].part_start;
                            eboot.part_status = '0';
                            fwrite(&eboot, sizeof (EBR),1,file);
                            int ebrSize = (int)sizeof (EBR);
                            int top = getSize() - ebrSize;
                            char buff = '1';//Para llenar byte a byte
                            for(int i = top; i > 0 ; i--)
                            {
                                fwrite(&buff,1,1,file);
                            }
                            printEnd(which);
                        }
                        else if(master.disk_fit =='W')
                        {
                            int  worst= partIndex;
                            for(int i = 0; i < 4; i++){
                                if(master.mbr_partitions[i].part_start == -1 || (master.mbr_partitions[i].part_status == '1' && master.mbr_partitions[i].part_size>=getSize())){
                                    if(i != partIndex){
                                        if(master.mbr_partitions[worst].part_size < master.mbr_partitions[i].part_size){
                                            worst= i;
                                            break;
                                        }
                                    }
                                }
                            }
                            // Seteando la metadata de la partición.
                            if(worst ==0)
                            {
                                master.mbr_partitions[worst].part_start = sizeof (MBR);
                            }
                            else
                            {
                                master.mbr_partitions[worst].part_start = master.mbr_partitions[worst-1].part_start + master.mbr_partitions[worst-1].part_size;
                            }
                            master.mbr_partitions[worst].part_size = getSize();
                            master.mbr_partitions[worst].part_status = '0';
                            master.mbr_partitions[worst].part_type = 'E';
                            master.mbr_partitions[worst].part_fit = getFit();
                            strcpy(master.mbr_partitions[worst].part_name, this->name);
                            // Guardamos el MBR actualizado
                            fseek(file,0, SEEK_SET);
                            fwrite(&master,sizeof (MBR),1,file);

                            // Guardamos la partición extendida
                            fseek(file, master.mbr_partitions[worst].part_start, SEEK_SET);
                            EBR eboot;
                            eboot.part_fit = getFit();
                            strcpy(eboot.part_name,this->name);
                            eboot.part_next = -1;
                            eboot.part_size = 0;
                            eboot.part_start = master.mbr_partitions[worst].part_start;
                            eboot.part_status = '0';
                            fwrite(&eboot, sizeof (EBR),1,file);
                            int ebrSize = (int)sizeof (EBR);
                            int top = getSize() - ebrSize;
                            char buff = '1';//Para llenar byte a byte
                            for(int i = 0; i < top ; i++)
                            {
                                fwrite(&buff,1,1,file);
                            }
                            printEnd(which);
                        }
                    }
                    else
                    {
                        printf("La partición %s  ya existe, por lo que no se puede volver a crear en este disco.\n",this->name);
                    }
                }
                else
                {
                    printf("Espacio insuficiente para %s en el disco.\n", this->name);
                }
            }
            else
            {
                printf("MUCHO TEXTO, ya existen 4 particiones en este disco.");
            }

        }
        else
        {
            printf("Partición extendida ya existe en este disco.\n");
        }
        fclose(file);
    }

    /**
     * Método para crear una partición lógica.
    */
    void logicPart(kink which)
    {
        FILE *file;
        MBR master;
        if(which == principal){
            file = fopen(path,"rb+");
        }
        else {
            file = fopen(raid_path.c_str(),"rb+");
        }
        if(file != NULL)
        {
            fseek(file,0,SEEK_SET);
            fread(&master,sizeof (MBR),1,file);
            int extIndex;
            bool hayExtendida =false;
            for(int i =0;i<4;i++)
            {
                if(master.mbr_partitions[i].part_type == 'E')
                {
                    hayExtendida = true;
                    extIndex = i;
                }
            }
            if(!doesExist(this->name,which))
            {
                if(hayExtendida)
                {
                    EBR ebr;
                    int init = master.mbr_partitions[extIndex].part_start;//  Voy al inicio de la partición extendida
                    fseek(file,init, SEEK_SET);
                    fread(&ebr, sizeof (EBR),1,file);//Leo el EBR inicial que siempre se crea
                    if(ebr.part_size != 0) // Cuando no sea la primera, el EBR inicial apunta a -1
                    {
                        while((ebr.part_next != -1) && (ftell(file) < (master.mbr_partitions[extIndex].part_size + master.mbr_partitions[extIndex].part_start))){
                            fseek(file,ebr.part_next,SEEK_SET);
                            fread(&ebr,sizeof(EBR),1,file);
                        }
                        int needed= ebr.part_size + ebr.part_start + getSize();
                        cout << "Espacio necesario para la partición: " << to_string(getSize()) << " bytes" << endl;
                        bool fitIn = needed <= (master.mbr_partitions[extIndex].part_size + master.mbr_partitions[extIndex].part_start);
                        if(fitIn)
                        {
                            //Seteamos los datos del EBR actualizados.
                            ebr.part_next = ebr.part_start + ebr.part_size;
                            fseek(file,ftell(file)-sizeof (EBR),SEEK_SET);
                            fwrite(&ebr, sizeof(EBR),1 ,file);

                            fseek(file,ebr.part_start + ebr.part_size, SEEK_SET);
                            ebr.part_fit = getFit();//
                            strcpy(ebr.part_name, this->name);//
                            ebr.part_size = getSize();//
                            ebr.part_next = -1;//
                            ebr.part_status = '0';//
                            ebr.part_start = ftell(file);//
                            fwrite(&ebr,sizeof (EBR),1, file);
                            fseek(file,init,SEEK_SET);
                            fread(&ebr, sizeof (EBR),1, file);
                            printEnd(which);
                        }
                        else
                        {
                            cout << "Espacio insuficiente en la partición "<<this->name<< " extendida." << endl;
                        }
                    }
                    else/**Caso especial en el que sea la partición inicial.*/
                    {
                        cout << "Espacio necesario para la partición " << to_string(getSize()) << endl;
                        if(master.mbr_partitions[extIndex].part_size > getSize())//Si cabe la partición lógica en la totalidad
                        {
                            // Seteando datos
                            ebr.part_fit = getFit();
                            strcpy(ebr.part_name,this->name);
                            ebr.part_next = -1;
                            ebr.part_size = getSize();
                            ebr.part_start = init;
                            ebr.part_status = '0';

                            //Escribirla en memoria
                            fseek(file, init,SEEK_SET);
                            fwrite(&ebr,sizeof (EBR),1,file);
                            printEnd(which);
                        }
                        else
                        {
                            cout << "Espacio insuficiente en la partición extendida." << endl;
                        }
                    }
                }
                else
                {
                    cout << "Se necesita una partición extendida para montar una lógica." << endl;
                }
            }
            else
            {
                cout << "La partición "<< this->name <<" que deseas crear ya existe." << endl;
            }
        }
        else
        {
            cout << "El disco no existe." << endl;
        }
        fclose(file);
    }
    /**
     * Método para verificar que la partición no exista ya.
    */
    bool doesExist(char* name, kink which)
    {
        FILE *file;
        if(which==principal)
        {
            file =fopen(path,"rb+");
        }
        else
        {
            file = fopen(raid_path.c_str(), "rb+");
        }
        MBR _master;
        fseek(file,0,SEEK_SET);
        fread(&_master, sizeof (MBR), 1, file);
        int extendedIndex = -1;
        for(int i = 0; i<4; i++)
        {
            if(strcmp(_master.mbr_partitions[i].part_name, name) == 0)
            {
                return true;
            }
            if(_master.mbr_partitions[i].part_type == 'E')
            {
                extendedIndex = i;
            }
        }
        if(extendedIndex != -1)//En caso de que ya exista una partición extendida
        {
            fseek(file, _master.mbr_partitions[extendedIndex].part_start, SEEK_SET);//Me coloco al inicio de la partición.
            EBR extended;
            while (((ftell(file)< (_master.mbr_partitions[extendedIndex].part_size + _master.mbr_partitions[extendedIndex].part_start))
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

    /**
     * Mensaje de confirmación de creación de particiones.
    */
    void printEnd(kink which)
    {
        switch (which) {
        case principal:
            printf("Partición %s creada con éxito.\n",this->name);
            break;
        case raid:
            printf("Respaldo actualizado.\n");
            break;
        }
    }

    /**
     * Metodo para modificar particion
     */
    void modifyPart(){
        cout << "Operación add exitosa" << endl;
        return;
    }

    /**
     * Verifica que los atributos de la instruccion esten correctos semanticamente
     */
    void semantic(){

        if(operation == create_)
        {
            //size, path y name obligatorios.
            if(size !=0 && path !=0 && name!=0)
            {
                if(add ==0 && _delete == 0)
                {
                }
                else
                {
                    correct = false;
                    cout << "Los parámetros add y delete no son compatibles." << endl;
                }
            }
            else
            {
                correct = false;
                cout << "Al crear los parametros obligatorios son size, path y name." << endl;
            }
        }
        else if(operation == delete_)
        {
            //Solo deben de estar los parametros delete, name y path.
            if(_delete != 0 && path !=0 && name != 0)
            {
                if(size == 0 && type==empty && fit == 0 && add == 0 )
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
            if(add != 0 && path!=0 && name != 0)
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

    /**
     * Método principal para ejecutar la instrucción.
    */
    void run()
    {
        semantic();
        char path_[raid_path.size()+1];
        raid_path.copy(path_, raid_path.size()+1);
        path_[raid_path.size()] = '\0';
        FILE *file = fopen(path, "rb+");
        FILE *raidf = fopen(path_, "rb+");

        if(file == NULL)
        {
            printf("El disco al que intentas acceder no existe.\n");
        }
        else if(raidf == NULL)
        {
            printf("El disco RAID es inaccesible.\n");
        }
        else
        {
            fclose(file);
            fclose(raidf);
            switch (this->operation) {
            case delete_:
                deletePart(principal);
                break;
            case modify_:
                modifyPart();
                break;
            case create_:
                createPart();
                break;
            default:
                printf("No se ha podido establecer que acción desea hacer.\n");
            }
        }

    }

    /**
     * Método para imprimir el MBR
    */
    void DebugBinario()
    {
        FILE *file = fopen(path, "rb+");
        fseek(file,0,SEEK_SET);
        MBR master;
        fread(&master,sizeof (MBR),1,file);
        fclose(file);
    }

    /*
     * Método para eliminar particiones
    */
    void deletePart(kink which){
        FILE *file;
        //Abrir el archivo.
        if((file = fopen(this->path, "r+b"))){
            //Buscarla en la lista de montadas.
            bool mount = false;
            /*QList<MOUNT_>::iterator i;
            for(i = mounted->begin(); i != mounted->end(); i++)
            {
                if(i->getName() == this->name)
                {
                    mount = true;
                }
            }*/

            // Aquí termina la busqueda para determinar esto.
            if(!mount){
                MBR master;
                fseek(file,0,SEEK_SET);
                fread(&master,sizeof (MBR),1,file);
                int index = -1;
                int extIndex = -1;
                bool isExtended = false;
                string opcion = "";
                char buffer = '1';

                /// Buscando la partición
                for(int i = 0; i < 4; i++){
                    if(strcmp(master.mbr_partitions[i].part_name ,name) == 0){
                        index = i;
                        if(master.mbr_partitions[i].part_type == 'E')
                            isExtended = true;
                        break;
                    }else if(master.mbr_partitions[i].part_type == 'E'){
                        extIndex = i;
                    }
                }
                string tipo("full");
                if(index != -1)
                {
                    if(isExtended)
                    {
                        if(strcmp(this->_delete,tipo.c_str()))
                        {
                            master.mbr_partitions[index].part_status = '1';
                            strcpy(master.mbr_partitions[index].part_name,"");
                            fseek(file,0,SEEK_SET);
                            fwrite(&master,sizeof(MBR),1,file);
                            fseek(file,master.mbr_partitions[index].part_start,SEEK_SET);
                            fwrite(&buffer,1,master.mbr_partitions[index].part_size,file);
                            if(which == principal)
                            {
                                cout << "Particion extendida eliminada con exito" << endl;
                            }
                            else
                            {
                                cout << "Partición extendida eliminada del RAID." << endl;
                            }
                        }
                        else
                        {
                            master.mbr_partitions[index].part_status = '1';
                            strcpy(master.mbr_partitions[index].part_name,"");
                            fseek(file,0,SEEK_SET);
                            fwrite(&master,sizeof(MBR),1,file);
                            if(which == principal)
                            {
                                cout << "Particion extendida eliminada con exito" << endl;
                            }
                            else
                            {
                                cout << "Partición extendida eliminada del RAID." << endl;
                            }

                        }
                    }
                    else
                    {
                        if(strcmp(this->_delete,tipo.c_str()))
                        {
                            master.mbr_partitions[index].part_status = '1';
                            strcpy(master.mbr_partitions[index].part_name,"");
                            fseek(file,0,SEEK_SET);
                            fwrite(&master,sizeof(MBR),1,file);
                            fseek(file,master.mbr_partitions[index].part_start,SEEK_SET);
                            fwrite(&buffer,1,master.mbr_partitions[index].part_size,file);
                            if(which == principal)
                            {
                                cout << "Particion primaria eliminada con exito" << endl;
                            }
                            else
                            {
                                cout << "Partición primaria eliminada del RAID." << endl;
                            }
                        }
                        else
                        {
                            master.mbr_partitions[index].part_status = '1';
                            strcpy(master.mbr_partitions[index].part_name,"");
                            fseek(file,0,SEEK_SET);
                            fwrite(&master,sizeof(MBR),1,file);
                            if(which == principal)
                            {
                                cout << "Particion primaria eliminada con exito" << endl;
                            }
                            else
                            {
                                cout << "Partición primaria eliminada del RAID." << endl;
                            }
                        }

                    }
                }
                else
                {
                    if(extIndex != -1)
                    {
                        bool flag = false;
                        EBR extendedBoot;
                        fseek(file,master.mbr_partitions[extIndex].part_start, SEEK_SET);
                        fread(&extendedBoot,sizeof(EBR),1,file);
                        if(extendedBoot.part_size!=0){
                            fseek(file, master.mbr_partitions[extIndex].part_start,SEEK_SET);
                            while((fread(&extendedBoot,sizeof(EBR),1,file))!=0 && (ftell(file) < (master.mbr_partitions[extIndex].part_start + master.mbr_partitions[extIndex].part_size))) {
                                char* n = extendedBoot.part_name;
                                if((strcmp(n,this->name)==0) && extendedBoot.part_status != '1'){
                                    flag = true;
                                    break;
                                }else if(extendedBoot.part_next == -1){
                                    break;
                                }
                                fseek(file,extendedBoot.part_next,SEEK_SET);
                            }
                        }
                        if(flag)
                        {
                            if(strcmp(this->_delete,tipo.c_str()))
                            {
                                extendedBoot.part_status = '1';
                                strcpy(extendedBoot.part_name, "");
                                fseek(file, ftell(file)-sizeof(EBR),SEEK_SET);
                                fwrite(&extendedBoot,sizeof(EBR),1,file);
                                if(which == principal)
                                {
                                    cout << "Particion lógica eliminada con exito" << endl;
                                }
                                else
                                {
                                    cout << "Partición lógica eliminada del RAID." << endl;
                                }
                            }
                            else
                            {
                                extendedBoot.part_status = '1';
                                strcpy(extendedBoot.part_name, "");
                                fseek(file, ftell(file)-sizeof(EBR),SEEK_SET);
                                fwrite(&extendedBoot,sizeof(EBR),1,file);
                                fwrite(&buffer,1,extendedBoot.part_size,file);
                                if(which == principal)
                                {
                                    cout << "Particion lógica eliminada con exito" << endl;
                                }
                                else
                                {
                                    cout << "Partición lógica eliminada del RAID." << endl;
                                }

                            }
                        }
                        else
                        {
                            cout << "La partición que desea eliminar no existe." << endl;
                        }
                    }
                    else
                    {
                        cout << "La partición que desea eliminar no existe." << endl;
                    }
                }
            }
            else
            {
                cout << "La partición aún se encuentra montada, por favor desmontela." << endl;
            }
            fclose(file);
        }
        else
        {
            cout << "El disco especificado para borrar no existe." << endl;
        }
    }
};
