//  name:
//      sort_plays
//
//  purpose:
//      sort_plays sorts the nodes in increasing number of plays
//
//  parameters:
//      none
//
//  Copyright 2020, Gerrit A. van der Velde, Leiden

#include "header.h"

void sort_plays( void )
{
    int          d = 1;
    struct NODE *bnode;

    //  loop over all branches of the NODE tree
    wnode[d] = wnode[d-1]->d;
    while ( wnode[d] ) {

        //  get the oldest son
        if ( wnode[d]->d ) {
            d++;
            wnode[d] = wnode[d-1]->d;
        }

        //  we reached a leaf
        else {

            //  the bottomup loop
            while ( d && ! wnode[d]->r ) {

                //  sort the plays in ascending number of leaves
                wnode[d] = wnode[d-1]->d;
                bnode = wnode[d]->r;
                while ( wnode[d]->r ) {
                    while ( bnode ) {
                        if ( bnode->nplays > wnode[d]->nplays ) {
                            bnode->l->r = bnode->r;
                            if ( bnode->r )
                                bnode->r->l = bnode->l;
                            if ( wnode[d]->l )
                                wnode[d]->l->r = bnode;
                            else
                                wnode[d-1]->d = bnode;
                            bnode->l = wnode[d]->l;
                            bnode->r = wnode[d];
                            wnode[d]->l = bnode;
                            wnode[d] = bnode;
                            bnode = wnode[d]->r;
                            continue;
                        }
                        bnode = bnode->r;
                    }
                    wnode[d] = wnode[d]->r;
                    bnode = wnode[d]->r;
                }

                d--;
            }

            if ( ! d )
                break;

            wnode[d] = wnode[d]->r;
        }
    }
}
