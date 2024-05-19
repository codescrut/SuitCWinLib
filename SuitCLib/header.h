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
#define bump_index( lindex )            lindex = nextindex; icount++; if ( icountmax < icount ) { icountmax++; nextindex++; } else nextindex = nextindex->r
#define bump_node( lnode )              lnode = nextnode; ncount++; if ( ncountmax < ncount ) { ncountmax++; nextnode++; } else nextnode = nextnode->r; lnode->d = NIL; lnode->l = NIL; lnode->r = NIL
#define bump_quirkd( lquirkd )          lquirkd = nextquirkd; qdcount++; if ( qdcountmax < qdcount ) { qdcountmax++; nextquirkd++; check_quirk( qdcountmax, 2*NPLAYS) } else nextquirkd = nextquirkd->d; lquirkd->d = NIL; lquirkd->h = NIL; lquirkd->ic = boardcount
#define bump_quirkh( lquirkh )          lquirkh = nextquirkh; qhcount++; if ( qhcountmax < qhcount ) { lquirkh->t = tricks + (long) qhcountmax * boardcount; qhcountmax++; nextquirkh++; check_quirk( qdcountmax, 2*NPLAYS) } else nextquirkh = nextquirkh->h; lquirkh->d = NIL
#define check_quirk( i, j )             if ( i == j ) { printf( "No more memory for the QUIRK trees; run the analysis with the cutoff option activated\n" ); exit( 1 ); }
#define CUTOFF                          1000000
#define DUALHEAD                        YES
#define EPS                             1.0e-10
#define EW( p )                         ( ( p == west ) || ( p == east ) )
#define free_index( lindex )            icount--; lindex->r = nextindex; nextindex = lindex
#define free_node( lnode )              ncount--; lnode->r = nextnode; nextnode = lnode
#define free_quirkd( lquirkd )          qdcount--; lquirkd->d = nextquirkd; nextquirkd = lquirkd
#define free_quirkh( lquirkh )          qhcount--; lquirkh->h = nextquirkh; nextquirkh = lquirkh
#define low_c( c )                      ( ( c ) & -( c ) )
#define MAX( i, j )                     ( ( i ) > ( j ) ? ( i ) : ( j ) )
#define MIN( i, j )                     ( ( i ) < ( j ) ? ( i ) : ( j ) )
#define NBOARDS                         162
#define NCARDS                          13
#define NIL                             0
#define NNODE                           400
#define NO                              0
#define NPLAYS                          4000000L
#define NPLIES                          52
#define NS( p )                         ( ( p == north ) || ( p == south ) )
#define RENONCE                         ( (COLOR) 0 )
#define TICK                            1.0e-3
#define w_ew( lnode )                   ( lnode[d]->winc & ( brd[0][west] | brd[0][east] ) )
#define w_ns( lnode )                   ( lnode[d]->c & lnode[d]->winc )
#define YES                             1

//  list of typedef's and type declarations
//  ----------------------------------------------------------------------------
typedef unsigned short COLOR;
typedef enum { renonce, one, two, three, four, five, six, seven,
               eight, nine, ten, jack, queen, king, ace, ruff } CARD;
typedef enum { west, north, east, south, noplayer } SEATS;

struct INDEX {
    struct INDEX  *r;                   //  pointer to the right sibling index
    COLOR         c[4];                 //  the remaining cards of each player
    unsigned int  i:8,                  //  index in the board table
                  min:4,                //  minimum number of tricks won by NS
                  max:4;                //  maximum number of tricks won by NS
};

struct LIMIT {
    float         result;               //  the result in tricks of the play
    unsigned int  keep;                 //  indicator to keep this play
};

