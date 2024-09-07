//  name:
//      merge_quirkd
//
//  purpose:
//      merge_quirkd merges two QUIRKD trees
//
//  parameters:
//      d               in      the depth in the NODE tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

#define bottumup( a, b, bquirkd, bquirkdl )     if ( a && a > bquirkd[b]->il && bquirkdl[b]->h->t[a-1].i >= i ) a--; else if ( a && a == bquirkd[b]->il && b && bquirkdl[b-1]->h->t[a-1].i >= i ) { a--; b--; }

void merge_quirkd( int d )
{
    int           a = 0, b = 0, i, ih = 0, k = 0, l = 0, ns, s = 0, t = 0;
    struct QUIRKD *bquirkd[NBOARDS], *bquirkdl[NBOARDS], *lquirkd[NBOARDS], *lquirkdl[NBOARDS], *tquirkd[NBOARDS], *tquirkdl[NBOARDS];

    //  initialise
    bquirkdl[b] = &quirkd[d];
    bquirkd[b] = bquirkdl[b]->h ? bquirkdl[b]->h->d : NIL;
    lquirkdl[l] = &quirkd[d+1];
    lquirkd[l] = lquirkdl[l]->h ? lquirkdl[l]->h->d : NIL;
    tquirkdl[t] = &quirkd[d+2];
    bump_quirkh( tquirkdl[t]->h );
    bump_quirkd( tquirkdl[t]->h->d );
    tquirkd[t] = tquirkdl[t]->h->d;
    tquirkd[t]->ic = boardcount;
    tquirkd[t]->il = 0;
    tquirkd[t]->ih = 0;

    //  initialise the target QUIRKD tree
    i = a <= bquirkd[b]->ih && k <= lquirkd[l]->ih
        ? MIN( bquirkdl[b]->h->t[a].i, lquirkdl[l]->h->t[k].i )
        : a <= bquirkd[b]->ih ? bquirkdl[b]->h->t[a].i : lquirkdl[l]->h->t[k].i;
    ns = a <= bquirkd[b]->ih && k <= lquirkd[l]->ih && lquirkdl[l]->h->t[k].i == bquirkdl[b]->h->t[a].i
        ? MIN( lquirkdl[l]->h->t[k].ns, bquirkdl[b]->h->t[a].ns )
        : a <= bquirkd[b]->ih && bquirkdl[b]->h->t[a].i == i ? bquirkdl[b]->h->t[a].ns : lquirkdl[l]->h->t[k].ns;
    tquirkdl[t]->h->t[s].i = i;
    tquirkdl[t]->h->t[s].ns = ns;

    //  the main loop of the routine; all source QUIRKD nodes are combined
loop_down:
    while ( YES ) {

        i = a <= bquirkd[b]->ih && k <= lquirkd[l]->ih
            ? MIN( bquirkdl[b]->h->t[a].i, lquirkdl[l]->h->t[k].i )
            : a <= bquirkd[b]->ih ? bquirkdl[b]->h->t[a].i : lquirkdl[l]->h->t[k].i;
        ns = a <= bquirkd[b]->ih && k <= lquirkd[l]->ih && lquirkdl[l]->h->t[k].i == bquirkdl[b]->h->t[a].i
            ? MIN( lquirkdl[l]->h->t[k].ns, bquirkdl[b]->h->t[a].ns )
            : a <= bquirkd[b]->ih && bquirkdl[b]->h->t[a].i == i ? bquirkdl[b]->h->t[a].ns : lquirkdl[l]->h->t[k].ns;

        t = add_quirkd( i, ns, s, t, ih, tquirkd, tquirkdl );

        if ( a <= bquirkd[b]->ih && bquirkdl[b]->h->t[a].i == i ) {
            a++;
            if ( a > bquirkd[b]->ih && bquirkd[b]->d ) {
                b++;
                bquirkdl[b] = bquirkdl[b-1];
                bquirkd[b] = bquirkd[b-1]->d;
            }
        }

        if ( k <= lquirkd[l]->ih && lquirkdl[l]->h->t[k].i == i ) {
            k++;
            if ( k > lquirkd[l]->ih && lquirkd[l]->d ) {
                l++;
                lquirkdl[l] = lquirkdl[l-1];
                lquirkd[l] = lquirkd[l-1]->d;
            }
        }

        if ( a > bquirkd[b]->ih && k > lquirkd[l]->ih )
            break;

        s++;
    }

    ih = s;

    //  the bottomup loop
    while ( YES ) {

        bottumup( a, b, bquirkd, bquirkdl );
        bottumup( k, l, lquirkd, lquirkdl );

        if ( k == lquirkd[l]->il && lquirkdl[l]->h->t[k].i == i ) {
            if ( lquirkd[l]->h ) {
                lquirkdl[l] = lquirkd[l];
                lquirkd[l] = lquirkdl[l]->h->d;
                goto loop_down;
            }
            lquirkdl[l] = l ? lquirkdl[l-1] : &quirkd[d+1];
            lquirkd[l] = l ? lquirkd[l-1]->d : lquirkdl[l]->h->d;
        }

        if ( a == bquirkd[b]->il && bquirkdl[b]->h->t[a].i == i ) {
            if ( bquirkd[b]->h ) {
                bquirkdl[b] = bquirkd[b];
                bquirkd[b] = bquirkdl[b]->h->d;
                if ( s == tquirkd[t]->il && tquirkdl[t]->h->t[s].i == i ) {
                        tquirkdl[t] = t ? tquirkdl[t-1] : &quirkd[d+2];
                        tquirkd[t] = t ? tquirkd[t-1]->d : tquirkdl[t]->h->d;
                }
                goto loop_down;
            }
            bquirkdl[b] = b ? bquirkdl[b-1] : &quirkd[d];
            bquirkd[b] = b ? bquirkd[b-1]->d : bquirkdl[b]->h->d;
        }

        if ( ! s )
            break;

        //  backtrack one board
        s--;
        if ( s < tquirkd[t]->il ) {
            check_quirkd( d+2, t, tquirkd, tquirkdl );
            t--;
        }
        i = tquirkdl[t]->h->t[s].i;
    }

    //  cleanup
    delete_quirkd( d );
    delete_quirkd( d+1 );
    quirkd[d].h = quirkd[d+2].h;
    check_quirkd( d, t, tquirkd, tquirkdl );
}
