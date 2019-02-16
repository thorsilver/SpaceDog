//
// Created by Eric Silverman on 09/02/2019.
//

#ifndef SPACEDOG_TBS_SYZYGY_H
#define SPACEDOG_TBS_SYZYGY_H

#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include "defs.h"

//extern unsigned TB_LARGEST = 0;

struct TBpos
{
    uint64_t white;
    uint64_t black;
    uint64_t kings;
    uint64_t queens;
    uint64_t rooks;
    uint64_t bishops;
    uint64_t knights;
    uint64_t pawns;
    uint8_t castling;
    uint8_t rule50;
    uint8_t ep;
    bool turn;
    uint16_t move;
};

static bool parse_FEN_TB(struct TBpos *TBpos, const char *fen);
//unsigned tbProbeWDL(struct TBpos* TBpos);
unsigned tbProbeWDL(char *fen);
int probeRootDTZ(S_BOARD *pos, char *fen, int depth);

#endif //SPACEDOG_TBS_SYZYGY_H
