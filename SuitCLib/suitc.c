

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

void output_add(char* output, const char* s)
{
    if (!outputEnabled)
    {
        return;
    };
    size_t current_len = strlen(output);
    size_t extra_len = strlen(s);
    if (JSON_BUFFER_SIZE > (current_len + extra_len)) {
        strncat(output, s, JSON_BUFFER_SIZE);
    }

}

void output_error(char* output, const char* error)
{
    output_add(output, ",\n\"Error\":\"");
    output_add(output, error);
    output_add(output, "\"}");
}


void release_memory()
{
    int i;

    //  bump_quirkh malloc's a per-slot `t` array (boardcount * sizeof(TRICKS))
    //  the first time each pool slot is used. Indices [0, qhcountmax) own a
    //  `t`; [qhcountmax, NQUIRKS) do not. Free those before freeing the pool
    //  itself, otherwise every call leaks ~boardcount * qhcountmax * 2 bytes
    //  (measured at ~430 KB/call in the bridge service).
    if (savenextquirkh != NULL) {
        for (i = 0; i < qhcountmax; i++) {
            free(savenextquirkh[i].t);
            savenextquirkh[i].t = NIL;
        }
        free(savenextquirkh);
        savenextquirkh = NIL;
    }
    qhcountmax = 0;

    if (savenextindex != NULL) {
        free(savenextindex);
        savenextindex = NIL;
    }
    if (savewnode0 != NULL) {
        free(savewnode0);
        savewnode0 = NIL;
    }
    if (savenextquirkd != NULL) {
        free(savenextquirkd);
        savenextquirkd = NIL;
    }

    //  matchpoint score: also freed at the bottom of suitc_main on the normal
    //  path (which sets `score = NIL`), so this branch is a no-op there. It
    //  catches error returns and external suitc_free_memory() callers.
    if (score != NIL) {
        for (i = 0; i <= nplays; i++) {
            free(score[i].mp);
            score[i].mp = NIL;
        }
        free(score);
        score = NIL;
    }
}

//  Public: release all native heap held by the library and (on glibc) ask the
//  allocator to return memory to the OS. Safe to call when no call_suitc() is
//  in flight; the caller must serialize against call_suitc(), since SuitC has
//  no internal locking and shares all state via globals. Idempotent.
SUITC_API void suitc_free_memory(void)
{
    release_memory();
#if defined(__GLIBC__)
    extern int malloc_trim(size_t);
    malloc_trim(0);
#endif
}

