
#ifndef BP1_PROJ__COPY__TUPLE_H

    typedef struct {
        long adr_bloka_pozicije_upisa;
        int redni_broj_pozicije_upisa_u_bloku;
        int nadjena_pozicja;
        int postoji;
        int treba_novi_blok_za_kraj_datoteke;
        int treba_pomeranje;
    } tuple_t;

#define BP1_PROJ__COPY__TUPLE_H

#endif //BP1_PROJ__COPY__TUPLE_H