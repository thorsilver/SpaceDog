//
// Created by Eric Silverman on 12/10/2018.
//

#include <stdio.h>
#include "defs.h"
#define SQOFFBOARD(sq) (FilesBrd[(sq)]==OFFBOARD)

int CheckBoard(const S_BOARD *pos) {

    int t_pceNum[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int t_bigPce[2] = { 0, 0};
    int t_majPce[2] = { 0, 0};
    int t_minPce[2] = { 0, 0};
    int t_material[2] = { 0, 0};

    int sq64,t_piece,t_pce_num,sq120,colour,pcount;

    U64 t_pawns[3] = {0ULL, 0ULL, 0ULL};

    t_pawns[WHITE] = pos->pawns[WHITE];
    t_pawns[BLACK] = pos->pawns[BLACK];
    t_pawns[BOTH] = pos->pawns[BOTH];

    // check piece lists
    for(t_piece = wP; t_piece <= bK; ++t_piece) {
        for(t_pce_num = 0; t_pce_num < pos->pceNum[t_piece]; ++t_pce_num) {
            sq120 = pos->pList[t_piece][t_pce_num];
            ASSERT(pos->pieces[sq120]==t_piece);
        }
    }

    // check piece count and other counters
    for(sq64 = 0; sq64 < 64; ++sq64) {
        sq120 = SQ120(sq64);
        t_piece = pos->pieces[sq120];
        t_pceNum[t_piece]++;
        colour = PieceCol[t_piece];
        if( PieceBig[t_piece] == TRUE) t_bigPce[colour]++;
        if( PieceMin[t_piece] == TRUE) t_minPce[colour]++;
        if( PieceMaj[t_piece] == TRUE) t_majPce[colour]++;

        t_material[colour] += PieceVal[t_piece];
    }

    for(t_piece = wP; t_piece <= bK; ++t_piece) {
        ASSERT(t_pceNum[t_piece]==pos->pceNum[t_piece]);
    }

    // check bitboards count
    pcount = CNT(t_pawns[WHITE]);
    ASSERT(pcount == pos->pceNum[wP]);
    pcount = CNT(t_pawns[BLACK]);
    ASSERT(pcount == pos->pceNum[bP]);
    pcount = CNT(t_pawns[BOTH]);
    ASSERT(pcount == (pos->pceNum[bP] + pos->pceNum[wP]));

    // check bitboards squares
    while(t_pawns[WHITE]) {
        sq64 = POP(&t_pawns[WHITE]);
        ASSERT(pos->pieces[SQ120(sq64)] == wP);
    }

    while(t_pawns[BLACK]) {
        sq64 = POP(&t_pawns[BLACK]);
        ASSERT(pos->pieces[SQ120(sq64)] == bP);
    }

    while(t_pawns[BOTH]) {
        sq64 = POP(&t_pawns[BOTH]);
        ASSERT( (pos->pieces[SQ120(sq64)] == bP) || (pos->pieces[SQ120(sq64)] == wP) );
    }

    ASSERT(t_material[WHITE]==pos->material[WHITE] && t_material[BLACK]==pos->material[BLACK]);
    ASSERT(t_minPce[WHITE]==pos->minPce[WHITE] && t_minPce[BLACK]==pos->minPce[BLACK]);
    ASSERT(t_majPce[WHITE]==pos->majPce[WHITE] && t_majPce[BLACK]==pos->majPce[BLACK]);
    ASSERT(t_bigPce[WHITE]==pos->bigPce[WHITE] && t_bigPce[BLACK]==pos->bigPce[BLACK]);

    ASSERT(pos->side==WHITE || pos->side==BLACK);
    ASSERT(GeneratePosKey(pos)==pos->posKey);

    ASSERT(pos->enPas==NO_SQ || ( RanksBrd[pos->enPas]==RANK_6 && pos->side == WHITE)
           || ( RanksBrd[pos->enPas]==RANK_3 && pos->side == BLACK));

    ASSERT(pos->pieces[pos->KingSq[WHITE]] == wK);
    ASSERT(pos->pieces[pos->KingSq[BLACK]] == bK);

    ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

    //ASSERT(PceListOk(pos));

    return TRUE;
}

void UpdateListsMaterial(S_BOARD *pos) {

    int piece,sq,index,colour;

    for(index = 0; index < BRD_SQ_NUM; ++index) {
        sq = index;
        piece = pos->pieces[index];
        //ASSERT(PceValidEmptyOffbrd(piece));
        if(piece!=OFFBOARD && piece!= EMPTY) {
            colour = PieceCol[piece];
            //ASSERT(SideValid(colour));

            if( PieceBig[piece] == TRUE) pos->bigPce[colour]++;
            if( PieceMin[piece] == TRUE) pos->minPce[colour]++;
            if( PieceMaj[piece] == TRUE) pos->majPce[colour]++;

            pos->material[colour] += PieceVal[piece];

            ASSERT(pos->pceNum[piece] < 10 && pos->pceNum[piece] >= 0);

            pos->pList[piece][pos->pceNum[piece]] = sq;
            pos->pceNum[piece]++;


            if(piece==wK) pos->KingSq[WHITE] = sq;
            if(piece==bK) pos->KingSq[BLACK] = sq;

            if(piece==wP) {
                SETBIT(pos->pawns[WHITE],SQ64(sq));
                SETBIT(pos->pawns[BOTH],SQ64(sq));
            } else if(piece==bP) {
                SETBIT(pos->pawns[BLACK],SQ64(sq));
                SETBIT(pos->pawns[BOTH],SQ64(sq));
            }
        }
    }
}

int ParseFen(char *fen, S_BOARD *pos) {

    ASSERT(fen!=NULL);
    ASSERT(pos!=NULL);

    int  rank = RANK_8;
    int  file = FILE_A;
    int  piece = 0;
    int  count = 0;
    int  i = 0;
    int  sq64 = 0;
    int  sq120 = 0;

    ResetBoard(pos);

    while ((rank >= RANK_1) && *fen) {
        count = 1;
        switch (*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;

            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;

            default:
                printf("FEN error \n");
                return -1;
        }

        for (i = 0; i < count; i++) {
            sq64 = rank * 8 + file;
            sq120 = SQ120(sq64);
            if (piece != EMPTY) {
                pos->pieces[sq120] = piece;
            }
            file++;
        }
        fen++;
    }

    ASSERT(*fen == 'w' || *fen == 'b');

    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen += 2;

    for (i = 0; i < 4; i++) {
        if (*fen == ' ') {
            break;
        }
        switch(*fen) {
            case 'K': pos->castlePerm |= WKCA; break;
            case 'Q': pos->castlePerm |= WQCA; break;
            case 'k': pos->castlePerm |= BKCA; break;
            case 'q': pos->castlePerm |= BQCA; break;
            default:	     break;
        }
        fen++;
    }
    fen++;

    ASSERT(pos->castlePerm>=0 && pos->castlePerm <= 15);

    if (*fen != '-') {
        file = fen[0] - 'a';
        rank = fen[1] - '1';

        ASSERT(file>=FILE_A && file <= FILE_H);
        ASSERT(rank>=RANK_1 && rank <= RANK_8);

        pos->enPas = FR2SQ(file,rank);
    }

    pos->posKey = GeneratePosKey(pos);

    UpdateListsMaterial(pos);

    return 0;
}

const char *printSquare(const unsigned sq) {
    //static char result[3];
    if (sq >= 120 || SqOnBoard(sq) == 0)
        return "-";
    static char SqStr[3];

    int file = FilesBrd[sq];
    int rank = RanksBrd[sq];

    sprintf(SqStr, "%c%c", ('a'+file), ('1'+rank));

    return SqStr;
    /*result[0] = 'a' + FilesBrd[SQ64(sq)];
    result[1] = '1' + RanksBrd[SQ64(sq)];
    result[2] = 0;
    return result;*/
}

const char *printFEN(const S_BOARD *pos) {
    int emptyCount = 0;
    int strIndex = 0;
    static char result[256];
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq120 = FR2SQ(file, rank);
            int piece = pos->pieces[sq120];
            if (piece == EMPTY) {
                emptyCount++;
            } else {
                if (emptyCount != 0) {
                    result[strIndex++] = '0' + emptyCount;
                    emptyCount = 0;
                }
                result[strIndex++] = PceChar[piece];
            }
        }
        if (emptyCount != 0) {
            result[strIndex++] = '0' + emptyCount;
            emptyCount = 0;
        }
        if (rank != RANK_1) {
            result[strIndex++] = '/';
        }
    }
    strIndex +=
            sprintf(result + strIndex, " %c ", pos->side == WHITE ? 'w' : 'b');
    if (pos->castlePerm == 0) {
        result[strIndex++] = '-';
    } else {
        if (pos->castlePerm & WKCA)
            result[strIndex++] = 'K';
        if (pos->castlePerm & WQCA)
            result[strIndex++] = 'Q';
        if (pos->castlePerm & BKCA)
            result[strIndex++] = 'k';
        if (pos->castlePerm & BQCA)
            result[strIndex++] = 'q';
    }
    // TODO: Check if pos->hisPly / 2 + 1 is the correct formula
    strIndex += sprintf(result + strIndex, " %s %d %d", printSquare(pos->enPas),
                        pos->fiftyMove, (pos->hisPly + 1)/2);
    ASSERT(strIndex < 256);
    return result;
}

