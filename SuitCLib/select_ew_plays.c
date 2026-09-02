//  name
//      select_ew_plays
//
//  purpose:
//      select_ew_plays selects the north-south plays that wins tricks as early as possible
//
//  parameters:
//      d               in      the depth in the NODE tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

#define bwinc_ew()      ( bnode[d]->w_c & ( brd[0][west]  | brd[0][east]  ) )
#define bwinc_ns()      ( bnode[d]->w_c & ( brd[0][north] | brd[0][south] ) )
#define lwinc_ew()      ( lnode[d]->w_c & ( brd[0][west]  | brd[0][east]  ) )
#define lwinc_ns()      ( lnode[d]->w_c & ( brd[0][north] | brd[0][south] ) )

void select_ew_plays( int d )
{
    struct NODE  *bnode[NPLIES+1], *lnode[NPLIES+1];

    //  refuse rather than index the walker stacks past their end: a corrupted
    //  ->d chain must not reach the /GS cookie below bnode[]/lnode[]
    if (d < 1 || d > MAXPLY) {
        treeoverflow = YES;
        return;
    }

    //  initialise
    bnode[d-1] = tnode[d-1];
    lnode[d-1] = tnode[d-1];
    lnode[d] = tnode[d-1]->d;

    //  investigate the sons
    if ( tnode[d-1]->d->d && ! ply_overflow( d ) ) {
        tnode[d] = tnode[d-1]->d;
        while ( tnode[d] ) {
            select_ew_plays( d+1 );
            tnode[d] = tnode[d]->r;
        }
    }

    if ( NS(lnode[d]->p) ) {

        //  compare plays by the same player
    next_lnode:
        while ( lnode[d] && lnode[d]->r ) {

            bnode[d] = lnode[d]->r;
            while ( bnode[d] ) {

                if      (   lwinc_ns() &&   lwinc_ew() &&   bwinc_ns() &&   bwinc_ew() ) { bnode[d] = bnode[d]->r; }
                else if (   lwinc_ns() &&   lwinc_ew() &&   bwinc_ns() && ! bwinc_ew() ) { lnode[d] = delete_tree( d, lnode ); goto next_lnode; }
                else if (   lwinc_ns() &&   lwinc_ew() && ! bwinc_ns() &&   bwinc_ew() ) { bnode[d] = delete_tree( d, bnode ); }
                else if (   lwinc_ns() && ! lwinc_ew() &&   bwinc_ns() &&   bwinc_ew() ) { bnode[d] = delete_tree( d, bnode ); }
                else if (   lwinc_ns() && ! lwinc_ew() &&   bwinc_ns() && ! bwinc_ew() ) { bnode[d] = bnode[d]->r; }
                else if (   lwinc_ns() && ! lwinc_ew() && ! bwinc_ns() &&   bwinc_ew() ) { bnode[d] = delete_tree( d, bnode ); }
                else if ( ! lwinc_ns() &&   lwinc_ew() &&   bwinc_ns() &&   bwinc_ew() ) { lnode[d] = delete_tree( d, lnode ); goto next_lnode; }
                else if ( ! lwinc_ns() &&   lwinc_ew() &&   bwinc_ns() && ! bwinc_ew() ) { lnode[d] = delete_tree( d, lnode ); goto next_lnode; }
                else if ( ! lwinc_ns() &&   lwinc_ew() && ! bwinc_ns() &&   bwinc_ew() ) { bnode[d] = bnode[d]->r; }
            }

            lnode[d] = lnode[d]->r;
        }
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
