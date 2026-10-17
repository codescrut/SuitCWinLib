//  name:
//      create_tricks
//
//  purpose:
//      create_tricks is the driver program for creating tricks. First it
//      checks the if still more tricks have to be played or if restrictions
//      apply on who can lead. If more tricks have to be played it creates
//      the setup for the next trick.
//
//  parameters:
//      d               in      the depth in the tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"
int count(COLOR cards);

int create_tricks(int d)
{
    int          i, lead[4], p, q, t = (d + 3) / 4, wi;
    struct INDEX* lindex;
    struct SCARD* sc, * tc;

    //  collect the remaining cards; complete the current cards with the initial SCARD sequences
    for (p = 0; p < 4; p++) {
        lead[p] = NO;
        brd[t][p] = RENONCE;
        sc = &scards[0][p];
        lindex = wnode[d - 1]->i;
        while (lindex) {
            brd[t][p] |= lindex->c[p];
            lindex = lindex->r;
        }
        for (i = 1; i <= sc->n; i++)
            if (brd[t][p] & sc->c[i])
                brd[t][p] |= sc->c[i];
    }

    //  we need additional checks to handle the nfree condition correctly
    //  handle defensive plays with no cards left in north and south
    if (ewwinners &&
        (brd[t][north] || brd[t][south]) == RENONCE &&
        !(nruff[north] || nruff[south]) &&
        MIN(nfree[t][west], nfree[t][east]) > tricklimit - t &&
        (t > 1 || buildtree == 0)) {
        lindex = wnode[d - 1]->i;
        while (lindex) {
            lindex->max -= MAX(count(lindex->c[west]), count(lindex->c[east]));
            lindex->min = lindex->max;
            lindex = lindex->r;
        }
        wnode[d - 1]->term = YES;;
        return 0;
    }

    //  we need additional checks to handle the nfree condition correctly
    //  handle plays with no cards left in west and east
    if (defence == NO &&
        (brd[t][west] | brd[t][east]) == RENONCE &&
        !(nruff[west] || nruff[east]) &&
        MIN(nfree[t][north], nfree[t][south]) > tricklimit - t &&
        (t > 1 || buildtree == 0)) {
        lindex = wnode[d - 1]->i;
        while (lindex) {
            lindex->max += MIN(MAX(ncards[north], ncards[south]), tricklimit) - t + 1;
            lindex->min = lindex->max;
            lindex = lindex->r;
        }
        wnode[d - 1]->term = YES;;
        return 0;
    }

    //  determine who can lead to this trick
    if (t == 1 && leader != noplayer)
        lead[leader] = YES;
    else if (throwin & (1 << wnode[d - 1]->w_p) && brd[t][wnode[d - 1]->w_p])
        lead[wnode[d - 1]->w_p] = YES;
    else if (defence) {
        lead[west] = brd[t][west] && (wnode[d - 1]->w_p == west || t == 1 || nfree[t - 1][west]);
        lead[east] = brd[t][east] && (wnode[d - 1]->w_p == east || t == 1 || nfree[t - 1][east]);
    }
    else {
        lead[north] = brd[t][north] && (wnode[d - 1]->w_p == north || t == 1 || nfree[t - 1][north]);
        lead[south] = brd[t][south] && (wnode[d - 1]->w_p == south || t == 1 || nfree[t - 1][south]);
    }

    //  no more tricks can be played
    if (!(lead[west] || lead[north] || lead[east] || lead[south])) {
        wnode[d - 1]->term = YES;
        return 0;
    }

    //  we need additional checks to handle the nfree and nruff conditions correctly
    //  handle the last trick to shortcut the program
    if (t == ntricks && t > 1 && !(nruff[t - 1][west] || nruff[t - 1][north] || nruff[t - 1][east] || nruff[t - 1][south])) {

        p = brd[t][north] > brd[t][east] ? north : east;
        p = brd[t][south] > brd[t][p] ? south : p;
        p = brd[t][west] > brd[t][p] ? west : p;

        lindex = wnode[d - 1]->i;
        while (lindex) {
            wi = p;
            for (q = 0; q < 4; q++)
                if (nruff[t - 1][q] && !lindex->c[q])
                    wi = q;
            lindex->min = wnode[d - 1]->ns + (ewwinners ? NS(wi) ? 0 : -1 : NS(wi) ? 1 : 0);
            lindex->max = lindex->min;
            lindex = lindex->r;
        }

        wnode[d - 1]->e_up = wnode[d - 1]->e_down;
        if (defence == NO) {
            for (q = 1; q <= 3; q += 2)
                if (q == wnode[d - 1]->w_p && brd[t][q])
                    wnode[d - 1]->e_up |= 1;
        }
        wnode[d - 1]->term = YES;
        return 0;
    }

    //  build the SCARD sequences for the current trick
    for (p = 0; p < 4; p++) {

        //  build the SCARD sequences
        sc = &scards[0][p];
        tc = &scards[t][p];
        tc->n = -1;
        for (i = 1; i <= sc->n; i++)
            if (sc->c[i] & brd[t][p])
                tc->c[++tc->n] = sc->c[i];

        //  add renonces and ruffs
        tc->m = tc->n;
        if (ncardsmin[p] < t) {
            tc->c[++tc->m] = (COLOR)0;
            if (nruff[t - 1][p])
                tc->c[++tc->m] = addcard(ruff);
        }
    }

    //  lead from each hand in turn
    if (create_ply1(d, lead) != 0) {
		return -1;
    };
    return 0;
}
