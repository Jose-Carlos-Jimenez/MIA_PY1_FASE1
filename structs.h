#pragma once
#ifndef STRUCTS_H
#define STRUCTS_H

#endif // STRUCTS_H

#include <time.h>

/**
 * Structs que son útiles para llevar un manejo
 * lógico de la memoria.
*/

/**
 * Particiones primarias.
*/
struct Partition{
public:
    char part_status = '0';
    char part_type = '0';
    char part_fit = '0';
    int part_start = -1;
    int part_size = 0;
    char part_name[16] = "";
};

/**
 * Particiones lógicas.
*/
struct EBR
{
    char part_status='0';
    char part_fit='0';
    int part_start=-1;
    int part_size=0;
    int part_next=-1;
    char part_name[16];
};

/**
 * Master boot record, sirve para administrar la memoria distribuida
 * a las particiones a manera de meta data.
*/
struct MBR
{
public:
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_disk_signature;
    char disk_fit;
    Partition mbr_partitions[4];
};

