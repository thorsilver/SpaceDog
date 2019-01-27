//
// Created by Eric Silverman on 13/10/2018.
//

#include <stdio.h>
#include "defs.h"

const int PawnIsolated = -10;
const int PawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 };
const int RookOpenFile = 10;
const int RookSemiOpenFile = 5;
const int QueenOpenFile = 5;
const int QueenSemiOpenFile = 3;
const int BishopPair = 30;

const int PawnTable[64] = {
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
        10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
        5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
        0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
        5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
        10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
        20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

const int KnightTable[64] = {
        0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
        0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
        0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
        0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
        5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
        5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

const int BishopTable[64] = {
        0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
        0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
        0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
        0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
        0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
        0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
        0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

const int RookTable[64] = {
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0
};

const int KingE[64] = {
        -50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
        -10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
        0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
        0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
        0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
        0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
        -10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
        -50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50
};

const int KingO[64] = {
        0	,	5	,	5	,	-10	,	-10	,	0	,	10	,	5	,
        -30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
        -50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70
};

const int pval_bl[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
                          -5,  0,  0,  0,  0,  0,  0, -5,
                          -5,  0,  0,  0,  0,  0,  0, -5,
                          -5,  0,  3,  5,  5,  3,  0, -5,  /* [mg][black][sq] */
                          -5,  0,  5, 10, 10,  5,  0, -5,
                          -5,  0,  3,  5,  5,  3,  0, -5,
                          -5,  0,  0,  0,  0,  0,  0, -5,
                          0,  0,  0,  0,  0,  0,  0,  0 };

const int pval_wh[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
                          -5,  0,  0,  0,  0,  0,  0, -5,
                          -5,  0,  3,  5,  5,  3,  0, -5,
                          -5,  0,  5, 10, 10,  5,  0, -5,  /* [mg][white][sq] */
                          -5,  0,  3,  5,  5,  3,  0, -5,
                          -5,  0,  0,  0,  0,  0,  0, -5,
                          -5,  0,  0,  0,  0,  0,  0, -5,
                          0,  0,  0,  0,  0,  0,  0,  0 };

/* Knight PSTs */

const int nval_mg_bl[64] = { -41, -29, -27, -15, -15, -27, -29, -41,
                             -9,   4,  14,  20,  20,  14,   4,  -9,
                             -7,  10,  23,  29,  29,  23,  10,  -7,
                             -5,  12,  25,  32,  32,  25,  12,  -5,  /* [mg][black][sq] */
                             -5,  10,  23,  28,  28,  23,  10,  -5,
                             -7,  -2,  19,  19,  19,  19,  -2,  -7,
                             -9,  -6,  -2,   0,   0,  -2,  -6,  -9,
                             -31, -29, -27, -25, -25, -27, -29, -31 };

const int nval_mg_wh[64] = { -31, -29, -27, -25, -25, -27, -29, -31,
                             -9,  -6,  -2,   0,   0,  -2,  -6,  -9,
                             -7,  -2,  19,  19,  19,  19,  -2,  -7,
                             -5,  10,  23,  28,  28,  23,  10,  -5,  /* [mg][white][sq] */
                             -5,  12,  25,  32,  32,  25,  12,  -5,
                             -7,  10,  23,  29,  29,  23,  10,  -7,
                             -9,   4,  14,  20,  20,  14,   4,  -9,
                             -41, -29, -27, -15, -15, -27, -29, -41 };

const int nval_eg_bl[64] = { -41, -29, -27, -15, -15, -27, -29, -41,
                             -9,   4,  14,  20,  20,  14,   4,  -9,
                             -7,  10,  23,  29,  29,  23,  10,  -7,
                             -5,  12,  25,  32,  32,  25,  12,  -5,  /* [eg][black][sq] */
                             -5,  10,  23,  28,  28,  23,  10,  -5,
                             -7,  -2,  19,  19,  19,  19,  -2,  -7,
                             -9,  -6,  -2,   0,   0,  -2,  -6,  -9,
                             -31, -29, -27, -25, -25, -27, -29, -31 };

const int nval_eg_wh[64] = { -31, -29, -27, -25, -25, -27, -29, -31,
                             -9,  -6,  -2,   0,   0,  -2,  -6,  -9,
                             -7,  -2,  19,  19,  19,  19,  -2,  -7,
                             -5,  10,  23,  28,  28,  23,  10,  -5,  /* [eg][white][sq] */
                             -5,  12,  25,  32,  32,  25,  12,  -5,
                             -7,  10,  23,  29,  29,  23,  10,  -7,
                             -9,   4,  14,  20,  20,  14,   4,  -9,
                             -41, -29, -27, -15, -15, -27, -29, -41 };

/* Knight Outpots */

const int knight_outpost_bl[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 1, 4, 4, 4, 4, 1, 0,
                                    0, 2, 6, 8, 8, 6, 2, 0,
                                    0, 1, 4, 4, 4, 4, 1, 0,   /* [black][sq] */
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0 };

const int knight_outpost_wh[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 1, 4, 4, 4, 4, 1, 0,
                                    0, 2, 6, 8, 8, 6, 2, 0,   /* [white][sq] */
                                    0, 1, 4, 4, 4, 4, 1, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0 };

/* Bishop PSTs */

const int bval_mg_bl[64] = {  0,   0,   0,   0,   0,   0,   0,   0,
                              0,   4,   4,   4,   4,   4,   4,   0,
                              0,   4,   8,   8,   8,   8,   4,   0,
                              0,   4,   8,  12,  12,   8,   4,   0,
                              0,   4,   8,  12,  12,   8,   4,   0,  /* [mg][black][sq] */
                              0,   4,   8,   8,   8,   8,   4,   0,
                              0,   4,   4,   4,   4,   4,   4,   0,
                              -15, -15, -15, -15, -15, -15, -15, -15};

const int bval_mg_wh[64] = {-15, -15, -15, -15, -15, -15, -15, -15,
                            0,   4,   4,   4,   4,   4,   4,   0,
                            0,   4,   8,   8,   8,   8,   4,   0,
                            0,   4,   8,  12,  12,   8,   4,   0,
                            0,   4,   8,  12,  12,   8,   4,   0,  /* [mg][white][sq] */
                            0,   4,   8,   8,   8,   8,   4,   0,
                            0,   4,   4,   4,   4,   4,   4,   0,
                            0,   0,   0,   0,   0,   0,   0,   0};

const int bval_eg_bl[64] = {  0,   0,   0,   0,   0,   0,   0,   0,
                              0,   4,   4,   4,   4,   4,   4,   0,
                              0,   4,   8,   8,   8,   8,   4,   0,
                              0,   4,   8,  12,  12,   8,   4,   0,
                              0,   4,   8,  12,  12,   8,   4,   0,  /* [eg][black][sq] */
                              0,   4,   8,   8,   8,   8,   4,   0,
                              0,   4,   4,   4,   4,   4,   4,   0,
                              -15, -15, -15, -15, -15, -15, -15, -15};

const int bval_eg_wh[64] = {-15, -15, -15, -15, -15, -15, -15, -15,
                            0,   4,   4,   4,   4,   4,   4,   0,
                            0,   4,   8,   8,   8,   8,   4,   0,
                            0,   4,   8,  12,  12,   8,   4,   0,
                            0,   4,   8,  12,  12,   8,   4,   0,  /* [eg][white][sq] */
                            0,   4,   8,   8,   8,   8,   4,   0,
                            0,   4,   4,   4,   4,   4,   4,   0,
                            0,   0,   0,   0,   0,   0,   0,   0};

/* Bishop Outposts */

const int bishop_outpost_bl[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 1, 3, 3, 3, 3, 1, 0,
                                    0, 3, 5, 5, 5, 5, 3, 0,
                                    0, 1, 2, 2, 2, 2, 1, 0,   /* [black][sq] */
                                    0, 0, 1, 1, 1, 1, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0 };

const int bishop_outpost_wh[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 1, 1, 1, 1, 0, 0,
                                    0, 1, 2, 2, 2, 2, 1, 0,
                                    0, 3, 5, 5, 5, 5, 3, 0,   /* [white][sq] */
                                    0, 1, 3, 3, 3, 3, 1, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0 };

/* Queen PSTs */

const int qval_mg_bl[64] = { 0,   0,   0,   0,   0,   0,   0,   0,
                             0,   0,   4,   4,   4,   4,   0,   0,
                             0,   4,   4,   6,   6,   4,   4,   0,
                             0,   4,   6,   8,   8,   6,   4,   0,
                             0,   4,   6,   8,   8,   6,   4,   0,   /* [mg][black][sq] */
                             0,   4,   4,   6,   6,   4,   4,   0,
                             0,   0,   4,   4,   4,   4,   0,   0,
                             0,   0,   0,   0,   0,   0,   0,   0 };

const int qval_mg_wh[64] = { 0,   0,   0,   0,   0,   0,   0,   0,
                             0,   0,   4,   4,   4,   4,   0,   0,
                             0,   4,   4,   6,   6,   4,   4,   0,
                             0,   4,   6,   8,   8,   6,   4,   0,
                             0,   4,   6,   8,   8,   6,   4,   0,   /* [mg][white][sq] */
                             0,   4,   4,   6,   6,   4,   4,   0,
                             0,   0,   4,   4,   4,   4,   0,   0,
                             0,   0,   0,   0,   0,   0,   0,   0 };

const int qval_eg_bl[64] = { 0,   0,   0,   0,   0,   0,   0,   0,
                             0,   0,   4,   4,   4,   4,   0,   0,
                             0,   4,   4,   6,   6,   4,   4,   0,
                             0,   4,   6,   8,   8,   6,   4,   0,
                             0,   4,   6,   8,   8,   6,   4,   0,   /* [eg][black][sq] */
                             0,   4,   4,   6,   6,   4,   4,   0,
                             0,   0,   4,   4,   4,   4,   0,   0,
                             0,   0,   0,   0,   0,   0,   0,   0 };

const int qval_eg_wh[64] =  { 0,   0,   0,   0,   0,   0,   0,   0,
                              0,   0,   4,   4,   4,   4,   0,   0,
                              0,   4,   4,   6,   6,   4,   4,   0,
                              0,   4,   6,   8,   8,   6,   4,   0,
                              0,   4,   6,   8,   8,   6,   4,   0,   /* [eg][white][sq] */
                              0,   4,   4,   6,   6,   4,   4,   0,
                              0,   0,   4,   4,   4,   4,   0,   0,
                              0,   0,   0,   0,   0,   0,   0,   0 };

/* King PSTs (endgame only) */

const int kval_eg_bl[64] = {-40, -40, -40, -40, -40, -40, -40, -40,
                            -40, -10, -10, -10, -10, -10, -10, -40,
                            -40, -10,  60,  60,  60,  60, -10, -40,
                            -40, -10,  60,  60,  60,  60, -10, -40,
                            -40, -10,  40,  40,  40,  40, -10, -40,   /* [black][sq] */
                            -40, -10,  20,  20,  20,  20, -10, -40,
                            -40, -10, -10, -10, -10, -10, -10, -40,
                            -40, -40, -40, -40, -40, -40, -40, -40 };

const int kval_eg_wh[64] = {-40, -40, -40, -40, -40, -40, -40, -40,
                            -40, -10, -10, -10, -10, -10, -10, -40,
                            -40, -10,  20,  20,  20,  20, -10, -40,
                            -40, -10,  40,  40,  40,  40, -10, -40,
                            -40, -10,  60,  60,  60,  60, -10, -40,   /* [white][sq] */
                            -40, -10,  60,  60,  60,  60, -10, -40,
                            -40, -10, -10, -10, -10, -10, -10, -40,
                            -40, -40, -40, -40, -40, -40, -40, -40 };

int MaterialDraw(const S_BOARD *pos) {

    ASSERT(CheckBoard(pos));

    if (!pos->pceNum[wR] && !pos->pceNum[bR] && !pos->pceNum[wQ] && !pos->pceNum[bQ]) {
        if (!pos->pceNum[bB] && !pos->pceNum[wB]) {
            if (pos->pceNum[wN] < 3 && pos->pceNum[bN] < 3) {  return TRUE; }
        } else if (!pos->pceNum[wN] && !pos->pceNum[bN]) {
            if (abs(pos->pceNum[wB] - pos->pceNum[bB]) < 2) { return TRUE; }
        } else if ((pos->pceNum[wN] < 3 && !pos->pceNum[wB]) || (pos->pceNum[wB] == 1 && !pos->pceNum[wN])) {
            if ((pos->pceNum[bN] < 3 && !pos->pceNum[bB]) || (pos->pceNum[bB] == 1 && !pos->pceNum[bN]))  { return TRUE; }
        }
    } else if (!pos->pceNum[wQ] && !pos->pceNum[bQ]) {
        if (pos->pceNum[wR] == 1 && pos->pceNum[bR] == 1) {
            if ((pos->pceNum[wN] + pos->pceNum[wB]) < 2 && (pos->pceNum[bN] + pos->pceNum[bB]) < 2)	{ return TRUE; }
        } else if (pos->pceNum[wR] == 1 && !pos->pceNum[bR]) {
            if ((pos->pceNum[wN] + pos->pceNum[wB] == 0) && (((pos->pceNum[bN] + pos->pceNum[bB]) == 1) || ((pos->pceNum[bN] + pos->pceNum[bB]) == 2))) { return TRUE; }
        } else if (pos->pceNum[bR] == 1 && !pos->pceNum[wR]) {
            if ((pos->pceNum[bN] + pos->pceNum[bB] == 0) && (((pos->pceNum[wN] + pos->pceNum[wB]) == 1) || ((pos->pceNum[wN] + pos->pceNum[wB]) == 2))) { return TRUE; }
        }
    }
    return FALSE;
}

//#define ENDGAME_MAT (1 * PieceVal[wR] + 2 * PieceVal[wN] + 2 * PieceVal[wP] + PieceVal[wK])

int EvalPosition(const S_BOARD *pos) {

    ASSERT(CheckBoard(pos));

    int pce;
    int pceNum;
    int sq;
    int total_material = pos->material[WHITE] + pos->material[BLACK] - 100000;
    int score = pos->material[WHITE] - pos->material[BLACK];

    if(!pos->pceNum[wP] && !pos->pceNum[bP] && MaterialDraw(pos) == TRUE) {
        return 0;
    }

    pce = wP;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        score += PawnTable[SQ64(sq)];

        if( (IsolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
            //printf("wP Iso:%s\n",PrSq(sq));
            score += PawnIsolated;
        }

        if( (WhitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            //printf("wP Passed:%s\n",PrSq(sq));
            score += PawnPassed[RanksBrd[sq]];
        }

    }

    pce = bP;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        score -= PawnTable[MIRROR64(SQ64(sq))];

        if( (IsolatedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            //printf("bP Iso:%s\n",PrSq(sq));
            score -= PawnIsolated;
        }

        if( (BlackPassedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
            //printf("bP Passed:%s\n",PrSq(sq));
            score -= PawnPassed[7 - RanksBrd[sq]];
        }
    }

    pce = wN;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        //score += KnightTable[SQ64(sq)];
        if( (total_material <= 2500) ) {
            score += nval_eg_wh[SQ64(sq)];
        } else {
            score += nval_mg_wh[SQ64(sq)];
        }
        score += knight_outpost_wh[SQ64(sq)];
    }

    pce = bN;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        //score -= KnightTable[MIRROR64(SQ64(sq))];
        if( (total_material <= 2500) ) {
            score -= nval_eg_bl[SQ64(sq)];
        } else {
            score -= nval_mg_bl[SQ64(sq)];
        }
        score -= knight_outpost_bl[SQ64(sq)];
    }

    pce = wB;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        //score += BishopTable[SQ64(sq)];
        if( (total_material <= 2500) ) {
            score += bval_eg_wh[SQ64(sq)];
        } else {
            score += bval_mg_wh[SQ64(sq)];
        }
        score += bishop_outpost_bl[SQ64(sq)];
    }

    pce = bB;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        //score -= BishopTable[MIRROR64(SQ64(sq))];
        if( (total_material <= 2500) ) {
            score -= bval_eg_bl[SQ64(sq)];
        } else {
            score -= bval_mg_bl[SQ64(sq)];
        }
        score -= bishop_outpost_bl[SQ64(sq)];
    }

    pce = wR;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        score += RookTable[SQ64(sq)];

        ASSERT(FileRankValid(FilesBrd[sq]));

        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            score += RookOpenFile;
        } else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
            score += RookSemiOpenFile;
        }
    }

    pce = bR;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        score -= RookTable[MIRROR64(SQ64(sq))];
        ASSERT(FileRankValid(FilesBrd[sq]));

        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            score -= RookOpenFile;
        } else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
            score -= RookSemiOpenFile;
        }
    }

    pce = wQ;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        ASSERT(FileRankValid(FilesBrd[sq]));
        if( (total_material <= 2500) ) {
            score += qval_eg_wh[SQ64(sq)];
        } else {
            score += qval_mg_wh[SQ64(sq)];
        }
        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            score += QueenOpenFile;
        } else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
            score += QueenSemiOpenFile;
        }
    }

    pce = bQ;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        ASSERT(FileRankValid(FilesBrd[sq]));
        if( (total_material <= 2500) ) {
            score -= qval_eg_bl[SQ64(sq)];
        } else {
            score -= qval_mg_bl[SQ64(sq)];
        }
        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            score -= QueenOpenFile;
        } else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
            score -= QueenSemiOpenFile;
        }
    }
    //8/p6k/6p1/5p2/P4K2/8/5pB1/8 b - - 2 62
    pce = wK;
    sq = pos->pList[pce][0];
    ASSERT(SqOnBoard(sq));
    ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);

    //if( (pos->material[BLACK] <= ENDGAME_MAT) ) {
    if( (total_material <= 2500) ) {
        score += KingE[SQ64(sq)];
    } else {
        score += KingO[SQ64(sq)];
    }

    pce = bK;
    sq = pos->pList[pce][0];
    ASSERT(SqOnBoard(sq));
    ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);

    //if( (pos->material[WHITE] <= ENDGAME_MAT) ) {
    if( (total_material <= 2500) ) {
        score -= KingE[MIRROR64(SQ64(sq))];
    } else {
        score -= KingO[MIRROR64(SQ64(sq))];
    }

    if(pos->pceNum[wB] >= 2) score += BishopPair;
    if(pos->pceNum[bB] >= 2) score -= BishopPair;

    if(pos->side == WHITE) {
        return score;
    } else {
        return -score;
    }
}