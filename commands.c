#include "commands.h"

void commandManager(int param, char const *argv[]){

  //INFO
  if (strcmp(argv[1], "/info") == 0){
    if(param != 3){
      printf("Error. Nombre de paràmetres incorrecte.\n");
    }
    else{
      info(argv);
    }
  }
  //FIND
  else if (strcmp(argv[1], "/find") == 0){
    if(param != 4){
      printf("Error. Nombre de paràmetres incorrecte.\n");
    }
    else{
      find(argv);
    }
  }
  //DELETE
  else if (strcmp(argv[1], "/delete") == 0){
    if(param != 4){
      printf("Error. Nombre de paràmetres incorrecte.\n");
    }
    else{
      delete(argv);
    }
  }
  //DEFAULT
  else {
    printf("Error. La comanda no existeix\n");
  }
}
