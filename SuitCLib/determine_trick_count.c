//  name:
//      determine_trick_count
//
//  purpose:
//      determine_trick_count determines the minimum and maximum number of
//      tricks for each board in a node from the number of tricks in the
//      descending nodes. if the node has no descending nodes, the number
//      of tricks has already be set in add_node and nothing needs to be
//      done. otherwise check_nodes is called to remove all inferior
//      plays from its sons. The search of the sons is terminated when
//      a match is found for the played cards.
//
//  parameters:
//      d               in      the depth in the NODE tree
//      lnode           in      the NODE tree
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void determine_trick_count( int d, struct NODE **lnode )
{
    struct INDEX *bindex, *lindex;

    //  return if there are no sons
    if ( lnode[d-1]->d == NIL )
        return;

    //  clear the winning cards
    lnode[d-1]->winc = (d-1)%4 ? RENONCE : lnode[d-1]->winc;

    //  initialise the number of NS tricks
    lnode[d] = lnode[d-1]->d;
    while ( lnode[d] ) {
        if ( (d-1)%4 )
            lnode[d-1]->winc |= lnode[d]->winc;
        lindex = lnode[d-1]->i;
        bindex = lnode[d]->i;
        while( bindex ) {
            while( lindex->i < bindex->i )
                lindex = lindex->r;
            lindex->min = NCARDS;
            lindex->max = 0;
            bindex = bindex->r;
        }
        lnode[d] = lnode[d]->r;
    }

    //  determine the number of NS tricks
    lnode[d] = lnode[d-1]->d;
    while ( lnode[d] ) {
        lindex = lnode[d-1]->i;
        bindex = lnode[d]->i;
        while( bindex ) {
            while( lindex->i < bindex->i )
                lindex = lindex->r;
            lindex->min = MIN( bindex->min, lindex->min );
            lindex->max = MAX( bindex->max, lindex->max );
            bindex = bindex->r;
        }
        lnode[d] = lnode[d]->r;
    }

    //  check for remaining forks in the subtree
    lnode[d-1]->term = YES;
    lindex = lnode[d-1]->i;
    while ( lindex ) {
        if ( lindex->min != lindex->max )
            lnode[d-1]->term = NO;
        lindex = lindex->r;
    }

    if ( d > 1 && lnode[d-1]->term && ( d-1 == maxply || resultsonly || buildtree == 0 ) ) {
        lnode[d] = lnode[d-1]->d;
        while ( lnode[d] )
            lnode[d] = delete_tree( d, lnode );
    }
}
