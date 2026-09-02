//  name:
//      bump_codes
//
//  purpose:
//      bump_index  allocates a new INDEX entry
//      bump_node   allocates a new NODE entry
//      bump_quirkd allocates a new QUIRKD entry
//      bump_quirkh allocates a new QUIRKH entry
//      free_index  de-allocates a INDEX entry
//      free_node   de-allocates a NODE entry
//      free_quirkd de-allocates a QUIRKD entry
//      free_quirkh de-allocates a QUIRKH entry
//
//  parameters:
//      none
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

//  Pool exhaustion used to print a warning and carry on: `next*` had already
//  been advanced one past the end of the malloc'd block, so the following bump
//  handed out an out-of-pool pointer and wrote through it. The last entry of
//  each pool is now reserved as a sentinel, handed out repeatedly once the pool
//  is full, and `poolfull` is raised so suitc_main refuses the call instead of
//  analysing a tree built over corrupted entries.

#define bump_index( lindex ) \
    icount++; \
    if ( icountmax < icount && icountmax >= NINDICES - 1 ) { \
        icount--; \
        if ( ! poolfull ) { \
            printf( "No more memory for the INDEX entries; increase the value of NINDICES\n" ); \
            poolfull = YES; \
        } \
        lindex = savenextindex + ( NINDICES - 1 ); \
        nextindex = lindex; \
    } \
    else { \
        lindex = nextindex; \
        if ( icountmax < icount ) { \
            icountmax++; \
            nextindex++; \
        } \
        else \
            nextindex = nextindex->r; \
    } \
    lindex->r = NIL;

#define bump_node( lnode ) \
    ncount++; \
    if ( ncountmax < ncount && ncountmax >= NNODES - 1 ) { \
        ncount--; \
        if ( ! poolfull ) { \
            printf( "No more memory for the NODE entries; increase the value of NNODES\n" ); \
            poolfull = YES; \
        } \
        lnode = savewnode0 + ( NNODES - 1 ); \
        nextnode = lnode; \
    } \
    else { \
        lnode = nextnode; \
        if ( ncountmax < ncount ) { \
            ncountmax++; \
            nextnode++; \
        } \
        else \
            nextnode = nextnode->r; \
    } \
    lnode->r = NIL; \
    lnode->d = NIL;

#define bump_quirkd( lquirkd ) \
    qdcount++; \
    if ( qdcountmax < qdcount && qdcountmax >= 2 * NQUIRKS - 1 ) { \
        qdcount--; \
        if ( ! poolfull ) { \
            printf( "No more memory for the QUIRKD entries; increase the value of NQUIRKS\n" ); \
            poolfull = YES; \
        } \
        lquirkd = savenextquirkd + ( 2 * NQUIRKS - 1 ); \
        nextquirkd = lquirkd; \
    } \
    else { \
        lquirkd = nextquirkd; \
        if ( qdcountmax < qdcount ) { \
            qdcountmax++; \
            nextquirkd++; \
        } \
        else \
            nextquirkd = nextquirkd->d; \
    } \
    lquirkd->d = NIL; \
    lquirkd->h = NIL; \
    lquirkd->ic = boardcount;

//  the sentinel's `t` is allocated once in suitc_main, so neither the pool-full
//  path nor a failing TRICKS allocation needs exit() -- a library must never
//  take the host process down with it.
#define bump_quirkh( lquirkh ) \
    qhcount++; \
    if ( qhcountmax < qhcount && qhcountmax >= NQUIRKS - 1 ) { \
        qhcount--; \
        if ( ! poolfull ) { \
            printf( "No more memory for the QUIRKH entries; increase the value of NQUIRKS\n" ); \
            poolfull = YES; \
        } \
        lquirkh = savenextquirkh + ( NQUIRKS - 1 ); \
        nextquirkh = lquirkh; \
    } \
    else { \
        lquirkh = nextquirkh; \
        if ( qhcountmax < qhcount ) { \
            lquirkh->t = ( struct TRICKS *) malloc( boardcount * sizeof( struct TRICKS ) ); \
            if ( ! lquirkh->t ) { \
                if ( ! poolfull ) { \
                    printf( "The memory assignment for the TRICKS entries failed\n" ); \
                    poolfull = YES; \
                } \
                qhcount--; \
                lquirkh = savenextquirkh + ( NQUIRKS - 1 ); \
                nextquirkh = lquirkh; \
            } \
            else { \
                qhcountmax++; \
                nextquirkh++; \
            } \
        } \
        else \
            nextquirkh = nextquirkh->h; \
    } \
    memset( lquirkh->t, 0xff, boardcount * sizeof( struct TRICKS ) ); \
    lquirkh->d = NIL;

#define free_index( lindex ) \
    icount--; \
    lindex->r = nextindex; \
    nextindex = lindex;

#define free_node( lnode ) \
    ncount--; \
    lnode->r = nextnode; \
    nextnode = lnode;

#define free_quirkd( lquirkd ) \
    qdcount--; \
    lquirkd->d = nextquirkd; \
    nextquirkd = lquirkd;

#define free_quirkh( lquirkh ) \
    qhcount--; \
    lquirkh->h = nextquirkh; \
    nextquirkh = lquirkh;
