#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "defs.h"

#define fen1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define fen2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define fen3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define fen4 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

#define PAWNMOVESW "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNMOVESB "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define KNIGHTSKINGS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"
#define ROOKS "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1"
#define QUEENS "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 0 1 "
#define BISHOPS "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1 "

#define CASTLE1 "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
#define CASTLE2 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

#define PERFTFEN "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"


#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define PERFT "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

int main(int argc, char *argv[]) {

    AllInit();

    S_BOARD pos[1];
    S_SEARCHINFO info[1];
    info->quit = FALSE;
    pos->HashTable->pTable = NULL;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    EngineOptions->SanMode = 0;
    EngineOptions->summary = 0;
    EngineOptions->use_TBs = 0;

    int opt_index = 0;
    int hash_size = 0;
    char *syzygypath = NULL;
    char *mode = NULL;
    char *bookname = NULL;


    while (( opt_index = getopt(argc, argv, "h:s:m:b:")) != -1) {
        switch(opt_index) {
            case 'h':
                hash_size = atoi(optarg);
                InitHashTable(pos->HashTable, hash_size);
                break;
            case 's':
                syzygypath = optarg;
                EngineOptions->use_TBs = 1;
                strcpy(EngineOptions->EGTB_PATH, syzygypath);
                InitTBs(EngineOptions->EGTB_PATH);
                break;
            case 'b':
                bookname = optarg;
                strcpy(EngineOptions->BookName, bookname);
                printf("Using opening book: %s\n", bookname);
                InitPolyBook();
                break;
            case 'm':
                mode = optarg;
                if (!strcmp(mode, "uci")) {
                    Uci_Loop(pos, info);
                    if (info->quit == TRUE) {
                        free(pos->HashTable->pTable);
                        CleanPolyBook();
                        return 0;
                    }
                }
                if(!strcmp(mode, "xboard")) {
                    XBoard_Loop(pos, info);
                    if (info->quit == TRUE) {
                        free(pos->HashTable->pTable);
                        CleanPolyBook();
                        return 0;
                    }
                }
                if(!strcmp(mode, "console")) {
                    Console_Loop(pos, info);
                    if (info->quit == TRUE) {
                        free(pos->HashTable->pTable);
                        CleanPolyBook();
                        return 0;
                    }
                }
            case ':':
                fprintf(stderr, "Option -%c got a bad value!\n", optopt);
                break;
            case '?':
                fprintf(stderr, "Unrecognized option: '-%c'\n", optopt);
                return -1;
            default:
                strcpy(EngineOptions->BookName, "bookfish.bin");
                printf("Using default opening book (Bookfish.bin)\n");
                InitPolyBook();
                InitHashTable(pos->HashTable, 64);
                strcpy(EngineOptions->EGTB_PATH, "syzygy/");
                break;
        }
    }

    strcpy(EngineOptions->BookName, "bookfish.bin");
    printf("Using default opening book (Bookfish.bin)\n");
    InitPolyBook();
    InitHashTable(pos->HashTable, 64);
    strcpy(EngineOptions->EGTB_PATH, "syzygy/");
    printf("Welcome to SpaceDog! Type 'dog' to play in console mode. \n\n");
    printf("SpaceDog > ");

    char line[256];
    while (TRUE) {
        memset(&line[0], 0, sizeof(line));

        fflush(stdout);
        if (!fgets(line, 256, stdin))
            continue;
        if (line[0] == '\n')
            continue;
        if (!strncmp(line, "uci",3)) {
            Uci_Loop(pos, info);
            if(info->quit == TRUE) break;
            continue;
        } else if (!strncmp(line, "xboard",6))	{
            XBoard_Loop(pos, info);
            if(info->quit == TRUE) break;
            continue;
        } else if (!strncmp(line, "dog",3))	{
            Console_Loop(pos, info);
            if(info->quit == TRUE) break;
            continue;
        } else if(!strncmp(line, "quit",4))	{
            break;
        }
    }

    free(pos->HashTable->pTable);
    CleanPolyBook();
    return 0;
}