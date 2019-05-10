#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>
#include <getopt.h>
#include "sha256.h"
#include "sha256.c"
#include "reverse.h"
#include "reverse.c"

bool check(u_int8_t **buff, int size){
	bool vide=true;//de base il considere que le buffer est vide
	for(int i=0; i<size;i++){
		if(buff[i]!=0)//si le buffer n est pas vide 
			vide=false;
	}
	return vide;
}

int main(int argc, char *argv[]){
	bool vide;
	uint8_t *byte=(uint8_t *) 4;
	uint8_t *tabvide[6];
	uint8_t *tabrempli[9];
	tabrempli[7]=byte;
	vide=check(tabvide, 6);
	printf("tableau vide: %d \n", vide);
	vide=check(tabrempli, 9);
	printf("tableau rempli: %d \n", vide);
}
