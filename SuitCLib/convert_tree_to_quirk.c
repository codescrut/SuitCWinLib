//  name:
//      convert_tree_to_quirk
//
//  purpose:
//      convert_tree_to_quirk converts the NODE tree to QUIRKD tree. For
//      each terminal node it builds a QUIRKD node with calls to bump_quirkd
//      and bump_quirkh. Thereafter it combines NS nodes with calls to
//      fold_quirkd, and EW nodes by calls to merge_quirkd.
//
//  parameters:
//      none
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void convert_tree_to_quirk( void )
{
    int          d = 1, i;
    struct INDEX *lindex;

    //  the tree contains no more branches
    if ( wnode[0]->term ) {
        i = -1;
        bump_quirkh( quirkd[0].h );
        bump_quirkd( quirkd[0].h->d );
        lindex = wnode[0]->i;
        while ( lindex ) {
            quirkd[0].h->t[++i].i = lindex->i;
            quirkd[0].h->t[i].ns = lindex->max;
            lindex = lindex->r;
        }
        quirkd[0].h->d->il = 0;
        quirkd[0].h->d->ih = i;
        return;
    }

    //  loop over all branches of the NODE tree
    quirkd[d-1].h = NIL;
    wnode[d] = wnode[d-1]->d;
    while ( wnode[d] ) {

        //  get the oldest son
        if ( wnode[d]->d ) {
            d++;
            quirkd[d-1].h = NIL;
            wnode[d] = wnode[d-1]->d;
        }

        //  we reached a leaf
        else {

            //  build the QUIRKD tree
            i = -1;
            bump_quirkh( quirkd[d].h );
            bump_quirkd( quirkd[d].h->d );
            lindex = wnode[d]->i;
            while ( lindex ) {
                quirkd[d].h->t[++i].i = lindex->i;
                quirkd[d].h->t[i].ns = lindex->max;
                lindex = lindex->r;
            }
            quirkd[d].h->d->il = 0;
            quirkd[d].h->d->ih = i;

            //  build the QUIRD tree and get the next node in the NODE tree
            while ( YES ) {

                if ( quirkd[d-1].h == NIL )
                    quirkd[d-1].h = quirkd[d].h;

                //  fold north-south QUIRKD trees
                else if ( NS(wnode[d]->p) )
                    fold_quirkd( d-1 );

                //  merge east-west QUIRKD trees
                else if ( EW(wnode[d]->p) ) {

                    //  reduce the number of branches in the QUIRKD trees
                    if ( cutoff && d <= 4 && (long) count_quirkd( d-1 ) * (long) count_quirkd( d ) > cutoff )
                        limit_quirkd( d-1 );

                    merge_quirkd( d-1 );
                }

                if ( wnode[d]->r ) {
                    wnode[d] = wnode[d]->r;
                    break;
                }

                d--;
                if ( ! d )
                    return;
            }
        }
    }
}
