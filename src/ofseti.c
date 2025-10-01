#include "../include/ofseti.h"

#include "../include/boje.h"

void upisi_ofsete(ofesti_t ofseti) {

    FILE* f = fopen( "../pomocne_dat/ofesti.bin", "wb+");
    if ( f == NULL ) {
        printf(ANSI_COLOR_RED "GRESKA: datoteka sa ofsetima nije uspesno otvorena" ANSI_COLOR_RESET);
    }

    fseek(f, 0, SEEK_SET);
    fwrite(&ofseti, sizeof(ofesti_t), 1, f);
    fflush(f);
}

ofesti_t procitaj_ofsete() {
    FILE* f = fopen( "../pomocne_dat/ofesti.bin", "rb");
    if ( f == NULL ) {
        printf(ANSI_COLOR_RED "GRESKA: datoteka sa ofsetima nije uspesno otvorena" ANSI_COLOR_RESET);
    }

    ofesti_t res;
    fseek(f, 0, SEEK_SET);
    fread(&res, sizeof(ofesti_t), 1, f);

    return res;
}


