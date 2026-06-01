#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define SECTOR_SIZE 512

unsigned char * lire_secteur(char * disque_name, int num_sect) {
    
    /* 1. Ouvrir le disque en lecture binaire */
    FILE *disk = fopen(disque_name, "rb");
    if (disk == NULL) {
        perror("Erreur : impossible d'ouvrir le disque");
        return NULL;
    }

    /* 2. Placer le curseur au début du secteur souhaité */
    // SEEK_SET : commencer depuis le début du disque
    // num_sect * SECTOR_SIZE : avancer de l'offset calculé
    if (fseek(disk, num_sect * SECTOR_SIZE, SEEK_SET) != 0) {
        perror("Erreur seek secteur");
        fclose(disk);
        return NULL;
    }

    /* 3. Allouer un buffer de 512 octets (taille d'un secteur) */
    unsigned char *buffer = malloc(sizeof(unsigned char) * SECTOR_SIZE);

    /* 4. Lire 512 blocs de 1 octet depuis le disque vers le buffer */
    if (fread(buffer, 1, SECTOR_SIZE, disk) != SECTOR_SIZE) {
        perror("Erreur lecture secteur");
        fclose(disk);
        return NULL;
    }

    /* 5. Fermer le disque */
    fclose(disk);

    return buffer;
}
