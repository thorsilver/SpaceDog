//
// Created by Eric Silverman on 16/04/2019.
//

/*
 *  Copyright (C) 2015-2016, Marcel van Kervinck
 *  All rights reserved
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdbool.h>
#include <time.h>
#include "defs.h"
#include "endgame.h"


/* Pretty Fast KPK Bitbase Generator by Marcel van Kervinck */

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

enum { N = a2-a1, S = -N, E = b1-a1, W = -E }; // Derived geometry

#define inPawnZone(square) (rank(square)!=rank1 && rank(square)!=rank8)
#define conflict(wKing,wPawn,bKing) (wKing==wPawn || wPawn==bKing || wKing==bKing)

#define dist(a,b) (abs(file(a)-file(b)) | abs(rank(a)-rank(b))) // logical-or as max()
#define wInCheck(wKing,wPawn,bKing) (dist(wKing,bKing)==1)
#define bInCheck(wKing,wPawn,bKing) (dist(wKing,bKing)==1                     \
                                  || (file(wPawn)!=fileA && wPawn+N+W==bKing) \
                                  || (file(wPawn)!=fileH && wPawn+N+E==bKing))

// Square set macros (no need to adapt these to the specific geometry)
#define bit(i) (1ULL << (i))
#define mask 0x0101010101010101ULL
#define allW(set) ((set) >> 8)
#define allE(set) ((set) << 8)
#define allS(set) (((set) & ~mask) >> 1)
#define allN(set) (((set) << 1) & ~mask)
#define allKing(set) (allW(allN(set)) | allN(set) | allE(allN(set)) \
                    | allW(set)                   | allE(set)       \
                    | allW(allS(set)) | allS(set) | allE(allS(set)))

#define arrayLen(a) ((int) (sizeof(a) / sizeof((a)[0])))
enum { white, black };

#define kpIndex(wKing,wPawn) (((wKing) << 5) + (file(wPawn) << 3) + rank(wPawn))
#define wKingSquare(ix) ((ix)>>5)
#define wPawnSquare(ix) square(((ix)>>3)&3, (ix)&7)

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

uint64_t kpkTable[2][64*32];
static const int kingSteps[] = { N+W, N, N+E, W, E, S+W, S, S+E };

// Drive enemy King towards the edge of the board
static const int edgePush[64] = {
        100, 90, 80, 70, 70, 80, 90, 100,
        90 , 70, 60, 50, 50, 60, 70,  90,
        80 , 60, 40, 30, 30, 40, 60,  80,
        70 , 50, 30, 20, 20, 30, 50,  70,
        70 , 50, 30, 20, 20, 30, 50,  70,
        80 , 60, 40, 30, 30, 40, 60,  80,
        90 , 70, 60, 50, 50, 60, 70,  90,
        100, 90, 80, 70, 70, 80, 90, 100
};

// Drive King towards the corner of the boards (A1/H8)
/*static const int cornerPushB[64] = {
        100, 100, 70, 60, 50, 40, 10,  5,
        100, 70, 60, 50, 40, 10, 10, 10,
        70, 60, 50, 40, 10, 10, 10, 40,
        60, 50, 40, 10, 10, 10, 40, 50,
        50, 40, 10, 10, 10, 40, 50, 60,
        40, 10, 10, 10, 40, 50, 60, 70,
        10, 10, 10, 40, 50, 60, 70, 100,
        5 , 10, 40, 50, 60, 70, 100, 100
};*/

static const int cornerPushB[64] = {
        200, 190, 180, 170, 160, 150, 140, 130,
        190, 180, 170, 160, 150, 140, 130, 140,
        180, 170, 155, 140, 140, 125, 140, 150,
        170, 160, 140, 120, 110, 140, 150, 160,
        160, 150, 140, 110, 120, 140, 160, 170,
        150, 140, 125, 140, 140, 155, 170, 180,
        140, 130, 140, 150, 160, 170, 180, 190,
        130, 140, 150, 160, 170, 180, 190, 200
};

static const int cornerPushW[64] = {
        130, 140, 150, 160, 170, 180, 190, 200,
        140, 130, 140, 150, 160, 170, 180, 190,
        150, 140, 125, 140, 140, 155, 170, 180,
        160, 150, 140, 110, 120, 140, 160, 170,
        170, 160, 140, 120, 110, 140, 150, 160,
        180, 170, 155, 140, 140, 125, 140, 150,
        190, 180, 170, 160, 150, 140, 130, 140,
        200, 190, 180, 170, 160, 150, 140, 130
};

