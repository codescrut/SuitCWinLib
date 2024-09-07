//  name:
//      count_quirkd
//
//  purpose:
//      count_quirkd counts the number of plays
//
//  parameters:
//      d               in      the depth in the NODE tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

int count_quirkd( int d )
{
    int           ix = 0, l = 0;
    struct QUIRKD *lquirkd[NBOARDS], *lquirkdl[NBOARDS];

    //  initialise
    lquirkdl[0] = &quirkd[d];
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;

    while ( lquirkd[l] ) {

        //  get the oldest son
        if ( lquirkd[l]->d ) {
            l++;
            lquirkdl[l] = lquirkdl[l-1];
            lquirkd[l] = lquirkd[l-1]->d;
        }

        //  get the next sibling
        else {
            ix++;
            while ( l && lquirkd[l]->h == NIL )
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkd[l]->h ? lquirkd[l]->h->d : NIL;
        }
    }

    return( ix );
}
