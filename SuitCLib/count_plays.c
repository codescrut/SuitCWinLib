//  name:
//      count_plays
//
//  purpose:
//      count_plays counts the number of leaves for each node in the NODE tree
//
//  parameters:
//      none
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void count_plays( void )
{
    int          d = 1;
    struct NODE *bnode;

    //  loop over all branches of the NODE tree
    wnode[d] = wnode[d-1]->d;
    wnode[d-1]->nplays = 0;
    while ( wnode[d] ) {

        //  get the oldest son
        wnode[d]->nplays = 0;
        if ( wnode[d]->d ) {
            d++;
            wnode[d] = wnode[d-1]->d;
        }

        //  we reached a leaf
        else {

            wnode[d]->nplays = 1;
            wnode[d-1]->nplays += wnode[d]->nplays;

            while ( d && ! wnode[d]->r ) {
                d--;
                if ( d )
                    wnode[d-1]->nplays += wnode[d]->nplays;
            }

            if ( ! d )
                break;

            wnode[d] = wnode[d]->r;
        }
    }
}
