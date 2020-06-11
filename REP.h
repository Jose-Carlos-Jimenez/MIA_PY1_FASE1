#ifndef REP_H
#define REP_H

#endif // REP_H

#include "MOUNT.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <structs.h>
#include "parser.h"
#include <libgen.h>
#include <chrono>
#include <thread>

using namespace std;

class REP_
{
private:
    char *id; /// Identificador de la partición montada.
    char *ruta;/// Destino del reporte.
    string path;/// Ruta auxiliar del disco que se leera.
    char *name;/// Nombre que se le dará al reporte.
    bool correct;/// Semántica de la acción.
    bool isMounted;/// Indica si la partición que se desea está montada.
public:
    /**
     * Constructor
    */
    REP_():id(0),ruta(0),path(""),name(0),correct(false),isMounted(false){}
    /**
     * Configura el id de la partición montada a escribir.
     * @param id: identificador de la partición montada en el sistema.
    */
    void setId(char* id);

    /**
     * Configura la ruta en donde se guardará el reporte.
     * @param path: ruta que se pretende guardar el reporte.
    */
    void setPath(char* path);

    /**
     * Configura la ruta en la que se guardará
     * ya que se necesitan variantes para guardar el dot
     * y para la imagen.
    */
    void confRuta();

    /**
     * Guarda el valor de la ruta
     * @param ruta: dirección en la que se almacenará el reporte.
    */
    void setRuta(char* ruta)
    {
        this->ruta = ruta;
    }

    /**
     * Guarda el nombre de la partición montada que se reportará.
     * @param name: indica el nombre de la partición en el sistema de particiones montadas.
    */
    void setName(char* name)
    {
        this->name = name;
    }

    /*
     * Obtiene el id de la partición.
    */
    char* getId()
    {
        return this->id;
    }

    /*
     * Obtiene la ruta que se eligió como destino.
    */
    char* getRuta()
    {
        return  this->ruta;
    }

    /**
     * Obtiene el nombre que se le dará al rerporte.
    */
    char* getName()
    {
        return  this->name;
    }

    /**
     * Método para verificar semántica de la operación
    */
    void semantic();

    /**
     * Método para ejecutar
    */
    void run();

    /**
     * Método para convertir un string a minúsculas.
    */
    string toLowerString(string input);

    /**
     * Método para generar el reporte de MBR
    */
    void reportMBR();

    /**
     * Método para generar el reporte del disco
    */
    void reportDISK();
};

extern QList<MOUNT_> *mounted;

void REP_::setId(char *id)
{
    this->id = id;
    if(id[0] == '\"')
    {
        string aux = id;
        aux = aux.substr(1,aux.size()-2);
        strcpy(this->id, aux.c_str());
    }
}

void REP_::setPath(char *id)
{
    this->path = "";
    path.append(id);
    if(path[0]== '\"')
    {
        path=path.substr(1, path.size()-2);
    }
}

void REP_::confRuta()
{
    QList<MOUNT_>::iterator i;
    cout << "Buscando montada... " << endl;
    for(i=mounted->begin(); i != mounted->end(); i++)
    {
        cout << i->getId() << endl;
        if(i->getId() == this->getId())
        {
            char letras[255];
            strcpy(letras,i->getPath().c_str());
            this->ruta =letras;
            this->isMounted = true;
            break;
        }
    }
}

void REP_::semantic()
{
    if(this->getId() !=0 && this->getName() != 0 && this->path != "")
    {
        this->correct = true;
    }
}

void REP_::run()
{
    semantic();
    if(this->correct)
    {
        this->confRuta();
        if(isMounted)
        {
            string nameS(this->name);
            if(toLowerString(nameS) == "mbr")
            {
                reportMBR();
            }
            else if(toLowerString(nameS) == "disk")
            {
                reportDISK();
            }

        }
        else
        {
            cout << "La partición que deseas reportar no está montada." << endl;
        }
    }
    else
    {
        cout << "Faltan parámetros para generar el reporte." << endl;
    }
}

