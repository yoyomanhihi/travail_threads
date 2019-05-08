#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>
#include "sha256.h"
#include "sha256.c"
#include "reverse.h"
#include "reverse.c"
int taille_fichier;
uint8_t **buffer1;//taille M+2 avec M qui vaut le nombre de type de fichier (1, 2 ou 3)
char* buffer2[5];//taille N+2 avec N le nombre de threads
int size1=3;//taille de buffer1, sert plus tard
int size2=5;//taille de buffer2, sert plus tard
int debut1;//debut du buffer1 a partir de quoi le buffer est vide
int fin1;//fin de la zone vide de buffer1
int debut2=0;//idem que debut1 mais pour buffer2
int fin2=0;//idem que fin1 mais pour buffer2
int j=1; //j va augmenter jusqu a atteindre taille_fichier dans decrypteur qui est une condition de la boucle while

//creation des semaphores 
sem_t empty1; 
sem_t empty2;
sem_t full1;
sem_t full2;

//creation des mutex
pthread_mutex_t mut1;//mutex servant a la fonction lire
pthread_mutex_t mut2;//mutex servant a la fonction decrypteur
pthread_mutex_t mut3;//mutex servant a ecrire dans fichier de sortie

//creation d une structure list_t et d une structure node_t permettant de stocker les candidats dans ecrire
typedef struct node{//represente un noeud 
	struct node *next;
	char* candid;
} node_t;
typedef struct list{//represente la liste qui sert a stocker les candidats dans ecrire
	struct node *first;
	int sizelist;
} list_t;

//variables utiles pour getopt
int cflag=0;
int tvalue = 1;
char *ovalue = NULL;
int tab [50];

//fonction count qui compte les voyelles ou les consonnes en fonction du critere de selection
int count(char* mot){ 
	int len=strlen(mot); //variable qui comporte la taille du mot 
	int count=0; //nombre de voyelle ou consonne
	if(cflag==0) {//le critere est voyelle
		for(int i=0; i<len; i++){
			if(mot[i]=='a' || mot[i]=='e' || mot[i]=='i' || mot[i]=='o' || mot[i]=='u' || mot[i]=='y')
				count++;
		}
	}
	else {//le critere est consonne
		for(int i=0; i<len; i++){
			if(mot[i]!='a' && mot[i]!='e' && mot[i]!='i' && mot[i]!='o' && mot[i]!='u' && mot[i]!='y')
				count++;
		}
	}
	return count;
}

//producteur 1 qui lit dans les fichiers
void *lire(void *fd){
	int fd1=*(int *)fd;//cast en int du fd donne en argument 
	buffer1=(uint8_t **) malloc(3*sizeof(uint8_t *));
	debut1=0;
	u_int8_t *rbuf1 = (u_int8_t *)malloc(sizeof(u_int8_t)*32); //cree le buffer pour read
	while(read(fd1, rbuf1, sizeof(u_int8_t)*32)>0){//Tant qu'il y a a lire
		u_int8_t *rbuf = (u_int8_t *)malloc(sizeof(u_int8_t)*32); // cree le buffer pour read7
		for(int i=0; i<32; i++){//copie de rbuf1 dans rbuf 
			rbuf[i]=rbuf1[i];
		}
		sem_wait(&empty1); 
		int err1 = pthread_mutex_lock(&mut1); //lock le mut1
		if(err1!=0){//si erreur
			perror("mutex lock read"); 
		}
		buffer1[debut1]=rbuf;//inserer le hash dans le buffer a la premiere case libre
		if(debut1==(size1-1)){//si debut1 est a la derniere case
			debut1=-1;//revient au debut
		}
		debut1++;//sinon debut1 augmente de 1;
		err1 = pthread_mutex_unlock(&mut1);//unlock le mut1
		if(err1!=0){//si erreur
			perror("mutex unlock read");
		}
		sem_post(&full1); 
	}
	free(rbuf1);
	if(close(fd1)==-1){//si erreur lors de la fermeture
		perror("error close file read");
	}
	pthread_exit(NULL);	
}


