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

// Déclaration de la semaphore
static sem_t semaphore;

typedef struct
{
    int fdSocketCommunication;
    struct sockaddr_in coordonneesAppelant;
} Com;

// Structure définissant une place
typedef struct
{
    char numDossier[11];
    char nom[50];
    char prenom[50];

    // 0 = place indisponible, 1 = place disponible
    int disponible;
} Place;

// Création d'un tableau de 100 places
Place tablePlaces[100];

void *connexion(void *arg);
void affichePlaces(int s);
void prendUnePlace(int s);
void annulePlace(int s);

int main()
{
    srand(time(NULL));
    // Initialisation de la semaphore
    sem_init(&semaphore, PTHREAD_PROCESS_SHARED, 1);

    // Initialisation et nettoyage des places
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
        else // Permet la gestion de multiples clients (pour éviter que le dernier client connecté bloque tous les autres)
        {
            // Allocation de la mémoire de la taille de la structure Com
            Com *d = (Com *)malloc(sizeof(Com));
            // Affectation des données de "donnees" aux nouvelles variables de "d"
            d->coordonneesAppelant = donnees.coordonneesAppelant;
            d->fdSocketCommunication = donnees.fdSocketCommunication;

            pthread_t my_thread;
            // Création d'un nouveau thread avec les données de "d"
            pthread_create(&my_thread, NULL, connexion, (void *)d);
        }
    }

    // Destruction de sémaphore
    sem_destroy(&semaphore);
    // Fermeture du serveur
    close(fdSocketAttente);

    return 0;
}

// Thread qui permet de connecter un client
void *connexion(void *arg)
{
    int boucle = 1;
    Com *structure = (Com *)arg;
    char buffer[256];

    printf("Client connecté. IP : %s\n", inet_ntoa(structure->coordonneesAppelant.sin_addr));

    while (boucle == 1)
    {
        int nbRecu = recv(structure->fdSocketCommunication, buffer, 256, 0);

        // Si le serveur recoit quelque chose du client connecté alors..
        if (nbRecu > 0)
        {
            // ..on reçoit le choix du client en char
            buffer[nbRecu] = 0;
            //Affichage du choix reçu
            printf("\nRecu: %s\n", buffer);
            // Conversion du choix en int
            int choix = atoi(buffer);

            // Appelle la fonction voulue en fonction du choix
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
                // Ecriture d'un message si le client ne souhaite pas se déconnecter afnin de compléter la boucle
                write(structure->fdSocketCommunication, "Vous restez encore un peu avec nous alors :)", strlen("Vous restez encore un peu avec nous alors :)"));
                break;

            default:
                break;
            }
        }
        else
        {
            // On met la boucle à 0 sinon dès qu'un client se déconnecte, il déconnecte tous les autres clients et ferme le serveur
            boucle = 0;
        }
    }

    // Fermeture de la connexion entre le client actuel et le serveur
    close(structure->fdSocketCommunication);
    // Libération de la mémoire allouée à la structure
    free(structure);
    printf("Fin du client\nAttente de connexion\n");
    // Fin du thread actuel
    pthread_exit(NULL);
}

/**
 * @brief Affiche le nombre de places restantes
 * @return void
 */
void affichePlaces(int s)
{

    int nbPlaces = 0;
    char message[100] = "Nombre de places restantes : ";
    char stringPlaces[4];

    // Début de la zone critique à protéger
    sem_wait(&semaphore);
    for (int i = 0; i < 100; i++)
    {
        // On parcourt toues les places et on incrémente nbPlaces dès qu'il y en une de disponible
        if (tablePlaces[i].disponible == 1)
            nbPlaces++;
    }
    // Fin de la zone critique à protéger
    sem_post(&semaphore);

    // Conversion de l'entier nbPlaces en char dans la variable stringPlaces
    sprintf(stringPlaces, "%d", nbPlaces);
    // Concaténation du nombre de places disponibles avec le message correspondant
    strcat(message, stringPlaces);

    write(s, message, strlen(message));
}

