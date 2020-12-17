#ifndef _utilities_h_
#define _utilities_h_

#include <stdio.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define VOLUM  2
#define ARXIU  3

typedef struct {
	char nom[17];
	char tipus[5];
	unsigned int inode_mida;
	unsigned int inode_total;
	unsigned int inode_prim;
	unsigned int inode_grup;
	unsigned int inode_free;
	unsigned int bloc_total;
	unsigned int bloc_res;
	unsigned int bloc_free;
	unsigned long bloc_mida;
	unsigned int bloc_prim;
	unsigned int bloc_grup;
	unsigned int frag_grup;
	time_t check;
	time_t mount;
	time_t write;
} Ext;

typedef struct {
	char tipus[8];
	char nom[8];
	char etiqueta[12];
	unsigned char fat_taules;
	unsigned short int sectors_mida;
	unsigned char sectors_cluster;
	unsigned short int sectors_res;
	unsigned short int sectors_max;
	unsigned short int sectors_per_fat;
} Fat;

typedef struct {
    unsigned long i_mode;
    unsigned long i_uid;
    unsigned long i_size;
    unsigned long i_atime;
    unsigned long i_ctime;
    unsigned long i_mtime;
    unsigned long i_dtime;
    unsigned long i_gid;
    unsigned long i_links_count;
    unsigned long i_blocks;
    unsigned long i_flags;
    unsigned long i_osd1;
    unsigned long i_block[15];
    unsigned long i_generation;
    unsigned long i_file_acl;
    unsigned long i_dir_acl;
    unsigned long i_faddr;
    unsigned long i_osd2;
} INODE;

typedef struct {
    unsigned int inode;
    unsigned short int rec_len;
    unsigned char name_len;
    unsigned char file_type;
    char file_name[255];
} DirectoriExt;

void aMajus(char *file);

char tipus(int fd);

Ext llegirExt2Info(Ext ext, int fd);
Fat llegirFatInfo(Fat fat, int fd);

int buscarEnFat(char const *file, int fd, int borrar);
int buscarEnExt(char const *file, int fd, int borrar);

#endif
