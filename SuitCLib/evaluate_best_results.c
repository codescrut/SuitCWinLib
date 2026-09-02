//  name:
//      evaluate_best_results
//
//  purpose:
//      evaluate_best_results evaluates best_results using the QUIRK tree
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

int evaluate_best_results(void)
{
    int           i, ih, ix = 0, k, l = 0, match, nreport = 0;
    COLOR         optcode;
    double        tricksum;
    struct INDEX* lindex;
    struct QUIRKD* lquirkd[NBOARDS], * lquirkdl[NBOARDS];
    struct RESULT result[NBOARDS];

    //  initialise
    maxns = 0;
    minns = tricklimit;
    lquirkdl[0] = quirkdl;
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;
    for (k = 0; k < boardcount; k++) {
        nslimits[0][k] = ntricks;
        nslimits[1][k] = 0;
    }

    //  initialise the results
    nbest_plays = 0;
    best_result.result = 0.0;
    for (i = 0; i <= ntricks; i++) {
        best_result.tricks[i] = ewwinners ? 1.0 : 0.0;
        best_result.res[i] = 0.0;
    }

    while (lquirkd[l]) {

        //  il/ih are 8 bit fields, so a stale range can reach past boardcount
        //  and index t[], result[] and nslimits[] out of bounds. Check it once
        //  per node, before the entries themselves are read.
        if (lquirkd[l]->il <= lquirkd[l]->ih && lquirkd[l]->ih >= boardcount) {
            resultrange = YES;
            return 0;
        }

        //  compute the results
        for (k = lquirkd[l]->il; k <= lquirkd[l]->ih; k++) {

            //  t[] is filled per board by convert_tree_to_quirk, and a QUIRKD
            //  whose [il,ih] range reaches an entry its header never filled
            //  reads the 0xff fill instead. ns is an 8 bit field, so such a
            //  value indexes tricks[]/perc[]/best_N[] -- all NCARDS+1 long,
            //  two of them on the stack -- hundreds of entries past the end,
            //  destroying the /GS cookie and killing the host process with an
            //  uncatchable STATUS_STACK_BUFFER_OVERRUN. Refuse the call.
            if (lquirkdl[l]->h->t[k].ns > ntricks) {
                resultrange = YES;
                return 0;
            }

            minns = MIN(minns, lquirkdl[l]->h->t[k].ns);
            maxns = MAX(maxns, lquirkdl[l]->h->t[k].ns);
            nslimits[0][k] = MIN(nslimits[0][k], lquirkdl[l]->h->t[k].ns);
            nslimits[1][k] = MAX(nslimits[1][k], lquirkdl[l]->h->t[k].ns);
            result[k].result = prob[k] * dupl[k] * lquirkdl[l]->h->t[k].ns + (k ? result[k - 1].result : 0.0);
            for (i = 0; i <= ntricks; i++)
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

            //  determine the best results
            optcode = 0x0;
            ih = lquirkd[l]->ih;
            if (best_result.result < result[ih].result - EPS) {
                optcode |= 0x8000;
                best_result.result = result[ih].result;
                for (k = 0; k <= nbest_plays; k++)
                    best_play[k].optcode ^= best_play[k].optcode & 0x8000;
            }
            else if (best_result.result < result[ih].result + EPS)
                optcode |= 0x8000;

            tricksum = 0.0;
            if (ewwinners) {
                for (i = 0; i <= ntricks; i++) {
                    tricksum += result[ih].tricks[i];
                    if (best_result.tricks[i] > tricksum + EPS || best_result.tricks[i] > tricksum - EPS && best_result.res[i] < result[ih].result + EPS) {
                        best_result.tricks[i] = tricksum;
                        best_result.res[i] = result[ih].result;
                    }
                }
            }

            else {
                for (i = ntricks; i >= 0; i--) {
                    tricksum += result[ih].tricks[i];
                    if (best_result.tricks[i] < tricksum - EPS || best_result.tricks[i] < tricksum + EPS && best_result.res[i] < result[ih].result - EPS) {
                        optcode |= 1 << i;
                        best_result.tricks[i] = tricksum;
                        best_result.res[i] = result[ih].result;
                        for (k = 0; k <= nbest_plays; k++)
                            best_play[k].optcode ^= best_play[k].optcode & (1 << i);
                    }
                    else if (best_result.tricks[i] < tricksum + EPS && best_result.res[i] < result[ih].result + EPS)
                        optcode |= 1 << i;
                }
            }

            if (optcode) {

                //  locate an existing optimal play
                for (k = 0; k <= nbest_plays; k++) {
                    if (best_play[k].optcode) {
                        match = YES;
                        for (i = 0; i <= ntricks; i++)
                            if (best_play[k].tricks[i] < result[ih].tricks[i] - EPS || best_play[k].tricks[i] > result[ih].tricks[i] + EPS)
                                match = NO;
                        if (match) {
                            best_play[k].dupl++;
                            goto goon;
                        }
                    }
                }

                //  insert this optimal play in the list
                for (k = 0; k <= nbest_plays; k++)
                    if (!best_play[k].optcode)
                        break;
                best_play[k].optcode = optcode;
                best_play[k].dupl = 1;
                best_play[k].result = result[ih].result;
                for (i = 0; i <= ntricks; i++)
                    best_play[k].tricks[i] = result[ih].tricks[i];
                if (k > nbest_plays)
                    nbest_plays++;
            }

            //  get the next sibling
        goon:
            ix++;
            while (l && lquirkd[l]->h == NIL)
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkd[l]->h ? lquirkd[l]->h->d : NIL;
        }
    }

    //  remove empty slots
    k = -1;
    for (l = 0; l <= nbest_plays; l++) {
        if (best_play[l].optcode) {
            k++;
            best_play[k].optcode = best_play[l].optcode;
            best_play[k].dupl = best_play[l].dupl;
            best_play[k].result = best_play[l].result;
            for (i = 0; i <= ntricks; i++)
                best_play[k].tricks[i] = best_play[l].tricks[i];
        }
    }
    nbest_plays = k + 1;

    //  determine the number of plays that will be printed
    if (SUITCAPP) {
        for (k = 0; k <= nbest_plays; k++)
            nreport += best_play[k].dupl;
        if (nreport > 10)
            for (k = 0; k <= nbest_plays; k++)
                best_play[k].dupl = 1 + (10 - nbest_plays) * (best_play[k].dupl - 1) / (nreport - nbest_plays);
    }

    return(ix);
}
