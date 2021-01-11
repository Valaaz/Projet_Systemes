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
#include <semaphore.h>

#define PORT 6000
static sem_t semaphore;

typedef struct
{
    int fdSocketCommunication;
    struct sockaddr_in coordonneesAppelant;
} Com;

typedef struct
{
    char numDossier[11];
    char nom[50];
    char prenom[50];
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
    srand(time(NULL));
    sem_init(&semaphore, PTHREAD_PROCESS_SHARED, 1);
    for (int i = 0; i < 100; i++)
    {
        strcpy(tablePlaces[i].numDossier, "");
        strcpy(tablePlaces[i].nom, "");
        strcpy(tablePlaces[i].prenom, "");
        tablePlaces[i].disponible = 1;
    }

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
            Com *d = (Com *)malloc(sizeof(Com));
            d->coordonneesAppelant = donnees.coordonneesAppelant;
            d->fdSocketCommunication = donnees.fdSocketCommunication;
            pthread_t my_thread;
            pthread_create(&my_thread, NULL, connexion, (void *)d);
        }
    }

    sem_destroy(&semaphore);
    close(fdSocketAttente);

    return 0;
}

void *connexion(void *arg)
{
    int boucle = 1;
    Com *structure = (Com *)arg;
    char buffer[256];

    printf("Client connecté. IP : %s\n", inet_ntoa(structure->coordonneesAppelant.sin_addr));

    while (boucle == 1)
    {
        int nbRecu = recv(structure->fdSocketCommunication, buffer, 256, 0);

        if (nbRecu > 0)
        {
            buffer[nbRecu] = 0;
            printf("\nRecu: %s\n", buffer);
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
    free(structure);
    printf("Fin du client\nAttente de connexion\n");
    pthread_exit(NULL);
}

void affichePlaces(int s)
{

    int nbPlaces = 0;
    char message[100] = "Nombre de places restantes : ";
    char stringPlaces[4];

    sem_wait(&semaphore);
    for (int i = 0; i < 100; i++)
    {
        if (tablePlaces[i].disponible == 1)
            nbPlaces++;
    }
    sem_post(&semaphore);

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
    char message[500] = "Merci pour votre réservation, votre numéro de dossier est :";
    char nomClient[50];
    char prenomClient[50];
    char num[50] = "";
    char places[4];
    char liste[500] = "";
    char ticket[4];

    recv(s, nomClient, 50, 0);
    recv(s, prenomClient, 50, 0);

    sem_wait(&semaphore);
    for (int i = 0; i < 100; i++)
    {
        if (tablePlaces[i].disponible)
        {
            sprintf(places, "%d", 1 + i);
            strcat(places, " | ");
            strcat(liste, places);
        }
    }
    sem_post(&semaphore);
    send(s, liste, strlen(liste), 0);

    recv(s, ticket, 4, 0);
    int numPlace = atoi(ticket) - 1;
    int x = -1;

    sem_wait(&semaphore);
    if (!tablePlaces[numPlace].disponible)
    {
        printf("%s", "cette place est déjà réservée");
        strcpy(message, "Cette place est déjà réservée");
    }
    else
    {
        x = 1000000000 + rand() % (9999999999 + 1 - 1000000000);
        if (x < 0)
            x *= -1;

        sprintf(num, "%d", x);

        tablePlaces[numPlace].disponible = 0;
        strcpy(tablePlaces[numPlace].nom, nomClient);
        strcpy(tablePlaces[numPlace].prenom, prenomClient);
        strcpy(tablePlaces[numPlace].numDossier, num);

        strcat(message, num);
        strcat(message, ". Votre place est la n°");
        strcat(message, ticket);

        printf("La place n° %d a été réservé par %s %s, le numéro de dossier est : %s\n", numPlace + 1, nomClient, prenomClient, num);
    }
    sem_post(&semaphore);

    write(s, message, strlen(message));
}

void annulePlace(int s)
{
    char nom[50];
    char num[50];
    recv(s, nom, 50, 0);
    recv(s, num, 50, 0);

    int annule = 0;

    sem_wait(&semaphore);
    for (int i = 0; i < 100; i++)
    {

        if ((strcmp(tablePlaces[i].nom, nom) == 0) && (strcmp(tablePlaces[i].numDossier, num) == 0))
        {
            tablePlaces[i].disponible = 1;
            write(s, "Place annulée avec succès", 28);
            annule = 1;
            printf("La place n° %d a bien été annulée.\n", i + 1);
            break;
        }
    }
    sem_post(&semaphore);

    if (annule == 0)
    {
        write(s, "Ce n° de dossier ou ce nom n'est affecté à aucune place", 59);
    }
}