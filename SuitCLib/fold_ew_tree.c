//  name:
//      fold_ew_tree
//
//  purpose:
//      fold_ew_tree folds the tree when east-west branches are merged
//
//  parameters:
//      d               in      the depth in the NODE tree
//      lnode           in      the first sibling NODE tree
//      bnode           in      the second sibling NODE tree
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

int fold_ew_tree( int d, struct NODE **lnode, struct NODE **bnode )
{
    int          match;
    struct NODE  *tnode;

    bnode[d] = bnode[d-1]->d;
    lnode[d] = lnode[d-1]->d;
    if ( EW(lnode[d]->p) && EW(bnode[d]->p) ) {

        match = YES;
        while ( bnode[d] ) {

            while ( lnode[d]->r && ( lnode[d]->p < bnode[d]->p || ( lnode[d]->p == bnode[d]->p && lnode[d]->c > bnode[d]->c ) ) )
                lnode[d] = lnode[d]->r;

            //  both branches make the same play, so fold both trees
            if ( lnode[d]->p == bnode[d]->p && lnode[d]->c == bnode[d]->c ) {
                if ( ! bnode[d]->d || fold_ew_tree( d+1, lnode, bnode ) ) {
                    merge_nodes( lnode[d], bnode[d] );
                    bnode[d] = bnode[d]->r;
                }
                else
                    return( NO );
            }

            //  both branches make different plays; copy the bnode subtree into the lnode tree
            else {

                tnode = copy_tree( d, bnode[d] );

                //  copy bnode[d] as the left sibling of lnode[d]
                if ( ( lnode[d]->p == bnode[d]->p && lnode[d]->c < bnode[d]->c ) || lnode[d]->p > bnode[d]->p ) {
                    tnode->r = lnode[d];
                    if ( lnode[d]->l )
                        lnode[d]->l->r = tnode;
                    else
                        lnode[d-1]->d = tnode;
                    tnode->l = lnode[d]->l;
                    lnode[d]->l = tnode;
                }

                //  copy bnode[d] as the right sibling of lnode[d]
                else if ( lnode[d]->p < bnode[d]->p || ( lnode[d]->p == bnode[d]->p && lnode[d]->c > bnode[d]->c ) ) {
                    tnode->l = lnode[d];
                    lnode[d]->r = tnode;
                    tnode->r = NIL;
                }

                bnode[d] = bnode[d]->r;
            }
        }
    }

    else if ( NS(lnode[d]->p) && NS(bnode[d]->p) ) {

        match = NO;
    next_lnode:
        while ( lnode[d] ) {

            bnode[d] = bnode[d-1]->d;
            while ( bnode[d] ) {

                if ( lnode[d]->p == bnode[d]->p && lnode[d]->c == bnode[d]->c ) {
                    if ( ! bnode[d]->d || fold_ew_tree( d+1, lnode, bnode ) ) {
                        match = YES;
                        lnode[d] = lnode[d]->r;
                    }
                    else
                        lnode[d] = delete_tree( d, lnode );
                    goto next_lnode;
                }

                bnode[d] = bnode[d]->r;
            }

            lnode[d] = delete_tree( d, lnode );
        }
    }

    //  one of the branches is a throwin play
    else
        return( NO );

    //  determine the winning cards
    lnode[d] = lnode[d-1]->d;
    lnode[d-1]->winc = (d-1)%4 ? RENONCE : lnode[d-1]->winc;
    while ( lnode[d] ) {
        if ( (d-1)%4 )
            lnode[d-1]->winc |= lnode[d]->winc;
        lnode[d] = lnode[d]->r;
    }

    return( match );
}
