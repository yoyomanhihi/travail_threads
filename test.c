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
char* buffer2[5];//taille N+2 avec N le nombre de threads
int size1=3;//taille de buffer1, sert plus tard
int size2=5;//taille de buffer2, sert plus tard
int debut1=0;//debut du buffer1 a partir de quoi le buffer est vide
int fin1=0;//fin de la zone vide de buffer1
int debut2=0;//idem que debut1 mais pour buffer2
int fin2=0;//idem que fin1 mais pour buffer2
int casehash=0;//indique dans quelle case du tableau du hash placer le byte
bool lecture=true;//indique que lire est encore en cours
bool decryptage=true;//indique que decrypteur est encore en cours
pthread_mutex_t mut1;
int err=pthread_mutex_init(&mut1, NULL);//initialise le mutex1
if (err!=0){ //si erreur
	perror("mutex init"); 
}
sem_t empty1; 
int e1=sem_init(&empty1, 0, 3);//cree semaphore vide qui compte 3 slots vides consommateur 1
if(e1==-1)//si erreur
	perror("create empty1");

sem_t full1;
int f1=sem_init(&full1, 0, 0); //cree semaphore vide qui compte 0 slot rempli consommateur 1
if(f1==-1)//si erreur
	perror("create full1");

sem_t empty2; 
int e2=sem_init(&empty2, 0, 3);//cree semaphore vide qui compte 5 slots vides producteur 2
if(e2==-1)//si erreur
	perror("create empty2");
	
sem_t full2;
int f2=sem_init(&full2, 0, 0); //cree semaphore vide qui compte 0 slot rempli producteur 2
if(f2==-1)//si erreur
	perror("create full2");

pthread_mutex_t mut2;//Pour le test je recree un mutex mais toujours le meme dans cracker je pense
int err2 = pthread_mutex_init(&mut2, NULL);
if(err2!=0)//si erreur
	perror("init mutex2 decrypt");

pthread_mutex_t mut3;//mutex servant a ecrire dans fichier de sortie
int err3 = pthread_mutex_init(&mut3, NULL);
if(err3!=0)//si erreur
	perror("init mutex3 decrypt");


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
	u_int8_t hash[32];//cree la variable hash qui stockera ce qui est lu
	while(read(fd1, rbuf1, sizeof(u_int8_t))>0){//Tant qu'il y a a lire
		hash [casehash]=*rbuf1; //donner la valeur a la variable hash
		if(casehash==31){
			sem_wait(&empty1); //peut etre regarder en cas d erreur
			int err1 = pthread_mutex_lock(&mut1); //lock le mut1
			if(err1!=0){//si erreur
				perror("mutex lock read"); 
			}
			buffer1[debut1]=hash;//inserer le hash dans le buffer a la premiere case libre
			if(debut1==(size1-1)){//si debut1 est a la derniere case
					debut1=0;//revient au debut
			}
			else{
				debut1++;//sinon debut1 augmente de 1;
			}
			err1 = pthread_mutex_unlock(&mut1);//unlock le mut1
			if(err1!=0){//si erreur
				perror("mutex unlock read");
			}
			sem_post(&full1); //meme chose en cas d erreur
			casehash=-1;
		}
		casehash++;
	}
	lecture=false;
}
int count(char* mot){ //compter les voyelles ou les consonnes mais faudra regarder comment on gere ce cas
	int len=strlen(mot); //variable qui comporte la taille du mot 
	int count=0; //nombre de voyelle ou consonne
	if(true) {//pas le bon if mais pour dire que le critere est voyelle
		for(int i=0; i<len; i++){
			if(mot[i]=='a' || mot[i]=='e' || mot[i]=='i' || mot[i]=='o' || mot[i]=='u' || mot[i]=='y')
				count++;
		}
	}
	else {//si le critere de selection est consonne
		for(int i=0; i<len; i++){
			if(mot[i]!='a' && mot[i]!='e' && mot[i]!='i' && mot[i]!='o' && mot[i]!='u' && mot[i]!='y')
				count++;
		}
	}
	return count;
}

