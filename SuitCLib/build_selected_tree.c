//  name:
//      build_selected_tree
//
//  purpose:
//      build_selected_tree builds the NODE tree that mirrors the selected result
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void build_selected_tree(void)
{
    int           accept, d = 0;
    struct INDEX* lindex;
    struct NODE* lnode[NPLIES + 1];

    lnode[0] = wnode[0];
    tnode[0] = NIL;

    while (lnode[d]) {

        //  check the number of tricks for north and south
        accept = YES;
        if (NS(lnode[d]->p) && (tnode[d] || lnode[d]->r)) {
            lindex = lnode[d]->i;
            while (lindex) {
                if (lindex->max < nstricks[lindex->i])
                    accept = NO;
                lindex = lindex->r;
            }
        }

        //  the node is accepted
        if (accept) {

            //  copy the tree if it contains no forks and the node if it contains forks
            if (tnode[d]) {
                tnode[d]->r = lnode[d]->term ? copy_tree(d, lnode[d]) : copy_node(lnode[d]);
                tnode[d]->r->l = tnode[d];
                tnode[d] = tnode[d]->r;
            }

            else {
                tnode[d] = lnode[d]->term ? copy_tree(d, lnode[d]) : copy_node(lnode[d]);
                tnode[d]->l = NIL;
                if (d)
                    tnode[d - 1]->d = tnode[d];
            }

            //  get the next sibling
            if (lnode[d]->term)
                lnode[d] = lnode[d]->r;

            //  get the son
            else {
                d++;
                lnode[d] = lnode[d - 1]->d;
                tnode[d] = NIL;
            }
        }

        //  the node is not accepted; continue with the next sibling
        else
            lnode[d] = lnode[d]->r;

        //  get the next node
        while (d && !lnode[d]) {

            //   check the result
            //   tnode[d] is NIL when every sibling at this depth was rejected
            //   (lindex->max < nstricks[..] for all of them); there is then no
            //   selected subtree to check -- dereferencing it is an access violation.
            if (tnode[d]) {
                if (NS(tnode[d]->p))
                    check_ns_plays(d, tnode);
                else
                    check_ew_plays(d, tnode);
            }

            d--;
            lnode[d] = lnode[d]->r;
        }
    }
}
