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

uint8_t *buffer1[3];//taille M+2 avec M qui vaut le nombre de type de fichier (1, 2 ou 3)
int size1=3;//taille de buffer1, sert plus tard
int debut1=0;//debut du buffer1 a partir de quoi le buffer est vide
int fin1=0;//fin de la zone vide de buffer1
int casehash=0;

void lire(){//producteur qui lit dans les fichiers
	int fd1=open("test-input/01_4c_1k.bin", O_RDONLY);//ouverture du fichier
	if(fd1==-1){ //si erreur
		perror("open file");
	}
	u_int8_t *rbuf1 = (u_int8_t *)malloc(sizeof(u_int8_t)); // cree le buffer pour read
	u_int8_t hash[32];
	if(read(fd1, rbuf1, sizeof(u_int8_t))==-1){ //si erreur
		close(fd1); //on ferme le fd qui a ete ouvert en cas d erreur de read
		perror("read file");
	}
	while(read(fd1, rbuf1, sizeof(u_int8_t))>0){//Tant qu'il y a a lire
		hash[casehash]=*rbuf1;
		if(casehash==31){
			casehash=-1;
			buffer1[debut1]=hash;
		}
		casehash++;
		if(debut1==(size1-1)){//si debut1 est a la derniere case
			debut1=0;//revient au debut
		}
		else{
			debut1++;//sinon debut1 augmente de 1;
		}
	}
}

void decrypteur(){//threads de calculs
	char *bufferInter=(char *) malloc(sizeof(char)*17); //buffer intermediaire utile a la fonction reversehash je beug jsp comment faire pour que le mot soit dans bufferinter
	u_int8_t *mdp=(u_int8_t *) malloc(sizeof(u_int8_t)*32);
	while(1==1){//tant que le thread a encore a lire et que le buffer n est pas vide, juste a verifier que la condition debut1!=fin1 n accepte pas un buffer totalement rempli
		mdp=buffer1[fin1];//prend la premiere valeur de buf1 apres l espace vide
		if(fin1==(size1-1)){//si fin1 est a la derniere case
			fin1=0;//revient au debut
		}
		else{
			fin1++;//sinon fin1 augmente de 1;
		}
		if (reversehash(mdp, bufferInter, 17)==true){//si reversehash a trouve un inverse il le stocke dans bufferInter mais??
			printf("%s \n", bufferInter);
		}
	}	
}

void test(){
	char *bufferInter=(char *) malloc(sizeof(char)*17);
	int fd1=open("test-input/01_4c_1k.bin", O_RDONLY);//ouverture du fichier
	if(fd1==-1){ //si erreur
		perror("open file");
	}
	u_int8_t *rbuf1 = (u_int8_t *)malloc(sizeof(u_int8_t)); // cree le buffer pour read
	u_int8_t hash[32];
	int casehash=0;
	if(read(fd1, rbuf1, sizeof(u_int8_t)==-1)){ //si erreur
		close(fd1); //on ferme le fd qui a ete ouvert en cas d erreur de read
		perror("read file");
	}
	while(read(fd1, rbuf1, sizeof(u_int8_t)>0)){//Tant qu'il y a a lire
		hash[casehash]=*rbuf1;
		if(casehash==31){
			if (reversehash(hash, bufferInter, 17)==true){//si reversehash a trouve un inverse il le stocke dans bufferInter mais??
				printf("%s \n", bufferInter);
				casehash=-1;
			}
		}
		casehash++;
	}
}

int main(){
	test();
}
