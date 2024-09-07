//  name:
//      print_node
//
//  purpose:
//      print_node prints a NODE entry
//
//  parameters:
//      lnode           in      the node to be printed
//      ident           in      an identification string
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void print_node( struct NODE *lnode, char *ident )
{
    int          j, k, p;
    char         out[14];
    static char  *symbols[] = { "-123456789TJQKA", "-123456789TBVHA", "-123456789tjqka", "-123456789tbvha" };
    struct INDEX *lindex;

    if ( lnode )
        printf( "\nnode=%lx", (unsigned long) lnode );
    else {
        printf( "\nnode=%lx ident=%s\n", (unsigned long) lnode, ident );
        return;
    }
    printf( " l=%lx", (unsigned long) lnode->l );
    printf( " r=%lx", (unsigned long) lnode->r );
    printf( " d=%lx", (unsigned long) lnode->d );
    printf( " ident=%s\n", ident );
    printf( "p=%d", lnode->p );
    printf( " cards=%04x", lnode->c >> 2 );
    printf( " winc=%04x", lnode->winc >> 2 );
    printf( " match=%d", lnode->match );
    printf( " term=%d\n", lnode->term );

    lindex = lnode->i;
    while ( lindex ) {
        for ( p = 0; p < 4; p++ ) {
            k = 0;
            out[0] = '-';
            out[1] = '\0';
            for ( j = NCARDS+1; j >= 0; j-- ) {
                if ( addcard(j) & lindex->c[p] ) {
                    out[k] = symbols[2][j];
                    out[++k] = '\0';
                }
            }
            printf( "%-14s", out );
        }
        printf( "   min=%d", ( defence ? tricklimit - lindex->max : lindex->min ) );
        printf( "   max=%d\n", ( defence ? tricklimit - lindex->min : lindex->max ) );
        lindex = lindex->r;
    }
}
