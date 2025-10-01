#include <stdio.h>
#include <string.h>
#include "../include/student.h"
#include "../include/argpod.h"

#include <stdlib.h>
#include <stdio.h>

#include "../include/boje.h"

void formiraj_praznu_datoteku_studenta() {
	char naziv[256];
	printf("Unesite naziv nove datoteke za studente: ");
	scanf("%s", naziv);

    char puna_putanja[512];
    snprintf(puna_putanja, sizeof(puna_putanja), "%s%s", PUTANJA_FOLDERA_STUDENT, naziv);
    printf("Puna putanja do datoteke: %s\n", puna_putanja);

	FILE *f = fopen(puna_putanja, "wb");
	if (!f) {
		printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke neuspesno!\n" ANSI_COLOR_RESET);
		return;
	}
    
	blok_student_t blok = {0};
	blok.studenti[0].studentski_broj = OZNAKA_KRAJA_DATOTEKE;
	fwrite(&blok, sizeof(blok), 1, f);
	fclose(f);
	printf(ANSI_COLOR_GREEN"OK: prazna datoteka '%s' sa f1=%d je formirana.\n" ANSI_COLOR_RESET, naziv, FAKTOR_BLOKIRANJA_STUDENT);
}

void upis_novog_studenta_helper () {
	printf("Unesite naziv datoteke u koju zelite da unesete novog studenta: ");
	char naziv[50];
	char puna_putanja[50];
	scanf("%s", naziv);

	sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_STUDENT, naziv);
	FILE *f = fopen(puna_putanja, "rb+");
	if (!f) {
		printf(ANSI_COLOR_RED"Greska pri otvaranju datoteke!\n" ANSI_COLOR_RESET);
		return;
	}

	student_t student = {0};
	blok_student_t blok = {0};

	while(1) {
		printf("Unesite studentski broj: ");
		scanf("%d", &student.studentski_broj);
		getchar();
		int nadjen = 0;
		if (student.studentski_broj == 0) {
			printf(ANSI_COLOR_RED"GRESKA: Studentski broj nije validan. Pokusajte ponovo.\n" ANSI_COLOR_RESET);
			continue;
		}
		rewind(f);

		while (fread(&blok, sizeof(blok_student_t), 1, f)) {

			for (int i = 0; i < FAKTOR_BLOKIRANJA_STUDENT; i++) {
				if (blok.studenti[i].studentski_broj == OZNAKA_KRAJA_DATOTEKE) {
					break;
				}
				if (blok.studenti[i].studentski_broj == student.studentski_broj) {
					printf(ANSI_COLOR_RED "GRESKA: Student sa ovim brojem vec postoji. Pokusajte sa drugim studentskim brojem.\n" ANSI_COLOR_RESET);
					nadjen = 1;
					break;
				}
			}
			if (nadjen) break;
		}
		if (!nadjen) break;
	}
	printf("Unesite ime i prezime: ");
	fgets(student.ime_i_prezime, IME_PREZIME_DUZINA + 1, stdin);
	student.ime_i_prezime[strcspn(student.ime_i_prezime, "\n")] = 0;
	printf("Unesite smer: ");
	fgets(student.smer, SMER_DUZINA + 1, stdin);
	student.smer[strcspn(student.smer, "\n")] = 0;
	printf("Unesite broj dosijea: ");
	scanf("%d", &student.broj_dosijea);

	while (1) {
		printf("Unesite godinu upisa: ");
		scanf("%d", &student.godina_upisa);

		if (student.godina_upisa < 1000 || student.godina_upisa > TRENUTNA_GOD) {
			printf(ANSI_COLOR_RED"GRESKA: godina upisa nije validna. Pokusajte ponovo.\n" ANSI_COLOR_RESET);
		} else {
			break;
		}
	}

	while (1) {
		printf("Unesite godinu studija: ");
		scanf("%d", &student.godina_studija);

		if (student.godina_studija < 1 || student.godina_studija > 7) {
			printf(ANSI_COLOR_RED"GRESKA: godina upisa nije validna. Pokusajte ponovo.\n" ANSI_COLOR_RESET);
		} else {
			break;
		}
	}

	upis_novog_studenta(student, f);
}

