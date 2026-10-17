//  name:
//      print_suitc
//
//  purpose:
//      print_suitc prints the information about the suit combination and restrictions
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void print_suitc(void)
{
    int          i, k, n, p, q;
    char         out[NCARDS + 1];
    static const char  seats[] = "WNES";
    static const char  seat[4][6] = { "West", "North", "East", "South" };
    static const char* symbols[] = { "-123456789TJQKA", "-123456789TBVHA", "-123456789tjqka", "-123456789tbvha" };

    //  print the 4 hands
    for (q = 0; q < 4; q++) {
        p = (q == 0 ? north : (q == 1 ? south : (q == 2 ? west : east)));
        k = 0;
        out[0] = '-';
        out[1] = '\0';
        for (i = NCARDS + 1; i >= 0; i--) {
            if (addcard(i) & brd[0][p]) {
                out[k] = symbols[printoption & 0x1][i];
                out[++k] = '\0';
            }
        }
        printf("\t%-5s: %s\n", seat[p], out);
    }
    printf("\n");

    //  print additional information
    if (cutoff && !SUITCAPP)
        printf("\tcutoff              : %ld\n", cutoff);
    if (tricklimit < ntricks)
        printf("\tterminate play after: %d tricks\n", tricklimit);
    if (leader != noplayer) {
        printf("\tthe lead is in      : %s\n", seat[leader]);
        if (opening_lead != 0x7fff)
            for (i = 0; i <= NCARDS + 1; i++)
                if (opening_lead & addcard(i))
                    printf("\tthe lead is the     : %c\n", symbols[printoption & 0x1][i]);
    }
    if (nrules) {
        for (n = 0; n < nrules; n++) {
            if (n == 0) {
                if (defence)
                    printf("\tsecond hand strategy: ");
                else
                    printf("\tthird hand strategy : ");
            }
            else
                printf("\t                      ");
            for (i = 14; i >= 2; i--)
                if (h2rule[n] & addcard(i))
                    printf("%c", symbols[printoption & 0x1][i]);
            printf(" -> ");
            for (i = 14; i >= 2; i--)
                if (h3rule[n] & addcard(i))
                    printf("%c", symbols[printoption & 0x1][i]);
            printf("\n");
        }
    }
    if (throwin) {
        printf("\tthrowin is enabled  : ");
        for (p = 0; p < 4; p++)
            if (throwin & (1 << p))
                printf("%c", seats[p]);
        printf("\n");
    }
    for (p = 0; p < 4; p++)
        if (nfree[0][p] < ncards[p])
            printf("\t#free entries %-5s : %d\n", seat[p], nfree[0][p]);
    for (p = 0; p < 4; p++)
        if (nruff[0][p])
            printf("\tnumb. of ruffs %-5s: %d\n", seat[p], nruff[0][p]);
    if (matchpoint == IMP) {
        for (n = 0; n <= ntricks; n++)
            if (pointscore[n])
                printf("\t#score for %d tricks : %d\n", n, pointscore[n]);
    }
    if (eastfixed) {
        k = 0;
        out[0] = '-';
        out[1] = '\0';
        for (i = NCARDS + 1; i >= 0; i--) {
            if (addcard(i) & eastfixed) {
                out[k] = symbols[printoption & 0x1][i];
                out[++k] = '\0';
            }
        }
        printf("\tfixed east cards    : %s\n", out);
    }
    if (westfixed) {
        k = 0;
        out[0] = '-';
        out[1] = '\0';
        for (i = NCARDS + 1; i >= 0; i--) {
            if (addcard(i) & westfixed) {
                out[k] = symbols[printoption & 0x1][i];
                out[++k] = '\0';
            }
        }
        printf("\tfixed west cards    : %s\n", out);
    }
    if (ncardsmin[west] != 0)
        printf("\tminimum length west : %d\n", ncardsmin[west]);
    if (westvac != NCARDS)
        printf("\t#vacant places west : %d\n", westvac);
    if (ncardsmin[east] != 0)
        printf("\tminimum length east : %d\n", ncardsmin[east]);
    if (eastvac != NCARDS)
        printf("\t#vacant places east : %d\n", eastvac);

    fflush(stdout);
}
