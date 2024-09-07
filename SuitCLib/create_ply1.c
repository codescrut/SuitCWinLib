//  name:
//      create_ply1
//
//  purpose:
//      create_ply1 handles the lead
//
//  parameters:
//      d               in      the depth in the tree
//      lead[4]         in      flag to indicate valid leads
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void create_ply1( int d, int lead[4] )
{
    int          i, p, t = st[d].t;
    struct NODE  znode;
    struct SCARD *tc;

    wnode[d] = &znode;
    znode.r = NIL;
    for ( p = 0; p < 4; p++ ) {

        if ( lead[p] ) {

            //  adjust the number of free entries
            nfree[t][west] = nfree[t-1][west];
            nfree[t][east] = nfree[t-1][east];
            nfree[t][north] = nfree[t-1][north];
            nfree[t][south] = nfree[t-1][south];
            if ( ( t > 1 ) && ( w[t-1] != p ) )
                nfree[t][p] -= 1;

            //  investigate all SCARD sequences
            tc = &scards[t][p];
            for ( i = 0; i <= tc->n; i++ ) {

                st[d].c = tc->c[i];
                st[d].w = p;
                st[d].winc = tc->c[i];

                //  check for a specified opening lead
                if ( t == 1 && leader == p && ! ( st[d].c & opening_lead ) )
                    continue;

                //  create a new node
                if ( ! add_node( d, p, tc->c[i] ) )
                    continue;

                //  handle the 2nd hand play
                create_ply2( d+1, (p+1)%4 );
            }
        }
    }

    //  check the plays
    znode.r->l = NIL;
    wnode[d-1]->d = znode.r;
    NS(znode.r->p) ? check_ns_plays( d, wnode ) : check_ew_plays( d, wnode );
}
