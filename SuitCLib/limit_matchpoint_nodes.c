//  name:
//      limit_matchpoint_nodes
//
//  purpose:
//      limit_matchpoint_nodes limits the number of NODES for the MP or IMP evaluation.
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void limit_matchpoint_nodes(void)
{
    int           count, i, ix = 0, ih = 0, k, l = 0;
    double        testlow, testval, testhigh, tsum;
    struct LIMIT* results;
    struct QUIRKD* lquirkd[NBOARDS], * lquirkdl[NBOARDS];
    struct RESULT result[NBOARDS];

    //  initialise
    lquirkdl[0] = quirkdl;
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;
    results = (struct LIMIT*)malloc((nplays + 2) * sizeof(struct LIMIT));

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

        //  we reached a leaf
        else {

            //  store the result and set the best_{MAX|MP|N} flags
            ix++;
            tsum = 0.0;
            ih = lquirkd[l]->ih;
            results[ix].keep = NO;
            results[ix].result = result[ih].result;
            if (ewwinners) {
                for (i = minns; i <= maxns; i++) {
                    tsum += result[ih].tricks[i];
                    results[ix].keep |= tsum < best_result.tricks[i] + EPS && result[ih].result > best_result.res[i] - EPS;
                }
            }
            else {
                for (i = maxns; i >= minns; i--) {
                    tsum += result[ih].tricks[i];
                    results[ix].keep |= tsum > best_result.tricks[i] - EPS && result[ih].result > best_result.res[i] - EPS;
                }
            }

            //  get the next sibling
            while (l && lquirkd[l]->h == NIL)
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkd[l]->h ? lquirkd[l]->h->d : NIL;
        }
    }

    //  find the lowest result
    testlow = 100.0 * ntricks;
    for (ix = 1; ix <= nplays; ix++)
        testlow = MIN(testlow, results[ix].result);

    testhigh = best_result.result;
    while ((testhigh - testlow) > 0.01) {

        //  count the number of accepted plays
        count = 0;
        testval = 0.5 * (testhigh + testlow);
        for (ix = 1; ix <= nplays; ix++)
            if (results[ix].result > testval - EPS)
                count++;

        if (count < maxnodes)
            testhigh = testval;
        else
            testlow = testval;
    }

    //  delete all unaccepted plays from the list and clean up
    for (ix = 1; ix <= nplays; ix++)
        results[ix].keep |= testlow < results[ix].result;
    nplays = clean_quirk(0, results);

    if (matchpoint == MP) {
        printf("\t#nodes for MP plays : %d\n", nplays);
        printf("\tcutoff for MP plays : %6.4f\n", best_result.result - testlow); fflush(stdout);
    }
    if (matchpoint == IMP) {
        printf("\t#nodes for IMP plays: %d\n", nplays);
        printf("\tcutoff for IMP plays: %6.4f\n", best_result.result - testlow); fflush(stdout);
    }
}