struct NODE {
    struct NODE   *d,                   //  pointer to the oldest son node
                  *l,                   //  pointer to the left sibling node
                  *r;                   //  pointer to the right sibling node
    struct INDEX  *i;                   //  pointer to the index list of boards
    COLOR         c,                    //  the cards played in this ply
                  winc;                 //  winning cards at the last ply of the current trick
    unsigned int  match:1,              //  indicator that a match exists for this node
                  nplays:28,            //  the number of leaves in the NODE subtree
                  p:2,                  //  the player of this node
                  term:1;               //  indicator that no more choices exist
};

struct QUIRKD {
    struct QUIRKD *d;                   //  pointer to the son QUIRKD entry
    struct QUIRKH *h;                   //  pointer to the sibling QUIRKH entry
    unsigned int  ic:8,                 //  the lowest board number for which the QUIRKD tree is checked
                  ih:8,                 //  the highest board index in this sequence
                  il:8;                 //  the lowest board index in this sequence
};

struct QUIRKH {
    union {
        struct QUIRKD *d;               //  pointer to the companion QUIRKD entry
        struct QUIRKH *h;               //  pointer to the sibling QUIRKH entry
    };
    struct TRICKS *t;                   //  pointer to the TRICKS entry
};

struct RESULT {
    double        result,               //  the result for the MAX play
                  res[NCARDS+1],        //  the result of the safety play for N tricks
                  tricks[NCARDS+1];     //  the probability to win N tricks
};

struct SCARD {
    int           m,                    //  the number of SCARD sequences including ruffs and voids
                  n;                    //  the number of SCARD sequences
    COLOR         c[10];                //  the SCARD sequences
};

struct SCORE {
    struct SCORE  *l,                   //  pointer to the next valid left SCORE entry
                  *r,                   //  pointer to the next valid right SCORE entry
                  *b;                   //  pointer to the first equivalent SCORE entry
    unsigned int  best_MAX:1,           //  the indicator that this is an optimal MAX play
                  best_MP:1,            //  the indicator that this is a MP play
                  best_N:1,             //  the indicator that this is an optimal safety play
                  ix:28,                //  the counter for the SCORE entries
                  w:1;                  //  the indicator for weakly dominated plays
    double        *mp,                  //  the value of the MP plays
                  mpscore,              //  the matchpoint score (used only for MAX plays)
                  perc,                 //  the contribution to the MP play
                  result;               //  the result in tricks of the play
};

struct TRICKS {
    unsigned short i:8,                 //  the board index
                   ns:8;                //  the number of north-south tricks
};

