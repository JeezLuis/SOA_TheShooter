#include "info.h"

void info(char const *argv[]){
  int fd = open(argv[VOLUM], O_RDONLY);
	Ext ext;
	Fat fat;
	if ( fd < 0 ) {
    printf("Error. Volum no trobat.\n");
	}
  else{
    switch (tipus(fd)) {
      case 'f': //FAT
          fat = llegirFatInfo(fat, fd);
          printFatInfo(fat);
        break;
      case 'e': //EXT
          ext = llegirExt2Info(ext, fd);
          printExt2Info(ext);
        break;

      case 'u':
        printf("Sistema d’arxius no és ni EXT2 ni FAT16.\n");
        break;
    }
    close(fd);
  }
}

//Printa tota la informacio del Ext
void printExt2Info(Ext ext) {
	printf("--------- Fylesystem  Information --------- \n");
	printf("\nFylesystem: %s",ext.tipus);
	printf("\n\nINODE INFO\n");
	printf("Mida Inode: %d \n",ext.inode_mida);
	printf("Num Inodes: %d \n",ext.inode_total);
	printf("Primer Inode: %d \n",ext.inode_prim);
	printf("Inodes Grup: %d \n",ext.inode_grup);
	printf("Inodes Lliures: %d \n",ext.inode_free);
	printf("\n\nINFO BLOC\n");
	printf("Mida Bloc: %lu \n",ext.bloc_mida);
	printf("Blocs Reservats: %d \n",ext.bloc_res);
	printf("Blocs Lliures: %d \n",ext.bloc_free);
	printf("Total Blocs: %d \n",ext.bloc_total);
	printf("Primer Bloc: %d \n",ext.bloc_prim);
	printf("Blocs grup: %d \n",ext.bloc_grup);
	printf("Flags grup: %d \n",ext.frag_grup);
	printf("\n\nVOLUME INFO\n");
	printf("Nom volum: %s \n",ext.nom);
	printf("Ultima comprov: %s", ctime(&(ext.check)));
	printf("\rUltim muntatge: %s", ctime(&(ext.mount)));
	printf("\rUltima escriptura: %s\n", ctime(&(ext.write)));
}

//Printa tota la informacio del Fat
void printFatInfo(Fat fat) {
	printf("---- Fylesystem  Information ---- \n\n");
	printf("Fylesystem: %s \n",fat.tipus);
	printf("System Name: %s \n",fat.nom);
	printf("Mida del sector: %d \n",fat.sectors_mida);
	printf("Sectors Per Cluster: %d \n",fat.sectors_cluster);
	printf("Sectors Reservats: %d \n",fat.sectors_res);
	printf("Numero de FATs: %d \n",fat.fat_taules);
	printf("MaxRootEntries: %d \n",fat.sectors_max);
	printf("Sectors per FAT: %d \n",fat.sectors_per_fat);
  printf("Label: %s\n\n",fat.etiqueta);
}
