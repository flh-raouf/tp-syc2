#include <stdio.h>
#include <stdlib.h>

#define TAILLE_GO 1024 * 1024 * 1024

struct { /* structure du MBR */
    unsigned char prog[446];

    struct partition {
        unsigned char etat;      /* etat de la partition */
        unsigned char chs1[3];
        unsigned char type;      /* type de la partition */
        unsigned char chs2[3];
        unsigned char adrlba[4]; /* nombre secteurs avant la partition */
        unsigned char taille[4]; /* taille de la partition */
    } tp[4];

    unsigned char aa55[2];
} mbr, EBR;

void Partition_etendue(long int adrlba, int *j, FILE *disque)
{
    unsigned char etat;   /* etat de la partition */
    unsigned char fs;     /* type du systeme de fichiers */

    unsigned int adr;     /* nombre de secteurs qui precedent la partition : adresse LBA */
    unsigned int taille;  /* nombre de secteurs de la partition */

    int n1, err;
    double y, t;

    /* Se positionner sur le debut de la partition etendue */
    err = fseek(disque, adrlba * 512, SEEK_SET);
    if (err != 0) {
        printf("\n Erreur : %d Le deplacement n'a pas ete effectue sur le disque\n", err);
        exit(0);
    }

    n1 = fread(&EBR, 512, 1, disque);
    if (n1 <= 0) {
        printf("\n erreur de lecture n = %d", n1);
        exit(0);
    }

    adr = *(int *) &(EBR.tp[0].adrlba);
    taille = *(int *) &(EBR.tp[0].taille);
    etat = EBR.tp[0].etat;
    fs = EBR.tp[0].type;
    y = (double) taille * 512;
    t = y / TAILLE_GO;

    printf("\n| sda%d | x'%02x' | x'%02x' | %9d | %9d | %8.3f |",
           *j, etat, fs, adr + adrlba, taille, t);

    *j = *j + 1;

    if (EBR.tp[1].type == 0x05) {
        adr = *(int *) &(EBR.tp[1].adrlba);
        Partition_etendue(adr + adrlba, j, disque);
    }
}
