//
// Created by Eric Silverman on 09/02/2019.
//

#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syzygy.h"
#include "fathom/src/tbprobe.h"
//#include "defs.h"

#define BOARD_RANK_1            0x00000000000000FFull
#define BOARD_FILE_A            0x8080808080808080ull
#define square(r, f)            (8 * (r) + (f))
#define rank(s)                 ((s) >> 3)
#define file(s)                 ((s) & 0x07)
#define board(s)                ((uint64_t)1 << (s))

static const char *wdl_to_str[5] =
        {
                "0-1",
                "1/2-1/2",
                "1/2-1/2",
                "1/2-1/2",
                "1-0"
        };



/*
 * Parse a FEN string.
 */
static bool parse_FEN_TB(struct TBpos *TBpos, const char *fen)
{
    uint64_t white = 0, black = 0;
    uint64_t kings, queens, rooks, bishops, knights, pawns;
    kings = queens = rooks = bishops = knights = pawns = 0;
    bool turn;
    unsigned rule50 = 0, move = 1;
    unsigned ep = 0;
    unsigned castling = 0;
    char c;
    int r, f;

    if (fen == NULL)
        goto fen_parse_error;

    for (r = 7; r >= 0; r--)
    {
        for (f = 0; f <= 7; f++)
        {
            unsigned s = (r * 8) + f;
            uint64_t b = board(s);
            c = *fen++;
            switch (c)
            {
                case 'k':
                    kings |= b;
                    black |= b;
                    continue;
                case 'K':
                    kings |= b;
                    white |= b;
                    continue;
                case 'q':
                    queens |= b;
                    black |= b;
                    continue;
                case 'Q':
                    queens |= b;
                    white |= b;
                    continue;
                case 'r':
                    rooks |= b;
                    black |= b;
                    continue;
                case 'R':
                    rooks |= b;
                    white |= b;
                    continue;
                case 'b':
                    bishops |= b;
                    black |= b;
                    continue;
                case 'B':
                    bishops |= b;
                    white |= b;
                    continue;
                case 'n':
                    knights |= b;
                    black |= b;
                    continue;
                case 'N':
                    knights |= b;
                    white |= b;
                    continue;
                case 'p':
                    pawns |= b;
                    black |= b;
                    continue;
                case 'P':
                    pawns |= b;
                    white |= b;
                    continue;
                default:
                    break;
            }
            if (c >= '1' && c <= '8')
            {
                unsigned jmp = (unsigned)c - '0';
                f += jmp-1;
                continue;
            }
            goto fen_parse_error;
        }
        if (r == 0)
            break;
        c = *fen++;
        if (c != '/')
            goto fen_parse_error;
    }
    c = *fen++;
    if (c != ' ')
        goto fen_parse_error;
    c = *fen++;
    if (c != 'w' && c != 'b')
        goto fen_parse_error;
    turn = (c == 'w');
    c = *fen++;
    if (c != ' ')
        goto fen_parse_error;
    c = *fen++;
    if (c != '-')
    {
        do
        {
            switch (c)
            {
                case 'K':
                    castling |= TB_CASTLING_K; break;
                case 'Q':
                    castling |= TB_CASTLING_Q; break;
                case 'k':
                    castling |= TB_CASTLING_k; break;
                case 'q':
                    castling |= TB_CASTLING_q; break;
                default:
                    goto fen_parse_error;
            }
            c = *fen++;
        }
        while (c != ' ');
        fen--;
    }
    c = *fen++;
    if (c != ' ')
        goto fen_parse_error;
    c = *fen++;
    if (c >= 'a' && c <= 'h')
    {
        unsigned file = c - 'a';
        c = *fen++;
        if (c != '3' && c != '6')
            goto fen_parse_error;
        unsigned rank = c - '1';
        ep = square(rank, file);
        if (rank == 2 && turn)
            goto fen_parse_error;
        if (rank == 5 && !turn)
            goto fen_parse_error;
        if (rank == 2 && ((tb_pawn_attacks(ep, true) & (black & pawns)) == 0))
            ep = 0;
        if (rank == 5 && ((tb_pawn_attacks(ep, false) & (white & pawns)) == 0))
            ep = 0;
    }
    else if (c != '-')
        goto fen_parse_error;
    c = *fen++;
    if (c != ' ')
        goto fen_parse_error;
    char clk[4];
    clk[0] = *fen++;
    if (clk[0] < '0' || clk[0] > '9')
        goto fen_parse_error;
    clk[1] = *fen++;
    if (clk[1] != ' ')
    {
        if (clk[1] < '0' || clk[1] > '9')
            goto fen_parse_error;
        clk[2] = *fen++;
        if (clk[2] != ' ')
        {
            if (clk[2] < '0' || clk[2] > '9')
                goto fen_parse_error;
            c = *fen++;
            if (c != ' ')
                goto fen_parse_error;
            clk[3] = '\0';
        }
        else
            clk[2] = '\0';
    }
    else
        clk[1] = '\0';
    rule50 = atoi(clk);
    move = atoi(fen);

    TBpos->white = white;
    TBpos->black = black;
    TBpos->kings = kings;
    TBpos->queens = queens;
    TBpos->rooks = rooks;
    TBpos->bishops = bishops;
    TBpos->knights = knights;
    TBpos->pawns = pawns;
    TBpos->castling = castling;
    TBpos->rule50 = rule50;
    TBpos->ep = ep;
    TBpos->turn = turn;
    TBpos->move = move;
    return true;

    fen_parse_error:
    return false;
}

