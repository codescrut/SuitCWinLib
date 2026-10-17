//  name:
//      merge_nodes
//
//  purpose:
//      merge_nodes merges the boards of the second node with the boards
//      of the first node to create a new list of boards for the first node.
//
//  parameters:
//      lnode           in      the first sibling
//      bnode           in      the second sibling
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void merge_nodes(struct NODE* lnode, struct NODE* bnode)
{
    int          p;
    struct INDEX* lindex, * bindex, * aindex;

    //  add the played cards
    lnode->c |= bnode->c;
    lnode->w_c |= bnode->w_c;

    //  merge the boards
    aindex = NIL;
    lindex = lnode->i;
    bindex = bnode->i;
    while (lindex || bindex) {
        if (lindex && bindex) {
            if (lindex->i < bindex->i) {
                aindex = lindex;
                lindex = lindex->r;
            }
            else if (lindex->i == bindex->i) {
                aindex = lindex;
                lindex = lindex->r;
                bindex = bindex->r;
            }
            else if (lindex->i > bindex->i) {
                if (aindex)
                    aindex->r = nextindex;
                else
                    lnode->i = nextindex;
                bump_index(aindex);
                aindex->i = bindex->i;
                for (p = 0; p < 4; p++)
                    aindex->c[p] = bindex->c[p];
                aindex->min = bindex->min;
                aindex->max = bindex->max;
                aindex->r = lindex;
                bindex = bindex->r;
            }
        }
        else if (lindex) {
            aindex = lindex;
            lindex = lindex->r;
        }
        else if (bindex) {
            if (aindex)
                aindex->r = nextindex;
            else
                lnode->i = nextindex;
            bump_index(aindex);
            aindex->i = bindex->i;
            for (p = 0; p < 4; p++)
                aindex->c[p] = bindex->c[p];
            aindex->min = bindex->min;
            aindex->max = bindex->max;
            aindex->r = lindex;
            bindex = bindex->r;
        }
    }
}
