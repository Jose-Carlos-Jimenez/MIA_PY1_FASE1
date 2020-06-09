#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <QString>
#include <QDebug>
#include <string>
#include <iostream>
#include <scanner.h>
#include <parser.h>
#include <fstream>
#include <cctype>

using namespace std;

extern int yylex(void);
extern char *yytext;
extern int SourceLine;
extern FILE *yyin;
extern int yyparse(void);

string lowerCase(string n)
{
    string res;
    if( n != "")
    {
        foreach (char c, n) {
            res+=tolower(c);
        }
        return res;
    }
    return n;
}

void parsear()
{
    system("clear");
    while(true)
    {
        string operacion;
        std::cout << "\u001b[31m" << "[MIA] :" << "\x1B[0m";
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
        else if(lowerCase(operacion.substr(0,4))== "exec")
        {

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
                qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<Operaciones terminadas >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
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
                qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<Operaciones terminadas >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
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
