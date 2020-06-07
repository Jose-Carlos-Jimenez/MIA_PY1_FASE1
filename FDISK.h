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
     * Directorio en donde se creara la particion
     */
    char *path;
    /*
     * Directorio para actualizar el raid
    */
    string raid_path;
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
    FDISK_():size(0),fit(0),_delete(0),add(0),type(empty),operation(create_){};

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
        raid_path = path;
        this->raid_path = raid_path.substr(0, raid_path.size()-5) + "_raid.disk";

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
        char aux [2];
        strcpy(aux,fit);
        return toupper(aux[0]);
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
            break;
        case empty:
            printf("El tipo de partición no ha podido ser establecido, operación abortada.\n");
            break;
        default:
            break;
        }
    }
    /*
     * Método para crear partición primaria.
    */
    void primaryPart(kink which)
    {
        // Abrir el MBR del disco para validar
        FILE *file;
        if(which == principal){file = fopen(path,"rb+");}
        else {
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
                        for(int i = 0; i < bytes; i++){
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
            printf("Espacio insuficiente para esta partición en el disco.\n");
        }



    }

    /*
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
                            eboot.part_size = this->size;
                            eboot.part_start = master.mbr_partitions[partIndex].part_start;
                            eboot.part_status = '0';
                            fwrite(&eboot.part_name, sizeof (EBR),1,file);
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
                            eboot.part_size = this->size;
                            eboot.part_start = master.mbr_partitions[best].part_start;
                            eboot.part_status = '0';
                            fwrite(&eboot.part_name, sizeof (EBR),1,file);
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
                            eboot.part_size = this->size;
                            eboot.part_start = master.mbr_partitions[worst].part_start;
                            eboot.part_status = '0';
                            fwrite(&eboot.part_name, sizeof (EBR),1,file);
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
        fclose(file);
    }

    /*
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

    /*
     * Mensaje de confirmación de creación de particiones.
    */
    void printEnd(kink which)
    {
        switch (which) {
        case principal:
            printf("Partición creada con éxito.\n");
            break;
        case raid:
            printf("Respaldo actualizado.\n");
            break;
        }
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
                deletePart();
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
            DebugBinario();
        }

    }

    /*
     * Método para imprimir el MBR
    */
    void DebugBinario()
    {
        FILE *file = fopen(path, "rb+");
        cout << "Debugueando..." << endl;
        fseek(file,0,SEEK_SET);
        MBR master;
        fread(&master,sizeof (MBR),1,file);
        cout << master.mbr_fecha_creacion << endl;
        fclose(file);
    }
};

// fdisk -path=/home/jose/d.disk -size=8 -unit=k -type=e -name=part1 -fit=FF
