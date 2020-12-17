#include "delete.h"

void delete(char const *argv[]){
  int fd = open(argv[VOLUM], O_RDWR);
  if ( fd < 0 ) {
    printf("Error. Volum no trobat.\n");
  }
  else{

    switch (tipus(fd)) {
      case 'f': //FAT
          if(buscarEnFat(argv[ARXIU], fd, 1) == 1){
            printf("El fitxer %s ha estat eliminat.\n", argv[ARXIU]);
          }
        break;
      case 'e': //EXT

          if(buscarEnExt(argv[ARXIU], fd, 1) == 1){
            printf("El fitxer %s ha estat eliminat.\n", argv[ARXIU]);
          }
        break;
      case 'u':
        printf("Error. Volum no formatat en FAT16 ni EXT2.\n");
        break;
    }
    close(fd);
  }
}