//  list of variables
//  ----------------------------------------------------------------------------
char   b_print[NCARDS+2][512];          //  the board_header
COLOR  board[4][8192];                  //  the holding for all possible distributions of the east-west cards
COLOR  brd[NCARDS+1][4];                //  the unplayed cards at the start of each trick
COLOR  eastfixed;                       //  the known cards in the east hand
COLOR  h2rule[NCARDS];                  //  the second hand conditions of the third hand rules
COLOR  h3rule[NCARDS];                  //  the rules for the third hand strategy
COLOR  westfixed;                       //  the known cards in the west hand
double a[NNODE][2*NNODE];               //  the matrix with MP scores
double perc[NBOARDS];                   //  the percentage of the distributions for which the percentage is defined
double prob[8192];                      //  the a priori probabilities for the boards
int    b_count[NCARDS+1];               //  the number of entries in each line
int    boardcount;                      //  the number of EW distributions
int    buildtree;                       //  the type of the NODE tree. 0=truncated tree, 1=full tree, 2=selected tree
int    compacttree;                     //  north-south win tricks as soon as possible
int    defence;                         //  lead from east of west in each trick
int    dupl[8192];                      //  the multiplicity of the boards
int    eastvac;                         //  the number of vacant places in the east hand
int    ew_c_count;                      //  the number of rows in the board header
int    icount;                          //  counter for the number of active index entries
int    icountmax;                       //  the maximum number of active index entries
int    keepewequal;                     //  flag to indicate that all equivalent EW plays are kept
int    keepewweak;                      //  flag to indicate that weakly dominated EW plays are kept
int    keepnsequal;                     //  flag to indicate that all equivalent NS plays are kept
int    keepnsweak;                      //  flag to indicate that weakly dominated NS plays are kept
int    leader;                          //  the openings leader at trick one
int    matchpoint;                      //  generate the matchpoint plays
int    maxnodes;                        //  the maximum number of nodes used for the MP evaluattion
int    maxns;                           //  the maximum number of ns_tricks
int    maxply;                          //  the maximum depth in the tree
int    mergeselect;                     //  1=merge the EW plays, 2=merge the NS plays, 4=select the NS plays
int    minns;                           //  the minimum number of ns_tricks
int    ncards[4];                       //  the maximum number of cards in each hand
int    ncardsmin[4];                    //  the minimum number of cards in each hand
int    ncount;                          //  counter for the number of active NODE entries
int    ncountmax;                       //  the maximum number of active NODE entries
int    nfree[NCARDS+1][4];              //  the number of remaining directional leads
int    nplays;                          //  number of possible plays
int    nruff[NCARDS+1][4];              //  the number of remaining ruffs
int    nrules;                          //  the number of rules for the 3d hand strategy
int    ns[NCARDS+1];                    //  the number of tricks won by north-south
int    nstricks[NBOARDS];               //  the number of tricks for north-south on each board
int    ntricks;                         //  the number of tricks to be played
int    opening_lead;                    //  the card that is lead at trick one
int    printallresults;                 //  print a full report of all results
int    printboards;                     //  print the complete board set
int    printfulltree;                   //  print the full NODE tree
int    printoption;                     //  printoption for the game trees (0=English lower, 1=Dutch lower, 2=English upper, 3=Dutch upper)\n" );
int    printsummary;                    //  print a summary of the results
int    printtrunctree;                  //  print the truncated NODE tree
int    qdcount;                         //  the number of active QUIRKD entries
int    qdcountmax;                      //  the maximum number of active QUIRKD entries
int    qhcount;                         //  the number of active QUIRKH entries
int    qhcountmax;                      //  the maximum number of active QUIRKH entries
int    reorder;                         //  flag to indicate that Jeroen Warmerdam's order of the boards is used
int    resultsonly;                     //  report only the results, no plays
int    sortplays;                       //  sort the plays in decreasing number of leaves
int    throwin;                         //  the designated player leads to the current trick if he won the last trick
int    trace;                           //  trace options: 1=timing, 2=minimax progress
int    tricklimit;                      //  limit on the number of tricks that are played
int    w[NCARDS+1];                     //  the winner of each trick
int    westvac;                         //  the number of vacant places in the west hand
int    z_count;                         //  the maximum number cards in the west hand
int    zermelo;                         //  1: delete weakly dominated plays in evaluate_mp_results, 2: delete weakly dominated plays in check_zermelo
long   cutoff;                          //  limit on the product of the number of quirks while merging
struct INDEX  *nextindex;               //  the first unused slot in the INDEX list
struct NODE   *nextnode;                //  the first unused slot in the NODE list
struct NODE   *tnode[NPLIES+1];         //  the stack of target NODE entries
struct NODE   *wnode[NPLIES+1];         //  the stack of NODE entries
struct QUIRKD *nextquirkd;              //  the first unused slot in the QUIRKD list
struct QUIRKD quirkd[NPLIES+2];         //  the QUIRKD anchors
struct QUIRKH *nextquirkh;              //  the first unused slot in the QUIRKH list
struct RESULT best_result;              //  the best result of all plays
struct SCORE  *score;                   //  the score of each play
struct SCARD  scards[NCARDS+1][4];      //  the SCARD sequences for each trick
struct TRICKS *tricks;                  //  the TRICKS array

struct {
    COLOR        c;                     //  the cards that are played
    COLOR        winc;                  //  the provisional winning cards of this trick
    unsigned int ld,                    //  YES if the lead is made in this ply
                 t,                     //  the trick number
                 w;                     //  the provisional winner of this trick
} st[NPLIES+1];                         //  the description of each ply

