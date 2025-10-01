#ifndef STUDENT_H
#define STUDENT_H

#define IME_PREZIME_DUZINA 50
#define SMER_DUZINA 4
#define FAKTOR_BLOKIRANJA_STUDENT 5 
#define PUTANJA_FOLDERA_STUDENT "../podaci/student/"
#define OZNAKA_KRAJA_DATOTEKE (-1)
#define TRENUTNA_GOD 2025
#include "tuple.h"
#include <stdio.h>

typedef struct {
	int studentski_broj; 
	char ime_i_prezime[IME_PREZIME_DUZINA + 1];
	char smer[SMER_DUZINA + 1];
	int broj_dosijea;
	int godina_upisa;
	int godina_studija; 
} student_t;

typedef struct {
	student_t studenti[FAKTOR_BLOKIRANJA_STUDENT];
} blok_student_t;

typedef struct {
	student_t student;
	long adr_bloka;
	int redni_broj_sloga_u_bloku;
} student_i_pozicija_t;

void formiraj_praznu_datoteku_studenta();
void upis_novog_studenta_helper();
tuple_t nadji_mesto(student_t novi_student, FILE* f);
void upis_novog_studenta(student_t novi_student, FILE* f);
void prikazi_godinu_i_indeks();
void prikazi_studente_prva_godina_usmeni();
void modifikuj_studenta();
student_i_pozicija_t* get_studente_prve_godine();
void ispis_stanja_datoteke_student();

#endif // STUDENT_H