string REP_::toLowerString(string input)
{
    std::for_each(input.begin(), input.end(), [](char & c){
        c = ::tolower(c);
    });
    return input;
}

void REP_::reportMBR()
{
    string dir = this->getRuta();//Debo cambiar esta ruta por la que encuentre de la partición montada
    FILE *f;
    if((f = fopen(dir.c_str(),"rb+")))
    {
        string dot = "digraph DISK_MBR{\n";
        dot+= "MBR[\n";
        dot+="shape=none;label=<<TABLE CELLSPACING='-1' CELLBORDER='1' WIDTH='800' HEIGHT= '1000'>\n";
        dot+= "<tr><td WIDTH='400'>NOMBRE</td><td WIDTH='400'>VALOR</td></tr>";
        MBR m;
        fseek(f,0,SEEK_SET);
        fread(&m,sizeof(MBR),1,f);
        dot += "<tr>  <td><b>mbr_tamaño</b></td><td>";
        dot += to_string(m.mbr_tamano);
        dot +=" bytes</td>  </tr>\n";
        struct tm *tm;
        char fecha[100];
        tm = localtime(&m.mbr_fecha_creacion);
        strftime(fecha,100,"%d/%m/%y %H:%S", tm);
        dot+= "<tr>  <td><b>mbr_fecha_creacion</b></td> <td>";
        dot+=fecha;
        dot+= "</td>  </tr>\n";
        dot+="<tr>  <td><b>mbr_disk_signature</b></td> <td>";
        dot+=to_string(m.mbr_disk_signature);
        dot+= "</td>  </tr>\n";
        dot+= "<tr>  <td><b>Disk_fit</b></td> <td>";
        dot.push_back(m.disk_fit);
        dot+= "</td>  </tr>\n";
        int ext = -1;
        for (int i = 0; i < 4; i++){
            if(m.mbr_partitions[i].part_start!=-1 && m.mbr_partitions[i].part_status!='1'){
                if(m.mbr_partitions[i].part_type == 'E')ext = i;
                char status[3];
                if(m.mbr_partitions[i].part_status == '0')strcpy(status,"0");
                else if(m.mbr_partitions[i].part_status == '2')strcpy(status,"2");
                dot+= "<tr>  <td><b>part_status_";
                dot+= to_string(i+1);
                dot+= "</b></td> <td>";
                dot.push_back(m.mbr_partitions[i].part_status);
                dot+= "</td>  </tr>\n";
                dot+= "<tr>  <td><b>part_type_";
                dot+= to_string(i+1);
                dot+="</b></td> <td>";
                dot.push_back(m.mbr_partitions[i].part_type);
                dot+= "</td>  </tr>\n";
                dot+= "<tr>  <td><b>part_fit_";
                dot+= to_string(i+1);
                dot+= "</b></td> <td>";
                dot.push_back(m.mbr_partitions[i].part_fit);
                dot+= "</td>  </tr>\n";
                dot+= "<tr>  <td><b>part_start_";
                dot+= to_string(i+1);
                dot+= "</b></td> <td>";
                dot+= to_string(m.mbr_partitions[i].part_start);
                dot+= "</td>  </tr>\n";
                dot+= "<tr>  <td><b>part_size_";
                dot+= to_string(i+1);
                dot+= "</b></td> <td>";
                dot+= to_string(m.mbr_partitions[i].part_size);
                dot+= "</td>  </tr>\n";
                dot+= "<tr>  <td><b>part_name_";
                dot+= to_string(i+1);
                dot+= "</b></td> <td>";
                dot+= m.mbr_partitions[i].part_name;
                dot+= "</td>  </tr>\n";
            }
        }
        dot += "</TABLE>\n>];\n";

        if(ext != -1){
            int index_ebr = 1;
            EBR ebr;
            fseek(f,m.mbr_partitions[ext].part_start,SEEK_SET);
            while(fread(&ebr,sizeof(EBR),1,f)!=0 && (ftell(f) < m.mbr_partitions[ext].part_start + m.mbr_partitions[ext].part_size)) {

                if(ebr.part_status != '1'){
                    dot+="subgraph cluster_";
                    dot+= to_string(index_ebr);
                    dot+= "{\n label=\"EBR_";
                    dot+= to_string(index_ebr);
                    dot+="\"\n";
                    dot+= "\ntbl_";
                    dot+= to_string(index_ebr);
                    dot+= "[shape=box, label=<\n ";
                    dot+= "<TABLE border='0' cellborder='1' cellspacing='0'  width='300' height='160' >\n ";
                    dot+= "<tr>  <td width='150'><b>Nombre</b></td> <td width='150'><b>Valor</b></td>  </tr>\n";
                    char status[3];
                    if(ebr.part_status == '0')strcpy(status,"0");
                    else if(ebr.part_status == '2')strcpy(status,"2");
                    dot+="<tr>  <td><b>part_status_1</b></td> <td>";
                    dot+=status;
                    dot+= "</td>  </tr>\n";
                    dot+= "<tr>  <td><b>part_fit_1</b></td> <td>";
                    dot.append(1,ebr.part_fit);
                    dot+= "</td>  </tr>\n";
                    dot+= "<tr>  <td><b>part_start_1</b></td> <td>";
                    dot+= to_string(ebr.part_start);
                    dot+= "</td>  </tr>\n";
                    dot+= "<tr>  <td><b>part_size_1</b></td> <td>";
                    dot+= to_string(ebr.part_size);
                    dot+= "</td>  </tr>\n";
                    dot+= "<tr>  <td><b>part_next_1</b></td> <td>";
                    dot+= to_string(ebr.part_next);
                    dot+= "</td>  </tr>\n";
                    dot+= "<tr>  <td><b>part_name_1</b></td> <td>";
                    dot+= ebr.part_name;
                    dot+= "</td>  </tr>\n";
                    dot+= "</TABLE>\n>];\n}\n";
                    index_ebr++;
                }
                if(ebr.part_next == -1)break;
                else fseek(f,ebr.part_next,SEEK_SET);
            }
        }
        dot+="}\n";
        fclose(f);

        // Obtener la ruta.
        string pathClone = this->path;
        string pathTwice = this->path;
        string terminacion;
        pathTwice += ".txt";
        const size_t last_slash_idx = pathClone.find_last_of(".");
        if (std::string::npos != last_slash_idx)
        {
            terminacion = pathClone.substr(last_slash_idx, pathClone.length());
            pathClone = pathClone.substr(0, last_slash_idx);
        }

        // Creando comando para crear la carpeta.
        char* ruta_aux = new char(this->path.length());
        strcpy(ruta_aux, this->path.c_str());
        char* path = new char(this->path.length());
        strcpy(path,this->path.c_str());
        string comando = "sudo mkdir -p \'";
        comando+= dirname(path);
        comando+= '\'';
        system(comando.c_str());

        // Comando para permisos.
        string comandow = "sudo chmod -R 777 \'";
        comandow+= dirname(ruta_aux);
        comandow += '\'';
        system(comandow.c_str());

        string cEliminar = "sudo rm '";
        cEliminar += pathTwice + "\'";
        system(cEliminar.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // Creando el file.
        FILE *nuevo = fopen(pathTwice.c_str(),"w+");
        freopen(NULL,"w+",nuevo);
        char dest[dot.length()];
        strcpy(dest,dot.c_str());
        fprintf(nuevo,"%s\n",dest);
        fclose(nuevo);
        string comandoS = "dot \'";
        comandoS += pathTwice;
        comandoS +="\' -o \'" + pathClone + terminacion + " \' -T" + terminacion.substr(1,terminacion.length());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        system(comandoS.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        cout << "Reporte del MBR ha sido creado." << endl;
    }
    else
    {
        cout << "El disco no existe." << endl;
    }

}

void REP_::reportDISK()
{
    string pathCopy = this->getRuta();//Debo de ponerle la path obtenida
    FILE *fp;
    if((fp = fopen(pathCopy.c_str(),"r"))){
        string dest ="digraph G{\n\n";
        dest+= "tbl[\nshape=box\nlabel=<\n";
        dest+=  "<table border='0' cellborder='2' width='500' height=\"180\">\n";
        dest+=  "<tr>\n";
        dest+=  "<td height='200' width='100'> MBR </td>\n";
        MBR master;
        fseek(fp,0,SEEK_SET);
        fread(&master,sizeof(MBR),1,fp);
        int size = master.mbr_tamano;
        double used = 0;
        for(int i = 0; i < 4; i++){
            int partSize = master.mbr_partitions[i].part_size;
            if(master.mbr_partitions[i].part_start > -1){

                double op1=size;
                double op2=partSize;
                double real_ = (op2/op1);
                double real = real_*100.0;

                double width = 200;
                used += real;
                if(master.mbr_partitions[i].part_status != '1'){
                    if(master.mbr_partitions[i].part_type == 'E')//
                    {
                        EBR ebr;
                        dest+= "<td  height='200' width='100'>\n     <table border='0'  height='200' WIDTH='100' cellborder='1'>\n";
                        dest+= "<tr>  <td height='60' colspan='15'>EXTENDIDA</td>  </tr>\n     <tr>\n";
                        fseek(fp, master.mbr_partitions[i].part_start,SEEK_SET);
                        fread(&ebr,sizeof(EBR),1,fp);
                        if(ebr.part_size != 0){
                            fseek(fp, master.mbr_partitions[i].part_start,SEEK_SET);
                            while(fread(&ebr,sizeof (EBR),1,fp)!=0 && (ftell(fp) < (master.mbr_partitions[i].part_start + master.mbr_partitions[i].part_size))){
                                double parcial = ebr.part_size;
                                double porcentaje_real = (parcial*100)/size;
                                if(porcentaje_real!=0){
                                    if(ebr.part_status != '1'){
                                        dest+=  "<td height='200' width='75'>EBR</td>\n";
                                        dest+=  "     <td height='200' width='150'>LOGICA<br/>Porcentaje: ";
                                        dest += to_string(porcentaje_real);
                                        dest+= "%</td>\n";
                                    }else{
                                        dest+=  "      <td height='200' width='150'>LIBRE <br/> Porcentaje: ";
                                        dest+= to_string(porcentaje_real);
                                        dest+= "%</td>\n";
                                    }
                                    if(ebr.part_next==-1){
                                        parcial = (master.mbr_partitions[i].part_start + master.mbr_partitions[i].part_size) - (ebr.part_start + ebr.part_size);
                                        porcentaje_real = (parcial*100)/size;
                                        if(porcentaje_real!=0){
                                            dest+=  "     <td height='200' width='150'>LIBRE <br/> Porcentaje: ";
                                            dest+= to_string(porcentaje_real);
                                            dest+="% </td>\n";
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        fseek(fp,ebr.part_next,SEEK_SET);
                                    }
                                }
                            }
                        }else{
                            dest+= "     <td height='140'> Utilizado: ";
                            dest+= real;
                            dest+= "%</td>";
                        }
                        dest+= "     </tr>\n     </table>\n     </td>\n";

                        if(i!=3){
                            int p1 = master.mbr_partitions[i].part_start + master.mbr_partitions[i].part_size;
                            int p2 = master.mbr_partitions[i+1].part_start;
                            if(master.mbr_partitions[i+1].part_start != -1){
                                if((p2-p1)!=0){
                                    int fragmentacion = p2-p1;
                                    double real = (fragmentacion*100)/size;
                                    double porcentaje_aux = 200;
                                    dest+= "     <td height='200' width='";
                                    dest+= to_string(porcentaje_aux);
                                    dest+= "'>LIBRE<br/> Utilizado: ";
                                    dest+= to_string(real);
                                    dest+= "%</td>\n";
                                }
                            }
                        }else{
                            int p1 = master.mbr_partitions[i].part_start + master.mbr_partitions[i].part_size;
                            int mbr_size = size + (int)sizeof(MBR);
                            if((mbr_size-p1)!=0){
                                double libre = (mbr_size - p1) + sizeof(MBR);
                                double real = (libre*100)/size;
                                double porcentaje_aux = 100;
                                dest+=  "     <td height='200' width='";
                                dest+= to_string(porcentaje_aux);
                                dest+="'>LIBRE<br/> Utilizado:: ";
                                dest+= to_string(real);
                                dest+="%</td>\n";
                            }
                        }
                    }else{
                        dest+=  "<td height='200' width='200'>PRIMARIA <br/> Utilizado: ";
                        dest+= to_string(real);
                        dest+= "%</td>\n";
                        if(i==3){
                            int suposed = master.mbr_partitions[i].part_start + master.mbr_partitions[i].part_size;
                            int mbr_size = size + (int)sizeof(MBR);
                            if((mbr_size-suposed)!=0){
                                double free = (mbr_size - suposed) + sizeof(MBR);
                                double real = (free*100)/size;
                                double aux = real*4;
                                dest+=  "<td height='200' width='";
                                dest+= to_string(aux);
                                dest+= "'>LIBRE<br/> Utilizado: ";
                                dest+= to_string(real);
                                dest+="%</td>\n";
                            }
                        }else{
                            int suposed = master.mbr_partitions[i].part_start + master.mbr_partitions[i].part_size;
                            int exact = master.mbr_partitions[i+1].part_start;
                            if(master.mbr_partitions[i+1].part_start != -1){
                                if((exact-suposed)!=0){
                                    int fragment = exact-suposed;
                                    double _real = (fragment*100)/size;
                                    double aux = (_real*500)/100;
                                    dest+= "<td height='200' width='";
                                    dest+= to_string(aux);
                                    dest+="'>LIBRE<br/> Utilizado: ";
                                    dest+="%</td>\n";
                                }
                            }
                        }
                    }
                }else{
                    dest+= "     <td height='200' width='";
                    dest+= to_string(width);
                    dest+= "'>LIBRE <br/> Utilizado: ";
                    dest+= to_string(real);
                    dest+= "%</td>\n";
                }
            }
        }
        dest+= "</tr> \n     </table>        \n>];\n\n}";

        // Obtener la ruta.
        string pathClone = this->path;
        string pathTwice = this->path;
        string terminacion;
        pathTwice += ".txt";
        const size_t last_slash_idx = pathClone.find_last_of(".");
        if (std::string::npos != last_slash_idx)
        {
            terminacion = pathClone.substr(last_slash_idx, pathClone.length());
            pathClone = pathClone.substr(0, last_slash_idx);
        }

        // Creando comando para crear la carpeta.
        char* ruta_aux = new char(this->path.length());
        strcpy(ruta_aux, this->path.c_str());
        char* path = new char(this->path.length());
        strcpy(path,this->path.c_str());
        string comando = "sudo mkdir -p \'";
        comando+= dirname(path);
        comando+= '\'';
        system(comando.c_str());

        // Comando para permisos.
        string comandow = "sudo chmod -R 777 \'";
        comandow+= dirname(ruta_aux);
        comandow += '\'';
        system(comandow.c_str());

        //Eliminando el reporte previo
        string cEliminar = "sudo rm '";
        cEliminar += pathTwice + "\'";
        system(cEliminar.c_str());
        string dotF = dest;
        char arreglo[dotF.length()];
        strcpy(arreglo, dotF.c_str());
        // Creando el file.
        FILE *nuevo = fopen(pathTwice.c_str(),"w");
        fprintf(nuevo,"%s\n",arreglo);
        fclose(nuevo);
        string comandoS = "dot \'";
        comandoS += pathTwice;
        comandoS +="\' -o \'" + pathClone + terminacion + " \' -T" + terminacion.substr(1,terminacion.length());
        system(comandoS.c_str());
        cout << "Reporte del DISK ha sido creado." << endl;
    }
}
