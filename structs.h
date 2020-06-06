#ifndef STRUCTS_H
#define STRUCTS_H

#endif // STRUCTS_H

#include <time.h>
struct Partition{
public:
    char part_status = '0';
    char part_type = '0';
    char part_fit = '0';
    int part_start = -1;
    int part_size = 0;
    char part_name[16] = "";
};

struct EBR
{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
};

struct MBR
{
public:
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_disk_signature;
    char disk_fit;
    Partition mbr_partition_1;
    Partition mbr_partition_2;
    Partition mbr_partition_3;
    Partition mbr_partition_4;
};
