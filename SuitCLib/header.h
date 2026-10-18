#pragma once
#ifndef HEADER_H_SUITC
#define HEADER_H_SUITC

// Standard Includes (from your original file)
#include "pch.h" // Potentially review for Linux compatibility
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// DLL Export/Import Macros
#if defined(_WIN32) || defined(_WIN64)
    #ifdef SUITCLIB_EXPORTS  // Define this when building the DLL
        #define SUITC_API __declspec(dllexport)
    #else // Define this when using the DLL
        #define SUITC_API __declspec(dllimport)
    #endif
#elif __GNUC__ >= 4 // For GCC 4.0 and later
    #define SUITC_API __attribute__ ((visibility ("default")))
#else
    #define SUITC_API // For other compilers or older GCC
#endif

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
// SUITCAPP needs to be defined elsewhere if this is to be a conditional compilation path.
// For example, compile with -DSUITCAPP=1 or #define SUITCAPP 1 somewhere before this.
// If SUITCAPP is not defined, this might lead to a compile error or unexpected behavior.
// Assuming SUITCAPP will be defined, e.g., as 0 or 1.
#ifndef SUITCAPP
#define SUITCAPP 0 // Provide a default if not defined by compiler
#endif
#define NQUIRKS                         ( SUITCAPP ? 4000000L : 80000000L )
#define NPLIES                          52
#define NS( p )                         ( ( p == north ) || ( p == south ) )
#define RENONCE                         ( (COLOR) 0 )
#define TICK                            1.0e-3
// Note: w_ew and w_ns macros depend on 'd', 'brd', and 'lnode' being in scope.
// This is fine for their usage within function bodies.
// #define w_ew( lnode )                   ( lnode[d]->w_c & ( brd[0][west] | brd[0][east] ) )
// #define w_ns( lnode )                   ( lnode[d]->c & lnode[d]->w_c )
#define YES                             1

//  the walker stacks bnode[]/lnode[]/tnode[] hold NPLIES+1 entries, so MAXPLY is
//  the highest index that may legally be written. A well formed tree never gets
//  near it (13 cards x 4 hands = 52 plies), but a corrupted ->d chain walks past
//  the end of a stack array and trips the /GS cookie, which kills the process
//  outright: __fastfail cannot be caught by the host. Every descend is guarded.
#define MAXPLY                          NPLIES
#define ply_overflow( d )               ( ( d ) >= MAXPLY ? ( treeoverflow = YES ) : NO )
#define MAX_PLAYED_TRICKS               NCARDS
#define DISCARD                         ( (COLOR) 0xFFFF )

// Define JSON related constants before they might be used (though not directly in struct defs here)
#define MAX_TREE_ELEMENTS               2048
#define JSON_BUFFER_SIZE                (32768*4)
#define JSON_OUTPUTBUFFER_SAFETYMARGIN  4096
#define JSON_OPTIMUM_DETAILS_SAFETY_BLOCKS 3


//  list of typedef's and type declarations (Struct definitions must come before their use in arrays/instances)
//  ----------------------------------------------------------------------------
typedef unsigned short COLOR;
typedef enum { renonce, one, two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace, ruff } CARD;
typedef enum { west, north, east, south, noplayer } SEATS;

// Forward declaration for structs used as pointers within other structs if cyclic dependency existed
// (Not strictly needed here based on current structure, but good practice to be aware of)
struct INDEX;
struct NODE;
struct QUIRKD;
struct QUIRKH;
struct TRICKS;
struct LIMIT; // Added forward declaration as it's used in clean_quirk prototype

struct INDEX {
    struct INDEX* r;                   //  pointer to the right sibling index
    COLOR         c[4];                 //  the remaining cards of each player
    unsigned int  i : 8,                  //  index in the board table
        min : 4,                //  minimum number of tricks won by NS
        max : 4;                //  maximum number of tricks won by NS
};