void PrintBoard(const S_BOARD *pos) {

    int sq,file,rank,piece;

    printf("\nGame Board:\n\n");

    /*for(rank = RANK_8; rank >= RANK_1; rank--) {
        printf("%d  ",rank+1);
        for(file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file,rank);
            piece = pos->pieces[sq];
            printf("%3c",PceChar[piece]);
        }
        printf("\n");
    }*/


    for(rank = RANK_8; rank >= RANK_1; rank--) {
        printf("\t+---+---+---+---+---+---+---+---+\n");
        printf("%d\t",rank+1);
        for(file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file,rank);
            piece = pos->pieces[sq];
            printf("| %c ",PceChar[piece]);
        }
        printf("|\n");
    }
    printf("\t+---+---+---+---+---+---+---+---+\n");
    printf("\n\t");
    for(file = FILE_A; file <= FILE_H; file++) {
        printf("%3c ",'a'+file);
    }
    printf("\n\n");
    printf("side:%c\n",SideChar[pos->side]);
    printf("enPas:%s\n", printSquare(pos->enPas));
    printf("castle:%c%c%c%c\n",
           pos->castlePerm & WKCA ? 'K' : '-',
           pos->castlePerm & WQCA ? 'Q' : '-',
           pos->castlePerm & BKCA ? 'k' : '-',
           pos->castlePerm & BQCA ? 'q' : '-'
    );
    printf("PosKey:%llX\n",pos->posKey);
    //char currentFEN = printFEN(pos);
    printf("FEN: %s\n\n",printFEN(pos));
    WriteFenLog(printFEN(pos));
    if(EngineOptions->texLog == 1) {
        WriteTEX(printFEN(pos));
    };
    //WriteTEX(printFEN(pos));
}

