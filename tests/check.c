#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>

bool check(u_int8_t **buff, int size){
	bool vide=true;//de base il considere que le buffer est vide
	for(int i=0; i<size;i++){
		if(buff[i]!=0)//si le buffer n est pas vide 
			vide=false;
	}
	return vide;
}

int main(int argc, char *argv[]){
	bool vide=false;
	uint8_t byte[32];
	byte[0]=(uint8_t) 4;
	uint8_t *tabvide[6];
	uint8_t *tabrempli[9];
	tabrempli[0]=byte;
	vide=check(tabvide, 6);
	printf("tableau vide: %d \n", vide);
	vide=check(tabrempli, 9);
	printf("tableau rempli: %d \n", vide);
}
