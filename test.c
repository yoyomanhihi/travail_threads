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
	int s=sem_init(sem1, 0, 3);// deuxieme parametre 0 ou 1 pas trop sur
	if(s==-1){
		perror("create semaphore1");
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
void decrypteur(){//threads de calculs
	u_int8_t mdp;
	sem_t *sem1=(sem_t *) malloc(sizeof(sem_t));//recree sem1 pour compilation ici mais dans cracker ce sera le meme que au dessus
	int s=sem_init(sem1, 0, 3);// deuxieme parametre 0 ou 1 pas trop sur
	if(s==-1){
		perror("create semaphore1");
	}
	sem_t *sem2=(sem_t *) malloc(sizeof(sem_t));
	int s2=sem_init(sem1, 0, 3);// deuxieme parametre 0 ou 1 pas trop sur
	if(s2==-1){
		perror("create semaphore2");
	}
	pthread_mutex_t *mut1=(pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));// je recree mut1 ici mais il faudra le faire une seule fois dans cracker au debut
	pthread_mutex_init(mut1, NULL);// verifier si pas erreur ?
	pthread_mutex_t *mut2=(pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));//Pour le test je recree un mutex mais toujours le meme dans cracker je pense
	pthread_mutex_init(mut2, NULL);// verifier si pas erreur ?
	while(1==1){//tant qu'il y a a decrypter dans buffer1, je ne sais pas comment faire ca donc j ecris time temporairement pour compiler
		sem_wait(sem1);//attente d'un slot rempli de buffer1
		pthread_mutex_lock(mut1);
		mdp=remove(hash);//prend un hash dans buffer1
		pthread_mutex_unlock(mut1);
		sem_post(sem1);
		mdp=reversehash(mdp);//applique reversehash
		sem_wait(sem2);//attente d'un slot libre
		pthread_mutex_lock(mut2);
		insert_mdp();//insere dans buffer2
		pthread_mutex_unlock(mut2);
		sem_post(sem2);
	}
}