void ResetBoard(S_BOARD *pos) {

    int index = 0;

    for(index = 0; index < BRD_SQ_NUM; ++index){
        pos->pieces[index] = OFFBOARD;
    }

    for(index = 0; index < 64; ++index) {
        pos->pieces[SQ120(index)] = EMPTY;
    }

    for(index = 0; index < 2; ++index){
        pos->bigPce[index] = 0;
        pos->majPce[index] = 0;
        pos->minPce[index] = 0;
        pos->material[index] = 0;
    }

    for(index = 0; index < 3; ++index){
        pos->pawns[index] = 0ULL;
    }

    for(index = 0; index < 13; ++index){
        pos->pceNum[index]=0;
    }

    pos->KingSq[WHITE] = pos->KingSq[BLACK] = NO_SQ;
    pos->side = BOTH;
    pos->enPas = NO_SQ;
    pos->fiftyMove = 0;
    pos->ply = 0;
    pos->hisPly = 0;
    pos->castlePerm = 0;
    pos->posKey = 0ULL;
}

void MirrorBoard(S_BOARD *pos) {

    int tempPiecesArray[64];
    int tempSide = pos->side^1;
    int SwapPiece[13] = { EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK };
    int tempCastlePerm = 0;
    int tempEnPas = NO_SQ;

    int sq;
    int tp;

    if (pos->castlePerm & WKCA) tempCastlePerm |= BKCA;
    if (pos->castlePerm & WQCA) tempCastlePerm |= BQCA;

    if (pos->castlePerm & BKCA) tempCastlePerm |= WKCA;
    if (pos->castlePerm & BQCA) tempCastlePerm |= WQCA;

    if (pos->enPas != NO_SQ)  {
        tempEnPas = SQ120(Mirror64[SQ64(pos->enPas)]);
    }

    for (sq = 0; sq < 64; sq++) {
        tempPiecesArray[sq] = pos->pieces[SQ120(Mirror64[sq])];
    }

    ResetBoard(pos);

    for (sq = 0; sq < 64; sq++) {
        tp = SwapPiece[tempPiecesArray[sq]];
        pos->pieces[SQ120(sq)] = tp;
    }

    pos->side = tempSide;
    pos->castlePerm = tempCastlePerm;
    pos->enPas = tempEnPas;

    pos->posKey = GeneratePosKey(pos);

    UpdateListsMaterial(pos);

    ASSERT(CheckBoard(pos));
}