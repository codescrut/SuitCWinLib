//  name:
//      print_tree
//
//  purpose:
//      print_tree prints a NODE tree
//
//  parameters:
//      d_root          in      the ply of the root of the tree
//      d_high          in      the highest ply to be printed
//      root            in      the root of the tree
//      printboards     in      flag to print the boards
//      printsiblings   in      flag to print the siblings of the root
//
//  Copyright 2024, Gerrit A. van der Velde, Leiden

#include "header.h"

void add_tree_element(struct TREE_ELEMENT* treeElements, int* pTreeElementCount, char content, int type, int pos)
{
    if (treeElements == NULL || *pTreeElementCount >= MAX_TREE_ELEMENTS)
    {
        return;
    }

    treeElements[*pTreeElementCount].content = content;
    treeElements[*pTreeElementCount].type = type;
    treeElements[*pTreeElementCount].pos = pos;

    (*pTreeElementCount)++;

}


void print_tree(char* optimumdetails, bool addJsonOptimumDetails, int d_root, int d_high, struct NODE* root, int printboards, int printsiblings, struct TREE_ELEMENT* treeElements, int* pTreeElementCount)
{
    int          b, d, i, k, l, maxply, w[NPLIES + 1], wlen;
    char         out[40000];
    COLOR        cards;
    static const char  hexdigit[] = "0123456789abcdef";
    static const char* symbols[] = { "-123456789tjqka=", "-123456789tbvha=", "-123456789TJQKA=", "-123456789TBVHA=" };
    static const char  seats[] = "WNES";
    struct NODE* lnode[NPLIES + 1];
    struct INDEX* lindex;
    struct SCARD* sc;

    char          str[40000];
    *pTreeElementCount = 0;

    //  return if the root of the tree is the NIL entry
    if (root->d == NIL)
        return;

    //  determine the length of the output strings
    maxply = 0;
    for (d = d_root; d <= NPLIES; d++)
        w[d] = 1;
    d = d_root;
    lnode[d_root] = root;
    while (lnode[d]) {

        k = 0;
        for (i = ace + 1; i >= 0; i--)
            if ((addcard(i) & lnode[d]->c) && !(addcard(i + 1) & lnode[d]->c))
                k++;
        w[d] = MAX(w[d], k);

        if (lnode[d]->d && !ply_overflow(d)) {
            d++;
            maxply = MAX(maxply, d);
            lnode[d] = lnode[d - 1]->d;
        }
        else {
            while (d > d_root && !lnode[d]->r)
                d--;
            lnode[d] = lnode[d]->r;
        }
    }

    maxply = MIN(maxply, d_high);
    wlen = 2 * (maxply - d_root + 1) + 2 * ((maxply - d_root + 1) / 4) + 2 * ((maxply - d_root) / 4) + 4;
    for (d = d_root; d <= maxply; d++)
        wlen += w[d];

    //  print the board header
    // printf("Print the board header\n");
    for (l = 0; l < wlen; l++)
        out[l] = ' ';
    if (printboards) {
        for (i = z_count - 1; i >= 0; i--) {
            l = wlen;
            for (k = 0; k < boardcount; k++) { out[l++] = ' '; out[l++] = b_print[i][k]; }
            out[l++] = '\0';
            if (!SUITCAPP) printf("%s\n", out);
            snprintf(str, sizeof(str), "%s\n", out);
            if (addJsonOptimumDetails)
            {
                output_add(optimumdetails, str);
            }
        }
    }

    //  loop over the nodes
    l = 0;
    d = d_root;
    lnode[d] = root;
    // printf("Loop the nodes\n");

    while (lnode[d]) {

        // printf("Handle the leader\n");
        //  handle the leader to the trick
        if ((d & 0x3) == 0x1) { 
            out[l++] = seats[lnode[d]->p]; 
            add_tree_element(treeElements, pTreeElementCount, out[l - 1], 0, l - 1);
            out[l++] = ':';
        }
        // printf("complete the set of the played cards\n");
        //  complete the set of the played cards
        cards = lnode[d]->c;
        sc = &scards[0][lnode[d]->p];
        for (i = 0; i <= sc->n; i++)
            if (cards & sc->c[i])
                cards |= sc->c[i];

        //  select the NS card to be played; lead highest from equals, follow lowest from equals
        if (!(foldselect & 0x2) && NS(lnode[d]->p) && !printhighcard && cards != 0x1 && cards != addcard(ruff)) {
            cards &= brd[(d - 1) / 4][lnode[d]->p];
            if ((d & 0x3) == 1)
                while (cards ^ low_c(cards))
                    cards ^= low_c(cards);
            else
                cards = low_c(cards);
            brd[(d + 3) / 4][lnode[d]->p] = brd[(d - 1) / 4][lnode[d]->p] ^ cards;
        }

        // printf("Translate\n");
        //  translate the hand into card symbols
        k = l;
        for (i = 0; i < w[d] + 2; i++) out[l++] = ' ';
        for (i = ace + 1; i >= 0; i--)
            if ((addcard(i) & cards) && !(addcard(i + 1) & cards))
            {
                out[k++] = symbols[printoption][i];
                add_tree_element(treeElements, pTreeElementCount, out[k - 1], 1, k - 1);
            }
        if ((d & 0x3) == 0x0) { out[l++] = ' '; out[l++] = ' '; }

        // printf("Sons\n");
        //  handle the sons
        if (lnode[d]->d && d < maxply) {
            d++;
            lnode[d] = lnode[d - 1]->d;
        }

        else {

            //  fill in of unmade plies
            for (b = d + 1; b <= maxply; b++) {
                if ((b & 0x3) == 0x1) { out[l++] = ' '; out[l++] = ' '; }
                out[l++] = '.';
                for (i = 0; i < w[b] + 1; i++) out[l++] = ' ';
                if ((b & 0x3) == 0x0) { out[l++] = ' '; out[l++] = ' '; }
            }

            //  print the result on the board
            out[l++] = (ewwinners ? 'e' : 'n');
            out[l++] = (ewwinners ? 'w' : 's');
            out[l++] = '=';
            k = l;
            for (i = 0; i < boardcount; i++) { out[l++] = '-'; out[l++] = ' '; }
            out[l] = '\0';
            lindex = lnode[d]->i;
            while (lindex) {
                out[k + 2 * lindex->i] = (ewwinners ? hexdigit[tricklimit - lindex->min] : hexdigit[lindex->max]);
                lindex = lindex->r;
            }
            if (!SUITCAPP) printf("%s\n", out);
            snprintf(str, sizeof(str), "%s\n", out);
            if (addJsonOptimumDetails)
            {
                output_add(optimumdetails, str);
            }

            //  get the next sibling
            while (d > d_root && !lnode[d]->r)
                d--;
            if (d == d_root && !(printsiblings && lnode[d]->r))
                break;
            lnode[d] = lnode[d]->r;

            //  fill in the leading unreported plies
            l = 0;
            for (b = d_root; b < d; b++) {
                if ((b & 0x3) == 0x1) { out[l++] = ' '; out[l++] = ' '; }
                if ((b & 0x3) == 0x0) { out[l++] = ' '; out[l++] = ' '; }
                for (k = 0; k < w[b] + 2; k++) out[l++] = ' ';
            }
        }
    }
}
