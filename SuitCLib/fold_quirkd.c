//  name:
//      fold_quirkd
//
//  purpose:
//      fold_quirkd folds two QUIRKD trees
//
//  parameters:
//      d               in      the depth in the NODE tree
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void fold_quirkd( int d )
{
    int           ih, l = 0, s = 0, t = 0;
    struct QUIRKD *lquirkd[NBOARDS], *lquirkdl[NBOARDS], *tquirkd[NBOARDS], *tquirkdl[NBOARDS];

    //  initialise
    lquirkdl[l] = &quirkd[d+1];
    lquirkd[l] = lquirkdl[l]->h ? lquirkdl[l]->h->d : NIL;
    tquirkdl[t] = &quirkd[d];
    tquirkd[t] = tquirkdl[t]->h ? tquirkdl[t]->h->d : NIL;

    //  determine the highest index in the TRICKS array
    while ( lquirkd[l]->d ) {
        l++;
        lquirkd[l] = lquirkd[l-1]->d;
    }
    ih = lquirkd[l]->ih;

    //  loop over all nodes of the source QUIRKD tree
    l = 0;
    while ( lquirkd[l] ) {

        //  add a new node to the target QUIRKD tree
        t = add_quirkd( lquirkdl[l]->h->t[s].i, lquirkdl[l]->h->t[s].ns, s, t, ih, tquirkd, tquirkdl );

        //  get the oldest son
        if ( s < lquirkd[l]->ih || lquirkd[l]->d ) {
            s++;
            if ( s > lquirkd[l]->ih ) {
                l++;
                lquirkdl[l] = lquirkdl[l-1];
                lquirkd[l] = lquirkd[l-1]->d;
            }
        }

        //  we reached a leave
        else {

            //  backtrack untill a branch in the source QUIRKD tree
            while ( l && ! lquirkd[l]->h )
                l--;
            s = lquirkd[l]->il;

            //  backtrack the target QUIRKD tree and check each branch
            while ( tquirkd[t]->il > s ) {
                check_quirkd( d, t, tquirkd, tquirkdl );
                t--;
            }

            //  get the younger sibling in the source QUIRKD tree
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkdl[l]->h ? lquirkdl[l]->h->d : NIL;
        }
    }

    //  check the root of the target QUIRKD tree
    delete_quirkd( d+1 );
    check_quirkd( d, t, tquirkd, tquirkdl );
}
