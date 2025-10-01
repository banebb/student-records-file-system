#include "../include/argpod.h"
#include "../include/ofseti.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>


void formiraj_praznu_datoteku_argpod() {
    printf("=== Formiranje datoteke agregatne podrske ===\n");

    char naziv[50];
    char puna_putanja[100];

    printf("Unesite naziv datoteke odakle zelite da citate informacije o studentima: ");
    scanf("%s", naziv);
    sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_STUDENT, naziv);

    FILE *f_stud = fopen(puna_putanja, "rb");
    if (!f_stud) {
        printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke sa podacima o studentu neuspesno!\n" ANSI_COLOR_RESET);
        return;
    }

    printf("Unesite naziv datoteke odakle zelite da citate informacije o polaganjima ispita: ");
    scanf("%s", naziv);
    sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_POLAGANJE, naziv);

    FILE *f_pol = fopen(puna_putanja, "rb");
    if (!f_pol) {
        printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke sa podacima o polaganjima ispita neuspesno!\n" ANSI_COLOR_RESET);
        fclose(f_stud);
        return;
    }

    FILE *f_agr = fopen("../podaci/index_sek/agrpod.bin", "wb+");
    if (!f_agr) {
        printf(ANSI_COLOR_RED"GRESKA: kreiranje datoteke za agregatnu podrsku neuspesno!\n" ANSI_COLOR_RESET);
        fclose(f_stud);
        fclose(f_pol);
        return;
    }

    ofesti_t ofseti;
    ofseti.adresa_pocetka_primarne_zone = 0;

    int broj_ucitanih_blokova = 0;
    blok_student_t blok_student = {0};
    blok_primarne_zone_t blok_primarne_zone = {0};

    cvor_indeksne_zone_t *listovi = malloc(100 * sizeof(cvor_indeksne_zone_t));
    memset(listovi, 0, 100 * sizeof(cvor_indeksne_zone_t));
    int max_listova = 100;
    int broj_lista = 0;
    int poz_unutar_cvora = 0;

    printf("Procesiranje podataka o studentima...\n");

    while (fread(&blok_student, sizeof(blok_student), 1, f_stud)) {
        memset(&blok_primarne_zone, 0, sizeof(blok_primarne_zone));

        int ima_validne_studente = 0;
        for (int i = 0; i < FAKTOR_BLOKIRANJA_STUDENT; i++) {
            if (blok_student.studenti[i].studentski_broj != 0 &&
                blok_student.studenti[i].studentski_broj != OZNAKA_KRAJA_DATOTEKE) {
                ima_validne_studente = 1;
                break;
            }
        }

        if (!ima_validne_studente) {
            continue;
        }

        for (int i = 0; i < FAKTOR_BLOKIRANJA_STUDENT; i++) {
            if (blok_student.studenti[i].studentski_broj == OZNAKA_KRAJA_DATOTEKE) {
                break;
            }

            if (blok_student.studenti[i].studentski_broj == 0) {
                continue;
            }

            blok_primarne_zone.slog_primarne_zone[i].studentski_broj = blok_student.studenti[i].studentski_broj;
            strcpy(blok_primarne_zone.slog_primarne_zone[i].ime_i_prezime, blok_student.studenti[i].ime_i_prezime);
            strcpy(blok_primarne_zone.slog_primarne_zone[i].smer, blok_student.studenti[i].smer);
            blok_primarne_zone.slog_primarne_zone[i].broj_dosijea = blok_student.studenti[i].broj_dosijea;
            blok_primarne_zone.slog_primarne_zone[i].godina_upisa = blok_student.studenti[i].godina_upisa;
            blok_primarne_zone.slog_primarne_zone[i].godina_studija = blok_student.studenti[i].godina_studija;
            blok_primarne_zone.slog_primarne_zone[i].obrisan = 0;

            agregacija_t agregatni_podaci = nadji_agregatne_podatke(blok_student.studenti[i].studentski_broj, f_pol);

            if (agregatni_podaci.kontrola) {
                blok_primarne_zone.slog_primarne_zone[i].broj_polozenih_ispita = agregatni_podaci.broj_polozenih_ispita;
                blok_primarne_zone.slog_primarne_zone[i].broj_izlazaka_usmeni = agregatni_podaci.broj_izlazaka_usmeni;
                blok_primarne_zone.slog_primarne_zone[i].broj_izlazaka_pismeni = agregatni_podaci.broj_izlazaka_pismeni;
                blok_primarne_zone.slog_primarne_zone[i].prosecna_ocena = agregatni_podaci.prosecna_ocena;
            } else {
                blok_primarne_zone.slog_primarne_zone[i].broj_polozenih_ispita = 0;
                blok_primarne_zone.slog_primarne_zone[i].broj_izlazaka_usmeni = 0;
                blok_primarne_zone.slog_primarne_zone[i].broj_izlazaka_pismeni = 0;
                blok_primarne_zone.slog_primarne_zone[i].prosecna_ocena = 0.0;
            }
        }

        long adresa_bloka = ftell(f_agr);
        fwrite(&blok_primarne_zone, sizeof(blok_primarne_zone), 1, f_agr);
        broj_ucitanih_blokova++;

        int min_key = 0;
        for (int i = 0; i < FAKTOR_BLOKIRANJA_PRIMARNE_ZONE; i++) {
            if (blok_primarne_zone.slog_primarne_zone[i].studentski_broj != 0 &&
                blok_primarne_zone.slog_primarne_zone[i].studentski_broj != OZNAKA_KRAJA_DATOTEKE) {
                if (min_key == 0 || blok_primarne_zone.slog_primarne_zone[i].studentski_broj < min_key) {
                    min_key = blok_primarne_zone.slog_primarne_zone[i].studentski_broj;
                }
            }
        }

        if (broj_ucitanih_blokova == 1) {
            listovi[0].kljucevi[0] = min_key;
            listovi[0].adrese_dece_blokova[0] = adresa_bloka;
            listovi[0].broj_kljuceva = 1;
            listovi[0].da_li_je_list = 1;
            listovi[0].adresa_prvog_prekoracioca[0] = -1;
            poz_unutar_cvora = 1;
        } else {
            if (poz_unutar_cvora < FAKTOR_BLOKIRANJA_INDEKSNA_ZONE) {
                listovi[broj_lista].kljucevi[poz_unutar_cvora] = min_key;
                listovi[broj_lista].adrese_dece_blokova[poz_unutar_cvora] = adresa_bloka;
                listovi[broj_lista].broj_kljuceva++;
                listovi[broj_lista].adresa_prvog_prekoracioca[poz_unutar_cvora] = -1;
                poz_unutar_cvora++;
            } else {
                broj_lista++;
                if (broj_lista >= max_listova) {
                    max_listova += 100;
                    listovi = realloc(listovi, max_listova * sizeof(cvor_indeksne_zone_t));
                    memset(&listovi[broj_lista], 0, (max_listova - broj_lista) * sizeof(cvor_indeksne_zone_t));
                }

                listovi[broj_lista].kljucevi[0] = min_key;
                listovi[broj_lista].adrese_dece_blokova[0] = adresa_bloka;
                listovi[broj_lista].broj_kljuceva = 1;
                listovi[broj_lista].da_li_je_list = 1;
                listovi[broj_lista].adresa_prvog_prekoracioca[0] = -1;
                poz_unutar_cvora = 1;
            }
        }
    }

    if (broj_ucitanih_blokova == 0) {
        printf(ANSI_COLOR_RED"GRESKA: Nema podataka o studentima za procesiranje!\n" ANSI_COLOR_RESET);
        fclose(f_stud);
        fclose(f_pol);
        fclose(f_agr);
        free(listovi);
        return;
    }

    printf("Procesiran je %d blok(ova) studenata.\n", broj_ucitanih_blokova);
    ofseti.adresa_kraja_primarne_zone = ftell(f_agr);

    printf("Kreiranje stabla indeksa...\n");

    listovi[0].kljucevi[0] = 0;
    for (int i = 0; i <= broj_lista; i++) {
        listovi[i].adresa_cvora = ftell(f_agr);
        fwrite(&listovi[i], sizeof(cvor_indeksne_zone_t), 1, f_agr);
    }

    cvor_indeksne_zone_t *trenutni_nivo = listovi;
    int broj_cvorova_trenutnog_nivoa = broj_lista + 1;

    while (broj_cvorova_trenutnog_nivoa > 1) {
        int broj_cvorova_novog_nivoa = (broj_cvorova_trenutnog_nivoa + FAKTOR_BLOKIRANJA_INDEKSNA_ZONE - 1) / FAKTOR_BLOKIRANJA_INDEKSNA_ZONE;
        cvor_indeksne_zone_t *novi_nivo = malloc(broj_cvorova_novog_nivoa * sizeof(cvor_indeksne_zone_t));
        memset(novi_nivo, 0, broj_cvorova_novog_nivoa * sizeof(cvor_indeksne_zone_t));

        int pozicija_u_novom_nivou = 0;
        int pozicija_u_novom_cvoru = 0;

        for (int i = 0; i < broj_cvorova_trenutnog_nivoa; i++) {
            if (pozicija_u_novom_cvoru < FAKTOR_BLOKIRANJA_INDEKSNA_ZONE) {
                novi_nivo[pozicija_u_novom_nivou].kljucevi[pozicija_u_novom_cvoru] = trenutni_nivo[i].kljucevi[0];
                novi_nivo[pozicija_u_novom_nivou].adrese_dece_blokova[pozicija_u_novom_cvoru] = trenutni_nivo[i].adresa_cvora;
                novi_nivo[pozicija_u_novom_nivou].broj_kljuceva++;
                novi_nivo[pozicija_u_novom_nivou].da_li_je_list = 0;
                novi_nivo[pozicija_u_novom_nivou].adresa_prvog_prekoracioca[pozicija_u_novom_cvoru] = -1;
                pozicija_u_novom_cvoru++;
            } else {
                novi_nivo[pozicija_u_novom_nivou].adresa_cvora = ftell(f_agr);
                fwrite(&novi_nivo[pozicija_u_novom_nivou], sizeof(cvor_indeksne_zone_t), 1, f_agr);

                pozicija_u_novom_nivou++;
                pozicija_u_novom_cvoru = 0;

                novi_nivo[pozicija_u_novom_nivou].kljucevi[pozicija_u_novom_cvoru] = trenutni_nivo[i].kljucevi[0];
                novi_nivo[pozicija_u_novom_nivou].adrese_dece_blokova[pozicija_u_novom_cvoru] = trenutni_nivo[i].adresa_cvora;
                novi_nivo[pozicija_u_novom_nivou].broj_kljuceva++;
                novi_nivo[pozicija_u_novom_nivou].da_li_je_list = 0;
                novi_nivo[pozicija_u_novom_nivou].adresa_prvog_prekoracioca[pozicija_u_novom_cvoru] = -1;
                pozicija_u_novom_cvoru++;
            }
        }

        if (novi_nivo[pozicija_u_novom_nivou].broj_kljuceva > 0) {
            novi_nivo[pozicija_u_novom_nivou].adresa_cvora = ftell(f_agr);
            fwrite(&novi_nivo[pozicija_u_novom_nivou], sizeof(cvor_indeksne_zone_t), 1, f_agr);
        }

        if (trenutni_nivo != listovi) {
            free(trenutni_nivo);
        }
        trenutni_nivo = novi_nivo;
        broj_cvorova_trenutnog_nivoa = broj_cvorova_novog_nivoa;
    }

    ofseti.adresa_pocetka_indeksne_zone = trenutni_nivo[0].adresa_cvora;

    ofseti.adresa_pocetka_zone_prekoracenja = ftell(f_agr);
    ofseti.adresa_slobodnog_prostora = ftell(f_agr);

    upisi_ofsete(ofseti);

    free(listovi);
    if (trenutni_nivo != listovi) {
        free(trenutni_nivo);
    }
    fclose(f_stud);
    fclose(f_pol);
    fclose(f_agr);

    printf(ANSI_COLOR_GREEN"Datoteka agregatne podrske je uspesno kreirana!\n" ANSI_COLOR_RESET);
}

