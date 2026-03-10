//  name:
//      count_quirk
//
//  purpose:
//      count_quirk counts the number of leaves in the QUIRK tree
//
//  parameters:
//      d               in      the depth in the NODE tree
//
//  Copyright 2025, Gerrit A. van der Velde, Leiden

#include "header.h"

int count_quirk(int d)
{
    int           ix = 0, l = 0;
    struct QUIRKD* lquirkd[NBOARDS], * lquirkdl[NBOARDS];

    //  initialize
    lquirkdl[0] = &quirkdl[d];
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;

    while (lquirkd[l]) {

        //  get the oldest son
        if (lquirkd[l]->d) {
            l++;
            lquirkdl[l] = lquirkdl[l - 1];
            lquirkd[l] = lquirkd[l - 1]->d;
        }

        //  get the next sibling
        else {
            ix++;
            while (l && lquirkd[l]->h == NIL)
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkd[l]->h ? lquirkd[l]->h->d : NIL;
        }
    }

    return(ix);
}
