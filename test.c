#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
u_int8_t buffer1[3];//taille M+2 avec M qui vaut le nombre de type de fichier (1, 2 ou 3)
char* buffer2[5];//taille N+2 avec N le nombre de threads		
void lire(){//producteur qui lit dans les fichiers
	sem_t *sem1=(sem_t *) malloc(sizeof(sem_t));
	int s=sem_init(sem1, 0, 3);
	if(s==-1){
		perror("create semaphore");
	}
	pthread_mutex_t *mut1=(pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mut1, NULL);// verifier si pas erreur ?
	int fd1=open("test-input/01_4c_1k.bin", O_RDONLY);//ouvertue du fichier
	if(fd1!=0){
		perror("open file");
	}
	int r=read(fd1, buffer1, sizeof(u_int8_t));//lit le fichier
	if(r!=0){
		perror("read file");
	}
	while(r!=0){//Tant qu'il y a a lire
		r=read(fd1, buffer1, sizeof(u_int8_t));//lit le fichier
		if(r==0){
			u_int8_t hash;//cree la variable hash qui stockera ce qui est lu
			hash=fd1; //donner la valeur a la variable hash
			sem_wait(sem1);
			pthread_mutex_lock(mut1);
			buffer1[0]=hash;//inserer le hash dans le buffer, je ne sais pas comment le faire entre ou il y a une place libre donc je le mets en position 0 mais a changer
			pthread_mutex_unlock(mut1);
			sem_post(sem1);
		}

	}
}