/*static const int cornerPushW[64] = {
        5 ,  10,  40, 50, 60, 70, 100, 100,
        10,  10,  10, 40, 50, 60, 70,  100,
        40,  10,  10, 10, 40, 50, 60,  70,
        50,  40,  10, 10, 10, 40, 50,  60,
        60,  50,  40, 10, 10, 10, 40,  50,
        70,  60,  50, 40, 10, 10, 10,  40,
        100, 70,  60, 50, 40, 10, 10,  10,
        100, 100, 70, 60, 50, 40, 10,   5

};*/

static const int kingClose[8] = { 0, 0, 100, 70, 50, 30, 10, 0};
static const int kingFar[8] = { 0, 5, 20, 40, 60, 80, 90, 100 };

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

int manhattanDistance(int sq1, int sq2) {
    int file1 = FilesBrd[sq1];
    int file2 = FilesBrd[sq2];
    int rank1 = RanksBrd[sq1];
    int rank2 = RanksBrd[sq2];
    int rankDist = abs(rank1 - rank2);
    int fileDist = abs(file1 - file2);
    return rankDist + fileDist;
}

int sq_distance(int sq1, int sq2){
    int file1 = FilesBrd[sq1];
    int file2 = FilesBrd[sq2];
    int rank1 = RanksBrd[sq1];
    int rank2 = RanksBrd[sq2];
    int rankDist = abs(rank1 - rank2);
    int fileDist = abs(file1 - file2);
    return rankDist > fileDist ? rankDist : fileDist;
}

int kpkProbe(int side, int wKing, int wPawn, int bKing)
{
    if (!kpkTable[0][1]) kpkGenerate();

    if (file(wPawn) >= 4) {
        wKing ^= square(7, 0);
        wPawn ^= square(7, 0);
        bKing ^= square(7, 0);
    }
    int ix = kpIndex(wKing, wPawn);
    int bit = (kpkTable[side][ix] >> bKing) & 1;
    return (side == white) ? bit : -bit;
}

int kpkProbeBlack(int side, int wKing, int bPawn, int bKing) {
    int bSide = side ^ 1;
    int blackKing = mirror_board[wKing];
    int whiteKing = mirror_board[blackKing];
    int blackPawn = mirror_board[bPawn];
    return kpkProbe(bSide, whiteKing, blackPawn, blackKing);
}

int kpkGenerate(void)
{
    uint64_t valid[ arrayLen(kpkTable[0]) ];

    for (int ix=0; ix<arrayLen(kpkTable[0]); ix++) {
        int wKing = wKingSquare(ix), wPawn = wPawnSquare(ix);

        // Positions after winning pawn promotion (we can ignore stalemate here)
        if (rank(wPawn) == rank8 && wKing != wPawn) {
            uint64_t lost = ~allKing(bit(wKing)) & ~bit(wKing) & ~bit(wPawn);
            if (dist(wKing, wPawn) > 1)
                lost &= ~allKing(bit(wPawn));
            kpkTable[black][ix] = lost;
        }

        // Valid positions after black move, pawn capture allowed
        valid[ix] = ~allKing(bit(wKing));
        if (rank(wPawn) != rank8 && file(wPawn) != fileA) valid[ix] &= ~bit(wPawn+N+W);
        if (rank(wPawn) != rank8 && file(wPawn) != fileH) valid[ix] &= ~bit(wPawn+N+E);
    }

    int changed;
    do {
        for (int ix=0; ix<arrayLen(kpkTable[0]); ix++) {
            int wKing = wKingSquare(ix), wPawn = wPawnSquare(ix);
            if (!inPawnZone(wPawn))
                continue;

            // White king moves
            uint64_t won = 0;
            for (int i=0; i<arrayLen(kingSteps); i++) {
                int to = wKing + kingSteps[i];
                int jx = ix + kpIndex(kingSteps[i], 0);
                if (dist(wKing, to & 63) == 1 && to != wPawn)
                    won |= kpkTable[black][jx] & ~allKing(bit(to));
            }
            // White pawn moves
            if (wPawn+N != wKing) {
                won |= kpkTable[black][ix+kpIndex(0,N)] & ~bit(wPawn+N);
                if (rank(wPawn) == rank2 && wPawn+N+N != wKing)
                    won |= kpkTable[black][ix+kpIndex(0,N+N)]
                           & ~bit(wPawn+N) & ~bit(wPawn+N+N);
            }
            kpkTable[white][ix] = won & ~bit(wPawn);
        }

        changed = 0;
        for (int ix=0; ix<arrayLen(kpkTable[0]); ix++) {
            if (!inPawnZone(wPawnSquare(ix)))
                continue;

            // Black king moves
            uint64_t isBad = kpkTable[white][ix] | ~valid[ix];
            uint64_t canDraw = allKing(~isBad);
            uint64_t hasMoves = allKing(valid[ix]);
            uint64_t lost = hasMoves & ~canDraw;

            changed += (kpkTable[black][ix] != lost);
            kpkTable[black][ix] = lost;
        }
    } while (changed);

    printf("KPK Bitbase Generated!\n");
    return sizeof kpkTable;
}

