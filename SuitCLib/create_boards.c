//  name:
//      create_boards
//
//  purpose:
//      create_boards is the driver program for the construction of the
//      full board set
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

int count(COLOR cards)
{
    int           i = 0;
    while (cards) {
        i++;
        cards ^= low_c(cards);
    }
    return (i);
}

int create_boards(char* output)
{
    int          i, k, l, p, q;
    COLOR        cards, fixed;
    struct INDEX* lindex = NIL;
    struct SCARD* sc;

    //  check for duplicate cards
    brd[0][east] = brd[0][west];
    if (brd[0][north] & brd[0][south] || brd[0][north] & brd[0][west] || brd[0][south] & brd[0][west]) {
        printf("ERROR: The input contains duplicate cards\n");
        return 1;
    }

    //  determine the number of cards in each hand
    for (p = 0; p < 4; p++)
        ncards[p] = count(brd[0][p]);
    ncardsmin[east] = MAX(ncardsmin[east], MAX(count(eastfixed), ncards[west] - westvac));
    ncardsmin[west] = MAX(ncardsmin[west], MAX(count(westfixed), ncards[east] - eastvac));
    ncards[east] = ncards[east] - ncardsmin[west];
    ncards[west] = ncards[west] - ncardsmin[east];

    //  determine the number of tricks to be played and the maximum ply number to be printed
    ntricks = ewwinners ? MAX(ncards[east], ncards[west])
        : defence ? MIN(MAX(ncards[north], ncards[south]), MAX(ncards[east], ncards[west]))
        : MAX(ncards[north], ncards[south]);
    tricklimit = MIN(tricklimit, ntricks);
    maxprint = MAX(ncards[north], ncards[south]) - 1;
    maxprint = MIN(maxprint, count(brd[0][west]) / 2);
    maxprint = tricklimit ? MAX(maxprint, 1) : 0;
    maxprint *= 4;

    //  determine the SCARD sequences for all players
    for (p = 0; p < 4; p++) {

        q = NO;
        sc = &scards[0][p];
        sc->c[sc->n = 0] = (COLOR)0;
        cards = brd[0][west] | brd[0][north] | brd[0][east] | brd[0][south];
        cards ^= cards & brd[0][p];

        for (i = ace; i >= two; i--) {
            if (brd[0][p] & addcard(i)) {
                if (q)
                    sc->c[sc->n] |= addcard(i);
                else {
                    q = YES;
                    sc->c[++sc->n] = addcard(i);
                }
            }
            else if (cards & addcard(i))
                q = NO;
        }
    }

    //  translate the lead for west in canonical order
    if (leader == west && opening_lead != 0x7fff) {
        sc = &scards[0][west];
        for (i = 0; i <= sc->n; i++)
            if (opening_lead & sc->n)
                for (k = two; k < ace; k++)
                    if (addcard(k) & opening_lead && addcard(k + 1) & sc->c[i])
                        opening_lead = (opening_lead ^ addcard(k)) | addcard(k + 1);
    }

    //  translate the lead for east in canonical order
    if (leader == east && opening_lead != 0x7fff) {
        sc = &scards[0][east];
        for (i = 0; i <= sc->n; i++)
            if (opening_lead & sc->c[i])
                for (k = ace; k > two; k--)
                    if (addcard(k) & opening_lead && addcard(k - 1) & sc->c[i])
                        opening_lead = (opening_lead ^ addcard(k)) | addcard(k - 1);
    }

    //  translate the fixed cards for west in canonical order
    if (westfixed) {
        fixed = RENONCE;
        sc = &scards[0][west];
        for (i = 0; i <= sc->n; i++) {
            if (westfixed & sc->c[i]) {
                for (k = ace; k >= two; k--) {
                    if (addcard(k) & westfixed && addcard(k) & sc->c[i]) {
                        l = k;
                        while (addcard(l + 1) & sc->c[i] && !(addcard(l + 1) & fixed))
                            l++;
                        fixed |= addcard(l);
                    }
                }
            }
        }
        westfixed = fixed;
    }

    //  translate the fixed cards for east in canonical order
    if (eastfixed) {
        fixed = RENONCE;
        sc = &scards[0][east];
        for (i = 0; i <= sc->n; i++) {
            if (eastfixed & sc->c[i]) {
                for (k = two; k <= ace; k++) {
                    if (addcard(k) & eastfixed && addcard(k) & sc->c[i]) {
                        l = k;
                        while (addcard(l - 1) & sc->c[i] && !(addcard(l - 1) & fixed))
                            l--;
                        fixed |= addcard(l);
                    }
                }
            }
        }
        eastfixed = fixed;
    }

    //  generate the full set of boards
    if (create_all_boards() != 0) {
        output_error(output, "No boards are left");
        return -1;
    }

    //  initialise the root of the tree
    wnode[0]->d = NIL;
    wnode[0]->l = NIL;
    wnode[0]->r = NIL;
    wnode[0]->i = nextindex;
    wnode[0]->c = RENONCE;
    wnode[0]->e_down = 0;
    wnode[0]->e_up = 0;
    wnode[0]->ns = ewwinners ? tricklimit : 0;
    wnode[0]->p = defence ? north : west;
    wnode[0]->w_c = RENONCE;
    wnode[0]->w_p = wnode[0]->p;
    wnode[0]->term = NO;

    //  create the index list for the root
    for (i = 0; i < boardcount; i++) {
        bump_index(lindex);
        lindex->r = nextindex;
        lindex->i = i;
        lindex->min = 0;
        lindex->max = tricklimit;
        for (p = 0; p < 4; p++)
            lindex->c[p] = board[p][i];
    }

    lindex->r = NIL;

    //  generate the board header
    create_board_header();

    //  print the information about the suit combination and restrictions
    if (!SUITCAPP) print_suitc();

    return 0;
}
