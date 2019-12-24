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

    void trait(int fdr,int fdq;);             /* traitement du serveur */
    void enlever_bit_parite(char *trame);
    int enlever_bit_transparence(char *trame);
    void enlever_fanion(char *trame);


main()
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
        read(fdq, quest, 100); //lit la question à l'aide du buffer  quest

        //sscanf(quest, "%s", trame); // sscanf permet de lire dans buffer  quest la question
        if (strcmp(quest, "Ciao") == 0) {
            strcpy(rep, "Bye");
            write(fdr, rep, 10);
            break;
        }  

        enlever_fanion(quest);
        printf("\n éliminer le fanion  \n");
        puts(quest);

        enlever_bit_transparence(quest);
        printf("\n éliminer les bits de transparence \n" );
        puts(quest);

        enlever_bit_parite(quest);
        printf("\n  éliminer le bit de parité \n" );
        puts(quest);
        
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