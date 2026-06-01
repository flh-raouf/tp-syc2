#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define TAILLE_SECTEUR 512
#define TAILLE_GO 1024 * 1024 * 1024

struct { /* structure de l'entete GPT */
    unsigned char signature[8];
    unsigned char version[4];
    unsigned char tailleEntete[4];
    unsigned char CRC32Entete[4];
    unsigned char reserve[4];
    unsigned char adrEntete[8];
    unsigned char adrCopieEntete[8];
    unsigned char adrDebutZdd[8]; /* debut de la zone de donnees */
    unsigned char adrFinZdd[8];   /* fin de la zone de donnees */
    unsigned char GUID[16];
    unsigned char adrTP[8];       /* adresse LBA de la table des partitions */
    unsigned char nbEntreeTP[4];
    unsigned char tailleEntreeTP[4];
    unsigned char CRC32TP[4];
    unsigned char reserve2[420];
} EnteteGPT;

struct partitionGPT {
    unsigned char typeGUID[16];
    unsigned char uniqueGUID[16];
    unsigned char adrDebutPart[8]; /* premiere adresse LBA */
    unsigned char adrFinPart[8];   /* derniere adresse LBA */
    unsigned char attributs[8];
    unsigned char nomPart[72];
} TP[128];

int entree_vide(struct partitionGPT p)
{
    int i;

    for (i = 0; i < 16; i++) {
        if (p.typeGUID[i] != 0) {
            return 0;
        }
    }

    return 1;
}
