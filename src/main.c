#include <stdio.h>
#include <stdlib.h>
#include "../include/tuple.h"
#include "../include/boje.h"
#include "../include/student.h"
#include "../include/polaganje_ispita.h"
#include "../include/argpod.h"
#include "../include/test.h"

// Deklaracije funkcija za opcije menija
void meni_studenta();
void meni_polaganja();
void meni_argpod();

int main() {
	int izbor;
	do {
		printf("\n===== GLAVNI MENI =====\n");
		printf("1. Rad sa datotekom studenata\n");
		printf("2. Rad sa datotekom polaganja ispita\n");
		printf("3. Rad sa datotekom koja sadrzi agregirane podatke\n");
		printf("4. Test funkcije (ubacivanje pregenerisanih podataka)\n");
		printf("0. Izlaz iz programa\n");
		printf("Izaberite opciju: ");
		scanf("%d", &izbor);
		getchar(); // zbog enter-a
		switch(izbor) {
			case 1:
				meni_studenta();
				break;
			case 2:
				meni_polaganja();
				break;
			case 3:
				meni_argpod();
				break;
			case 4:
				ubaci_pregeniranse_studente_sturente();
				//ubaci_pregeniranse_polaganje_ispita();
			case 0:
				printf("Izlaz iz programa.\n");
				break;
			default:
				printf("Nepoznata opcija! Pokusajte ponovo.\n");
		}
	} while(izbor != 0);
	return 0;
}

void meni_studenta() {
	int izbor;
	do {
		printf("\n--- MENI STUDENTI ---\n");
		printf("1. Formiranje prazne datoteke za evidenciju studenata\n");
		printf("2. Upis novog studenta\n");
		printf("3. Prikaz godine studija i indeksa studenta za studentski broj\n");
		printf("4. Prikaz svih studenata prve godine sa usmenim ispitima\n");
		printf("5. Modifikacija podataka o studentu\n");
		printf("6. Ipis stanja datoteke\n");
		printf("0. Povratak na glavni meni\n");
		printf("Izaberite opciju: ");
		scanf("%d", &izbor);
		getchar();
		switch(izbor) {
			case 1:
				formiraj_praznu_datoteku_studenta();
				break;
			case 2:
				upis_novog_studenta_helper();
				break;
			case 3:
				prikazi_godinu_i_indeks();
				break;
			case 4:
				ispis_svih_studenata_prve_godine_sa_usmenim();
				break;
			case 5:
				modifikuj_studenta();
				break;
			case 6:
				ispis_stanja_datoteke_student();
			case 0:
				break;
			default:
				printf("Nepoznata opcija! Pokusajte ponovo.\n");
		}
	} while(izbor != 0);
}

void meni_polaganja() {
	int izbor;
	do {
		printf("\n--- MENI POLAGANJA ISPITA ---\n");
		printf("1. Formiranje prazne datoteke za evidenciju polaganja ispita\n");
		printf("2. Upis novog polaganja ispita\n");
		printf("3. Prikaz svih studenata prve godine sa usmenim ispitima\n");
		printf("4. Ispis stanja datoteke\n");
		printf("0. Povratak na glavni meni\n");
		printf("Izaberite opciju: ");
		scanf("%d", &izbor);
		getchar();
		switch(izbor) {
			case 1:
				formiraj_praznu_datoteku_polaganja();
				break;
			case 2:
				upis_novo_polaganje_helper();
				break;
			case 3:
				ispis_svih_studenata_prve_godine_sa_usmenim();
				break;
			case 4:
				ispis_stanja_datoteke_polaganje();
				break;
			case 0:
				break;
			default:
				printf("Nepoznata opcija! Pokusajte ponovo.\n");
		}
	} while(izbor != 0);
}

void meni_argpod() {
	int izbor;
	int studentski_broj;
	do {
		printf("\n--- MENI DAT SA AGREGIRANIM PODACIMA ---\n");
		printf("1. Formiranje datoteke sa agregiranim podacima\n");
		printf("2. Prikaz broja polozenih ispita na osnovu studentkog broja\n");
		printf("3. Prikaz svih studenata kod kojih je broj izlazaka na usmeni jednak borju izlazaka na pisemni\n");
		printf("4. Logicko brisanje sloga\n");
		printf("5. Ispis stanja datoteke\n");
		printf("0. Povratak na glavni meni\n");
		printf("Izaberite opciju: ");
		scanf("%d", &izbor);

		switch(izbor) {
			case 1:
				formiraj_praznu_datoteku_argpod();
				break;
			case 2:
				studentski_broj;
				printf("Unesite studentski broj: ");
				scanf("%d", &studentski_broj);
				prikaz_broja_polozenih_ispita_na_osnovu_studentkog_broja(studentski_broj);
				break;
			case 3:
				prikaz_svih_studenata_usmeni_eq_pisemni();
				break;
			case 4:
				printf("Unesite studentski broj: ");
				scanf("%d", &studentski_broj);
				brisanje_sloga(studentski_broj);
				break;
			case 5:
				ispis_stanja_datoteke_indeks_sek();
				break;
			case 0:
				break;
			default:
				printf("Nepoznata opcija! Pokusajte ponovo.\n");
		}
	} while (izbor != 0);
}
