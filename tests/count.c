#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>
int cflag=0;
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

int main(int argc, char *argv[]){
	int a;

	while((a=getopt (argc, argv, "c"))!=-1){
		switch(a){
			case 'c'://si on met l argument -c pour executer cracker
				cflag=1;
				break;
		}
	}
	char *mot=argv[optind];
	int resultat=count(mot);
	printf("resultat: %d \n", resultat);
}
