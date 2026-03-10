//  name:
//      use_suit_entries
//
//  purpose:
//      use_suit_entries selects plays in the final NODE tree that maximize
//      the number of entries in the suit itself
//
//  parameters:
//      none
//
//  Copyright 2025, Gerrit A. van der Velde, Leiden

#include "header.h"

void use_suit_entries( void )
{
    int           d = 1;
    struct NODE   *bnode[NPLIES+1], *lnode[NPLIES+1];

    bnode[0] = tnode[0];
    lnode[0] = tnode[0];
    tnode[d] = tnode[d-1]->d;
    while ( d ) {

        //  loop untill a leave
        while ( tnode[d]->d ) {
            d++;
            tnode[d] = tnode[d-1]->d;
        }

        //  compare all siblings
        while ( d ) {

            if ( d%2 == 1 && NS(tnode[d-1]->d->p) && tnode[d-1]->d->r ) {

                bnode[d-1] = tnode[d-1];
                lnode[d-1] = tnode[d-1];
                lnode[d] = lnode[d-1]->d;
            next_lnode:
                while ( lnode[d] ) {
                    bnode[d] = lnode[d]->r;
                    while ( bnode[d] ) {
                        if ( lnode[d]->e_up > bnode[d]->e_up )
                            bnode[d] = delete_tree( d, bnode );
                        else if ( bnode[d]->e_up > lnode[d]->e_up ) {
                            lnode[d] = delete_tree( d, lnode );
                            goto next_lnode;
                        }
                        else
                            bnode[d] = bnode[d]->r;
                    }
                    lnode[d] = lnode[d]->r;
                }
            }

            //  keep track of the entries
            lnode[d] = tnode[d-1]->d;
            tnode[d-1]->e_up = NS(lnode[d]->p) ? 0 : 0xff;
            while ( lnode[d] ) {
                tnode[d-1]->e_up = NS(lnode[d]->p) ? MAX( tnode[d-1]->e_up, lnode[d]->e_up ) : MIN( tnode[d-1]->e_up, lnode[d]->e_up );
                lnode[d] = lnode[d]->r;
            }

            //  backtrack
            d--;
            if ( d == 0 )
                break;
            tnode[d] = tnode[d]->r;
            if ( tnode[d] )
                break;
        }
    }
}
