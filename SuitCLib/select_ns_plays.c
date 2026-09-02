//  name
//      select_ns_plays
//
//  purpose:
//      select_ns_plays selects the favoured human oriented plays
//
//  parameters:
//      d               in      the depth in the NODE tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void select_ns_plays(int d)
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
    lnode[d] = tnode[d - 1]->d;

    //  investigate the sons
    if (tnode[d - 1]->d->d && !ply_overflow(d)) {
        tnode[d] = tnode[d - 1]->d;
        while (tnode[d]) {
            select_ns_plays(d + 1);
            tnode[d] = tnode[d]->r;
        }
    }

    if (NS(lnode[d]->p)) {

    next_lnode:
        while (lnode[d] && lnode[d]->r) {

            bnode[d] = lnode[d]->r;
            while (bnode[d]) {

                //  select the card that minimizes the number of winning east-west cards
                if (w_ew(lnode) > w_ew(bnode)) {
                    lnode[d] = delete_tree(d, lnode);
                    goto next_lnode;
                }
                else if (w_ew(lnode) < w_ew(bnode))
                    bnode[d] = delete_tree(d, bnode);

                //  in 3rd or 4th hand follow suit with the lowest card
                else if ((d - 1) % 4 + 1 >= 3) {
                    lnode[d] = delete_tree(d, lnode);
                    goto next_lnode;
                }

                //  lead the highest winning card
                else if (w_ns(lnode) && w_ns(bnode) && lnode[d]->c < bnode[d]->c) {
                    lnode[d] = delete_tree(d, lnode);
                    goto next_lnode;
                }
                else if (w_ns(lnode) && w_ns(bnode) && lnode[d]->c > bnode[d]->c)
                    bnode[d] = delete_tree(d, bnode);

                //  lead the winning card
                else if (w_ns(bnode)) {
                    lnode[d] = delete_tree(d, lnode);
                    goto next_lnode;
                }
                else if (w_ns(lnode))
                    bnode[d] = delete_tree(d, bnode);

                //  lead the lowest card or lead from the south hand
                else {
                    lnode[d] = delete_tree(d, lnode);
                    goto next_lnode;
                }
            }

            lnode[d] = lnode[d]->r;
        }
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
