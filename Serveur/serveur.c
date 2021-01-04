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
void choixAction();

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
    int boucle = 1;
    Com *structure = (Com *)arg;
    char tampon[100];
    char nom[10];
    char nbPlaces[4];

    printf("Client connecté. IP : %s\n", inet_ntoa(structure->coordonneesAppelant.sin_addr));
    recv(structure->fdSocketCommunication, nom, 10, 0);

    printf("Client : %s\n", nom);

    while (boucle == 1)
    {
        write(structure->fdSocketCommunication, "Début", 7);

        //choixAction(structure->fdSocketCommunication);

        int nbRecu = recv(structure->fdSocketCommunication, tampon, 100, 0);
        //recv(structure->fdSocketCommunication, tampon, 99, 0);

        if (nbRecu > 0)
        {
            tampon[nbRecu] = 0;
            printf("Recu: %s\n", tampon);
        }
        else
        {
            boucle = 0;
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

/**
 * @brief Affiche ce que le client peut faire
 * @return void
 */
void choixAction(int s)
{
    int choix = 0;

    sleep(1); //Attends une seconde avant que le menu ne s'affichent afin de ne pas embrouiller l'utilisateur

    write(s, "\nQue voulez-vous faire ?", 100);
    write(s, "Consulter les places disponibles (1)", 100);
    write(s, "Réserver une place (2)", 100);
    write(s, "Annuler une place (3)", 100);
    write(s, "Quitter (4)\n", 100);

    while (choix < 1 || choix > 4)
    {
        scanf("%d", &choix);
        switch (choix)
        {
        case 1:
            //affichePlace(&x);
            write(s, "Cas 1", 6);
            break;

        case 2:
            //prendUnePlace(&x);
            write(s, "Cas 2", 6);
            break;

        case 3:
            //annuleUnePlace(&x);
            write(s, "Cas 3", 6);
            break;

        case 4:
            //deconnexion();
            break;

        default:
            printf("Veuillez choisir un chiffre entre 1 et 4\n");
            break;
        }
    }
}