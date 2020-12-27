#include <stdio.h>

int places = 100;

/**
 * @brief Décremente le nombre de place
 * @return void
 */
void prendUnePlace()
{
    places -= 1;
}

/**
 * @brief Incrémente le nombre de place
 * @return void
 */
void annuleUnePlace()
{
    places += 1;
}

/**
 * @brief Affiche le nombre de place
 * @return void
 */
void affichePlace()
{
    printf("Il reste %d places", places);
}

/**
 * @brief Affiche ce que le client peut faire
 * @return void
 */
void choixAction(char nom[])
{
    int choix = 0;

    printf("-----------Bienvenue-----------\n");
    printf("Client: %s\n", nom);
    printf("Menu :\n");
    printf("Que voulez-vous faire ?\n");
    printf("Consulter les places disponibles (1)\n");
    printf("Réserver une place (2)\n");
    printf("Annuler une place (3)\n");

    while (choix < 1 || choix > 3)
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

        default:
            printf("Veuillez choisir un chiffre entre 1 et 3");
            break;
        }
    }
}