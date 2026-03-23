//  name:
//      apply_played_tricks
//
//  purpose:
//      Filters board distributions based on played tricks and applies
//      restricted choice (Bayesian) probability adjustments.
//      After processing, updates the board state, leader, and
//      recomputes derived structures for continued analysis.
//
//  Copyright 2025, Gerrit A. van der Velde, Leiden
//  Restricted choice implementation by Lars Kappel Hansen

#include "header.h"


static int count_cards(COLOR c)
{
    int n = 0;
    while (c) {
        n++;
        c ^= low_c(c);
    }
    return n;
}


//  Find the SCARD group mask that contains the played card for player p.
//  Boards use canonical SCARD representatives, so we must match by group
//  rather than by exact card.
static COLOR find_scard_group(int p, COLOR card)
{
    struct SCARD* sc = &scards[0][p];
    int n;

    for (n = 1; n <= sc->n; n++)
        if (sc->c[n] & card)
            return sc->c[n];

    return RENONCE;
}


int apply_played_tricks(char* output)
{
    int    t, ply, p, i, pp;
    int    trick_leader;
    int    ply_player[4];
    COLOR  card, group;
    double scale;
    int    new_boardcount;

    if (nplayedtricks == 0)
        return 0;

    //  a leader must be specified (either via -L flag or first trick's seat prefix)
    if (leader == noplayer) {
        if (played_leader[0] != noplayer)
            leader = played_leader[0];
        else {
            printf("A leader must be specified with -L when using played tricks; the program stops\n");
            output_error(output, "A leader must be specified with -L when using played tricks");
            return -1;
        }
    }

    trick_leader = leader;

    for (t = 0; t < nplayedtricks; t++) {

        //  Step 1: map ply positions to players
        //  a per-trick leader override (n/e/s/w prefix) takes precedence
        if (played_leader[t] != noplayer)
            trick_leader = played_leader[t];
        ply_player[0] = trick_leader;
        ply_player[1] = (trick_leader + 1) % 4;
        ply_player[2] = (trick_leader + 2) % 4;
        ply_player[3] = (trick_leader + 3) % 4;

        //  Step 2: validate declarer (N/S) cards
        for (ply = 0; ply < played_length[t]; ply++) {
            p = ply_player[ply];
            card = played_card[t][ply];

            if (NS(p) && card != DISCARD) {
                if (!(brd[0][p] & card)) {
                    printf("Played card not found in %s's hand (trick %d); the program stops\n",
                           p == north ? "North" : "South", t + 1);
                    output_error(output, "Played card not found in declarer hand");
                    return -1;
                }
            }
        }

        //  Step 3: filter boards based on consistency with played cards.
        //  For E/W players, boards use canonical SCARD representatives, so we
        //  check SCARD group membership rather than exact card presence.
        //  Restricted choice is captured automatically through dupl[] and
        //  renormalization — no explicit probability adjustment is needed.
        scale = 0.0;

        for (i = 0; i < boardcount; i++) {
            int consistent = YES;

            for (ply = 0; ply < played_length[t]; ply++) {
                p = ply_player[ply];
                card = played_card[t][ply];

                if (EW(p)) {
                    if (card == DISCARD) {
                        //  discard means this defender is void
                        if (board[p][i] != RENONCE) {
                            consistent = NO;
                            break;
                        }
                    }
                    else {
                        //  find the SCARD group containing the played card
                        group = find_scard_group(p, card);
                        //  check that this defender has a card from this group
                        if (!(board[p][i] & group)) {
                            consistent = NO;
                            break;
                        }
                    }
                }
            }

            if (consistent) {
                scale += prob[i] * dupl[i];
            }
            else {
                prob[i] = 0.0;
            }
        }

        //  Step 4: check that at least one board survives
        if (scale < EPS) {
            printf("No board distributions are consistent with played trick %d; the program stops\n", t + 1);
            output_error(output, "No boards consistent with played trick");
            return -1;
        }

        //  Step 5: remove played cards from board[][] and brd[0][]
        for (ply = 0; ply < played_length[t]; ply++) {
            p = ply_player[ply];
            card = played_card[t][ply];

            if (card != DISCARD) {
                //  remove from brd[0] and board[][] for N/S hands
                if (NS(p)) {
                    brd[0][p] &= ~card;
                    for (i = 0; i < boardcount; i++)
                        board[p][i] = brd[0][p];
                }

                //  remove one canonical card from the SCARD group for E/W hands
                if (EW(p)) {
                    group = find_scard_group(p, card);
                    for (i = 0; i < boardcount; i++)
                        if (prob[i] > EPS)
                            board[p][i] ^= low_c(board[p][i] & group);
                }
            }
        }

        //  recompute brd[0][west] and brd[0][east] as union of surviving boards
        brd[0][west] = RENONCE;
        brd[0][east] = RENONCE;
        for (i = 0; i < boardcount; i++) {
            if (prob[i] > EPS) {
                brd[0][west] |= board[west][i];
                brd[0][east] |= board[east][i];
            }
        }

        //  Step 6: compact boards (remove zero-probability entries)
        new_boardcount = 0;
        for (i = 0; i < boardcount; i++) {
            if (prob[i] > EPS) {
                if (new_boardcount != i) {
                    board[north][new_boardcount] = board[north][i];
                    board[south][new_boardcount] = board[south][i];
                    board[east][new_boardcount]  = board[east][i];
                    board[west][new_boardcount]  = board[west][i];
                    dupl[new_boardcount]         = dupl[i];
                    prob[new_boardcount]         = prob[i];
                }
                new_boardcount++;
            }
        }
        boardcount = new_boardcount;

        //  Step 7: renormalize probabilities
        scale = 0.0;
        for (i = 0; i < boardcount; i++)
            scale += prob[i] * dupl[i];
        for (i = 0; i < boardcount; i++)
            prob[i] /= scale;

        //  Step 8: do not change the leader based on trick winner.
        //  The caller (BEN) already knows who is on lead and passes
        //  the correct hand as North. The played tricks are only used
        //  to filter boards and remove cards.
    }

    //  After all played tricks: update derived state
    //  leader stays as originally set, but if leader has no cards left,
    //  transfer to partner (e.g. South played all cards, North continues)
    if (brd[0][leader] == RENONCE) {
        leader = (leader + 2) % 4;
    }

    //  clear opening lead restriction (we are past the opening)
    opening_lead = 0x7fff;

    //  recompute ncards[] from brd[0][]
    for (p = 0; p < 4; p++)
        ncards[p] = count_cards(brd[0][p]);

    //  recompute ntricks, tricklimit, maxprint
    ntricks = ewwinners ? MAX(ncards[east], ncards[west])
        : defence ? MIN(MAX(ncards[north], ncards[south]), MAX(ncards[east], ncards[west]))
        : MAX(ncards[north], ncards[south]);
    tricklimit = MIN(tricklimit, ntricks);
    maxprint = MAX(ncards[north], ncards[south]) - 1;
    maxprint = MIN(maxprint, count_cards(brd[0][west]) / 2);
    maxprint = tricklimit ? MAX(maxprint, 1) : 0;
    maxprint *= 4;

    //  recompute SCARD sequences for all players
    {
        int   q;
        struct SCARD* sc;
        COLOR cards;

        for (pp = 0; pp < 4; pp++) {
            q = NO;
            sc = &scards[0][pp];
            sc->c[sc->n = 0] = (COLOR)0;
            cards = brd[0][west] | brd[0][north] | brd[0][east] | brd[0][south];
            cards ^= cards & brd[0][pp];

            for (i = ace; i >= two; i--) {
                if (brd[0][pp] & addcard(i)) {
                    if (q)
                        sc->c[sc->n] |= addcard(i);
                    else {
                        q = YES;
                        sc->c[++sc->n] = addcard(i);
                    }
                }
                else if (cards & addcard(i))
                    q = NO;
            }
        }
    }

    //  rebuild the root node and its index list
    {
        struct INDEX* lindex = NIL;

        icount = 0;
        icountmax = 0;
        nextindex = savenextindex;

        wnode[0]->d = NIL;
        wnode[0]->l = NIL;
        wnode[0]->r = NIL;
        wnode[0]->i = nextindex;
        wnode[0]->c = RENONCE;
        wnode[0]->e_down = 0;
        wnode[0]->e_up = 0;
        wnode[0]->ns = ewwinners ? tricklimit : 0;
        wnode[0]->p = defence ? north : west;
        wnode[0]->w_c = RENONCE;
        wnode[0]->w_p = wnode[0]->p;
        wnode[0]->term = NO;

        for (i = 0; i < boardcount; i++) {
            bump_index(lindex);
            lindex->r = nextindex;
            lindex->i = i;
            lindex->min = 0;
            lindex->max = tricklimit;
            for (pp = 0; pp < 4; pp++)
                lindex->c[pp] = board[pp][i];
        }
        lindex->r = NIL;
    }

    //  regenerate the board header
    create_board_header();

    //  print the updated suit combination info
    if (!SUITCAPP) print_suitc();

    return 0;
}