/**
 * @brief Décremente le nombre de place
 * @return void
 */
void prendUnePlace(int s)
{
    char message[500] = "Merci pour votre réservation, votre numéro de dossier est : ";
    char nomClient[50];
    char prenomClient[50];
    char num[50] = "";
    char places[4];
    char liste[500] = "";
    char ticket[4];

    // Réception du nom du client
    recv(s, nomClient, 50, 0);
    // Réception du prénom du client
    recv(s, prenomClient, 50, 0);

    // Début de la zone critique à protéger
    sem_wait(&semaphore);
    for (int i = 0; i < 100; i++)
    {
        // Concaténation de chaque place disponible avec la place précédente afin de former une liste visuelle
        if (tablePlaces[i].disponible)
        {
            sprintf(places, "%d", 1 + i);
            strcat(places, " | ");
            strcat(liste, places);
        }
    }
    // Fin de la zone critique à protéger
    sem_post(&semaphore);

    // Envoi de la liste au client
    send(s, liste, strlen(liste), 0);

    // Réception de la place souhaitée par le client
    recv(s, ticket, 4, 0);
    // Conversion du ticket en int avec -1 car notre boucle parcours le tableau de 0 à 99
    int numPlace = atoi(ticket) - 1;
    int x = -1;

    // Début de la zone critique à protéger
    sem_wait(&semaphore);
    // Si la place souhaitée n'est pas disponible on prévient le client
    if (!tablePlaces[numPlace].disponible)
    {
        printf("%s", "cette place est déjà réservée");
        strcpy(message, "Cette place est déjà réservée");
    }
    else
    {
        // Génère un nombre aléatoire à 10 chiffres
        x = 1000000000 + rand() % (9999999999 + 1 - 1000000000);
        // Si le nombre généré est négatif on le multiplie par -1 pour le rendre positif
        if (x < 0)
            x *= -1;

        sprintf(num, "%d", x);

        // On rend la place indisponible car réservée et on attribut les informations du client aux variables de la structure
        tablePlaces[numPlace].disponible = 0;
        strcpy(tablePlaces[numPlace].nom, nomClient);
        strcpy(tablePlaces[numPlace].prenom, prenomClient);
        strcpy(tablePlaces[numPlace].numDossier, num);

        // Affichage de place réservée
        strcat(message, num);
        strcat(message, ". Votre place est la n°");
        strcat(message, ticket);

        printf("La place n° %d a été réservé par %s %s, le numéro de dossier est : %s\n", numPlace + 1, nomClient, prenomClient, num);
    }
    // Fin de la zone critique à protéger
    sem_post(&semaphore);

    // Ecriture du message dans le client en fonction de si la place est disponible ou non
    write(s, message, strlen(message));
}

/**
 * @brief Incrémente le nombre de place
 * @return void
 */
void annulePlace(int s)
{
    char nom[50];
    char num[50];

    // Réception du nom du client
    recv(s, nom, 50, 0);
    // Réception du numéro de dossier du client
    recv(s, num, 50, 0);

    // Booléen indiquant si la place a bien été annulée (1) si non (0)
    int annule = 0;

    // Début de la zone critique à protéger
    sem_wait(&semaphore);
    for (int i = 0; i < 100; i++)
    {
        // Si le nom et le numéro de dossier du client correspondent aux informations d'une place dans le tableau, on annule cette place
        if ((strcmp(tablePlaces[i].nom, nom) == 0) && (strcmp(tablePlaces[i].numDossier, num) == 0))
        {
            tablePlaces[i].disponible = 1;
            write(s, "Place annulée avec succès", 28);
            annule = 1;
            printf("La place n° %d a bien été annulée.\n", i + 1);
            // On break afin de quitter la boucle
            break;
        }
    }

    // Fin de la zone critique à protéger
    sem_post(&semaphore);

    // Si le nom et le numéro de dossier du client ne correspondent à aucune place, on prévient le client
    if (annule == 0)
    {
        write(s, "Ce n° de dossier ou ce nom n'est affecté à aucune place", 59);
    }
}