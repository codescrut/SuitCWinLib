//  name:
//      main
//
//  purpose:
//      Standalone executable wrapper for suitc_main.
//      Passes command-line arguments through and prints the JSON output.
//
//  usage:
//      SuitC [options] North South WestEast
//      SuitC -wv9 -ev8 K3 2 AQJT987654

#include "header.h"

int main(int argc, char **argv)
{
    char output[JSON_BUFFER_SIZE];
    char optimumdetails[JSON_BUFFER_SIZE];

    output[0] = '\0';
    optimumdetails[0] = '\0';

    int rc = suitc_main(argc, argv, output, optimumdetails);

    if (output[0])
        printf("%s\n", output);
    if (optimumdetails[0])
        printf("%s\n", optimumdetails);

    return rc;
}
