#ifndef _info_h_
#define _info_h_

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "utilities.h"

void info(char const *argv[]);

void printExt2Info(Ext ext2);
void printFatInfo(Fat fat);

#endif
