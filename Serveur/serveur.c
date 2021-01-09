#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

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
    int disponible;
} Place;

Place tablePlaces[100];

void *connexion(void *arg);
void choixAction();
void affichePlaces(int s);
void prendUnePlace(int s);
void deconnexion(int s);

int main()
{
    for (int i = 1; i <= 100; i++)
    {
        tablePlaces[i].numDossier = NULL;
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
        write(structure->fdSocketCommunication, "\n\nQue voulez-vous faire ?\nConsulter les places disponibles (1)\nRéserver une place (2)\nAnnuler une place(3)\nQuitter (4)\n", strlen("\nQue voulez-vous faire ?\nConsulter les places disponibles (1)\nRéserver une place (2)\nAnnuler une place(3)\nQuitter (4)\n"));

        int nbRecu = recv(structure->fdSocketCommunication, buffer, 256, 0);

        if (nbRecu > 0)
        {
            buffer[nbRecu] = 0;
            printf("Recu: %s\n", buffer);
            int choix = atoi(buffer);
            fflush(stdout);
            choixAction(structure->fdSocketCommunication, choix);
        }
        else
        {
            boucle = 0;
        }
    }

    close(structure->fdSocketCommunication);
    printf("Fin du client n°%s\nAttente de connexion\n", nom);
}

/**
 * @brief Effectue une action en fonction du choix du client
 * @return void
 */
void choixAction(int s, int choix)
{
    //sleep(1); //Attends une seconde avant que le menu ne s'affichent afin de ne pas embrouiller l'utilisateur

    switch (choix)
    {
    case 1:
        affichePlaces(s);
        break;

    case 2:
        prendUnePlace(s);
        break;

    case 3:
        //annuleUnePlace(&x);
        write(s, "Cas 3", 6);
        break;

    case 4:
        deconnexion(s);
        break;

    default:
        printf("Veuillez choisir un chiffre entre 1 et 4\n");
        break;
    }
}

/**
 * @brief Affiche le nombre de place restant
 * @return void
 */
void affichePlaces(int s)
{
    int nbPlaces = 0;
    char message[100] = "Nombre de places restantes : ";
    char stringPlaces[4];

    for (int i = 1; i <= 100; i++)
    {
        if (tablePlaces[i].disponible == 1)
            nbPlaces++;
    }

    sprintf(stringPlaces, "%d", nbPlaces);
    strcat(message, stringPlaces);

    write(s, message, strlen(message));
    //sleep(1);
    //write(s, "\nQue voulez-vous faire ?\nConsulter les places disponibles (1)\nRéserver une place (2)\nAnnuler une place(3)\nQuitter (4)\n", strlen("\nQue voulez-vous faire ?\nConsulter les places disponibles (1)\nRéserver une place (2)\nAnnuler une place(3)\nQuitter (4)\n"));
}

/**
 * @brief Décremente le nombre de place
 * @return void
 */
void prendUnePlace(int s)
{
    //pthread_mutex_lock(&mutex);

    int i;
    char message[100] = "Vous avez la place n°";
    char stringPlaces[4];
    for (i = 1; i <= 100; i++)
    {
        if (tablePlaces[i].disponible == 1)
        {
            tablePlaces[i].disponible = 0;
            break;
        }
    }
    if (tablePlaces[100].disponible == 0)
        write(s, "Désolé, il n'y a plus de places disponibles..\n", strlen("Désolé, il n'y a plus de places disponibles..\n"));
    else
    {
        sprintf(stringPlaces, "%d", i);
        strcat(message, stringPlaces);
        write(s, message, strlen(message));
    }

    //pthread_mutex_unlock(&mutex);
}

/**
 * @brief Déconnecte le client du serveur
 * @return void
 */
void deconnexion(int s)
{
    write(s, "Déconnexion..", strlen("Déconnexion.."));
    //shutdown(s, SHUT_RDWR);
    //send(s, "0", strlen("0"), 0);
}