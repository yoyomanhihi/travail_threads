#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "sha256.h"
#include "sha256.c"
#include "reverse.h"
#include "reverse.c"

void lire(){//producteur qui lit dans les fichiers
	int fd1=open("test-input/01_4c_1k.bin", O_RDONLY);//ouverture du fichier
	if(fd1==-1){ //si erreur
		perror("open file");
	}
	u_int8_t *rbuf1 = (u_int8_t *)malloc(sizeof(u_int8_t)); // cree le buffer pour read
	if(read(fd1, rbuf1, sizeof(u_int8_t))==-1){ //si erreur
		close(fd1); //on ferme le fd qui a ete ouvert en cas d erreur de read
		perror("read file");
	}
	while(read(fd1, rbuf1, sizeof(u_int8_t))>0){//Tant qu'il y a a lire
		u_int8_t hash;//cree la variable hash qui stockera ce qui est lu
		hash=*rbuf1; //donner la valeur a la variable hash
		printf("%d \n", hash);
	}
}
int main(){
	lire();
}
