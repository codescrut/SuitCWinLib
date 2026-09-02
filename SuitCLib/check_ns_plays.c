//  name:
//      check_ns_plays
//
//  purpose:
//      check_ns_plays checks the sons against each other.
//
//  parameters:
//      d               in      the depth in the NODE tree
//      lnode           in      the NODE tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void check_ns_plays(int d, struct NODE** lnode)
{
    int          bstrong, bweak, lstrong, lweak;
    struct NODE* bnode[NPLIES + 1];
    struct INDEX* lindex, * bindex;

    //  refuse rather than index the walker stacks past their end: a corrupted
    //  ->d chain must not reach the /GS cookie below bnode[]/lnode[]
    if (d < 1 || d > MAXPLY) {
        treeoverflow = YES;
        return;
    }

    //  compare all siblings
    bnode[d - 1] = lnode[d - 1];
    lnode[d] = lnode[d - 1]->d;
next_lnode:
    while (lnode[d]) {
        bnode[d] = lnode[d]->r;

    next_bnode:
        while (bnode[d]) {

            //  initialise
            bstrong = YES;
            lstrong = YES;
            bweak = YES;
            lweak = YES;

            //  compare the result for all EW distributions
            lindex = lnode[d]->i;
            bindex = bnode[d]->i;
            while (lindex) {

                //  mark plays that are worse than the minimum that can be won
                if (buildtree == 2 && bindex->max < nstricks[lindex->i]) {
                    bnode[d] = delete_tree(d, bnode);
                    goto next_bnode;
                }
                if (buildtree == 2 && lindex->max < nstricks[lindex->i]) {
                    lnode[d] = delete_tree(d, lnode);
                    goto next_lnode;
                }

                //  check the number of tricks
                lstrong &= lindex->min > bindex->max;
                bstrong &= bindex->min > lindex->max;
                lweak &= lindex->min >= bindex->max;
                bweak &= bindex->min >= lindex->max;

                lindex = lindex->r;
                bindex = bindex->r;
            }

            //  select the plays
            if (lstrong)
                bnode[d] = delete_tree(d, bnode);

            else if (bstrong) {
                lnode[d] = delete_tree(d, lnode);
                goto next_lnode;
            }

            else if (bweak && lweak && buildtree)
                bnode[d] = bnode[d]->r;

            else if (bweak && lweak) {
                lnode[d] = delete_tree(d, lnode);
                goto next_lnode;
            }

            else if ((lweak || bweak) && buildtree)
                bnode[d] = bnode[d]->r;

            else if (lweak)
                bnode[d] = delete_tree(d, bnode);

            else if (bweak) {
                lnode[d] = delete_tree(d, lnode);
                goto next_lnode;
            }

            else
                bnode[d] = bnode[d]->r;
        }

        lnode[d] = lnode[d]->r;
    }

    determine_trick_count(d, lnode);
}
