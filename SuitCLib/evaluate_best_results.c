//  name:
//      evaluate_best_results
//
//  purpose:
//      evaluate_best_results evaluates best_results using the QUIRKD tree
//
//  parameters:
//      none
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

int evaluate_best_results( void )
{
    int           i, ih, ix = 0, k, l = 0;
    double        tsum;
    struct QUIRKD *lquirkd[NBOARDS], *lquirkdl[NBOARDS];
    struct RESULT result[NBOARDS];

    //  initialise
    maxns = 0;
    minns = tricklimit;
    lquirkdl[0] = quirkd;
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;

    //  initialise the results
    best_result.result = 0.0;
    for ( i = 0; i <= ntricks; i++ ) {
        best_result.tricks[i] = defence ? 1.0 : 0.0;
        best_result.res[i] = 0.0;
    }

    while ( lquirkd[l] ) {

        //  compute the results
        for ( k = lquirkd[l]->il; k <= lquirkd[l]->ih; k++ ) {
            minns = MIN( minns, lquirkdl[l]->h->t[k].ns );
            maxns = MAX( maxns, lquirkdl[l]->h->t[k].ns );
            result[k].result = prob[k] * dupl[k] * lquirkdl[l]->h->t[k].ns + ( k ? result[k-1].result : 0.0 );
            for ( i = 0; i <= ntricks; i++ )
                result[k].tricks[i] = k ? result[k-1].tricks[i] : 0.0;
            result[k].tricks[lquirkdl[l]->h->t[k].ns] += prob[k] * dupl[k];
        }

        //  get the oldest son
        if ( lquirkd[l]->d ) {
            l++;
            lquirkdl[l] = lquirkdl[l-1];
            lquirkd[l] = lquirkd[l-1]->d;
        }

        //  we reached a leaf
        else {

            //  determine the best results
            tsum = 0.0;
            ih = lquirkd[l]->ih;
            if ( best_result.result < result[ih].result )
                best_result.result = result[ih].result;
            if ( defence ) {
                for ( i = 0; i <= ntricks; i++ ) {
                    tsum += result[ih].tricks[i];
                    if ( best_result.tricks[i] > tsum + EPS || best_result.tricks[i] > tsum - EPS && best_result.res[i] < result[ih].result + EPS ) {
                        best_result.tricks[i] = tsum;
                        best_result.res[i] = result[ih].result;
                    }
                }
            }
            else {
                for ( i = ntricks; i >= 0; i-- ) {
                    tsum += result[ih].tricks[i];
                    if ( best_result.tricks[i] < tsum - EPS || best_result.tricks[i] < tsum + EPS && best_result.res[i] < result[ih].result + EPS ) {
                        best_result.tricks[i] = tsum;
                        best_result.res[i] = result[ih].result;
                    }
                }
            }

            //  get the next sibling
            ix++;
            while ( l && lquirkd[l]->h == NIL )
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkd[l]->h ? lquirkd[l]->h->d : NIL;
        }
    }

    return( ix );
}