struct LIMIT { // Definition of LIMIT
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

// QUIRKD and QUIRKH might have a mutual dependency via pointers, which is fine.
// Definition of TRICKS must come before its use as a pointer in QUIRKH.
struct TRICKS {
    unsigned short i : 8,                 //  the board index
        ns : 8;                //  the number of north-south tricks
};

struct QUIRKD { // Definition of QUIRKD
    struct QUIRKD* d;                   //  pointer to the son QUIRKD entry
    struct QUIRKH* h;                   //  pointer to the sibling QUIRKH entry
    unsigned int  ic : 8,                 //  the lowest board number for which the QUIRK tree is checked
        ih : 8,                 //  the highest board index in this sequence
        il : 8;                 //  the lowest board index in this sequence
};

struct QUIRKH {
    union { // Anonymous union
        struct QUIRKD* d;               //  pointer to the companion QUIRKD entry
        struct QUIRKH* h;               //  pointer to the sibling QUIRKH entry
    }; // Removed the semicolon that was here, as it's part of the union declaration
    struct TRICKS* t;                   //  pointer to the TRICKS entry
};


struct RESULT { // Definition of RESULT
    COLOR         optcode;              //  the code setting for optimal plays
    int           dupl;                 //  the number of plays with identical results
    double        result,               //  the result for the MAX play
        res[NCARDS + 1],        //  the result of the safety play for N tricks
        tricks[NCARDS + 1];     //  the probability to win N tricks
};

struct SCARD { // Definition of SCARD
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

// Definition for the anonymous struct used by 'st' - given a name: ST_PlyData
struct ST_PlyData {
    COLOR        c;                     //  the cards that are played
    COLOR        winc;                  //  the provisional winning cards of this trick
    unsigned int ld,                    //  YES if the lead is made in this ply
                 t,                     //  the trick number
                 w;                     //  the provisional winner of this trick
};

struct TREE_ELEMENT {
    char content; //the value e.g. n, 8, a, 9, 4, -,...
    int type; //type 0:seat, 1:card
    int pos; //position in 'out' array
};


//  list of variables - DECLARED as extern
//  ----------------------------------------------------------------------------
//      Copyright 2024, Gerrit A. van der Velde, Leiden

extern SUITC_API double stats[4];
extern SUITC_API char   b_print[NCARDS + 2][512];
extern SUITC_API COLOR  board[4][8192];
extern SUITC_API COLOR  brd[NCARDS + 1][4];
extern SUITC_API COLOR  eastfixed;
extern SUITC_API COLOR  h2rule[NCARDS];
extern SUITC_API COLOR  h3rule[NCARDS];
extern SUITC_API COLOR  westfixed;
extern SUITC_API double a[NNODE][2 * NNODE];
extern SUITC_API double perc[NBOARDS];
extern SUITC_API double prob[8192];
extern SUITC_API int    boardcount;
extern SUITC_API int    buildtree;
extern SUITC_API int    defence;
extern SUITC_API int    dupl[8192];
extern SUITC_API int    eastvac;
extern SUITC_API int    ew_c_count;
extern SUITC_API int    ewwinners;
extern SUITC_API int    icount;
extern SUITC_API int    icountmax;
extern SUITC_API int    leader;
extern SUITC_API int    matchpoint;
extern SUITC_API int    matchpointtime;
extern SUITC_API int    maxnodes;
extern SUITC_API int    maxns;
extern SUITC_API int    maxprint;
extern SUITC_API int    foldmode;
extern SUITC_API int    foldselect;
extern SUITC_API int    foldskip;
extern SUITC_API int    minns;
extern SUITC_API int    nbest_plays;
extern SUITC_API int    ncards[4];
extern SUITC_API int    ncardsmin[4];
extern SUITC_API int    ncount;
extern SUITC_API int    ncountmax;
extern SUITC_API int    nfree[NCARDS + 1][4];
extern SUITC_API int    nplayedtricks;
extern SUITC_API int    played_length[MAX_PLAYED_TRICKS];
extern SUITC_API int    played_leader[MAX_PLAYED_TRICKS];
extern SUITC_API COLOR  played_card[MAX_PLAYED_TRICKS][4];
extern SUITC_API int    nplays;
extern SUITC_API int    nruff[NCARDS + 1][4];
extern SUITC_API int    nrules;
extern SUITC_API int    nslimits[2][NBOARDS];
extern SUITC_API int    nstricks[NBOARDS];
extern SUITC_API int    ntricks;
extern SUITC_API int    opening_lead;
extern SUITC_API int    pointscore[NCARDS + 1];
extern SUITC_API int    printallmatchpoint;
extern SUITC_API int    printallresults;
extern SUITC_API int    printboards;
extern SUITC_API int    printfulltree;
extern SUITC_API int    printhighcard;
extern SUITC_API int    printoption;
extern SUITC_API int    printscard;
extern SUITC_API int    printsummary;
extern SUITC_API int    printtrunctree;
extern SUITC_API int    printconsole;
extern SUITC_API int    qdcount;
extern SUITC_API int    qdcountmax;
extern SUITC_API int    qhcount;
extern SUITC_API int    qhcountmax;
extern SUITC_API int    reorder;
extern SUITC_API int    resultsonly;
extern SUITC_API int    usesuitentries;
extern SUITC_API int    poolfull;
extern SUITC_API int    resultrange;
extern SUITC_API int    trace;
extern SUITC_API int    treeoverflow;
extern SUITC_API int    tricklimit;
extern SUITC_API int    westvac;
extern SUITC_API int    z_count;
extern SUITC_API int    zermelo;
extern SUITC_API long   cutoff;

// Pointers to structs
extern SUITC_API struct INDEX* nextindex;
extern SUITC_API struct NODE* nextnode;
extern SUITC_API struct QUIRKD* nextquirkd;
extern SUITC_API struct QUIRKH* nextquirkh;
extern SUITC_API struct SCORE* score;

// Arrays of struct pointers (size of pointer is known)
extern SUITC_API struct NODE* tnode[NPLIES + 1];
extern SUITC_API struct NODE* wnode[NPLIES + 1];

// Arrays of structs (struct definitions must be complete before these lines)
extern SUITC_API struct QUIRKD quirkdl[NPLIES + 1];   // QUIRKD defined above
extern SUITC_API struct RESULT best_play[32];         // RESULT defined above
extern SUITC_API struct SCARD  scards[NCARDS + 1][4]; // SCARD defined above
extern SUITC_API struct ST_PlyData st[NPLIES+1];      // ST_PlyData defined above

// Instance of a struct (struct definition must be complete)
extern SUITC_API struct RESULT best_result;           // RESULT defined above

// Other global variables
extern SUITC_API struct INDEX* savenextindex;
extern SUITC_API struct NODE* savewnode0;
extern SUITC_API struct QUIRKD* savenextquirkd;
extern SUITC_API struct QUIRKH* savenextquirkh;

extern SUITC_API int noOfOptimumDetails;
extern SUITC_API bool outputEnabled;


//  list of include files (e.g., for external libraries or other modules of your project)
//  ----------------------------------------------------------------------------
#include "bump_codes.h" // Ensure this file is compatible and follows extern rules if it defines globals


//  Macros that might use global variables or types defined above
//  Putting them here ensures all types and variables they might reference are declared.
//  Original placement of w_ew and w_ns macros:
//  These macros use 'd', 'brd', 'lnode'. 'brd' is a global variable.
//  'd' and 'lnode' are expected to be local variables or parameters where these macros are used.
#define w_ew( lnode_param ) ( (lnode_param)[d]->w_c & ( brd[0][west] | brd[0][east] ) )
#define w_ns( lnode_param ) ( (lnode_param)[d]->c & (lnode_param)[d]->w_c )
// Changed 'lnode' to 'lnode_param' in macro definition to avoid potential conflict if 'lnode' is also a global,
// though it's more likely a parameter name in functions using these. If 'd' can also be global, review its usage.


//  complete list of routines - FUNCTION PROTOTYPES
//  ----------------------------------------------------------------------------
SUITC_API struct NODE* add_node(int d_param, int p, COLOR cards); // struct NODE is defined
SUITC_API int            apply_played_tricks(char* output);
SUITC_API int            add_quirk(int i, int ns, int s, int t, int ih, struct QUIRKD** tquirkd, struct QUIRKD** tquirkdl); // struct QUIRKD is defined
SUITC_API void           build_selected_tree(void);
SUITC_API void           check_ew_plays(int d_param, struct NODE** lnode_param);
SUITC_API void           check_ns_plays(int d_param, struct NODE** lnode_param);
SUITC_API void           check_quirk(int d_param, int t, struct QUIRKD** tquirkd, struct QUIRKD** tquirkdl);
SUITC_API int            check_zermelo(struct SCORE* lscore); // struct SCORE is defined
SUITC_API int            clean_quirk(int d_param, struct LIMIT* results); // struct LIMIT is defined
SUITC_API void           convert_tree_to_quirk(void);
SUITC_API struct NODE* copy_node(struct NODE* lnode_param);
SUITC_API struct NODE* copy_tree(int d_, struct NODE* rootn);
SUITC_API int            count_quirk(int d_param);
SUITC_API int            create_all_boards(void);
SUITC_API void           create_board_header(void);
SUITC_API int            create_boards(char* output);
SUITC_API int            create_ply1(int d_param, int* lead);
SUITC_API int            create_ply2(int d_param, int p);
SUITC_API int            create_ply3(int d_param, int p);
SUITC_API int            create_ply4(int d_param, int p);
SUITC_API int            create_tricks(int d_param);
SUITC_API void           delete_board(int d_param, struct NODE** lnode_param, int b);
SUITC_API void           delete_quirk(int d_param);
SUITC_API struct NODE* delete_tree(int d_, struct NODE** lnode_param);
SUITC_API void           determine_trick_count(int d_param, struct NODE** lnode_param);
SUITC_API int            evaluate_best_results(void);
SUITC_API int            evaluate_mp_results(void);
SUITC_API void           evaluate_mp_scores(void);
SUITC_API int            fold_ew_tree(int d_param, struct NODE** lnode_param, struct NODE** bnode);
SUITC_API int            fold_ns_tree(int d_param, COLOR c, struct NODE** lnode_param, struct NODE** bnode);
SUITC_API void           fold_quirk(int d_param);
SUITC_API void           fold_tree(int d_param, int p);
SUITC_API void           limit_matchpoint_nodes(void);
SUITC_API void           limit_quirk(int d_param);
SUITC_API void           merge_nodes(struct NODE* lnode_param, struct NODE* bnode);
SUITC_API void           merge_quirk(int d_param);
SUITC_API void           minimax(int nnode_param, int* jpiv, double* p_param); // Renamed parameters to avoid conflict
SUITC_API void           print_all_results(char* output, char* optimumdetails);
SUITC_API void           print_boards(void);
SUITC_API void           print_node(struct NODE* lnode_param, char* ident);
SUITC_API void           print_quirk(struct QUIRKD* rootqdl, char* ident);
SUITC_API void           print_results(char* output, char* optimumdetails, bool addJsonOptimumDetails, bool* firstOutput, int best_MAX, int best_MP, int* best_N, double mpscore);
SUITC_API void           print_stat(int trace_param); // Renamed trace
SUITC_API void           print_suitc(void);
SUITC_API void           print_summary_results(void);
SUITC_API void           print_tree(char* optimumdetails, bool addJsonOptimumDetails, int d_root, int d_high, struct NODE* rootn, int printboards_param, int printsiblings, struct TREE_ELEMENT* treeElements, int* pTreeElementCount); // Renamed printboards
SUITC_API int            process_args(int argc, char** argv);
SUITC_API void           select_ns_plays(int d_param);
SUITC_API int            suitc_main(int argc, char** argv, char* output, char* optimumdetails);
SUITC_API void           suitc_free_memory(void);
SUITC_API void           use_suit_entries(void);

SUITC_API void create_game_tree_json_value(struct TREE_ELEMENT* treeElements, int treeElementsCount, char* gameTreeJsonValue); // struct TREE_ELEMENT defined
SUITC_API void output_add(char* output, const char* s);
SUITC_API void output_error(char* output, const char* error);

#endif // HEADER_H_SUITC