int kpkSelfCheck(void)
{
    int counts[] = {                // As given by Steven J. Edwards (1996):
            163328 / 2, 168024 / 2, // - Legal positions per side
            124960 / 2, 97604  / 2  // - Non-draw positions per side
    };
    for (int ix=0; ix<arrayLen(kpkTable[0]); ix++) {
        int wKing = wKingSquare(ix), wPawn = wPawnSquare(ix);
        for (int bKing=0; bKing<boardSize; bKing++) {
            if (!inPawnZone(wPawn) || conflict(wKing, wPawn, bKing))
                continue;
            counts[0] -= !bInCheck(wKing, wPawn, bKing);
            counts[1] -= !wInCheck(wKing, wPawn, bKing);
            counts[2] -= !bInCheck(wKing, wPawn, bKing) && ((kpkTable[white][ix] >> bKing) & 1);
            counts[3] -= !wInCheck(wKing, wPawn, bKing) && ((kpkTable[black][ix] >> bKing) & 1);
        }
    }
    return !counts[0] && !counts[1] && !counts[2] && !counts[3];
}

/*----------------------------------------------------------------------+
 |         TESTS                                                        |
 +----------------------------------------------------------------------*/

struct {
    int side, wKing, wPawn, bKing, expected;
} tests[] = {
        { 0, a1, a2, a8, 0 },
        { 0, a1, a2, h8, 1 },
        { 1, a1, a2, a8, 0 },
        { 1, a1, a2, h8, -1 },
        { 1, a1, a2, g2, 0 },
        { 1, a1, a2, g1, -1 },
        { 0, a5, a4, d4, 1 },
        { 1, a5, a4, d4, 0 },
        { 0, a1, f4, a3, 1 },
        { 1, a1, f4, a3, 0 },
        { 1, a3, a4, f3, -1 },
        { 0, h6, g6, g8, 1 },
        { 0, h3, h2, b7, 1 },
        { 1, a5, a4, e6, 0 },
        { 1, f8, g6, h8, 0 },
        { 0, f6, g5, g8, 1 },
        { 0, d1, c3, f8, 1 },
        { 0, d4, c4, e6, 1 },
        { 0, c6, d6, d8, 1 },
        { 1, d6, e6, d8, -1 },
        { 0, g6, g5, h8, 1 },
        { 1, g6, g5, h8, -1 },
        { 0, e4, e3, e6, 0 },
        { 1, e4, e3, e6, -1 },
        { 1, h3, b2, h5, -1 },
        { 0, g2, b2, g5, 1 },
        { -1 },
};

int pfkpkTests(void)
{
    int err = 0;
    bool passed;

    /*
     *  kpkGenerate speed
     */
    clock_t start = clock();
    int size = kpkGenerate();
    clock_t finish = clock();
    printf("kpkGenerate CPU time [seconds]: %g\n",
           (double)(finish - start) / CLOCKS_PER_SEC);

    /*
     *  kpkTable size
     */
    printf("kpkTable size [bytes]: %d\n", size);

    /*
     *  kpkSelfCheck
     */
    passed = kpkSelfCheck();
    printf("kpkSelfCheck: %s\n", passed ? "OK" : "FAILED");
    if (!passed)
        err = EXIT_FAILURE;

    /*
     *  kpkProbe
     */
    int nrPassed = 0;
    int ix;
    for (ix=0; tests[ix].side>=0; ix++) {
        int result = kpkProbe(tests[ix].side,
                              tests[ix].wKing,
                              tests[ix].wPawn,
                              tests[ix].bKing);
        if (result == tests[ix].expected)
            nrPassed++;
    }
    passed = (nrPassed == ix);
    printf("kpkProbe %d/%d: %s\n", nrPassed, ix, passed ? "OK" : "FAILED");
    if (!passed)
        err = EXIT_FAILURE;

    return err;
}

// Evaluate King + (Queen or Rook) vs King
int evalKXK(S_BOARD *pos, unsigned strongSide, int piece)
{
    unsigned weakSide = strongSide ^ 1;
    int eval = 0;

    /*// Stalemate detection
    if (pos->side == weakSide) {
        S_MOVELIST list[1];
        GenerateAllMoves(pos,list);

        int MoveNum = 0;
        int found = 0;
        for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
            if (!MakeMove(pos,list->moves[MoveNum].move))  {
                continue;
            }
            found++;
            TakeMove(pos);
            break;
        }
        if(found == 0) {
            return 0;
        }
    }*/

    int strongKSq = SQ64(pos->KingSq[strongSide]);
    int weakKSq = SQ64(pos->KingSq[weakSide]);

    eval = TBWIN + PieceVal[piece] + kingClose[sq_distance(strongKSq, weakKSq)] + edgePush[weakKSq];

    return strongSide == pos->side ? eval : -eval;
}

