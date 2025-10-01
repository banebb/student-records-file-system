
#ifndef BP1_PROJ__COPY__OFSETI_H
#define BP1_PROJ__COPY__OFSETI_H
#include <stdio.h>

typedef struct {
    long adresa_pocetka_primarne_zone;
    long adresa_pocetka_indeksne_zone;
    long adresa_pocetka_zone_prekoracenja;
    long adresa_slobodnog_prostora;
    long adresa_kraja_primarne_zone;
}ofesti_t;

void upisi_ofsete(ofesti_t ofseti);
ofesti_t procitaj_ofsete();


#endif //BP1_PROJ__COPY__OFSETI_H