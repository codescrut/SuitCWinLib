//  name:
//      create_ply4
//
//  purpose:
//      create_ply4 handles the 4th hand play
//
//  parameters:
//      d               in      the depth in the tree
//      p               in      the player of this ply
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

int create_ply4(int d, int p)
{
    int          i, t = (d + 3) / 4;
    struct INDEX* lindex;
    struct NODE  znode;

    wnode[d] = &znode;
    znode.r = NIL;
    for (i = 0; i <= scards[t][p].m; i++) {

        //  create a new node
        if (!add_node(d, p, scards[t][p].c[i]))
            continue;

        //  determine the winner of the trick
        wnode[d]->ns += ewwinners ? NS(wnode[d]->w_p) ? 0 : -1 : NS(wnode[d]->w_p) ? 1 : 0;

        //  store the number of north-south winners
        lindex = wnode[d]->i;
        while (lindex) {
            lindex->max = wnode[d]->ns;
            lindex->min = lindex->max;
            lindex = lindex->r;
        }

        //  handle the next trick
        if (t == tricklimit)
            wnode[d]->term = YES;
        else
            if (create_tricks(d + 1) != 0) {
                return -1;
            };
    }

    //  no play is possible at this ply (e.g. every board was filtered away by
    //  ncardsmin from -wv/-ev/-wm/-em after played tricks removed cards). Report
    //  the error instead of dereferencing a NIL node.
    if (!znode.r) {
        printf("ERROR: no plays are available at ply %d\n", d);
        return -1;
    }

    //  check the plays
    znode.r->l = NIL;
    wnode[d - 1]->d = znode.r;
    NS(znode.r->p) ? check_ns_plays(d, wnode) : check_ew_plays(d, wnode);
	return 0;
}
