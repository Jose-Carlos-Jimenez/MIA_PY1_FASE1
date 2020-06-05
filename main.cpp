#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <QString>
#include <QDebug>
#include <iostream>
#include <scanner.h>
#include <parser.h>
#include <fstream>

using namespace std;

extern int yylex(void);
extern char *yytext;
extern int SourceLine;
extern FILE *yyin;
extern int yyparse(void);

void parsear()
{
    system("clear");
    while(true)
    {
        string operacion;
        cout << "\nMIA ->";
        std::getline(std::cin, operacion);
        QString qString(operacion.c_str());
        if(operacion=="exit")
        {
            system("clear");
            break;
        }
        else if(operacion == "clear")
        {
            system("clear");
        }
        else if(operacion.length() ==0)
        {
        }
        else if(operacion.substr(0,4)== "exec")
        {

            /*exec -path="/home/jose/Archivos/Proyecto 1 [FASE 1]/Archivo de prueba.txt"*/
            string ruta = operacion.substr(operacion.find("=")+1, operacion.length() - 1);
            if(ruta[0]== '\"')
            {
                ruta = ruta.substr(1,ruta.length()-2);
            }
            ifstream fe(ruta);
            string input;
            string linea;
            while(getline(fe, linea))
            {
                input +=linea;
                input += '\n';
            }
            SourceLine = 1;
            YY_BUFFER_STATE bufferState = yy_scan_string(input.c_str());
            if(yyparse()==0)
            {
                qDebug() << "Analizado con éxito";
            }
            else
            {
                qDebug() << "Errores sintácticos en la entrada";
            }
            yy_delete_buffer(bufferState);
        }
        else
        {
            SourceLine = 1;
            YY_BUFFER_STATE bufferState = yy_scan_string(qString.toUtf8().constData());
            if(yyparse()==0)
            {
                qDebug() << "Analizado con éxito";
            }
            else
            {
                qDebug() << "Errores sintácticos en la entrada";
            }
            yy_delete_buffer(bufferState);
        }
    }
}

int main()
{
    parsear();
}
