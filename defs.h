//
// Created by Eric Silverman on 12/10/2018.
//

#ifndef VICE_CHESS_DEFS_H
#define VICE_CHESS_DEFS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
//#include <jmorecfg.h>

#define MAX_HASH 1024

//#define DEBUG
#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed ", #n); \
printf("On %s ", __DATE__); \
printf("At %s ", __TIME__); \
printf("In File %s ", __FILE__); \
printf("At Line %d\n", __LINE__); \
exit(1);}
#endif

typedef unsigned long long U64;

#define NAME "SpaceDog 1.0"
#define BRD_SQ_NUM 120

#define MAXGAMEMOVES 2048
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64

#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN_LOG_FILE "fens.txt"
#define TEX_GAME_LOG "game_record.tex"
#define SAN_GAME_LOG "game_record.pgn"
#define SAN_SUMMARY "game_summary.tex"
#define TB_PATH "syzygy/"

#define INFINITE 30000
#define ISMATE (INFINITE - MAXDEPTH)
#define TBWIN 20000



enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

enum { WHITE, BLACK, BOTH };
enum { UCIMODE, XBOARDMODE, CONSOLEMODE };

enum{
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

enum { FALSE, TRUE };

enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

typedef struct {
    int move;
    int score;
} S_MOVE;

typedef struct {
    S_MOVE moves[MAXPOSITIONMOVES];
    int count;
} S_MOVELIST;

enum {  HFNONE, HFALPHA, HFBETA, HFEXACT};

typedef struct {
    U64 posKey;
    int move;
    int score;
    int depth;
    int flags;
} S_HASHENTRY;

typedef struct {
    S_HASHENTRY *pTable;
    int numEntries;
    int newWrite;
    int overWrite;
    int hit;
    int cut;
} S_HASHTABLE;

typedef struct {
    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    U64 posKey;
} S_UNDO;

// Board representation structure

typedef struct {
    int pieces[BRD_SQ_NUM];
    U64 pawns[3];
    int KingSq[2];
    int side;
    int enPas;
    int fiftyMove;
    int ply;
    int hisPly;

    int castlePerm;

    //int texLog;

    U64 posKey;

    int pceNum[13];
    int bigPce[2];
    int majPce[2];
    int minPce[2];
    int material[2];

    S_UNDO history[MAXGAMEMOVES];

    // Piece list
    int pList[13][10];

    S_HASHTABLE HashTable[1];
    int PvArray[MAXDEPTH];

    int searchHistory[13][BRD_SQ_NUM];
    int searchKillers[2][MAXDEPTH];

} S_BOARD;

typedef struct {
    int starttime;
    int stoptime;
    int depth;
    int depthset;
    int timeset;
    int movestogo;
    int infinite;

    long nodes;
    long tbhits;

    int quit;
    int stopped;

    float fh;
    float fhf;
    int nullCut;
    long pruned;
    long lmr;

    int GAME_MODE;
    int POST_THINKING;

} S_SEARCHINFO;

typedef struct {
    int UseBook;
    char BookName[50];
    int texLog;
    int newTexLog;
    int SanMode;
    int summary;
    int use_TBs;
    char EGTB_PATH[50];
    int TB_PROBE_DEPTH;
} S_OPTIONS;

// eval vector
typedef struct {
    int matScore;
    int tempo;
    int phase;
    int pawns[2];
    int knights[2];
    int bishops[2];
    int rooks[2];
    int queens[2];
    int kings[2];
    int pairs[2];
} S_EVAL;

/* GAME MOVE */

/*
 * 0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
 * 0000 0000 0000 0011 1111 1000 0000 -> To >> 7 0x7F
 * 0000 0000 0011 1100 0000 0000 0000 -> Captured >> 14 0xF
 * 0000 0000 0100 0000 0000 0000 0000 -> En Passant 0x40000
 * 0000 0000 1000 0000 0000 0000 0000 -> Pawn Start 0x80000
 * 0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece >> 20 0xF
 * 0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000
 * */

#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m)>>7) & 0x7F)
#define CAPTURED(m) (((m)>>14) & 0xF)
#define PROMOTED(m) (((m)>>20) & 0xF)

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000

#define MFLAGCAP 0x7C000
#define MFLAGPROM 0xF00000

#define NOMOVE 0

/* MACROS */

#define FR2SQ(f, r)  ( (21 + (f) ) + ( (r) * 10 ) )
#define SQ64(sq120) (Sq120ToSq64[sq120])
#define SQ120(sq64) (Sq64ToSq120[(sq64)])
#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)
#define CLRBIT(bb,sq) ((bb) &= ClearMask[(sq)])
#define SETBIT(bb,sq) ((bb) |= SetMask[(sq)])

#define IsBQ(p) (PieceBishopQueen[(p)])
#define IsRQ(p) (PieceRookQueen[(p)])
#define IsKn(p) (PieceKnight[(p)])
#define IsKi(p) (PieceKing[(p)])

#define MIRROR64(sq) (Mirror64[(sq)])

/* GLOBALS */

extern int Sq120ToSq64[BRD_SQ_NUM];
extern int Sq64ToSq120[64];
extern U64 SetMask[64];
extern U64 ClearMask[64];
extern U64 PieceKeys[13][120];
extern U64 SideKey;
extern U64 CastleKeys[16];
extern char PceChar[];
extern char SideChar[];
extern char RankChar[];
extern char FileChar[];
extern int PieceBig[13];
extern int PieceMaj[13];
extern int PieceMin[13];
extern int PieceVal[13];
extern int PieceCol[13];
extern int PiecePawn[13];

extern int PieceSlides[13];

extern int Mirror64[64];

extern int FilesBrd[BRD_SQ_NUM];
extern int RanksBrd[BRD_SQ_NUM];

extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];

