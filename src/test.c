#include "../include/student.h"
#include "../include/polaganje_ispita.h"
#include "../include/argpod.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ubaci_pregeniranse_studente_sturente() {
    printf("Unesite naziv datoteke za ubacivanje pregeniranih studenata: ");
    char naziv[50];
    char puna_putanja[100];
    scanf("%s", naziv);

    sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_STUDENT, naziv);
    FILE *f = fopen(puna_putanja, "rb+");
    if (!f) {
        printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke neuspesno!\n" ANSI_COLOR_RESET);
        return;
    }

    student_t studenti[20] = {
        {1, "Ana Petrovic", "SIIT", 1001, 2023, 1},
        {2, "Marko Nikolic", "SIIT", 1002, 2023, 1},
        {3, "Milica Stojanovic", "SIIT", 1003, 2023, 1},
        {4, "Stefan Jovanovic", "SIIT", 1004, 2022, 2},
        {5, "Jovana Milic", "SIIT", 1005, 2022, 2},
        {6, "Nikola Radovic", "SIIT", 1006, 2022, 2},
        {7, "Tamara Pantic", "SIIT", 1007, 2021, 3},
        {8, "Aleksandar Radic", "SIIT", 1008, 2021, 3},
        {9, "Jelena Mitrovic", "SIIT", 1009, 2021, 3},
        {10, "Milan Savic", "SIIT", 1010, 2020, 4},
        {11, "Sara Bogdanovic", "SIIT", 1011, 2020, 4},
        {12, "Luka Stankovic", "SIIT", 1012, 2019, 5},
        {13, "Marija Djordjevic", "SIIT", 1013, 2019, 5},
        {14, "Petar Lazic", "SIIT", 1014, 2018, 6},
        {15, "Nevena Filipovic", "SIIT", 1015, 2017, 7},
        {16, "Marija Mirosavljevic", "SIIT", 1, 2019, 5},
        {17, "Marko Stojanac", "SIIT", 2, 2019, 5},
        {18, "Petar Strahinjic", "SIIT", 3, 2018, 6},
        {19, "Nevena Filipovic", "SIIT", 4, 2017, 7},
        {20, "Milos Majstorovic", "SIIT", 5, 2024, 1}
    };

    printf("Ubacivanje pregeniranih studenata...\n");

    for (int i = 0; i < 20; i++) {
        FILE *f = fopen(puna_putanja, "rb+");
        if (!f) {
            printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke neuspesno za studenta %d!\n" ANSI_COLOR_RESET, i+1);
            return;
        }

        upis_novog_studenta(studenti[i], f);
        printf("Ubacen student %d/15: %s (ID: %d)\n", i+1, studenti[i].ime_i_prezime, studenti[i].studentski_broj);
    }

    printf(ANSI_COLOR_GREEN"Uspesno ubaceno 15 pregeniranih studenata!\n" ANSI_COLOR_RESET);
}

