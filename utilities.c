#include "utilities.h"

//Converteix una array de chars a majuscules
void aMajus(char *file){
  for(int i = 0; i < (int) strlen(file); i++){
    if(file[i] >= 'a' && file[i] <= 'z'){
      file[i] = 'A' + (file[i] - 'a');
    }
  }
}

//Diu de quin tius es un sistema de fitxers: Fat o Ext
char tipus(int fd){
  int clusters;
	unsigned short int s;

  //FAT
	lseek(fd, 19, SEEK_SET);
  read(fd, &clusters, 2);
	if (clusters >= 4085 && clusters < 65525) {
		return 'f';
	}

	//EXT
	lseek(fd, (1024+56), SEEK_SET);
  read(fd, &s, sizeof(unsigned short int));
	if ( s == 0xef53){ //EXT2_SUPER_MAGIC
		return 'e';
	}

	return 'u';
}

Fat llegirFatInfo(Fat fat, int fd) {
	char tmp_type[8];
	bzero(fat.tipus, 8);
	bzero(fat.etiqueta, 12);

	lseek(fd, 54, SEEK_SET);
  read(fd, &tmp_type, 8);
  strncpy(fat.tipus, tmp_type,5);
	lseek(fd, 3,	SEEK_SET);
  read(fd, &(fat.nom), 8);
	lseek(fd, 43, SEEK_SET);
  read(fd, &(fat.etiqueta), 11);
	lseek(fd, 11, SEEK_SET);
  read(fd, &(fat.sectors_mida), sizeof(unsigned short int));
	lseek(fd, 13, SEEK_SET);
  read(fd, &(fat.sectors_cluster), sizeof(unsigned char));
	lseek(fd, 14, SEEK_SET);
  read(fd, &(fat.sectors_res), sizeof(unsigned short int));
	lseek(fd, 16, SEEK_SET);
  read(fd, &(fat.fat_taules), sizeof(unsigned short int));
	lseek(fd, 17, SEEK_SET);
  read(fd, &(fat.sectors_max), sizeof(unsigned short int));
	lseek(fd, 22, SEEK_SET);
  read(fd, &(fat.sectors_per_fat), sizeof(unsigned short int));

	return fat;
}

Ext llegirExt2Info(Ext ext, int fd){
  strcpy(ext.tipus,"EXT2");

  lseek(fd,(1024+0), SEEK_SET);
  read(fd, &(ext.inode_total), sizeof(unsigned int));
  read(fd, &(ext.bloc_total), sizeof(unsigned int));
  read(fd, &(ext.bloc_res), sizeof(unsigned int));
  read(fd, &(ext.bloc_free), sizeof(unsigned int));
  read(fd, &(ext.inode_free), sizeof(unsigned int));
  read(fd, &(ext.bloc_prim), sizeof(unsigned int));
  read(fd, &(ext.bloc_mida), sizeof(unsigned int));

  lseek(fd, (1024+32), SEEK_SET);
  read(fd, &(ext.bloc_grup), sizeof(unsigned int));
  read(fd, &(ext.frag_grup), sizeof(unsigned int));
  read(fd, &(ext.inode_grup), sizeof(unsigned int));

  lseek(fd, (1024+48), SEEK_SET);
  read(fd, &(ext.write),	sizeof(unsigned int));

  lseek(fd, (1024+64),	SEEK_SET);
  read(fd, &(ext.check),	sizeof(unsigned int));

  lseek(fd, (1024+44),SEEK_SET);
  read(fd, &(ext.mount),	sizeof(unsigned int));

  lseek(fd, (1024+84),	SEEK_SET);
  read(fd, &(ext.inode_prim), sizeof(unsigned int));
  read(fd, &(ext.inode_mida),	sizeof(unsigned int));

  lseek(fd, (1024+120), SEEK_SET);
  read(fd, &(ext.nom), 16 );
  ext.bloc_mida =  1024 << ext.bloc_mida;

  return ext;
}