//consommateur 1 et producteur 2 qui decrypte les mdp
void *decrypteur(){
	char *bufferInter=(char *) malloc(sizeof(char)*17);//buffer intermediaire utile a la fonction reversehash
	u_int8_t *mdp;
	while((debut1!=fin1)||j<=taille_fichier){//tant qu on a pas decrypte tous les elements du fichier
		sem_wait(&full1);//attente d un slot rempli
		pthread_mutex_lock(&mut1); //lock le mut1
		j++;
		printf("j = %d\n",j);
		mdp=buffer1[fin1];//prend la premiere valeur de buf1 apres l espace vide
		if(fin1==(size1-1))//si fin1 est a la derniere case
			fin1=-1;//revient au debut
		
		fin1++;//sinon fin1 augmente de 1;
		pthread_mutex_unlock(&mut1); //unlock mut1
		sem_post(&empty1); //1 slot vide en plus
		bool trouve=reversehash(mdp, bufferInter, 17);//si reversehash a trouve un inverse il le stocke dans bufferInter 
		printf("%s\n", bufferInter);
		sem_wait(&empty2);//attente d'un slot libre (5 max)
		pthread_mutex_lock(&mut2);
		if(trouve==true){
			buffer2[debut2]=bufferInter;//on remet dans le deuxieme buffer le mdp passe dans reversehash qui se trouve dans bufferinter
			if(debut2==(size2-1)){//si debut2 est a la derniere case
				debut2=0;//revient au debut
			}
			else{
				debut2++;//sinon debut2 augmente de 1;
			}
		}
		pthread_mutex_unlock(&mut2);
		sem_post(&full2); //1 slot rempli en plus (5 max)
	}
	printf("je sors de la boucle decrypteur \n");
	pthread_exit(NULL);		
}

void *ecrire(){
	list_t *list=(list_t *) malloc(sizeof(list_t));
	int max=-1;//maximum de voyelles ou consonnes lues
	int k=1;
	printf("taille du fichier = %d\n", taille_fichier);
	char* candidat=NULL;//variabe servant a stocker la valeur lue dans buffer2 initialisee a null au depart
	while(k<=taille_fichier||debut2!=fin2){//tant que k n'a pas atteint la taille du fichier ou que debut2 est different de fin2
		sem_wait(&full2);
		pthread_mutex_lock(&mut2);
		candidat=buffer2[fin2];// verifier que pas de probleme
		if(fin2==(size2-1))//si fin2 est a la derniere case
			fin2=0;//revient au debut
		
		else
			fin2++;//sinon fin2 augmente de 1;
		
		int test=count(candidat);//on regarde le nombre de voyelles/consonnes en fonction du critere grace a count
		pthread_mutex_unlock(&mut2);
		sem_post(&empty2);
		if(test==max){//si on trouve un autre candidat avec le meme nombre de voyelles/consonnes
			//printf("test==max \n");
			char* corr=(char *) malloc (sizeof(strlen(candidat)+1));//variable de correction de bug
			for(int i=0; i<=strlen(candidat); i++){
				corr[i]=candidat[i];
			}
			pthread_mutex_lock(&mut3);
			node_t *n=(node_t *) malloc(sizeof(node_t));//creation du nouveau noeud	
			if(n==NULL){//si erreur
				perror("node creation");
			}
			node_t *run=list->first;//noeud runner pour parcourir la liste
			int doublons=-1;
			while(run->next!=NULL){
				if(strcmp(run->candid, corr)==0){//gestion des doublons, si le candidat existe deja, on initialise doublons a 0
					doublons=0;
					printf("doublons = %d\n", doublons);
				}
				run=run->next;
			}
			if(doublons!=0){//si le candidat n'existe pas on l ajoute a la liste
				run->next=n;
				n->next=NULL;
				n->candid=corr;
				list->sizelist++;
			}
			k++;
			pthread_mutex_unlock(&mut3);
		}
		else if(test>max){//si on trouve un candidat plus precis encore 
			//printf("test>max \n");
			char* corr=(char *) malloc (sizeof(strlen(candidat)+1));//variable de correction de bug
			for(int i=0; i<=strlen(candidat); i++){
				corr[i]=candidat[i];
			}
			pthread_mutex_lock(&mut3);
			node_t *n=(node_t *) malloc(sizeof(node_t));//creation d un noeud
			if(n==NULL){
				perror("node creation");
			}
			n->candid=corr;
			list->first=n;
			n->next=NULL;
			list->sizelist=1;
			k++;	
			max=test;//max devient test 
			pthread_mutex_unlock(&mut3);
		}
		else{
			pthread_mutex_lock(&mut3);
			k++;
			pthread_mutex_unlock(&mut3);
		}
	}
	printf("je suis sorti de la boucle ecrire \n");
	node_t *n=list->first;//noeud runner pour parcourir la liste
	int count=0;
	if(ovalue==NULL){
		while(n!=NULL){//parcourt la liste 
			count++;
			printf("Candidat : %s \n", n->candid);
			n=n->next;
		}
	}
	else{
		int fd2=open(ovalue, O_WRONLY|O_TRUNC|O_CREAT);//ouverture du fichier ou les candidats seront ecrits
		if(fd2==-1){
			perror("error open File write");
		}
		while(n!=NULL){//parcourt la liste 
			char *wbuff1=n->candid;
			int b=write(fd2, wbuff1, sizeof(char)*(strlen(n->candid)));
			count++;
			if(b==-1){
				perror("write error");
			}
			n=n->next;
		}
		if(close(fd2)==-1){
		perror("error close file write");
		}
	}
	printf("Le nombre de candidats est de %d\n", count);
	pthread_exit(NULL);	
}

