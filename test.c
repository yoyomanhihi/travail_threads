#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
u_int8_t buffer1[3];//taille M+2 avec M qui vaut le nombre de type de fichier (1, 2 ou 3)
char* buffer2[5];//taille N+2 avec N le nombre de threads
int fd1=open("test-input/01_4c_1k.bin", O_RDONLY);//ouvertue du fichier
if(fd1!=0){
	error(err,"open file");
}		
void lire("test-input/01_4c_1k.bin"){//producteur qui lit dans les fichiers
	uint8_t hash;//cree la variable hash qui stockera ce qui est lu
	int r=read(fd1, buffer1, sizeof(uint8_t);//lit le fichier
	if(r!=0){
		error(err,"read file");
	}
	while(r!=0){//Tant qu'il y a a lire
		r=read(fd1, buffer, sizeof(uint8_t);//lit le fichier
		if(r==0){
			hash=fd1; //donner la valeur a la variable hash
			sem_wait(&empty);
			pthread_mutex_lock(&mutex);
			//inserer le hash dans le buffer
			pthread_mutex_unlock(&mutex);
			sem_post(&full);
		}

	}
}
