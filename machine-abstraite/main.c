#include <stdio.h>
#include "./functions/afficher-secteur.c"
#include "./functions/list-disques.c"

int main() {
    liste_disques();
    afficher_secteur("/dev/sda", 0);
    return 0;
}
