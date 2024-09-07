//  name:
//      limit_quirkd
//
//  purpose:
//      limit_quirkd limits the number of branches in the QUIRKD trees
//      that are kept during the merging process. QUIRKD branches are deleted
//      untill the product of the number of QUIRKD branches in quirkd[d] and
//      quirkd[d+1] is about the value of cutoff.
//
//  parameters:
//      d               in      the depth in the QUIRKD tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void limit_quirkd( int d )
{
    int           b, count[2], i, ih, ix, k, l, nplays[2];
    double        best_result[2], highresult, lowresult, testresult;
    struct LIMIT  *results[2];
    struct RESULT result[NBOARDS];
    struct QUIRKD *lquirkd[NBOARDS], *lquirkdl[NBOARDS];

    //  evaluate results for both nodes.
    highresult = 0.0;
    lowresult = 100.0 * ntricks;
    for ( i = 0; i < 2; i++ ) {

        //  initialise
        l = 0;
        ix = 0;
        best_result[i] = 0.0;
        lquirkdl[l] =  &quirkd[d+i];
        lquirkd[l] = lquirkdl[l]->h ? lquirkdl[l]->h->d : NIL;

        //  reserve memory for the results entries
        results[i] = (struct LIMIT *) malloc( ( NPLAYS + 2 ) * sizeof( struct LIMIT ) );

        //  loop over all nodes in the tree
        while( lquirkd[l] ) {

            //  compute the results
            for ( k = lquirkd[l]->il; k <= lquirkd[l]->ih; k++ ) {
                b = lquirkdl[l]->h->t[k].i;
                result[k].result = prob[b] * dupl[b] * lquirkdl[l]->h->t[k].ns + ( k ? result[k-1].result : 0.0 );
            }

            //  get the next distribution
            if ( lquirkd[l]->d ) {
                l++;
                lquirkdl[l] = lquirkdl[l-1];
                lquirkd[l] = lquirkd[l-1]->d;
            }

            //  we reached a terminal node; process the result
            else {

                //  determine the best result and store the result
                ix++;
                ih = lquirkd[l]->ih;
                results[i][ix].result = result[ih].result;
                best_result[i] = best_result[i] < result[ih].result ? result[ih].result : best_result[i];

                //  get the next distribution
                while ( l && ! lquirkd[l]->h )
                    l--;
                lquirkdl[l] = lquirkd[l];
                lquirkd[l] = lquirkdl[l]->h ? lquirkdl[l]->h->d : NIL;
            }
        }

        nplays[i] = ix;

        //  determine the limits of the search process
        for ( ix = 1; ix <= nplays[i]; ix++ ) {
            lowresult = MIN( lowresult, best_result[i] - results[i][ix].result );
            highresult = MAX( highresult, best_result[i] - results[i][ix].result );
        }
    }

    //  determine the cutoff level
    testresult = 0.5 * ( highresult + lowresult );
    while ( highresult - lowresult > 1.0e-4 ) {

        //  count the number of plays
        for ( i = 0; i < 2; i++ ) {
            count[i] = 0;
            for ( ix = 1; ix <= nplays[i]; ix++ )
                if ( best_result[i] - results[i][ix].result < testresult )
                    count[i]++;
        }

        //  determine the new limits for the search
        if ( (long) count[0] * (long) count[1] < cutoff )
            lowresult = testresult;
        else
            highresult = testresult;
        testresult = 0.5 * ( highresult + lowresult );
    }

    //  remove unaccepted plays from the list and clean up
    for ( i = 0; i < 2; i++ ) {
        for ( ix = 1; ix <= nplays[i]; ix++ )
            results[i][ix].keep = best_result[i] - results[i][ix].result < testresult;
        clean_quirkd( d+i, results[i] );
    }

    if ( ! SUITCAPP ) printf( "\tcutoff:%8d%6d ->%8d%6d\n", nplays[0], nplays[1], count[0], count[1] );
    fflush( stdout );
}
