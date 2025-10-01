#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/boje.h"
#include  "../include/student.h"
#include  "../include/polaganje_ispita.h"


#ifndef BP1_PROJ__COPY__ARGPOD_H
#define BP1_PROJ__COPY__ARGPOD_H
#define FAKTOR_BLOKIRANJA_PRIMARNE_ZONE FAKTOR_BLOKIRANJA_STUDENT
#define FAKTOR_BLOKIRANJA_INDEKSNA_ZONE 2
#define FAKTOR_BLOKIRANJA_ZONE_PREKORACENJA 1
#define PUTANJA_FOLDERA_ARGPOD "../podaci/index_sek/"


typedef struct {
    int studentski_broj;
    char ime_i_prezime[IME_PREZIME_DUZINA + 1];
    char smer[SMER_DUZINA + 1];
    int broj_dosijea;
    int godina_upisa;
    int godina_studija;
    int broj_polozenih_ispita;
    int broj_izlazaka_usmeni;
    int broj_izlazaka_pismeni;
    float prosecna_ocena;
    int obrisan;
} slog_primarne_zone_t;

typedef struct {
    slog_primarne_zone_t slog_primarne_zone[FAKTOR_BLOKIRANJA_PRIMARNE_ZONE];
} blok_primarne_zone_t;

typedef struct {
    int broj_kljuceva;
    int kljucevi[FAKTOR_BLOKIRANJA_INDEKSNA_ZONE];
    long adrese_dece_blokova[FAKTOR_BLOKIRANJA_INDEKSNA_ZONE];
    long adresa_prvog_prekoracioca[FAKTOR_BLOKIRANJA_INDEKSNA_ZONE];
    int da_li_je_list;
    long adresa_cvora;
} cvor_indeksne_zone_t;

typedef struct {
    slog_primarne_zone_t sadrzaj;
    long adresa_sledeceg_prekoracioca;
} slog_zone_prekoracenja_t;

typedef struct{
    long addr_blok;
    long addr_prvog_prek;
    long addr_cvora;
} adr_bloka_addr_prek_t;


void formiraj_praznu_datoteku_argpod();
void prikaz_broja_polozenih_ispita_na_osnovu_studentkog_broja(int studentski_broj);
void prikaz_svih_studenata_usmeni_eq_pisemni();
void ispis_stanja_datoteke_indeks_sek();
void brisanje_sloga(int studentski_broj);
void propagacija_unosa_studenta(student_t student);
adr_bloka_addr_prek_t nadji_slog(int studentski_broj) ;


#endif //BP1_PROJ__COPY__ARGPOD_H