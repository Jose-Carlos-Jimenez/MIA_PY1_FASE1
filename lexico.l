/*--------- Definiciones de FLEX----------*/
%option noyywrap
%option case-insensitive
%option yylineno
%option nounput
/*------------Definiciones de C ------------*/
%{
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "parser.h"

int SourceLine = 1;
%}

/*-----------Expresiones regulares ---------*/
comentarios [#]([^\n])*
numero [0-9]+
numero_negativo [-][0-9]+
string (\"(\\.|[^\\"])*\")
ruta "/"(["/"a-zA-Z\-_"."0-9])+
id ([a-zA-Z0-9"_""-"])+

%% // <<<<<<<<<<<<<<<<<<< [Reglas lexicas] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><
('\s'|'\r'|'\t')+ {};
"\n" {SourceLine++;}
{comentarios} {};
{numero} {return numero;};
{numero_negativo}     {return (numero_negativo);};
[-] {return(guion);};
"full" {return (full);};
"fast" {return (fast);};
"size" {return (size);};
"fit" {return (fit);};
"unit" {return (unit);};
"path" {return (path);};
"type" {return (type);};
"name" {return (name);};
"delete" {return (delete_o);};
"add" {return (add);};
"id" {return (id);};
"mkdisk" {return (mkdisk);};
"rmdisk" {return (rmdisk);};
"fdisk" {return (fdisk);};
"mount" {return (mount);};
"unmount" {return (unmount);};
"exec" {return (exec);};
"rep" {return (rep);};
"k" {return (k);};
"m"  {return (m);};
"bf" {return (bf);};
"ff" {return (ff);};
"wf" {return (wf);};
"b" {return (b);};
"p" {return (p);};
"e" {return (e);};
"l" {return (l);};
"=" {return (igual);};
{string} {return (cadena_esp);};
{ruta} {return(ruta);};
{id} {return(id);};
. {}
%%//<<<<<<<<<<<<<<<<<<<<<<<<<<<< [Reglas léxicas] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>


