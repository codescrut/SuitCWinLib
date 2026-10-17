//  name:
//      evaluate_mp_results
//
//  purpose:
//      evaluate_mp_results calculates the percentage contributions to the MP or IMP plays
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

int evaluate_mp_results(void)
{
    int           best_MP, goon = YES, i, ix, jpiv[2 * NNODE], list, n, nnode;
    double        p[NNODE];
    struct SCORE* bscore, * lscore;

    //  compare the scores, unlink dominated plays and combine equvalent plays
    while (goon) {
        goon = NO;
        lscore = score[0].r;
        if (trace & 0x2)
            printf("\n");
        while (lscore->r) {
            goon |= check_zermelo(lscore);
            lscore = lscore->r;
        }
    }

    //  delete the weakly dominated plays
    if (zermelo == 1) {
        lscore = score[0].r;
        while (lscore->r) {
            if (lscore->w & 0x1) {
                free(lscore->mp);
                lscore->mp = NIL;
                bscore = lscore;
                while (bscore) {
                    bscore->best_MP = NO;
                    bscore = bscore->b;
                }
                lscore->r->l = lscore->l;
                lscore->l->r = lscore->r;
            }
            lscore = lscore->r;
        }
    }

    //  count the number of MP or IMP plays
    nnode = 0;
    lscore = score[0].r;
    while (lscore->r) {
        if (lscore->best_MP)
            nnode++;
        lscore = lscore->r;
    }

    //  check the number of plays
    if (nnode > NNODE) {
        printf("\nERROR: Increase NNODE to at least %d\n", nnode);
        return -1;
    }

    //  there is only one MP or IMP play, so there is no need to call minimax
    if (nnode == 1) {
        list = 0;
        lscore = score[0].r;
        while (lscore) {
            list++;
            lscore->perc = 1.0;
            lscore = lscore->b;
        }
    }

    //  we have potentially a mixed MP or IMP strategy, so the minimax routine is used
    else if (nnode > 1) {

        //  collect the MP or IMP scores
        nnode = 0;
        lscore = score[0].r;
        while (lscore->r) {
            if (lscore->best_MP) {
                n = 0;
                bscore = score[0].r;
                while (bscore->r) {
                    a[nnode][n++] = 100.0 * lscore->mp[bscore->ix];
                    bscore = bscore->r;
                }
                nnode++;
            }
            lscore = lscore->r;
        }

        //  compute the MP or IMP results with the minimax solver
        minimax(nnode, jpiv, p);

        //  store the percentages
        n = 0;
        lscore = score[0].r;
        while (lscore->r) {
            lscore->perc = (jpiv[n] != -1) && (p[jpiv[n]] > EPS) ? p[jpiv[n]] : 0.0;
            bscore = lscore;
            while (bscore) {
                bscore->perc = lscore->perc;
                bscore = bscore->b;
            }
            n++;
            lscore = lscore->r;
        }

        //  store the MP or IMP results
        n = 0;
        list = 0;
        nnode = 0;
        lscore = score[0].r;
        while (lscore->r) {

            //  store the result
            if (lscore->perc != 0.0) {
                i = 0;
                nnode++;
                bscore = lscore;
                while (bscore) {
                    i++;
                    bscore = bscore->b;
                }
                list = MAX(list, i);
            }

            //  delete the entry from the list
            else {
                free(lscore->mp);
                lscore->mp = NIL;
                bscore = lscore;
                while (bscore) {
                    bscore->best_MP = NO;
                    bscore = bscore->b;
                }
                lscore->l->r = lscore->r;
                lscore->r->l = lscore->l;
            }
            n++;
            lscore = lscore->r;
        }
    }

    //  calculate the MP or IMP scores
    for (i = 1; i <= nplays; i++) {
        lscore = score[0].r;
        while (lscore->r) {
            score[i].mpscore -= lscore->perc * lscore->mp[score[i].ix];
            lscore = lscore->r;
        }
    }

    //  clear the matchpoint flag if no MAX play scores worse than a MP or IMP play
    if (nnode == 1) {

        //  clear the matchpoint flag if the list contains a MAX play
        best_MP = NO;
        for (i = 1; i <= nplays; i++)
            best_MP |= score[i].best_MAX != score[i].best_MP;

        //  clear the best_MP flag
        if (!best_MP) {
            for (i = 1; i <= nplays; i++)
                score[i].best_MP = NO;
            matchpoint = best_MP || printallmatchpoint ? matchpoint : NO;
        }
    }

    //  print the MP or IMP matrix for a mixed MP or IMP strategy
    if (matchpoint && (nnode > 1 || list > 1)) {

        //  renumber the nodes
        if (!printallresults) {
            ix = 0;
            for (i = 1; i <= nplays; i++)
                if (score[i].best_MAX || score[i].best_MP || score[i].best_N)
                    score[i].ix = ix++;
        }

        //  print lists of equivalent MP or IMP plays
        lscore = score[0].r;
        if (matchpoint == MP)
            printf("\n\n\nThe matchpoint strategy\n=======================\n\n");
        else
            printf("\n\n\nThe IMP strategy\n================\n\n");
        while (lscore->r) {
            printf("\tperc=%7.4f    list:", 100.0 * lscore->perc);
            bscore = lscore;
            while (bscore) {
                if (ix + 1 < 10)
                    printf(" %01d", bscore->ix + 1);
                else if (ix + 1 < 100)
                    printf(" %02d", bscore->ix + 1);
                else if (ix + 1 < 1000)
                    printf(" %03d", bscore->ix + 1);
                else if (ix + 1 < 10000)
                    printf(" %04d", bscore->ix + 1);
                else
                    printf(" %05d", bscore->ix + 1);
                bscore = bscore->b;
            }
            printf("\n");
            lscore = lscore->r;
        }

        //  restore the node numbers
        if (!printallresults)
            for (ix = 1; ix <= nplays; ix++)
                if (score[ix].best_MAX || score[ix].best_MP || score[ix].best_N)
                    score[ix].ix = ix - 1;
    }
    return 0;
}
