//
// Created by Eric Silverman on 23/02/2019.
//

#include <stdio.h>
#include "defs.h"
#include "eval.h"

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

    S_EVAL eval;
    static const S_EVAL emptyEval;
    eval = emptyEval;

    int pce;
    int pceNum;
    int sq;
    int final_eval;
    int total_material = pos->material[WHITE] + pos->material[BLACK] - 100000;
    eval.matScore = pos->material[WHITE] - pos->material[BLACK];

    if(!pos->pceNum[wP] && !pos->pceNum[bP] && MaterialDraw(pos) == TRUE) {
        return 0;
    }

    pce = wP;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        eval.pawns[WHITE] += PawnTable[SQ64(sq)];

        if( (IsolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
            //printf("wP Iso:%s\n",PrSq(sq));
            eval.pawns[WHITE] += PawnIsolated;
        }

        if( (WhitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            //printf("wP Passed:%s\n",PrSq(sq));
            eval.pawns[WHITE] += PawnPassed[RanksBrd[sq]];
        }

    }

    pce = bP;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        eval.pawns[BLACK] -= PawnTable[MIRROR64(SQ64(sq))];

        if( (IsolatedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            //printf("bP Iso:%s\n",PrSq(sq));
            eval.pawns[BLACK] -= PawnIsolated;
        }

        if( (BlackPassedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
            //printf("bP Passed:%s\n",PrSq(sq));
            eval.pawns[BLACK] -= PawnPassed[7 - RanksBrd[sq]];
        }
    }

    pce = wN;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        //score += KnightTable[SQ64(sq)];
        if( (total_material <= 2500) ) {
            eval.knights[WHITE] += nval_eg_wh[SQ64(sq)];
        } else {
            eval.knights[WHITE] += nval_mg_wh[SQ64(sq)];
        }
        eval.knights[WHITE] += knight_outpost_wh[SQ64(sq)];
    }

    pce = bN;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        //score -= KnightTable[MIRROR64(SQ64(sq))];
        if( (total_material <= 2500) ) {
            eval.knights[BLACK] -= nval_eg_bl[SQ64(sq)];
        } else {
            eval.knights[BLACK] -= nval_mg_bl[SQ64(sq)];
        }
        eval.knights[BLACK] -= knight_outpost_bl[SQ64(sq)];
    }

    pce = wB;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        //score += BishopTable[SQ64(sq)];
        if( (total_material <= 2500) ) {
            eval.bishops[WHITE] += bval_eg_wh[SQ64(sq)];
        } else {
            eval.bishops[WHITE] += bval_mg_wh[SQ64(sq)];
        }
        eval.bishops[WHITE] += bishop_outpost_bl[SQ64(sq)];
    }

    pce = bB;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        //score -= BishopTable[MIRROR64(SQ64(sq))];
        if( (total_material <= 2500) ) {
            eval.bishops[BLACK] -= bval_eg_bl[SQ64(sq)];
        } else {
            eval.bishops[BLACK] -= bval_mg_bl[SQ64(sq)];
        }
        eval.bishops[BLACK] -= bishop_outpost_bl[SQ64(sq)];
    }

    pce = wR;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        eval.rooks[WHITE] += RookTable[SQ64(sq)];

        ASSERT(FileRankValid(FilesBrd[sq]));

        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            eval.rooks[WHITE] += RookOpenFile;
        } else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
            eval.rooks[WHITE] += RookSemiOpenFile;
        }
    }

    pce = bR;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        eval.rooks[BLACK] -= RookTable[MIRROR64(SQ64(sq))];
        ASSERT(FileRankValid(FilesBrd[sq]));

        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            eval.rooks[BLACK] -= RookOpenFile;
        } else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
            eval.rooks[BLACK] -= RookSemiOpenFile;
        }
    }

    pce = wQ;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        ASSERT(FileRankValid(FilesBrd[sq]));
        if( (total_material <= 2500) ) {
            eval.queens[WHITE] += qval_eg_wh[SQ64(sq)];
        } else {
            eval.queens[WHITE] += qval_mg_wh[SQ64(sq)];
        }
        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            eval.queens[WHITE] += QueenOpenFile;
        } else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
            eval.queens[WHITE] += QueenSemiOpenFile;
        }
    }

    pce = bQ;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        ASSERT(FileRankValid(FilesBrd[sq]));
        if( (total_material <= 2500) ) {
            eval.queens[BLACK] -= qval_eg_bl[SQ64(sq)];
        } else {
            eval.queens[BLACK] -= qval_mg_bl[SQ64(sq)];
        }
        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            eval.queens[BLACK] -= QueenOpenFile;
        } else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
            eval.queens[BLACK] -= QueenSemiOpenFile;
        }
    }
    //8/p6k/6p1/5p2/P4K2/8/5pB1/8 b - - 2 62
    pce = wK;
    sq = pos->pList[pce][0];
    ASSERT(SqOnBoard(sq));
    ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);

    //if( (pos->material[BLACK] <= ENDGAME_MAT) ) {
    if( (total_material <= 2500) ) {
        eval.kings[WHITE] += KingE[SQ64(sq)];
    } else {
        eval.kings[WHITE] += KingO[SQ64(sq)];
    }

    pce = bK;
    sq = pos->pList[pce][0];
    ASSERT(SqOnBoard(sq));
    ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);

    //if( (pos->material[WHITE] <= ENDGAME_MAT) ) {
    if( (total_material <= 2500) ) {
        eval.kings[BLACK] -= KingE[MIRROR64(SQ64(sq))];
    } else {
        eval.kings[BLACK] -= KingO[MIRROR64(SQ64(sq))];
    }

    if(pos->pceNum[wB] >= 2) eval.pairs[WHITE] += BishopPair;
    if(pos->pceNum[bB] >= 2) eval.pairs[BLACK] -= BishopPair;
    if(pos->pceNum[wN] >= 2) eval.pairs[WHITE] += KnightPair;
    if(pos->pceNum[bN] >= 2) eval.pairs[BLACK] -= KnightPair;
    if(pos->pceNum[wR] >= 2) eval.pairs[WHITE] += RookPair;
    if(pos->pceNum[bR] >= 2) eval.pairs[BLACK] -= RookPair;

    final_eval = eval.matScore + eval.tempo + (eval.pawns[WHITE] + eval.pawns[BLACK]) + (eval.knights[WHITE] + eval.knights[BLACK])
            + (eval.bishops[WHITE] + eval.bishops[BLACK]) + (eval.rooks[WHITE] + eval.rooks[BLACK])
            + (eval.queens[WHITE] + eval.queens[BLACK]) + (eval.kings[WHITE] + eval.kings[BLACK])
                 + (eval.pairs[WHITE] + eval.pairs[BLACK]);

    if(pos->side == WHITE) {
        return final_eval;
    } else {
        return -final_eval;
    }
}