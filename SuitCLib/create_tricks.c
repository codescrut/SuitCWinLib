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
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void create_tricks( int d )
{
    int          i, lead[4], p, q, t = st[d].t;
    COLOR        c;
    struct INDEX *lindex;
    struct SCARD *sc, *tc;

    //  collect the remaining cards; complete the current cards with the initial SCARD sequences
    for ( p = 0; p < 4; p++ ) {
        lead[p] = NO;
        brd[t][p] = RENONCE;
        sc = &scards[0][p];
        lindex = wnode[d-1]->i;
        while( lindex ) {
            brd[t][p] |= lindex->c[p];
            lindex = lindex->r;
        }
        for ( i = 1; i <= sc->n; i++ )
            if ( brd[t][p] & sc->c[i] )
                brd[t][p] |= sc->c[i];
    }

    //  determine who can lead to this trick
    if ( t == 1 && leader != noplayer )
        lead[leader] = YES;
    else if ( throwin != noplayer && throwin == w[t-1] && brd[t][throwin] >> 2 )
        lead[throwin] = YES;
    else if ( defence ) {
        lead[west] = brd[t][west] && ( w[t-1] == west || nfree[t-1][west] );
        lead[east] = brd[t][east] && ( w[t-1] == east || nfree[t-1][east] );
    }
    else {
        lead[north] = brd[t][north] && ( w[t-1] == north || nfree[t-1][north] );
        lead[south] = brd[t][south] && ( w[t-1] == south || nfree[t-1][south] );
    }

    //  no more tricks are to be played
    if ( t == ntricks || ! ( lead[west] || lead[north] || lead[east] || lead[south] ) ) {

        wnode[d-1]->term = YES;

        if ( ( lead[west] || lead[north] || lead[east] || lead[south] ) && t == ntricks ) {

            ns[t] = ns[t-1];
            p = brd[t][north] > brd[t][east] ? north : east;
            p = brd[t][south] > brd[t][p]    ? south : p;
            p = brd[t][west]  > brd[t][p]    ? west  : p;
            for ( q = 0; q < 4; q++ )
                if ( nruff[t-1][q] && ! brd[t][q] )
                    p = q;
            ns[t] += defence ? NS(p) ? 0 : -1 : NS(p) ? 1 : 0;

            lindex = wnode[d-1]->i;
            while( lindex ) {
                lindex->min = ns[t];
                lindex->max = lindex->min;
                lindex = lindex->r;
            }
        }

        else if ( throwin != noplayer ) {

            ns[t] = ns[t-1] + tricklimit - t + 1;
            lindex = wnode[d-1]->i;
            while( lindex ) {
                lindex->min = ns[t];
                lindex->max = lindex->min;
                lindex = lindex->r;
            }
        }

        return;
    }

    //  build the SCARD sequences for the current trick
    for ( p = 0; p < 4; p++ ) {

        //  build the SCARD sequences
        sc = &scards[0][p];
        tc = &scards[t][p];
        tc->n = -1;
        for ( i = 1; i <= sc->n; i++ )
            if ( sc->c[i] & brd[t][p] )
                tc->c[++tc->n] = sc->c[i];

        //  add renonces and ruffs
        tc->m = tc->n;
        if ( ncardsmin[p] < t ) {
            tc->c[++tc->m] = (COLOR) 0;
            if ( nruff[t][p] )
                tc->c[++tc->m] = addcard(ruff);
        }
    }

    //  lead from each hand in turn
    create_ply1( d, lead );
}