tuple_t nadji_mesto(student_t novi_student, FILE* f) {
	const int id = novi_student.studentski_broj;
	blok_student_t blok = {0};
	rewind(f);
	while (fread(&blok, sizeof(blok_student_t), 1, f)) {
		for (int i = 0 ; i < FAKTOR_BLOKIRANJA_STUDENT; i++) {
			if (blok.studenti[i].studentski_broj == id) {
				return (tuple_t){ftell(f), i, 1, 1, 0, 0};
			}
			if (blok.studenti[i].studentski_broj == OZNAKA_KRAJA_DATOTEKE) {
				if (i + 1 < FAKTOR_BLOKIRANJA_STUDENT) {
					return (tuple_t){ftell(f), i, 1, 0, 0, 0};
				}
				return (tuple_t){ftell(f), i, 1, 0, 1, 0};
			}
			if (blok.studenti[i].studentski_broj > id) {
				return (tuple_t){ftell(f), i, 1, 0, 0, 1};
			}
		}
	}

	return (tuple_t){-1, -1, 0, 0, 0, 0};
}

void upis_novog_studenta(student_t novi_student, FILE* f) {

	tuple_t info_o_poziciji = nadji_mesto(novi_student, f);
	blok_student_t blok = {0};
	student_t temp = {0}, temp2 = {0};

	if (info_o_poziciji.postoji) {

		printf(ANSI_COLOR_RED"GRESKA: Student sa ovim brojem vec postoji!\n" ANSI_COLOR_RESET);
		fclose(f);

	} else {

		if (!info_o_poziciji.nadjena_pozicja) {
			printf(ANSI_COLOR_RED "GRESKA: pozicija nije prodnadjena iz nepoznatog razloga" ANSI_COLOR_RESET);
		} else {
			if (info_o_poziciji.adr_bloka_pozicije_upisa != -1 && info_o_poziciji.redni_broj_pozicije_upisa_u_bloku != -1) {

				if (!info_o_poziciji.treba_pomeranje) {

					if (!info_o_poziciji.treba_novi_blok_za_kraj_datoteke) { // kraj datoteke nije u ovom bloku

						fseek(f, info_o_poziciji.adr_bloka_pozicije_upisa - sizeof(blok_student_t), SEEK_SET);
						fread(&blok, sizeof(blok_student_t), 1, f);
						blok.studenti[info_o_poziciji.redni_broj_pozicije_upisa_u_bloku] = novi_student;
						blok.studenti[info_o_poziciji.redni_broj_pozicije_upisa_u_bloku + 1].studentski_broj = OZNAKA_KRAJA_DATOTEKE;
						fseek(f, info_o_poziciji.adr_bloka_pozicije_upisa - sizeof(blok_student_t), SEEK_SET);
						fwrite(&blok, sizeof(blok_student_t), 1, f);
						fclose(f);



						printf(ANSI_COLOR_GREEN"OK: Student je uspesno dodat u datoteku.\n" ANSI_COLOR_RESET);

					} else { //kraj datoteke je u ovom bloku

						fseek(f, info_o_poziciji.adr_bloka_pozicije_upisa - sizeof(blok_student_t), SEEK_SET);
						fread(&blok, sizeof(blok_student_t), 1, f);
						blok.studenti[info_o_poziciji.redni_broj_pozicije_upisa_u_bloku] = novi_student;
						fseek(f, info_o_poziciji.adr_bloka_pozicije_upisa - sizeof(blok_student_t), SEEK_SET);
						fwrite(&blok, sizeof(blok_student_t), 1, f);

						blok_student_t novi_blok = {0};
						novi_blok.studenti[0].studentski_broj = OZNAKA_KRAJA_DATOTEKE;
						fwrite(&novi_blok, sizeof(blok_student_t), 1, f);
						fclose(f);



						printf(ANSI_COLOR_GREEN"OK: Student je uspesno dodat u datoteku.\n" ANSI_COLOR_RESET);

					}

				} else { // nadjena je pozicija u bloku, ali je potrebno pomeranje ostalih blokova

					fseek(f, info_o_poziciji.adr_bloka_pozicije_upisa - sizeof(blok_student_t), SEEK_SET);
					fread(&blok, sizeof(blok_student_t), 1, f);
					temp = blok.studenti[info_o_poziciji.redni_broj_pozicije_upisa_u_bloku];
					blok.studenti[info_o_poziciji.redni_broj_pozicije_upisa_u_bloku] = novi_student;
					// pomeranje ostalih u bloku gde se upisuje
					for (int i = info_o_poziciji.redni_broj_pozicije_upisa_u_bloku + 1; i < FAKTOR_BLOKIRANJA_STUDENT; i++) {
						if (blok.studenti[i].studentski_broj != OZNAKA_KRAJA_DATOTEKE) {

							temp2 = blok.studenti[i];
							blok.studenti[i] = temp;
							temp = temp2;

						} else { // dosli smo do kraja datoteke u ovom bloku

							if (i + 1 < FAKTOR_BLOKIRANJA_STUDENT) { // jos ima mesta u ovom bloku
								blok.studenti[i] = temp;
								blok.studenti[i + 1].studentski_broj = OZNAKA_KRAJA_DATOTEKE;
								fseek(f, info_o_poziciji.adr_bloka_pozicije_upisa - sizeof(blok_student_t), SEEK_SET);
								fwrite(&blok, sizeof(blok_student_t), 1, f);
								fclose(f);
								printf(ANSI_COLOR_GREEN"OK: Student je uspesno dodat u datoteku.\n" ANSI_COLOR_RESET);
								return;
							}
							// nema mesta u ovom bloku, treba novi blok
							blok.studenti[i] = temp;
							fseek(f, info_o_poziciji.adr_bloka_pozicije_upisa - sizeof(blok_student_t), SEEK_SET);
							fwrite(&blok, sizeof(blok_student_t), 1, f);

							blok_student_t novi_blok = {0};
							novi_blok.studenti[0].studentski_broj = OZNAKA_KRAJA_DATOTEKE;
							fwrite(&novi_blok, sizeof(blok_student_t), 1, f);
							fclose(f);
							printf(ANSI_COLOR_GREEN"OK: Student je uspesno dodat u datoteku.\n" ANSI_COLOR_RESET);
							return;

						}
					}

					fseek(f, -sizeof(blok), SEEK_CUR);
					fwrite(&blok, sizeof(blok_student_t), 1, f);

					//nismo dosli do kraja datoteke nastavljamo pomeranje slogova u ostalim blokovima
					while (fread(&blok, sizeof(blok_student_t), 1, f)) {
						// pomeranje ostalih u bloku
						for (int i = 0; i < FAKTOR_BLOKIRANJA_STUDENT; i++) {
							if (i != OZNAKA_KRAJA_DATOTEKE) { //ako nije oznaka kraja datoteke nastavi pomeranje

								temp2 = blok.studenti[i];
								blok.studenti[i] = temp;
								temp = temp2;

							} else { // dosli smo do kraja datoteke u ovom bloku

								if (i + 1 < FAKTOR_BLOKIRANJA_STUDENT) { // jos ima mesta u ovom bloku
									blok.studenti[i] = temp;
									blok.studenti[i + 1].studentski_broj = OZNAKA_KRAJA_DATOTEKE;
									fseek(f, -sizeof(blok_student_t), SEEK_SET);
									fwrite(&blok, sizeof(blok_student_t), 1, f);
									fclose(f);
									printf(ANSI_COLOR_GREEN"OK: Student je uspesno dodat u datoteku.\n" ANSI_COLOR_RESET);
									return;
								}
								// nema mesta u ovom bloku, treba novi blok
								blok.studenti[i] = temp;
								fseek(f, -sizeof(blok_student_t), SEEK_SET);
								fwrite(&blok, sizeof(blok_student_t), 1, f);

								blok_student_t novi_blok = {0};
								novi_blok.studenti[0].studentski_broj = OZNAKA_KRAJA_DATOTEKE;
								fwrite(&novi_blok, sizeof(blok_student_t), 1, f);
								fclose(f);
								printf(ANSI_COLOR_GREEN"OK: Student je uspesno dodat u datoteku.\n" ANSI_COLOR_RESET);
								return;

							}
						}
						fseek(f, -sizeof(blok_student_t), SEEK_CUR);
						fwrite(&blok, sizeof(blok_student_t), 1, f);
					}
				}
			}
		}
	}
}

