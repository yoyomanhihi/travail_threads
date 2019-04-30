#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
void cracker(File.bin){
	u_int8_t buffer1[3];//taille M+2 avec M qui vaut le nombre de type de fichier (1, 2 ou 3)
	char* buffer2[5];//taille N+2 avec N le nombre de threads		
	void lire(){//producteur qui lit dans les fichiers
		sem_t sem1;
		int s=sem_init(&sem1, 0, 3);// initialise la semaphore
		if(s==-1){
			perror("create semaphore");
		}
		pthread_mutex_t mut1;
		if (pthread_mutex_init(&mut1, NULL)!=0){ //si erreur different de 0 
			perror("mutex init"); 
		}
		
		int fd1=open("test-input/01_4c_1k.bin", O_RDONLY);//ouverture du fichier
		if(fd1==-1){
			perror("open file");
		}
		u_int8_t *rbuf1 = (u_int8_t *)malloc(sizeof(u_int8_t)); //buffer pour read
		int r=read(fd1, rbuf1, sizeof(u_int8_t)); //lit le fichier
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
				buffer1[0]=hash;//inserer le hash dans le buffer, je ne sais pas comment le faire entre ou il y a une place libre donc je le mets en position 0 mais a changer
				pthread_mutex_unlock(&mut1);//adresse et pas mut1
				sem_post(&sem1); //meme chose en cas d erreur
			}
		}
	}
	int a=pthread_create(pthread_t* restrict thread, NULL, lire, argument);//cree le thread libre
	if(a!=0){
		error(err,"phtread_create");
	}
	void decrypteur(buffer1){//threads de calculs
		uint8_t mdp;
		while(true){//tant qu'il y a a decrypter
			sem_wait(&full);//attente d'un slot rempli de buffer1
			pthread_mutex_lock(&mutex);
			mdp=remove(hash);//prend un hash dans buffer1
			pthread_mutex_unlock(&mutex);
			sem_post(&empty);
			mdp=reversehash(mdp);//applique reversehash
			sem_wait(&empty);//attente d'un slot libre
			pthread_mutex_lock(&mutex);
			insert_mdp();//insere dans buffer2
			pthread_mutex_unlock(&mutex);
			sem_post(&full);
		}
	}
	int b=pthread_create(pthread_t* restrict thread, NULL, decrypteur, argument);//cree le thread hasheur
	if(b!=0){
		error(err,"phtread_create");
	}
	void ecrire(){
		int max=0;
		char* candidat;
		int fd2=open(fileout, O_WRONLY|O_CREAT|O_TRUNC);//peut etre troisieme parametre qui correspond aux droits du fichier mais inutile je pense
		if(fd2==-1){
			error(err,"open fileout");
		}
		while(true){//tant qu'il y a à analyser dans buffer2
			sem_wait(&full);
			pthread_mutex_lock(&mutex);
			candidat=remove(mdp);
			pthread_mutex_unlock;
			sem_post(&empty);
			int a=count(candidat);//count=fonction qui compte les voyelles ou consonnes, a creer ou qui existe deja
			if(a>max){
				pthread_mutex_lock;
				clear(fileout);//vider le fichier
				int b=write(fd2, ptr, sizeof(char*(1+strlen(candidat))));//ptr a verifier, je comprends bof vers quoi ca pointe
				if(b==-1){
					error(err, "writeerror");
				}
				pthread_mutex_unlock;
			}
			if(a==max){
				pthread_mutex_unlock;
				int b=write(fd2, ptr, sizeof(char*(1+strlen(candidat))));//ptr a verifier, je comprends bof vers quoi ca pointe
				if(b==-1){
					error(err, "writeerror");
				}
				pthread_mutex_unlock;
			}
		}
	}
	int c=pthread_create(pthread_t* restrict thread, NULL, argument);
	if(b!=0){
		error(err,"phtread_create");
	}
}
