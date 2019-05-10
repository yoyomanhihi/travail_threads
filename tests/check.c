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
	bool vide1=false;
	uint8_t **tabrempli;
	tabrempli=(u_int8_t **)malloc(9*sizeof(uint8_t *));
	uint8_t **tabvide;
	tabvide=(u_int8_t **)malloc(6*sizeof(uint8_t *));
	uint8_t *rbuf1 = (uint8_t *)malloc(sizeof(uint8_t)*32);
	*rbuf1=101;
	tabrempli[1]=rbuf1;
	vide1=check(tabvide, 6);
	printf("tableau vide: %d \n", vide1);
	vide1=check(tabrempli, 9);
	printf("tableau rempli: %d \n", vide1);
}
