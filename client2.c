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


//void enlever_bit_parite(char *trame);
char *crc(char * quest);
char *xor(char *s1,char *s2);
int enlever_bit_transparence(char *trame);
void enlever_fanion(char *trame);

void main()
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
        char copyQuest[100]="";
        char fcs[16];
        int l;

        printf("\n\n entrer les données en binaire à envoyer à l'aide du protocole lapB  :\n");
        
        if (gets(quest) == NULL)
            exit(2);

        if (strcmp(quest, "Ciao") != 0)
        {
            //ajouter_bit_parite(quest);
            //printf("\n ajout du bit de parité \n");
            //puts(quest);
            strcpy(copyQuest,quest);
            char *c = crc(copyQuest);
            printf("crc\n");
            puts(c);
            //ajouter crc
            
            l=strlen(c);

            for(int i=l-16;i<l;i++)
            {  
                fcs[i-l+16]=c[i];
            }
            printf("fcs l= %d\n",l);
            puts(fcs);

            printf("\n ajout du fcs \n");
            strcat(quest,fcs);
            puts(quest);


            ajouter_bit_transparence(quest) ;
            printf("\n ajout des bits de transparence \n" );
            puts(quest);
            
            ajouter_fanion(quest) ;
            printf("\n ajout du fanion  \n" );
            puts(quest);
        }

        /*if (strcmp(quest, "Ciao") == 0)

        {
            
            Les trames non numérotées ou trames  U (Unnumbered) sont utilisées pour  
            toutes les fonctions de contrôle de la liaison telles que l’initialisation, 
            la libération... Elles ne transportent pas de données.
            
            printf("trame non numéroté DISC\n");
            strcpy(quest, "01010011\0"); //trame non numéroté DISC
            ajouter_fanion(quest) ;


        }
        */


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


//Fonction CRC :
//Fonction qui calcule le crc sur un message afin de pouvoir toujours détecter les erreurs de certains types,
// comme celles dues par exemple, aux interférences lors de la transmission.

char *crc(char *quest)
{ 
    char *s2="10001000000100001";//le polynome generateur
    char temp[18];
    char *result=malloc(sizeof(char)*18);
    //strcat(d,quest);
    strcat(quest,"0000000000000000");//on multiplie le polynome du msg par le degre du generateur
    int k,p=0,i=0,j=16;
    i=0;
    int lenght = strlen(quest);
    while(j<lenght)
    {
        for(k=i;k<j+1;k++)
        {
            temp[k-i]=quest[k];
        }
        temp[j+1]='\0';
        result = xor(temp,s2);
        k=p;
        while(result[k-p]!='\0')
        { 
            quest[k]=result[k-p];
            k++;  
        }
        p=0;
        while(quest[p]=='0')
        {
            p++;
        }
        i=0;
        j=16;
        i+=p;
        j+=p;
    } 
    return quest;
}
//Fonction XOR (ou exclusif)
//Cette fonction calcule le ou exclusif de deux chaines est retourne une chaine résultat 
//que je l’utilise pour calculer le CRC.

char * xor(char *s1,char *s2)
{
  char * temp=malloc(sizeof(char)*17);
  int i;
  for(i=0;i<17;i++)
  {
    if(*(s1+i)== *(s2+i))
    {
      *(temp+i) = '0';
    }
    else
    {
      *(temp+i) = '1';
    }

  }
  temp[i]='\0';
  return temp;
}

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