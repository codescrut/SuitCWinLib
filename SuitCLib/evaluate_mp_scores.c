//  name:
//      evaluate_mp_scores
//
//  purpose:
//      evaluate_mp_scores determines the optimal plays in case that
//      the MP or IMP score is taken into account.
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void evaluate_mp_scores(void)
{
    int           b, bx, i, ih, ix = 0, k, l = 0, v;
    int           imptable[] = { 10, 40, 80, 120, 160, 210, 260, 310, 360, 420, 490, 590, 740, 890,
                                 1090, 1290, 1490, 1740, 1990, 2240, 2490, 2990, 3490, 3990 };
    double        mpscore[NBOARDS], tsum;
    struct QUIRKD* bquirkd[NBOARDS], * bquirkdl[NBOARDS], * lquirkd[NBOARDS], * lquirkdl[NBOARDS];
    struct RESULT result[NBOARDS];
    struct SCORE* ascore, * lscore;

    //  create the links for the SCORE entries
    score = (struct SCORE*)malloc((nplays + 2) * sizeof(struct SCORE));
    score[0].l = NIL;
    for (i = 0; i <= nplays; i++) {
        score[i + 1].l = &score[i];
        score[i].r = &score[i + 1];
        score[i].b = NIL;
        score[i].ix = i - 1;
        score[i].mp = NIL;
        score[i].mpscore = 0.0;
    }
    score[nplays + 1].r = NIL;

    //  initialise
    lquirkdl[0] = quirkdl;
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;

    //  loop over the QUIRK tree
    while (lquirkd[l]) {

        //  compute the results
        for (k = lquirkd[l]->il; k <= lquirkd[l]->ih; k++) {
            result[k].result = prob[k] * dupl[k] * lquirkdl[l]->h->t[k].ns + (k ? result[k - 1].result : 0.0);
            for (i = minns; i <= maxns; i++)
                result[k].tricks[i] = k ? result[k - 1].tricks[i] : 0.0;
            result[k].tricks[lquirkdl[l]->h->t[k].ns] += prob[k] * dupl[k];
        }

        //  get the oldest son
        if (lquirkd[l]->d) {
            l++;
            lquirkdl[l] = lquirkdl[l - 1];
            lquirkd[l] = lquirkd[l - 1]->d;
        }

        //  we reached a leaf; process the result
        else {

            //  store the score
            ix++;
            score[ix].ix = ix - 1;
            score[ix].result = result[lquirkd[l]->ih].result;
            score[ix].mp = NIL;

            //  set the best_{MAX|MP|N} flags for each play
            tsum = 0.0;
            ih = lquirkd[l]->ih;
            score[ix].best_N = NO;
            score[ix].best_MP = YES;
            score[ix].best_MAX = score[ix].result > best_result.result - EPS;
            if (ewwinners) {
                for (i = minns; i <= maxns; i++) {
                    tsum += result[ih].tricks[i];
                    score[ix].best_N |= tsum < best_result.tricks[i] + EPS && result[ih].result > best_result.res[i] - EPS;
                }
            }
            else {
                for (i = maxns; i >= minns; i--) {
                    tsum += result[ih].tricks[i];
                    score[ix].best_N |= tsum > best_result.tricks[i] - EPS && result[ih].result > best_result.res[i] - EPS;
                }
            }

            //  get the next sibling
            while (l && lquirkd[l]->h == NIL)
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkd[l]->h ? lquirkd[l]->h->d : NIL;
        }
    }

    //  initialise
    ix = 0;
    lquirkdl[0] = quirkdl;
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;

    //  loop over the quirk tree
    if (trace & 0x2)
        printf("\n");
    while (lquirkd[l]) {

        //  get the oldest son
        if (lquirkd[l]->d) {
            l++;
            lquirkdl[l] = lquirkdl[l - 1];
            lquirkd[l] = lquirkd[l - 1]->d;
        }

        //  we reached a leaf
        else {

            //  allocate memory for the MP or IMP scores
            ix++;
            lscore = &score[ix];
            lscore->mp = (double *) malloc(nplays * sizeof(double));
            for (i = 0; i < nplays; i++)
                lscore->mp[i] = 0.0;

            //  perform the second loop over the QUIRK tree
            b = 0;
            bx = 0;
            bquirkdl[0] = quirkdl;
            bquirkd[0] = bquirkdl[0]->h ? bquirkdl[0]->h->d : NIL;
            while (bquirkd[b]) {

                //  calculate the MP or IMP score
                v = 0;
                while (lquirkd[v]->ih < bquirkd[b]->il)
                    v++;
                for (k = bquirkd[b]->il; k <= bquirkd[b]->ih; k++) {
                    if (lquirkd[v]->ih < k)
                        v++;
                    if (matchpoint == MP)
                        mpscore[k] = lquirkdl[v]->h->t[k].ns > bquirkdl[b]->h->t[k].ns ? 1.0 : lquirkdl[v]->h->t[k].ns < bquirkdl[b]->h->t[k].ns ? -1.0 : 0.0;
                    else if (matchpoint == IMP) {
                        mpscore[k] = pointscore[lquirkdl[v]->h->t[k].ns] - pointscore[bquirkdl[b]->h->t[k].ns];
                        mpscore[k] = mpscore[k] >= 0 ? mpscore[k] : -mpscore[k];
                        i = 24;
                        while (i && imptable[i - 1] >= mpscore[k])
                            i--;
                        mpscore[k] = (pointscore[lquirkdl[v]->h->t[k].ns] >= pointscore[bquirkdl[b]->h->t[k].ns] ? i : -i);
                    }
                    mpscore[k] = prob[k] * dupl[k] * mpscore[k];
                    mpscore[k] += (k ? mpscore[k - 1] : 0.0);
                }

                //  get the oldest son
                if (bquirkd[b]->d) {
                    b++;
                    bquirkdl[b] = bquirkdl[b - 1];
                    bquirkd[b] = bquirkd[b - 1]->d;
                }

                //  we reached a leaf; store the MP or IMP score and get the next sibling
                else {
                    bx++;
                    lscore->mp[score[bx].ix] = mpscore[bquirkd[b]->ih];
                    while (b && !bquirkd[b]->h)
                        b--;
                    bquirkdl[b] = bquirkd[b];
                    bquirkd[b] = bquirkdl[b]->h ? bquirkd[b]->h->d : NIL;
                }
            }

            //  compare the scores and unlink dominated ones
            check_zermelo(lscore);

            //  get the next node
            while (l && !lquirkd[l]->h)
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkdl[l]->h ? lquirkd[l]->h->d : NIL;
        }
    }
}
