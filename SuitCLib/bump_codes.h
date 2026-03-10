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

#define bump_index( lindex ) \
    icount++; \
    lindex = nextindex; \
    if ( icountmax < icount ) { \
        icountmax++; \
        nextindex++; \
        if ( icountmax == NINDICES ) { \
            printf( "No more memory for the INDEX entries; increase the value of NINDICES\n" ); \
            print_stat( 'P' ); \
        } \
    } \
    else \
        nextindex = nextindex->r; \
    lindex->r = NIL;

#define bump_node( lnode ) \
    ncount++; \
    lnode = nextnode; \
    if ( ncountmax < ncount ) { \
        ncountmax++; \
        nextnode++; \
        if ( ncountmax == NNODES ) { \
            printf( "No more memory for the NODE entries; increase the value of NNODES\n" ); \
            print_stat( 'P' ); \
        } \
    } \
    else \
        nextnode = nextnode->r; \
    lnode->r = NIL; \
    lnode->d = NIL;

#define bump_quirkd( lquirkd ) \
    qdcount++; \
    lquirkd = nextquirkd; \
    if ( qdcountmax < qdcount ) { \
        qdcountmax++; \
        nextquirkd++; \
        if ( qdcountmax == 2 * NQUIRKS ) { \
            printf( "No more memory for the QUIRKH entries; increase the value of NQUIRKS\n" ); \
            print_stat( 'P' ); \
        } \
    } \
    else \
        nextquirkd = nextquirkd->d; \
    lquirkd->d = NIL; \
    lquirkd->h = NIL; \
    lquirkd->ic = boardcount;

#define bump_quirkh( lquirkh ) \
    qhcount++; \
    lquirkh = nextquirkh; \
    if ( qhcountmax < qhcount ) { \
        lquirkh->t = ( struct TRICKS *) malloc( boardcount * sizeof( struct TRICKS ) ); \
        if ( ! lquirkh->t ) { \
            printf( "The memory assignment for the TRICKS entries failed\n" ); \
            exit( 99 ); \
        } \
        qhcountmax++; \
        nextquirkh++; \
        if ( qhcountmax == NQUIRKS ) { \
            printf( "No more memory for the QUIRKH entries; increase the value of NQUIRKS\n" ); \
            print_stat( 'P' ); \
        } \
    } \
    else \
        nextquirkh = nextquirkh->h; \
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
