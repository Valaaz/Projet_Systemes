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

void *connexion(void *arg);

int main()
{
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
    //pthread_mutex_lock(&mutex);

    Com *structure = (Com *)arg;
    char tampon[100];
    char nom[10];
    char nbPlaces[4];

    printf("Client connecté. IP : %s\n", inet_ntoa(structure->coordonneesAppelant.sin_addr));
    recv(structure->fdSocketCommunication, nom, 10, 0);

    printf("Client : %s\n", nom);

    while (1)
    {
        send(structure->fdSocketCommunication, place, 100, 0);
        printf("Envoyé : %s\n", place);

        int nbRecu = recv(structure->fdSocketCommunication, nbPlaces, 100, 0);
        //recv(structure->fdSocketCommunication, tampon, 99, 0);

        if (nbRecu > 0)
        {
            nbPlaces[nbRecu] = 0;
            int x = atoi(nbPlaces);

            printf("Recu: %s\n", nbPlaces);
            sprintf(nbPlaces, "%d", x);
            strncpy(place, nbPlaces, 3);
        }

        //fgets(tampon, 100, stdin);
        //send(structure->fdSocketCommunication, tampon, strlen(tampon), 0);

        /*
        int choix = 2;
        printf("Choix :\n");
        scanf("%d", &choix);
        if (choix == 1)
        {
            //printf("Adresse : %ls\n", pointeur);
            //printf("Il reste %d places\n", *pointeur);
            printf("Il reste %s places\n", nbPlaces);
        }
        */
    }

    close(structure->fdSocketCommunication);
    printf("Fin du client n°%s\nAttente de connexion\n", nom);

    //pthread_mutex_unlock(&mutex);
}