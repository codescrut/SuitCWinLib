//  name:
//      print_results
//
//  purpose:
//      print_results prints the results of the currently selected play
//
//  parameters:
//      best_MAX        in      indicator for MAX
//      best_MP         in      indicator for MATCHPOINT
//      best_N          in      indicator for N tricks
//      mpscore         in      the MP or IMP score of this play
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void optimum_output_add(char* optimumdetails, const char* str, bool add)
{
    if (!add)
    {
        return;
    }
    output_add(optimumdetails, str);
}

void output_add_optimum(char* output, bool* firstOptimum, const char* str)
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

void print_results(char* output, char* optimumdetails, bool addJsonOptimumDetails, bool* firstOutput, int best_MAX, int best_MP, int* best_N, double mpscore)
{
    int          i, n;
    double       result, perc[NCARDS + 1];
    char          str[100];

    result = 0.0;
    for (i = minns; i <= maxns; i++)
        perc[i] = 0.0;

    for (n = 0; n < boardcount; n++) {
        perc[nstricks[n]] += dupl[n] * prob[n];
        result += dupl[n] * prob[n] * nstricks[n];
    }

    if (*firstOutput)
    {
        *firstOutput = false;
    }
    else
    {
        output_add(output, "},");
    }
    output_add(output, "\n{\"OptimumPlayFor\":\n\"");

    bool firstOptimum = true;

    if (!SUITCAPP) printf("\n\n\nThe optimum play for:");
    optimum_output_add(optimumdetails, "\n\n\nThe optimum play for:", addJsonOptimumDetails);
    if (best_MAX) {
        if (ewwinners)
        {
            if (!SUITCAPP) printf(" MIN");
            optimum_output_add(optimumdetails, " MIN", addJsonOptimumDetails);
            output_add_optimum(output, &firstOptimum, "MIN");
        }
        else
        {
            if (!SUITCAPP) printf(" MAX");
            optimum_output_add(optimumdetails, " MAX", addJsonOptimumDetails);
            output_add_optimum(output, &firstOptimum, "MAX");
        }
    }
    if (matchpoint && best_MP)
    {
        if (!SUITCAPP) printf(" %s", matchpoint == IMP ? "IMP" : "MP");
        optimum_output_add(optimumdetails, " MP", addJsonOptimumDetails);
        output_add_optimum(output, &firstOptimum, "MP");
    }
    if (ewwinners) {
        for (i = maxns - 1; i >= minns; i--)
            if (best_N[i])
            {
                if (!SUITCAPP) printf(" L%d", tricklimit - i);
                sprintf(str, "L%d", tricklimit - i);
                optimum_output_add(optimumdetails, " ", addJsonOptimumDetails);
                optimum_output_add(optimumdetails, str, addJsonOptimumDetails);
                output_add_optimum(output, &firstOptimum, str);
            }

    }
    else if (defence) {
        for (i = minns + 1; i <= maxns; i++)
            if (best_N[i])
            {
                if (!SUITCAPP) printf(" S%d", i);
                sprintf(str, "S%d", i);
                optimum_output_add(optimumdetails, " ", addJsonOptimumDetails);
                optimum_output_add(optimumdetails, str, addJsonOptimumDetails);
                output_add_optimum(output, &firstOptimum, str);

            }
    }
    else {
        for (i = minns + 1; i <= maxns; i++)
            if (best_N[i])
            {
                if (!SUITCAPP) printf(" N%d", i);
                sprintf(str, "N%d", i);
                optimum_output_add(optimumdetails, " ", addJsonOptimumDetails);
                optimum_output_add(optimumdetails, str, addJsonOptimumDetails);
                output_add_optimum(output, &firstOptimum, str);
            }
    }
    if (!SUITCAPP) printf("\n=====================");
    optimum_output_add(optimumdetails, "\n=====================", addJsonOptimumDetails);
    if (best_MAX)
    {
        if (!SUITCAPP) printf("====");
        optimum_output_add(optimumdetails, "====", addJsonOptimumDetails);
    }
    if (matchpoint && best_MP)
    {
        if (!SUITCAPP) printf("===");
        optimum_output_add(optimumdetails, "===", addJsonOptimumDetails);
    }
    if (ewwinners) {
        for (i = maxns - 1; i >= minns; i--) {
            if (best_N[i]) {
                if (!SUITCAPP) printf("===");
                if (tricklimit - i >= 10)
                    printf("=");
                optimum_output_add(optimumdetails, "===", addJsonOptimumDetails);
            }
        }
    }
    else {
        for (i = minns + 1; i <= maxns; i++) {
            if (best_N[i]) 
            {
                if (!SUITCAPP) printf("===");
                if (i >= 10)
                    if (!SUITCAPP) printf("=");
                optimum_output_add(optimumdetails, "===", addJsonOptimumDetails);
            }
        }
    }
    if (!SUITCAPP) printf("\n\n");
    optimum_output_add(optimumdetails, "\n\n", addJsonOptimumDetails);
    output_add(output, "\",");


    if (ewwinners) {
        for (i = maxns; i >= minns; i--)
            if (perc[i] < 1.0 - EPS)
                printf("\t%d losers: %8.4f\n", tricklimit - i, 100.0 * perc[i]);
        printf("\tResult  : %8.4f\n", tricklimit - result + EPS);
        sprintf(str, "    Result  : %8.4f\n", result);
        optimum_output_add(optimumdetails, str, addJsonOptimumDetails);

        sprintf(str, "],\n\"Result\":%0.4f\n", result);
        output_add(output, str);
    }
    else {
        output_add(output, "\"Plays\":[");

        bool first = true;
        for (i = maxns; i >= minns; i--)
            if (perc[i] > EPS)
            {
                if (!SUITCAPP) printf("\t%d tricks: %8.4f\n", i, 100.0 * perc[i]);
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
        if (!SUITCAPP) printf("\tResult  : %8.4f\n", result);
        sprintf(str, "\tResult  : %8.4f\n", result);
        optimum_output_add(optimumdetails, str, addJsonOptimumDetails);

        sprintf(str, "],\n\"Result\":%0.4f\n", result);
        output_add(output, str);

    }

    //  print the IMP or MP score
    if (matchpoint && ((best_MAX && best_MP) == NO || printallmatchpoint))
    {
        if (!SUITCAPP) printf("\t%sscore: %8.4f\n", matchpoint == IMP ? "IMP" : "MP-", mpscore + EPS);
        sprintf(str, "    MP-score: %8.4f\n", 100.0 * mpscore + EPS);
        optimum_output_add(optimumdetails, str, addJsonOptimumDetails);
    }

}