extern U64 FileBBMask[8];
extern U64 RankBBMask[8];

extern U64 BlackPassedMask[64];
extern U64 WhitePassedMask[64];
extern U64 IsolatedMask[64];

extern S_OPTIONS EngineOptions[1];

/* FUNCTIONS */


// init.c
extern void AllInit();
extern void InitFilesRanksBrd();
extern void InitTBs(char *tbPath);


// bitboards.c
extern void PrintBitBoard(U64 bb);
extern int PopBit(U64 *bb);
extern int CountBits(U64 b);

// hashkeys.c
extern U64 GeneratePosKey(const S_BOARD *pos);

// board.c
extern int ParseFen(char *fen, S_BOARD *pos);
extern void ResetBoard(S_BOARD *pos);
extern const char *printSquare(const unsigned sq);
extern char *printFEN(const S_BOARD *pos);
extern void PrintBoard(const S_BOARD *pos);
extern void UpdateListsMaterial(S_BOARD *pos);
extern int CheckBoard(const S_BOARD *pos);
extern void MirrorBoard(S_BOARD *pos);

// attack.c
extern int SqAttacked(const int sq, const int side, const S_BOARD *pos);

// io.c
extern char *PrSq(const int sq);
extern char *PrMove(const int move);
char *PrMoveSAN(S_BOARD *pos,  int move);
int CheckEnd(S_BOARD *pos, int move);
extern void PrintMoveList(const S_MOVELIST *list);
extern int ParseMove(char *ptrChar, S_BOARD *pos);
extern int CheckMove(S_BOARD *pos, int move);
extern int ParseMoveTB(char *ptrChar, S_BOARD *pos);
extern void WriteFenLog(char *fen);
extern void InitTEX();
extern void NewTEX();
extern void NewWriteTEX(const char *fen, char *move, char *san, int ply);
extern void InitSummary();
extern void EndSummary();
extern void WriteTEX(char *fen);
extern void WriteMoveTEX(char *move, int ply);
extern void InitSanLog();
extern void SanLog(char *move, int sideToMove, int ply);
extern void GameSummary(char *move, int sideToMove, int ply);
extern void EndTEX();

// validate.c
extern int SqOnBoard(const int sq);
extern int SideValid(const int side);
extern int FileRankValid(const int fr);
extern int PieceValidEmpty(const int pce);
extern int PieceValid(const int pce);
extern void MirrorEvalTest(S_BOARD *pos);
extern int SqIs120(const int sq);
extern int PceValidEmptyOffbrd(const int pce);
extern int MoveListOk(const S_MOVELIST *list,  const S_BOARD *pos);
extern void DebugAnalysisTest(S_BOARD *pos, S_SEARCHINFO *info);

// movegen.c
extern void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list);
extern void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list);
extern int MoveExists(S_BOARD *pos, const int move);
extern void InitMvvLva();

// makemove.c
extern int MakeMove(S_BOARD *pos, int move);
extern void TakeMove(S_BOARD *pos);
extern void MakeNullMove(S_BOARD *pos);
extern void TakeNullMove(S_BOARD *pos);

// perft.c
extern void PerftTest(int depth, S_BOARD *pos);

// search.c
extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);

// misc.c
extern int GetTimeMs();
extern void ReadInput(S_SEARCHINFO *info);

// pvtable.c
extern void InitHashTable(S_HASHTABLE *table, const int MB);
extern void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth);
extern int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth);
extern int ProbePvMove(const S_BOARD *pos);
extern int GetPvLine(const int depth, S_BOARD *pos);
extern void ClearHashTable(S_HASHTABLE *table);

// evaluate.c/eval.c
extern int EvalPosition(const S_BOARD *pos);
extern void MirrorEvalTest(S_BOARD *pos);

// uci.c
extern void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info);

// xboard.c
extern void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info);
extern void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info);
extern int checkresult(S_BOARD *pos);
extern int ThreeFoldRep(const S_BOARD *pos);
extern int DrawMaterial(const S_BOARD *pos);

// polybook.c
extern void ListBookMoves(U64 polyKey);
extern int GetBookMove(S_BOARD *board);
extern void InitPolyBook();
extern void CleanPolyBook();

#endif //VICE_CHESS_DEFS_H
