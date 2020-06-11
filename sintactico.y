
/*------------------------------Código utilizado----------------------------------*/

%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string.h>
#include <scanner.h>
#include <typeinfo>
#include <MKDISK.h>
#include <RMDISK.h>
#include <FDISK.h>
#include <REP.h>
#include <MOUNT.h>
#include <UNMOUNT.h>
#include <QList>



using namespace std;

extern int yylex(void);
extern char *yytext;
extern  int SourceLine;
extern FILE *yyin;
extern QList<MOUNT_>* mounted = new QList<MOUNT_>();

MKDISK_* mkdisk_ = new MKDISK_();
RMDISK_* rmdisk_ = new RMDISK_();
FDISK_* fdisk_ = new FDISK_();
REP_* rep_ = new REP_();
MOUNT_* mount_ = new MOUNT_();
UNMOUNT_* unmount_ = new UNMOUNT_();
void yyerror(const char *s);

string delimiter="-----------------------------------------------------------------------------------------------------------------------------";

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
%token<STRING> disk
%token<STRING> ruta
%token<STRING> guion
%token<STRING> mbr

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
           | error{}
;

MKDISK:mkdisk MKDISKPS {mkdisk_->printMk();std::cout << delimiter << std::endl;;mkdisk_ = new MKDISK_();}
;

MKDISKPS: MKDISKPS MKDISKP
        | MKDISKP
;

MKDISKP: guion size igual numero {mkdisk_->setSize($4);}
       | guion fit igual bf {mkdisk_->setFit($4);}
       | guion fit igual ff {mkdisk_->setFit($4);}
       | guion fit igual wf {mkdisk_->setFit($4);}
       | guion unit igual k {mkdisk_->setUnit($4);}
       | guion unit igual m {mkdisk_->setUnit($4);}
       | guion path igual ruta {mkdisk_->setPath($4);}
       | guion path igual cadena_esp {mkdisk_->setPath($4);}
;

RMDISK: rmdisk guion path igual ruta {rmdisk_->setPath($5);rmdisk_->borrarDisco();std::cout << delimiter << std::endl; rmdisk_ = new RMDISK_();}
      | rmdisk guion path igual cadena_esp {rmdisk_->setPath($5);rmdisk_->borrarDisco();std::cout << delimiter << std::endl; rmdisk_ = new RMDISK_();}
;

FDISK: fdisk FDISKPS{fdisk_->run();std::cout << delimiter << std::endl;fdisk_ = new FDISK_();}
;

FDISKPS: FDISKP
       | FDISKPS FDISKP
;
FDISKP:   guion size igual numero {fdisk_->setSize($4);}
        | guion unit igual b {fdisk_->setUnit($4);}
        | guion unit igual k {fdisk_->setUnit($4);}
        | guion unit igual m {fdisk_->setUnit($4);}
        | guion path igual ruta {fdisk_->setPath($4);}
        | guion path igual cadena_esp {fdisk_->setPath($4);}
        | guion type igual p {fdisk_->setType($4);}
        | guion type igual e {fdisk_->setType($4);}
        | guion type igual l {fdisk_->setType($4);}
        | guion fit igual bf {fdisk_->setFit($4);}
        | guion fit igual ff {fdisk_->setFit($4);}
        | guion fit igual wf {fdisk_->setFit($4);}
        | guion delete_o igual fast {fdisk_->setDelete($4);}
        | guion delete_o igual full {fdisk_->setDelete($4);}
        | guion name igual id {fdisk_->setName($4);}
        | guion name igual cadena_esp {fdisk_->setName($4);}
        | guion add igual numero {fdisk_->setAdd($4);}
        | guion add igual numero_negativo {fdisk_->setAdd($4);}
;

MOUNT: mount MOUNTPS {mount_->run();std::cout << delimiter << std::endl;mount_= new MOUNT_();}
;

MOUNTPS: MOUNTP
       | MOUNTPS MOUNTP
;

MOUNTP:   guion path igual ruta {mount_->setPath($4);}
        | guion path igual cadena_esp {mount_->setPath($4);}
        | guion name igual id {mount_->setName($4);}
        | guion name igual cadena_esp {mount_->setName($4);}
;

UNMOUNT: unmount guion id igual id {unmount_->setId($5);unmount_->run();std::cout << delimiter << std::endl;unmount_= new UNMOUNT_();}
       | unmount guion id igual cadena_esp {unmount_->setId($5);unmount_->run();std::cout << delimiter << std::endl;unmount_= new UNMOUNT_();}
;

EXEC: exec guion path igual ruta
    | exec guion path igual cadena_esp
;

REP: rep REPPS{rep_->run();std::cout << delimiter << std::endl;rep_ = new REP_();}
;

REPPS: REPP
     | REPPS REPP
;

REPP: guion path igual ruta {rep_->setPath($4);}
    | guion path igual cadena_esp {rep_->setPath($4);}
    | guion id igual id {rep_->setId($4);}
    | guion id igual cadena_esp {rep_->setId($4);}
    | guion name igual mbr {rep_->setName($4);}
    | guion name igual disk {rep_->setName($4);}
;
%%

void yyerror(const char *s)
{
    printf("Error sintactico en la linea %i: %s\n", SourceLine, s);
    std::cout << delimiter << std::endl;
}
