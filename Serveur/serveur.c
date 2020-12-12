#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 6000
int main()
{
    int fdSocketAttente;
    int fdSocketCommunication;
    struct sockaddr_in coordonneesServeur;
    struct sockaddr_in coordonneesAppelant;
    char tampon[100];
    int nbRecu;
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

    socklen_t tailleCoord = sizeof(coordonneesAppelant);
    while (1)
    {
        if ((fdSocketCommunication = accept(fdSocketAttente, (struct sockaddr *)&coordonneesAppelant,
                                            &tailleCoord)) == -1)
        {
            printf("erreur de accept\n");
            exit(-1);
        }

        printf("Client connecté. IP : %s\n", inet_ntoa(coordonneesAppelant.sin_addr));
        nbRecu = recv(fdSocketCommunication, tampon, 99, 0);
        if (nbRecu > 0)
        {
            tampon[nbRecu] = 0;
            printf("Recu:%s\n", tampon);
        }

        fgets(tampon, 100, stdin);
        send(fdSocketCommunication, tampon, strlen(tampon), 0);

        close(fdSocketCommunication);
        printf("Attente de connexion\n");
    }

    close(fdSocketAttente);

    return 0;
}