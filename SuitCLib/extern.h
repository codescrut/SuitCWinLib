//      Copyright 2024, Gerrit A. van der Velde, Leiden

extern char   b_print[NCARDS+2][512];          //  the board_header
extern COLOR  board[4][8192];                  //  the holding for all possible distributions of the east-west cards
extern COLOR  brd[NCARDS+1][4];                //  the unplayed cards at the start of each trick
extern COLOR  eastfixed;                       //  the known cards in the east hand
extern COLOR  h2rule[NCARDS];                  //  the second hand conditions of the third hand rules
extern COLOR  h3rule[NCARDS];                  //  the rules for the third hand strategy
extern COLOR  westfixed;                       //  the known cards in the west hand
extern double a[NNODE][2*NNODE];               //  the matrix with MP or IMP scores
extern double perc[NBOARDS];                   //  the percentage of the distributions for which the percentage is defined
extern double prob[8192];                      //  the a priori probabilities for the boards
extern double times[4];                        //  array with timings
extern int    b_count[NCARDS+1];               //  the number of entries in each line
extern int    boardcount;                      //  the number of EW distributions
extern int    buildtree;                       //  the type of the NODE tree. 0=truncated tree, 1=full tree, 2=selected tree
extern int    compacttree;                     //  north-south win tricks as soon as possible
extern int    defence;                         //  lead from east of west in each trick
extern int    dupl[8192];                      //  the multiplicity of the boards
extern int    eastvac;                         //  the number of vacant places in the east hand
extern int    ew_c_count;                      //  the number of rows in the board header
extern int    icount;                          //  counter for the number of active index entries
extern int    icountmax;                       //  the maximum number of active index entries
extern int    keepewequal;                     //  flag to indicate that all equivalent EW plays are kept
extern int    keepewweak;                      //  flag to indicate that weakly dominated EW plays are kept
extern int    keepnsequal;                     //  flag to indicate that all equivalent NS plays are kept
extern int    keepnsweak;                      //  flag to indicate that weakly dominated NS plays are kept
extern int    leader;                          //  the openings leader at trick one
extern int    matchpoint;                      //  generate the matchpoint plays
extern int    matchpointtime;                  //  print the matchpoint analysis time
extern int    maxnodes;                        //  the maximum number of nodes used for the MP or IMP evaluation
extern int    maxns;                           //  the maximum number of ns_tricks
extern int    maxply;                          //  the maximum depth in the tree
extern int    mergeselect;                     //  1=merge the EW plays, 2=merge the NS plays, 4=select the NS plays
extern int    minns;                           //  the minimum number of ns_tricks
extern int    nbest_plays;                     //  the number of elements in array best_play
extern int    ncards[4];                       //  the maximum number of cards in each hand
extern int    ncardsmin[4];                    //  the minimum number of cards in each hand
extern int    ncount;                          //  counter for the number of active NODE entries
extern int    ncountmax;                       //  the maximum number of active NODE entries
extern int    nfree[NCARDS+1][4];              //  the number of remaining directional leads
extern int    nplays;                          //  number of possible plays
extern int    nruff[NCARDS+1][4];              //  the number of remaining ruffs
extern int    nrules;                          //  the number of rules for the 3d hand strategy
extern int    ns[NCARDS+1];                    //  the number of tricks won by north-south
extern int    nslimits[2][NBOARDS];            //  the lower and upper bound for the number of north-south tricks
extern int    nstricks[NBOARDS];               //  the number of tricks for north-south on each board
extern int    ntricks;                         //  the number of tricks to be played
extern int    opening_lead;                    //  the card that is lead at trick one
extern int    pointscore[NCARDS+1];            //  the pointscore for each number of tricks
extern int    printallmatchpoint;              //  print the MP or IMP results for all plays
extern int    printallresults;                 //  print a full report of all results
extern int    printboards;                     //  print the complete board set
extern int    printfulltree;                   //  print the full NODE tree
extern int    printhighcard;                   //  print the the highest card of a north-south card sequence in the game tree
extern int    printoption;                     //  printoption for the game trees (0=English lower, 1=Dutch lower, 2=English upper, 3=Dutch upper)\n" );
extern int    printscard;                      //  print different card symbols for each card in a card sequence in the board header
extern int    printsummary;                    //  print a summary of the results
extern int    printtrunctree;                  //  print the truncated NODE tree
extern int    qdcount;                         //  the number of active QUIRKD entries
extern int    qdcountmax;                      //  the maximum number of active QUIRKD entries
extern int    qhcount;                         //  the number of active QUIRKH entries
extern int    qhcountmax;                      //  the maximum number of active QUIRKH entries
extern int    reorder;                         //  flag to indicate that Jeroen Warmerdam's order of the boards is used
extern int    resultsonly;                     //  report only the results, no plays
extern int    sortplays;                       //  sort the plays in decreasing number of leaves
extern int    throwin;                         //  the designated player leads to the current trick if he won the last trick
extern int    trace;                           //  trace options: 1=timing, 2=minimax progress
extern int    tricklimit;                      //  limit on the number of tricks that are played
extern int    w[NCARDS+1];                     //  the winner of each trick
extern int    westvac;                         //  the number of vacant places in the west hand
extern int    z_count;                         //  the maximum number cards in the west hand
extern int    zermelo;                         //  1: delete weakly dominated plays in evaluate_mp_results, 2: delete weakly dominated plays in check_zermelo
extern long   cutoff;                          //  limit on the product of the number of quirks while merging
extern struct INDEX  *nextindex;               //  the first unused slot in the INDEX list
extern struct NODE   *nextnode;                //  the first unused slot in the NODE list
extern struct NODE   *tnode[NPLIES+1];         //  the stack of target NODE entries
extern struct NODE   *wnode[NPLIES+1];         //  the stack of NODE entries
extern struct QUIRKD *nextquirkd;              //  the first unused slot in the QUIRKD list
extern struct QUIRKD quirkd[NPLIES+2];         //  the QUIRKD anchors
extern struct QUIRKH *nextquirkh;              //  the first unused slot in the QUIRKH list
extern struct RESULT best_play[32];            //  the plays that give the best results for MAX and N<n>
extern struct RESULT best_result;              //  the best result of all plays
extern struct SCARD  scards[NCARDS+1][4];      //  the SCARD sequences for each trick
extern struct SCORE  *score;                   //  the score of each play
extern struct STACK  st[NPLIES+1];             //  the description of each ply
