#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "lire-secteur.c"

#define SECTOR_SIZE 512

void afficher_secteur(char * disque_name, int num_sect) {

    /* 1. Lire le secteur depuis le disque */
    unsigned char *buffer; // On pouvait l'ecrire uint8_t * buffer
    buffer = lire_secteur(disque_name, num_sect);

    printf("Contenu du secteur %d de %s :\n", num_sect, disque_name);

    /* 2. Parcourir les 512 octets par blocs de 16 (32 lignes) */
    for (int i = 0; i < SECTOR_SIZE; i += 16) {
        printf("%04d      ", i);              // numero de la ligne (offset)
        for (int j = 0; j < 16; j++)
            printf("%02x ", buffer[i + j]);   // octet en hexa sur 2 chiffres
        printf("\n");
    }

    /* 3. Liberer le buffer */
    free(buffer);

}
