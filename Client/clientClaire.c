#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 6000

void prendUnePlace(int s);
void annulePlace(int s);
void deconnexion();
void viderBuffer();
int readC(char *chaine, int longueur);

int quitter = 1;

int main()
{
    int fdSocket;
    int longueurAdresse;
    char choix[10];
    int res;
    //char nom[] = "c1";
    char tampon[100];
    char buffer[256];
    char integer[4];
    char nom[256];

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
        printf("\nQue voulez-vous faire ?\nConsulter les places disponibles (1)\nRéserver une place (2)\nAnnuler une place(3)\nQuitter (4)\n");

        printf("Choix : ");
        do
        {
            readC(choix, 10);
            res = atoi(choix);
            if (res < 1 || res > 4)
                printf("Veuillez choisir un chiffre entre 1 et 4\n");
        } while (res < 1 || res > 4);
        sprintf(tampon, "%d", res);
        send(fdSocket, tampon, strlen(tampon), 0);

        switch (res)
        {
        case 1:
            break;
        case 2:
            prendUnePlace(fdSocket);
            break;

        case 3:
            annulePlace(fdSocket);
            break;

        case 4:
            //deconnexion();
            break;

        default:
            printf("Veuillez choisir un chiffre entre 1 et 4\n");
            break;
        }

        int nbRecu = recv(fdSocket, buffer, 256, 0);

        if (nbRecu > 0)
        {
            buffer[nbRecu] = 0;
            printf("%s\n", buffer);
            fflush(stdout);
        }
    }

    close(fdSocket);
}

/**
 * @brief Décremente le nombre de place
 * @return void
 */
void prendUnePlace(int s)
{
    char nom[50];
    char prenom[50];

    printf("Saississez votre nom : \n");
    scanf("%s", nom);
    printf("nom : %s\n", nom);
    send(s, nom, strlen(nom) + 1, 0);
    printf("Saississez votre prénom : \n");
    scanf("%s", prenom);
    printf("prenom : %s\n", prenom);
    send(s, prenom, strlen(prenom) + 1, 0);
}

void annulePlace(int s)
{
    char num[50];
    int numDossier;
    printf("Saississez votre nom : \n");
    scanf("%s", num);
    send(s, num, strlen(num) + 1, 0);
}

// Les 2 fonctions suivantes ont été repris du projet C du premier semestre 2019
/**
 * @brief Vide le buffer
 * @return void
 */
void viderBuffer()
{
    int c = 0;
    while (c != '\n' && c != EOF)
        c = getchar();
}

/**
 * @brief Permet de lire une string
 * @return 1 ou EXIT_FAILURE
 */
int readC(char *chaine, int longueur)
{
    char *positionEntree = NULL;

    if (fgets(chaine, longueur, stdin) != NULL)
    {
        positionEntree = strchr(chaine, '\n');
        if (positionEntree != NULL)
            *positionEntree = '\0';
        else
            viderBuffer();
        return 1;
    }
    else
    {
        viderBuffer();
        return EXIT_FAILURE;
    }
}

/**
 * @brief Déconnecte le client du serveur
 * @return void
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
 */

/**
 * @brief Incrémente le nombre de place
 * @return void
void annuleUnePlace(int *places)
{
    int choix;
    printf("Voulez-vous vraiment annuler votre place ?\nOui (1) || Non (0)\n");
    while (choix < 0 || choix > 1)
    {
        scanf("%d", &choix);
        if (choix < 0 || choix > 1)
            printf("Veuillez entrer un chiffre entre 0 et 1\n");
    }
    if (choix == 1)
    {
        *places += 1;
        printf("Place annulée\n");
        affichePlace(places);
    }
    else
        printf("Vous gardez votre place\n");
}
 */