//Ens diu la longitud de un string si es que aquesta es correcta per a funcionar en Fat
int fatStrlen(char* string){
    int size = strlen(string);
    int i;

    for(i = 0; i < size; i++){
        if(string[i] == ' ' || string[i] == '\0'){
          break;
        }
        else{
          //Caracters prohibits
          if((string[i] == '+') || (string[i] == ',') || (string[i] == ';') || (string[i] == '=') || (string[i] == '[') || (string[i] == ']')){
            return 0;
          }
        }
    }
    string[i] = '\0';
    return i;
}

//Printem la informacio si hem trobat l'arxiu
void printaTrobat(int tipus,unsigned int size){

  switch (tipus) {
    //Fitxer
    case 1:
          printf("Fitxer trobat. Ocupa %d bytes.\n", size);
      break;
    //Directori
    case 2:
        if (size == 0) {
          printf("Directori trobat.\n");
        }
        else{
          printf("Directori trobat. Ocupa %d bytes.\n", size);
        }
      break;
  }


}

//Funcio reciursiva per a buscar l'arxiu en subdirectoris
int buscarFatEnSubdirectori(char * buscar, int fd, int cluster, Fat fat, int borrar){
  char name[10];                //El nom del arxiu
  char extension[4];            //La extensio del arxiu
  char file_name[27];           //Nom + extensio per a comparar

  int start_file;               //Primer aruxiu del sistema
  int mida_nom = 0;             //Mida de el nom trobat
  int extension_length = 0;     //Mida de la extensio trobada
  int new_cluster;              //Cluster on es troben les dades del directori
  unsigned int filesize = 0;
  unsigned char atributes = 0;  //Atributs per a saber si es arxiu o directori


  //Primer cal trobar on comença el nostre cluster
  int rootDirectorySector = fat.sectors_per_fat * fat.fat_taules + 1;
  int rootDirectorySize = (fat.sectors_max * 32) / fat.sectors_mida;
  start_file = (rootDirectorySector + rootDirectorySize + cluster - 2) * fat.sectors_mida;

  for(int i = 0; i < 1024/32; i++){
      lseek(fd, start_file, SEEK_SET);

      //Netejo Les arrays
      bzero(name,10);
      bzero(extension,4);
      bzero(file_name,27);

      //DIR_name
      read(fd,name,8);
      mida_nom = fatStrlen(name);
      strcat(file_name, name);

      //DIR_extension
      read(fd, extension, 3);
      extension_length = fatStrlen(extension);

      //DIR_attr
      read(fd, &atributes, 1);

      if(mida_nom > 0){

        if ( strcmp(name,".") != 0 && strcmp(name,"..") != 0 ){

          if(extension_length > 0){
              strcat(file_name, ".");
              strcat(file_name, extension);
          }   //if

          //offset 12

          lseek(fd,14 , SEEK_CUR);
          cluster = 0;
          read(fd, &new_cluster,2);

          //DIR_FileSize
          read(fd, &filesize, 4);

          //Miro si es el que busco
          if(strcmp(file_name, buscar) == 0 ){
              if (borrar == 1) {
                unsigned char delete_flag = 0xE5;
                lseek(fd, start_file, SEEK_SET);
                write(fd,&delete_flag,1);
                return 1;
              }

              //Miro si es un fitxer o un directori
              if((atributes & 0x10) == 0x10){
                printaTrobat(2,filesize);
                return 1;
              }
              if((atributes & 0x20) == 0x20){
                printaTrobat(1,filesize);
                return 1;
              }
          }   //if

          //Si es un directori, busco a dins recursivament

          if((atributes & 0x10) == 0x10){
            if(1 == buscarFatEnSubdirectori(buscar, fd, new_cluster, fat, borrar)){
              return 1;
            }
          }
        }
      }
      //Paso al seguent element
      start_file += 32;
  }   //for



  return 0;
}

