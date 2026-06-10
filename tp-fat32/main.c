#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fat32.c"

int main(void)
{
    FILE *disque;

    /*
     * adrPartition est l'adresse LBA du debut de la partition FAT32.
     * On peut la trouver avant avec le programme MBR ou GPT.
     */
    unsigned int adrPartition;

    char nomDisque[50];
    char chemin[100] = "/dev/";

    printf("Entrer le nom du disque physique (ex: sdb): ");
    scanf("%s", nomDisque);

    printf("Entrer l'adresse LBA de debut de la partition FAT32: ");
    scanf("%u", &adrPartition);

    /* Exemple : si nomDisque = "sdb", chemin devient "/dev/sdb". */
    disque = fopen(strcat(chemin, nomDisque), "rb"); /* disque physique */
    if (disque == NULL) {
        printf("\n Erreur : le disque n'est pas ouvert\n");
        exit(0);
    }

    /*
     * 1) Lire le boot sector FAT32.
     * Le boot sector est le premier secteur de la partition FAT32,
     * donc il se trouve a l'adresse adrPartition.
     */
    lire_boot_fat32(disque, adrPartition);

    /*
     * 2) Afficher les champs importants du boot sector :
     * secteurs reserves, nombre de FAT, taille d'une FAT, cluster racine...
     */
    afficher_infos_boot(adrPartition);

    /*
     * 3) Avec les informations du boot sector, calculer l'adresse du
     * repertoire racine puis afficher ses entrees de 32 octets.
     */
    afficher_repertoire_racine(disque, adrPartition);

    printf("\n======================================================= Fin =======================================================\n");

    fclose(disque);
    return 0;
}
