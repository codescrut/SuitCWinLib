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
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void create_boards(char* output)
{
    int          ec, i, k, l, p, q, wc;
    COLOR        cards = 0, fixed = 0;
    struct INDEX *lindex = NULL;
    struct SCARD *sc;

    //  check for duplicate cards
    brd[0][east] = brd[0][west];
    if ( brd[0][north] & brd[0][south] || brd[0][north] & brd[0][west] || brd[0][south] & brd[0][west] ) {
        printf( "ERROR: The input contains duplicate cards\n" );
        exit( 1 );
    }

    //  determine the number of cards in each hand
    for ( p = 0; p < 4; p++ ) {
        ncards[p] = 0;
        cards = brd[0][p];
        while ( cards ) {
            ncards[p]++;
            cards ^= low_c( cards );
        }
    }

    //  determine the number of tricks to be played and the maximum ply number to be printed
    ntricks = defence ? ncards[west] - MIN( ncardsmin[west], ncardsmin[east] ) : MAX( ncards[north], ncards[south] );
    tricklimit = MIN( tricklimit, ntricks );
    maxply = MAX( 0, 4 * MIN( ntricks - 1, ncards[west] / 2 ) );

    //  determine the minimum number of cards in each hand
    ec = 0;
    cards = eastfixed;
    while ( cards ) {
        ec++;
        cards ^= low_c(cards);
    }
    wc = 0;
    cards = westfixed;
    while ( cards ) {
        wc++;
        cards ^= low_c(cards);
    }
    ncardsmin[east] = MAX( ncardsmin[east], ec );
    ncardsmin[west] = MAX( ncardsmin[west], wc );
    ncardsmin[north] = ncards[north];
    ncardsmin[south] = ncards[south];
    ncards[east] = ncards[west] - ncardsmin[west];
    ncards[west] = ncards[west] - ncardsmin[east];

    //  determine the SCARD sequences for all players
    for ( p = 0; p < 4; p++ ) {

        q = NO;
        sc = &scards[0][p];
        sc->c[sc->n=0] = (COLOR) 0;
        cards = brd[0][west] | brd[0][north] | brd[0][east] | brd[0][south];
        cards ^= cards & brd[0][p];

        for ( i = ace; i >= two; i-- ) {
            if ( brd[0][p] & addcard(i) ) {
                if ( q )
                    sc->c[sc->n] |= addcard(i);
                else {
                    q = YES;
                    sc->c[++sc->n] = addcard(i);
                }
            }
            else if ( cards & addcard(i) )
                q = NO;
        }
    }

    //  translate the lead for west in canonical order
    if ( leader == west && opening_lead != 0x7fff ) {
        sc = &scards[0][west];
        for ( i = 0; i <= sc->n; i++ )
            if ( opening_lead & sc->n )
                for ( k = two; k < ace; k++ )
                    if ( addcard(k) & opening_lead && addcard(k+1) & sc->c[i] )
                        opening_lead = ( opening_lead ^ addcard(k) ) | addcard(k+1);
    }

    //  translate the lead for east in canonical order
    if ( leader == east && opening_lead != 0x7fff ) {
        sc = &scards[0][east];
        for ( i = 0; i <= sc->n; i++ )
            if ( opening_lead & sc->c[i] )
                for ( k = ace; k > two; k-- )
                    if ( addcard(k) & opening_lead && addcard(k-1) & sc->c[i] )
                        opening_lead = ( opening_lead ^ addcard(k) ) | addcard(k-1);
    }

    //  translate the fixed cards for west in canonical order
    if ( westfixed ) {
        fixed = RENONCE;
        sc = &scards[0][west];
        for ( i = 0; i <= sc->n; i++ ) {
            if ( westfixed & sc->c[i] ) {
                for ( k = ace; k >= two; k-- ) {
                    if ( addcard(k) & westfixed && addcard(k) & sc->c[i] ) {
                        l = k;
                        while ( addcard(l+1) & sc->c[i] && ! ( addcard(l+1) & fixed ) )
                            l++;
                        fixed |= addcard(l);
                    }
                }
            }
        }
        westfixed = fixed;
    }

    //  translate the fixed cards for east in canonical order
    if ( eastfixed ) {
        fixed = RENONCE;
        sc = &scards[0][east];
        for ( i = 0; i <= sc->n; i++ ) {
            if ( eastfixed & sc->c[i] ) {
                for ( k = two; k <= ace; k++ ) {
                    if ( addcard(k) & eastfixed && addcard(k) & sc->c[i] ) {
                        l = k;
                        while ( addcard(l-1) & sc->c[i] && ! ( addcard(l-1) & fixed ) )
                            l--;
                        fixed |= addcard(l);
                    }
                }
            }
        }
        eastfixed = fixed;
    }

    //  generate the full set of boards
    create_all_boards();

    //  initialise the root of the tree
    wnode[0]->d = NIL;
    wnode[0]->l = NIL;
    wnode[0]->r = NIL;
    wnode[0]->i = nextindex;
    wnode[0]->c = RENONCE;
    wnode[0]->p = defence ? north : west;
    wnode[0]->winc = RENONCE;
    wnode[0]->term = 0;

    //  create the index list for the root
    for ( i = 0; i < boardcount; i++ ) {
        bump_index( lindex );
        lindex->r = nextindex;
        lindex->i = i;
        lindex->min = 0;
        lindex->max = tricklimit;
        for ( p = 0; p < 4; p++ )
            lindex->c[p] = board[p][i];
    }
    lindex->r = NIL;

    //  generate the board header
    create_board_header();

    //  print the boards
    print_boards(output);
}
