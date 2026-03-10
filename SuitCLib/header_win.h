#pragma once
#ifndef HEADER_H_SUITC
#define HEADER_H_SUITC
#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
//SuitC
//Copyright 2020, Gerrit A. van der Velde, Leiden
//
//Windows library version created by
//Lars Kappel Hansen
//https://bridge.esmarkkappel.dk
//  list of cpp defines
//  ----------------------------------------------------------------------------
#define addcard( c )                    ( ( (COLOR) 1 ) << ( c ) )
#define BLANK                           NO
#define CUTOFF                          1000000L
#define EPS                             1.0e-10
#define EW( p )                         ( ( p == west ) || ( p == east ) )
#define IMP                             2
#define low_c( c )                      ( ( c ) & -( c ) )
#define MAX( i, j )                     ( ( i ) > ( j ) ? ( i ) : ( j ) )
#define MIN( i, j )                     ( ( i ) < ( j ) ? ( i ) : ( j ) )
#define MP                              1
#define NBOARDS                         162
#define NCARDS                          13
#define NIL                             0
#define NINDICES                        16000000L    //  -Dw: 135892684L
#define NNODE                           400
#define NNODES                          10000000L    //  -Dw:  97253581L
#define NO                              0
#define NQUIRKS                         ( SUITCAPP ? 4000000L : 80000000L )
#define NPLIES                          52
#define NS( p )                         ( ( p == north ) || ( p == south ) )
#define RENONCE                         ( (COLOR) 0 )
#define TICK                            1.0e-3
#define w_ew( lnode )                   ( lnode[d]->w_c & ( brd[0][west] | brd[0][east] ) )
#define w_ns( lnode )                   ( lnode[d]->c & lnode[d]->w_c )
#define YES                             1
//  list of typedef's and type declarations
//  ----------------------------------------------------------------------------
typedef unsigned short COLOR;
typedef enum { renonce, one, two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace, ruff } CARD;
typedef enum { west, north, east, south, noplayer } SEATS;
struct INDEX {
struct INDEX* r;                   //  pointer to the right sibling index
COLOR         c[4];                 //  the remaining cards of each player
unsigned int  i : 8,                  //  index in the board table
min : 4,                //  minimum number of tricks won by NS
max : 4;                //  maximum number of tricks won by NS
};
struct LIMIT {
float         result;               //  the result in tricks of the play
unsigned int  keep;                 //  indicator to keep this play
};
struct NODE {
struct NODE* d,                   //  pointer to the oldest son node
* l,                   //  pointer to the left sibling node
* r;                   //  pointer to the right sibling node
struct INDEX* i;                   //  pointer to the index list of boards
COLOR         c,                    //  the cards played in this ply
w_c;                  //  the winning card of the current trick
unsigned int  e_down : 8,             //  the topdown indicator for entries in the suit itself
e_up : 8,               //  the bottomup indicator for entries in the suit itself
match : 1,              //  indicator that a match exists for this node in fold_ns_tree
ns : 4,                 //  the number of tricks for north-south (ewwinners==NO) or east-west (ewwinners==YES)
p : 2,                  //  the player of this node
term : 1,               //  indicator that no more choices exist
w_p : 2;                //  the winning player of the current trick
};
struct QUIRKD {
struct QUIRKD* d;                   //  pointer to the son QUIRKD entry
struct QUIRKH* h;                   //  pointer to the sibling QUIRKH entry
unsigned int  ic : 8,                 //  the lowest board number for which the QUIRK tree is checked
ih : 8,                 //  the highest board index in this sequence
il : 8;                 //  the lowest board index in this sequence
};
struct QUIRKH {
union {
struct QUIRKD* d;               //  pointer to the companion QUIRKD entry
struct QUIRKH* h;               //  pointer to the sibling QUIRKH entry
};
struct TRICKS* t;                   //  pointer to the TRICKS entry
};
struct RESULT {
COLOR         optcode;              //  the code setting for optimal plays
int           dupl;                 //  the number of plays with identical results
double        result,               //  the result for the MAX play
res[NCARDS + 1],        //  the result of the safety play for N tricks
tricks[NCARDS + 1];     //  the probability to win N tricks
};
struct SCARD {
int           m,                    //  the number of SCARD sequences including ruffs and voids
n;                    //  the number of SCARD sequences
COLOR         c[10];                //  the SCARD sequences
};
struct SCORE {
struct SCORE* l,                   //  pointer to the next valid left SCORE entry
* r,                   //  pointer to the next valid right SCORE entry
* b;                   //  pointer to the first equivalent SCORE entry
unsigned int  best_MAX : 1,           //  the indicator that this is an optimal MAX play
best_MP : 1,            //  the indicator that this is a MP or IMP play
best_N : 1,             //  the indicator that this is an optimal safety play
ix : 28,                //  the counter for the SCORE entries
w : 1;                  //  the indicator for weakly dominated plays
double* mp,                  //  the value of the MP or IMP plays
mpscore,              //  the matchpoint score (used only for MAX plays)
perc,                 //  the contribution to the MP or IMP play
result;               //  the result in tricks of the play
};
struct TRICKS {
unsigned short i : 8,                 //  the board index
ns : 8;                //  the number of north-south tricks
};
//  list of variables
//  ----------------------------------------------------------------------------
//      Copyright 2024, Gerrit A. van der Velde, Leiden
char   b_print[NCARDS + 2][512];          //  the board_header
COLOR  board[4][8192];                  //  the holding for all possible distributions of the east-west cards
COLOR  brd[NCARDS + 1][4];                //  the unplayed cards at the start of each trick
COLOR  eastfixed;                       //  the known cards in the east hand
COLOR  h2rule[NCARDS];                  //  the second hand conditions of the third hand rules
COLOR  h3rule[NCARDS];                  //  the rules for the third hand strategy
COLOR  westfixed;                       //  the known cards in the west hand
double a[NNODE][2 * NNODE];               //  the matrix with MP or IMP scores
double perc[NBOARDS];                   //  the percentage of the distributions for which the percentage is defined
double prob[8192];                      //  the a priori probabilities for the boards
double stats[4];                        //  array with timings
int    boardcount;                      //  the number of EW distributions
int    buildtree;                       //  the type of the NODE tree. 0=truncated tree, 1=full tree, 2=selected tree
int    defence;                         //  lead from east of west in each trick
int    dupl[8192];                      //  the multiplicity of the boards
int    eastvac;                         //  the number of vacant places in the east hand
int    ew_c_count;                      //  the number of rows in the board header
int    ewwinners;                       //  print east-west winners instead of north-south winners
int    icount;                          //  counter for the number of active index entries
int    icountmax;                       //  the maximum number of active index entries
int    leader;                          //  the openings leader at trick one
int    matchpoint;                      //  generate the matchpoint plays
int    matchpointtime;                  //  print the matchpoint analysis time
int    maxnodes;                        //  the maximum number of nodes used for the MP or IMP evaluation
int    maxns;                           //  the maximum number of ns_tricks
int    maxprint;                        //  the maximum number of tricks to be printed
int    foldselect;                      //  1=fold the EW plays, 2=fold the NS plays, 4=select the NS plays
int    minns;                           //  the minimum number of ns_tricks
int    nbest_plays;                     //  the number of elements in array best_play
int    ncards[4];                       //  the maximum number of cards in each hand
int    ncardsmin[4];                    //  the minimum number of cards in each hand
int    ncount;                          //  counter for the number of active NODE entries
int    ncountmax;                       //  the maximum number of active NODE entries
int    nfree[NCARDS + 1][4];              //  the number of remaining directional leads
int    nplays;                          //  number of possible plays
int    nruff[NCARDS + 1][4];              //  the number of remaining ruffs
int    nrules;                          //  the number of rules for the 3d hand strategy
int    nslimits[2][NBOARDS];            //  the lower and upper bound for the number tricks for north-south (ewwinners==NO) or east-west (ewwinners==YES) on each board
int    nstricks[NBOARDS];               //  the number of tricks for north-south (ewwinners==NO) or east-west (ewwinners==YES) on each board
int    ntricks;                         //  the number of tricks to be played
int    opening_lead;                    //  the card that is lead at trick one
int    pointscore[NCARDS + 1];            //  the pointscore for each number of tricks
int    printallmatchpoint;              //  print the MP or IMP results for all plays
int    printallresults;                 //  print a full report of all results
int    printboards;                     //  print the complete board set
int    printfulltree;                   //  print the full NODE tree
int    printhighcard;                   //  print the the highest card of a north-south card sequence in the game tree
int    printoption;                     //  printoption for the game trees (0=English lower, 1=Dutch lower, 2=English upper, 3=Dutch upper)\n" );
int    printscard;                      //  print different card symbols for each card in a card sequence in the board header
int    printsummary;                    //  print a summary of the results
int    printtrunctree;                  //  print the truncated NODE tree
int    printconsole;                    //  print the truncated NODE tree
int    qdcount;                         //  the number of active QUIRKD entries
int    qdcountmax;                      //  the maximum number of active QUIRKD entries
int    qhcount;                         //  the number of active QUIRKH entries
int    qhcountmax;                      //  the maximum number of active QUIRKH entries
int    reorder;                         //  flag to indicate that Jeroen Warmerdam's order of the boards is used
int    resultsonly;                     //  report only the results, no plays
int    usesuitentries;                  //  maximize the use of entries in the suit itself
int    trace;                           //  trace options: 1=timing, 2=minimax progress
int    tricklimit;                      //  limit on the number of tricks that are played
int    westvac;                         //  the number of vacant places in the west hand
int    z_count;                         //  the maximum number cards in the west hand
int    zermelo;                         //  1: delete weakly dominated plays in evaluate_mp_results, 2: delete weakly dominated plays in check_zermelo
long   cutoff;                          //  limit on the product of the number of quirks while merging
struct INDEX* nextindex;               //  the first unused slot in the INDEX list
struct NODE* nextnode;                //  the first unused slot in the NODE list
struct NODE* tnode[NPLIES + 1];         //  the stack of target NODE entries
struct NODE* wnode[NPLIES + 1];         //  the stack of NODE entries
struct QUIRKD* nextquirkd;              //  the first unused slot in the QUIRKD list
struct QUIRKD quirkdl[NPLIES + 1];        //  pointers to the root of QUIRK trees
struct QUIRKH* nextquirkh;              //  the first unused slot in the QUIRKH list
struct RESULT best_play[32];            //  the plays that give the best results for MAX and N<n>
struct RESULT best_result;              //  the best result of all plays
struct SCARD  scards[NCARDS + 1][4];      //  the SCARD sequences for each trick
struct SCORE* score;                   //  the score of each play
struct {
COLOR        c;                     //  the cards that are played
COLOR        winc;                  //  the provisional winning cards of this trick
unsigned int ld,                    //  YES if the lead is made in this ply
t,                     //  the trick number
w;                     //  the provisional winner of this trick
} st[NPLIES+1];                         //  the description of each ply
//  list of include files
//  ----------------------------------------------------------------------------
#include "bump_codes.h"
//  complete list of routines
//  ----------------------------------------------------------------------------
struct NODE* add_node(int d, int p, COLOR cards);
int            add_quirk(int i, int ns, int s, int t, int ih, struct QUIRKD** tquirkd, struct QUIRKD** tquirkdl);
void           build_selected_tree(void);
void           check_ew_plays(int d, struct NODE** lnode);
void           check_ns_plays(int d, struct NODE** lnode);
void           check_quirk(int d, int t, struct QUIRKD** tquirkd, struct QUIRKD** tquirkdl);
int            check_zermelo(struct SCORE* lscore);
int            clean_quirk(int d, struct LIMIT* results);
void           convert_tree_to_quirk(void);
struct NODE* copy_node(struct NODE* lnode);
struct NODE* copy_tree(int d_, struct NODE* rootn);
int            count_quirk(int d);
int            create_all_boards(void);
void           create_board_header(void);
int            create_boards(char* output);
int            create_ply1(int d, int* lead);
int            create_ply2(int d, int p);
int            create_ply3(int d, int p);
int            create_ply4(int d, int p);
int            create_tricks(int d);
void           delete_board(int d, struct NODE** lnode, int b);
void           delete_quirk(int d);
struct NODE* delete_tree(int d_, struct NODE** lnode);
void           determine_trick_count(int d, struct NODE** lnode);
int            evaluate_best_results(void);
int            evaluate_mp_results(void);
void           evaluate_mp_scores(void);
int            fold_ew_tree(int d, struct NODE** lnode, struct NODE** bnode);
int            fold_ns_tree(int d, COLOR c, struct NODE** lnode, struct NODE** bnode);
void           fold_quirk(int d);
void           fold_tree(int d, int p);
void           limit_matchpoint_nodes(void);
void           limit_quirk(int d);
void           merge_nodes(struct NODE* lnode, struct NODE* bnode);
void           merge_quirk(int d);
void           minimax(int nnode, int* jpiv, double* p);
void           print_all_results(char* output, char* optimumdetails);
void           print_boards(void);
void           print_node(struct NODE* lnode, char* ident);
void           print_quirk(struct QUIRKD* rootqdl, char* ident);
void           print_results(char* output, char* optimumdetails, bool addJsonOptimumDetails, bool* firstOutput, int best_MAX, int best_MP, int* best_N, double mpscore);
void           print_stat(int trace);
void           print_suitc(void);
void           print_summary_results(void);
void           print_tree(char* optimumdetails, bool addJsonOptimumDetails, int d_root, int d_high, struct NODE* rootn, int printboards, int printsiblings, struct TREE_ELEMENT* treeElements, int* pTreeElementCount);
int            process_args(int argc, char** argv);
void           select_ns_plays(int d);
int            suitc_main(int argc, char** argv, char* output, char* optimumdetails);
void           use_suit_entries(void);
struct INDEX* savenextindex;
struct NODE* savewnode0;
struct QUIRKD* savenextquirkd;
struct QUIRKH* savenextquirkh;
#define MAX_TREE_ELEMENTS                         2048
struct TREE_ELEMENT {
char content; //the value e.g. n, 8, a, 9, 4, -,...
int type; //type 0:seat, 1:card
int pos; //position in 'out' array
};
void create_game_tree_json_value(struct TREE_ELEMENT* treeElements, int treeElementsCount, char* gameTreeJsonValue);
#define JSON_BUFFER_SIZE                       32768*4
#define JSON_OUTPUTBUFFER_SAFETYMARGIN            4096
#define JSON_OPTIMUM_DETAILS_SAFETY_BLOCKS           3
int noOfOptimumDetails;
bool outputEnabled;
void output_add(char* output, const char* s);
#endif // HEADER_H_SUITC