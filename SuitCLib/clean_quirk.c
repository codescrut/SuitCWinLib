//  name:
//      clean_quirk.c
//
//  purpose:
//      clean_quirk.c removes QUIRK tree branches that cannot result in the best play
//
//  parameters:
//      d               in      the depth in the NODE tree
//      results         in      the LIMIT array for the plays
//
//  Copyright 2025, Gerrit A. van der Velde, Leiden

#include "header.h"

int clean_quirk(int d, struct LIMIT* results)
{
    int           ix = 0, k, l = 0, nplays = 0;
    struct QUIRKD* lquirkd[NBOARDS], * lquirkdl[NBOARDS];

    //  initialise
    lquirkdl[0] = &quirkdl[d];
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;

    while (lquirkd[l]) {

        //  get the oldest son
        if (lquirkd[l]->d) {
            l++;
            lquirkdl[l] = lquirkdl[l - 1];
            lquirkd[l] = lquirkd[l - 1]->d;
        }

        //  we reached a leaf; delete obsolete quirks
        else {

            ix++;
            if (results[ix].keep)
                nplays++;

            else {

                //  delete the oldest son
                if (l && lquirkd[l] == lquirkd[l - 1]->d) {

                    lquirkd[l - 1]->d = lquirkd[l]->h->d;
                    for (k = lquirkd[l]->il; k <= lquirkd[l]->ih; k++)
                        lquirkdl[l]->h->t[k] = lquirkd[l]->h->t[k];

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

                    continue;
                }

                //  delete the younger son
                else if (lquirkd[l] == lquirkdl[l]->h->d) {

                    free_quirkh(lquirkdl[l]->h);
                    lquirkdl[l]->h = lquirkd[l]->h;
                    free_quirkd(lquirkd[l]);
                    lquirkd[l] = lquirkdl[l];

                    //  no branch exists anymore; delete the node
                    if (l && lquirkd[l]->h == NIL && lquirkd[l] == lquirkd[l - 1]->d) {
                        lquirkd[l - 1]->d = lquirkdl[l]->d;
                        lquirkd[l - 1]->ih = lquirkdl[l]->ih;
                        free_quirkd(lquirkdl[l]);
                        l--;
                    }
                }
            }

            //  get the next sibling
            while (l && lquirkd[l]->h == NIL)
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkd[l]->h ? lquirkd[l]->h->d : NIL;
        }
    }

    free(results);
    return(nplays);
}
