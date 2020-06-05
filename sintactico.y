
/*------------------------------Código utilizado----------------------------------*/

%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <QString>
#include <QDebug>
#include <iostream>
#include <scanner.h>

using namespace std;

extern int yylex(void);
extern char *yytext;
extern  int SourceLine;
extern FILE *yyin;
void yyerror( const char *s)
{
    printf("Error sintáctico en la fila %u\n",SourceLine);
}
%}

/*---------------Declaración de tokens utilizados en el léxico--------------------*/

%start INICIO

%token numero
%token numero_negativo
%token full
%token fast
%token size
%token fit
%token unit
%token path
%token type
%token name
%token delete_o
%token add
%token id
%token mkdisk
%token rmdisk
%token fdisk
%token mount
%token unmount
%token exec
%token rep
%token k
%token m
%token bf
%token ff
%token wf
%token b
%token p
%token e
%token l
%token igual
%token cadena_esp
%token id
%token ruta
%token guion
%token eof

/*----------------------Declaración de producciones------------------------*/

%type<TEXT> INICIO
%type<TEXT> INSTRUCCIONES
%type<TEXT> INSTRUCCION
%type<TEXT> MKDISK
%type<TEXT> MKDISKPS
%type<TEXT> MKDISKP
%type<TEXT> RMDISK
%type<TEXT> FDISK
%type<TEXT> FDISKPS
%type<TEXT> FDISKP
%type<TEXT> MOUNT
%type<TEXT> MOUNTPS
%type<TEXT> MOUNTP
%type<TEXT> UNMOUNT
%type<TEXT> EXEC
%type<TEXT> REP
%type<TEXT> REPPS
%type<TEXT> REPP

/*-------------------------------- Opciones --------------------------------------*/

%error-verbose
%locations


%union{
char TEXT [255];
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
           | error
;

MKDISK:mkdisk MKDISKPS
;

MKDISKPS: MKDISKPS MKDISKP
        | MKDISKP
;

MKDISKP: guion size igual numero
       | guion fit igual bf
       | guion fit igual ff
       | guion fit igual wf
       | guion unit igual k
       | guion unit igual m
       | guion path igual ruta
       | guion path igual cadena_esp
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

