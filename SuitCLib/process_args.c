//  name:
//      process_args
//
//  purpose:
//      process_args processes the command line
//
//  to produce a list of options:
//      SuitC {?|-?|-h}
//
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void process_args( int argc, char **argv )
{
    int          i, k, n, p = north, pl, printoptions = NO, sign, val;
    char         c;
    COLOR        cards;
    static char  symbol1[] = "-123456789TJQKA", symbol2[] = "-123456789tjqka", symbol3[] = "-123456789TBVHA", symbol4[] = "-123456789tbvha";

    for ( i = 1; i < argc; i++ ) {
        if ( argv[i][0] == '-' ) {
            c = argv[i][1];
            if ( c == '?' )
                printoptions = YES;
            else if ( c == 'A' )
                printallresults = YES;
            else if ( c == 'a' )
                printsummary = YES;
            else if ( c == 'b' )
                printboards = YES;
            else if ( c == 'c' ) {
                k = 2;
                val = 0;
                while ( argv[i][k] ) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                cutoff = val ? MAX(val, 10000) : CUTOFF;
            }
            else if ( c == 'D' )
                defence = YES;
            else if ( c == 'd' )
                printtrunctree = YES;
            else if ( c == 'E' || c == 'W' ) {
                k = 2;
                cards = RENONCE;
                while ( argv[i][k] ) {
                    c = argv[i][k];
                    for ( n = two; n <= ace; n++ )
                        if ( ( c == symbol1[n] ) || ( c == symbol2[n] ) || ( c == symbol3[n] ) || ( c == symbol4[n] ) )
                            cards |= addcard(n);
                    k++;
                }
                if ( argv[i][1] == 'E' )
                    eastfixed = cards;
                else
                    westfixed = cards;
            }
            else if ( c == 'e' || c == 'w' ) {
                k = 3;
                val = 0;
                while ( argv[i][k] ) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                if ( c == 'e' && argv[i][2] == 'v' )
                    eastvac = val;
                else if ( c == 'e' && argv[i][2] == 'm' )
                    ncardsmin[east] = val;
                else if ( c == 'w' && argv[i][2] == 'v' )
                    westvac = val;
                else if ( c == 'w' && argv[i][2] == 'm' )
                    ncardsmin[west] = val;
            }
            else if ( c == 'F' ) {
                k = 2;
                val = 0;
                while ( argv[i][k] ) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                mergeselect = val;
            }
            else if ( c == 'f' )
                printfulltree = YES;
            else if ( c == 'H' ) {
                c = argv[i][2];
                throwin = c == 'e' ? east : c == 'w' ? west : noplayer;
                if ( throwin == noplayer ) {
                    printf( "Incorrect player can be thrown in: %s; the program stops\n", argv[i] );
                    exit( 1 );
                }
            }
            else if ( c == 'h' )
                printoptions = YES;
            else if ( c == 'n' ) {
                c = argv[i][2];
                if ( c == 'C' )
                    compacttree = NO;
                else if ( c == 'K' ) {
                    if ( argv[i][3] ) {
                        if ( argv[i][3] == 'e' || argv[i][3] == 'w' )
                            keepewweak = NO;
                        else if ( argv[i][3] == 'n' || argv[i][3] == 's' )
                            keepnsweak = NO;
                        else {
                            printf( "Incorrect option: %s; the program stops\n", argv[i] );
                            exit( 1 );
                        }
                    }
                    else {
                        keepewweak = NO;
                        keepnsweak = NO;
                    }
                }
                else if ( c == 'k' ) {
                    if ( argv[i][3] ) {
                        if ( argv[i][3] == 'e' || argv[i][3] == 'w' )
                            keepewequal = NO;
                        else if ( argv[i][3] == 'n' || argv[i][3] == 's' )
                            keepnsequal = NO;
                        else {
                            printf( "Incorrect option: %s; the program stops\n", argv[i] );
                            exit( 1 );
                        }
                    }
                    else {
                        keepewequal = NO;
                        keepnsequal = NO;
                    }
                }
            }
            else if ( c == 'L' ) {
                c = argv[i][2];
                leader = c == 'n' ? north : c == 'e' ? east : c == 's' ? south : c == 'w' ? west : noplayer;
                if ( leader == noplayer ) {
                    printf( "Incorrect player in %s; the program stops\n", argv[i] );
                    exit( 1 );
                }
                w[0] = leader;
                if ( argv[i][3] ) {
                    c = argv[i][3];
                    opening_lead = 0x0;
                    for ( n = two; n <= ace; n++ )
                        if ( ( c == symbol1[n] ) || ( c == symbol2[n] ) || ( c == symbol3[n] ) || ( c == symbol4[n] ) )
                            opening_lead |= addcard(n);
                }
            }
            else if ( c == 'l' ) {
                c = argv[i][2];
                pl = c == 'w' ? west : c == 'n' ? north : c == 'e' ? east : c == 's' ? south : -1;
                if ( pl < 0 ) {
                    printf( "Incorrect player in %s; the program stops\n", argv[i] );
                    exit( 1 );
                }
                nfree[0][pl] = argv[i][3] - '0';
            }
            else if ( c == 'M' ) {
                k = 2;
                val = 0;
                while ( argv[i][k] ) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                matchpoint = YES;
                if ( val )
                    maxnodes = val;
            }
            else if ( c == 'o' )
                reorder = YES;
            else if ( c == 'P' )
                 printoption= argv[i][2] - '0';
            else if ( c == 'R' ) {
                c = argv[i][2];
                pl =  c == 'n' ? north :  c == 's' ? south : c == 'w' ? west : c == 'e' ? east : -1;
                if ( pl < 0 ) {
                    printf( "Incorrect player in %s; the program stops\n", argv[i] );
                    exit( 1 );
                }
                nruff[0][pl] = argv[i][3] - '0';
            }
            else if ( c == 'r' )
                resultsonly = YES;
            else if ( c == 'S' ) {
                k = 2;
                cards = RENONCE;
                while ( argv[i][k] ) {
                    c = argv[i][k];
                    if ( c == ':' ) {
                        h2rule[nrules] = cards;
                        cards = RENONCE;
                    }
                    for ( n = two; n <= ace; n++ )
                        if ( ( c == symbol1[n] ) || ( c == symbol2[n] ) || ( c == symbol3[n] ) || ( c == symbol4[n] ) )
                            cards |= addcard(n);
                    k++;
                }
                h3rule[nrules] = cards;
                nrules++;
                buildtree = 1;
            }
            else if ( c == 's' )
                sortplays = YES;
            else if ( c == 'T' ) {
                k = 2;
                val = 0;
                while ( argv[i][k] ) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                trace = val;
            }
            else if ( c == 't' ) {
                k = 2;
                val = 0;
                while ( argv[i][k] ) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                tricklimit = val;
            }
            else if ( c == 'z' ) {
                k = 2;
                val = 0;
                while ( argv[i][k] ) {
                    val = 10 * val + argv[i][k] - '0';
                    k++;
                }
                zermelo = val;
            }
            else {
                printf( "Incorrect option: %s; the program stops\n", argv[i] );
                exit( 1 );
            }
        }

        //  enter the hands in the order: north, south, east-west
        else {
            k = 0;
            brd[0][p] = RENONCE;
            while ( argv[i][k] ) {
                c = argv[i][k];
                for ( n = two; n <= ace; n++ )
                    if ( ( c == symbol1[n] ) || ( c == symbol2[n] ) || ( c == symbol3[n] ) || ( c == symbol4[n] ) )
                        brd[0][p] |= addcard(n);
                k++;
            }
            p = ( p == north ? south : west );
        }
    }

    //  clear some options if only the results are required
    if ( resultsonly ) {
        keepewequal = NO;
        keepewweak = NO;
        keepnsequal = NO;
        keepnsweak = NO;
        printallresults = NO;
    }

    //  print the options
    if ( ! printoptions )
        return;

    printf( "\n" );
    printf( "calling sequence:\n" );
    printf( "\n" );
    printf( "SuitC [{-?|-h}] [-A] [-a] [-b] [-nC] [-c<cutoff>] [-D] [-d] [-E<cards>] [-em<n>] [-ev<n>] [-F<options>] [-f] [-H{e|w}] [-h]\n" );
    printf( "      [-nK[{e|n|s|w}]] [-k[{e|n|s|w}]] [-L{n|e|s|w}[<card>]] [-l{n|e|s|w}<n>] [-M[nodes]] [-o] [-P<n>] [-R{n|e|s|w}<n>] \n" );
    printf( "      [-r] [-S<card2>:<card3>] [-s] [-T<n>] [-t<tricks>] [-W<cards>] [-wm<n>] [-wv<n>] [-z<n>] <ncards> <scards> <ewcards>\n" );
    printf( "\n" );
    printf( "options:\n" );
    printf( "    {-?|-h}         print the options of SuitC\n" );
    printf( "    -A              print a full report of all results\n" );
    printf( "    -a              print a summary of the results\n" );
    printf( "    -b              print a list of the boards\n" );
    printf( "    -c<cutoff>      the limit in millions on the product of the number of quirks while merging quirk trees\n" );
    printf( "    -D              the defenders lead to each trick\n" );
    printf( "    -d              print the truncated NODE tree\n" );
    printf( "    -E<cards>       all east hands contain <cards>\n" );
    printf( "    -em<n>          the minimum number of cards in the east hand is <n>\n" );
    printf( "    -ev<n>          the number of vacant places in the east hand is <n>\n" );
    printf( "    -F<n>           1=merge the EW plays, 2=merge the NS plays, 4=select the NS plays\n" );
    printf( "    -f              print the full NODE tree\n" );
    printf( "    -H{e|w}         the designated player leads to the current trick if he won the last trick\n" );
    printf( "    -nC             north-south win tricks as soon as possible\n" );
    printf( "    -nK             don't keep weakly dominated nodes\n" );
    printf( "    -nK{e|w}        don't keep weakly dominated EW nodes\n" );
    printf( "    -nK{n|s}        don't keep weakly dominated NS nodes\n" );
    printf( "    -nk             don't keep tied nodes\n" );
    printf( "    -nk{e|w}        don't keep tied EW nodes\n" );
    printf( "    -nk{n|s}        don't keep tied NS nodes\n" );
    printf( "    -L{n|e|s|w}[<card>]\n" );
    printf( "                    the lead is in {n|e|s|w} at trick one\n" );
    printf( "                    card <card> is lead if a card is specified\n" );
    printf( "    -l{n|e|s|w}<n>  limit the number of free directed leads from {n|e|s|w} to <n>\n" );
    printf( "    -M[nodes]       generate the optimal matchpoint play with maximal <nodes> nodes\n" );
    printf( "    -o              sort the boards in the same order as that used by Jeroen Warmerdam\n" );
    printf( "    -P<n>           select printoption (0=English lower, 1=Dutch lower, 2=English upper, 3=Dutch upper\n" );
    printf( "    -R{n|e|s|w}<n>  the number of ruffs available in {n|e|s|w}\n" );
    printf( "    -S<card2>:<card3> play in 3rd hand a card from card3 if 2nd hand plays a card from card2\n" );
    printf( "    -s              sort the plays in decreasing number of leaves\n" );
    printf( "    -r              print only the results\n" );
    printf( "    -T<n>           trace options: 1=timing, 2=minimax progress\n" );
    printf( "    -t<tricks>      limit the number of tricks that are played\n" );
    printf( "    -W<cards>       all west hands contain <cards>\n" );
    printf( "    -wm<n>          the minimum number of cards in the west hand is <n>\n" );
    printf( "    -wv<n>          the number of vacant places in the west hand is <n>\n" );
    printf( "    -z<n>           delete weakly dominated plays in evaluate_mp_results (zermelo==2) or in check_zermelo (zermelo==1)\n" );
    printf( "\n" );
    printf( "    <n>             a numeric value between 0 and 13\n" );
    printf( "    <cards>, etc    a sequence of Dutch or English card symbols.\n" );
    printf( "                    <cards> can have the special values '.', '=' or '#'\n" );
    printf( "                    for the north or south hand to denote a renonce\n" );
    printf( "\n" );
    exit( 0 );
}