char *transTBMove(unsigned to, unsigned from, unsigned promotes) {
    static char moveStr[6];
    char promoChar;
    if (promotes != TB_PROMOTES_NONE)
    {
        switch (promotes)
        {
            case TB_PROMOTES_QUEEN:
                promoChar = 'q'; break;
            case TB_PROMOTES_ROOK:
                promoChar = 'r'; break;
            case TB_PROMOTES_BISHOP:
                promoChar = 'b'; break;
            case TB_PROMOTES_KNIGHT:
                promoChar = 'n'; break;
        }
    }

    if(promotes == TB_PROMOTES_NONE) {
        sprintf(moveStr, "%c%c%c%c%c", ('a' + file(from)), ('1' + rank(from)), ('a' + file(to)), ('1' + rank(to)), '\0');
    } else {
        sprintf(moveStr, "%c%c%c%c%c", ('a' + file(from)), ('1' + rank(from)), ('a' + file(to)), ('1' + rank(to)), promoChar);
    }
    printf("Translated TB Move: %s\n", moveStr);
    return moveStr;
};

static void move_to_str(const struct TBpos *pos, unsigned move, char *str)
{
    uint64_t occ      = pos->black | pos->white;
    uint64_t us       = (pos->turn? pos->white: pos->black);
    unsigned from     = TB_GET_FROM(move);
    unsigned to       = TB_GET_TO(move);
    unsigned r        = rank(from);
    unsigned f        = file(from);
    unsigned promotes = TB_GET_PROMOTES(move);
    bool     capture  = (occ & board(to)) != 0 || (TB_GET_EP(move) != 0);
    uint64_t b = board(from), att = 0;
    if (b & pos->kings)
        *str++ = 'K';
    else if (b & pos->queens)
    {
        *str++ = 'Q';
        att = tb_queen_attacks(to, occ) & us & pos->queens;
    }
    else if (b & pos->rooks)
    {
        *str++ = 'R';
        att = tb_rook_attacks(to, occ) & us & pos->rooks;
    }
    else if (b & pos->bishops)
    {
        *str++ = 'B';
        att = tb_bishop_attacks(to, occ) & us & pos->bishops;
    }
    else if (b & pos->knights)
    {
        *str++ = 'N';
        att = tb_knight_attacks(to) & us & pos->knights;
    }
    else
        att = tb_pawn_attacks(to, !pos->turn) & us & pos->pawns;
    if ((b & pos->pawns) && capture)
        *str++ = 'a' + f;
    else if (tb_pop_count(att) > 1)
    {
        if (tb_pop_count(att & (BOARD_FILE_A >> f)) == 1)
            *str++ = 'a' + f;
        else if (tb_pop_count(att & (BOARD_RANK_1 << (8*r))) == 1)
            *str++ = '1' + r;
        else
        {
            *str++ = 'a' + f;
            *str++ = '1' + r;
        }
    }
    if (capture)
        *str++ = 'x';
    *str++ = 'a' + file(to);
    *str++ = '1' + rank(to);
    if (promotes != TB_PROMOTES_NONE)
    {
        *str++ = '=';
        switch (promotes)
        {
            case TB_PROMOTES_QUEEN:
                *str++ = 'Q'; break;
            case TB_PROMOTES_ROOK:
                *str++ = 'R'; break;
            case TB_PROMOTES_BISHOP:
                *str++ = 'B'; break;
            case TB_PROMOTES_KNIGHT:
                *str++ = 'N'; break;
        }
    }
    *str++ = '\0';
}

