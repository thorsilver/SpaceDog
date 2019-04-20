//
// Created by Eric Silverman on 16/04/2019.
//

#ifndef SPACEDOG_FRC_ENDGAME_H
#define SPACEDOG_FRC_ENDGAME_H

#endif //SPACEDOG_FRC_ENDGAME_H

#include "defs.h"

/*static const int sq_colors[64] =
        {
                WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK,
                BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE,
                WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK,
                BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE,
                WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK,
                BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE,
                WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK,
                BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE
        };*/

static const int sq_colors[64] = {
        BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE,
        WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK,
        BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE,
        WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK,
        BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE,
        WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK,
        BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE,
        WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK
};

/*
 *  Board geometry
 */

enum file { fileA, fileB, fileC, fileD, fileE, fileF, fileG, fileH };
enum rank { rank1, rank2, rank3, rank4, rank5, rank6, rank7, rank8 };

#define rank(square)       ((square) >> 3)
#define file(square)       ((square) & 7)
#define square(file, rank) ((file) + ((rank) << 3))

enum square {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,
    boardSize
};

static const unsigned int mirror_board[] = {
        a8, b8, c8, d8, e8, f8, g8, h8,
        a7, b7, c7, d7, e7, f7, g7, h7,
        a6, b6, c6, d6, e6, f6, g6, h6,
        a5, b5, c5, d5, e5, f5, g5, h5,
        a4, b4, c4, d4, e4, f4, g4, h4,
        a3, b3, c3, d3, e3, f3, g3, h3,
        a2, b2, c2, d2, e2, f2, g2, h2,
        a1, b1, c1, d1, e1, f1, g1, h1
};

/*enum file { fileA, fileB, fileC, fileD, fileE, fileF, fileG, fileH };
enum rank { rank1, rank2, rank3, rank4, rank5, rank6, rank7, rank8 };

#define file(square)       ((square) >> 3)
#define rank(square)       ((square) & 7)
#define square(file, rank) (((file) << 3) + (rank))

enum square {
    a1, a2, a3, a4, a5, a6, a7, a8,
    b1, b2, b3, b4, b5, b6, b7, b8,
    c1, c2, c3, c4, c5, c6, c7, c8,
    d1, d2, d3, d4, d5, d6, d7, d8,
    e1, e2, e3, e4, e5, e6, e7, e8,
    f1, f2, f3, f4, f5, f6, f7, f8,
    g1, g2, g3, g4, g5, g6, g7, g8,
    h1, h2, h3, h4, h5, h6, h7, h8,
    boardSize
};*/

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

/*
 *  Probe a KPK position from the in memory endgame table.
 *  Returns 0 for draw, 1 for win and -1 for loss.
 *
 *  The position must be legal for meaningful results.
 *  `side' is 0 for white to move and 1 for black to move.
 *
 *  If the table has not been generated yet, this will be
 *  done automatically at the first invocation.
 */
int kpkProbe(int side, int wKing, int wPawn, int bKing);

/*
 *  Explicitly generate the KPK table.
 *  Returns the memory size for info.
 *  This can take up to 2 milliseconds on a 2.6GHz Intel i7.
 */
int kpkGenerate(void);

/*
 *  Perform a self check on the bitbase.
 *  Returns 0 on failure, 1 for success.
 */
int kpkSelfCheck(void);

int evalBlackKPK(S_BOARD *pos);

int evalWhiteKPK(S_BOARD *pos);

int evalKPK(S_BOARD *pos, unsigned int strongSide);

int evalKXK(S_BOARD *pos, unsigned strongSide, int piece);

int evalKBNK(const S_BOARD *pos, int strongSide, int bish_sq);