//  complete list of routines
//  ----------------------------------------------------------------------------
struct NODE   *add_node( int d, int p, COLOR cards );
int            add_quirkd( int i, int ns, int s, int t, int ih, struct QUIRKD **tquirkd, struct QUIRKD **tquirkdl );
void           build_selected_tree( void );
void           check_ew_plays( int d, struct NODE **lnode );
void           check_ns_plays( int d, struct NODE **lnode );
void           check_quirkd( int d, int t, struct QUIRKD **tquirkd, struct QUIRKD **tquirkdl );
int            check_zermelo( struct SCORE *lscore );
int            clean_quirkd( int d, struct LIMIT *results );
void           convert_tree_to_quirk( void );
struct NODE   *copy_node( struct NODE *lnode );
struct NODE   *copy_tree( int d_, struct NODE *rootn );
void           count_plays( void );
int            count_quirkd( int d );
void           create_all_boards( void );
void           create_board_header( void );
void           create_boards(char* output);
void           create_ply1( int d, int lead[4] );
void           create_ply2( int d, int p );
void           create_ply3( int d, int p );
void           create_ply4( int d, int p );
void           create_tricks( int d );
void           delete_board( int d, struct NODE **lnode, int b );
void           delete_quirkd( int d );
struct NODE   *delete_tree( int d_, struct NODE **lnode );
void           determine_trick_count( int d, struct NODE **lnode );
int            evaluate_best_results( void );
void           evaluate_mp_results( void );
void           evaluate_mp_scores( void );
int            fold_ew_tree( int d, struct NODE **lnode, struct NODE **bnode );
int            fold_ns_tree( int d, COLOR c, struct NODE **lnode, struct NODE **bnode );
void           fold_quirkd( int d );
void           limit_matchpoint_nodes( void );
void           limit_quirkd( int d );
void           merge_nodes( struct NODE *lnode, struct NODE *bnode );
void           merge_quirkd( int d );
void           merge_tree( int d, int p );
void           minimax( int nnode, int *jpiv, double *p );
void           print_all_results(char* output, char* optimumdetails);
void           print_boards(char* output);
void           print_node( struct NODE *lnode, char *ident );
void           print_play_count( void );
void           print_quirkd( struct QUIRKD *rootqdl, char *ident );
void           print_results(char* output, char* optimumdetails, bool addJsonOptimumDetails, bool* firstOutput, int best_MAX, int best_MP, int *best_N, double mpscore );
void           print_summary_results( void );
void           print_tree(char* optimumdetails, bool addJsonOptimumDetails, int d_, struct NODE *rootn, int printboards, int printsiblings, struct TREE_ELEMENT* treeElements, int* pTreeElementCount);
void           process_args( int argc, char **argv );
void           select_ew_plays( int d );
void           select_ns_plays( int d );
void           sort_plays( void );
int            suitc_main(int argc, char** argv, char* output, char* optimumdetails);


struct INDEX* savenextindex;
struct NODE* savewnode0;
struct QUIRKD* savenextquirkd;
struct QUIRKH* savenextquirkh;
struct TRICKS* savetricks;

struct SCORE* savescore;
struct LIMIT* saveresults;

#define MAX_TREE_ELEMENTS                         2048

struct TREE_ELEMENT {
    char content; //the value e.g. n, 8, a, 9, 4, -,... 
    int type; //type 0:seat, 1:card
    int pos; //position in 'out' array
};

void create_game_tree_json_value(struct TREE_ELEMENT* treeElements, int treeElementsCount, char* gameTreeJsonValue);

#define JSON_BUFFER_SIZE                         32768
#define JSON_OUTPUTBUFFER_SAFETYMARGIN            4096
#define JSON_OPTIMUM_DETAILS_SAFETY_BLOCKS           3

int noOfOptimumDetails;
bool outputEnabled;
void output_add(char* output, char* s);


#endif // HEADER_H_SUITC

