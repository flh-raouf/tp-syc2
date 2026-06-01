#include <stdio.h>
#include <stdlib.h>

#include "partition-etendue.c"

#define TAILLE_GO 1024 * 1024 * 1024


int main(void)
{
    unsigned char etat;   /* etat de la partition */
    unsigned char fs;     /* type du systeme de fichiers */

    unsigned int adr;     /* nombre de secteurs qui precedent la partition : adresse LBA */
    unsigned int taille;  /* nombre de secteurs de la partition */

    int n1;
    int j;
    double x, taille_octets, taille_go;

    FILE *disque;

    disque = fopen("/dev/sdb", "rb"); /* disque physique */
    if (disque == NULL) {
        printf("\n Erreur : le disque n'est pas ouvert\n");
        exit(0);
    }

    /* Lecture du MBR du flashdisk */
    n1 = fread(&mbr, 512, 1, disque);
    if (n1 <= 0) {
        printf("\n erreur de lecture n = %d\n", n1);
        exit(0);
    }

    printf("\n\n\t************ Table de Partition du disque ************\n");
    printf("\n| N° Partition | Etat | Type SF | Adresse LBA | Nbres de secteurs | Taille en GO |");
    printf("\n|--------------|------|---------|-------------|-------------------|--------------|");

    /* Conversion adresse et taille de la table des partitions */
    j = 1;

    for (int i = 0; i < 4; i++) {
        adr = *(int *) &(mbr.tp[i].adrlba);
        taille = *(int *) &(mbr.tp[i].taille);
        etat = mbr.tp[i].etat;
        fs = mbr.tp[i].type;
        taille_octets = (double) taille * 512;
        taille_go = taille_octets / TAILLE_GO;

        printf("\n| sda%d | x'%02x' | x'%02x' | %9d | %9d | %8.3f |",
               j, etat, fs, adr, taille, taille_go);

        j++;

        if (fs == 0x05) {
            Partition_etendue(adr, &j, disque);
        }
    }

    printf("\n======================================================= Fin =======================================================\n");

    fclose(disque);
    return 0;
}
