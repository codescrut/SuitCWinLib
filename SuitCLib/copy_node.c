//  name:
//      copy_node
//
//  purpose:
//      copy_node copies a NODE node
//
//  parameters:
//      lnode           in      the old node
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

struct NODE *copy_node( struct NODE *lnode )
{
    int          p;
    struct INDEX *lindex, *tindex = NULL;
    struct NODE  *tnode;

    //  create the new node
    bump_node( tnode );
    tnode->d = NIL;
    tnode->l = NIL;
    tnode->r = NIL;
    tnode->c = lnode->c;
    tnode->winc = lnode->winc;
    tnode->p = lnode->p;
    tnode->term = lnode->term;

    //  copy the boards
    tnode->i = nextindex;
    lindex = lnode->i;
    while ( lindex ) {
        bump_index( tindex );
        tindex->i = lindex->i;
        for ( p = 0; p < 4; p++ )
            tindex->c[p] = lindex->c[p];
        tindex->min = lindex->min;
        tindex->max = lindex->max;
        tindex->r = nextindex;
        lindex = lindex->r;
    }

    //  clear the right sibling of the last board
    tindex->r = NIL;

    return( tnode );
}
