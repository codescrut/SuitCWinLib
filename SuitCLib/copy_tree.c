//  name:
//      copy_tree
//
//  purpose:
//      copy_tree copies a tree
//
//  parameters:
//      d_              in      the depth in the NODE tree
//      lnode           in      the NODE tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

struct NODE* copy_tree(int d_, struct NODE* rootn)
{
    int          d = d_, p, ply = d_;
    struct INDEX* lindex, * tindex = NIL;
    struct NODE* lnode[NPLIES + 1], * tnode[NPLIES + 1];

    //  refuse rather than index lnode[]/tnode[] past their end
    if (d_ < 0 || d_ > MAXPLY) {
        treeoverflow = YES;
        return(NIL);
    }

    //  loop over the nodes
    lnode[d] = rootn;
    tnode[d] = NIL;
    while (lnode[d]) {

        //  create the new node
        if (tnode[d]) {
            bump_node(tnode[d]->r);
            tnode[d]->r->l = tnode[d];
            tnode[d] = tnode[d]->r;
        }
        else {
            bump_node(tnode[d]);
            if (d > ply)
                tnode[d - 1]->d = tnode[d];
            tnode[d]->l = NIL;
        }

        //  fill in the fields
        tnode[d]->d = NIL;
        tnode[d]->r = NIL;
        tnode[d]->c = lnode[d]->c;
        tnode[d]->e_down = lnode[d]->e_down;
        tnode[d]->e_up = lnode[d]->e_up;
        tnode[d]->ns = lnode[d]->ns;
        tnode[d]->w_c = lnode[d]->w_c;
        tnode[d]->w_p = lnode[d]->w_p;
        tnode[d]->p = lnode[d]->p;
        tnode[d]->term = lnode[d]->term;

        //  copy the boards
        tnode[d]->i = nextindex;
        lindex = lnode[d]->i;
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

        //  handle the sons
        if (lnode[d]->d && !ply_overflow(d)) {
            d++;
            lnode[d] = lnode[d - 1]->d;
            tnode[d] = NIL;
        }

        //  get the next node
        else
            lnode[d] = lnode[d]->r;

        //  get the next node
        while ((d > ply) && !lnode[d]) {
            d--;
            lnode[d] = lnode[d]->r;
        }
        if (d == ply)
            break;
    }

    return(tnode[d]);
}
