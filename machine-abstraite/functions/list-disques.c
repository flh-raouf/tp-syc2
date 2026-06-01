#include <stdio.h>
#include <dirent.h>
#include <string.h>

void liste_disques() {
    printf("Liste des disques physiques disponibles :\n");

    /* 1. Ouvrir le dossier /dev */
    DIR *dir = opendir("/dev");
    
    if (dir == NULL) {
        perror("Erreur : impossible d'ouvrir /dev");
        return;
    }

    /* 2. Parcourir toutes les entrées du dossier */
    struct dirent *entry;    // pointeur vers une entrée du répertoire

    while ((entry = readdir(dir)) != NULL) {
        // Comparer les 2 premiers caractères du nom avec "sd"
        // strncmp == 0 signifie que les chaînes sont égales
        if (strncmp(entry->d_name, "sd", 2) == 0) {
            // if(strlen(entry->d_name)==3){
            printf("/dev/%s\n", entry->d_name);
            // }
        }
    }

    /* 3. Fermer le dossier */
    closedir(dir);
    
}
