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
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void create_ply4( int d, int p )
{
    int          i, t = st[d].t;
    struct INDEX *lindex;
    struct NODE  znode;
    struct SCARD *tc;

    wnode[d] = &znode;
    znode.r = NIL;
    tc = &scards[t][p];
    for ( i = 0; i <= tc->m; i++ ) {

        st[d].c = tc->c[i];
        st[d].w = st[d-1].winc > tc->c[i] ? st[d-1].w : p;
        st[d].winc = st[d-1].winc > tc->c[i] ? st[d-1].winc : tc->c[i];

        //  create a new node
        if ( ! add_node( d, p, tc->c[i] ) )
            continue;

        //  determine the winner of the trick
        w[t] = st[d].w;
        wnode[d]->winc = st[d].winc;
        ns[t] = ns[t-1] + ( defence ? NS(w[t]) ? 0 : -1 : NS(w[t]) ? 1 : 0 );

        //  store the number of north-south winners
        lindex = wnode[d]->i;
        while ( lindex ) {
            lindex->max = ns[t];
            lindex->min = lindex->max;
            lindex = lindex->r;
        }

        //  handle the next trick
        if ( t == tricklimit )
            wnode[d]->term = YES;
        else
            create_tricks( d+1 );
    }

    //  check the plays
    znode.r->l = NIL;
    wnode[d-1]->d = znode.r;
    NS(znode.r->p) ? check_ns_plays( d, wnode ) : check_ew_plays( d, wnode );
}
