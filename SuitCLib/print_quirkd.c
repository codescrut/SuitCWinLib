//  name:
//      print_quirkd
//
//  purpose:
//      print_quirkd prints a QUIRKD tree
//
//  parameters:
//      rootqdl         in      the left sibling of root of the QUIRKD tree
//      ident           in      identification string
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void print_quirkd( struct QUIRKD *rootqdl, char *ident )
{
    int           i, l = 0, o = 0;
    char          out[2*NBOARDS+1];
    struct QUIRKD *lquirkd[NBOARDS+1];
    struct TRICKS *t;

    if ( rootqdl->h )
        printf( "\nrootqdl->h=%lx", (unsigned long) rootqdl->h );
    else {
        printf( "\nrootqdl->h=%lx ident=%s\n", (unsigned long) rootqdl->h, ident );
        return;
    }

    printf( " rootqdl->h->d=%lx", rootqdl->h->d );
    printf( " ident=%s\n", ident );
    fflush( stdout );

    t = rootqdl->h->t;
    lquirkd[l] = rootqdl->h->d;

    while ( lquirkd[l] ) {

        for ( i = lquirkd[l]->il; i <= lquirkd[l]->ih ; i++ ) {
            while ( o < 2 * t[i].i ) {
                out[o++] = ' ';
                out[o++] = '-';
            }
            out[o++] = ' ';
            sprintf( &out[o++], "%x", t[i].ns );
        }

        if ( lquirkd[l]->d ) {
            l++;
            lquirkd[l] = lquirkd[l-1]->d;
        }

        else {

            out[o] = '\0';
            printf( "%s\n", out );

            while ( l && lquirkd[l]->h == NIL )
                l--;

            if ( l == 0 && lquirkd[l]->h == NIL )
                return;

            o = 0;
            t = lquirkd[l]->h->t;
            lquirkd[l] = lquirkd[l]->h->d;
            i = lquirkd[l]->il;
            if ( BLANK ) {
                while ( o < 2 * t[i].i ) {
                    out[o++] = ' ';
                    out[o++] = ' ';
                }
            }
        }
    }
}
