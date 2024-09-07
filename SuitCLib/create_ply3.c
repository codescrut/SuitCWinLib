//  name:
//      create_ply3
//
//  purpose:
//      create_ply3 handles the 3rd hand play
//
//  parameters:
//      d               in      the depth in the tree
//      p               in      the player of this ply
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void create_ply3( int d, int p )
{
    int          i, n, t = st[d].t;
    struct NODE  znode;
    struct SCARD *tc;

    wnode[d] = &znode;
    znode.r = NIL;
    tc = &scards[t][p];
    for ( i = 0; i <= tc->m; i++ ) {

        st[d].c = tc->c[i];
        st[d].w = st[d-1].winc > tc->c[i] ? st[d-1].w : p;
        st[d].winc = st[d-1].winc > tc->c[i] ? st[d-1].winc : tc->c[i];

        //  check for selected third hand strategies in trick one
        if ( d == 3 && nrules )
            for ( n = 0; n < nrules; n++ )
                if ( wnode[d-1]->c & h2rule[n] && ! ( st[d].c & h3rule[n] ) )
                    goto next_i;

        //  create a new node
        if ( ! add_node( d, p, tc->c[i] ) )
            continue;

        //  handle the 4th hand play
        create_ply4( d+1, (p+1)%4 );

    next_i:
        continue;
    }

    //  check for errors in the specification of the third hand strategy
    if ( nrules && ! znode.r ) {
        printf( "ERROR: The third hand strategy is incorrectly specified\n" );
        exit( 1 );
    }

    //  check the plays
    znode.r->l = NIL;
    wnode[d-1]->d = znode.r;
    NS(znode.r->p) ? check_ns_plays( d, wnode ) : check_ew_plays( d, wnode );
}
