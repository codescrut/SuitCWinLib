//  name:
//      print_boards
//
//  purpose:
//      print_boards prints the information about the suit combination
//
//  parameters:
//      none
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void print_boards(char* output)
{
    int          i, j, k, n, p, q;
    char         out[NCARDS+1];
    static char  *symbols[] = { "-123456789TJQKA", "-123456789TBVHA", "-123456789tjqka", "-123456789tbvha" };
    static char  seat[4][6] = { "West", "North", "East", "South" };
    struct INDEX *lindex;

    //  print the 4 hands
    for ( q = 0; q < 4; q++ ) {
        p = ( q == 0 ? north : ( q == 1 ? south : ( q == 2 ? west : east ) ) );
        k = 0;
        out[0] = '-';
        out[1] = '\0';
        for ( i = NCARDS+1; i >= 0; i-- ) {
            if ( addcard(i) & brd[0][p] ) {
                out[k] = symbols[printoption&0x1][i];
                out[++k] = '\0';
            }
        }
        printf( "\t%-5s: %s\n", seat[p], out );

        char str[50];
        sprintf(str, "\"%s\":\"%s\",", seat[p], out);
        strncat(output, str, 1024);
    }
    printf( "\n" );

    //  print additional information
    if ( cutoff && cutoff == CUTOFF )
        printf( "\tcutoff              : enabled\n" );
    else if ( cutoff )
        printf( "\tcutoff              : %d\n", cutoff );
    if ( tricklimit < ntricks )
        printf( "\tterminate play after: %d tricks\n", tricklimit );
    if ( w[0] != noplayer ) {
        printf( "\tthe lead is in      : %s\n", seat[w[0]] );
        if ( opening_lead != 0x7fff )
            for ( i = 0; i <= NCARDS+1; i++ )
                if ( opening_lead & addcard(i) )
                    printf( "\tthe lead is the     : %c\n", symbols[printoption&0x1][i] );
    }
    if ( nrules ) {
        for ( n = 0; n < nrules; n++ ) {
            if ( n == 0 ) {
                if ( defence )
                    printf( "\tsecond hand strategy: " );
                else
                    printf( "\tthird hand strategy : " );
            }
            else
                printf( "\t                      " );
            for ( i = 14; i >= 2; i-- )
                if ( h2rule[n] & addcard(i) )
                    printf( "%c", symbols[printoption&0x1][i] );
            printf( " -> " );
            for ( i = 14; i >= 2; i-- )
                if ( h3rule[n] & addcard(i) )
                    printf( "%c", symbols[printoption&0x1][i] );
            printf( "\n" );
        }
    }
    if ( throwin != noplayer )
        printf( "\tthrowin is enabled  : %s\n", seat[throwin] );
    for ( p = 0; p < 4; p++ )
        if ( nfree[0][p] < ncards[p] )
            printf( "\t#free entries %-5s : %d\n", seat[p], nfree[0][p] );
    for ( p = 0; p < 4; p++ )
        if ( nruff[0][p] )
            printf( "\tnumb. of ruffs %-5s: %d\n", seat[p], nruff[0][p] );
    if ( eastfixed ) {
        k = 0;
        out[0] = '-';
        out[1] = '\0';
        for ( i = NCARDS+1; i >= 0; i-- ) {
            if ( addcard(i) & eastfixed ) {
                out[k] = symbols[printoption&0x1][i];
                out[++k] = '\0';
            }
        }
        printf( "\tfixed east cards    : %s\n", out );
    }
    if ( westfixed ) {
        k = 0;
        out[0] = '-';
        out[1] = '\0';
        for ( i = NCARDS+1; i >= 0; i-- ) {
            if ( addcard(i) & westfixed ) {
                out[k] = symbols[printoption&0x1][i];
                out[++k] = '\0';
            }
        }
        printf( "\tfixed west cards    : %s\n", out );
    }
    if ( ncardsmin[west] != 0 )
        printf( "\tminimum length west : %d\n", ncardsmin[west] );
    if ( westvac != NCARDS )
        printf( "\t#vacant places west : %d\n", westvac );
    if ( ncardsmin[east] != 0 )
        printf( "\tminimum length east : %d\n", ncardsmin[east] );
    if ( eastvac != NCARDS )
        printf( "\t#vacant places east : %d\n", eastvac );

    //  print the full set of boards
    if ( printboards ) {

        i = 0;
        lindex = wnode[0]->i;
        printf( "\n\n%s\n%s\n",
                "   Probabilities    West          East",
                "   =============    ====          ====" );

        while ( lindex ) {

            printf( "%5d x %7.6f    ", dupl[i], prob[i] );
            for ( p = 0; p < 4; p += 2 ) {
                k = 0;
                out[0] = '-';
                out[1] = '\0';
                for ( j = NCARDS+1; j >= 0; j-- ) {
                    if ( addcard(j) & lindex->c[p] ) {
                        out[k] = symbols[printoption&0x1][j];
                        out[++k] = '\0';
                    }
                }
                printf( "%-14s", out );
            }
            printf( "\n" );

            i++;
            lindex = lindex->r;
        }
        printf( "\n\n" );
    }
    fflush( stdout );
}