// Evaluate King + Pawn vs King (Black side).  Uses PFKPK bitbase.
int evalBlackKPK(S_BOARD *pos) {
    int pawnSq;
    int eval;
    int kpkResult;
    int wkSq = SQ64(pos->KingSq[BLACK]);
    int bkSq = SQ64(pos->KingSq[WHITE]);
    pawnSq = SQ64(pos->pList[bP][0]);

    kpkResult = kpkProbeBlack(pos->side, wkSq, pawnSq, bkSq);
    eval = TBWIN + PieceVal[bP] + (8 - SQ120(RanksBrd[pawnSq]));
    return (kpkResult * eval);
}

// Evaluate King + Pawn vs King (White side).  Uses PFKPK bitbase.
int evalWhiteKPK(S_BOARD *pos) {
    int pawnSq;
    int eval;
    int kpkResult;
    int wkSq = SQ64(pos->KingSq[WHITE]);
    int bkSq = SQ64(pos->KingSq[BLACK]);
    pawnSq = SQ64(pos->pList[wP][0]);

    kpkResult = kpkProbe(pos->side, wkSq, pawnSq, bkSq);
    eval = TBWIN + PieceVal[wP] + (8 - SQ120(RanksBrd[pawnSq]));
    return (kpkResult * eval);
}

// Evaluate King + Pawn vs King. Uses PFKPK bitbase.
int evalKPK(S_BOARD *pos, unsigned int strongSide)
{
    int pawnSq = 0;
    int eval = 0;
    int kpkResult = 0;

    int wkSq = SQ64(pos->KingSq[WHITE]);
    int bkSq = SQ64(pos->KingSq[BLACK]);
    if(strongSide == WHITE) {
        pawnSq = SQ64(pos->pList[wP][0]);
    } else if(strongSide == BLACK) {
        pawnSq = SQ64(pos->pList[bP][0]);
    }

    if(strongSide == WHITE) {
        kpkResult = kpkProbe(pos->side, wkSq, pawnSq, bkSq);
        if(kpkResult == 0) {
            return 0;
        } else {
            eval = TBWIN + PieceVal[wP] + RanksBrd[pawnSq];
        }
    } else if(strongSide == BLACK) {
        kpkResult = kpkProbeBlack(pos->side, wkSq, pawnSq, bkSq);
        if(kpkResult == 0) {
            return 0;
        } else {
            eval = TBWIN + PieceVal[bP] + (8-RanksBrd[pawnSq]);
            //printf("pawnSQ: %d BK: %d WK: %d kpk: %d eval: %d\n", pawnSq, wkSq, bkSq, kpkResult, eval);
        }
    }

    /*if(kpkResult == 0) {
        return 0;
    } else if(kpkResult == 1){
        eval = TBWIN + PieceVal[wP] + RanksBrd[pawnSq];
    } else if(kpkResult == -1) {
        eval = TBWIN + PieceVal[bP] + (8 - RanksBrd[pawnSq]);
    }*/
    //printf("pawnSQ: %d BK: %d WK: %d kpk: %d eval: %d\n", pawnSq, wkSq, bkSq, kpkResult, eval);
    return strongSide == pos->side ? eval : -eval;
}

// Evaluate King + Bishop + Knight vs King
int evalKBNK(const S_BOARD *pos, int strongSide, int bish_sq)
{
    int bish_colour = 0;
    int eval = 0;
    //unsigned weakSide = strongSide ^ 1;

    int strongKSq = 0;
    int weakKSq = 0;
    if(strongSide == WHITE){
        strongKSq = SQ64(pos->KingSq[WHITE]);
        weakKSq = SQ64(pos->KingSq[BLACK]);
        bish_colour = sq_colors[bish_sq];
    } else if(strongSide == BLACK) {
        strongKSq = SQ64(pos->KingSq[BLACK]);
        weakKSq = SQ64(pos->KingSq[WHITE]);
        bish_colour = sq_colors[bish_sq];
    }

    //printf("wkSQ: %d bkSQ: %d wbSQ: %d\n", strongKSq, weakKSq, bish_colour);
    if(bish_colour == BLACK) {
        eval = TBWIN + kingClose[sq_distance(strongKSq, weakKSq)] + cornerPushB[weakKSq];
    } else if(bish_colour == WHITE) {
        eval = TBWIN + kingClose[sq_distance(strongKSq, weakKSq)] + cornerPushW[weakKSq];
    }

    return strongSide == pos->side ? eval : -eval;
}