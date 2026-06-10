#include <stdio.h>
#include <stdlib.h>

#define TAILLE_SECTEUR 512
#define TAILLE_KO 1024

/*
 * Le secteur boot FAT32 est le premier secteur de la partition FAT32.
 * Il contient les informations necessaires pour trouver :
 * - la FAT,
 * - la zone des clusters,
 * - le repertoire racine.
 *
 * Les tableaux unsigned char servent a lire les champs exactement comme
 * ils sont stockes sur le disque. Les valeurs de 2 ou 4 octets sont en
 * little endian.
 */
struct { /* structure du secteur boot FAT32 */
    unsigned char saut[3];
    unsigned char fabricant[8];

    unsigned char nbOctetsSecteur[2];   /* position 11 */
    unsigned char nbSecteursCluster;    /* position 13 */
    unsigned char nbSecteursReserves[2];/* position 14 */
    unsigned char nbFat;                /* position 16 */
    unsigned char nbEntreesRacine[2];
    unsigned char nbSecteursPetit[2];
    unsigned char media;
    unsigned char nbSecteursFat16[2];
    unsigned char nbSecteursPiste[2];
    unsigned char nbTetes[2];
    unsigned char nbSecteursAvant[4];
    unsigned char nbSecteursPartition[4];

    unsigned char nbSecteursFat32[4];   /* position 36 */
    unsigned char flags[2];
    unsigned char version[2];
    unsigned char premierClusterRacine[4]; /* position 44 */
    unsigned char secteurFsInfo[2];
    unsigned char secteurBootCopie[2];
    unsigned char reserve[12];
    unsigned char numeroLecteur;
    unsigned char reserveNT;
    unsigned char signatureEtendue;
    unsigned char numeroSerie[4];
    unsigned char nomVolume[11];
    unsigned char typeSysteme[8];

    unsigned char programme[420];
    unsigned char aa55[2];
} boot;

/*
 * Une entree de repertoire FAT32 occupe toujours 32 octets.
 * Elle peut decrire un fichier, un dossier, un nom long, une entree vide...
 */
struct entreeRepertoire { /* entree de repertoire FAT32 : 32 octets */
    unsigned char nom[8];
    unsigned char extension[3];
    unsigned char attribut;
    unsigned char reserveNT;
    unsigned char millisecondes;
    unsigned char heureCreation[2];
    unsigned char dateCreation[2];
    unsigned char dateDernierAcces[2];
    unsigned char premierClusterHaut[2];
    unsigned char heureModification[2];
    unsigned char dateModification[2];
    unsigned char premierClusterBas[2];
    unsigned char tailleFichier[4];
};

unsigned int valeur2(unsigned char champ[2])
{
    /* Convertir un champ little endian de 2 octets en entier. */
    return *(unsigned short int *) champ;
}

unsigned int valeur4(unsigned char champ[4])
{
    /* Convertir un champ little endian de 4 octets en entier. */
    return *(unsigned int *) champ;
}

void lire_boot_fat32(FILE *disque, unsigned int adrPartition)
{
    int n1, err;

    /*
     * Aller au debut de la partition FAT32.
     * fseek travaille en octets, donc on multiplie le LBA par 512.
     */
    err = fseek(disque, adrPartition * TAILLE_SECTEUR, SEEK_SET);
    if (err != 0) {
        printf("\n Erreur : le deplacement vers le boot sector a echoue\n");
        exit(0);
    }

    /* Lire les 512 octets du boot sector dans la variable globale boot. */
    n1 = fread(&boot, TAILLE_SECTEUR, 1, disque);
    if (n1 <= 0) {
        printf("\n erreur de lecture n = %d\n", n1);
        exit(0);
    }
}

