#pragma once
#ifndef HEADER_H_SUITC
#define HEADER_H_SUITC


#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
// Added for SuitCWinLib
#include <stdbool.h>

//      Copyright 2024, Gerrit A. van der Velde, Leiden

// 
//Windows library version created by
//Lars Kappel Hansen
//https://bridge.esmarkkappel.dk

//  list of cpp defines
//  ----------------------------------------------------------------------------
#define addcard( c )                    ( ( (COLOR) 1 ) << ( c ) )
#define BLANK                           NO
#define CUTOFF                          1000000
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
#define NINDICES                        16000000L
#define NNODE                           400
#define NNODES                          8000000L
#define NO                              0
#define NPLAYS                          ( SUITCAPP ? 4000000L : 160000000L )
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
typedef enum { renonce, one, two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace, ruff } CARD;
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
    COLOR         optcode;              //  the code setting for optimal plays
    int           dupl;                 //  the number of plays with identical results
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
                  best_MP:1,            //  the indicator that this is a MP or IMP play
                  best_N:1,             //  the indicator that this is an optimal safety play
                  ix:28,                //  the counter for the SCORE entries
                  w:1;                  //  the indicator for weakly dominated plays
    double        *mp,                  //  the value of the MP or IMP plays
                  mpscore,              //  the matchpoint score (used only for MAX plays)
                  perc,                 //  the contribution to the MP or IMP play
                  result;               //  the result in tricks of the play
};

struct STACK {
    COLOR        c;                     //  the cards that are played
    COLOR        winc;                  //  the provisional winning cards of this trick
    unsigned int ld,                    //  YES if the lead is made in this ply
                 t,                     //  the trick number
                 w;                     //  the provisional winner of this trick
};

struct TRICKS {
    unsigned short i:8,                 //  the board index
                   ns:8;                //  the number of north-south tricks
};

//  list of include files
//  ----------------------------------------------------------------------------
#include "extern.h"
#include "bump_codes.h"

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
int            create_boards( void );
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
void           print_stat( int trace );
void           print_suitc( void );
void           print_summary_results( void );
void           print_tree(char* optimumdetails, bool addJsonOptimumDetails, int d_, struct NODE* rootn, int printboards, int printsiblings, struct TREE_ELEMENT* treeElements, int* pTreeElementCount);
void           process_args( int argc, char **argv );
void           select_ew_plays( int d );
void           select_ns_plays( int d );
void           sort_plays( void );
int            suitc_main(int argc, char** argv, char* output, char* optimumdetails);

// Added for SuitCWinLib
struct INDEX* savenextindex;
struct NODE* savewnode0;
struct QUIRKD* savenextquirkd;
struct QUIRKH* savenextquirkh;
struct TRICKS* savetricks;

struct SCORE* savescore;
struct LIMIT* saveresults;
struct TRICKS* tricks;                  //  the TRICKS array


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

