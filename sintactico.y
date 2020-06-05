
/*------------------------------Código utilizado----------------------------------*/

%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <QString>
#include <QDebug>
#include <iostream>
#include <scanner.h>
#include <typeinfo>
#include <MKDISK.h>
#define YYSTYPE char *

using namespace std;

extern int yylex(void);
extern char *yytext;
extern  int SourceLine;
extern FILE *yyin;

MKDISK_* mkdisk_ = new MKDISK_();

void yyerror( const char *s)
{
    printf("Error sintáctico en la fila %u\n",SourceLine);
    s=s;
}
%}

/*---------------Declaración de tokens utilizados en el léxico--------------------*/

%start INICIO

%token<NUM> numero
%token<NUM> numero_negativo
%token<STRING> full
%token<STRING> fast
%token<STRING> size
%token<STRING> fit
%token<STRING> unit
%token<STRING> path
%token<STRING> type
%token<STRING> name
%token<STRING> delete_o
%token<STRING> add
%token<STRING> mkdisk
%token<STRING> rmdisk
%token<STRING> fdisk
%token<STRING> mount
%token<STRING> unmount
%token<STRING> exec
%token<STRING> rep
%token<STRING> k
%token<STRING> m
%token<STRING> bf
%token<STRING> ff
%token<STRING> wf
%token<STRING> b
%token<STRING> p
%token<STRING> e
%token<STRING> l
%token<STRING> igual
%token<STRING> cadena_esp
%token<STRING> id
%token<STRING> ruta
%token<STRING> guion
%token eof

/*----------------------Declaración de producciones------------------------*/

%type<STRING> INICIO
%type<STRING> INSTRUCCIONES
%type<STRING> INSTRUCCION
%type<STRING> MKDISK
%type<STRING> MKDISKPS
%type<STRING> MKDISKP
%type<STRING> RMDISK
%type<STRING> FDISK
%type<STRING> FDISKPS
%type<STRING> FDISKP
%type<STRING> MOUNT
%type<STRING> MOUNTPS
%type<STRING> MOUNTP
%type<STRING> UNMOUNT
%type<STRING> EXEC
%type<STRING> REP
%type<STRING> REPPS
%type<STRING> REPP

/*-------------------------------- Opciones --------------------------------------*/

%error-verbose
%locations


%union{
char* STRING;
char* NUM;
}

/*------------------ Declaración de la gramática -------------------------*/

%%
INICIO: INSTRUCCIONES;

INSTRUCCIONES: INSTRUCCION
             | INSTRUCCIONES INSTRUCCION
;

INSTRUCCION: MKDISK
           | RMDISK
           | FDISK
           | MOUNT
           | UNMOUNT
           | REP
           | EXEC
;

MKDISK:mkdisk MKDISKPS {mkdisk_->printMk();printf("-------------\n");mkdisk_ = new MKDISK_();}
;

MKDISKPS: MKDISKPS MKDISKP
        | MKDISKP
;

MKDISKP: guion size igual numero {mkdisk_->setSize(yytext);}
       | guion fit igual bf {mkdisk_->setFit(yytext);}
       | guion fit igual ff {mkdisk_->setFit(yytext);}
       | guion fit igual wf {mkdisk_->setFit(yytext);}
       | guion unit igual k {mkdisk_->setUnit(yytext);}
       | guion unit igual m {mkdisk_->setUnit(yytext);}
       | guion path igual ruta {mkdisk_->setPath(yytext);}
       | guion path igual cadena_esp {mkdisk_->setPath(yytext);}
;

RMDISK: rmdisk guion path igual ruta
      | rmdisk guion path igual cadena_esp
;

FDISK: fdisk FDISKPS
;

FDISKPS: FDISKP
       | FDISKPS FDISKP
;
FDISKP:   guion size igual numero
        | guion unit igual b
        | guion unit igual k
        | guion unit igual m
        | guion path igual ruta
        | guion path igual cadena_esp
        | guion type igual p
        | guion type igual e
        | guion type igual l
        | guion fit igual bf
        | guion fit igual ff
        | guion fit igual wf
        | guion delete_o igual fast
        | guion delete_o igual full
        | guion name igual id
        | guion name igual cadena_esp
        | guion add igual numero
        | guion add igual numero_negativo
;

MOUNT: mount MOUNTPS;

MOUNTPS: MOUNTP
       | MOUNTPS MOUNTP
;

MOUNTP:   guion path igual ruta
        | guion path igual cadena_esp
        | guion name igual id
        | guion name igual cadena_esp
;

UNMOUNT: unmount guion id igual id
       | unmount guion id igual cadena_esp
;

EXEC: exec guion path igual ruta
    | exec guion path igual cadena_esp
;

REP: rep REPPS
;

REPPS: REPP
     | REPPS REPP
;

REPP: guion path igual ruta
    | guion path igual cadena_esp
    | guion id igual id
    | guion id igual cadena_esp
    | guion name igual id
    | guion name igual cadena_esp
;
%%

