//  name:
//      print_summary_results
//
//  purpose:
//      print_summary_results prints a summary of the results using the QUIRK tree
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void print_summary_results(void)
{
    int           i, j, k, l = 0, n, o = 0;
    char          out[2 * NBOARDS + 1];
    static char   hexdigit[] = "0123456789abcdef";
    struct QUIRKD* lquirkd[NBOARDS], * lquirkdl[NBOARDS];
    struct RESULT result[NBOARDS];

    //  print the header for the list of all results
    out[0] = '\0';
    printf("\n\n\nThe complete list of play theoretic acceptable results\n");
    printf("======================================================\n\n");
    for (i = z_count - 1; i >= 0; i--) {
        for (j = 0; j < boardcount; j++)
            printf(" %c", b_print[i][j]);
        printf("\n");
    }
    for (i = 0; i < boardcount; i++)
        printf("--");
    if (ewwinners)
        for (i = maxns; i >= minns; i--)
            printf("  -- %x --", tricklimit - i);
    else
        for (i = minns; i <= maxns; i++)
            printf("  -- %x --", i);
    printf("  + R ++\n");

    //  loop over all quirks in the tree
    lquirkdl[0] = quirkdl;
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;
    while (lquirkd[l]) {

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

            //  build the output string
            for (i = o; i < k; i++) {
                out[o++] = ' ';
                out[o++] = '-';
            }
            out[o++] = ' ';
            out[o++] = ewwinners ? hexdigit[tricklimit - lquirkdl[l]->h->t[k].ns] : hexdigit[lquirkdl[l]->h->t[k].ns];
        }

        //  get the oldest son
        if (lquirkd[l]->d) {
            l++;
            lquirkdl[l] = lquirkdl[l - 1];
            lquirkd[l] = lquirkd[l - 1]->d;
        }

        //  we reached a leaf; print the results and get the next distribution
        else {

            out[o] = '\0';
            printf("%s", out);
            if (BLANK)
                for (n = 0; n < o; n++)
                    out[n] = ' ';
            if (ewwinners) {
                for (i = maxns; i >= minns; i--)
                    printf("%9.4f", 100.0 * result[lquirkd[l]->ih].tricks[i]);
                printf("%8.4f\n", tricklimit - result[lquirkd[l]->ih].result);
            }
            else {
                for (i = minns; i <= maxns; i++)
                    printf("%9.4f", 100.0 * result[lquirkd[l]->ih].tricks[i]);
                printf("%8.4f\n", result[lquirkd[l]->ih].result);
            }

            //  get the next sibling
            while (l && lquirkd[l]->h == NIL)
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkd[l]->h ? lquirkd[l]->h->d : NIL;

            o = lquirkd[l] ? 2 * lquirkd[l]->il : 0;
            out[o] = '\0';
        }
    }
}
