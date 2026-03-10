//  name:
//      check_zermelo
//
//  purpose:
//      check_zermelo and evaluate_mp_results use Zermelo's algorithm to eliminate
//      dominated MP and IMP plays from the SCORE list. strongly dominated plays are
//      deleted from the SCORE list in check_zermelo, while weakly dominated
//      ones are deleted from the SCORE list in check_zermelo if zermelo==2 and
//      marked in check_zermelo otherwiise. marked weakly dominated plays are
//      deleted from the SCORE list in evaluate_mp_results if zermelo==1.
//
//  parameters:
//      lscore          in      the highest entry to be checked in the SCORE list
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

int check_zermelo(struct SCORE* lscore)
{
    int           bstrong, bweak, ix, goon = NO, lstrong, lweak;
    struct SCORE* ascore, * bscore;

    lscore->w = 0;
    bscore = lscore->l;
    while (bscore->l) {

        //  compare the scores
        bstrong = YES;
        lstrong = YES;
        bweak = YES;
        lweak = YES;
        ascore = score[0].r;
        while (ascore->r) {
            ix = ascore->ix;
            bstrong &= bscore->mp[ix] > (lscore->mp[ix] + EPS);
            lstrong &= lscore->mp[ix] > (bscore->mp[ix] + EPS);
            bweak &= bscore->mp[ix] > (lscore->mp[ix] - EPS);
            lweak &= lscore->mp[ix] > (bscore->mp[ix] - EPS);
            ascore = ascore->r;
        }

        //  both plays are equivalent; add the first play to the list of equivalent plays
        if (lweak && bweak) {
            if (trace & 0x2)
                printf("lbequal   l_ix=%04d   l_w=%d   b_ix=%04d   b_w=%d\n", lscore->ix, lscore->w, bscore->ix, bscore->w);
            goon = YES;
            free(lscore->mp);
            lscore->mp = NIL;
            ascore = bscore;
            while (ascore->b)
                ascore = ascore->b;
            ascore->b = lscore;
            lscore->r->l = lscore->l;
            lscore->l->r = lscore->r;
            break;
        }

        //  the second play is dominated, so delete it
        else if (lstrong || (lweak && zermelo == 2)) {
            if (trace & 0x2)
                printf("b_del     l_ix=%04d   l_w=%d   b_ix=%04d   b_w=%d\n", lscore->ix, lscore->w, bscore->ix, bscore->w);
            goon = YES;
            free(bscore->mp);
            bscore->mp = NIL;
            ascore = bscore;
            while (ascore) {
                ascore->best_MP = NO;
                ascore = ascore->b;
            }
            bscore->r->l = bscore->l;
            bscore->l->r = bscore->r;
        }

        //  the first play is dominated, so delete it
        else if (bstrong || (bweak && zermelo == 2)) {
            if (trace & 0x2)
                printf("l_del     l_ix=%04d   l_w=%d   b_ix=%04d   b_w=%d\n", lscore->ix, lscore->w, bscore->ix, bscore->w);
            goon = YES;
            free(lscore->mp);
            lscore->mp = NIL;
            ascore = lscore;
            while (ascore) {
                ascore->best_MP = NO;
                ascore = ascore->b;
            }
            lscore->r->l = lscore->l;
            lscore->l->r = lscore->r;
            break;
        }

        //  the second play is weakly dominated
        else if (lweak) {
            bscore->w = 1;
            if (trace & 0x2)
                printf("lweak     l_ix=%04d   l_w=%d   b_ix=%04d   b_w=%d\n", lscore->ix, lscore->w, bscore->ix, bscore->w);
        }

        //  the first play is weakly dominated
        else if (bweak) {
            lscore->w = 1;
            if (trace & 0x2)
                printf("bweak     l_ix=%04d   l_w=%d   b_ix=%04d   b_w=%d\n", lscore->ix, lscore->w, bscore->ix, bscore->w);
        }

        bscore = bscore->l;
    }

    return(goon);
}
