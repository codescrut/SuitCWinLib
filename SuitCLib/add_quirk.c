//  name:
//      add_quirk
//
//  purpose:
//      add_quirk adds a new entry to the QUIRK tree
//
//  parameters:
//      i               in      the board index
//      ns              in      the number of tricks won by north-south
//      s               in      the index in the TRICKS array
//      t               in      the depth in the QUIRK tree
//      ih              in      the last entry in the TRICKS array
//      tquirkd         in/out  the target QUIRK tree
//      tquirkdl        in/out  the left sibling of the current QUIRK branch
//
//  Copyright 2025, Gerrit A. van der Velde, Leiden

#include "header.h"

int add_quirk(int i, int ns, int s, int t, int ih, struct QUIRKD** tquirkd, struct QUIRKD** tquirkdl)
{
    int           h;
    struct QUIRKD* tquirkdn;
    struct QUIRKH* tquirkhn;

next_branch:

    //  we are building a new branch of the QUIRK tree; add this board
    if (s == tquirkd[t]->ih + 1 && !tquirkd[t]->d) {
        tquirkd[t]->ih = s;
        tquirkdl[t]->h->t[s].i = i;
        tquirkdl[t]->h->t[s].ns = ns;
    }

    //  the new board matches the current board
    else if (ns == tquirkdl[t]->h->t[s].ns) {

        //  there is a son, so this son becomes the current board
        if (s == tquirkd[t]->ih && tquirkd[t]->d) {
            t++;
            tquirkdl[t] = tquirkdl[t - 1];
            tquirkd[t] = tquirkd[t - 1]->d;
        }

        //  there is no son, because we are building a new branch
        else;
    }

    //  the new board is an older sibling of the current board
    else if (ns > tquirkdl[t]->h->t[s].ns) {

        //  we are at the root of the current branches
        if (s == tquirkd[t]->il) {

            //  the new sibling has an older and younger sibling
            if (tquirkd[t] == tquirkdl[t]->h->d) {
                bump_quirkh(tquirkhn);
                bump_quirkd(tquirkhn->d);
                tquirkhn->d->h = tquirkdl[t]->h;
                tquirkdl[t]->h = tquirkhn;
                tquirkd[t] = tquirkdl[t]->h->d;
                tquirkd[t]->il = s;
                tquirkd[t]->ih = s;
                tquirkdl[t]->h->t[s].i = i;
                tquirkdl[t]->h->t[s].ns = ns;
            }

            //  the new sibling is the oldest sibling
            else if (tquirkd[t] == tquirkd[t - 1]->d) {
                bump_quirkd(tquirkdn);
                bump_quirkh(tquirkdn->h);
                tquirkd[t - 1]->d = tquirkdn;
                tquirkdn->h->d = tquirkd[t];
                tquirkdn->il = s;
                tquirkdn->ih = s;
                for (h = s; h <= ih; h++)
                    tquirkdn->h->t[h] = tquirkdl[t]->h->t[h];
                tquirkdl[t]->h->t[s].i = i;
                tquirkdl[t]->h->t[s].ns = ns;
                tquirkd[t] = tquirkdn;
            }
        }

        //  we split the branch at the current board
        else if (s > tquirkd[t]->il) {
            bump_quirkd(tquirkdn);
            bump_quirkh(tquirkdn->h);
            bump_quirkd(tquirkdn->h->d);
            tquirkdn->h->d->d = tquirkd[t]->d;
            tquirkd[t]->d = tquirkdn;
            tquirkdn->il = s;
            tquirkdn->ih = s;
            tquirkdn->h->d->il = s;
            tquirkdn->h->d->ih = tquirkd[t]->ih;
            tquirkd[t]->ih = s - 1;
            for (h = s; h <= ih; h++)
                tquirkdn->h->t[h] = tquirkdl[t]->h->t[h];
            tquirkdl[t]->h->t[s].i = i;
            tquirkdl[t]->h->t[s].ns = ns;
            t++;
            tquirkd[t] = tquirkdn;
            tquirkdl[t] = tquirkdl[t - 1];
        }
    }

    //  the new board is an younger sibling of the current board
    else if (ns < tquirkdl[t]->h->t[s].ns) {

        //  we are at the root of the current branches
        if (s == tquirkd[t]->il) {

            //  a younger sibling exists; check this sibling
            if (tquirkd[t]->h) {
                tquirkdl[t] = tquirkd[t];
                tquirkd[t] = tquirkdl[t]->h->d;
                goto next_branch;
            }

            //  the board is the youngest sibling
            else if (tquirkd[t]->h == NIL) {
                bump_quirkh(tquirkhn);
                bump_quirkd(tquirkhn->d);
                tquirkd[t]->h = tquirkhn;
                tquirkhn->d->il = s;
                tquirkhn->d->ih = s;
                tquirkhn->t[s].i = i;
                tquirkhn->t[s].ns = ns;
                tquirkdl[t] = tquirkd[t];
                tquirkd[t] = tquirkhn->d;
            }
        }

        //  we split the branch at the current board
        else if (s > tquirkd[t]->il) {
            bump_quirkd(tquirkdn);
            bump_quirkh(tquirkdn->h);
            bump_quirkd(tquirkdn->h->d);
            tquirkdn->d = tquirkd[t]->d;
            tquirkd[t]->d = tquirkdn;
            tquirkdn->il = s;
            tquirkdn->ih = tquirkd[t]->ih;
            tquirkdn->h->d->il = s;
            tquirkdn->h->d->ih = s;
            tquirkd[t]->ih = s - 1;
            tquirkdn->h->t[s].i = i;
            tquirkdn->h->t[s].ns = ns;
            t++;
            tquirkdl[t] = tquirkdn;
            tquirkd[t] = tquirkdn->h->d;
        }
    }

    return(t);
}