void afficher_infos_boot(unsigned int adrPartition)
{
    unsigned int nbOctetsSecteur;
    unsigned int nbSecteursCluster;
    unsigned int nbSecteursReserves;
    unsigned int nbFat;
    unsigned int nbSecteursFat;
    unsigned int premierClusterRacine;
    unsigned int tailleCluster;
    unsigned int adrFat;
    unsigned int adrPremierCluster;
    unsigned int adrRacine;

    /* Recuperer les champs importants depuis le boot sector. */
    nbOctetsSecteur = valeur2(boot.nbOctetsSecteur);          /* offset 11 */
    nbSecteursCluster = boot.nbSecteursCluster;               /* offset 13 */
    nbSecteursReserves = valeur2(boot.nbSecteursReserves);    /* offset 14 */
    nbFat = boot.nbFat;                                       /* offset 16 */
    nbSecteursFat = valeur4(boot.nbSecteursFat32);            /* offset 36 */
    premierClusterRacine = valeur4(boot.premierClusterRacine);/* offset 44 */

    /* Taille d'un cluster en octets = octets/secteur * secteurs/cluster. */
    tailleCluster = nbOctetsSecteur * nbSecteursCluster;

    /*
     * Formules FAT32 importantes :
     * FAT1 commence apres la zone reservee.
     * La zone des clusters commence apres toutes les FAT.
     * Le repertoire racine est generalement le cluster 2.
     */
    adrFat = adrPartition + nbSecteursReserves;
    adrPremierCluster = adrPartition + nbSecteursReserves + nbFat * nbSecteursFat;
    adrRacine = adrPremierCluster + (premierClusterRacine - 2) * nbSecteursCluster;

    printf("\n\n\t************ Informations FAT32 ************\n");
    printf("\n| Information | Valeur |");
    printf("\n|-------------|--------|");
    printf("\n| Adresse LBA partition | %9u |", adrPartition);
    printf("\n| Octets par secteur | %9u |", nbOctetsSecteur);
    printf("\n| Secteurs par cluster | %9u |", nbSecteursCluster);
    printf("\n| Taille cluster en Ko | %9.3f |", (double) tailleCluster / TAILLE_KO);
    printf("\n| Secteurs reserves | %9u |", nbSecteursReserves);
    printf("\n| Nombre de FAT | %9u |", nbFat);
    printf("\n| Secteurs par FAT | %9u |", nbSecteursFat);
    printf("\n| Cluster racine | %9u |", premierClusterRacine);
    printf("\n| Adresse LBA FAT1 | %9u |", adrFat);
    printf("\n| Adresse LBA 1er cluster | %9u |", adrPremierCluster);
    printf("\n| Adresse LBA racine | %9u |", adrRacine);
    printf("\n| Signature | x'%02x%02x' |", boot.aa55[1], boot.aa55[0]);
}

void afficher_repertoire_racine(FILE *disque, unsigned int adrPartition)
{
    unsigned int nbSecteursCluster;
    unsigned int nbSecteursReserves;
    unsigned int nbFat;
    unsigned int nbSecteursFat;
    unsigned int premierClusterRacine;
    unsigned int adrPremierCluster;
    unsigned int adrRacine;
    unsigned int tailleCluster;
    unsigned int tailleFichier;
    unsigned int clusterHaut, clusterBas, premierCluster;

    unsigned int i;
    int n1, err;
    unsigned char *cluster;
    struct entreeRepertoire *entree;

    /* Reprendre les memes champs pour calculer l'adresse du repertoire racine. */
    nbSecteursCluster = boot.nbSecteursCluster;
    nbSecteursReserves = valeur2(boot.nbSecteursReserves);
    nbFat = boot.nbFat;
    nbSecteursFat = valeur4(boot.nbSecteursFat32);
    premierClusterRacine = valeur4(boot.premierClusterRacine);

    /*
     * LBA du cluster i =
     * adresse du premier cluster + (i - 2) * secteurs par cluster.
     */
    adrPremierCluster = adrPartition + nbSecteursReserves + nbFat * nbSecteursFat;
    adrRacine = adrPremierCluster + (premierClusterRacine - 2) * nbSecteursCluster;
    tailleCluster = nbSecteursCluster * TAILLE_SECTEUR;

    /* On reserve une zone memoire pour lire tout le cluster racine. */
    cluster = malloc(tailleCluster);
    if (cluster == NULL) {
        printf("\n Erreur : allocation impossible\n");
        exit(0);
    }

    /* Se placer au debut du cluster du repertoire racine. */
    err = fseek(disque, adrRacine * TAILLE_SECTEUR, SEEK_SET);
    if (err != 0) {
        printf("\n Erreur : le deplacement vers le repertoire racine a echoue\n");
        exit(0);
    }

    /* Lire tout le cluster racine. */
    n1 = fread(cluster, tailleCluster, 1, disque);
    if (n1 <= 0) {
        printf("\n erreur de lecture n = %d\n", n1);
        exit(0);
    }

    printf("\n\n\t************ Premier cluster du repertoire racine ************\n");
    printf("\n| Nom court | Attr | Premier cluster | Taille octets |");
    printf("\n|-----------|------|-----------------|---------------|");

    /*
     * Chaque entree de repertoire fait 32 octets.
     * On avance donc de 32 en 32 dans le cluster.
     */
    for (i = 0; i < tailleCluster; i += 32) {
        entree = (struct entreeRepertoire *) (cluster + i);

        /* 0x00 : il n'y a plus d'entrees apres celle-ci. */
        if (entree->nom[0] == 0x00) {
            break;
        }

        /* 0xE5 : entree supprimee logiquement. */
        if (entree->nom[0] == 0xE5) {
            continue;
        }

        /* 0x0F : entree de nom long, on la saute pour garder un affichage simple. */
        if (entree->attribut == 0x0F) {
            continue;
        }

        /*
         * En FAT32, le numero du premier cluster est coupe en deux :
         * - poids fort a l'offset 20,
         * - poids faible a l'offset 26.
         */
        clusterHaut = valeur2(entree->premierClusterHaut);
        clusterBas = valeur2(entree->premierClusterBas);
        premierCluster = clusterHaut * 65536 + clusterBas;
        tailleFichier = valeur4(entree->tailleFichier);

        printf("\n| %.8s.%.3s | x'%02x' | %15u | %13u |",
               entree->nom,
               entree->extension,
               entree->attribut,
               premierCluster,
               tailleFichier);
    }

    free(cluster);
}
