//  name:
//      create_board_header
//
//  purpose:
//      create_board_header constructs the description of the header
//      info for the boards
//
//  parameters:
//      none
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void create_board_header( void )
{
    int          i, j, cnt;
    char         out[NCARDS+1];
    COLOR        allc, c;
    static char  *symbols[] = { "-123456789TJQKA", "-123456789TBVHA" };
    struct INDEX *lindex;

    for ( i = 0; i < NCARDS; i++ )
        b_count[i] = boardcount;

    if ( DUALHEAD ) {

        i = 0;
        lindex = wnode[0]->i;
        while ( lindex ) {

            //  handle the west cards
            cnt = 0;
            allc = lindex->c[west];
            while ( allc ) {
                c = low_c( allc );
                for ( j = 0; j <= NCARDS+1; j++ ) {
                    if ( addcard(j) & c ) {
                        out[++cnt] = symbols[printoption&0x1][j];
                        break;
                    }
                }
                allc ^= c;
            }

            for ( j = 0; j < cnt; j++ )
                b_print[j][i] = out[cnt-j];
            b_print[cnt++][i] = ( ( i == boardcount-1 ) && ! lindex->c[west] ? '-' : ' ' );

            //  handle the east cards
            allc = lindex->c[east];
            while ( allc ) {
                c = low_c( allc );
                for ( j = 0; j <= NCARDS+1; j++ ) {
                    if ( addcard(j) & c ) {
                        b_print[cnt++][i] = symbols[printoption&0x1][j];
                        break;
                    }
                }
                allc ^= c;
            }
            if ( ( i == 0 ) && ! lindex->c[east] )
                b_print[cnt-1][i] = '-';
            z_count = cnt;

            i++;
            lindex = lindex->r;
        }
    }
    else {

        i = 0;
        ew_c_count = 0;
        lindex = wnode[0]->i;
        while ( lindex ) {
            cnt = 0;
            allc = lindex->c[west];
            while ( allc ) {
                c = low_c( allc );
                for ( j = NCARDS+1; j >= 0; j-- ) {
                    if ( addcard(j) & c ) {
                        out[++cnt] = symbols[printoption&0x1][j];
                        break;
                    }
                }
                allc ^= c;
            }

            b_count[cnt] = i;
            for ( j = 0; j < cnt; j++ )
                b_print[j][i] = out[cnt-j];
            ew_c_count = ( cnt > ew_c_count ? cnt : ew_c_count );

            if ( i == boardcount-1 && ! lindex->c[west] ) {
                b_count[0] += 1;
                b_print[0][boardcount-1] = '-';
            }

            i++;
            lindex = lindex->r;
        }
    }
}
