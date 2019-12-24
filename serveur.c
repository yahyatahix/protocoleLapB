    /* Serveur: retourne resultat (requete a+b)
     * Cree les fifos cli2serv et serv2cli
     * LANCER LE SERVEUR D'ABORD (car il ecrase fifos)
     */

    #include <sys/types.h>
    #include <fcntl.h>
    #include <string.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include<unistd.h>

    #define QUESTION "cli2serv"
    #define REPONSE "serv2cli"

    void ajouter_fanion(char* trame);
    void ajouter_bit_transparence(char *trame);
    void ajouter_bit_parite(char* trame);

    void trait(int fdr,int fdq;);
    
    char *crc(char *quest);
    char *xor(char *s1,char *s2);
    
    /* traitement du serveur */
    void enleverFcs(char *trame, char fcs[16]);
    void enlever_bit_parite(char *trame);
    void verifierCRC(char *fcs , char * quest);
    int  enlever_bit_transparence(char *trame);
    void enlever_fanion(char *trame);


void main()
{
    int fdq, fdr;
    unlink(QUESTION); //The unlink function deletes the file name filename
    unlink(REPONSE);
/* Creation fifos */
    if (mkfifo(QUESTION, 0644) == -1 ||
        mkfifo(REPONSE, 0644) == -1) {
        perror("Impossible creer fifos");
        exit(2);
    }
/* Attente des ouvertures Clients */
/* fdr et fdq descripteurs reponse/question */

    fdq = open(QUESTION, O_RDONLY);
    fdr = open(REPONSE, O_WRONLY);
    trait(fdr, fdq);
    close(fdq);
    close(fdr);
    unlink(QUESTION);
    unlink(REPONSE);
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

void trait(fdr, fdq)
int fdr, fdq;
/* fdr et fdq descripteurs reponse/question */
{
    
    //char *trame ;
    

/* traitement serveur
   * envoi reponse a question
   * trame  venant de client.
   * arret question = "Ciao"
 */

    while (1) {
        char quest[100];
        char rep[100];
        char fcs[16];
        read(fdq, quest, 100); //lit la question à l'aide du buffer  quest

        //sscanf(quest, "%s", trame); // sscanf permet de lire dans buffer  quest la question
        if (strcmp(quest, "01010011\0") == 0) {
            printf(" \n ************ trame non numéroté UA \n");
            strcpy(rep, "01110011\0");
            write(fdr, rep, 10);
            printf(" \n Bye serveur \n");
            break;
        }  

        enlever_fanion(quest);
        printf("\n **************éliminer le fanion ************** \n");
        puts(quest);

        enlever_bit_transparence(quest);
        printf("\n  **************éliminer les bits de transparence  **************\n" );
        puts(quest);

        enleverFcs(quest,fcs);
        printf("\n **************fcs enlever est :  ************** %s  \n", fcs);

        printf(" ************** quest reçu :  **************\n");
        puts(quest);

        //printf("\n  vérifier crc\n" );
        //verifierCRC(fcs ,  quest);
        //puts(quest);

        
        sprintf(rep, "%s", quest); //ssprintf permet d'écrire dans le buffer rep la reponse
        write(fdr, rep, 100);

         
    }
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

void enleverFcs(char *trame, char fcs[16])
{
    char resu[100];
    int i;
    for(i=0;i<strlen(trame);i++)
        {   
            if (i<strlen(trame)-16)
            {
                resu[i]=trame[i];
            }
            else
                fcs[i-strlen(trame)+16]=trame[i];
            
        }
    fcs[16]='\0';

    resu[strlen(trame)-16]='\0';
    strcpy(trame,resu);

}

void verifierCRC(char *fcs , char * quest)
{   int l;
    char copyQuest[100];
    char fcs2[5];
            strcpy(copyQuest,quest);
            char *c = crc(copyQuest);
            printf("crc\n");
            puts(c);
            //ajouter crc
            
            l=strlen(c);

            for(int i=l-16;i<l;i++)
            {  
                fcs2[i-l+16]=c[i];
            }
            printf("fcs l= %d\n",l);
            puts(fcs2);

        if(strcmp(fcs,fcs2)==0)
            printf("message bien recue\n");
        else 
            printf("message errone\n");
}


char * crc(char * quest)
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