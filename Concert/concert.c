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