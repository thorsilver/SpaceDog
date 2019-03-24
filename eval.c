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

int CheckMob(S_BOARD *pos, int side) {
    // Mobility
    int flipped = 0;
    if(pos->side != side) {
        MakeNullMove(pos);
        flipped = 1;
        //printf("Board flipped!");
    }
    S_MOVELIST moblist[1];
    GenerateAllMoves(pos, moblist);
    int bishMob = 0;
    int knightMob = 0;
    int rookMob = 0;
    int queenMob = 0;
    int moveNum = 0;
    for(moveNum = 0; moveNum < moblist->count; ++moveNum){
        /*if(!MakeMove(pos, moblist->moves[moveNum].move)){
            continue;
        }*/
        //TakeMove(pos);
        int from = FROMSQ(moblist->moves[moveNum].move);
        int movingPiece = pos->pieces[from];
        //printf("Moving Piece: %d\n", movingPiece);
        if(pos->side == WHITE && movingPiece != 0) {
            if (movingPiece == wN) {
                knightMob += 1;
            } else if (movingPiece == wB) {
                bishMob += 1;
            } else if (movingPiece == wR) {
                rookMob += 1;
            } else if (movingPiece == wQ) {
                queenMob += 1;
            }
        } else if (pos->side == BLACK && movingPiece != 0) {
            if (movingPiece == bN) {
                knightMob += 1;
            } else if (movingPiece == wB) {
                bishMob += 1;
            } else if (movingPiece == wR) {
                rookMob += 1;
            } else if (movingPiece == wQ) {
                queenMob += 1;
            }
        }
    }
    if (flipped == 1) {
        TakeNullMove(pos);
    }
    int mobScore = (4 * (knightMob - 4)) + (3 * (bishMob - 7)) + (4 * (rookMob - 7)) + (4 * (queenMob - 14));
    return mobScore;
}


//#define ENDGAME_MAT (1 * PieceVal[wR] + 2 * PieceVal[wN] + 2 * PieceVal[wP] + PieceVal[wK])

