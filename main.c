//Lluis Farre Castan (lluis.farre)

#include <stdio.h>
#include <string.h>

#include "commands.h"

int main(int argc, char const *argv[]) {
  if(argc <= 1){
    printf("Falta comanda\n");
  }
  else{
    commandManager(argc,argv);
  }

  return 0;
}