unsigned tbProbeWDL(struct TBpos* TBpos){

    int cardinality = tb_pop_count(TBpos->white | TBpos->black);

    if (TBpos->ep != 0
            ||  TBpos->castling != 0
            ||  TBpos->rule50 >= 50
            ||  cardinality > (int)TB_LARGEST)
        return TB_RESULT_FAILED;

    return tb_probe_wdl(
            TBpos->white, TBpos->black, TBpos->kings,
            TBpos->queens, TBpos->rooks, TBpos->bishops, TBpos->knights, TBpos->pawns,
            TBpos->rule50, TBpos->castling, TBpos->ep, TBpos->turn);
};

int probeRootDTZ(S_BOARD *pos, char *fen, int depth) {
    struct TBpos TBpos0;
    struct TBpos *TBpos = &TBpos0;
    parse_FEN_TB(TBpos, fen);
    unsigned result = tb_probe_root(TBpos->white, TBpos->black, TBpos->kings,
                                    TBpos->queens, TBpos->rooks, TBpos->bishops, TBpos->knights, TBpos->pawns,
                                    TBpos->rule50, TBpos->castling, TBpos->ep, TBpos->turn, NULL);
    if (   result == TB_RESULT_FAILED
           || result == TB_RESULT_CHECKMATE
           || result == TB_RESULT_STALEMATE)
        return 0;

    unsigned wdl, dtz, to, from, ep, testpromo;
    int promo = 0;
    // Extract Fathom's score representations
    wdl = TB_GET_WDL(result);
    dtz = TB_GET_DTZ(result);

    // Extract Fathom's move representation
    to    = TB_GET_TO(result);
    from  = TB_GET_FROM(result);
    ep    = TB_GET_EP(result);
    testpromo = TB_GET_PROMOTES(result);
    /*if(testpromo == TB_PROMOTES_NONE) {
        promo = 0;
    } else if (testpromo == TB_PROMOTES_QUEEN) {
        promo = 1;
    } else if (testpromo == TB_PROMOTES_ROOK) {
        promo = 2;
    } else if (testpromo == TB_PROMOTES_BISHOP) {
        promo = 3;
    } else if (testpromo == TB_PROMOTES_KNIGHT) {
        promo = 4;
    }*/

    if(testpromo != TB_PROMOTES_NONE) {
        switch (testpromo) {
            case TB_PROMOTES_QUEEN:
                promo = 1;
                break;
            case TB_PROMOTES_ROOK:
                promo = 2;
                break;
            case TB_PROMOTES_BISHOP:
                promo = 3;
                break;
            case TB_PROMOTES_KNIGHT:
                promo = 4;
                break;
        }
    }

    char *TBmove = transTBMove(to, from, testpromo);

    int finalmove;
    finalmove = ParseMove(TBmove, pos);
    if(finalmove == NOMOVE) {
        printf("Bad TB move!\n");
    }

    return finalmove;
}