adr_bloka_addr_prek_t nadji_blok(int studentski_broj) {
    FILE* f = fopen("../podaci/index_sek/agrpod.bin", "rb");
    if (!f) {
        printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke za agregatnu podrsku neuspesno!\n" ANSI_COLOR_RESET);
        return (adr_bloka_addr_prek_t){-1, -1, -1};
    }
    ofesti_t ofseti = procitaj_ofsete();
    fseek(f, ofseti.adresa_pocetka_indeksne_zone, SEEK_SET);
    cvor_indeksne_zone_t cvor = {0};
    while (1) {
        fread(&cvor, sizeof(cvor_indeksne_zone_t), 1, f);
        if (cvor.broj_kljuceva == 0) {
            fclose(f);
            return (adr_bloka_addr_prek_t){-1, -1, -1};
        }
        if (cvor.da_li_je_list) {
            printf("DEBUG: Reached leaf node at address %ld\n", ftell(f) - sizeof(cvor_indeksne_zone_t));
            for (int i = 1; i < cvor.broj_kljuceva; i++) {
                if (studentski_broj < cvor.kljucevi[i] && studentski_broj >= cvor.kljucevi[i - 1]) {
                    long adresa = cvor.adrese_dece_blokova[i - 1];
                    fclose(f);
                    return (adr_bloka_addr_prek_t){adresa, cvor.adresa_prvog_prekoracioca[i - 1], ftell(f) - sizeof(cvor_indeksne_zone_t)};
                }
                if (i == cvor.broj_kljuceva - 1) {
                    long adresa = cvor.adrese_dece_blokova[i];
                    fclose(f);
                    return (adr_bloka_addr_prek_t){adresa, cvor.adresa_prvog_prekoracioca[i], ftell(f) - sizeof(cvor_indeksne_zone_t)};
                }
            }
            fclose(f);
            return (adr_bloka_addr_prek_t){-1, -1, -1};
        } else {
            printf("DEBUG: Descending internal node at address %ld\n", ftell(f) - sizeof(cvor_indeksne_zone_t));
            int found = 0;
            for (int i = 1; i < cvor.broj_kljuceva; i++) {
                if ((studentski_broj < cvor.kljucevi[i] && studentski_broj >= cvor.kljucevi[i - 1])) {
                    fseek(f, cvor.adrese_dece_blokova[i - 1], SEEK_SET);
                    found = 1;
                    break;
                }
                if (i == cvor.broj_kljuceva - 1)
                {
                    fseek(f, cvor.adrese_dece_blokova[i], SEEK_SET);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                fseek(f, cvor.adrese_dece_blokova[cvor.broj_kljuceva - 1], SEEK_SET);
                printf("DEBUG: Descending to rightmost child\n");

            }
        }
    }
}

void prikaz_broja_polozenih_ispita_na_osnovu_studentkog_broja(int studentski_broj) {
    FILE* f_agr = fopen("../podaci/index_sek/agrpod.bin", "rb");
    if (!f_agr) {
        printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke za agregatnu podrsku neuspesno!\n" ANSI_COLOR_RESET);
        return;
    }
    adr_bloka_addr_prek_t adresa_bloka = nadji_blok(studentski_broj);
    if (adresa_bloka.addr_blok == -1) {
        printf(ANSI_COLOR_RED"GRESKA: Student sa unetim brojem ne postoji!\n" ANSI_COLOR_RESET);
        fclose(f_agr);
        return;
    }

    blok_primarne_zone_t blok_primarne_zone = {0};
    fseek(f_agr, adresa_bloka.addr_blok, SEEK_SET);
    fread(&blok_primarne_zone, sizeof(blok_primarne_zone), 1, f_agr);

    for (int i = 0; i < FAKTOR_BLOKIRANJA_PRIMARNE_ZONE; i++) {
        if (blok_primarne_zone.slog_primarne_zone[i].studentski_broj == studentski_broj && !blok_primarne_zone.slog_primarne_zone[i].obrisan) {
            printf(ANSI_COLOR_GREEN"Student sa brojem %d ima %d polozenih ispita. Adresa bloka: %ld, Redni broj sloga u bloku: %d\n" ANSI_COLOR_RESET, studentski_broj, blok_primarne_zone.slog_primarne_zone[i].broj_polozenih_ispita, adresa_bloka.addr_blok, i);
            fclose(f_agr);
            return;
        }
    }

    slog_zone_prekoracenja_t slog_prek = {0};
    long trenutna_adresa_prek = adresa_bloka.addr_prvog_prek;
    fseek(f_agr, trenutna_adresa_prek, SEEK_SET);
    do {
        fread(&slog_prek, sizeof(slog_zone_prekoracenja_t), 1, f_agr);

        if (slog_prek.sadrzaj.studentski_broj == studentski_broj && !slog_prek.sadrzaj.obrisan) {
            printf(ANSI_COLOR_GREEN"Student sa brojem %d ima %d polozenih ispita. Adresa bloka: %ld, Nadjeni slog se nalazio u lancu prekoracioca\n" ANSI_COLOR_RESET, studentski_broj, slog_prek.sadrzaj.broj_polozenih_ispita, adresa_bloka.addr_prvog_prek);
            fclose(f_agr);
            return;
        }
    } while (slog_prek.adresa_sledeceg_prekoracioca != -1);

    printf(ANSI_COLOR_RED"GRESKA: Student sa unetim brojem ne postoji!\n" ANSI_COLOR_RESET);
    return;
}

void ispis_stanja_datoteke_indeks_sek() {

    FILE* f_agr = fopen("../podaci/index_sek/agrpod.bin", "rb");
    if (!f_agr) {
        printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke za agregatnu podrsku neuspesno!\n" ANSI_COLOR_RESET);
        return;
    }
    ofesti_t ofseti = procitaj_ofsete();

    printf("\n--- PRIMARNA ZONA ---\n");
    blok_primarne_zone_t blok = {0};
    fseek(f_agr, ofseti.adresa_pocetka_primarne_zone, SEEK_SET);
    long prim_end = ofseti.adresa_kraja_primarne_zone;
    while (ftell(f_agr) < prim_end && fread(&blok, sizeof(blok_primarne_zone_t), 1, f_agr)) {
        printf("Blok @ %ld\n", ftell(f_agr) - sizeof(blok_primarne_zone_t));
        for (int i = 0; i < FAKTOR_BLOKIRANJA_PRIMARNE_ZONE; i++) {
            if (blok.slog_primarne_zone[i].studentski_broj == 0) break;
            printf("  [%d] %s | %s | Dosije: %d | Upis: %d | Godina: %d | Polozeno: %d | Usmeni: %d | Pismeni: %d | Prosek: %.2f | Obrisano: %d\n",
                blok.slog_primarne_zone[i].studentski_broj,
                blok.slog_primarne_zone[i].ime_i_prezime,
                blok.slog_primarne_zone[i].smer,
                blok.slog_primarne_zone[i].broj_dosijea,
                blok.slog_primarne_zone[i].godina_upisa,
                blok.slog_primarne_zone[i].godina_studija,
                blok.slog_primarne_zone[i].broj_polozenih_ispita,
                blok.slog_primarne_zone[i].broj_izlazaka_usmeni,
                blok.slog_primarne_zone[i].broj_izlazaka_pismeni,
                blok.slog_primarne_zone[i].prosecna_ocena,
                blok.slog_primarne_zone[i].obrisan);
        }
    }

    printf("\n--- ZONA INDEKSA ---\n");
    long index_start = ofseti.adresa_kraja_primarne_zone;
    long index_end = ofseti.adresa_pocetka_zone_prekoracenja;
    int max_nodes = 1024;
    cvor_indeksne_zone_t *nodes = malloc(max_nodes * sizeof(cvor_indeksne_zone_t));
    int node_count = 0;
    fseek(f_agr, index_start, SEEK_SET);
    while (ftell(f_agr) < index_end && fread(&nodes[node_count], sizeof(cvor_indeksne_zone_t), 1, f_agr)) {
        node_count++;
        if (node_count >= max_nodes) break;
    }
    int level = 0;
    int nodes_in_level = 1;
    int idx = node_count - 1;
    while (idx >= 0) {
        printf("Level %d:\n", level);
        int next_nodes_in_level = 0;
        for (int i = 0; i < nodes_in_level && idx - i >= 0; i++) {
            cvor_indeksne_zone_t *cvor = &nodes[idx - i];
            printf("  Cvor @ %ld | Kljucevi:", cvor->adresa_cvora);
            for (int k = 0; k < cvor->broj_kljuceva; k++) printf(" %d", cvor->kljucevi[k]);
            printf(" | Potomci:");
            for (int k = 0; k < cvor->broj_kljuceva; k++) printf(" %ld", cvor->adrese_dece_blokova[k]);
            printf(" | List: %d | Prekoracioci: ", cvor->da_li_je_list);
            for (int k = 0; k < cvor->broj_kljuceva; k++) printf(" %ld ", cvor->adresa_prvog_prekoracioca[k]);
            printf("\n");
            if (!cvor->da_li_je_list) next_nodes_in_level += cvor->broj_kljuceva;
        }
        idx -= nodes_in_level;
        nodes_in_level = next_nodes_in_level;
        level++;
    }
    free(nodes);

    printf("\n--- ZONA PREKORACIOCA ---\n");
    slog_zone_prekoracenja_t slog = {0};
    fseek(f_agr, ofseti.adresa_pocetka_zone_prekoracenja, SEEK_SET);
    long overflow_end = ofseti.adresa_slobodnog_prostora;
    while (ftell(f_agr) < overflow_end && fread(&slog, sizeof(slog_zone_prekoracenja_t), 1, f_agr)) {
        printf("Prekoracioc @ %ld: %d | %s | %s | Dosije: %d | Upis: %d | Godina: %d | Polozeno: %d | Usmeni: %d | Pismeni: %d | Prosek: %.2f | Sledeci prekoracioc: %ld\n",
            ftell(f_agr) - sizeof(slog_zone_prekoracenja_t),
            slog.sadrzaj.studentski_broj,
            slog.sadrzaj.ime_i_prezime,
            slog.sadrzaj.smer,
            slog.sadrzaj.broj_dosijea,
            slog.sadrzaj.godina_upisa,
            slog.sadrzaj.godina_studija,
            slog.sadrzaj.broj_polozenih_ispita,
            slog.sadrzaj.broj_izlazaka_usmeni,
            slog.sadrzaj.broj_izlazaka_pismeni,
            slog.sadrzaj.prosecna_ocena,
            slog.adresa_sledeceg_prekoracioca);
    }
    fclose(f_agr);
}

void brisanje_sloga(int studentski_broj) {
    FILE* f_agr = fopen("../podaci/index_sek/agrpod.bin", "rb+");
    if (!f_agr) {
     printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke za agregatnu podrsku neuspesno!\n" ANSI_COLOR_RESET);
     return;
    }
    adr_bloka_addr_prek_t adresa_bloka = nadji_blok(studentski_broj);
    if (adresa_bloka.addr_blok == -1) {
     printf(ANSI_COLOR_RED"GRESKA: Student sa unetim brojem ne postoji!\n" ANSI_COLOR_RESET);
     fclose(f_agr);
     return;
    }

    blok_primarne_zone_t blok_primarne_zone = {0};
    fseek(f_agr, adresa_bloka.addr_blok, SEEK_SET);
    fread(&blok_primarne_zone, sizeof(blok_primarne_zone), 1, f_agr);
    int nadjen = 0;

    for (int i = 0; i < FAKTOR_BLOKIRANJA_PRIMARNE_ZONE; i++) {
        if (blok_primarne_zone.slog_primarne_zone[i].studentski_broj == studentski_broj) {
            nadjen = 1;
            if (blok_primarne_zone.slog_primarne_zone[i].obrisan) {
                printf(ANSI_COLOR_RED"GRESKA: Student sa unetim brojem je vec obrisan!\n" ANSI_COLOR_RESET);fclose(f_agr);return;
            }
            blok_primarne_zone.slog_primarne_zone[i].obrisan = 1;
            fseek(f_agr, -sizeof(blok_primarne_zone), SEEK_CUR);
            fwrite(&blok_primarne_zone, sizeof(blok_primarne_zone), 1, f_agr);
            fflush(f_agr);
            printf(ANSI_COLOR_GREEN"Student sa brojem %d je uspesno obrisan.\n" ANSI_COLOR_RESET, studentski_broj);
            fclose(f_agr);
            return;
        }
    }

    if (!nadjen && adresa_bloka.addr_prvog_prek != -1) {
        slog_zone_prekoracenja_t slog_prek = {0};
        long trenutna_adresa_prek = adresa_bloka.addr_prvog_prek;
        fseek(f_agr, trenutna_adresa_prek, SEEK_SET);
        do {
            fread(&slog_prek, sizeof(slog_zone_prekoracenja_t), 1, f_agr);

            if (slog_prek.sadrzaj.studentski_broj == studentski_broj) {
                nadjen = 1;
                if (slog_prek.sadrzaj.obrisan) {
                    printf(ANSI_COLOR_RED"GRESKA: Student sa unetim brojem je vec obrisan!\n" ANSI_COLOR_RESET);
                    fclose(f_agr);
                    return;
                }
                slog_prek.sadrzaj.obrisan = 1;
                fseek(f_agr, -sizeof(slog_zone_prekoracenja_t), SEEK_CUR);
                fwrite(&slog_prek, sizeof(slog_zone_prekoracenja_t), 1, f_agr);
                fflush(f_agr);
                printf(ANSI_COLOR_GREEN"Student sa brojem %d je uspesno obrisan.\n" ANSI_COLOR_RESET, studentski_broj);
                fclose(f_agr);
                return;
            }
            trenutna_adresa_prek = slog_prek.adresa_sledeceg_prekoracioca;
        } while (trenutna_adresa_prek != -1);
    } else if (!nadjen) {
        printf(ANSI_COLOR_RED"GRESKA: Student sa unetim brojem ne postoji!\n" ANSI_COLOR_RESET);
        fclose(f_agr);
        return;
    }


}

void traverse_in_order(long node_address, FILE* f_agr, int* brojac_pronadjenih) {
        if (node_address == -1) return;

        cvor_indeksne_zone_t cvor = {0};
        fseek(f_agr, node_address, SEEK_SET);
        fread(&cvor, sizeof(cvor_indeksne_zone_t), 1, f_agr);

        if (cvor.da_li_je_list) {
            for (int i = 0; i < cvor.broj_kljuceva; i++) {
                long block_address = cvor.adrese_dece_blokova[i];

                blok_primarne_zone_t blok = {0};
                long saved_pos = ftell(f_agr);
                fseek(f_agr, block_address, SEEK_SET);
                fread(&blok, sizeof(blok_primarne_zone_t), 1, f_agr);

                for (int j = 0; j < FAKTOR_BLOKIRANJA_PRIMARNE_ZONE; j++) {
                    slog_primarne_zone_t* slog = &blok.slog_primarne_zone[j];

                    if (slog->studentski_broj == 0 ) {
                        break;
                    }
                    if (slog->obrisan) {
                        continue;
                    }

                    if (slog->broj_izlazaka_usmeni == slog->broj_izlazaka_pismeni) {
                        (*brojac_pronadjenih)++;
                        printf("Student %d:\n", *brojac_pronadjenih);
                        printf("  Studentski broj: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", slog->studentski_broj);
                        printf("  Ime i prezime: " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "\n", slog->ime_i_prezime);
                        printf("  Smer: " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "\n", slog->smer);
                        printf("  Broj dosijea: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", slog->broj_dosijea);
                        printf("  Godina upisa: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", slog->godina_upisa);
                        printf("  Godina studija: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", slog->godina_studija);
                        printf("  Broj polozenih ispita: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", slog->broj_polozenih_ispita);
                        printf("  Broj izlazaka usmeni: " ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET "\n", slog->broj_izlazaka_usmeni);
                        printf("  Broj izlazaka pismeni: " ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET "\n", slog->broj_izlazaka_pismeni);
                        printf("  Prosecna ocena: " ANSI_COLOR_CYAN "%.2f" ANSI_COLOR_RESET "\n", slog->prosecna_ocena);
                        printf("--------------------------------------------------------------------\n");
                    }
                }

                if (cvor.adresa_prvog_prekoracioca[i] != -1) {
                    long overflow_addr = cvor.adresa_prvog_prekoracioca[i];
                    slog_zone_prekoracenja_t slog_overflow = {0};

                    while (overflow_addr != -1) {
                        fseek(f_agr, overflow_addr, SEEK_SET);
                        fread(&slog_overflow, sizeof(slog_zone_prekoracenja_t), 1, f_agr);

                        if (slog_overflow.sadrzaj.broj_izlazaka_usmeni == slog_overflow.sadrzaj.broj_izlazaka_pismeni) {
                            (*brojac_pronadjenih)++;
                            printf("Student %d (iz zone prekoracenja):\n", *brojac_pronadjenih);
                            printf("  Studentski broj: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", slog_overflow.sadrzaj.studentski_broj);
                            printf("  Ime i prezime: " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "\n", slog_overflow.sadrzaj.ime_i_prezime);
                            printf("  Smer: " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "\n", slog_overflow.sadrzaj.smer);
                            printf("  Broj dosijea: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", slog_overflow.sadrzaj.broj_dosijea);
                            printf("  Godina upisa: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", slog_overflow.sadrzaj.godina_upisa);
                            printf("  Godina studija: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", slog_overflow.sadrzaj.godina_studija);
                            printf("  Broj polozenih ispita: " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", slog_overflow.sadrzaj.broj_polozenih_ispita);
                            printf("  Broj izlazaka usmeni: " ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET "\n", slog_overflow.sadrzaj.broj_izlazaka_usmeni);
                            printf("  Broj izlazaka pismeni: " ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET "\n", slog_overflow.sadrzaj.broj_izlazaka_pismeni);
                            printf("  Prosecna ocena: " ANSI_COLOR_CYAN "%.2f" ANSI_COLOR_RESET "\n", slog_overflow.sadrzaj.prosecna_ocena);
                            printf("  Sledeci prekoracioc: " ANSI_COLOR_CYAN "%ld" ANSI_COLOR_RESET "\n", slog_overflow.adresa_sledeceg_prekoracioca);
                            printf("--------------------------------------------------------------------\n");
                        }

                        overflow_addr = slog_overflow.adresa_sledeceg_prekoracioca;
                    }
                }

                fseek(f_agr, saved_pos, SEEK_SET);
            }
        } else {
            for (int i = 0; i < cvor.broj_kljuceva; i++) {
                traverse_in_order(cvor.adrese_dece_blokova[i], f_agr, brojac_pronadjenih);
            }
        }
    }

void prikaz_svih_studenata_usmeni_eq_pisemni() {
    FILE* f_agr = fopen("../podaci/index_sek/agrpod.bin", "rb");
    if (!f_agr) {
        printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke za agregatnu podrsku neuspesno!\n" ANSI_COLOR_RESET);
        return;
    }

    ofesti_t ofseti = procitaj_ofsete();
    int brojac_pronadjenih = 0;

    printf("\n=== STUDENTI SA JEDNAKIM BROJEM IZLAZAKA NA USMENI I PISMENI ===\n");
    printf("--------------------------------------------------------------------\n");

    traverse_in_order(ofseti.adresa_pocetka_indeksne_zone, f_agr, &brojac_pronadjenih);

    if (brojac_pronadjenih == 0) {
        printf(ANSI_COLOR_YELLOW "Nema studenata sa jednakim brojem izlazaka na usmeni i pismeni deo ispita.\n" ANSI_COLOR_RESET);
    } else {
        printf(ANSI_COLOR_GREEN "Ukupno pronadjeno %d studenata sa jednakim brojem izlazaka na usmeni i pismeni deo ispita.\n" ANSI_COLOR_RESET, brojac_pronadjenih);
    }

    fclose(f_agr);
}