void prikazi_godinu_i_indeks() {
	student_t student = {0};
	blok_student_t blok = {0};

	printf("Unesite studentski broj kojeg zelite da nadjete: ");
	scanf("%d", &student.studentski_broj);

	printf("Unesite naziv datoteke u kojoj zelite da nadjete studenta: ");
	char naziv[50];
	char puna_putanja[50];
	scanf("%s", naziv);
	sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_STUDENT, naziv);
	FILE *f = fopen(puna_putanja, "rb");
	if (!f) {
		printf(ANSI_COLOR_RED"Greska pri otvaranju datoteke!\n" ANSI_COLOR_RESET);
	} else {
		tuple_t nadjen_student = nadji_mesto(student, f);
		printf("\n====================================================\n");
		if (nadjen_student.postoji) {
			fseek(f, nadjen_student.adr_bloka_pozicije_upisa - sizeof(blok_student_t), SEEK_SET);
			fread(&blok, sizeof(blok_student_t), 1, f);

			student = blok.studenti[nadjen_student.redni_broj_pozicije_upisa_u_bloku];

			printf("Godina studija : " ANSI_COLOR_CYAN " %d" ANSI_COLOR_RESET "  |  Indeks: " ANSI_COLOR_CYAN " %s %d/%d" ANSI_COLOR_RESET "  |  Adresa bloka: " ANSI_COLOR_CYAN " %ld" ANSI_COLOR_RESET "  |  Redni broj sloga u bloku: " ANSI_COLOR_CYAN " %d" ANSI_COLOR_RESET "\n", student.godina_studija, student.smer, student.broj_dosijea, student.godina_upisa, nadjen_student.adr_bloka_pozicije_upisa, nadjen_student.redni_broj_pozicije_upisa_u_bloku);
			fclose(f);
		} else {
			printf(ANSI_COLOR_RED"Student sa studentskim brojem %d nije pronadjen u datoteci %s.\n" ANSI_COLOR_RESET, student.studentski_broj, naziv);
			fclose(f);
		}
		printf("====================================================\n");
	}

}


