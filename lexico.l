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
ruta "/"("/"|"-"|"_"|"."|[a-zA-Z0-9])+
id ([a-zA-Z])([a-zA-Z]|[0-9]|"_")*

%% // <<<<<<<<<<<<<<<<<<< [Reglas lexicas] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><
('\s'|'\r'|'\t')+ {};
"\n" {SourceLine++;}
{comentarios} {};
{numero} {yylval.NUM = strdup(yytext);return (numero);};
{numero_negativo}     {yylval.NUM = strdup(yytext);return (numero_negativo);};
"full" {yylval.STRING = strdup(yytext);return (full);};
"fast" {yylval.STRING = strdup(yytext);return (fast);};
"mbr" {yylval.STRING = strdup(yytext);return (mbr);};
"size" {yylval.STRING = strdup(yytext);return (size);};
"fit" {yylval.STRING = strdup(yytext);return (fit);};
"unit" {yylval.STRING = strdup(yytext);return (unit);};
"path" {yylval.STRING = strdup(yytext);return (path);};
"type" {yylval.STRING = strdup(yytext);return (type);};
"name" {yylval.STRING = strdup(yytext);return (name);};
"delete" {yylval.STRING = strdup(yytext);return (delete_o);};
"add" {yylval.STRING = strdup(yytext);return (add);};
"disk" {yylval.STRING = strdup(yytext);return(disk);};
"id" {yylval.STRING = strdup(yytext);return (id);};
"mkdisk" {yylval.STRING = strdup(yytext);return (mkdisk);};
"rmdisk" {yylval.STRING = strdup(yytext);return (rmdisk);};
"fdisk" {yylval.STRING = strdup(yytext);return (fdisk);};
"mount" {yylval.STRING = strdup(yytext);return (mount);};
"unmount" {yylval.STRING = strdup(yytext);return (unmount);};
"exec" {yylval.STRING = strdup(yytext);return (exec);};
"rep" {yylval.STRING = strdup(yytext);return (rep);};
"k" {yylval.STRING = strdup(yytext);return (k);};
"m"  {yylval.STRING = strdup(yytext);return (m);};
"bf" {yylval.STRING = strdup(yytext);return (bf);};
"ff" {yylval.STRING = strdup(yytext);return (ff);};
"wf" {yylval.STRING = strdup(yytext);return (wf);};
"b" {yylval.STRING = strdup(yytext);return (b);};
"p" {yylval.STRING = strdup(yytext);return (p);};
"e" {yylval.STRING = strdup(yytext);return (e);};
"l" {yylval.STRING = strdup(yytext);return (l);};
"=" {yylval.STRING = strdup(yytext);return (igual);};
{string} {yylval.STRING = strdup(yytext);return (cadena_esp);};
{ruta} {yylval.STRING = strdup(yytext);return(ruta);};
{id} {yylval.STRING = strdup(yytext);return(id);};
[-] {yylval.STRING = strdup(yytext);return(guion);};
. {}
%%//<<<<<<<<<<<<<<<<<<<<<<<<<<<< [Reglas léxicas] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>


