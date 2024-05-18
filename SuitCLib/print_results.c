//  name:
//      print_results
//
//  purpose:
//      print_results prints the results of the siblings at play=1
//
//  parameters:
//      best_MAX        in      indicator for MAX
//      best_MP         in      indicator for MATCHPOINT
//      best_N          in      indicator for N tricks
//      mpscore         in      the matchpoint score of this play
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void optimum_output_add(char* optimumdetails, char* str, bool add)
{
    if (!add)
    {
        return;
    }
    output_add(optimumdetails, str);
}

void output_add_optimum(char* output, bool* firstOptimum, char* str)
{
    if (*firstOptimum)
    {
        *firstOptimum = false;
    }
    else
    {
        output_add(output, ", ");
    }
    output_add(output, str);
}

void print_results(char* output, char* optimumdetails, bool addJsonOptimumDetails, bool* firstOutput, int best_MAX, int best_MP, int *best_N, double mpscore )
{
    int          i, n;
    double       result, perc[NCARDS+1];
    struct SCORE *lscore;
    char          str[100];

    result = 0.0;
    for ( i = minns; i <= maxns; i++ )
        perc[i] = 0.0;

    for ( n = 0; n < boardcount; n++ ) {
        perc[nstricks[n]] += dupl[n] * prob[n];
        result += dupl[n] * prob[n] * nstricks[n];
    }

    if (*firstOutput)
    {
        *firstOutput = false;
    }
    else
    {
        output_add(output, ",");
    }
    output_add(output, "{\"OptimumPlayFor\":\"");

    bool firstOptimum = true;
    printf( "\n\n\nThe optimum play for:" );
    optimum_output_add(optimumdetails, "\n\n\nThe optimum play for:", addJsonOptimumDetails);
    if ( best_MAX ) {
        if (defence)
        {
            printf(" MIN");
            optimum_output_add(optimumdetails, " MIN", addJsonOptimumDetails);
            output_add_optimum(output, &firstOptimum, "MIN");
        }
        else
        {
            printf(" MAX");
            optimum_output_add(optimumdetails, " MAX", addJsonOptimumDetails);
            output_add_optimum(output, &firstOptimum, "MAX");
        }
    }
    if (matchpoint && best_MP)
    {
        printf(" MP");
        optimum_output_add(optimumdetails, " MP", addJsonOptimumDetails);
        output_add_optimum(output, &firstOptimum, "MP");
    }
    if ( defence ) {
        for ( i = maxns - 1; i >= minns; i-- )
            if (best_N[i])
            {
                printf(" L%d", tricklimit - i);
                sprintf(str, "L%d", tricklimit - i);
                optimum_output_add(optimumdetails, " ", addJsonOptimumDetails);
                optimum_output_add(optimumdetails, str, addJsonOptimumDetails);
                output_add_optimum(output, &firstOptimum, str);
            }
    }
    else {
        for ( i = minns + 1; i <= maxns; i++ )
            if (best_N[i])
            {
                printf(" N%d", i);
                sprintf(str, "N%d", i);
                optimum_output_add(optimumdetails, " ", addJsonOptimumDetails);
                optimum_output_add(optimumdetails, str, addJsonOptimumDetails);
                output_add_optimum(output, &firstOptimum, str);
            }
    }
    printf( "\n=====================" );
    optimum_output_add(optimumdetails, "\n=====================", addJsonOptimumDetails);
    if (best_MAX)
    {
        printf("====");
        optimum_output_add(optimumdetails, "====", addJsonOptimumDetails);
    }
    if (matchpoint && best_MP)
    {
        printf("===");
        optimum_output_add(optimumdetails, "===", addJsonOptimumDetails);
    }
    if ( defence ) {
        for ( i = maxns - 1; i >= minns; i-- )
            if (best_N[i])
            {
                printf("===");
                optimum_output_add(optimumdetails, "===", addJsonOptimumDetails);
            }
    }
    else {
        for ( i = minns + 1; i <= maxns; i++ )
            if (best_N[i])
            {
                printf("===");
                optimum_output_add(optimumdetails, "===", addJsonOptimumDetails);
            }
    }
    printf( "\n\n" );
    optimum_output_add(optimumdetails, "\n\n", addJsonOptimumDetails);
    output_add(output, "\",");


    if ( defence ) {
        for ( i = maxns; i >= minns; i-- )
            if (perc[i] < 1.0 - EPS)
            {
                printf("\t%d losers: %8.4f\n", tricklimit - i, 100.0 * perc[i]);
            }

        printf( "\tResult  : %8.4f\n", tricklimit - result + EPS );
    }
    else {
        output_add(output, "\"Plays\":[");
        
        bool first = true;
        for (i = maxns; i >= minns; i--)
            if (perc[i] > EPS)
            {
                printf("\t%d tricks: %8.4f\n", i, 100.0 * perc[i]);
                sprintf(str, "\t%d tricks: %8.4f\n", i, 100.0 * perc[i]);
                optimum_output_add(optimumdetails, str, addJsonOptimumDetails);
                if (first)
                {
                    first = false;
                }
                else
                {
                    output_add(output, ",");
                };
                sprintf(str, "{\"Tricks\":%d,\"Percentage\":%0.4f}", i, 100.0 * perc[i]);
                output_add(output, str);
            }
        printf( "\tResult  : %8.4f\n", result );
        sprintf(str, "    Result  : %8.4f\n", result);
        optimum_output_add(optimumdetails, str, addJsonOptimumDetails);

        sprintf(str, "],\"Result\":%0.4f\n", result);
        output_add(output, str);
    }

    //  print the MP score
    if (matchpoint && best_MAX && !best_MP)
    {
        printf("\tMP-score: %8.4f\n", 100.0 * mpscore + EPS);
        sprintf(str, "    MP-score: %8.4f\n", 100.0 * mpscore + EPS);
        optimum_output_add(optimumdetails, str, addJsonOptimumDetails);
    }
}