//Busca un arxiu en un fitxer sistema en Fat
int buscarEnFat(char const *file, int fd, int borrar){
  char name[10];                //El nom del arxiu
  char extension[4];            //La extensio del arxiu
  char file_name[14];           //Nom + extensio per a comparar

  int start_file;               //Primer aruxiu del sistema
  int mida_nom = 0;             //Mida de el nom trobat
  int extension_length = 0;     //Mida de la extensio trobada
  int prim_dir;                 //Ubicacio primer directori
  int cluster;                  //Cluster on es troben les dades del directori
  unsigned int filesize = 0;
  unsigned char atributes = 0;  //Atributs per a saber si es arxiu o directori

  //Primer cal que passem a majuscules per a fer la cerca
  char buscar[strlen(file)];    //Element que volem buscar
  strcpy(buscar, file);
  aMajus(buscar);

  //Ara cal buscar on comenca el primer directori i poso alla el punter
  Fat fat;
  fat = llegirFatInfo(fat, fd);
  prim_dir = ((fat.sectors_res * fat.sectors_mida) + (fat.fat_taules * fat.sectors_per_fat * fat.sectors_mida));
  start_file = prim_dir;

  // Obtenim posicio del rootDirectory

  //Recorrem tots els arxius del root memory
  for(int i = 0; i < fat.sectors_max; i++){
      lseek(fd, start_file, SEEK_SET);

      //Netejo Les arrays
      bzero(name,10);
      bzero(extension,4);
      bzero(file_name,14);

      //DIR_name
      read(fd,name,8);
      mida_nom = fatStrlen(name);
      strcat(file_name, name);

      if(mida_nom > 0){
          //DIR_extension
          read(fd, extension, 3);
          extension_length = fatStrlen(extension);

          if(extension_length > 0){
              strcat(file_name, ".");
              strcat(file_name, extension);
          }   //if

          //Offset: 11

          //DIR_attr
          read(fd, &atributes, 1);

          //offset 12

          lseek(fd,14 , SEEK_CUR);

          cluster = 0;
          read(fd, &cluster,2);

          //DIR_FileSize
          read(fd, &filesize, 4);

          //Miro si es el que busco
          if(strcmp(file_name, buscar) == 0){
              if (borrar == 1) {
                unsigned char delete_flag = (unsigned char) 0xE5;
                lseek(fd, start_file, SEEK_SET);
                write(fd,&delete_flag,1);
                return 1;
              }

              //Miro si es un fitxer o un directori
              if((atributes & 0x10) == 0x10){
                printaTrobat(2,filesize);
                return 1;
              }
              if((atributes & 0x20) == 0x20){
                printaTrobat(1,filesize);
                return 1;
              }
          }   //if

          //Si es un directori, busco a dins recursivament

          if((atributes & 0x10) == 0x10){
            if(1 == buscarFatEnSubdirectori(buscar, fd, cluster, fat, borrar)){
              return 1;
            }
          }
      }   //if
      //Paso al seguent element
      start_file += 32;
  }   //for

  printf("Error. Fitxer no trobat.\n");
  return 0;
}

//Ens dona tota la informacio sobre un directori
DirectoriExt infoDirectori(int fd, unsigned long data_block_pos){

    DirectoriExt ext_directory;

    lseek(fd, data_block_pos, SEEK_SET);
    read(fd, &ext_directory.inode, sizeof(unsigned int));
    read(fd, &ext_directory.rec_len, sizeof(unsigned short int));
    read(fd, &ext_directory.name_len, sizeof(unsigned char));
    read(fd, &ext_directory.file_type, sizeof(unsigned char));
    read(fd, &ext_directory.file_name, ext_directory.name_len);

    // Tornem a la posicio on estavem
    lseek(fd, data_block_pos, SEEK_SET);

    return ext_directory;
}

//Busca l'inici de les taules segons el primer grup
unsigned long buscaTaulaInode(int fd, Ext ext){

    unsigned long group_table_position = 0;
    unsigned long table_position = 0;
    unsigned long inode_position = 0;

    group_table_position = ext.bloc_mida * (ext.bloc_prim + 1);

    lseek(fd, group_table_position + 8, SEEK_SET);
    read(fd, &table_position, 4);

    inode_position = table_position * ext.bloc_mida;

    return inode_position;
}


unsigned long buscarPosInode(int fd, Ext ext, int num){
    unsigned long offset;

    //Miro en quin bloc es troba el INODE
    unsigned long bloc = (unsigned long) num / ext.inode_grup;
    num = num - (ext.inode_grup * bloc);

    //Mida de cada bloc
    unsigned long block_size = ext.bloc_grup * ext.bloc_mida;

    offset = (block_size * bloc) + buscaTaulaInode(fd, ext) + (num - 1) * ext.inode_mida;

    return offset;
}

