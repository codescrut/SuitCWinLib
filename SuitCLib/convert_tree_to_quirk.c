//  name:
//      convert_tree_to_quirk
//
//  purpose:
//      convert_tree_to_quirk converts the NODE tree to QUIRK tree. For
//      each terminal node it builds a QUIRK node with calls to bump_quirkd
//      and bump_quirkh. Thereafter it combines NS nodes with calls to
//      fold_quirk, and EW nodes by calls to merge_quirk.
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void convert_tree_to_quirk(void)
{
    int          d = 1, i;
    struct INDEX* lindex;

    //  the tree contains no more branches
    if (wnode[0]->term) {
        i = -1;
        bump_quirkh(quirkdl[0].h);
        bump_quirkd(quirkdl[0].h->d);
        lindex = wnode[0]->i;
        while (lindex) {
            quirkdl[0].h->t[++i].i = lindex->i;
            quirkdl[0].h->t[i].ns = lindex->max;
            lindex = lindex->r;
        }
        quirkdl[0].h->d->il = 0;
        quirkdl[0].h->d->ih = i;
        return;
    }

    //  loop over all branches of the NODE tree
    quirkdl[d - 1].h = NIL;
    wnode[d] = wnode[d - 1]->d;
    while (wnode[d]) {

        //  get the oldest son
        if (wnode[d]->d && !ply_overflow(d)) {
            d++;
            quirkdl[d - 1].h = NIL;
            wnode[d] = wnode[d - 1]->d;
        }

        //  we reached a leaf
        else {

            //  build the QUIRK tree
            i = -1;
            bump_quirkh(quirkdl[d].h);
            bump_quirkd(quirkdl[d].h->d);
            lindex = wnode[d]->i;
            while (lindex) {
                quirkdl[d].h->t[++i].i = lindex->i;
                quirkdl[d].h->t[i].ns = lindex->max;
                lindex = lindex->r;
            }
            quirkdl[d].h->d->il = 0;
            quirkdl[d].h->d->ih = i;

            //  build the QUIRD tree and get the next node in the NODE tree
            while (YES) {

                if (quirkdl[d - 1].h == NIL)
                    quirkdl[d - 1].h = quirkdl[d].h;

                //  fold north-south QUIRK trees
                else if (NS(wnode[d]->p))
                    fold_quirk(d);

                //  merge east-west QUIRK trees
                else if (EW(wnode[d]->p)) {

                    //  reduce the number of branches in the QUIRK trees
                    if (cutoff && d <= 4 && (long)count_quirk(d - 1) * (long)count_quirk(d) > cutoff)
                        limit_quirk(d - 1);

                    merge_quirk(d);
                }

                if (wnode[d]->r) {
                    wnode[d] = wnode[d]->r;
                    break;
                }

                d--;
                if (!d)
                    return;
            }
        }
    }
}
