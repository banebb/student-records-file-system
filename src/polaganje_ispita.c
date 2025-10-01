// Serijska datoteka polaganja: dodavanje na kraj, uz proveru jedinstvenosti identifikatora
#include "../include/polaganje_ispita.h"
#include "../include/student.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/boje.h"

void formiraj_praznu_datoteku_polaganja() {
	char naziv[256];
	printf("Unesite naziv nove datoteke za polaganje ispita: ");
	scanf("%255s", naziv);

	char puna_putanja[512];
	snprintf(puna_putanja, sizeof(puna_putanja), "%s%s", PUTANJA_FOLDERA_POLAGANJE, naziv);
	printf("Puna putanja do datoteke: %s\n", puna_putanja);

	FILE *f = fopen(puna_putanja, "wb");
	if (!f) {
		printf("Greska pri otvaranju datoteke!\n");
		return;
	}
    
	blok_polaganje_t blok = {0};
	blok.ispiti[0].id = OZNAKA_KRAJA_DATOTEKE;
	fwrite(&blok, sizeof(blok), 1, f);
	fclose(f);
	printf("Prazna datoteka '%s' sa f2=%d je formirana.\n", naziv, FAKTOR_BLOKIRANJA_POLAGANJE_ISPITA);
}

void upis_novo_polaganje_helper() {

	printf("Unesite naziv datoteke u koju zelite da unesete novo polaganje ispita: ");
	char naziv[50];
	char puna_putanja[50];
	scanf("%s", naziv);

	sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_POLAGANJE, naziv);
	FILE *f = fopen(puna_putanja, "rb+");
	if (!f) {
		printf(ANSI_COLOR_RED"Greska pri otvaranju datoteke!\n" ANSI_COLOR_RESET);
		return;
	}

	polaganje_ispita_t polaganje_ispita = {0};
	blok_polaganje_t blok = {0};

	while(1) {
		printf("Unesite identifikator: ");
		scanf("%d", &polaganje_ispita.id);
		getchar();
		int nadjen = 0;

		rewind(f);

		while (fread(&blok, sizeof(blok_polaganje_t), 1, f)) {

			for (int i = 0; i < FAKTOR_BLOKIRANJA_POLAGANJE_ISPITA; i++) {
				if (blok.ispiti[i].studentski_broj == OZNAKA_KRAJA_DATOTEKE) {
					break;
				}
				if (blok.ispiti[i].id == polaganje_ispita.id) {
					printf(ANSI_COLOR_RED "GRESKA: Polaganje sa ovim identifikatorom vec postoji. Pokusajte sa drugim identifikatorom .\n" ANSI_COLOR_RESET);
					nadjen = 1;
					break;
				}
			}
			if (nadjen) break;
		}
		if (!nadjen) break;
	}

	while (1) {

		printf("Unesite studentski broj studenta koji je izasao na polaganje: ");
		scanf("%d", &polaganje_ispita.studentski_broj);
		getchar();
		if (polaganje_ispita.studentski_broj < 1) {
			printf(ANSI_COLOR_RED"GRESKA: Studentski broj nije validan. Pokusajte ponovo.\n" ANSI_COLOR_RESET);
		} else {
			char naziv[50];
			printf("Unesite naziv datoteke iz koje zelite da ucitate studenta: ");
			scanf("%s", naziv);
			char puna_putanja[100];
			sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_STUDENT, naziv);
			FILE *f_student = fopen(puna_putanja, "rb");
			if (f_student == NULL) {
				printf(ANSI_COLOR_RED"Greska pri otvaranju datoteke!\n" ANSI_COLOR_RESET);
				continue;
			}
			tuple_t nadjen_student = nadji_mesto((student_t){.studentski_broj = polaganje_ispita.studentski_broj}, f_student);
			if (!nadjen_student.postoji) {
				printf(ANSI_COLOR_RED "Student sa studentskim brojem: %d ne postoji." ANSI_COLOR_RESET, polaganje_ispita.studentski_broj);
				fclose(f_student);
				continue;
			}
			break;
		}

	}

	printf("Unesite naziv predmeta: ");
	scanf("%30s", polaganje_ispita.naziv_predmeta);
	getchar();

	while (1) {
		printf("Unesite ocenu (5-10): ");
		scanf("%d", &polaganje_ispita.ocena);
		getchar();
		if (polaganje_ispita.ocena < 5 || polaganje_ispita.ocena > 10) {
			printf(ANSI_COLOR_RED"GRESKA: Ocena nije validna. Pokusajte ponovo.\n" ANSI_COLOR_RESET);
		} else {
			break;
		}
	}

	printf("Unesite broj predispitnih poena: ");
	scanf("%d", &polaganje_ispita.broj_predispitnih_poena);
	getchar();

	printf("Unesite broj ispitnih poena: ");
	scanf("%d", &polaganje_ispita.broj_ispitnih_poena);
	getchar();

	printf("Da li je ispit polozio na usmeni (DA/NE): \n");
	printf("\t1) DA\n");
	printf("\t2) NE\n");
	int izbor;
	while (1) {
		printf("Unesite opciju (1 ili 2): ");
		scanf("%d", &izbor);
		getchar();
		if (izbor == 1) {
			strcpy(polaganje_ispita.usmeni, "DA");
			break;
		}
		if (izbor == 2) {
			strcpy(polaganje_ispita.usmeni, "NE");
			break;
		}
		printf(ANSI_COLOR_RED"GRESKA: Nepoznata opcija. Pokusajte ponovo.\n" ANSI_COLOR_RESET);
	}

	upis_novo_polaganje(polaganje_ispita, f);
}

