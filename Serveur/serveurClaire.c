#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

//#include "../Concert/concert.c"

#define PORT 6000

//pthread_mutex_t mutex;

int places = 100;
char place[4];

typedef struct
{
    int fdSocketCommunication;
    struct sockaddr_in coordonneesAppelant;
    char nom[3];
} Com;

typedef struct
{
    char *numDossier;
    char *nom;
    char *prenom;
    int disponible;
} Place;

Place tablePlaces[100];

void *connexion(void *arg);
void affichePlaces(int s);
void prendUnePlace(int s);
void annulePlace(int s);
void deconnexion(int s);

int main()
{
    for (int i = 0; i < 100; i++)
    {
        tablePlaces[i].numDossier = "";
        tablePlaces[i].nom = "";
        tablePlaces[i].prenom = "";
        tablePlaces[i].disponible = 1;
    }

    sprintf(place, "%d", places);
    Com donnees;
    int fdSocketAttente;
    struct sockaddr_in coordonneesServeur;
    int longueurAdresse;
    fdSocketAttente = socket(PF_INET, SOCK_STREAM, 0);
    if (fdSocketAttente < 0)
    {
        printf("socket incorrecte\n");
        exit(-1);
    }

    /*
    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        printf("initialisation du mutex echouée\n");
    }
    */

    // On prépare l’adresse d’attachement locale
    longueurAdresse = sizeof(struct sockaddr_in);
    memset(&coordonneesServeur, 0x00, longueurAdresse);
    coordonneesServeur.sin_family = PF_INET;
    // toutes les interfaces locales disponibles
    coordonneesServeur.sin_addr.s_addr = htonl(INADDR_ANY);
    // toutes les interfaces locales disponibles
    coordonneesServeur.sin_port = htons(PORT);

    if (bind(fdSocketAttente, (struct sockaddr *)&coordonneesServeur,
             sizeof(coordonneesServeur)) == -1)
    {
        printf("erreur de bind\n");
        exit(-1);
    }
    if (listen(fdSocketAttente, 5) == -1)
    {
        printf("erreur de listen\n");
        exit(-1);
    }

    printf("Attente\n");

    socklen_t tailleCoord = sizeof(donnees.coordonneesAppelant);
    while (1)
    {
        if ((donnees.fdSocketCommunication = accept(fdSocketAttente, (struct sockaddr *)&donnees.coordonneesAppelant,
                                                    &tailleCoord)) == -1)
        {
            printf("erreur de accept\n");
            exit(-1);
        }
        else
        {
            pthread_t my_thread;
            int ret1 = pthread_create(&my_thread, NULL, connexion, (void *)&donnees);
            //pthread_join(my_thread, NULL);
        }
    }

    //pthread_mutex_destroy(&mutex);

    close(fdSocketAttente);

    return 0;
}

void *connexion(void *arg)
{
    int boucle = 1;
    Com *structure = (Com *)arg;
    char buffer[256];
    char nom[10];
    char nbPlaces[4];

    printf("Client connecté. IP : %s\n", inet_ntoa(structure->coordonneesAppelant.sin_addr));
    recv(structure->fdSocketCommunication, nom, 10, 0);

    printf("Client : %s\n", nom);

    while (boucle == 1)
    {
        int nbRecu = recv(structure->fdSocketCommunication, buffer, 256, 0);

        if (nbRecu > 0)
        {
            buffer[nbRecu] = 0;
            printf("Recu: %s\n", buffer);
            int choix = atoi(buffer);

            switch (choix)
            {
            case 1:
                affichePlaces(structure->fdSocketCommunication);
                break;

            case 2:
                prendUnePlace(structure->fdSocketCommunication);
                break;

            case 3:
                annulePlace(structure->fdSocketCommunication);
                break;

            case 4:
                write(structure->fdSocketCommunication, "Vous restez encore un peu avec nous alors :)", strlen("Vous restez encore un peu avec nous alors :)"));
                break;

            default:
                break;
            }
        }
        else
        {
            boucle = 0;
        }
    }

    close(structure->fdSocketCommunication);
    printf("Fin du client n°%s\nAttente de connexion\n", nom);
}

void affichePlaces(int s)
{

    int nbPlaces = 0;
    char message[100] = "Nombre de places restantes : ";
    char stringPlaces[4];

    for (int i = 0; i < 100; i++)
    {
        if (tablePlaces[i].disponible == 1)
            nbPlaces++;
    }

    sprintf(stringPlaces, "%d", nbPlaces);
    strcat(message, stringPlaces);

    write(s, message, strlen(message));
}

/**
 * @brief Décremente le nombre de place
 * @return void
 */
void prendUnePlace(int s)
{
    /*char places[256]; 
    int j=0; 
    while (j<100) {
        if (tablePlaces[j].disponible==1){  
            char p;
            strcat(places, j); 
        }
        else 
            j++;
    }

    write (s, "Voici les places disponibles : ", 32); 
    write (s, places, strlen(places)); */
    //pthread_mutex_lock(&mutex);

    int i;
    char message[63] = "Merci pour votre réservation, votre numéro de dossier est : ";
    char nomClient[50];
    char prenomClient[50];
    char num[50];
    char places[256]; 

    int Recu = recv(s, nomClient, 50, 0);
    printf("ok %d, value : %s\n", Recu, nomClient);
    int Recu2 = recv(s, prenomClient, 50, 0);
    printf("ok %d, value : %s\n", Recu2, prenomClient);
    fflush(stdout);
    printf("ok");


    

    for (int i = 0; i < 100; i++)
    {

        if (tablePlaces[i].disponible == 1)
        {
            int n=0;
            while (n < 10) {
                num[n] = '0'+(rand()%10);
                n++;
            }

            tablePlaces[i].disponible = 0;
            tablePlaces[i].nom = strdup(nomClient);
            tablePlaces[i].prenom = strdup(prenomClient);
            tablePlaces[i].numDossier = strdup(num);
            printf("Nom client[%d] : %s\n", i, nomClient);

            break;
        }
    }

    strcat(message, num);
    write(s, message, strlen(message));*/
}

void annulePlace(int s)
{
    char nom[50];
    char num[50];
    int Recu = recv(s, nom, 50, 0);
    printf("ok %d, value : %s\n", Recu, nom);
    int Recu2 = recv(s, num, 50, 0);
    printf("ok %d, value : %s\n", Recu2, num);

    int annule = 0;

    for (int i = 0; i < 100; i++)
    {
        printf("%s", tablePlaces[i].numDossier);
        
        if ((strcmp(tablePlaces[i].nom, nom) == 0) && (strcmp(tablePlaces[i].numDossier, num) == 0))
       {
            tablePlaces[i].disponible = 1;
            tablePlaces[i].nom = "";
            tablePlaces[i].prenom = "";
            tablePlaces[i].numDossier = "";
            write(s, "Place annulée avec succès", 28);
            annule = 1;
            break;
        }
    }

    if (annule == 0)
    {
        write(s, "Ce n° de dossier ou ce nom n'est affecté à aucune place", 59);
    }
}