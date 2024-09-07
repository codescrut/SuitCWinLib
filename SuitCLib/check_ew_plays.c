//  name:
//      check_ew_plays
//
//  purpose:
//      check_ew_plays checks a node against its siblings. it loops over the
//      siblings and deletes all boards that constitute an inferior play
//
//  parameters:
//      d               in      the depth in the NODE tree
//      lnode           in      the NODE tree
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void check_ew_plays( int d, struct NODE **lnode )
{
    int          b, bstrong, bweak, lstrong, lweak, recheck;
    struct INDEX *aindex, *bindex, *lindex, anchorb, anchorl;
    struct NODE  *bnode[NPLIES+1];

    bnode[d-1] = lnode[d-1];

recheck_all_nodes:
    recheck = NO;
    lnode[d] = lnode[d-1]->d;

next_lnode:
    while ( lnode[d] ) {
        bnode[d] = lnode[d]->r;

    next_bnode:
        while ( bnode[d] ) {

            //  initialise
            anchorl.r = lnode[d]->i;
            anchorb.r = bnode[d]->i;
            lindex = &anchorl;
            bindex = &anchorb;

            //  check the number of tricks on each board
            while( lindex->r && bindex->r ) {

                while( lindex->r && lindex->r->i < bindex->r->i )
                    lindex = lindex->r;
                if ( ! lindex->r )
                    break;

                while( bindex->r && bindex->r->i < lindex->r->i )
                    bindex = bindex->r;
                if ( ! bindex->r )
                    break;

                //  handle a board that occurs in both nodes
                if ( lindex->r && bindex->r && lindex->r->i == bindex->r->i ) {

                    //  initialise
                    b = lindex->r->i;
                    lstrong = lindex->r->max < bindex->r->min;
                    bstrong = bindex->r->max < lindex->r->min;
                    lweak = lindex->r->max <= bindex->r->min;
                    bweak = bindex->r->max <= lindex->r->min;

                    //  select the plays
                    if ( buildtree == 2 && bindex->r->max > nstricks[b] )
                        goto delete_bboard;

                    else if ( buildtree == 2 && lindex->r->max > nstricks[b] )
                        goto delete_lboard;

                    else if ( lstrong )
                        goto delete_bboard;

                    else if ( bstrong )
                        goto delete_lboard;

                    else if ( bweak && lweak && keepewequal && buildtree ) {
                        lindex = lindex->r;
                        continue;
                    }

                    else if ( bweak && lweak )
                        goto delete_lboard;

                    else if ( ( lweak || bweak ) && keepewweak && buildtree ) {
                        lindex = lindex->r;
                        continue;
                    }

                    else if ( lweak )
                        goto delete_bboard;

                    else if ( bweak )
                        goto delete_lboard;

                    else if ( YES ) {
                        lindex = lindex->r;
                        continue;
                    }

                    //  delete board
                delete_lboard:
                    recheck = YES;
                    aindex = lindex->r;
                    lindex->r = lindex->r->r;
                    lnode[d]->i = anchorl.r;
                    lindex = lindex->r;
                    free_index( aindex );

                    //  handle the sons if any boards left
                    if ( lnode[d]->i ) {
                        delete_board( d+1, lnode, b );
                        determine_trick_count( d+1, lnode );
                        break;
                    }
                    else {
                        lnode[d] = delete_tree( d, lnode );
                        goto next_lnode;
                    }

                    //  delete board
                delete_bboard:
                    recheck = YES;
                    aindex = bindex->r;
                    bindex->r = bindex->r->r;
                    bnode[d]->i = anchorb.r;
                    free_index( aindex );

                    //  handle the sons if any boards left
                    if ( bnode[d]->i ) {
                        delete_board( d+1, bnode, b );
                        determine_trick_count( d+1, bnode );
                        break;
                    }
                    else {
                        bnode[d] = delete_tree( d, bnode );
                        goto next_bnode;
                    }
                }
            }

            bnode[d] = bnode[d]->r;
        }

        lnode[d] = lnode[d]->r;
    }

    if ( recheck )
        goto recheck_all_nodes;

    //  determine the number of NS tricks of the parent node
    determine_trick_count( d, lnode );
}
