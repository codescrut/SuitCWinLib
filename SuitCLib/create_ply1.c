//  name:
//      create_ply1
//
//  purpose:
//      create_ply1 handles the lead
//
//  parameters:
//      d               in      the depth in the tree
//      lead            in      flag to indicate valid leads
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

int create_ply1(int d, int* lead)
{
    int          i, p, t = (d + 3) / 4;
    struct NODE  znode;

    wnode[d] = &znode;
    znode.r = NIL;
    for (p = 0; p < 4; p++) {

        if (lead[p]) {

            //  adjust the number of free entries
            nfree[t][west] = nfree[t - 1][west];
            nfree[t][east] = nfree[t - 1][east];
            nfree[t][north] = nfree[t - 1][north];
            nfree[t][south] = nfree[t - 1][south];
            if (t > 1 && wnode[d - 1]->w_p != p)
                nfree[t][p] -= 1;

            //  investigate all SCARD sequences
            for (i = 0; i <= scards[t][p].n; i++) {

                //  check for a specified opening lead
                if (t == 1 && leader == p && !(scards[t][p].c[i] & opening_lead))
                    continue;

                //  create a new node
                if (!add_node(d, p, scards[t][p].c[i]))
                    continue;

                //  mark an entry in the suit itself
                wnode[d]->e_down |= t > 1 && wnode[d - 1]->w_p == p ? 1 << (ntricks - t) : 0;

                //  handle the 2nd hand play
                if (create_ply2(d + 1, (p + 1) % 4) != 0) {
                    return -1;
				};
            }
        }
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
