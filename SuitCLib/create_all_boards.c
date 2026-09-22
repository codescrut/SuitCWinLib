//  name:
//      create_all_boards
//
//  purpose:
//      create_all_boards create the complete boardset
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

double fac(int n, int r)
{
    double        f = 1.0;
    int           i;
    if (n < 0 || r < 0 || n < r)
        return(0.0);
    for (i = 0; i < r; i++)
        f = (f * (n - i)) / (i + 1);
    return (f);
}

int create_all_boards(void)
{
    int          activ[8192], cnt, cnt0, cnt1, ec, i, ic, ii, inx[NBOARDS], j, k, n, offs, q, wc;
    COLOR        c, ci, cj, dboard[4][8192];
    double       scale, ddupl[NBOARDS], dprob[NBOARDS];
    struct SCARD* sc;

    //  count the cards
    ic = 0;
    wc = westvac;
    ec = eastvac;
    c = brd[0][west] | brd[0][east];
    while (c) {
        ic++;
        c ^= low_c(c);
    }

    //  create the first board
    cnt0 = 0;
    cnt1 = 0;
    cnt = 0;
    board[north][cnt0 + cnt] = brd[0][north];
    board[south][cnt0 + cnt] = brd[0][south];
    board[east][cnt0 + cnt] = RENONCE;
    board[west][cnt0 + cnt] = brd[0][west] | brd[0][east];
    dupl[cnt0 + cnt] = 1;
    prob[cnt0 + cnt] = fac(wc, ic) * fac(ec, 0) / (fac(wc + ec, ic) * fac(ic, 0));
    cnt++;
    cnt1 += cnt;

    //  create all other boards
    ii = 0;
    while (cnt) {
        ii++;
        cnt = 0;
        c = brd[0][west] | brd[0][east];
        while (c) {
            for (i = cnt0; i < cnt1; i++) {
                if (low_c(c) > board[east][i]) {
                    board[north][cnt1 + cnt] = board[north][i];
                    board[south][cnt1 + cnt] = board[south][i];
                    board[east][cnt1 + cnt] = board[east][i] | low_c(c);
                    board[west][cnt1 + cnt] = (brd[0][west] | brd[0][east]) ^ ((brd[0][west] | brd[0][east]) & board[east][cnt1 + cnt]);
                    dupl[cnt1 + cnt] = 1;
                    prob[cnt1 + cnt] = fac(wc, ic - ii) * fac(ec, ii) / (fac(wc + ec, ic) * fac(ic, ii));
                    cnt++;
                }
                else
                    break;
            }
            c ^= low_c(c);
        }
        cnt0 = cnt1;
        cnt1 += cnt;
    }

    //  combine boards that differ only by cards of the same rank
    cnt = 0;
    cnt0 = 0;
    for (k = 0; k <= ic; k++) {

        offs = fac(ic, k);
        for (j = cnt0; j < cnt0 + offs; j++)
            activ[j] = YES;

        sc = &scards[0][east];
        for (n = 1; n <= sc->n; n++) {
            for (j = cnt0 + 1; j < cnt0 + offs; j++) {
                if (activ[j]) {
                    for (i = cnt0; i < j; i++) {
                        ci = (sc->c[n] & board[east][i]) ^ board[east][i];
                        cj = (sc->c[n] & board[east][j]) ^ board[east][j];
                        if (activ[i] && ci == cj) {
                            dupl[i] += dupl[j];
                            activ[j] = NO;
                            break;
                        }
                    }
                }
            }
        }

        for (j = cnt0; j < cnt0 + offs; j++) {
            if (activ[j]) {
                board[north][cnt] = board[north][j];
                board[south][cnt] = board[south][j];
                board[east][cnt] = board[east][j];
                board[west][cnt] = board[west][j];
                dupl[cnt] = dupl[j];
                prob[cnt] = prob[j];
                cnt++;
            }
        }
        cnt0 += offs;
    }
    boardcount = cnt;

    //  discards boards that have too few or too many cards
    cnt = 0;
    for (j = 0; j < boardcount; j++) {

        //  count the cards
        wc = 0;
        c = board[west][j];
        while (c) {
            wc++;
            c ^= low_c(c);
        }
        ec = 0;
        c = board[east][j];
        while (c) {
            ec++;
            c ^= low_c(c);
        }

        //  keep only boards within bounds
        if (ncardsmin[west] <= wc && ncardsmin[east] <= ec &&
            westfixed == (board[west][j] & westfixed) &&
            eastfixed == (board[east][j] & eastfixed) &&
            prob[j] > EPS) {
            board[north][cnt] = board[north][j];
            board[south][cnt] = board[south][j];
            board[east][cnt] = board[east][j];
            board[west][cnt] = board[west][j];
            dupl[cnt] = dupl[j];
            prob[cnt] = prob[j];
            cnt++;
        }
    }
    boardcount = cnt;

    //  keep only the boards that are consistent with the opening lead
    if (leader != noplayer) {
        cnt = 0;
        for (i = 0; i < boardcount; i++) {
            if (board[leader][i] & opening_lead) {
                board[north][cnt] = board[north][i];
                board[south][cnt] = board[south][i];
                board[east][cnt] = board[east][i];
                board[west][cnt] = board[west][i];
                dupl[cnt] = dupl[i];
                prob[cnt] = prob[i];
                cnt++;
            }
        }
        boardcount = cnt;
    }

    //  renumber the boards
    if (reorder) {
        for (i = 0; i < boardcount; i++)
            inx[i] = i;
        for (i = 0; i < boardcount; i++) {
            for (j = i + 1; j < boardcount; j++) {
                if (board[west][inx[i]] < board[west][inx[j]]) {
                    k = inx[i];
                    inx[i] = inx[j];
                    inx[j] = k;
                }
            }
        }
        for (i = 0; i < boardcount; i++) {
            for (q = 0; q < 4; q++)
                dboard[q][i] = board[q][inx[i]];
            ddupl[i] = dupl[inx[i]];
            dprob[i] = prob[inx[i]];
        }
        for (i = 0; i < boardcount; i++) {
            for (q = 0; q < 4; q++)
                board[q][i] = dboard[q][i];
            dupl[i] = ddupl[i];
            prob[i] = dprob[i];
        }
    }

    //  if no boards are left
    if (boardcount == 0) {
        return -1;
    }

    //  collect the cards in each hand and scale the percentages
    scale = 0.0;
    for (q = 0; q < 4; q++)
        brd[0][q] = RENONCE;
    for (cnt = 0; cnt < boardcount; cnt++)
        scale += prob[cnt] * dupl[cnt];
    for (cnt = 0; cnt < boardcount; cnt++) {
        for (q = 0; q < 4; q++)
            brd[0][q] |= board[q][cnt];
        prob[cnt] /= scale;
    }

    //  check for the lead of an illegal card
    if (leader != noplayer) {
        if (!(brd[0][leader] & opening_lead)) {
            printf("The opening lead is not found in the hand of the leader\n");
            return -1;
        }
    }

    return 0;
}
