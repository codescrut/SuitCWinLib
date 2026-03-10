//  name:
//      delete_quirk
//
//  purpose:
//      delete_quirk deletes a QUIRK tree
//
//  parameters:
//      d               in      the depth in the tree
//
//  Copyright 2025, Gerrit A. van der Velde, Leiden

#include "header.h"

void delete_quirk(int d)
{
    int           l = 0;
    struct QUIRKD* lquirkd[NBOARDS];

    lquirkd[l] = quirkdl[d].h->d;
    while (quirkdl[d].h) {

        //  get the oldest son
        if (lquirkd[l]->d) {
            l++;
            lquirkd[l] = lquirkd[l - 1]->d;
        }

        //  get the next sibling at the root of the QUIRK tree son
        else if (l == 0) {
            free_quirkh(quirkdl[d].h);
            quirkdl[d].h = lquirkd[l]->h;
            free_quirkd(lquirkd[l]);
            if (quirkdl[d].h)
                lquirkd[l] = quirkdl[d].h->d;
        }

        //  get the next sibling
        else if (l && lquirkd[l] == lquirkd[l - 1]->d && lquirkd[l]->h) {

            lquirkd[l - 1]->d = lquirkd[l]->h->d;
            free_quirkh(lquirkd[l]->h);
            free_quirkd(lquirkd[l]);
            lquirkd[l] = lquirkd[l - 1]->d;

            //  no branch exists anymore; delete the node
            if (lquirkd[l]->h == NIL) {
                lquirkd[l - 1]->d = lquirkd[l]->d;
                lquirkd[l - 1]->ih = lquirkd[l]->ih;
                free_quirkd(lquirkd[l]);
                l--;
            }
        }
    }
}
