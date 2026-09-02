//  name:
//      delete_tree
//
//  purpose:
//      delete_tree deletes a NODE tree
//
//  parameters:
//      d_              in      the depth in the NODE tree
//      lnode           in      the NODE tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

struct NODE* delete_tree(int d_, struct NODE** lnode)
{
    int          d = d_;
    struct INDEX* lindex;
    struct NODE  znode;

    //  refuse rather than index the caller's lnode[] past its end
    if (d_ < 0 || d_ > MAXPLY) {
        treeoverflow = YES;
        return(NIL);
    }

    //  loop over the nodes
    while (lnode[d]) {

        //  handle the sons
        if (lnode[d]->d) {

            //  the caller's lnode[] holds MAXPLY+1 entries; refuse the descend
            //  instead of writing past its end
            if (ply_overflow(d))
                return(NIL);

            d++;
            lnode[d] = lnode[d - 1]->d;
        }

        else {

            //  delete the boards
            lindex = lnode[d]->i;
            while (lindex) {
                lnode[d]->i = lindex->r;
                free_index(lindex);
                lindex = lnode[d]->i;
            }

            //  delete the node and get the next one
            if (lnode[d]->l)
                lnode[d]->l->r = lnode[d]->r;
            else if (d)
                lnode[d - 1]->d = lnode[d]->r;
            if (lnode[d]->r)
                lnode[d]->r->l = lnode[d]->l;
            znode.r = lnode[d]->r;
            free_node(lnode[d]);
            lnode[d] = znode.r;

            //  we are finished; return the right sibling
            if (d == d_)
                return(lnode[d]);

            //  go back to the parent
            if (!lnode[d])
                d--;
        }
    }
    return(NIL);
}
