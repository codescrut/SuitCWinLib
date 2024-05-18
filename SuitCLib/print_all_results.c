//  name:
//      print_all_results
//
//  purpose:
//      print_all_results prints the results of the plays
//
//  parameters:
//      none
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void print_all_results(char* output, char* optimumdetails)
{
    int           accept, best_MAX, best_MP, best_N[NCARDS], i, ih, ix = 0, k, l = 0;
    double        mpscore, tsum;
    struct INDEX  *tindex;
    struct NODE   *lnode[NPLIES+1];
    struct QUIRKD *lquirkd[NBOARDS], *lquirkdl[NBOARDS];
    struct RESULT result[NBOARDS];
    struct SCORE  *lscore;

    //  initialise
    lquirkdl[0] = quirkd;
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;

    output_add(output, "\"OptimumPlays\":[");
    bool firstOutput = true;

    //  loop over all nodes in the tree
    while( lquirkd[l] ) {

        //  collect the info on the number of tricks
        for ( k = lquirkd[l]->il; k <= lquirkd[l]->ih; k++ )
             nstricks[lquirkdl[l]->h->t[k].i] = lquirkdl[l]->h->t[k].ns;

        //  compute the results
        for ( k = lquirkd[l]->il; k <= lquirkd[l]->ih; k++ ) {
            result[k].result = result[k-1].result + prob[k] * dupl[k] * lquirkdl[l]->h->t[k].ns;
            for ( i = minns; i <= maxns; i++ )
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

            //  check for an optimal play
            ix++;
            ih = lquirkd[l]->ih;
            tsum = 0.0;
            best_MAX = result[ih].result > best_result.result - EPS;
            best_MP = matchpoint ? score[ix].best_MP : NO;
            accept = best_MAX | best_MP;
            if ( defence ) {
                for ( i = minns; i <= maxns; i++ ) {
                    tsum += result[ih].tricks[i];
                    best_N[i] = tsum < best_result.tricks[i] + EPS && result[ih].result > best_result.res[i] - EPS;
                    accept |= best_N[i];
                }
            }
            else {
                for ( i = maxns; i >= minns; i-- ) {
                    tsum += result[ih].tricks[i];
                    best_N[i] = tsum > best_result.tricks[i] - EPS && result[ih].result > best_result.res[i] - EPS;
                    accept |= best_N[i];
                }
            }

            //  print the results of this play
            if ( accept || printallresults ) {

                //  print the results
                if ( matchpoint && best_MAX && ! best_MP ) {
                    mpscore = 0.0;
                    lscore = score[0].r;
                    while ( lscore->r ) {
                        mpscore -= lscore->perc * lscore->mp[score[ix].ix];
                        lscore = lscore->r;
                    }
                }
                else
                    mpscore = 0.0;
                
                int outputBufferUsed = strlen(output);
                outputEnabled = outputBufferUsed <= JSON_BUFFER_SIZE - JSON_OUTPUTBUFFER_SAFETYMARGIN;

                int optimumdetailsBufferUsed = strlen(optimumdetails);
                int optimumdetailsMax = JSON_BUFFER_SIZE;
                if (noOfOptimumDetails > 0)
                {
                    optimumdetailsMax = JSON_BUFFER_SIZE - JSON_OPTIMUM_DETAILS_SAFETY_BLOCKS * (optimumdetailsBufferUsed / noOfOptimumDetails);
                }
                bool addJsonOptimumDetails = optimumdetailsBufferUsed <= optimumdetailsMax;

                noOfOptimumDetails++;

                print_results(output, optimumdetails, addJsonOptimumDetails, &firstOutput, best_MAX, best_MP, best_N, mpscore );


                //  create and print a node tree for this play
                if ( ! resultsonly ) {

                    //  create the tree
                    build_selected_tree();

                    //  create the tree
                    if ( ! compacttree && maxply )
                        select_ew_plays( 1 );

                    //  merge identical plays and select the preferred ones
                    if ( maxply && mergeselect & 0x1 )
                        merge_tree( 1, west );
                    if ( maxply && mergeselect & 0x2 )
                        merge_tree( 1, north );
                    if ( maxply && mergeselect & 0x4 )
                        select_ns_plays( 1 );

                    //  print the tree
                    struct TREE_ELEMENT* treeElements = malloc((long)MAX_TREE_ELEMENTS * sizeof(struct TREE_ELEMENT));
                    int treeElementsCount;
                    printf( "\n" );
                    if (addJsonOptimumDetails)
                    {
                        output_add(optimumdetails, "\n");
                    }
                    if ( tnode[0]->d )
                        print_tree(optimumdetails, addJsonOptimumDetails, 1, tnode[0]->d, YES, YES, treeElements, &treeElementsCount);
                    else
                        print_tree(optimumdetails, addJsonOptimumDetails, 0, tnode[0], YES, YES, treeElements, &treeElementsCount);

                    
                    char gameTreeJsonValue[JSON_BUFFER_SIZE];
                    create_game_tree_json_value(treeElements, treeElementsCount, gameTreeJsonValue);

                    output_add(output, ",\"GameTree\":");
                    if (gameTreeJsonValue[0] != '\0')
                    {
                        output_add(output, gameTreeJsonValue);
                    }
                    else
                    {
                        output_add(output, "\"\"");
                    }
                    
                    //  delete the tree again
                    delete_tree( 0, tnode );
                    free(treeElements);
                }
                output_add(output, "}");
                outputEnabled = true;
            }

            //  get the next sibling
            while ( l && lquirkd[l]->h == NIL )
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkd[l]->h ? lquirkd[l]->h->d : NIL;
        }
    }
    output_add(output, "],");

}


