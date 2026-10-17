//  name:
//      fold_tree
//
//  purpose:
//      fold_tree folds subtrees if they contain a common structure of plays
//
//  parameters:
//      p               in      the NODE subtrees are merged for player p
//      d               in      the depth in the NODE tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void fold_tree( int d, int p )
{
    int          i, k = 0;
    struct NODE  *bnode[NPLIES+1], *lnode[NPLIES+1];

    //  refuse rather than index the walker stacks past their end: a corrupted
    //  ->d chain must not reach the /GS cookie below bnode[]/lnode[]
    if ( d < 1 || d > MAXPLY ) {
        treeoverflow = YES;
        return;
    }

    //  initialise
    bnode[d-1] = tnode[d-1];
    lnode[d-1] = tnode[d-1];
    bnode[d] = tnode[d-1]->d;

    if ( ( EW(p) && EW(bnode[d]->p) ) || ( NS(p) && NS(bnode[d]->p) ) ) {

        //  UP
        if ( foldmode == 1 ) {
            while ( bnode[d]->r )
                bnode[d] = bnode[d]->r;

        next_bnode:
            while ( bnode[d]->l ) {

                tnode[d] = bnode[d]->l;
                while ( tnode[d] ) {

                    if ( d > foldskip && tnode[d]->p == bnode[d]->p && ( EW(p) || NS(p) && w_ew(tnode) == w_ew(bnode) ) ) {

                        lnode[d] = copy_tree( d, tnode[d] );
                        lnode[d]->l = tnode[d];
                        lnode[d]->r = tnode[d]->r;
                        lnode[d]->l->r = lnode[d];
                        lnode[d]->r->l = lnode[d];

                        if ( ! bnode[d]->d || ( ! ply_overflow( d ) && ( NS(p) ? fold_ns_tree( d+1, lnode[d]->c | bnode[d]->c, lnode, bnode ) : fold_ew_tree( d+1, lnode, bnode ) ) ) ) {
                            merge_nodes( lnode[d], bnode[d] );
                            delete_tree( d, tnode );
                            tnode[d] = lnode[d];
                            lnode[d] = bnode[d]->l;
                            delete_tree( d, bnode );
                            bnode[d] = lnode[d];
                            goto next_bnode;
                        }

                        delete_tree( d, lnode );
                    }

                    tnode[d] = tnode[d]->l;
                }

                bnode[d] = bnode[d]->l;
            }
        }

        //  UPSTRIDE
        if ( foldmode == 2 ) {
            while ( ++k ) {

                i = 0;
                bnode[d] = bnode[d-1]->d;
                while ( bnode[d]->r )
                    bnode[d] = bnode[d]->r;
                tnode[d] = bnode[d];
                while ( i < k && tnode[d] ) {
                    i++;
                    tnode[d] = tnode[d]->l;
                }
                if ( ! tnode[d] )
                    break;

                while ( tnode[d] ) {

                    if ( d > foldskip && tnode[d]->p == bnode[d]->p && ( EW(p) || NS(p) && w_ew(tnode) == w_ew(bnode) ) ) {

                        lnode[d] = copy_tree( d, tnode[d] );
                        lnode[d]->l = tnode[d];
                        lnode[d]->r = tnode[d]->r;
                        lnode[d]->l->r = lnode[d];
                        lnode[d]->r->l = lnode[d];

                        if ( ! bnode[d]->d || ( ! ply_overflow( d ) && ( NS(p) ? fold_ns_tree( d+1, lnode[d]->c | bnode[d]->c, lnode, bnode ) : fold_ew_tree( d+1, lnode, bnode ) ) ) ) {
                            merge_nodes( lnode[d], bnode[d] );
                            delete_tree( d, tnode );
                            tnode[d] = lnode[d]->l;
                            lnode[d] = bnode[d]->l;
                            delete_tree( d, bnode );
                            bnode[d] = lnode[d];
                        }

                        else {
                            delete_tree( d, lnode );
                            bnode[d] = bnode[d]->l;
                            tnode[d] = tnode[d]->l;
                        }
                    }

                    else {
                        bnode[d] = bnode[d]->l;
                        tnode[d] = tnode[d]->l;
                    }
                }
            }
        }

        //  DOWN
        if ( foldmode == 3 ) {
            tnode[d] = tnode[d-1]->d;
            while ( tnode[d] && tnode[d]->r ) {

                bnode[d] = tnode[d]->r;
                while ( bnode[d] ) {

                    if ( d > foldskip && tnode[d]->p == bnode[d]->p && ( EW(p) || NS(p) && w_ew(tnode) == w_ew(bnode) ) ) {

                        lnode[d] = copy_tree( d, tnode[d] );
                        lnode[d]->l = tnode[d];
                        lnode[d]->r = tnode[d]->r;
                        lnode[d]->l->r = lnode[d];
                        lnode[d]->r->l = lnode[d];

                        if ( ! bnode[d]->d || ( ! ply_overflow( d ) && ( NS(p) ? fold_ns_tree( d+1, lnode[d]->c | bnode[d]->c, lnode, bnode ) : fold_ew_tree( d+1, lnode, bnode ) ) ) ) {
                            merge_nodes( lnode[d], bnode[d] );
                            delete_tree( d, tnode );
                            tnode[d] = lnode[d];
                            lnode[d] = bnode[d]->r;
                            delete_tree( d, bnode );
                            bnode[d] = lnode[d];
                        }

                        else {
                            delete_tree( d, lnode );
                            bnode[d] = bnode[d]->r;
                        }
                    }

                    else
                        bnode[d] = bnode[d]->r;
                }

                tnode[d] = tnode[d]->r;
            }
        }

        //  DOWNSTRIDE
        if ( foldmode == 4 ) {
            while ( ++k ) {

                i = 0;
                tnode[d] = bnode[d-1]->d;
                bnode[d] = tnode[d];
                while ( i < k && bnode[d] ) {
                    i++;
                    bnode[d] = bnode[d]->r;
                }
                if ( ! bnode[d] )
                    break;

                while ( bnode[d] ) {

                    if ( d > foldskip && tnode[d]->p == bnode[d]->p && ( EW(p) || NS(p) && w_ew(tnode) == w_ew(bnode) ) ) {

                        lnode[d] = copy_tree( d, tnode[d] );
                        lnode[d]->l = tnode[d];
                        lnode[d]->r = tnode[d]->r;
                        lnode[d]->l->r = lnode[d];
                        lnode[d]->r->l = lnode[d];

                        if ( ! bnode[d]->d || ( ! ply_overflow( d ) && ( NS(p) ? fold_ns_tree( d+1, lnode[d]->c | bnode[d]->c, lnode, bnode ) : fold_ew_tree( d+1, lnode, bnode ) ) ) ) {
                            merge_nodes( lnode[d], bnode[d] );
                            delete_tree( d, tnode );
                            tnode[d] = lnode[d];
                            lnode[d] = bnode[d]->r;
                            delete_tree( d, bnode );
                            bnode[d] = lnode[d];
                        }

                        else {
                            delete_tree( d, lnode );
                            tnode[d] = tnode[d]->r;
                            bnode[d] = bnode[d]->r;
                        }
                    }

                    else {
                        tnode[d] = tnode[d]->r;
                        bnode[d] = bnode[d]->r;
                    }
                }
            }
        }
    }

    //  investigate the sons
    tnode[d] = tnode[d-1]->d;
    while ( tnode[d] ) {
        if ( tnode[d]->d && ! ply_overflow( d ) )
            fold_tree( d+1, p );
        tnode[d] = tnode[d]->r;
    }

    //  determine the winning cards
    tnode[d] = tnode[d-1]->d;
    tnode[d-1]->w_c = (d-1)%4 ? RENONCE : tnode[d-1]->w_c;
    while ( tnode[d] ) {
        if ( (d-1)%4 )
            tnode[d-1]->w_c |= tnode[d]->w_c;
        tnode[d] = tnode[d]->r;
    }
}
