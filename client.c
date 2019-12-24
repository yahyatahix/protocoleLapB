/* Client: envoie expressions
   * Les fifos sont supposes crees
   * par le serveur et sont cli2serv
   * et serv2cli
 */

#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>

#define QUESTION "cli2serv"
#define REPONSE "serv2cli"

void trait(int fdr, int fdq);                   /* traitement client */
void ajouter_fanion(char* trame);
void ajouter_bit_transparence(char *trame);
void ajouter_bit_parite(char* trame);


void enlever_bit_parite(char *trame);
int enlever_bit_transparence(char *trame);
void enlever_fanion(char *trame);

main()
{
    int fdq, fdr;
    
    fdq = open(QUESTION, O_WRONLY); //file descriptor question
    
    if (fdq == -1) {
        fprintf(stderr, "Impossible ouvrire fifo %s\n", QUESTION);
        fprintf(stderr, "Lancer serveur d\'abord?\n");
        exit(2);
    }
    
    fdr = open(REPONSE, O_RDONLY);
    
    if (fdr == -1) {
        fprintf(stderr, "Impossible ouvrire fifo %s\n", REPONSE);
        fprintf(stderr, "Lancer serveur d\'abord?\n");
        exit(2);
    }
    
    trait(fdr, fdq);
    
    close(fdq);
    
    close(fdr);
    exit(0);
}

/*
*
*
*
Définition des fonctions
*
*
*
*/

void trait(int fdr, int fdq)

/* fdr et fdq descripteurs reponse/question */
{
    char rep[100];
    
/* traitement client
   * lecture de la trame
   * dans stdin et ecriture reponse
   * dans stdout. 
   Arret rep = "Bye"
*/

    while (1) {
        char quest[100] = "";
        
        printf("\n\n entrer les données en binaire à envoyer à l'aide du protocole lapB  :\n");
        
        if (gets(quest) == NULL)
            exit(2);

        if (strcmp(quest, "Ciao") != 0)
        {
            ajouter_bit_parite(quest);
            printf("\n ajout du bit de parité \n");
            puts(quest);

            ajouter_bit_transparence(quest) ;
            printf("\n ajout des bits de transparence \n" );
            puts(quest);
            
            ajouter_fanion(quest) ;
            printf("\n ajout du fanion  \n" );
            puts(quest);
        }


        write(fdq, quest, 100);   //écrire la question dans le buffer  quest
        printf("Client ->  \n");
        puts(quest);

        read(fdr, rep, 100);
        printf("Serveur ->  \n");
        puts(rep);
        
        if (strcmp(rep, "Bye") == 0)
            break;
    }
}

/*
Ajouter
*/

void ajouter_fanion(char * trame)
{
    char fanion[100];
    strcpy(fanion , "01111110") ;
    strcat(fanion,trame);
    strcat(fanion,"01111110");
    strcpy(trame,fanion);
}

void ajouter_bit_transparence(char *trame)
{
    char tmp[200];
    int j=0;
    int compteur=0;
    
    for( int i=0 ; i<strlen(trame) ; i++)
    {
        tmp[j]=trame[i];
        if(tmp[j]=='1') 
            compteur++;
        else 
            compteur=0;
        j++;
        
        if(compteur==5)
            {
                tmp[j]='0';
                j++;
                compteur=0;
            }
    }
    tmp[j]='\0';
    strcpy(trame,tmp);
    
}   

void ajouter_bit_parite(char* trame)
{
    
    int p=0;
    for(int i=0;i<strlen(trame);i++)
        {
            if(trame[i]=='1') 
                {
                    p++ ;
                }
        }
    //printf("%d\n" , p) ;
    if(p%2==0)
        {
            trame[strlen(trame)]='0';
            trame[strlen(trame)+1]='\0';
        }
    else 
        {
            trame[strlen(trame)]='1';
            trame[strlen(trame)+1]='\0';
        }
}

/*
Enlever
*/

void enlever_bit_parite(char *trame)
{
    char donnee[50];
    int p=0;
    int l=strlen(trame);
    for(int i=0;i<l-1;i++)
        {   
            donnee[i]=trame[i];   
        }
    
    strcpy(trame,donnee);
    
}
int enlever_bit_transparence(char *trame)
{
    char resu[100];
    int j=0;
    int cpt=0;
    int i;
    for(i=0 ; i<strlen(trame) ; i++)
    {
        resu[j]=trame[i];
        if(trame[i]=='1') cpt++;
        else cpt=0;
        j++;
        if(cpt==5)
            {
                if(trame[i+1]=='1') 
                {
                    
                    return 0; 
                    break;
                }
                else
                    {
                        i++;
                        cpt=0; 
                    }
            }
    }
    resu[j]='\0';
    strcpy(trame,resu);
    return 1;
}

void enlever_fanion(char *trame)
{

    char resu[100];
    int i;
    for(i=8;i<strlen(trame)-8;i++)
        {
            resu[i-8]=trame[i];
        }
    resu[i-8]='\0';
    strcpy(trame,resu);
}