//  name:
//      check_quirk
//
//  purpose:
//      check_quirk compares the branches in a QUIRK tree and deletes all inferior branches.
//      check_quirk is called from fold_quirk and merge_quirk.
//
//  parameters:
//      d               in      the depth in the NODE tree
//      t               in      the depth in the QUIRK tree
//      tquirkd         in      the QUIRK nodes in the QUIRK tree
//      tquirkdl        in      the left siblings of the QUIRK nodes in the QUIRK tree
//
//  Copyright 2025, Gerrit A. van der Velde, Leiden

#include "header.h"

void check_quirk(int d, int t, struct QUIRKD** tquirkd, struct QUIRKD** tquirkdl)
{
    int           a = tquirkd[t]->il, b = t, ic = tquirkd[t]->il, h, l = t;
    struct QUIRKD* lquirkd[NBOARDS], * lquirkdl[NBOARDS];

    //  initialize
    tquirkdl[t] = t ? tquirkdl[t - 1] : &quirkdl[d];
    tquirkd[t] = t ? tquirkd[t - 1]->d : tquirkdl[t]->h->d;
    if (t)
        lquirkd[l - 1] = tquirkd[t - 1];
    lquirkdl[l] = tquirkd[t];
    lquirkd[l] = lquirkdl[l]->h->d;

    while (tquirkd[t]->h) {

        //  loop topdown while i) the subtrees are unchecked, and ii) the l-plays is dominated, and iii) there are sons
        while ((lquirkd[l]->ic > tquirkd[t]->il || tquirkd[b]->ic > tquirkd[t]->il) &&
            lquirkdl[l]->h->t[a].ns <= tquirkdl[b]->h->t[a].ns && (a < lquirkd[l]->ih || lquirkd[l]->d)) {
            a++;
            if (a > tquirkd[b]->ih && tquirkd[b]->d) {
                b++;
                tquirkdl[b] = tquirkdl[b - 1];
                tquirkd[b] = tquirkd[b - 1]->d;
            }
            if (a > lquirkd[l]->ih && lquirkd[l]->d) {
                l++;
                lquirkdl[l] = lquirkdl[l - 1];
                lquirkd[l] = lquirkd[l - 1]->d;
            }
        }

        //  we reached a leaf with the l-branch dominated; delete the l-branch and mend the QUIRK tree
        if (a == lquirkd[l]->ih && !lquirkd[l]->d && lquirkdl[l]->h->t[a].ns <= tquirkdl[b]->h->t[a].ns) {

            //  reset the t-branch
            a = lquirkd[l]->il;
            while (tquirkd[b]->il > a)
                b--;
            if (b > t && tquirkd[b]->il == lquirkd[l]->il) {
                tquirkdl[b] = tquirkdl[b - 1];
                tquirkd[b] = tquirkd[b - 1]->d;
            }

            //  there is a younger brother
            if (lquirkd[l]->h) {  //  ... l --> n ...

                //  there is an older brother; delete the l-branch
                if (lquirkd[l] == lquirkdl[l]->h->d) {  //  ... o --> l --> n ...
                    free_quirkh(lquirkdl[l]->h);
                    lquirkdl[l]->h = lquirkd[l]->h;
                    free_quirkd(lquirkd[l]);
                    lquirkd[l] = lquirkdl[l]->h->d;
                }

                //  there is no older brother
                else if (lquirkd[l] == lquirkd[l - 1]->d) {  //  l --> n ...

                    //  delete the l-branch
                    lquirkd[l - 1]->d = lquirkd[l]->h->d;
                    for (h = lquirkd[l]->il; h <= lquirkd[l]->ih; h++)
                        lquirkdl[l]->h->t[h] = lquirkd[l]->h->t[h];
                    free_quirkh(lquirkd[l]->h);
                    free_quirkd(lquirkd[l]);
                    lquirkd[l] = lquirkd[l - 1]->d;

                    //  there is no younger brother; delete the l-branch
                    if (lquirkd[l]->h == NIL) {  //  l --> n
                        lquirkd[l - 1]->d = lquirkd[l]->d;
                        lquirkd[l - 1]->ih = lquirkd[l]->ih;
                        free_quirkd(lquirkd[l]);
                        l--;
                    }
                }

                //  restart the topdown loop
                continue;
            }

            //  there is no younger brother
            else if (!lquirkd[l]->h) {  //  ... o --> l

                //  delete the l-branch
                free_quirkh(lquirkdl[l]->h);
                lquirkdl[l]->h = lquirkd[l]->h;
                free_quirkd(lquirkd[l]);
                lquirkd[l] = lquirkdl[l];

                //  this isn't the root of the QUIRK tree; delete the current branch
                if (l && lquirkd[l] == lquirkd[l - 1]->d) {  //  o
                    lquirkd[l - 1]->d = lquirkd[l]->d;
                    lquirkd[l - 1]->ih = lquirkd[l]->ih;
                    free_quirkd(lquirkd[l]);
                    if (l > t)
                        l--;
                }
            }
        }

        //  locate the next branches to be compared
        while (YES) {

            a = MAX(lquirkd[l]->il, tquirkd[b]->il);
            if (lquirkd[l]->il == tquirkd[b]->il) {

                a = lquirkd[l]->il;
                if (l > t) {

                    if (lquirkd[l]->h) {
                        lquirkdl[l] = lquirkd[l];
                        lquirkd[l] = lquirkdl[l]->h->d;
                        break;
                    }

                    else if (tquirkd[b]->h) {
                        tquirkdl[b] = tquirkd[b];
                        tquirkd[b] = tquirkdl[b]->h->d;
                        lquirkdl[l] = lquirkdl[l - 1];
                        lquirkd[l] = lquirkd[l - 1]->d;
                        break;
                    }

                    else {
                        tquirkdl[b] = tquirkdl[b - 1];
                        tquirkd[b] = tquirkd[b - 1]->d;
                        b--;
                        lquirkdl[l] = lquirkdl[l - 1];
                        lquirkd[l] = lquirkd[l - 1]->d;
                        l--;
                    }
                }

                else if (l == t) {

                    if (lquirkd[l]->h) {
                        lquirkdl[l] = lquirkd[l];
                        lquirkd[l] = lquirkdl[l]->h->d;
                    }

                    else if (tquirkd[b]->h) {
                        tquirkdl[b] = tquirkd[b];
                        tquirkd[b] = tquirkd[b]->h->d;
                        if (tquirkd[b]->h) {
                            lquirkdl[l] = tquirkd[t];
                            lquirkd[l] = lquirkdl[l]->h->d;
                        }
                    }

                    break;
                }
            }

            else if (tquirkd[b]->il > lquirkd[l]->il) {

                if (tquirkd[b]->h) {
                    tquirkdl[b] = tquirkd[b];
                    tquirkd[b] = tquirkdl[b]->h->d;
                    break;
                }

                else {
                    tquirkdl[b] = tquirkdl[b - 1];
                    tquirkd[b] = tquirkd[b - 1]->d;
                    b--;
                }
            }

            else if (lquirkd[l]->il > tquirkd[b]->il) {

                if (lquirkd[l]->h) {
                    lquirkdl[l] = lquirkd[l];
                    lquirkd[l] = lquirkdl[l]->h->d;
                    break;
                }

                else {
                    lquirkdl[l] = lquirkdl[l - 1];
                    lquirkd[l] = lquirkd[l - 1]->d;
                    l--;
                }
            }
        }
    }

    //  insert the lowest board number for which the QUIRK tree has been checked
    b = t;
    tquirkd[b] = t ? tquirkd[t - 1]->d : quirkdl[d].h->d;
    while (tquirkd[b]) {

        while (tquirkd[b]->ic > tquirkd[t]->il) {
            tquirkd[b]->ic = ic;
            if (!tquirkd[b]->d)
                break;
            b++;
            tquirkd[b] = tquirkd[b - 1]->d;
        }

        while (b > t && tquirkd[b]->h == NIL)
            b--;
        tquirkd[b] = tquirkd[b]->h ? tquirkd[b]->h->d : NIL;
    }

    //  update the lowest board number for the parent QUIRK node
    tquirkd[t] = t ? tquirkd[t - 1]->d : quirkdl[d].h->d;
    if (t == 0) {
        tquirkd[t] = quirkdl[d].h->d;
        while (tquirkd[t]) {
            quirkdl[d].ic = MAX(quirkdl[d].ic, tquirkd[t]->ic);
            tquirkd[t] = tquirkd[t]->h ? tquirkd[t]->h->d : NIL;
        }
    }
    else if (tquirkd[t - 1]->d) {
        tquirkd[t] = tquirkd[t - 1]->d;
        while (tquirkd[t]) {
            tquirkd[t - 1]->ic = MAX(tquirkd[t - 1]->ic, tquirkd[t]->ic);
            tquirkd[t] = tquirkd[t]->h ? tquirkd[t]->h->d : NIL;
        }
    }
    else
        tquirkd[t - 1]->ic = ic;
}
