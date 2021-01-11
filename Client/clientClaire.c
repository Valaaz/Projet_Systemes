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
void deconnexion(int s);
void viderBuffer();
int readC(char *chaine, int longueur);

int quitter = 1;

int main()
{
    int fdSocket;
    int longueurAdresse;
    char choix[10];
    int res;
    char tampon[100];
    char buffer[500];
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
            deconnexion(fdSocket);
            break;

        default:
            printf("Veuillez choisir un chiffre entre 1 et 4\n");
            break;
        }

        int nbRecu = recv(fdSocket, buffer, 500, 0);

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
    char place[4];
    char listePlace[500] = "";

    printf("Saisissez votre nom : \n");
    scanf("%s", nom);
    printf("nom : %s\n", nom);
    send(s, nom, strlen(nom) + 1, 0);

    printf("Saisissez votre prénom : \n");
    scanf("%s", prenom);

    printf("prenom : %s\n", prenom);
    send(s, prenom, strlen(prenom) + 1, 0);

    recv(s, listePlace, 500, 0);
    printf("\n%s\n\n", listePlace);

    printf("Saisissez la place désirée : \n");
    scanf("%s", place);
    send(s, place, strlen(place) + 1, 0);
}

void annulePlace(int s)
{
    char num[50];
    char nom[50];
    int numDossier;
    printf("Merci de saisir votre nom et votre n° de dossier: \n");
    printf("Votre nom :");
    scanf("%s", nom);
    send(s, nom, strlen(nom) + 1, 0);
    printf("Votre n° de dossier:");
    scanf("%s", num);
    send(s, num, strlen(num) + 1, 0);
}

/**
 * @brief Déconnecte le client du serveur
 * @return void
 */
void deconnexion(int s)
{
    char choix[10];
    int res;
    printf("Voulez-vous vraiment vous déconnecter ?\nOui (1) || Non (0)\n");
    do
    {
        readC(choix, 10);
        res = atoi(choix);
        if (res < 0 || res > 1)
            printf("Veuillez choisir un chiffre entre 1 et 4\n");
    } while (res < 0 || res > 1);
    if (res == 1)
    {
        quitter = 0;
        printf("Déconnexion\n");
        close(s);
    }
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