//  name:
//      create_board_header
//
//  purpose:
//      create_board_header constructs the description of the header
//      info for the boards
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void create_board_header(void)
{
    int          cnt, i, j, n;
    char         out[NCARDS + 1];
    COLOR        allc, c;
    static const char* symbols[] = { "-123456789TJQKA", "-123456789TBVHA" };
    struct INDEX* lindex;
    struct SCARD* sc;

    i = 0;
    lindex = wnode[0]->i;
    while (lindex) {

        //  handle the west cards
        cnt = 0;
        allc = lindex->c[west];
        while (allc) {
            c = low_c(allc);
            for (j = 0; j <= NCARDS + 1; j++) {
                if (addcard(j) & c) {
                    if (!printscard) {
                        sc = &scards[0][west];
                        for (n = 1; n <= sc->n; n++) {
                            if (sc->c[n] & addcard(j)) {
                                while (sc->c[n] & addcard(j + 1))
                                    j++;
                                break;
                            }
                        }
                    }
                    out[++cnt] = symbols[printoption & 0x1][j];
                    break;
                }
            }
            allc ^= c;
        }

        for (j = 0; j < cnt; j++)
            b_print[j][i] = out[cnt - j];
        b_print[cnt++][i] = ((i == boardcount - 1) && !lindex->c[west] ? '-' : ' ');

        //  handle the east cards
        allc = lindex->c[east];
        while (allc) {
            c = low_c(allc);
            for (j = 0; j <= NCARDS + 1; j++) {
                if (addcard(j) & c) {
                    if (!printscard) {
                        sc = &scards[0][west];
                        for (n = 1; n <= sc->n; n++) {
                            if (sc->c[n] & addcard(j)) {
                                while (sc->c[n] & addcard(j + 1))
                                    j++;
                                break;
                            }
                        }
                    }
                    b_print[cnt++][i] = symbols[printoption & 0x1][j];
                    break;
                }
            }
            allc ^= c;
        }
        if ((i == 0) && !lindex->c[east])
            b_print[cnt - 1][i] = '-';
        z_count = cnt;

        i++;
        lindex = lindex->r;
    }
}
