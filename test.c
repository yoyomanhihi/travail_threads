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
	u_int8_t buffer1[3];//taille M+2 avec M qui vaut le nombre de type de fichier (1, 2 ou 3)
	char* buffer2[5];//taille N+2 avec N le nombre de threads
	int size1=3;//taille de buffer1, sert plus tard
	int size2=5;//taille de buffer2, sert plus tard
	int debut1=0;//debut du buffer1 a partir de quoi le buffer est vide
	int fin1=0;//fin de la zone vide de buffer1
	int debut2=0;//idem que debut1 mais pour buffer2
	int fin2=0;//idem que fin1 mais pour buffer2
	void lire(){//producteur qui lit dans les fichiers
		int 
		sem_t sem1;
		int s=sem_init(&sem1, 0, 3);// deuxieme parametre 0 je pense et c est &sem1 et pas sem1 car on a besoin de l adresse pointee par sem1
		if(s==-1){
			perror("create semaphore");
		}
		pthread_mutex_t mut1;
		if (pthread_mutex_init(&mut1, NULL)!=0){//si erreur c est different de 0 
			perror("mutex init"); 
		}
		
		int fd1=open("test-input/01_4c_1k.bin", O_RDONLY);//ouverture du fichier
		if(fd1==-1){
			perror("open file");
		}
		u_int8_t *rbuf1 = (u_int8_t *)malloc(sizeof(u_int8_t)); //buffer pour read
		int r=read(fd1, rbuf1, sizeof(u_int8_t));//lit le fichier
		if(r==-1){ //erreur
			close(fd1); //on ferme le fd qui a ete ouvert en cas d erreur de read
			perror("read file");
		}
		while(r>0){//Tant qu'il y a a lire
			r=read(fd1, rbuf1, sizeof(u_int8_t));//lit le fichier
			if(r==-1){
				close(fd1); //on ferme le fd qui a ete ouvert en cas d erreur de read
				perror("read file");
			}	
			if(r!=0){ //tant qu il reste quelque hose a lire
				u_int8_t hash;//cree la variable hash qui stockera ce qui est lu
				hash=*rbuf1; //donner la valeur a la variable hash
				sem_wait(&sem1); //peut etre regarder en cas d erreur
				pthread_mutex_lock(&mut1); //adresse de mut1 pas mut1
				buffer1[debut1]=hash;//inserer le hash dans le buffer a la premiere case libre
				if(debut1==(size1-1)){//si debut1 est a la derniere case
					debut1=0;//revient au debut
				}
				else{
					debut1++;//sinon debut1 augmente de 1;
				}
				pthread_mutex_unlock(&mut1);//adresse et pas mut1
				sem_post(&sem1); //meme chose en cas d erreur
			}
		}
	}
//consommateur 1 et producteur 2 
	void decrypteur(){//threads de calculs
	u_int8_t mdp;
	sem_t sem1;//recree sem1 pour compilation ici mais dans cracker ce sera le meme que au dessus
	int s=sem_init(&sem1, 0, 3);
	if(s==-1){//en cas d erreur
		perror("create semaphore1");
	}
	sem_t sem2;
	int s2=sem_init(&sem2, 0, 5);
	if(s2==-1){//en cas d erreur
		perror("create semaphore2");
	}
	pthread_mutex_t mut1;// je recree mut1 ici mais il faudra le faire une seule fois dans cracker au debut
	pthread_mutex_init(&mut1, NULL);// verifier si pas erreur ?
	pthread_mutex_t mut2;//Pour le test je recree un mutex mais toujours le meme dans cracker je pense
	pthread_mutex_init(&mut2, NULL);// verifier si pas erreur ?
	while(1==1){//tant qu'il y a a decrypter dans buffer1, je ne sais pas comment faire ca donc j ecris time temporairement pour compiler
		sem_wait(&sem1);//attente d'un slot rempli de buffer1 attention inverse
		pthread_mutex_lock(&mut1);
		mdp=buffer1[fin1];//prend la premiere valeur de buf1 apres l espace vide
		if(fin1==(size1-1)){//si fin1 est a la derniere case
			fin1=0;//revient au debut
		}
		else{
			fin1++;//sinon fin1 augmente de 1;
		}
		pthread_mutex_unlock(&mut1);
		sem_post(&sem1);//ce sera l inverse 
		mdp=reversehash(mdp);//applique reversehash
		sem_wait(&sem2);//attente d'un slot libre
		pthread_mutex_lock(&mut2);
		buffer2[debut2]=mdp;
		if(debut2==(size2-1)){//si debut2 est a la derniere case
			debut2=0;//revient au debut
		}
		else{
			debut2++;//sinon debut2 augmente de 1;
		}
		pthread_mutex_unlock(&mut2);
		sem_post(&sem2);
	}
}