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

void fold_tree(int d, int p)
{
    struct NODE* bnode[NPLIES + 1], * lnode[NPLIES + 1];

    //  refuse rather than index the walker stacks past their end: a corrupted
    //  ->d chain must not reach the /GS cookie below bnode[]/lnode[]
    if (d < 1 || d > MAXPLY) {
        treeoverflow = YES;
        return;
    }

    //  initialise
    bnode[d - 1] = tnode[d - 1];
    lnode[d - 1] = tnode[d - 1];
    bnode[d] = tnode[d - 1]->d;
    while (bnode[d]->r)
        bnode[d] = bnode[d]->r;

    if ((EW(p) && EW(bnode[d]->p)) || NS(p) && NS(bnode[d]->p)) {

    next_bnode:
        while (bnode[d]->l) {

            tnode[d] = bnode[d]->l;
            while (tnode[d]) {

                if (tnode[d]->p == bnode[d]->p && (EW(p) || (NS(p) && (w_ew(tnode) == w_ew(bnode))))) {

                    lnode[d] = copy_tree(d, tnode[d]);
                    lnode[d]->r = tnode[d]->r;
                    lnode[d]->r->l = lnode[d];
                    tnode[d]->r = lnode[d];
                    lnode[d]->l = tnode[d];

                    if (!bnode[d]->d || (!ply_overflow(d) && (NS(p) ? fold_ns_tree(d + 1, lnode[d]->c | bnode[d]->c, lnode, bnode) : fold_ew_tree(d + 1, lnode, bnode)))) {
                        merge_nodes(lnode[d], bnode[d]);
                        lnode[d] = bnode[d]->l;
                        delete_tree(d, bnode);
                        delete_tree(d, tnode);
                        bnode[d] = lnode[d];
                        goto next_bnode;
                    }

                    delete_tree(d, lnode);
                }

                tnode[d] = tnode[d]->l;
            }

            bnode[d] = bnode[d]->l;
        }
    }

    //  investigate the sons
    tnode[d] = tnode[d - 1]->d;
    while (tnode[d]) {
        if (tnode[d]->d && !ply_overflow(d))
            fold_tree(d + 1, p);
        tnode[d] = tnode[d]->r;
    }

    //  determine the winning cards
    tnode[d] = tnode[d - 1]->d;
    tnode[d - 1]->w_c = (d - 1) % 4 ? RENONCE : tnode[d - 1]->w_c;
    while (tnode[d]) {
        if ((d - 1) % 4)
            tnode[d - 1]->w_c |= tnode[d]->w_c;
        tnode[d] = tnode[d]->r;
    }
}