//Llegeix tota la informacio del inode donat un offset
INODE llegirInode(int fd, unsigned long offset){
    INODE inode;

    lseek(fd, offset, SEEK_SET);
    read(fd, &inode.i_mode, 2);
    read(fd, &inode.i_uid, 2);
    read(fd, &inode.i_size, 4);
    read(fd, &inode.i_atime, 4);
    read(fd, &inode.i_ctime, 4);
    read(fd, &inode.i_mtime, 4);
    read(fd, &inode.i_dtime, 4);
    read(fd, &inode.i_gid, 2);
    read(fd, &inode.i_links_count, 2);
    read(fd, &inode.i_blocks, 4);
    read(fd, &inode.i_flags, 4);
    read(fd, &inode.i_osd1, 4);

    for(int i = 0; i < 15; i++) {
      inode.i_block[i] = 0;
      read(fd, &inode.i_block[i], 4);
    }

    read(fd, &inode.i_generation, 4);
    read(fd, &inode.i_file_acl, 4);
    read(fd, &inode.i_dir_acl, 4);
    read(fd, &inode.i_faddr, 4);
    read(fd, &inode.i_osd2, 12);

    return inode;
}

void eliminaExt2(int fd, DirectoriExt directori, unsigned long dirOffset, Ext ext){

  unsigned long offset_inode = buscarPosInode(fd, ext, directori.inode);
  INODE inode = llegirInode(fd, offset_inode);

  //Invalido el nom
  lseek(fd,dirOffset + 8 , SEEK_SET);
  unsigned char deleted = 0xE5;
  write(fd,&deleted,1);


  //Poso els link count a 0
  deleted = 0x00;
  lseek(fd,offset_inode + 26 , SEEK_SET);
  write(fd,&deleted,2);

  //poso el deletion time
  lseek(fd,offset_inode + 20 , SEEK_SET);
  time_t seconds;
  seconds = time(NULL);
  write(fd, &seconds, 4);

  //Bitmap de blocs
  unsigned long grup_size = ext.bloc_grup * ext.bloc_mida;
  unsigned long grup_inode = (unsigned long) directori.inode / ext.inode_grup;
  unsigned long offset = ((grup_size * grup_inode) + buscaTaulaInode(fd, ext)) - (ext.bloc_mida * 2);

  unsigned char bitmap_aux;
  unsigned int mask;

  unsigned long grup_bloc;

  for(int i = 0; i < 15; i++){
    if(inode.i_block[i] != 0){

      grup_bloc = (unsigned long) inode.i_block[i] / ext.bloc_grup;
      inode.i_block[i] = inode.i_block[i] - (ext.bloc_grup * grup_bloc);

      lseek(fd, offset + (inode.i_block[i]/8), SEEK_SET);
      int bit = inode.i_block[i]%8;

      read(fd, &bitmap_aux, 1);
      mask = 0x0001 << bit;
      mask = mask >> 8;
      mask = !mask;
      bitmap_aux = bitmap_aux & (char) deleted;

      lseek(fd, offset + (inode.i_block[i]/8), SEEK_SET);
      write(fd, &bitmap_aux, 1);
    }
  }

  //Bitmap de inodes
  offset += ext.bloc_mida;

  unsigned long bloc_inode = (unsigned long) directori.inode / ext.inode_grup;
  directori.inode = directori.inode - (ext.inode_grup * bloc_inode);

  lseek(fd, offset + (directori.inode/8), SEEK_SET);
  int bit = directori.inode%8;

  read(fd, &bitmap_aux, 1);
  mask = 0x0001 << bit;
  mask = mask >> 8;
  mask = !mask;
  bitmap_aux = bitmap_aux & (char) deleted;

  lseek(fd, offset + (directori.inode/8), SEEK_SET);
  write(fd, &bitmap_aux, 1);
}