void upis_novo_polaganje(const polaganje_ispita_t novo_polaganje, FILE* f) {
	blok_polaganje_t blok = {0};
	rewind(f);
	while (fread(&blok, sizeof(blok_polaganje_t), 1, f)) {
		for (int i = 0; i < FAKTOR_BLOKIRANJA_POLAGANJE_ISPITA; i++) {
			if (blok.ispiti[i].id == OZNAKA_KRAJA_DATOTEKE) {
				blok.ispiti[i] = novo_polaganje;
				if (i + 1 < FAKTOR_BLOKIRANJA_POLAGANJE_ISPITA) {
					blok.ispiti[i + 1].id = OZNAKA_KRAJA_DATOTEKE;
					fseek(f, -sizeof(blok_polaganje_t), SEEK_CUR);
					fwrite(&blok, sizeof(blok_polaganje_t), 1, f);
					fclose(f);
					printf(ANSI_COLOR_GREEN"OK: Polaganje ispita je uspesno dodato u datoteku.\n" ANSI_COLOR_RESET);
					return;
				}
				fseek(f, -sizeof(blok_polaganje_t), SEEK_CUR);
				fwrite(&blok, sizeof(blok_polaganje_t), 1, f);


				blok_polaganje_t novi_blok = {0};
				novi_blok.ispiti[0].id = OZNAKA_KRAJA_DATOTEKE;
				fwrite(&novi_blok, sizeof(blok_polaganje_t), 1, f);
				fclose(f);
				printf(ANSI_COLOR_GREEN"OK: Polaganje ispita je uspesno dodato u datoteku.\n" ANSI_COLOR_RESET);
				return;
			}
		}
	}
}

