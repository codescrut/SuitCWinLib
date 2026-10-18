//  name:
//      process_args
//
//  purpose:
//      process_args processes the command line
//
//  to produce a list of options:
//      SuitC {?|-?|-h}
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

int process_args(int argc, char** argv)
{
    int          i, k, n, p = north, pl, posarg = 0, printoptions = NO, sign, val;
    char         c;
    COLOR        cards;
    static char  symbol1[] = "-123456789TJQKA", symbol2[] = "-123456789tjqka", symbol3[] = "-123456789TBVHA", symbol4[] = "-123456789tbvha";

    // Print all provided arguments
    // printf("Arguments provided:\n");
    // for (int i = 0; i < argc; i++) {
    //     printf("argv[%d]: %s\n", i, argv[i]);
    // }

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            c = argv[i][1];
            if (c == '?')
                printoptions = YES;
            else if (c == 'A')
                printallresults = YES;
            else if (c == 'a')
                printsummary = YES;
            else if (c == 'b')
                printboards = YES;
            else if (c == 'c') {
                k = 2;
                cutoff = 0;
                while (argv[i][k]) {
                    if (argv[i][k] == 'k' || argv[i][k] == 'K') cutoff = cutoff * 1000;
                    else if (argv[i][k] == 'm' || argv[i][k] == 'M') cutoff = cutoff * 1000000;
                    else if (argv[i][k] == 'g' || argv[i][k] == 'G') cutoff = cutoff * 1000000000;
                    else                                               cutoff = cutoff * 10 + argv[i][k] - '0';
                    k++;
                }
                cutoff = cutoff ? MAX(cutoff, 20000L) : CUTOFF;
            }
            else if (c == 'D') {
                defence = YES;
                if (argv[i][2] == 'W' || argv[i][2] == 'w') {
                    ewwinners = YES;
                    cutoff = CUTOFF;
                }
            }
            else if (c == 'd')
                printtrunctree = YES;
            else if (c == 'E' || c == 'W') {
                k = 2;
                cards = RENONCE;
                while (argv[i][k]) {
                    c = argv[i][k];
                    for (n = two; n <= ace; n++)
                        if ((c == symbol1[n]) || (c == symbol2[n]) || (c == symbol3[n]) || (c == symbol4[n]))
                            cards |= addcard(n);
                    k++;
                }
                if (argv[i][1] == 'E')
                    eastfixed = cards;
                else
                    westfixed = cards;
            }
            else if (c == 'e' || c == 'w') {
                k = 3;
                val = 0;
                while (argv[i][k]) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                if (c == 'e' && argv[i][2] == 'v')
                    eastvac = val;
                else if (c == 'e' && argv[i][2] == 'm')
                    ncardsmin[east] = val;
                else if (c == 'w' && argv[i][2] == 'v')
                    westvac = val;
                else if (c == 'w' && argv[i][2] == 'm')
                    ncardsmin[west] = val;
            }
            else if (c == 'F' && argv[i][2] == 's') {
                k = 3;
                val = 0;
                while (argv[i][k]) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                foldskip = val;
            }
            else if (c == 'F') {
                k = 2;
                val = 0;
                while (argv[i][k]) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                foldselect = val;
            }
            else if (c == 'f')
                printfulltree = YES;
            else if (c == 'h')
                printoptions = YES;
            else if (c == 'I') {
                matchpoint = IMP;
                matchpointtime = IMP;
                k = 2;
                n = -1;     //  set by <n>: in -I<n>:<score>
                val = 0;
                sign = +1;
                while (argv[i][k]) {
                    c = argv[i][k];
                    if (c == ':') {
                        n = val;
                        val = 0;
                    }
                    else if (c == '+')
                        sign = +1;
                    else if (c == '-')
                        sign = -1;
                    else
                        val = 10 * val + c - '0';
                    k++;
                }
                //  Rev 2.022 rejects a zero score; the library also rejects a
                //  missing or out of range trick count instead of writing
                //  pointscore[] through it
                if (val == 0 || n < 0 || n > NCARDS) {
                    printf("Incorrect specification of the IMP score: %s; the program stops\n", argv[i]);
                    return -1;
                }
                pointscore[n] = sign * val;
            }
            else if (c == 'L') {
                c = argv[i][2];
                leader = c == 'n' ? north : c == 'e' ? east : c == 's' ? south : c == 'w' ? west : noplayer;
                if (leader == noplayer) {
                    printf("Incorrect player in %s; the program stops\n", argv[i]);
                    return -1;
                }
                if (argv[i][3]) {
                    c = argv[i][3];
                    opening_lead = 0x0;
                    for (n = two; n <= ace; n++)
                        if ((c == symbol1[n]) || (c == symbol2[n]) || (c == symbol3[n]) || (c == symbol4[n]))
                            opening_lead |= addcard(n);
                }
            }
            else if (c == 'l') {
                c = argv[i][2];
                pl = c == 'w' ? west : c == 'n' ? north : c == 'e' ? east : c == 's' ? south : -1;
                if (pl < 0) {
                    printf("Incorrect player in %s; the program stops\n", argv[i]);
                    return -1;
                }
                nfree[0][pl] = argv[i][3] - '0';
            }
            else if (c == 'M') {
                k = 2;
                val = 0;
                while (argv[i][k]) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                matchpoint = MP;
                matchpointtime = MP;
                if (val)
                    maxnodes = val;
            }
            else if (c == 'm') {
                if ((argv[i][2] == 'U' || argv[i][2] == 'u') && (argv[i][3] == 'S' || argv[i][3] == 's'))
                    foldmode = 2;
                else if (argv[i][2] == 'U' || argv[i][2] == 'u')
                    foldmode = 1;
                else if ((argv[i][2] == 'D' || argv[i][2] == 'u') && (argv[i][3] == 'S' || argv[i][3] == 's'))
                    foldmode = 4;
                else if (argv[i][2] == 'D' || argv[i][2] == 'd')
                    foldmode = 3;
                else {
                    printf("Incorrect foldmode: %s; the program stops\n", argv[i]);
                    return -1;
                }
            }
            else if (c == 'o')
                reorder = SUITCAPP ? YES : NO;
            else if (c == 'P') {
                printoption = argv[i][2] - '0';
                if (printoption < 0 || printoption > 3) {
                    printf("Incorrect print option %s; the program stops\n", argv[i]);
                    return -1;
                }
            }
            else if (c == 'p') {
                c = argv[i][2];
                if (c == 'H')
                    printhighcard = YES;
                else if (c == 'I' || c == 'M')
                    printallmatchpoint = YES;
                else if (c == 'S')
                    printscard = SUITCAPP ? YES : YES;
                else {
                    printf("Incorrect print option; the program stops\n");
                    return -1;
                }
            }
            else if (c == 'R') {
                c = argv[i][2];
                pl = c == 'n' ? north : c == 's' ? south : c == 'w' ? west : c == 'e' ? east : -1;
                if (pl < 0) {
                    printf("Incorrect player in parameter; the program stops\n");
                    return -1;
                }
                nruff[0][pl] = argv[i][3] - '0';
            }
            else if (c == 'r')
                resultsonly = YES;
            else if (c == 'S') {
                k = 2;
                cards = RENONCE;
                while (argv[i][k]) {
                    c = argv[i][k];
                    if (c == ':') {
                        h2rule[nrules] = cards;
                        cards = RENONCE;
                    }
                    for (n = two; n <= ace; n++)
                        if ((c == symbol1[n]) || (c == symbol2[n]) || (c == symbol3[n]) || (c == symbol4[n]))
                            cards |= addcard(n);
                    k++;
                }
                h3rule[nrules] = cards;
                nrules++;
            }
            else if (c == 'T') {
                k = 2;
                val = 0;
                while (argv[i][k]) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                trace = val;
            }
            else if (c == 't') {
                k = 2;
                val = 0;
                while (argv[i][k]) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                tricklimit = val;
            }
            else if (c == 'u')
                usesuitentries = YES;
            else if (c == 'z') {
                k = 2;
                val = 0;
                while (argv[i][k]) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                zermelo = val;
            }
            else {
                printf("Incorrect option: %s; the program stops\n", argv[i]);
                return -1;
            }
        }

        //  enter the hands in the order: north, south, east-west
        //  4th and subsequent positional args are played tricks
        else {
            posarg++;
            if (posarg <= 3) {
                k = 0;
                brd[0][p] = RENONCE;
                while (argv[i][k]) {
                    c = argv[i][k];
                    for (n = two; n <= ace; n++)
                        if ((c == symbol1[n]) || (c == symbol2[n]) || (c == symbol3[n]) || (c == symbol4[n]))
                            brd[0][p] |= addcard(n);
                    k++;
                }
                p = (p == north ? south : west);
            }
            else {
                //  parse played trick: [n|e|s|w]<cards>, '-' for discard
                //  optional leading seat letter overrides the trick leader
                if (nplayedtricks >= MAX_PLAYED_TRICKS) {
                    printf("Too many played tricks specified; the program stops\n");
                    return -1;
                }
                k = 0;
                pl = 0;
                played_leader[nplayedtricks] = noplayer;
                c = argv[i][0];
                if (c == 'n' || c == 'e' || c == 's' || c == 'w') {
                    played_leader[nplayedtricks] = c == 'n' ? north : c == 'e' ? east : c == 's' ? south : west;
                    k = 1;
                }
                while (argv[i][k] && pl < 4) {
                    c = argv[i][k];
                    if (c == '-') {
                        played_card[nplayedtricks][pl] = DISCARD;
                    }
                    else {
                        played_card[nplayedtricks][pl] = RENONCE;
                        for (n = two; n <= ace; n++)
                            if ((c == symbol1[n]) || (c == symbol2[n]) || (c == symbol3[n]) || (c == symbol4[n]))
                                played_card[nplayedtricks][pl] = addcard(n);
                        if (played_card[nplayedtricks][pl] == RENONCE) {
                            printf("Invalid card '%c' in played trick %s; the program stops\n", c, argv[i]);
                            return -1;
                        }
                    }
                    pl++;
                    k++;
                }
                if (pl < 2) {
                    printf("Played trick must have at least 2 cards in %s; the program stops\n", argv[i]);
                    return -1;
                }
                played_length[nplayedtricks] = pl;
                nplayedtricks++;
            }
        }
    }

    //  print the options
    if (!printoptions)
        return 0;

    printf("\n");
    printf("calling sequence:\n");
    printf("\n");
    printf("SuitC [{-?|-h}] [-A] [-a] [-b] [-c<cutoff>[kKmMgG]] [-D[{W|w}]] [-d] [-E<cards>] [-em<n>] [-ev<n>] [-F<n>] [-Fs<n>] [-f]\n");
    printf("      [-L{n|e|s|w}[<card>]] [-l{n|e|s|w}<n>] [-M[nodes]] [-m{U|US|D|DS}] [-o] [-P<n>] [-p[{H|I|M|S}]] [-R{n|e|s|w}<n>] \n");
    printf("      [-r] [-S<card2>:<card3>] [-T<n>] [-t<tricks>] [-u] [-W<cards>] [-wm<n>] [-wv<n>] [-z<n>] <ncards> <scards> <ewcards>\n");
    printf("\n");
    printf("options:\n");
    printf("    {-?|-h}         print the options of SuitC\n");
    printf("    -A              print a full report of all results\n");
    printf("    -a              print a summary of the results\n");
    printf("    -b              print a list of the boards\n");
    printf("    -c<cutoff>[kKmMgG]\n");
    printf("                    the limit of the product of the number of quirks while merging quirk trees\n");
    printf("                    [kK] stands for 1000, [mM] stands for 1000000, [gG] stands for 1000000000\n");
    printf("    -D[{W|w}]       the defenders lead to each trick\n");
    printf("                    {W|w}: print east-west winners instead of north-south winners\n");
    printf("                    the -D option should be used only if east-west have at least twice the number of cards as north and south\n");
    printf("    -d              print the truncated NODE tree\n");
    printf("    -E<cards>       all east hands contain <cards>\n");
    printf("    -em<n>          the minimum number of cards in the east hand is <n>\n");
    printf("    -ev<n>          the number of vacant places in the east hand is <n>\n");
    printf("    -F<n>           1=fold the EW plays, 2=fold the NS plays, 4=select the NS plays, 8=select the NS plays followed by fold the EW plays\n");
    printf("    -Fs<n>          don't fold the first <n> plies of the tree\n");
    printf("    -f              print the full NODE tree\n");
    printf("    -I<n>:<score>   the score for <n> tricks is <score> points\n");
    printf("    -L{n|e|s|w}[<card>]\n");
    printf("                    the lead is in {n|e|s|w} at trick one\n");
    printf("                    card <card> is lead if a card is specified\n");
    printf("    -l{n|e|s|w}<n>  limit the number of free directed leads from {n|e|s|w} to <n>\n");
    printf("    -m{U|US|D|DS}   select the fold mode for the final game tree (U=UP, US=UPSTRIDE, D=DOWN, DS=DOWNSTRIDE)\n");
    printf("    -M[nodes]       generate the optimal MP play with maximal <nodes> nodes\n");
    printf("    -o              switch the order of the boards\n");
    printf("                    if SUITCAPP==NO:  print the east-west hands in decreasing number of cards in the West hand\n");
    printf("                    if SUITCAPP==YES: print the east-west hands in Warmerdams order\n");
    printf("    -P<n>           select printoption (0=English lower, 1=Dutch lower, 2=English upper, 3=Dutch upper\n");
    printf("    -p[{H|I|M|S}]   H: print the the highest card of a north-south card sequence in the game tree\n");
    printf("                    {I|M}: print MP or IMP information for all plays\n");
    printf("                    S: if SUITCAPP==NO:  print different card symbols for each card in a card sequence\n");
    printf("                       if SUITCAPP==YES: print the same card symbols for each card in a card sequence\n");
    printf("    -R{n|e|s|w}<n>  the number of ruffs available in {n|e|s|w}\n");
    printf("    -r              print only the results\n");
    printf("    -S<card2>:<card3> play in 3rd hand a card from card3 if 2nd hand plays a card from card2\n");
    printf("    -T<n>           trace options: 1=timing, 2=minimax progress\n");
    printf("    -t<tricks>      limit the number of tricks that are played\n");
    printf("    -u              select the plays that maximize the use of entries in the suit itself\n");
    printf("    -W<cards>       all west hands contain <cards>\n");
    printf("    -wm<n>          the minimum number of cards in the west hand is <n>\n");
    printf("    -wv<n>          the number of vacant places in the west hand is <n>\n");
    printf("    -z<n>           delete weakly dominated plays in check_zermelo (zermelo==1) or in evaluate_mp_results (zermelo==2)\n");
    printf("    <cards>, etc    a sequence of Dutch or English card symbols.\n");
    printf("                    <cards> can have the special values '.', '=' or '#'\n");
    printf("                    for the north or south hand to denote a renonce\n");
    printf("\n");
    exit(0);
}
