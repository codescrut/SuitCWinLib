//  name:
//      fold_ns_tree
//
//  purpose:
//      fold_ns_tree folds the tree when east-west branches are merged
//
//  parameters:
//      d               in      the depth in the NODE tree
//      c               in      the cards that will be merged
//      lnode           in      the first sibling NODE tree
//      bnode           in      the second sibling NODE tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

int fold_ns_tree(int d, COLOR c, struct NODE** lnode, struct NODE** bnode)
{
    //  refuse rather than index the walker stacks past their end: a corrupted
    //  ->d chain must not reach the /GS cookie below bnode[]/lnode[]
    if (d < 1 || d > MAXPLY) {
        treeoverflow = YES;
        return(NO);
    }

    bnode[d] = bnode[d - 1]->d;
    lnode[d] = lnode[d - 1]->d;
    if (EW(lnode[d]->p)) {
        while (lnode[d] && bnode[d]) {
            if (lnode[d]->p != bnode[d]->p || lnode[d]->c != bnode[d]->c)
                return(NO);
            if (bnode[d]->d && (ply_overflow(d) || !fold_ns_tree(d + 1, c, lnode, bnode)))
                return(NO);
            bnode[d] = bnode[d]->r;
            lnode[d] = lnode[d]->r;
        }
        if (lnode[d] || bnode[d])
            return(NO);
    }
    else if (NS(lnode[d]->p)) {

        while (bnode[d]) {
            bnode[d]->match = NO;
            bnode[d] = bnode[d]->r;
        }

        while (lnode[d]) {

            lnode[d]->match = NO;
            bnode[d] = bnode[d - 1]->d;
            while (bnode[d]) {

                if (lnode[d]->p == bnode[d]->p) {
                    if (c & lnode[d]->c && c & bnode[d]->c) {
                        if (w_ew(lnode) == w_ew(bnode) && (!bnode[d]->d || (!ply_overflow(d) && fold_ns_tree(d + 1, c, lnode, bnode)))) {
                            bnode[d]->match = YES;
                            lnode[d]->match = YES;
                            lnode[d]->c = c;
                        }
                    }
                    else if (lnode[d]->c == bnode[d]->c && (!bnode[d]->d || (!ply_overflow(d) && fold_ns_tree(d + 1, c, lnode, bnode)))) {
                        bnode[d]->match = YES;
                        lnode[d]->match = YES;
                    }
                }

                bnode[d] = bnode[d]->r;
            }

            if (!lnode[d]->match)
                return(NO);
            lnode[d] = lnode[d]->r;
        }

        while (bnode[d]) {
            if (!bnode[d]->match)
                return(NO);
            bnode[d] = bnode[d]->r;
        }
    }

    //  determine the winning cards
    lnode[d] = lnode[d - 1]->d;
    lnode[d - 1]->w_c = (d - 1) % 4 ? RENONCE : lnode[d - 1]->w_c;
    while (lnode[d]) {
        if ((d - 1) % 4)
            lnode[d - 1]->w_c |= lnode[d]->w_c;
        lnode[d] = lnode[d]->r;
    }

    return(YES);
}
