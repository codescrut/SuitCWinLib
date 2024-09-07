//  name:
//      minimax
//
//  purpose
//      minimax determines the optimal strategy for both players
//
//  parameters
//      nnode   in      the dimension of the MP or IMP matrix
//      jpiv    out     the jpiv array for the colmns
//      p       out     the probability of this play in the optimal MP or IMP strategy
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void printm( int nnode, int ip, int jp, int *jpiv, double *p, double *v, double z )
{
    int           i, j;

    printf( "\n%4d%4d             ", ip, jp );
    for ( j = 0; j < nnode; j++ )
        printf( "%9d", j );
    printf( "  " );
    for ( j = 0; j < nnode; j++ )
        printf( "%9d", j );
    printf( "\n========          p  " );
    for ( j = 0; j < nnode; j++ )
        printf( "%9d", jpiv[j] );
    printf( "  " );
    for ( j = 0; j < nnode; j++ )
        printf( "%9d", jpiv[nnode+j] );
    printf( "\n" );

    for ( i = 0; i < nnode; i++ ) {
        printf( "%8d  %9.4f  ", i, p[i]+EPS );
        for ( j = 0; j < nnode; j++ )
            printf( "%9.4f", a[i][j]+EPS );
        printf( "  " );
        for ( j = 0; j < nnode; j++ )
            printf( "%9.4f", a[i][nnode+j]+EPS );
        printf( "\n" );
    }

    printf( "     z/v  %9.4f  ", z+EPS );
    for ( j = 0; j < nnode; j++ )
        printf( "%9.4f", v[j]+EPS );
    printf( "  " );
    for ( j = 0; j < nnode; j++ )
        printf( "%9.4f", v[nnode+j]+EPS );
    printf( "\n" );
}

void minimax( int nnode, int *jpiv, double *p )
{
    int           i, ip = 0, j, jp = 0;
    double        aj, amax, v[2*NNODE], vj, z;

    //  fill in of the augmented matrix
    for ( i = 0; i < nnode; i++ )
        for ( j = 0; j < nnode; j++ )
            a[i][nnode+j] = 0.0;
    for ( j = 0; j < nnode; j++ )
        a[j][nnode+j] = 1.0;
    for ( i = 0; i < nnode; i++ )
        p[i] = 0.0;
    for ( j = 0; j < 2*nnode; j++ )
        v[j] = 0.0;
    for ( j = 0; j < nnode; j++ )
        jpiv[j] = -1;
    for ( j = 0; j < nnode; j++ )
        jpiv[nnode+j] = j;
    z = 0.0;

    if (  trace & 0x2 )
        printm( nnode, ip, jp, jpiv, p, v, z );

    //  select the pivot (=maximum) matrix element
    for ( i = 0; i < nnode; i++ ) {
        for ( j = 0; j < nnode; j++ ) {
            if ( a[i][j] > a[ip][jp]+EPS ) {
                ip = i;
                jp = j;
            }
        }
    }

    //  copy the negative of the row with the maximum to v, and subtract it from the other rows
    for ( j = 0; j < 2*nnode; j++ )
        v[j] = -a[ip][j];
    for ( i = 0; i < nnode; i++ )
        for ( j = 0; j < 2*nnode; j++ )
            a[i][j] += v[j];

    //  fill in the start probabilities p, and add them to the matrix a
    for ( i = 0; i < nnode; i++ ) {
        p[i] = -a[i][jp];
        for ( j = 0; j < nnode; j++ )
            a[i][j] += p[i];
    }

    //  adjust the ip, jp elements
    z = -v[jp];
    for ( j = 0; j < nnode; j++ ) {
        a[ip][j] = 1.0;
        v[j] += z;
    }
    a[ip][nnode+ip] = 0.0;
    p[ip] = 1.0;
    v[nnode+ip] = -1.0;
    jpiv[jp] = ip;
    jpiv[nnode+ip] = -1;

    //  perform a minimalisation step
    while ( YES ) {

        if (  trace & 0x2 )
            printm( nnode, ip, jp, jpiv, p, v, z );

        //  locate the maximum in the v-array
        jp = 0;
        for ( j = 0; j < 2*nnode; j++ )
            jp = v[j] > v[jp]+EPS ? j : jp;

        //  finished if no more positive elements
        if ( v[jp] < EPS )
            break;

        //  find the jpiv row
        ip = 0;
        amax = 1.0e30;
        for ( i = 0; i < nnode; i++ ) {
            if ( a[i][jp] > EPS ) {
                if ( ( p[i] / a[i][jp] ) < amax-EPS ) {
                    ip = i;
                    amax = p[ip] / a[ip][jp];
                }
            }
        }

        //  reset the jpiv elements
        for ( j = 0; j < 2*nnode; j++ )
            if ( jpiv[j] == ip )
                jpiv[j] = -1;
        jpiv[jp] = ip;

        //  normalize the jpiv row
        aj = a[ip][jp];
        p[ip] /= aj;
        for ( j = 0; j < 2*nnode; j++ )
            a[ip][j] /= aj;

        //  adjust the result vector and the result
        vj = v[jp];
        z -= vj * p[ip];
        for ( j = 0; j < 2*nnode; j++ )
            v[j] -= vj * a[ip][j];

        //  adjust the matrix a
        for ( i = 0; i < nnode; i++ ) {
            if ( i != ip ) {
                aj = a[i][jp];
                p[i] -= aj * p[ip];
                for ( j = 0; j < 2*nnode; j++ )
                    a[i][j] -= aj * a[ip][j];
            }
        }
    }
}
