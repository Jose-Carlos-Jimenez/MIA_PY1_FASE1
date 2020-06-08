#ifndef REP_H
#define REP_H

#endif // REP_H

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <structs.h>

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
        return  this->ruta;
    }
    char* getName()
    {
        return  this->name;
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
                reportMBR();
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

    /*
     * Método para convertir un string a minúsculas.
    */
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
    void reportMBR()
    {
        string dir = this->getRuta();
        FILE *f;
        if((f = fopen(dir.c_str(),"r")))
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
                        dot+= to_string(ext);
                        dot+="\"\n";
                        dot+= "\ntbl_";
                        dot+= to_string(ext);
                        dot+= "[shape=box, label=<\n ";
                        dot+= "<TABLE border=\'0\' cellborder=\'1\' cellspacing=\'0\'  width=\'300\' height=\'160\' >\n ";
                        dot+= "<tr>  <td width=\'150\'><b>Nombre</b></td> <td width=\'150\'><b>Valor</b></td>  </tr>\n";
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
            FILE *nuevo = fopen("reporte_mbr.txt","w");
            fprintf(nuevo,"%s\n",dot.c_str());
            fclose(nuevo);
            string comando = "dot reporte_mbr.txt -o reporte_mbr.png -Tpng -Gcharset=utf8";
            system(comando.c_str());
        }
        else
        {
            cout << "El disco no existe." << endl;
        }

    }
};