void ispis_svih_studenata_prve_godine_sa_usmenim() {

	const student_i_pozicija_t* niz = get_studente_prve_godine();
	if (niz == NULL) {
		printf("====================================================\n");
		printf(ANSI_COLOR_YELLOW"SKUP REZULTATA PRAZAN\n" ANSI_COLOR_RESET);
		return;
	}

	printf("Unesite naziv datoteke iz koje zelite da preuzmete usmene ispite: ");
	char naziv[50];
	char puna_putanja[50];
	scanf("%s", naziv);

	sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_POLAGANJE, naziv);
	FILE *f = fopen(puna_putanja, "rb+");
	if (!f) {
		printf(ANSI_COLOR_RED"GRESKA: Otvaranje datoteke neuspesno!\n" ANSI_COLOR_RESET);
		return;
	}

	blok_polaganje_t blok_polaganje;

	printf("====================================================\n");
	while (fread(&blok_polaganje, sizeof(blok_polaganje_t), 1, f)) {

		for (int i = 0; i < FAKTOR_BLOKIRANJA_POLAGANJE_ISPITA; i++) {
			if (blok_polaganje.ispiti[i].id == OZNAKA_KRAJA_DATOTEKE) {
				printf("====================================================\n");
				fclose(f);
				return;
			}

			for (int j = 0; niz[j].student.studentski_broj != 0; j++) {
				if (strcmp(blok_polaganje.ispiti[i].usmeni, "DA") == 0 && niz[j].student.studentski_broj == blok_polaganje.ispiti[i].studentski_broj) {
					printf("ID: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  " , blok_polaganje.ispiti[i].id);
					printf("Studentski broj: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  " , blok_polaganje.ispiti[i].studentski_broj);
					printf("Ime i prezime: " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "  |  " , niz[j].student.ime_i_prezime);
					printf("Adresa bloka iz kojeg je student: " ANSI_COLOR_CYAN "%ld" ANSI_COLOR_RESET "  |  " , niz[j].adr_bloka);
					printf("Redni broj sloga u bloku: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  " , niz[j].redni_broj_sloga_u_bloku);
					printf("Naziv predmeta: " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "  |  " , blok_polaganje.ispiti[i].naziv_predmeta);
					printf("Ocena: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  " , blok_polaganje.ispiti[i].ocena);
					printf("Broj predispitnih poena: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  ", blok_polaganje.ispiti[i].broj_predispitnih_poena);
					printf("Broj ispitnih poena: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  ", blok_polaganje.ispiti[i].broj_ispitnih_poena);
					printf("Usmeni: " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "  |  " , blok_polaganje.ispiti[i].usmeni);
					printf("Adresa bloka iz kojeg je polaganje: " ANSI_COLOR_CYAN "%ld" ANSI_COLOR_RESET "  |  " , ftell(f) - sizeof(blok_polaganje_t));
					printf("Redni broj sloga u bloku polaganja: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  \n" , i);
					printf("----------------------------------------------------\n");
				}
			}
		}

	}


}

void ispis_stanja_datoteke_polaganje() {
	printf("Unesite naziv datoteke iz koje zelite da ispisete stanje polaganja ispita: ");
	char naziv[50];
	char puna_putanja[50];
	scanf("%s", naziv);

	sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_POLAGANJE, naziv);
	FILE *f = fopen(puna_putanja, "rb+");
	if (!f) {
		printf(ANSI_COLOR_RED"GRESKA: Otvaranje datoteke neuspesno!\n" ANSI_COLOR_RESET);
		return;
	}

	blok_polaganje_t blok = {0};

	printf("====================================================\n");
	while (fread(&blok, sizeof(blok_polaganje_t), 1, f)) {
		for (int i = 0; i < FAKTOR_BLOKIRANJA_POLAGANJE_ISPITA; i++) {
			if (blok.ispiti[i].id == OZNAKA_KRAJA_DATOTEKE) {
				printf("====================================================\n");
				fclose(f);
				return;
			}
			printf("ID: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  " , blok.ispiti[i].id);
			printf("Studentski broj: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  " , blok.ispiti[i].studentski_broj);
			printf("Naziv predmeta: " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "  |  " , blok.ispiti[i].naziv_predmeta);
			printf("Ocena: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  " , blok.ispiti[i].ocena);
			printf("Broj predispitnih poena: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  ", blok.ispiti[i].broj_predispitnih_poena);
			printf("Broj ispitnih poena: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "  |  ", blok.ispiti[i].broj_ispitnih_poena);
			printf("Usmeni: " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "  |  \n" , blok.ispiti[i].usmeni);
			printf("----------------------------------------------------\n");
		}
	}

}

agregacija_t nadji_agregatne_podatke(int studentski_broj, FILE* f) {

	blok_polaganje_t blok = {0};
	agregacija_t agregacija = {0};
	agregacija.kontrola = 0;

	rewind(f);
	while (fread(&blok, sizeof(blok_polaganje_t), 1, f)) {
		for (int i = 0; i < FAKTOR_BLOKIRANJA_POLAGANJE_ISPITA; i++) {
			if (blok.ispiti[i].id == OZNAKA_KRAJA_DATOTEKE) {
				break;
			}
			if (blok.ispiti[i].studentski_broj == studentski_broj) {
				agregacija.kontrola = 1;

				if (strcmp(blok.ispiti[i].usmeni, "DA") == 0) {
					agregacija.broj_izlazaka_usmeni++;
				} else {
					agregacija.broj_izlazaka_pismeni++;
				}
				if (blok.ispiti[i].ocena > 5) {
					agregacija.broj_polozenih_ispita++;
					agregacija.prosecna_ocena = ((agregacija.prosecna_ocena * (agregacija.broj_polozenih_ispita - 1)) + blok.ispiti[i].ocena) / agregacija.broj_polozenih_ispita;
				}
			}
		}
	}
	return agregacija;
}