int suitc_main( int argc, char **argv, char* output, char* optimumdetails )
{
    outputEnabled = true;
    noOfOptimumDetails = 0;


    output_add(output, "{\"SuitCAnalysis\":\n{");
    savenextindex = NULL;
    savewnode0 = NULL;
    savenextquirkd = NULL;
    savenextquirkh = NULL;

    int           i = 0, d = 0, p = 0;
    char          str[100];

    //  set defaults and initial values
    buildtree = 0;
    cutoff = SUITCAPP ? CUTOFF : 0;
    defence = NO;
    eastfixed = RENONCE;
    eastvac = NCARDS;
    ewwinners = NO;
    leader = noplayer;
    matchpoint = NO;
    matchpointtime = NO;
    maxnodes = 16000;
    ncardsmin[east] = 0;
    ncardsmin[west] = 0;
    for (p = 0; p < 4; p++)
        nfree[0][p] = NCARDS;
    for (p = 0; p < 4; p++)
        nruff[0][p] = 0;
    opening_lead = 0x7fff;
    printallmatchpoint = NO;
    printallresults = NO;
    printboards = NO;
    printfulltree = NO;
    printhighcard = NO;
    printoption = 2;
    printscard = SUITCAPP ? YES : NO;
    printsummary = NO;
    printtrunctree = NO;
    foldselect = SUITCAPP ? 0x5 : 0x7;
    nrules = 0;
    reorder = SUITCAPP ? NO : YES;
    resultsonly = NO;
    usesuitentries = NO;
    trace = 0x0;
    tricklimit = NCARDS;
    for (d = 0; d < NPLIES + 1; d++)
        quirkdl[d].ic = boardcount;
    westfixed = RENONCE;
    westvac = NCARDS;
    zermelo = 1;
    nplayedtricks = 0;
    for (p = 0; p < 4; p++)
        brd[0][p] = RENONCE;

    //  process the command line
    if (process_args(argc, argv) != 0) {
        fprintf(stderr, "Usage: program [options] Leader[filename]\n");
        output_error(output, "Usage: program [options] Leader[filename]\n");
        return -1;
    }

    //  initialize the INDEX list
    icount = 0;
    icountmax = 0;
    nextindex = (struct INDEX *) malloc(NINDICES * sizeof(struct INDEX));
    savenextindex = nextindex;
    if (!nextindex) {
        printf("The memory assignment for the INDEX entries failed\n");
        output_error(output, "The memory assignment for the INDEX entries failed");
        release_memory();
        return -1;
    }

    //  initialize the NODE tree
    ncount = 1;
    ncountmax = 1;
    wnode[0] = (struct NODE *) malloc(NNODES * sizeof(struct NODE));
    savewnode0 = wnode[0];
    if (!wnode[0]) {
        printf("The memory assignment for the NODE entries failed\n");
        output_error(output, "The memory assignment for the NODE entries failed");
        release_memory();
        return -1;
    }
    nextnode = wnode[0];
    nextnode++;

    //  create the boards
    if (create_boards(output) != 0) {
        return -1;
    }

    //  apply played tricks (filter boards, restricted choice)
    if (nplayedtricks > 0) {
        if (apply_played_tricks(output) != 0) {
            release_memory();
            return -1;
        }
    }

    //  build the tree
    if (create_tricks(1) != 0) {
        return -1;
    }

    //  print the tree
    if (printtrunctree) {
        if (!SUITCAPP) printf("\n\nThe truncated NODE tree\n");
        if (!SUITCAPP) printf("=======================\n");
        int dummy;
        print_tree(optimumdetails, false, 1, NPLIES, wnode[0]->d, YES, YES, NULL, &dummy);

        if (!SUITCAPP) printf("\n\n\n");
        fflush(stdout);
    }

    stats[0] = TICK * clock();
    //  initialise the QUIRKD list
    qdcount = 0;
    qdcountmax = 0;
    nextquirkd = (struct QUIRKD *)malloc(2 * NQUIRKS * sizeof(struct QUIRKD));
    savenextquirkd = nextquirkd;
    if (!nextquirkd) {
        printf("The memory assignment for the QUIRKD entries failed\n");
        output_error(output, "The memory assignment for the QUIRKD entries failed");
        release_memory();
        return -1;
    }

    //  initialise the QUIRKH and TRICKS lists
    qhcount = 0;
    qhcountmax = 0;
    nextquirkh = (struct QUIRKH *) malloc(NQUIRKS * sizeof(struct QUIRKH));
    savenextquirkh = nextquirkh;
    if (!nextquirkh) {
        printf("The memory assignment for the QUIRKH entries failed\n");
        output_error(output, "The memory assignment for the QUIRKH entries failed");
        release_memory();
        return -1;
    }

    //  build the quirk tree
    convert_tree_to_quirk();

    //  determine best_result, the minimum and maximum number of tricks and the number of plays
    nplays = evaluate_best_results();

    if (!SUITCAPP) {
        if (ewwinners) {
            printf("\tmin number of losers: %d\n", tricklimit - maxns);
            printf("\tmax number of losers: %d\n", tricklimit - minns);
        }
        else {
            printf("\tmin number of tricks: %d\n", minns);
            printf("\tmax number of tricks: %d\n", maxns);
        }
        printf("\tnumber of plays     : %d\n", nplays);
    }

    //  restrict the number of plays for the MP or IMP analysis
    if (matchpoint && nplays > maxnodes)
        limit_matchpoint_nodes();

    if (!SUITCAPP) {
        printf("\n\tCopyright 2025, Gerrit A. van der Velde, Leiden\n");

        printf("\n\n\nThe optimal results\n");
        printf("===================\n\n");
    }

    output_add(output, "\"OptimalResults\":[\n");
    if (ewwinners) {
        for (i = maxns - 1; i >= minns; i--) {
            if (!SUITCAPP) printf("\t%d losers: %8.4f\n", tricklimit - i, 100.0 * best_result.tricks[i]);
            sprintf(str, "{\"Tricks\":%d,\"Percentage\":%0.4f}\n", i, 100.0 * best_result.tricks[i]);
            output_add(output, str);
        }
        if (!SUITCAPP) printf("\tResult  : %8.4f\n", tricklimit - best_result.result + EPS);
        sprintf(str, "],\"Result\":%0.4f,", best_result.result);
        output_add(output, str);
    }
    else {
        int iter = 0;
        for (i = maxns; i > minns; i--)
        {
            if (!SUITCAPP) printf("\t%d tricks: %8.4f\n", i, 100.0 * best_result.tricks[i]);

            if (iter > 0)
            {
                output_add(output, ",");
            }
            sprintf(str, "{\"Tricks\":%d,\"Percentage\":%0.4f}\n", i, 100.0 * best_result.tricks[i]);
            output_add(output, str);
            iter++;

        }

        if (!SUITCAPP) printf("\tResult  : %8.4f\n", best_result.result);
        sprintf(str, "],\"Result\":%0.4f,", best_result.result);
        output_add(output, str);
    }

    // print a summary of the results
    // printf("print_summary");
    if (printsummary)
        print_summary_results();
    if (printboards)
        print_boards();

    stats[1] = TICK * clock();

    //  evaluate the MP or IMP plays
    if (matchpoint) {
        evaluate_mp_scores();
        if (evaluate_mp_results() != 0) {
            return -1;
        };
    }

    stats[2] = TICK * clock();

    if (!resultsonly) {

        //  rebuild the full game tree
        icount = boardcount;
        icountmax = boardcount;
        ncount = 1;
        ncountmax = 1;
        nextnode = wnode[0] + 1;
        buildtree = 1;
        if (create_tricks(1) != 0) {
            return -1;
        }

        //  print the tree
        if (printfulltree) {
            if (!SUITCAPP) printf("\n\n\nThe full NODE tree\n");
            if (!SUITCAPP) printf("==================\n");
            int dummy;
            print_tree(optimumdetails, false, 1, NPLIES, wnode[0]->d, YES, YES, NULL, &dummy);
            fflush(stdout);
        }
    }

    //  print the results
    buildtree = 2;
    //printf("print_all_results\n");
    print_all_results(output, optimumdetails);
    stats[3] = TICK * clock();
    //print_stat(trace);
    //printf("\n\n\n\n");

    output_error(output, "No errors");
    output_add(output, "]}}");

    //  release matchpoint allocations: `score` (and its per-leaf `mp`) are
    //  malloc'd in evaluate_mp_scores and read by evaluate_mp_results and
    //  print_all_results. Now that those have all returned, free them here
    //  rather than at the end of evaluate_mp_scores (which would be UAF).
    //  evaluate_mp_results may have already freed some `mp` slots and set
    //  them to NIL; free(NULL) is a no-op so iterating every slot is safe.
    if (matchpoint && score) {
        for (i = 0; i <= nplays; i++)
            free(score[i].mp);
        free(score);
        score = NIL;
    }

    release_memory();

    return 0;

}