int EvalPosition(S_BOARD *pos) {

    ASSERT(CheckBoard(pos));

    S_EVAL eval;
    static const S_EVAL emptyEval;
    eval = emptyEval;

    int pce;
    int pceNum;
    int sq;
    int eval_mg, eval_eg, final_eval;
    int total_material = pos->material[WHITE] + pos->material[BLACK] - 100000;
    eval.matScore = pos->material[WHITE] - pos->material[BLACK];

    if(!pos->pceNum[wP] && !pos->pceNum[bP] && MaterialDraw(pos) == TRUE) {
        return 0;
    }

    // Calculate the game phase based on remaining material (Fruit Method)
    eval.phase = 24 - 4 * (pos->pceNum[wQ] + pos->pceNum[bQ])
                 - 2 * (pos->pceNum[wR] + pos->pceNum[bR])
                 - 1 * (pos->pceNum[wN] + pos->pceNum[bN])
                 |(pos->pceNum[wB] + pos->pceNum[bB]);
    eval.phase = (eval.phase * 256 + 12) / 24;

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

        eval.knights_eg[WHITE] += nval_eg_wh[SQ64(sq)];

        eval.knights[WHITE] += nval_mg_wh[SQ64(sq)];

        eval.knights[WHITE] += knight_outpost_wh[SQ64(sq)];
    }

    pce = bN;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        //score -= KnightTable[MIRROR64(SQ64(sq))];

        eval.knights_eg[BLACK] -= nval_eg_bl[SQ64(sq)];

        eval.knights[BLACK] -= nval_mg_bl[SQ64(sq)];

        eval.knights[BLACK] -= knight_outpost_bl[SQ64(sq)];
    }

    pce = wB;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        //score += BishopTable[SQ64(sq)];

        eval.bishops_eg[WHITE] += bval_eg_wh[SQ64(sq)];

        eval.bishops[WHITE] += bval_mg_wh[SQ64(sq)];

        eval.bishops[WHITE] += bishop_outpost_bl[SQ64(sq)];
    }

    pce = bB;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        //score -= BishopTable[MIRROR64(SQ64(sq))];

        eval.bishops_eg[BLACK] -= bval_eg_bl[SQ64(sq)];

        eval.bishops[BLACK] -= bval_mg_bl[SQ64(sq)];

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

        eval.queens_eg[WHITE] += qval_eg_wh[SQ64(sq)];

        eval.queens[WHITE] += qval_mg_wh[SQ64(sq)];

        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            eval.queens_eg[WHITE] += QueenOpenFile;
            eval.queens[WHITE] += QueenOpenFile;
        } else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
            eval.queens_eg[WHITE] += QueenSemiOpenFile;
            eval.queens[WHITE] += QueenSemiOpenFile;
        }
    }

    pce = bQ;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        ASSERT(FileRankValid(FilesBrd[sq]));

        eval.queens_eg[BLACK] -= qval_eg_bl[SQ64(sq)];

        eval.queens[BLACK] -= qval_mg_bl[SQ64(sq)];

        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            eval.queens_eg[BLACK] -= QueenOpenFile;
            eval.queens[BLACK] -= QueenOpenFile;
        } else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
            eval.queens_eg[BLACK] -= QueenSemiOpenFile;
            eval.queens[BLACK] -= QueenSemiOpenFile;
        }
    }
    //8/p6k/6p1/5p2/P4K2/8/5pB1/8 b - - 2 62
    pce = wK;
    sq = pos->pList[pce][0];
    ASSERT(SqOnBoard(sq));
    ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);

    eval.kings_eg[WHITE] += KingE[SQ64(sq)];

    eval.kings[WHITE] += KingO[SQ64(sq)];


    pce = bK;
    sq = pos->pList[pce][0];
    ASSERT(SqOnBoard(sq));
    ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);

    eval.kings_eg[BLACK] -= KingE[MIRROR64(SQ64(sq))];

    eval.kings[BLACK] -= KingO[MIRROR64(SQ64(sq))];

    if(pos->pceNum[wB] >= 2) eval.pairs[WHITE] += BishopPair;
    if(pos->pceNum[bB] >= 2) eval.pairs[BLACK] -= BishopPair;
    if(pos->pceNum[wN] >= 2) eval.pairs[WHITE] += KnightPair;
    if(pos->pceNum[bN] >= 2) eval.pairs[BLACK] -= KnightPair;
    if(pos->pceNum[wR] >= 2) eval.pairs[WHITE] += RookPair;
    if(pos->pceNum[bR] >= 2) eval.pairs[BLACK] -= RookPair;


    //int evalMobWhite = CheckMob(pos, WHITE);
    //printf("evalMobWhite = %d", evalMobWhite);
    //int evalMobBlack = CheckMob(pos, BLACK);
    //printf("evalMobBlack = %d", evalMobBlack);

    // Tempo score for side to move
    if (pos->side == WHITE) {
        eval.tempo = 25;
    } else {
        eval.tempo = -12;
    }

    /*final_eval = eval.matScore + eval.tempo + (eval.pawns[WHITE] + eval.pawns[BLACK]) + (eval.knights[WHITE] + eval.knights[BLACK])
            + (eval.bishops[WHITE] + eval.bishops[BLACK]) + (eval.rooks[WHITE] + eval.rooks[BLACK])
            + (eval.queens[WHITE] + eval.queens[BLACK]) + (eval.kings[WHITE] + eval.kings[BLACK])
                 + (eval.pairs[WHITE] + eval.pairs[BLACK]) + tempo[pos->side];*/

    // Calculate opening/middlegame eval total
    eval_mg = eval.matScore + eval.tempo + (eval.pawns[WHITE] + eval.pawns[BLACK]) + (eval.knights[WHITE] + eval.knights[BLACK])
              + (eval.bishops[WHITE] + eval.bishops[BLACK]) + (eval.rooks[WHITE] + eval.rooks[BLACK])
              + (eval.queens[WHITE] + eval.queens[BLACK]) + (eval.kings[WHITE] + eval.kings[BLACK])
              + (eval.pairs[WHITE] + eval.pairs[BLACK]);

    // Calculate endgame eval total
    eval_eg = eval.matScore + eval.tempo + (eval.pawns[WHITE] + eval.pawns[BLACK]) + (eval.knights_eg[WHITE] + eval.knights_eg[BLACK])
              + (eval.bishops_eg[WHITE] + eval.bishops_eg[BLACK]) + (eval.rooks[WHITE] + eval.rooks[BLACK])
              + (eval.queens_eg[WHITE] + eval.queens_eg[BLACK]) + (eval.kings_eg[WHITE] + eval.kings_eg[BLACK])
              + (eval.pairs[WHITE] + eval.pairs[BLACK]);

    // Scale eval  by game phase
    final_eval = (eval_mg * (256 - eval.phase) + eval_eg * eval.phase) / 256;

    if(pos->side == WHITE) {
        return final_eval;
    } else {
        return -final_eval;
    }
    //return final_eval;
}