void modifikuj_studenta() {
	student_t student = {0};
	blok_student_t blok = {0};

	printf("Unesite studentski broj kojeg zelite da izmenite: ");
	scanf("%d", &student.studentski_broj);

	printf("Unesite naziv datoteke u kojoj zelite da nadjete studenta: ");
	char naziv[50];
	char puna_putanja[50];
	scanf("%s", naziv);
	sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_STUDENT, naziv);
	FILE *f = fopen(puna_putanja, "rb+");
	if (!f) {
		printf(ANSI_COLOR_RED"Greska pri otvaranju datoteke!\n" ANSI_COLOR_RESET);
	} else {
		tuple_t nadjen_student = nadji_mesto(student, f);
		printf("\n====================================================\n");
		if (nadjen_student.postoji) {
			fseek(f, nadjen_student.adr_bloka_pozicije_upisa - sizeof(blok_student_t), SEEK_SET);
			fread(&blok, sizeof(blok_student_t), 1, f);

			student = blok.studenti[nadjen_student.redni_broj_pozicije_upisa_u_bloku];

			int unesena_opcija = 0;
			while (1) {

				printf("Izaberite koju informaciju zelite da izmenite:\n");
				printf("1. Ime i prezime: " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "\n", student.ime_i_prezime);
				printf("2. Smer: " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "\n", student.smer);
				printf("3. Broj dosijea: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", student.broj_dosijea);
				printf("4. Godina upisa: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", student.godina_upisa);
				printf("5. Godina studija: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", student.godina_studija);
				printf("0. Izlaz iz modifikacije i cuvanje\n");
				printf("Unesite opciju: ");
				scanf("%d", &unesena_opcija);
				getchar();

				switch (unesena_opcija) {
					case 1:
						printf("Uniseite novo ime i prezime(do 50 karaktera): ");
						fgets(student.ime_i_prezime, IME_PREZIME_DUZINA + 1, stdin);
						student.ime_i_prezime[strcspn(student.ime_i_prezime, "\n")] = 0;
						break;
					case 2:
						printf("Uniseite novi smer(do 4 karaktera): ");
						fgets(student.smer, SMER_DUZINA + 1, stdin);
						student.smer[strcspn(student.smer, "\n")] = 0;
						break;
					case 3:
						printf("Unesite novi broj dosijea: ");
						scanf("%d", &student.broj_dosijea);
						break;
					case 4:
						printf("Unesite novu godinu upisa: ");
						int godina_upisa = 0;
						scanf("%d", &godina_upisa);
						if (godina_upisa < 1000 || godina_upisa > TRENUTNA_GOD) {
							printf(ANSI_COLOR_RED"GRESKA: godina upisa nije validna. Pokusajte ponovo.\n" ANSI_COLOR_RESET);
							continue;
						}
						student.godina_upisa = godina_upisa;
						break;
					case 5:
						printf("Unesite novu godinu studija: ");
						scanf("%d", &student.godina_studija);
						break;
					case 0:
						printf("Izlaz iz modifikacije i cuvanje...\n");
						blok.studenti[nadjen_student.redni_broj_pozicije_upisa_u_bloku] = student;
						fseek(f, -sizeof(blok_student_t), SEEK_CUR);
						fwrite(&blok, sizeof(blok_student_t), 1, f);
						fclose(f);
						printf(ANSI_COLOR_GREEN"OK: Student je uspesno izmenjen u datoteci.\n" ANSI_COLOR_RESET);
						return;
					default:
						printf(ANSI_COLOR_RED"Nepoznata opcija! Pokusajte ponovo.\n" ANSI_COLOR_RESET);
						break;
				}

			}
		}
		printf(ANSI_COLOR_RED"Student sa studentskim brojem %d nije pronadjen u datoteci %s.\n" ANSI_COLOR_RESET, student.studentski_broj, naziv);
		fclose(f);
		printf("====================================================\n");
	}

}

