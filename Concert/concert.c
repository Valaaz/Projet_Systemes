#include <stdio.h>

int places = 100;

/**
 * @brief Affiche le nombre de place
 * @return void
 */
void affichePlace()
{
    printf("Il reste %d places\n", places);
}

/**
 * @brief Décremente le nombre de place
 * @return void
 */
void prendUnePlace()
{
    if (places == 0)
        printf("Désolé, il n'y a plus de places disponibles\n");
    else
    {
        places -= 1;
        affichePlace();
    }
}

/**
 * @brief Incrémente le nombre de place
 * @return void
 */
void annuleUnePlace()
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
        places += 1;
        printf("Place annulée\n");
        affichePlace();
    }
    else
        printf("Vous gardez votre place\n");
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

        default:
            printf("Veuillez choisir un chiffre entre 1 et 4\n");
            break;
        }
    }
}