void ubaci_pregeniranse_polaganje_ispita() {
    printf("Unesite naziv datoteke za ubacivanje pregeniranih polaganja ispita: ");
    char naziv[50];
    char puna_putanja[100];
    scanf("%s", naziv);

    sprintf(puna_putanja, "%s%s", PUTANJA_FOLDERA_POLAGANJE, naziv);
    FILE *f = fopen(puna_putanja, "rb+");
    if (!f) {
        printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke neuspesno!\n" ANSI_COLOR_RESET);
        return;
    }

    polaganje_ispita_t ispiti[62] = {
        // Student 1 - 4 exams (2 oral, 2 written)
        // {1001, 1, "Matematika 1", 8, 45, 35, "DA"},
        // {1002, 1, "Fizika", 6, 30, 20, "NE"},
        // {1003, 1, "Programiranje 1", 9, 50, 40, "DA"},
        // {1004, 1, "Engleski jezik", 7, 35, 25, "NE"},
        //
        // // Student 2 - 4 exams (2 oral, 2 written)
        // {1005, 2, "Matematika 1", 5, 25, 15, "DA"},
        // {1006, 2, "Fizika", 10, 55, 45, "NE"},
        // {1007, 2, "Programiranje 1", 8, 40, 35, "DA"},
        // {1008, 2, "Web dizajn", 7, 45, 25, "NE"},
        //
        // // Student 3 - 4 exams (3 oral, 1 written)
        // {1009, 3, "Matematika 1", 9, 50, 40, "DA"},
        // {1010, 3, "Algoritmi", 6, 30, 25, "DA"},
        // {1011, 3, "Baze podataka", 8, 45, 35, "DA"},
        // {1012, 3, "Operativni sistemi", 7, 40, 25, "NE"},
        //
        // // Student 4 - 5 exams (3 oral, 2 written)
        // {1013, 4, "Objektno programiranje", 9, 50, 40, "DA"},
        // {1014, 4, "Strukture podataka", 8, 45, 30, "NE"},
        // {1015, 4, "Racunarske mreze", 10, 60, 40, "DA"},
        // {1016, 4, "Softversko inzenjerstvo", 7, 35, 28, "NE"},
        // {1017, 4, "Statistika", 6, 30, 20, "DA"},
        //
        // // Student 5 - 4 exams (2 oral, 2 written)
        // {1018, 5, "Matematika 2", 8, 42, 33, "DA"},
        // {1019, 5, "Fizika 2", 7, 38, 27, "NE"},
        // {1020, 5, "Java programiranje", 9, 48, 42, "DA"},
        // {1021, 5, "XML tehnologije", 6, 32, 23, "NE"},
        //
        // // Student 6 - 4 exams (1 oral, 3 written)
        // {1022, 6, "Diskretna matematika", 8, 44, 36, "DA"},
        // {1023, 6, "Programski jezici", 9, 52, 38, "NE"},
        // {1024, 6, "Mikroprocesorski sistemi", 7, 36, 29, "NE"},
        // {1025, 6, "Elektronika", 6, 28, 22, "NE"},
        //
        // // Student 7 - 4 exams (4 oral, 0 written)
        // {1026, 7, "Upravljanje projektima", 10, 58, 42, "DA"},
        // {1027, 7, "Ekonomija", 8, 43, 37, "DA"},
        // {1028, 7, "Psihologija", 9, 49, 41, "DA"},
        // {1029, 7, "Sociologija", 7, 39, 31, "DA"},
        //
        // // Student 8 - 4 exams (2 oral, 2 written)
        // {1030, 8, "Analiza algoritama", 9, 51, 39, "DA"},
        // {1031, 8, "Kompajleri", 8, 46, 34, "NE"},
        // {1032, 8, "Vestacka inteligencija", 10, 57, 43, "DA"},
        // {1033, 8, "Robotika", 7, 41, 26, "NE"},
        //
        // // Student 9 - 5 exams (2 oral, 3 written)
        // {1034, 9, "Mobilne aplikacije", 8, 47, 33, "DA"},
        // {1035, 9, "Cloud computing", 9, 53, 37, "NE"},
        // {1036, 9, "Cyber security", 10, 59, 41, "NE"},
        // {1037, 9, "IoT tehnologije", 7, 38, 32, "DA"},
        // {1038, 9, "Big Data", 6, 34, 26, "NE"},
        //
        // // Student 10 - 4 exams (3 oral, 1 written)
        // {1039, 10, "Machine Learning", 9, 54, 36, "DA"},
        // {1040, 10, "Deep Learning", 8, 48, 32, "DA"},
        // {1041, 10, "Computer Vision", 10, 60, 40, "DA"},
        // {1042, 10, "Natural Language Processing", 7, 42, 28, "NE"},
        //
        // // Student 11 - 4 exams (1 oral, 3 written)
        // {1043, 11, "Blockchain tehnologije", 8, 45, 35, "DA"},
        // {1044, 11, "Kvantno racunarstvo", 9, 52, 38, "NE"},
        // {1045, 11, "Bioinformatika", 7, 39, 31, "NE"},
        // {1046, 11, "Geoinformatika", 6, 33, 27, "NE"},
        //
        // // Student 12 - 4 exams (2 oral, 2 written)
        // {1047, 12, "Teorija informacija", 10, 56, 44, "DA"},
        // {1048, 12, "Kriptografija", 9, 50, 40, "NE"},
        // {1049, 12, "Teorija grafova", 8, 44, 36, "DA"},
        // {1050, 12, "Kombinatorika", 7, 37, 33, "NE"},
        //
        // // Student 13 - 4 exams (3 oral, 1 written)
        // {1051, 13, "Distribuirani sistemi", 9, 49, 41, "DA"},
        // {1052, 13, "Paralelno programiranje", 8, 46, 34, "DA"},
        // {1053, 13, "Grid computing", 10, 58, 42, "DA"},
        // {1054, 13, "High Performance Computing", 7, 40, 30, "NE"},
        //
        // // Student 14 - 4 exams (1 oral, 3 written)
        // {1055, 14, "Digitalna obrada signala", 8, 43, 37, "DA"},
        // {1056, 14, "Kompjuterska grafika", 9, 51, 39, "NE"},
        // {1057, 14, "Multimedia sistemi", 7, 38, 32, "NE"},
        // {1058, 14, "Game Development", 6, 35, 25, "NE"},
        //
        // // Student 15 - 4 exams (2 oral, 2 written)
        // {1059, 15, "Diplomski rad", 10, 60, 40, "DA"},
        // {1060, 15, "Praksa u industriji", 9, 55, 35, "NE"},
        // {1061, 15, "Seminarski rad", 8, 48, 32, "DA"},
        // {1062, 15, "Zavrsni projekat", 10, 58, 42, "NE"},

        {1061, 17, "Seminarski rad", 8, 48, 32, "DA"},
        {1062, 17, "Zavrsni projekat", 10, 58, 42, "NE"}
    };

    printf("Ubacivanje pregeniranih polaganja ispita...\n");

    for (int i = 0; i < 1; i++) {
        FILE *f = fopen(puna_putanja, "rb+");
        if (!f) {
            printf(ANSI_COLOR_RED"GRESKA: otvaranje datoteke neuspesno za polaganje %d!\n" ANSI_COLOR_RESET, i+1);
            return;
        }

        upis_novo_polaganje(ispiti[i], f);
        printf("Ubaceno polaganje %d/62: %s - %s (ID: %d, Student: %d)\n",
               i+1, ispiti[i].naziv_predmeta, ispiti[i].usmeni, ispiti[i].id, ispiti[i].studentski_broj);
    }

    printf(ANSI_COLOR_GREEN"Uspesno ubaceno 62 pregeniranih polaganja ispita za 15 studenata!\n" ANSI_COLOR_RESET);
}
