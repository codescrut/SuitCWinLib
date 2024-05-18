

//  name:
//      suitc
//
//  purpose:
//      suitc is the main program for the analysis of suit combinations
//
//  to produce a list of options type:
//      SuitC {?|-?|-h}
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

int noOfOptimumDetails;
bool outputEnabled;

void output_add(char* output, char* s)
{
    if (!outputEnabled)
    {
        return;
    };
    strncat(output, s, JSON_BUFFER_SIZE);
}

void output_error(char* output, char* error)
{
    output_add(output, "\"Error\":\"");
    output_add(output, error);
    output_add(output, "\"}}");
}


void release_memory()
{
    if (savenextindex != NULL) free(savenextindex);
    if (savewnode0 != NULL) free(savewnode0);
    if (savenextquirkd != NULL) free(savenextquirkd);
    if (savenextquirkh != NULL) free(savenextquirkh);
    if (savetricks != NULL) free(savetricks);
    
    if (savescore != NULL) free(savescore);
    if (saveresults != NULL) free(saveresults);

}

int suitc_main( int argc, char **argv, char* output, char* optimumdetails )
{
    outputEnabled = true;
    noOfOptimumDetails = 0;


    output_add(output, "{\"SuitCAnalysis\":{");
    //output_error(output, "Test without analysing");
    //return -1;

    savenextindex = NULL;
    savewnode0 = NULL;
    savenextquirkd = NULL;
    savenextquirkh = NULL;
    savetricks = NULL;

    savescore = NULL;
    saveresults = NULL;

    int           i = 0, d = 0, p = 0;
    double        times[4];
    char          str[100];

    //  set defaults and initial values
    buildtree = 0;
    compacttree = YES;
    cutoff = 0;
    defence = NO;
    eastfixed = RENONCE;
    eastvac = NCARDS;
    keepewequal = YES;
    keepewweak = YES;
    keepnsequal = YES;
    keepnsweak = YES;
    leader = noplayer;
    matchpoint = NO;
    maxnodes = 16000;
    ncardsmin[east] = 0;
    ncardsmin[west] = 0;
    for ( p = 0; p < 4; p++ )
        nfree[0][p] = NCARDS;
    for ( p = 0; p < 4; p++ )
        nruff[0][p] = 0;
    opening_lead = 0x7fff;
    printallresults = NO;
    printboards = NO;
    printfulltree = NO;
    printoption = 2;
    printsummary = NO;
    printtrunctree = NO;
    mergeselect = 0x1 | 0x4;
    nrules = 0;
    reorder = NO;
    resultsonly = NO;
    sortplays = NO;
    for ( d = 0; d <= NPLIES; d++ ) {
        st[d].ld = ( d & 3 ) == 1;
        st[d].t = ( d + 3 ) / 4;
    }
    throwin = noplayer;
    trace = 0x0;
    tricklimit = NCARDS;
    for ( d = 0; d < NPLIES+2; d++ )
        quirkd[d].ic = boardcount;
    w[0] = noplayer;
    westfixed = RENONCE;
    westvac = NCARDS;
    zermelo = 1;

    //  process the command line
    process_args( argc, argv );

    //  initialize the INDEX list
    icount = 0;
    icountmax = 0;
    nextindex = malloc( 12000000 * sizeof( struct INDEX ) );
    savenextindex = nextindex;
    if ( ! nextindex ) {
        output_error(output, "The memory assignment for the INDEX entries failed");
        release_memory();
        return -1;
    }
    //  initialize the NODE tree
    ncount = 1;
    ncountmax = 1;
    wnode[0] = malloc( 8000000 * sizeof( struct NODE ) );
    savewnode0 = wnode[0];
    if ( ! wnode[0] ) {
        output_error(output, "The memory assignment for the NODE entries failed");
        release_memory();
        return -1;
    }
    nextnode = wnode[0];
    nextnode++;

    //  create the boards
    create_boards(output);
    //  build the tree
    ns[0] = defence ? tricklimit : 0;
    create_tricks( 1 );

    //  print the tree
    if ( printtrunctree ) {
        printf( "\n\nThe truncated NODE tree\n" );
        printf(   "=======================\n" );
        int dummy;
        print_tree(optimumdetails, false, 0, wnode[0], YES, YES, NULL, &dummy );
        printf( "\n\n\n" );
        fflush( stdout );
    }

    times[0] = TICK * clock();

    //  initialise the QUIRKD list
    qdcount = 0;
    qdcountmax = 0;
    nextquirkd = malloc( 2 * NPLAYS * sizeof( struct QUIRKD ) );
    savenextquirkd = nextquirkd;
    if ( ! nextquirkd ) {
        output_error(output, "The memory assignment for the QUIRKD entries failed");
        release_memory();
        return -1;
    }

    //  initialise the QUIRKH and TRICKS lists
    qhcount = 0;
    qhcountmax = 0;
    nextquirkh = malloc( NPLAYS * sizeof( struct QUIRKH ) );
    savenextquirkh = nextquirkh;
    if ( ! nextquirkh ) {
        output_error(output, "The memory assignment for the TRICKS entries failed");
        release_memory();
        return -1;
    }
    tricks = malloc( (long) boardcount * NPLAYS * sizeof( struct TRICKS ) );
    savetricks = tricks;
    if ( ! tricks ) {
        output_error(output, "The memory assignment for the TRICKS entries failed");
        release_memory();
        return -1;
    }

    //  build the quirk tree
    if ( sortplays ) {
        count_plays();
        sort_plays();
    }
    convert_tree_to_quirk();

    //  determine best_result, the minimum and maximum number of tricks and the number of plays
    nplays = evaluate_best_results();

    //output_error(output, "Exit suitc_main after evaluate_best_results");
    //return 0;

    if (trace & 0x1) {
        if (defence) {
            printf("\tmin number of losers: %d\n", tricklimit - maxns);
            printf("\tmax number of losers: %d\n", tricklimit - minns);
        }
        else {
            printf("\tmin number of tricks: %d\n", minns);
            printf("\tmax number of tricks: %d\n", maxns);

            sprintf(str, "\"MinTricks\":%d,", minns);
            output_add(output, str);

            sprintf(str, "\"MaxTricks\":%d,", maxns);
            output_add(output, str);
        }
        printf("\tnumber of plays     : %d\n", nplays);

        sprintf(str, "\"Plays\":%d,", nplays);
        output_add(output, str);
    }

    //  restrict the number of plays for the matchpoint analysis
    if ( matchpoint && nplays > maxnodes )
        limit_matchpoint_nodes();

    if (trace & 0x1)
        printf( "\n\tCopyright 2020, Gerrit A. van der Velde, Leiden\n" );

    if ( sortplays )
        print_play_count();

    printf( "\n\n\nThe optimal results\n" );
    printf(   "===================\n\n" );

    output_add(output, "\"OptimalResults\":[");

    if ( defence ) {
        for ( i = maxns - 1; i >= minns; i-- )
            printf( "\t%d losers: %8.4f\n", tricklimit - i, 100.0 * best_result.tricks[i] );
        printf( "\tResult  : %8.4f\n", tricklimit - best_result.result + EPS );
    }
    else {
        int iter = 0;
        for (i = maxns; i > minns; i--)
        {
            printf("\t%d tricks: %8.4f\n", i, 100.0 * best_result.tricks[i]);

            if (iter > 0)
            {
                output_add(output, ",");
            }
            sprintf(str, "{\"Tricks\":%d,\"Percentage\":%0.4f}", i, 100.0 * best_result.tricks[i]);
            output_add(output, str);
            iter++;

        }

        printf( "\tResult  : %8.4f\n", best_result.result );
        
        sprintf(str, "],\"Result\":%0.4f,", best_result.result);
        output_add(output, str);
    }
    
    //  print a summary of the results
    if ( printsummary )
        print_summary_results();

    times[1] = TICK * clock();

    //  evaluate the matchpoint plays
    if ( matchpoint ) {
        evaluate_mp_scores();
        evaluate_mp_results();
    }

    times[2] = TICK * clock();

    if ( ! resultsonly ) {

        //  rebuild the full game tree
        icount = boardcount;
        icountmax = boardcount;
        ncount = 1;
        ncountmax = 1;
        nextnode = wnode[0] + 1;
        buildtree = 1;
        create_tricks( 1 );

        //  print the tree
        if ( printfulltree ) {
            printf( "\n\n\nThe full NODE tree\n" );
            printf(   "==================\n" );
            int dummy;
            print_tree(optimumdetails, false, 0, wnode[0], YES, YES, NULL, &dummy );
            fflush( stdout );
        }
    }

    //  print the results
    buildtree = 2;
    print_all_results(output, optimumdetails);

    times[3] = TICK * clock();

    if ( trace & 0x1 ) {
        printf( "\n\n\n\tCounter        Maxvalue    Memory\n\t-------        --------    ------\n" );
        printf( "\tindexcount%13d%10.1f\n", icountmax, icountmax * sizeof( struct INDEX ) / 1048576. );
        printf( "\tnodecount%14d%10.1f\n", ncountmax, ncountmax * sizeof( struct NODE ) / 1048576. );
        printf( "\tquirkdcount%12d%10.1f\n", qdcountmax, qdcountmax * sizeof( struct QUIRKD ) / 1048576. );
        printf( "\tquirkhcount%12d%10.1f\n", qhcountmax, qhcountmax * sizeof( struct QUIRKH ) / 1048576. );
        printf( "\ttrickscount%12d%10.1f\n", qhcountmax, (long) qhcountmax * boardcount * sizeof( struct TRICKS ) / 1048576. );
        printf( "\n\tEvent                              Time\n" );
        printf( "\t--------------------------         ----\n" );
        printf( "\tThe tree is created        %12.3f\n", times[0] );
        printf( "\tThe quirk tree is build    %12.3f\n", times[1] );
        printf( "\tThe MP plays are evaluated %12.3f\n", times[2] );
        printf( "\tThe program is finished    %12.3f\n", times[3] );
    }

    printf( "\n\n\n\n" );

    //output_add(output, "}}");
    output_error(output, "No errors");

    release_memory();

    return 0;
    //exit( 0 );
}
