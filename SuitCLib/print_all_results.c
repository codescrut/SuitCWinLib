//  name:
//      print_all_results
//
//  purpose:
//      print_all_results prints the results of the plays
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void print_all_results(char* output, char* optimumdetails)
{
    //  best_N is indexed by trick count, which runs 0..ntricks inclusive, so
    //  it needs NCARDS+1 entries -- as res[]/tricks[]/perc[] all have
    int           accept, best_MAX, best_MP, best_N[NCARDS + 1], i, ih, ix = 0, k, l = 0;
    double        mpscore, tricksum;
    struct QUIRKD* lquirkd[NBOARDS], * lquirkdl[NBOARDS];
    struct RESULT result[NBOARDS];
    struct SCORE* lscore;

    output_add(output, "\n\"OptimumPlays\":[");
    bool firstOutput = true;

    //  initialize
    lquirkdl[0] = quirkdl;
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;

    //  loop over all nodes in the tree
    while (lquirkd[l]) {

        //  collect the info on the number of tricks
        for (k = lquirkd[l]->il; k <= lquirkd[l]->ih; k++)
            nstricks[lquirkdl[l]->h->t[k].i] = lquirkdl[l]->h->t[k].ns;

        //  compute the results
        for (k = lquirkd[l]->il; k <= lquirkd[l]->ih; k++) {
            if (k) {
                result[k].result = result[k - 1].result + prob[k] * dupl[k] * lquirkdl[l]->h->t[k].ns;
                for (i = minns; i <= maxns; i++)
                    result[k].tricks[i] = result[k - 1].tricks[i];
                result[k].tricks[lquirkdl[l]->h->t[k].ns] += prob[k] * dupl[k];
            }
            else {
                result[k].result = prob[k] * dupl[k] * lquirkdl[l]->h->t[k].ns;
                for (i = minns; i <= maxns; i++)
                    result[k].tricks[i] = 0.0;
                result[k].tricks[lquirkdl[l]->h->t[k].ns] = prob[k] * dupl[k];
            }
        }

        //  get the oldest son
        if (lquirkd[l]->d) {
            l++;
            lquirkdl[l] = lquirkdl[l - 1];
            lquirkd[l] = lquirkd[l - 1]->d;
        }

        //  we reached a leaf
        else {

            //  check for an optimal play
            ix++;
            ih = lquirkd[l]->ih;
            tricksum = 0.0;
            best_MAX = result[ih].result > best_result.result - EPS;
            best_MP = matchpoint ? score[ix].best_MP : NO;
            accept = best_MAX | best_MP;
            if (ewwinners) {
                for (i = minns; i <= maxns; i++) {
                    tricksum += result[ih].tricks[i];
                    best_N[i] = tricksum < best_result.tricks[i] + EPS && result[ih].result > best_result.res[i] - EPS;
                    accept |= best_N[i];
                }
            }
            else {
                for (i = maxns; i >= minns; i--) {
                    tricksum += result[ih].tricks[i];
                    best_N[i] = tricksum > best_result.tricks[i] - EPS && result[ih].result > best_result.res[i] - EPS;
                    accept |= best_N[i];
                }
            }

            //  restrict the number of plays that is reported
            if (accept && SUITCAPP) {
                for (k = 0; k < nbest_plays; k++) {
                    int match = YES;
                    for (i = minns; i <= maxns; i++)
                        if (best_play[k].tricks[i] < result[ih].tricks[i] - EPS || best_play[k].tricks[i] > result[ih].tricks[i] + EPS)
                            match = NO;
                    if (match) {
                        if (best_play[k].dupl)
                            best_play[k].dupl--;
                        else
                            accept = NO;
                        break;
                    }
                }
            }

            //  print the results of this play
            if (accept || printallresults) {

                //  print the results
                if (matchpoint) {
                    mpscore = 0.0;
                    lscore = score[0].r;
                    while (lscore->r) {
                        mpscore -= lscore->perc * lscore->mp[score[ix].ix];
                        lscore = lscore->r;
                    }
                }
                else
                    mpscore = 0.0;
                if (matchpoint == MP)
                    mpscore = 100. * mpscore;
                //print_results(best_MAX, best_MP, best_N, mpscore);
                size_t outputBufferUsed = strlen(output);
                outputEnabled = outputBufferUsed <= JSON_BUFFER_SIZE - JSON_OUTPUTBUFFER_SAFETYMARGIN;

                size_t optimumdetailsBufferUsed = strlen(optimumdetails);
                size_t optimumdetailsMax = JSON_BUFFER_SIZE;
                if (noOfOptimumDetails > 0)
                {
                    optimumdetailsMax = JSON_BUFFER_SIZE - JSON_OPTIMUM_DETAILS_SAFETY_BLOCKS * (optimumdetailsBufferUsed / noOfOptimumDetails);
                }
                bool addJsonOptimumDetails = optimumdetailsBufferUsed <= optimumdetailsMax;

                noOfOptimumDetails++;

                //printf("print_results\n");
                print_results(output, optimumdetails, addJsonOptimumDetails, &firstOutput, best_MAX, best_MP, best_N, mpscore);

                //  create and print a node tree for this play
                if (!resultsonly) {

                    //  create the tree
                    // printf("build_selected_tree\n");
                    build_selected_tree();
                    if (usesuitentries)
                        use_suit_entries();

                    //  fold identical plays and select the preferred ones
                    if (maxprint && foldselect & 0x8) {
                        select_ns_plays(1);
                        fold_tree(1, west);
                    }
                    if (maxprint && foldselect & 0x1)
                        fold_tree(1, west);
                    if (maxprint && foldselect & 0x2)
                        fold_tree(1, north);
                    if (maxprint && foldselect & 0x4)
                        select_ns_plays(1);

                    //  print the tree
                    struct TREE_ELEMENT* treeElements = (struct TREE_ELEMENT *) malloc((long)MAX_TREE_ELEMENTS * sizeof(struct TREE_ELEMENT));
                    int treeElementsCount;

                    if (addJsonOptimumDetails)
                    {
                        output_add(optimumdetails, "\n");
                    }
                    // printf("print_tree\n");
                    print_tree(optimumdetails, addJsonOptimumDetails, 1, maxprint, tnode[0]->d, YES, YES, treeElements, &treeElementsCount);
                    // printf("print_tree done\n");

                    //for (int i = 0; i < treeElementsCount; i++) {
                    //    if (treeElements[i].type == 0)
                    //        printf("\n");
                    //    printf("%c", treeElements[i].content);
                    //}
                    //printf("treeElementsCount %d\n", treeElementsCount);

                    static char gameTreeJsonValue[JSON_BUFFER_SIZE];  // static: too big for small host stacks
                    // printf("create_game_tree_json_value\n");
                    create_game_tree_json_value(treeElements, treeElementsCount, gameTreeJsonValue);
                    // printf("create_game_tree_json_value done\n");

                    output_add(output, ",\n\"GameTree\":");
                    if (gameTreeJsonValue[0] != '\0')
                    {
                        output_add(output, gameTreeJsonValue);
                    }
                    else
                    {
                        output_add(output, "\"\"");
                    }

                    //  delete the tree again
                    delete_tree(0, tnode);
                    free(treeElements);
                }
                outputEnabled = true;
            }

            //  get the next sibling
            while (l && lquirkd[l]->h == NIL)
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkd[l]->h ? lquirkd[l]->h->d : NIL;
        }
    }
    output_add(output, "\n");
    // printf("Done print_all_results\n");
}