int main(int argc, char *argv[]){
//ouverture du fichier
int fd=open("test-input/01_4c_1k.bin", O_RDONLY);
if(fd==-1) //si erreur
	perror("open file");

struct stat *recup=(struct stat *)malloc(sizeof(struct stat));
int test=fstat(fd, recup);//pour recuperer la taille du fichier
if(test==-1) //si erreur
	perror("stat");

taille_fichier = recup->st_size;
taille_fichier=taille_fichier/32; //divise par la taille d'un hash
free(recup);

//creation des semaphores
	int e1=sem_init(&empty1, 0, 3);//cree semaphore vide qui compte 3 slots vides consommateur 1
	if(e1==-1){//si erreur
		perror("create empty1");
	}
	int f1=sem_init(&full1, 0, 0); //cree semaphore vide qui compte 0 slot rempli consommateur 1
	if(f1==-1){//si erreur
		perror("create full1");
	}
	int e2=sem_init(&empty2, 0, 5);//cree semaphore vide qui compte 5 slots vides producteur 2
	if(e2==-1){//si erreur
		perror("create empty2");
	}
	int f2=sem_init(&full2, 0, 0); //cree semaphore vide qui compte 0 slot rempli producteur 2
	if(f2==-1){//si erreur
		perror("create full2");
	}

//creation des mutex
	int err=pthread_mutex_init(&mut1, NULL);
	if (err!=0) //si erreur 
	perror("mutex init"); 
	
	int err2 = pthread_mutex_init(&mut2, NULL);
	if(err2!=0)//si erreur
		perror("init mutex2 decrypt");
	
	int err3 = pthread_mutex_init(&mut3, NULL);
	if(err3!=0)//si erreur
		perror("init mutex3 decrypt");
	
//gerer les arguments
	int index;
	int z;
	int i=0;

	opterr=0;

	while((z=getopt (argc, argv, "co:t:"))!=-1){
		switch(z){
			case 'c':
				cflag=1;
				break;
			case 't':
				tvalue=atoi(optarg);
				break;
			case 'o':
				ovalue=optarg;
				break;
			case '?':
				if(optopt=='t'){
					fprintf(stderr, "Option -t requires an argument.\n");
				}
				if(optopt=='o'){
					fprintf(stderr, "Option -o requires an argument.\n");
				}
				else{
					fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
				}
				return 1;

		}
		
		for(index=optind; index<argc; index++){
			printf ("Non-option argument %s\n", argv[index]);
			int fd=open(argv[index], O_RDONLY);
			tab[i]=fd;
			i++;
		}

		for(int i=0; tab[i]!=0; i++){
			printf("%dppp", tab[i]);
		}
	}
	printf("cflag=%d, tvalue=%d, ovalue=%s\n", cflag, tvalue, ovalue);

//creation des threads
	int err_threads;
	pthread_t lire_t;
	pthread_t decrypter_t[tvalue];
	pthread_t ecrire_t;

	err_threads=pthread_create(&lire_t, NULL, &lire, &fd);//cree le premier thread lire
	if(err_threads!=0){
		perror("thread lire");
	}

	for(int i=0;i<tvalue;i++){
		err_threads=pthread_create(&(decrypter_t[i]), NULL, &decrypteur, NULL);//cree les seconds threads decrypteur en fonction de l argument donne lors de l excecution
		if(err_threads!=0){
			perror("thread decrypteur");
		}
	}

	err_threads=pthread_create(&ecrire_t, NULL, &ecrire, NULL);//cree le dernier thread ecrire
	if(err_threads!=0){
		perror("thread ecrire");
	}


	pthread_join(lire_t, NULL);
	for(int i=0;i<tvalue;i++){
		pthread_join(decrypter_t[i], NULL);
	}
	pthread_join(ecrire_t, NULL);

//destruction des mutex
	pthread_mutex_destroy(&mut1);
	pthread_mutex_destroy(&mut2);
	pthread_mutex_destroy(&mut3);

	return 0;
}
