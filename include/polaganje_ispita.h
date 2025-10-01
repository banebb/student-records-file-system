#ifndef POLAGANJE_ISPITA_H
#define POLAGANJE_ISPITA_H

#define NAZIV_PREDMETA_DUZINA 30
#define USMENI_DUZINA 2
#define FAKTOR_BLOKIRANJA_POLAGANJE_ISPITA 5
#define PUTANJA_FOLDERA_POLAGANJE "../podaci/polaganje_ispita/"
#include <stdio.h>

typedef struct {
	int id;
	int studentski_broj;
	char naziv_predmeta[NAZIV_PREDMETA_DUZINA + 1];
	int ocena; // 5-10
	int broj_predispitnih_poena;
	int broj_ispitnih_poena;
	char usmeni[USMENI_DUZINA + 1]; // "DA" ili "NE"
} polaganje_ispita_t;

typedef struct {
	polaganje_ispita_t ispiti[FAKTOR_BLOKIRANJA_POLAGANJE_ISPITA];
} blok_polaganje_t;

typedef struct {
	int broj_polozenih_ispita;
	int broj_izlazaka_usmeni;
	int broj_izlazaka_pismeni;
	float prosecna_ocena;
	int kontrola;
}agregacija_t;

// Deklaracije funkcija za rad sa polaganjima ispita
void formiraj_praznu_datoteku_polaganja();
void upis_novo_polaganje_helper();
void upis_novo_polaganje(polaganje_ispita_t novo_polaganje, FILE* f);
void ispis_svih_studenata_prve_godine_sa_usmenim();
void ispis_stanja_datoteke_polaganje();
agregacija_t nadji_agregatne_podatke(int studentski_broj, FILE* f);

#endif // POLAGANJE_ISPITA_H
