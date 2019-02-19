//
// Created by Eric Silverman on 13/10/2018.
//

#include <stdio.h>
#include <string.h>
#include "defs.h"

char *PrSq(const int sq) {

    static char SqStr[3];

    int file = FilesBrd[sq];
    int rank = RanksBrd[sq];

    sprintf(SqStr, "%c%c", ('a'+file), ('1'+rank));

    return SqStr;
}

char *PrMove(const int move) {

    static char MvStr[6];

    int ff = FilesBrd[FROMSQ(move)];
    int rf = RanksBrd[FROMSQ(move)];
    int ft = FilesBrd[TOSQ(move)];
    int rt = RanksBrd[TOSQ(move)];

    int promoted = PROMOTED(move);

    if(promoted) {
        char pchar = 'q';
        if(IsKn(promoted)) {
            pchar = 'n';
        } else if(IsRQ(promoted) && !IsBQ(promoted))  {
            pchar = 'r';
        } else if(!IsRQ(promoted) && IsBQ(promoted))  {
            pchar = 'b';
        }
        sprintf(MvStr, "%c%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt), pchar);
    } else {
        sprintf(MvStr, "%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt));
    }

    return MvStr;
}

char *PrMoveSAN(S_BOARD *pos,  int move) {

    static char sanStr[8];

    int ff = FilesBrd[FROMSQ(move)];
    int rf = RanksBrd[FROMSQ(move)];
    int ft = FilesBrd[TOSQ(move)];
    int rt = RanksBrd[TOSQ(move)];
    int to = TOSQ(move);
    int from = FROMSQ(move);

    int promoted = PROMOTED(move);
    int capture = CAPTURED(move);
    int castle = move & MFLAGCA;
    int opponent;
    int translated = 0;

    char pchar;

    // Who's the opponent?
    if(pos->side == WHITE) {
        opponent = BLACK;
    } else {
        opponent = WHITE;
    };

    // Convert null moves to SAN
    if (move == NOMOVE) {
        strcpy(sanStr, "null");
        return sanStr;
    }
    do {
        // Check for castling moves
        if (castle && (pos->pieces[from] == wK || pos->pieces[from] == bK)) {
            if (pos->side == WHITE) {
                if (to == C1)
                    strcpy(sanStr, "O-O-O");
                else
                    strcpy(sanStr, "O-O");
            } else if (pos->side == BLACK) {
                if (to == C8)
                    strcpy(sanStr, "O-O-O");
                else
                    strcpy(sanStr, "O-O");
            }
            break;
        }

        // Generate chars for promoted pieces
        if (promoted) {
            pchar = 'Q';
            if (IsKn(promoted)) {
                pchar = 'N';
            } else if (IsRQ(promoted) && !IsBQ(promoted)) {
                pchar = 'R';
            } else if (!IsRQ(promoted) && IsBQ(promoted)) {
                pchar = 'B';
            }
        }

        // Get the piece character
        char piecename;
        if (pos->pieces[from] == wP) {
            piecename = 'P';
        } else if (pos->pieces[from] == wN) {
            piecename = 'N';
        } else if (pos->pieces[from] == wB) {
            piecename = 'B';
        } else if (pos->pieces[from] == wR) {
            piecename = 'R';
        } else if (pos->pieces[from] == wQ) {
            piecename = 'Q';
        } else if (pos->pieces[from] == wK) {
            piecename = 'K';
        } else if (pos->pieces[from] == bP) {
            piecename = 'P';
        } else if (pos->pieces[from] == bN) {
            piecename = 'N';
        } else if (pos->pieces[from] == bB) {
            piecename = 'B';
        } else if (pos->pieces[from] == bR) {
            piecename = 'R';
        } else if (pos->pieces[from] == bQ) {
            piecename = 'Q';
        } else if (pos->pieces[from] == bK) {
            piecename = 'K';
        }

        // Check for ambiguity
        S_MOVELIST list[1];
        GenerateAllMoves(pos, list);
        int moves = 0;
        int col = 0;
        int row = 0;
        int usecol = 0;
        for (moves = 0; moves < list->count; ++moves) {
            int candidate = list->moves[moves].move;
            if (to == TOSQ(candidate) && from != FROMSQ(candidate) && pos->pieces[from] == pos->pieces[FROMSQ(candidate)]) {
                //printf("Possible ambiguous moves: %s    %s\n", PrMove(move), PrMove(candidate));
                if (ff == FilesBrd[FROMSQ(candidate)]) {
                    col = 1;
                    break;
                    //printf("Pieces on same file!!!\n\n");
                } else if (from != FROMSQ(candidate) && rf == RanksBrd[FROMSQ(candidate)]) {
                    row = 1;
                    break;
                    //printf("Pieces on same rank!!!\n\n");
                } else {
                    usecol = 1;
                    //printf("Pieces not on same rank or file!!!\n\n");
                    break;
                }
            }
        }

        //printf("Col Row Usecol: %d %d %d", col, row, usecol);

        // Translate moves
        if (pos->pieces[from] == wP || pos->pieces[from] == bP) {
            if (!capture && !promoted) {
                sprintf(sanStr, "%c%c", ('a' + ft), ('1' + rt));
            } else if (capture && !promoted) {
                sprintf(sanStr, "%cx%c%c", ('a' + ff), ('a' + ft), ('1' + rt));
            } else if (!capture && promoted) {
                sprintf(sanStr, "%c%c=%c", ('a' + ft), ('1' + rt), pchar);
            } else if (capture && promoted) {
                sprintf(sanStr, "%cx%c%c=%c", ('a' + ff), ('a' + ft), ('1' + rt), pchar);
            }
            //break;
        } else if (!capture && col == 0 && row == 0 && usecol == 0 && piecename != 'P') {
            sprintf(sanStr, "%c%c%c", piecename, ('a' + ft), ('1' + rt));
        } else if (capture && col == 0 && row == 0 && usecol == 0 && piecename != 'P') {
            sprintf(sanStr, "%cx%c%c", piecename, ('a' + ft), ('1' + rt));
        } else if (!capture && col == 1 && row == 0 && usecol == 0 && piecename != 'P') {
            sprintf(sanStr, "%c%c%c%c", piecename, ('1' + rf), ('a' + ft), ('1' + rt));
        } else if (capture && col == 1 && row == 0 && usecol == 0 && piecename != 'P') {
            sprintf(sanStr, "%c%cx%c%c", piecename, ('1' + rf), ('a' + ft), ('1' + rt));
        } else if (!capture && col == 0 && row == 1 && usecol == 0 && piecename != 'P') {
            sprintf(sanStr, "%c%c%c%c", piecename, ('a' + ff), ('a' + ft), ('1' + rt));
        } else if (capture && col == 0 && row == 1 && usecol == 0 && piecename != 'P') {
            sprintf(sanStr, "%c%cx%c%c", piecename, ('a' + ff), ('a' + ft), ('1' + rt));
        } else if (capture && col == 0 && row == 0 && usecol == 1 && piecename != 'P') {
            sprintf(sanStr, "%c%cx%c%c", piecename, ('a' + ff), ('a' + ft), ('1' + rt));
        } else if (!capture && col == 0 && row == 0 && usecol == 1 && piecename != 'P') {
            sprintf(sanStr, "%c%c%c%c", piecename, ('a' + ff), ('a' + ft), ('1' + rt));
        }

        /*//Check check/mate/stalemate
        S_MOVELIST list2[1];
        int moveNum = 0;
        int found = 0;
        int check = 0;
        int mate = 0;
        int stale = 0;
        for (moveNum = 0; moveNum < list2->count; ++moveNum) {
            if (MakeMove(pos, list2->moves[moves].move)) {
                found++;
                check = SqAttacked(pos->KingSq[opponent], opponent ^ 1, pos);
                TakeMove(pos);
            }
        }

        if (found == 0 && check == 0) {
            stale = 1;
        } else if (found == 0 && check == 1) {
            mate = 1;
        } else if (found != 0 && check == 1) {
            check = 1;
        }
        printf("Stale Check Checkmate: %d %d %d", stale, check, mate);

        int len = strlen(sanStr);
        if (found != 0 && check == TRUE) {
            //sanStr[7] = '+';
            sanStr[len] = '+';
            sanStr[len+1] = '\0';

        } else if (found == 0 && check == TRUE) {
            //sanStr[7] = '#';
            sanStr[len] = '#';
            sanStr[len+1] = '\0';
        } else if (found == 0 && check == FALSE) {
            printf("Stalemated!");
        }*/
        int len = strlen(sanStr);
        int result = CheckEnd(pos, move);
        if (result == 1) {
            sanStr[len] = '#';
            sanStr[len+1] = '\0';
        } else if (result == 2) {
            sanStr[len] = '#';
            sanStr[len+1] = '\0';
        } else if (result == 3) {
            sanStr[len] = '+';
            sanStr[len+1] = '\0';
        }

        translated = 1;
    } while (translated == 0);

    return sanStr;
}

int CheckEnd(S_BOARD *pos, int move) {
    ASSERT(CheckBoard(pos));
    MakeMove(pos, move);

    if (pos->fiftyMove > 100) {
        //printf("1/2-1/2 {fifty move rule (claimed by SpaceDog)}\n"); return TRUE;
        TakeMove(pos);
        return 0;
    }

    if (ThreeFoldRep(pos) >= 2) {
        //printf("1/2-1/2 {3-fold repetition (claimed by SpaceDog)}\n");
        TakeMove(pos);
        return 0;
    }

    if (DrawMaterial(pos) == TRUE) {
        //printf("1/2-1/2 {insufficient material (claimed by SpaceDog)}\n");
        TakeMove(pos);
        return 0;
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);

    int MoveNum = 0;
    int found = 0;
    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        found++;
        TakeMove(pos);
        break;
    }

    int InCheck = SqAttacked(pos->KingSq[pos->side],pos->side^1,pos);

    if(found != 0 && InCheck == TRUE) {
        TakeMove(pos);
        return 3;
    }

    if(InCheck == TRUE)	{
        if(pos->side == WHITE) {
            //printf("0-1 {black mates (claimed by SpaceDog)}\n");
            TakeMove(pos);
            return 1;
        } else {
            //printf("1-0 {white mates (claimed by SpaceDog)}\n");
            TakeMove(pos);
            return 2;
        }
    } else {
        //printf("\n1/2-1/2 {stalemate (claimed by SpaceDog)}\n");
        TakeMove(pos);
        return 0;
    }
}

int ParseMove(char *ptrChar, S_BOARD *pos) {

    ASSERT(CheckBoard(pos));

    if(ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
    if(ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
    if(ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
    if(ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

    int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

    ASSERT(SqOnBoard(from) && SqOnBoard(to));

    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
    int MoveNum = 0;
    int Move = 0;
    int PromPce = EMPTY;

    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        Move = list->moves[MoveNum].move;
        if(FROMSQ(Move)==from && TOSQ(Move)==to) {
            PromPce = PROMOTED(Move);
            if(PromPce!=EMPTY) {
                if(IsRQ(PromPce) && !IsBQ(PromPce) && ptrChar[4]=='r') {
                    return Move;
                } else if(!IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4]=='b') {
                    return Move;
                } else if(IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4]=='q') {
                    return Move;
                } else if(IsKn(PromPce)&& ptrChar[4]=='n') {
                    return Move;
                }
                continue;
            }
            return Move;
        }
    }

    return NOMOVE;
}

int ParseMoveTB(char *ptrChar, S_BOARD *pos) {

    ASSERT(CheckBoard(pos));

    if(ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
    if(ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
    if(ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
    if(ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

    int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

    ASSERT(SqOnBoard(from) && SqOnBoard(to));

    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
    int MoveNum = 0;
    int Move = 0;

    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        Move = list->moves[MoveNum].move;
        if(FROMSQ(Move)==from && TOSQ(Move)==to) {
                if(ptrChar[4]=='r') {
                    return Move;
                } else if(ptrChar[4]=='b') {
                    return Move;
                } else if(ptrChar[4]=='q') {
                    return Move;
                } else if(ptrChar[4]=='n') {
                    return Move;
                }
                continue;
            }
            return Move;
        }
        return NOMOVE;
    }

void PrintMoveList(const S_MOVELIST *list) {
    int index = 0;
    int score = 0;
    int move = 0;
    printf("MoveList:\n");

    for(index = 0; index < list->count; ++index) {

        move = list->moves[index].move;
        score = list->moves[index].score;

        printf("Move:%d > %s (score:%d)\n",index+1,PrMove(move),score);
    }
    printf("MoveList Total %d Moves:\n\n",list->count);
}

void WriteFenLog(char *fen) {
    FILE *fp;

    fp=fopen(FEN_LOG_FILE, "a");
    if(fp == NULL)
        exit(-1);
    fprintf(fp, "%s\n", fen);
    fclose(fp);
}

void InitTEX() {
    FILE *fp;
    fp=fopen(TEX_GAME_LOG, "a");
    if(fp == NULL)
        exit(-1);
    fprintf(fp, "\\documentclass{article}\n");
    fprintf(fp, "\\usepackage[utf8]{inputenc}\n");
    fprintf(fp, "\\usepackage[english]{babel}\n");
    fprintf(fp, "\\usepackage{skak}\n");
    fprintf(fp, "\\usepackage{geometry}\n");
    fprintf(fp, "\\geometry{textheight = 22.5cm}\n");
    fprintf(fp, "\\begin{document}\n");
    fprintf(fp, "{\\huge Visual game record generated by SpaceDog!}\n\n");
    fprintf(fp, "\\medskip\n\n");
    fprintf(fp, "\\newgame\n\n");
    fclose(fp);
};

void NewTEX() {
    FILE *fp;
    fp=fopen(TEX_GAME_LOG, "a");
    if(fp == NULL)
        exit(-1);
    fprintf(fp, "\\documentclass[a4paper]{article}\n");
    fprintf(fp, "\\usepackage[utf8]{inputenc}\n");
    fprintf(fp, "\\usepackage[english]{babel}\n");
    fprintf(fp, "\\usepackage{skak,chessboard}\n");
    fprintf(fp, "\\usepackage{geometry}\n");
    fprintf(fp, "\\geometry{textheight = 22.5cm}\n");
    fprintf(fp, "\\pagestyle{headings}\n");
    fprintf(fp, "\\markright{Diagrammed game record generated by SpaceDog}\n");
    fprintf(fp, "\\begin{document}\n");
    fprintf(fp, "\\centering\n");
    fprintf(fp, "\\medskip\n\n");
    fprintf(fp, "\\newgame\n\n");
    fclose(fp);
};

void NewWriteTEX(char *fen, char *move, char *san, int ply) {
    FILE *fp;
    int turn = (ply + 1)/2;
    fp=fopen(TEX_GAME_LOG, "a");
    if(fp == NULL)
        exit(-1);
    //fprintf(fp, "\\emph{Ply %d}: %s (%s)\n\n", ply, move, san);
    if(ply % 2 == 1) {
        fprintf(fp, "\\mainline{%d.%s}\n", turn, san);
    } else if(ply % 2 == 0) {
        fprintf(fp, "\\mainline{%d...%s}\n", turn, san);
    }
    fprintf(fp, "\\chessboard[largeboard]\n\n");
    fprintf(fp, "%s\n\n", fen);
    fprintf(fp, "\\bigskip\n\n");
    fclose(fp);
}

void InitSummary() {
    FILE *fp;
    fp=fopen(SAN_SUMMARY, "a");
    if(fp == NULL)
        exit(-1);
    fprintf(fp, "\\documentclass[a4paper]{article}\n");
    fprintf(fp, "\\usepackage[utf8]{inputenc}\n");
    fprintf(fp, "\\usepackage[english]{babel}\n");
    fprintf(fp, "\\usepackage{xskak}\n");
    fprintf(fp, "\\usepackage{geometry}\n");
    fprintf(fp, "\\usepackage{pifont, graphics, amssymb}\n");
    fprintf(fp, "\\geometry{textheight = 22cm}\n");
    fprintf(fp, "\\begin{document}\n");
    fprintf(fp, "\\newcommand\\getmovestyle[1]{\n");
    fprintf(fp, "\\ifthenelse\n");
    fprintf(fp, "{\\equal{#1}{N}}\n");
    fprintf(fp, "{\\def\\mymovestyle{[clockwise=false,style=knight]curvemove}}\n");
    fprintf(fp, "{\\ifthenelse\n");
    fprintf(fp, "{\\equal{#1}{}}\n");
    fprintf(fp, "{\\def\\mymovestyle{curvemove}}\n");
    fprintf(fp, "{\\def\\mymovestyle{straightmove}}}}\n");
    fprintf(fp, "{\\huge Game summary generated by SpaceDog}\n\n");
    fprintf(fp, "\\bigskip\n\n");
    fprintf(fp, "\\bigskip\n\n");
    fprintf(fp, "\\newchessgame\n\n");
    fprintf(fp, "\\mainline{");
    fclose(fp);
};

void EndSummary() {
    FILE *fp;
    fp=fopen(SAN_SUMMARY, "a");
    if(fp == NULL)
        exit(-1);
    fprintf(fp, "}\n");
    fprintf(fp, "\\medskip\n\n");
    fprintf(fp, "\\begin{center}\n");
    fprintf(fp, "\\xskakloop[step=1]{\n");
    fprintf(fp, "\\getmovestyle{\\xskakget{piecechar}}\n");
    fprintf(fp, "\\begin{tabular}{c}\n");
    fprintf(fp, "\\chessboard[pgfstyle=\\mymovestyle, color=blue, markmoves=\\xskakget{move}, pgfshortenend=0.3em, arrow=to, coloremph, color=red, markstyle=circle, markfield=\\xskakget{moveto},\n"
                "        emphfields=\\xskakget{moveto}, smallboard,setfen=\\xskakget{nextfen}]\\\\\n");
    fprintf(fp, "\\xskakget{opennr}\n");
    fprintf(fp, "\\xskakget{san}\n");
    fprintf(fp, "\\end{tabular}\\quad}\n");
    fprintf(fp, "\\end{center}\n");
    fprintf(fp, "\\end{document}\n");
    fclose(fp);
};

void WriteTEX(char *fen) {
    FILE *fp;
    fp=fopen(TEX_GAME_LOG, "a");
    if(fp == NULL)
        exit(-1);
    fprintf(fp, "%s\n\n", fen);
    fprintf(fp, "\\fenboard{%s}\n\n", fen);
    fprintf(fp, "\\showboard\n\n");
    fprintf(fp, "\\bigskip\n\n");
    fclose(fp);
}

void WritePV(const S_BOARD *pos, int depth) {
    int pvMoves, pvNum;
    FILE *fp;
    fp=fopen(TEX_GAME_LOG,"a");
    pvMoves = GetPvLine(depth, pos);
    fprintf(fp, "\\emph{PV:} ");
    for(pvNum=0; pvNum < pvMoves; ++pvNum) {
        fprintf(fp, "%s ", PrMove(pos->PvArray[pvNum]));
    };
    fprintf(fp, "\n\n");
    fclose(fp);
};

void WriteMoveTEX(char *move, int ply) {
    FILE *fp;
    fp=fopen(TEX_GAME_LOG, "a");
    if(fp == NULL)
        exit(-1);
    fprintf(fp, "\\emph{Ply %d:} %s\n\n", ply, move);
    fclose(fp);
};

void InitSanLog() {
    FILE *fp;
    fp=fopen(SAN_GAME_LOG, "a");
    if(fp == NULL)
        exit(-1);
    fprintf(fp, "[Event \"?\"]\n");
    fprintf(fp, "[Site \"?\"]\n");
    fprintf(fp, "[Date \"?\"]\n");
    fprintf(fp, "[Round \"?\"]\n");
    fprintf(fp, "[White \"?\"]\n");
    fprintf(fp, "[WhiteElo \"?\"]\n");
    fprintf(fp, "[Black \"?\"]\n");
    fprintf(fp, "[BlackElo \"?\"]\n");
    fprintf(fp, "[Result \"?\"]\n");
    fprintf(fp, "\n");
    fclose(fp);
};

void SanLog(char *move, int sideToMove, int ply) {
    FILE *fp;
    fp=fopen(SAN_GAME_LOG, "a");
    if(fp == NULL)
        exit(-1);
    int turn = (ply + 1)/2 + 1;
    if (sideToMove == WHITE) {
        fprintf(fp, "%d. %s", turn, move);
    } else {
        fprintf(fp, " %s ", move);
    }
    fclose(fp);
};

void GameSummary(char *move, int sideToMove, int ply) {
    FILE *fs;
    fs=fopen(SAN_SUMMARY, "a");
    if(fs == NULL)
        exit(-1);
    int turn = (ply + 1)/2 + 1;
    if (sideToMove == WHITE) {
        fprintf(fs, "%d. %s", turn, move);
    } else {
        fprintf(fs, " %s ", move);
    }
    fclose(fs);
};

void EndTEX() {
    FILE *fp;
    fp=fopen(TEX_GAME_LOG, "a");
    if(fp == NULL)
        exit(-1);
    fprintf(fp, "\\end{document}\n");
    fclose(fp);
};