student_i_pozicija_t* get_studente_prve_godine() {
	int mesta_u_nizu = 100;
	student_i_pozicija_t *niz = malloc(mesta_u_nizu * sizeof(student_i_pozicija_t));

	printf("Unesite naziv datoteke za prikupljanje podataka o studentima prve godine: ");
	char naziv[50];
	char puna_putanja[50];
	scanf("%s", naziv);

	sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_STUDENT, naziv);
	FILE *f = fopen(puna_putanja, "rb+");
	if (!f) {
		printf(ANSI_COLOR_RED"Greska pri otvaranju datoteke!\n" ANSI_COLOR_RESET);
		return NULL;
	}

	blok_student_t blok_student;
	int poz_u_nizu = 0;

	while (fread(&blok_student, sizeof(blok_student_t), 1, f)) {

		for (int i = 0; i < FAKTOR_BLOKIRANJA_STUDENT; i++) {
			if (blok_student.studenti[i].studentski_broj == OZNAKA_KRAJA_DATOTEKE) {
				fclose(f);
				return niz;
			}

			if (blok_student.studenti[i].godina_studija == 1) {
				niz[poz_u_nizu].student = blok_student.studenti[i];
				niz[poz_u_nizu].adr_bloka = ftell(f) - sizeof(blok_student_t);
				niz[poz_u_nizu].redni_broj_sloga_u_bloku = i;
				poz_u_nizu++;

				if (poz_u_nizu == mesta_u_nizu - 1) {
					mesta_u_nizu *= 2;
					niz = realloc(&niz, mesta_u_nizu * sizeof(student_i_pozicija_t));
				}
			}
		}
	}

	if (poz_u_nizu == 0) {
		return NULL;
	}
	return niz;
}

void ispis_stanja_datoteke_student() {
	printf("Unesite naziv datoteke za prikupljanje kompletnog sadrzaja datoteke: ");
	char naziv[50];
	char puna_putanja[50];
	scanf("%s", naziv);

	sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_STUDENT, naziv);
	FILE *f = fopen(puna_putanja, "rb+");
	if (!f) {
		printf(ANSI_COLOR_RED"Greska pri otvaranju datoteke!\n" ANSI_COLOR_RESET);
		return;
	}

	blok_student_t novi_blok = {0};
	printf("====================================================1\n");
	while (fread(&novi_blok, sizeof(blok_student_t), 1, f)) {
		for (int i = 0; i < FAKTOR_BLOKIRANJA_STUDENT; i++) {
			if (novi_blok.studenti[i].studentski_broj == OZNAKA_KRAJA_DATOTEKE) {
				printf("====================================================2\n");
				fclose(f);
				return;
			}

			printf("Student broj: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET"  |  ", novi_blok.studenti[i].studentski_broj);
			printf("Ime i prezime: " ANSI_COLOR_CYAN"%s" ANSI_COLOR_RESET"  |  ", novi_blok.studenti[i].ime_i_prezime);
			printf("Smer: " ANSI_COLOR_CYAN"%s" ANSI_COLOR_RESET"  |  ", novi_blok.studenti[i].smer);
			printf("Broj dosijea: " ANSI_COLOR_CYAN"%d" ANSI_COLOR_RESET"  |  ", novi_blok.studenti[i].broj_dosijea);
			printf("Godina upisa: " ANSI_COLOR_CYAN"%d" ANSI_COLOR_RESET"  |  " , novi_blok.studenti[i].godina_upisa);
			printf("Godina studija: " ANSI_COLOR_CYAN"%d" ANSI_COLOR_RESET"  |  ", novi_blok.studenti[i].godina_studija);
			printf("\n----------------------------------------------------\n");
		}
	}

}
