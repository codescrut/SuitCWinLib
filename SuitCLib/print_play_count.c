//  name:
//      print_play_count
//
//  purpose:
//      print_play_count prints the number of leaves in the NODE subtrees
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void print_play_count( void )
{
    int          d = 1, b, l = 0;
    char         out[800];
    struct NODE  *lnode[NPLIES+1];

    printf( "\nThe number of plays\n===================\n" );
    sprintf( &out[0], "%7d", wnode[d-1]->nplays );

    l = 0;
    lnode[d] = wnode[0]->d;
    while ( lnode[d] ) {

        sprintf( &out[7*d], "%7d", lnode[d]->nplays );

        //  handle the sons
        if ( lnode[d]->d ) {
            d++;
            lnode[d] = lnode[d-1]->d;
        }

        else {
            out[7*(d+1)] = '\0';
            printf( "%s\n", out );
            while ( d > 1 && ! lnode[d]->r ) {
                d--;
                l -= 7;
            }
            for ( b = 0; b < d; b++ )
                sprintf( &out[7*b], "%7s", "" );
            lnode[d] = lnode[d]->r;
        }
    }
}
