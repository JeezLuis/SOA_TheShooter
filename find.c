#include "find.h"

void find(char const *argv[]){
  int fd = open(argv[VOLUM], O_RDONLY);
	if ( fd < 0 ) {
    printf("Error. Volum no trobat.\n");
	}
  else{

    switch (tipus(fd)) {
      case 'f': //FAT
          buscarEnFat(argv[ARXIU], fd, 0);
        break;
      case 'e': //EXT
          buscarEnExt(argv[ARXIU], fd, 0);
        break;
      case 'u':
        printf("Error. Volum no formatat en FAT16 ni EXT2.\n");
        break;
    }
    close(fd);
  }
}