//consommateur 1 et producteur 2 
void decrypteur(){//threads de calculs
	char *bufferInter=(char *) malloc(sizeof(char)*17); //buffer intermediaire utile a la fonction reversehash je beug jsp comment faire pour que le mot soit dans bufferinter
	u_int8_t *mdp=(u_int8_t *) malloc(sizeof(u_int8_t)*32);
	while((lecture==true)||(debut1!=fin1)){//tant que le thread a encore a lire et que le buffer n est pas vide, juste a verifier que la condition debut1!=fin1 n accepte pas un buffer totalement rempli
		sem_wait(&full1);//attente d un slot rempli
		pthread_mutex_lock(&mut1); //lock le mut1
		mdp=buffer1[fin1];//prend la premiere valeur de buf1 apres l espace vide
		if(fin1==(size1-1)){//si fin1 est a la derniere case
			fin1=0;//revient au debut
		}
		else{
			fin1++;//sinon fin1 augmente de 1;
		}
		sem_post(&empty1); //1 slot vide en plus 
		pthread_mutex_unlock(&mut1); //unlock mut1
		bool trouve=reversehash(mdp, bufferInter, 17){//si reversehash a trouve un inverse il le stocke dans bufferInter 
		if(trouve==true){
			printf("%s \n", bufferInter);
			sem_wait(&empty2);//attente d'un slot libre (5 max)
			pthread_mutex_lock(&mut2);
			if(debut2==(size2-1)){//si debut2 est a la derniere case
				debut2=0;//revient au debut
			}
			else{
				debut2++;//sinon debut2 augmente de 1;
			}
		}
		sem_post(&full2); //1 slot rempli en plus (5 max)
		pthread_mutex_unlock(&mut2);
	}
	decryptage=false;	
}

void ecrire(){
	int max=0;//maximum de voyelles ou consonnes lu
	char* candidat=NULL;//variabe servant a stocker la valeur lue dans buffer2 initialisee a null au depart
	int fd2=open("File", O_WRONLY|O_CREAT|O_TRUNC);//peut etre troisieme parametre qui correspond aux droits du fichier mais inutile je pense nom du fichier de sortie File mais jsp si nom demande
	if(fd2==-1)
		perror("open File write");
	
	while((decryptage==true)&&(debut2!=fin2)){//tant que la fonction decrypteur n a pas tremine et que le buffer2 n est pas vide, verifier condition 2 que pas de faille
		sem_wait(&full2);
		pthread_mutex_lock(&mut2);
		candidat=buffer2[fin2];// verifier que pas de probleme
		if(fin2==(size2-1)){//si fin2 est a la derniere case
			fin2=0;//revient au debut
		}
		else{
			fin2++;//sinon fin2 augmente de 1;
		}
		pthread_mutex_unlock(&mut2);
		sem_post(&empty2);//pour moi le sem_post doit pas etre la truc bizarre avec le candidat
		int test=count(candidat);//voir si c est mieux une fonction pour cons ET voy ou voir si plus opti de faire deux fonctions en fonction du critere
		if(test>max){
			pthread_mutex_lock(&mut3);
			char* wbuff1=malloc (sizeof(char)*(strlen(candidat)+1));
			freopen("File", "w", stdout);//ferme et reouvre le fichier en le vidant w pour le mode ecriture, pas sur stdout
			int b=write(fd2, wbuff1, sizeof(char)*(strlen(candidat)+1));//candidat va etre ecrit dans le fichier File
			if(b==-1){
				perror("write error");
			}
			max=test; //max prend donc la valeur de test
			pthread_mutex_unlock(&mut3);
		}
		if(test==max){
			pthread_mutex_lock(&mut3);
			char* wbuff1=malloc (sizeof(char)*(strlen(candidat)+1));
			int b=write(fd2, wbuff1, sizeof(char)*(strlen(candidat)+1));
			if(b==-1){
				perror("write error");
			}
			pthread_mutex_unlock(&mut3);
		}
	}	
}
