#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gpt.c"

int main(void)
{
    unsigned long int adrTP;
    unsigned long int adrDebutPart;
    unsigned long int adrFinPart;
    unsigned long int taille;

    int nbEntreeTP;

    int n1, j;
    double taille_octets, taille_go;

    FILE *disque;

    disque = fopen("/dev/sda", "rb"); /* disque physique */
    if (disque == NULL) {
        printf("\n Erreur : le disque n'est pas ouvert\n");
        exit(0);
    }

    /* Lecture de l'entete GPT : il se trouve dans le secteur LBA 1 */
    if (fseek(disque, 1 * TAILLE_SECTEUR, SEEK_SET) != 0) {
        printf("\n Erreur : le deplacement vers l'entete GPT a echoue\n");
        exit(0);
    }

    n1 = fread(&EnteteGPT, TAILLE_SECTEUR, 1, disque);
    if (n1 <= 0) {
        printf("\n erreur de lecture n = %d\n", n1);
        exit(0);
    }

    adrTP = *(unsigned long int *) &(EnteteGPT.adrTP);
    nbEntreeTP = *(int *) &(EnteteGPT.nbEntreeTP);



    /* Lecture de la table des partitions GPT */
    if (fseek(disque, adrTP * TAILLE_SECTEUR, SEEK_SET) != 0) {
        printf("\n Erreur : le deplacement vers la table GPT a echoue\n");
        exit(0);
    }

    n1 = fread(&TP, sizeof(struct partitionGPT), 128, disque);
    if (n1 <= 0) {
        printf("\n erreur de lecture n = %d\n", n1);
        exit(0);
    }

    printf("\n\n\t************ Table des partitions du disque ************\n");
    printf("\n| Partition | LBA_debut |  LBA_fin  | Nbre_secteurs | Taille en GO |");
    printf("\n|-----------|-----------|-----------|---------------|--------------|");

    j = 1;

    for (int i = 0; i < 128 && i < nbEntreeTP; i++) {
        if (entree_vide(TP[i])) {
            break;
        }

        adrDebutPart = *(unsigned long int *) &(TP[i].adrDebutPart);
        adrFinPart = *(unsigned long int *) &(TP[i].adrFinPart);
        taille = adrFinPart - adrDebutPart + 1;

        taille_octets = (double) taille * TAILLE_SECTEUR;
        taille_go = taille_octets / TAILLE_GO;

        printf("\n| %s%d | %9lu | %9lu | %13lu | %12.3f |",
               "sda", j, adrDebutPart, adrFinPart, taille, taille_go);

        j++;
    }

    printf("\n======================================================= Fin =======================================================\n");

    fclose(disque);
    return 0;
}
