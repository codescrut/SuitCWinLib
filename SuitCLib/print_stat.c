//  name:
//      print_stat
//
//  purpose:
//      print_stat prints the statistics of an analysis
//
//  parameters:
//      trace           in      the print option
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void print_stat( int trace )
{
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
        if ( matchpointtime == IMP )
            printf( "\tThe IMP plays are evaluated%12.3f\n", times[2] );
        if ( matchpointtime == MP )
            printf( "\tThe MP plays are evaluated %12.3f\n", times[2] );
        printf( "\tThe program is finished    %12.3f\n", times[3] );
    }

    printf( "\n\n\n\n" );
    exit( 0 );
}
