//  name:
//      delete_board
//
//  purpose:
//      delete_board deletes the specified boards from a node and calls
//      itself recursively to delete the boards from each of its sons.
//
//  parameters:
//      d               in      the depth in the NODE tree
//      lnode           in      the NODE tree
//      b               in      the board to be deleted
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void delete_board(int d, struct NODE** lnode, int b)
{
    struct INDEX* bindex, * lindex;

    //  refuse rather than index the walker stacks past their end: a corrupted
    //  ->d chain must not reach the /GS cookie below bnode[]/lnode[]
    if (d < 1 || d > MAXPLY) {
        treeoverflow = YES;
        return;
    }

    //  check each son in turn
    lnode[d] = lnode[d - 1]->d;
    while (lnode[d]) {

        //  delete the marked board
        lindex = lnode[d]->i;
        bindex = NIL;
        while (lindex && lindex->i < b) {
            bindex = lindex;
            lindex = lindex->r;
        }

        if (lindex && lindex->i == b) {
            if (bindex)
                bindex->r = lindex->r;
            else
                lnode[d]->i = lindex->r;
            free_index(lindex);

            //  delete the boards from the sons
            if (lnode[d]->i) {
                if (!ply_overflow(d))
                    delete_board(d + 1, lnode, b);
                lnode[d] = lnode[d]->r;
            }
            else
                lnode[d] = delete_tree(d, lnode);
        }
        else
            lnode[d] = lnode[d]->r;
    }

    //  check the siblings
    if (lnode[d - 1]->d && NS(lnode[d - 1]->d->p))
        check_ns_plays(d, lnode);
    else if (lnode[d - 1]->d)
        check_ew_plays(d, lnode);
}
