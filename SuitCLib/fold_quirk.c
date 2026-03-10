//  name:
//      fold_quirk
//
//  purpose:
//      fold_quirk folds two QUIRK trees
//
//  parameters:
//      d               in      the depth in the NODE tree
//
//  Copyright 2025, Gerrit A. van der Velde, Leiden

#include "header.h"

void fold_quirk(int d)
{
    int           ih = 0, l = 0, s = 0, t = 0;
    struct QUIRKD* lquirkd[NBOARDS], * lquirkdl[NBOARDS], * tquirkd[NBOARDS], * tquirkdl[NBOARDS];

    //  initialise
    lquirkdl[l] = &quirkdl[d];
    lquirkd[l] = lquirkdl[l]->h ? lquirkdl[l]->h->d : NIL;
    tquirkdl[t] = &quirkdl[d - 1];
    tquirkd[t] = tquirkdl[t]->h ? tquirkdl[t]->h->d : NIL;

    //  determine the highest index in the TRICKS array
    while (lquirkd[l]->d) {
        l++;
        lquirkd[l] = lquirkd[l - 1]->d;
    }
    ih = lquirkd[l]->ih;

    //  loop over all nodes of the source QUIRK tree
    l = 0;
    while (lquirkd[l]) {

        //  add a new node to the target QUIRK tree
        t = add_quirk(lquirkdl[l]->h->t[s].i, lquirkdl[l]->h->t[s].ns, s, t, ih, tquirkd, tquirkdl);

        //  get the oldest son
        if (s < lquirkd[l]->ih || lquirkd[l]->d) {
            s++;
            if (s > lquirkd[l]->ih) {
                l++;
                lquirkdl[l] = lquirkdl[l - 1];
                lquirkd[l] = lquirkd[l - 1]->d;
            }
        }

        //  we reached a leaf
        else {

            //  backtrack untill a branch in the source QUIRK tree
            while (l && !lquirkd[l]->h)
                l--;
            s = lquirkd[l]->il;

            //  backtrack the target QUIRK tree and check each branch
            while (tquirkd[t]->il > s) {
                check_quirk(d - 1, t, tquirkd, tquirkdl);
                t--;
            }

            //  get the younger sibling in the source QUIRK tree
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkdl[l]->h ? lquirkdl[l]->h->d : NIL;
        }
    }

    //  check the root of the target QUIRK tree
    delete_quirk(d);
    if (quirkdl[d - 1].h->d->h)
        check_quirk(d - 1, t, tquirkd, tquirkdl);
}