//Busca recursivament un fitxer en els subdirectori del root indicat
int buscarExtEnSubdirectori(char const *file, int fd, Ext ext, unsigned int inodes, int borrar) {
  unsigned long tmp = 0;
  unsigned long offset = 0;
  unsigned long block_sum = 0;
  unsigned long dir_offset = 0;

  INODE inode;
  DirectoriExt ext_directory;

  offset = buscarPosInode(fd, ext, inodes);
  inode = llegirInode(fd, offset);


  //Recorrem tots els blocs del inode
  for (int bloc_actual = 0; bloc_actual < 15; bloc_actual++) {

      if (inode.i_block[bloc_actual] == 0) {
          return 0;
      }   //if

      dir_offset = inode.i_block[bloc_actual] * ext.bloc_mida;

      block_sum = 0;
      do {
          ext_directory = infoDirectori(fd, dir_offset);
          // Comprovem si el nombre coincideix i si hi ha una entrada valida

          //Si el inode es igual a 0 significa que no es fa servir
          if (ext_directory.inode != 0) {

            //Ignorem els . i ..
            if (strcmp(ext_directory.file_name,".") != 0 && strcmp(ext_directory.file_name,"..") != 0 ) {

              //Mirem si es el fitxer que busquem
              if (strcmp(ext_directory.file_name, file) == 0) {
                  if(borrar == 1){
                    eliminaExt2(fd, ext_directory, dir_offset, ext);
                    return 1;
                  }

                  INODE inode_aux = llegirInode(fd, buscarPosInode(fd, ext, ext_directory.inode));

                  if (ext_directory.file_type == 1){
                      printaTrobat(1,inode_aux.i_size);
                      return 1;
                  }
                  if(ext_directory.file_type == 2){
                      printaTrobat(2,inode_aux.i_size);
                      return 1;
                  }
              }
              else {
                  // Nomes nens interessen els directoris d'aquesta zona
                  if (ext_directory.file_type == 2) {


                    // Entrem al directori i ens el recorrem
                    tmp = buscarExtEnSubdirectori(file, fd, ext, ext_directory.inode, borrar);
                    if (tmp == 1 ) {
                        return 1;
                    }
                  }
              }
            }
          }

          dir_offset += ext_directory.rec_len;
          block_sum  += ext_directory.rec_len;
      } while (block_sum < ext.bloc_mida && ext_directory.rec_len != 0);  //do-while
  }

    return 0;
}

//Busca un arxiu en un fitxer sistema en Ext
int buscarEnExt(char const *file, int fd, int borrar){
  //Informacio general del sistema d'arxius
  Ext ext;
  ext = llegirExt2Info(ext, fd);

	unsigned long offset = buscarPosInode(fd,ext,1) + 128;
	unsigned long suma_bloc = 0;
  unsigned long dirOffset = 0;

	INODE inode = llegirInode(fd, offset);
	DirectoriExt directori;

  //Recorrem tots els blocs del inode root
	for (int bloc = 0; bloc < 15; bloc++ ) {

		if ( inode.i_block[bloc] == 0 ) {
      break;
		}

    //Offset total del directori
		dirOffset = inode.i_block[bloc] * ext.bloc_mida;

		do {

      //Obtenim la info del directori
			directori = infoDirectori(fd, dirOffset );

      //Mirem si el inode es fa servir
			if ( directori.inode != 0) {

        //ignorem les carpetes . i ..
        if ( strcmp(directori.file_name,".") != 0 && strcmp(directori.file_name,"..") != 0 ) {

  				//Comprovem que el nom sigui el vaild
  				if ( strcmp(directori.file_name, file) == 0 ) {

            if(borrar == 1){
              eliminaExt2(fd, directori, dirOffset, ext);
              return 1;
            }

            INODE inode_aux = llegirInode(fd, buscarPosInode(fd, ext, directori.inode));

            if (directori.file_type == 1){
                printaTrobat(1,inode_aux.i_size);
                return 1;
            }
            if(directori.file_type == 2){
                printaTrobat(2,inode_aux.i_size);
                return 1;
            }

  				} else {
  					if ( directori.file_type == 2 ) {
						  //Crida recursiva per a mirar dintre dels fitxers
							if ( buscarExtEnSubdirectori(file,fd,ext,directori.inode, borrar) == 1 ) {
								return 1;
  						}
  					}
  				}
        }
			}

			dirOffset += directori.rec_len;
			suma_bloc  += directori.rec_len;

		} while (suma_bloc < ext.bloc_mida);
	}
  printf("Error. Fitxer no trobat.\n");
  return 0;
}
