#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "../Concert/concert.c"

#define PORT 6000

void choixAction();
void deconnexion();

int quitter = 1;

int main()
{
    int fdSocket;
    int nbRecu;
    int longueurAdresse;
    char nom[] = "c1";
    char tampon[100];
    struct sockaddr_in coordonneesServeur;

    fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSocket < 0)
    {
        printf("socket incorrecte\n");
        exit(-1);
    }

    // On prépare les coordonnées du serveur
    longueurAdresse = sizeof(struct sockaddr_in);
    memset(&coordonneesServeur, 0x00, longueurAdresse);
    coordonneesServeur.sin_family = PF_INET;
    // adresse du serveur
    inet_aton("127.0.0.1", &coordonneesServeur.sin_addr);

    // toutes les interfaces locales disponibles
    coordonneesServeur.sin_port = htons(PORT);
    if (connect(fdSocket, (struct sockaddr *)&coordonneesServeur, sizeof(coordonneesServeur)) == -1)
    {
        printf("connexion impossible\n");
        exit(-1);
    }
    printf("connexion ok\n");
    send(fdSocket, nom, strlen(nom), 0);
    printf("Client: %s\n", nom);
    printf("\n-----------Bienvenue-----------\n");

    while (quitter != 0)
    {
        /*
        fgets(tampon, 100, stdin);
        send(fdSocket, tampon, strlen(tampon), 0);
        nbRecu = recv(fdSocket, tampon, 99, 0);
        if (nbRecu > 0)
        {
            tampon[nbRecu] = 0;
            printf("Recu:%s\n", tampon);
        }
        */
        choixAction();
    }

    close(fdSocket);
}

/**
 * @brief Affiche ce que le client peut faire
 * @return void
 */
void choixAction()
{
    int choix = 0;

    sleep(1); //Attends une seconde avant que le menu ne s'affichent afin de ne pas embrouiller l'utilisateur

    printf("\nQue voulez-vous faire ?\n");
    printf("Consulter les places disponibles (1)\n");
    printf("Réserver une place (2)\n");
    printf("Annuler une place (3)\n");
    printf("Quitter (4)\n");

    while (choix < 1 || choix > 4)
    {
        scanf("%d", &choix);
        switch (choix)
        {
        case 1:
            affichePlace();
            break;

        case 2:
            prendUnePlace();
            break;

        case 3:
            annuleUnePlace();
            break;

        case 4:
            deconnexion();
            break;

        default:
            printf("Veuillez choisir un chiffre entre 1 et 4\n");
            break;
        }
    }
}

/**
 * @brief Déconnecte le client du serveur
 * @return void
 */
void deconnexion()
{
    int choix;
    printf("Voulez-vous vraiment vous déconnecter ?\nOui (1) || Non (0)\n");
    while (choix < 0 || choix > 1)
    {
        scanf("%d", &choix);
        if (choix < 0 || choix > 1)
            printf("Veuillez entrer un chiffre entre 0 et 1\n");
    }
    if (choix == 1)
    {
        quitter = 0;
        printf("Déconnexion\n");
    }
    else
        printf("Vous restez encore un peu avec nous alors :)\n");
}