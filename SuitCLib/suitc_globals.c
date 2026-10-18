#include "header.h" // Or whatever your header file is named

// Define "SUITCLIB_EXPORTS"  if you are using this to control the SUITC_API macro
// For GCC, the SUITC_API macro will expand to visibility attribute, which is fine
// during definition as well.

//  list of variables - DEFINITIONS
//  ----------------------------------------------------------------------------
//      Copyright 2024, Gerrit A. van der Velde, Leiden
double stats[4] = { -1.0, -2.0, -3.0, -4.0 }; // Or 0.0, or any distinct values

char   b_print[NCARDS + 2][512];
COLOR  board[4][8192];
COLOR  brd[NCARDS + 1][4];
COLOR  eastfixed;
COLOR  h2rule[NCARDS];
COLOR  h3rule[NCARDS];
COLOR  westfixed;
double a[NNODE][2 * NNODE];
double perc[NBOARDS];
double prob[8192];
// Initialize times with some recognizable values

int    boardcount;
int    buildtree;
int    defence;
int    dupl[8192];
int    eastvac;
int    ew_c_count;
int    ewwinners;
int    icount;
int    icountmax;
int    leader;
int    matchpoint;
int    matchpointtime;
int    maxnodes;
int    maxns;
int    maxprint;
int    foldmode;
int    foldselect;
int    foldskip;
int    minns;
int    nbest_plays;
int    ncards[4];
int    ncardsmin[4];
int    ncount;
int    ncountmax;
int    nfree[NCARDS + 1][4];
int    nplayedtricks;
int    played_length[MAX_PLAYED_TRICKS];
int    played_leader[MAX_PLAYED_TRICKS];
COLOR  played_card[MAX_PLAYED_TRICKS][4];
int    nplays;
int    nruff[NCARDS + 1][4];
int    nrules;
int    nslimits[2][NBOARDS];
int    nstricks[NBOARDS];
int    ntricks;
int    opening_lead;
int    pointscore[NCARDS + 1];
int    printallmatchpoint;
int    printallresults;
int    printboards;
int    printfulltree;
int    printhighcard;
int    printoption;
int    printscard;
int    printsummary;
int    printtrunctree;
int    printconsole;
int    qdcount;
int    qdcountmax;
int    qhcount;
int    qhcountmax;
int    reorder;
int    resultsonly;
int    usesuitentries;
int    poolfull;
int    resultrange;
int    trace;
int    treeoverflow;
int    tricklimit;
int    westvac;
int    z_count;
int    zermelo;
long   cutoff;
struct INDEX* nextindex;
struct NODE* nextnode;
struct NODE* tnode[NPLIES + 1];
struct NODE* wnode[NPLIES + 1];
struct QUIRKD* nextquirkd;
struct QUIRKD quirkdl[NPLIES + 1];
struct QUIRKH* nextquirkh;
struct RESULT best_play[32];
struct RESULT best_result;
struct SCARD  scards[NCARDS + 1][4];
struct SCORE* score;

struct ST_PlyData st[NPLIES+1]; // Definition for the st array

struct INDEX* savenextindex;
struct NODE* savewnode0;
struct QUIRKD* savenextquirkd;
struct QUIRKH* savenextquirkh;

int noOfOptimumDetails;
bool outputEnabled;

// Note: Function definitions (like output_add, create_game_tree_json_value, suitc_main, etc.)
// should be in their respective .c/.cpp files, not here unless this IS that file.
// This suitc_globals.c is primarily for global variable DEFINITIONS.
