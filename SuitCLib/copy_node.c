//  name:
//      copy_node
//
//  purpose:
//      copy_node copies a NODE node
//
//  parameters:
//      lnode           in      the old node
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

struct NODE* copy_node(struct NODE* lnode)
{
    int          p;
    struct INDEX* lindex, * tindex = NIL;
    struct NODE* tnode;

    //  create the new node
    bump_node(tnode);
    tnode->d = NIL;
    tnode->l = NIL;
    tnode->r = NIL;
    tnode->c = lnode->c;
    tnode->e_down = lnode->e_down;
    tnode->e_up = lnode->e_up;
    tnode->ns = lnode->ns;
    tnode->w_c = lnode->w_c;
    tnode->w_p = lnode->w_p;
    tnode->p = lnode->p;
    tnode->term = lnode->term;

    //  copy the boards
    tnode->i = nextindex;
    lindex = lnode->i;
    while (lindex) {
        bump_index(tindex);
        tindex->i = lindex->i;
        for (p = 0; p < 4; p++)
            tindex->c[p] = lindex->c[p];
        tindex->min = lindex->min;
        tindex->max = lindex->max;
        tindex->r = nextindex;
        lindex = lindex->r;
    }

    tindex->r = NIL;

    return(tnode);
}
