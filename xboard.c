//
// Created by Eric Silverman on 13/10/2018.
//

// xboard.c

#include "stdio.h"
#include "defs.h"
#include "string.h"

int ThreeFoldRep(const S_BOARD *pos) {

    ASSERT(CheckBoard(pos));

    int i = 0, r = 0;
    for (i = 0; i < pos->hisPly; ++i)	{
        if (pos->history[i].posKey == pos->posKey) {
            r++;
        }
    }
    return r;
}

int DrawMaterial(const S_BOARD *pos) {
    ASSERT(CheckBoard(pos));

    if (pos->pceNum[wP] || pos->pceNum[bP]) return FALSE;
    if (pos->pceNum[wQ] || pos->pceNum[bQ] || pos->pceNum[wR] || pos->pceNum[bR]) return FALSE;
    if (pos->pceNum[wB] > 1 || pos->pceNum[bB] > 1) {return FALSE;}
    if (pos->pceNum[wN] > 1 || pos->pceNum[bN] > 1) {return FALSE;}
    if (pos->pceNum[wN] && pos->pceNum[wB]) {return FALSE;}
    if (pos->pceNum[bN] && pos->pceNum[bB]) {return FALSE;}

    return TRUE;
}

int checkresult(S_BOARD *pos) {
    ASSERT(CheckBoard(pos));

    if (pos->fiftyMove > 100) {
        printf("1/2-1/2 {fifty move rule (claimed by SpaceDog)}\n"); return TRUE;
    }

    if (ThreeFoldRep(pos) >= 2) {
        printf("1/2-1/2 {3-fold repetition (claimed by SpaceDog)}\n"); return TRUE;
    }

    if (DrawMaterial(pos) == TRUE) {
        printf("1/2-1/2 {insufficient material (claimed by SpaceDog)}\n"); return TRUE;
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

    if(found != 0) return FALSE;

    int InCheck = SqAttacked(pos->KingSq[pos->side],pos->side^1,pos);

    if(InCheck == TRUE)	{
        if(pos->side == WHITE) {
            printf("0-1 {black mates (claimed by SpaceDog)}\n");return TRUE;
        } else {
            printf("1-0 {white mates (claimed by SpaceDog)}\n");return TRUE;
        }
    } else {
        printf("\n1/2-1/2 {stalemate (claimed by SpaceDog)}\n");return TRUE;
    }
    return FALSE;
}

void PrintOptions() {
    printf("feature ping=1 setboard=1 colors=0 usermove=1 memory=1\n");
    printf("feature egt=\"syzygy\"\n");
    printf("feature done=1\n");
}

void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info) {

    info->GAME_MODE = XBOARDMODE;
    info->POST_THINKING = TRUE;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    PrintOptions(); // HACK

    int depth = -1, movestogo[2] = {30,30 }, movetime = -1;
    int time = -1, inc = 0;
    int engineSide = BOTH;
    int timeLeft;
    int sec;
    int mps;
    int move = NOMOVE;
    char inBuf[80], command[80];
    char egtbType[10], tbPath[60];
    int MB;

    engineSide = BLACK;
    ParseFen(START_FEN, pos);
    depth = -1;
    time = -1;

    while(TRUE) {

        fflush(stdout);

        if(pos->side == engineSide && checkresult(pos) == FALSE) {
            info->starttime = GetTimeMs();
            info->depth = depth;

            if(time != -1) {
                info->timeset = TRUE;
                time /= movestogo[pos->side];
                time -= 50;
                info->stoptime = info->starttime + time + inc;
            }

            if(depth == -1 || depth > MAXDEPTH) {
                info->depth = MAXDEPTH;
            }

            printf("time:%d start:%d stop:%d depth:%d timeset:%d movestogo:%d mps:%d\n",
                   time,info->starttime,info->stoptime,info->depth,info->timeset, movestogo[pos->side], mps);
            SearchPosition(pos, info);

            if(mps != 0) {
                movestogo[pos->side^1]--;
                if(movestogo[pos->side^1] < 1) {
                    movestogo[pos->side^1] = mps;
                }
            }

        }

        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
        fflush(stdout);
        if (!fgets(inBuf, 80, stdin))
            continue;

        sscanf(inBuf, "%s", command);

        printf("command seen:%s\n",inBuf);

        if(!strcmp(command, "quit")) {
            info->quit = TRUE;
            break;
        }

        if(!strcmp(command, "force")) {
            engineSide = BOTH;
            continue;
        }

        if(!strcmp(command, "protover")){
            PrintOptions();
            continue;
        }

        if(!strcmp(command, "sd")) {
            sscanf(inBuf, "sd %d", &depth);
            printf("DEBUG depth:%d\n",depth);
            continue;
        }

        if(!strcmp(command, "st")) {
            sscanf(inBuf, "st %d", &movetime);
            printf("DEBUG movetime:%d\n",movetime);
            continue;
        }

        if(!strcmp(command, "time")) {
            sscanf(inBuf, "time %d", &time);
            time *= 10;
            printf("DEBUG time:%d\n",time);
            continue;
        }

        if(!strcmp(command, "polykey")) {
            PrintBoard(pos);
            GetBookMove(pos);
            continue;
        }

        if(!strcmp(command, "memory")) {
            sscanf(inBuf, "memory %d", &MB);
            if(MB < 4) MB = 4;
            if(MB > MAX_HASH) MB = MAX_HASH;
            printf("Set Hash to %d MB\n",MB);
            InitHashTable(pos->HashTable, MB);
            continue;
        }

        if(!strcmp(command, "level")) {
            sec = 0;
            movetime = -1;
            if( sscanf(inBuf, "level %d %d %d", &mps, &timeLeft, &inc) != 3) {
                sscanf(inBuf, "level %d %d:%d %d", &mps, &timeLeft, &sec, &inc);
                printf("DEBUG level with :\n");
            }	else {
                printf("DEBUG level without :\n");
            }
            timeLeft *= 60000;
            timeLeft += sec * 1000;
            movestogo[0] = movestogo[1] = 30;
            if(mps != 0) {
                movestogo[0] = movestogo[1] = mps;
            }
            time = -1;
            printf("DEBUG level timeLeft:%d movesToGo:%d inc:%d mps%d\n",timeLeft,movestogo[0],inc,mps);
            continue;
        }

        if(!strcmp(command, "ping")) {
            printf("pong%s\n", inBuf+4);
            continue;
        }

        if(!strcmp(command, "new")) {
            ClearHashTable(pos->HashTable);
            engineSide = BLACK;
            ParseFen(START_FEN, pos);
            depth = -1;
            time = -1;
            continue;
        }

        if(!strcmp(command, "setboard")){
            engineSide = BOTH;
            ParseFen(inBuf+9, pos);
            continue;
        }

        if(!strcmp(command, "go")) {
            engineSide = pos->side;
            continue;
        }

        if(!strcmp(command, "egtpath")) {
            //sscanf(inBuf, "egtpath %s %s", egtbType, tbPath);
            sscanf(inBuf, "egtpath %s %s", egtbType, tbPath);
            if(!strcmp(egtbType, "syzygy")) {
                EngineOptions->use_TBs = 1;
                strcpy(EngineOptions->EGTB_PATH, tbPath);
                printf("Using TB file path: %s\n", EngineOptions->EGTB_PATH);
                InitTBs(tbPath);
            } else {
                printf("Sorry, SpaceDog only supports Syzygy endgame tablebases!\n");
            }

            continue;
        }

        if(!strcmp(command, "usermove")){
            movestogo[pos->side]--;
            move = ParseMove(inBuf+9, pos);
            if(move == NOMOVE) continue;
            MakeMove(pos, move);
            checkresult(pos);
            pos->ply=0;
        }
    }
}


void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info) {

    printf("Welcome to SpaceDog In Console Mode!\n");
    printf("Type help for commands\n\n");

    info->GAME_MODE = CONSOLEMODE;
    info->POST_THINKING = TRUE;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int depth = MAXDEPTH, movetime = 3000;
    int engineSide = BOTH;
    int move = NOMOVE;
    char inBuf[80], command[80], syzygypath[50];

    engineSide = BLACK;
    ParseFen(START_FEN, pos);
    //EngineOptions->SanMode = 1;
    //InitTEX();

    while(TRUE) {

        fflush(stdout);

        if(pos->side == engineSide && checkresult(pos) == FALSE) {
            info->starttime = GetTimeMs();
            info->depth = depth;

            if(movetime != 0) {
                info->timeset = TRUE;
                info->stoptime = info->starttime + movetime;
            }

            SearchPosition(pos, info);
        }

        printf("\nSpaceDog > ");

        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
        fflush(stdout);
        if (!fgets(inBuf, 80, stdin))
            continue;

        sscanf(inBuf, "%s", command);

        if(!strcmp(command, "help")) {
            printf("Commands:\n");
            printf("quit - quit game\n");
            printf("force - computer will not think\n");
            printf("print - show board\n");
            printf("post - show thinking\n");
            printf("nopost - do not show thinking\n");
            printf("new - start new game\n");
            printf("go - set computer thinking\n");
            printf("depth x - set depth to x\n");
            printf("time x - set thinking time to x seconds (depth still applies if set)\n");
            printf("view - show current depth and movetime settings\n");
            printf("setboard x - set position to fen x\n");
            printf("texlog - write game record in TeX and initialise the file\n");
            printf("pgnlog - write game record in PGN and initialise the file\n");
            printf("startsum - start fancy summary file in TeX\n");
            printf("endtex - write closing statement to TeX game record\n");
            printf("endsum - complete and close fancy summary in TeX\n");
            printf("usetb - use Syzygy tablebases (in folder \"syzygy\"\n");
            printf("** note ** - to reset time and depth, set to 0\n");
            printf("enter moves using b7b8q notation\n\n\n");
            continue;
        }

        if(!strcmp(command, "mirror")) {
            engineSide = BOTH;
            MirrorEvalTest(pos);
            continue;
        }

        if(!strcmp(command, "eval")) {
            PrintBoard(pos);
            printf("Eval:%d",EvalPosition(pos));
            MirrorBoard(pos);
            PrintBoard(pos);
            printf("Eval:%d",EvalPosition(pos));
            continue;
        }

        if(!strcmp(command, "setboard")){
            engineSide = BOTH;
            ParseFen(inBuf+9, pos);
            continue;
        }

        if(!strcmp(command, "quit")) {
            info->quit = TRUE;
            break;
        }

        if(!strcmp(command, "post")) {
            info->POST_THINKING = TRUE;
            continue;
        }

        if(!strcmp(command, "print")) {
            PrintBoard(pos);
            continue;
        }

        if(!strcmp(command, "nopost")) {
            info->POST_THINKING = FALSE;
            continue;
        }

        if(!strcmp(command, "force")) {
            engineSide = BOTH;
            continue;
        }

        if(!strcmp(command, "view")) {
            if(depth == MAXDEPTH) printf("depth not set ");
            else printf("depth %d",depth);

            if(movetime != 0) printf(" movetime %ds\n",movetime/1000);
            else printf(" movetime not set\n");

            continue;
        }

        if(!strcmp(command, "depth")) {
            sscanf(inBuf, "depth %d", &depth);
            if(depth==0) depth = MAXDEPTH;
            continue;
        }

        if(!strcmp(command, "time")) {
            sscanf(inBuf, "time %d", &movetime);
            movetime *= 1000;
            continue;
        }

        if(!strcmp(command, "new")) {
            ClearHashTable(pos->HashTable);
            engineSide = BLACK;
            ParseFen(START_FEN, pos);
            continue;
        }

        if(!strcmp(command, "go")) {
            engineSide = pos->side;
            continue;
        }

        if(!strcmp(command, "texlog")) {
            InitTEX();
            EngineOptions->texLog = 1;
            continue;
        };

        if(!strcmp(command, "pgnlog")) {
            InitSanLog();
            EngineOptions->SanMode = 1;
            continue;
        };

        if(!strcmp(command, "startsum")) {
            InitSummary();
            EngineOptions->summary = 1;
            continue;
        };

        if(!strcmp(command, "endtex")) {
            EndTEX();
            continue;
        }

        if(!strcmp(command, "endsum")) {
            EndSummary();
            continue;
        }

        if(!strcmp(command, "syzygypath")) {
            sscanf(inBuf, "syzygypath %s", syzygypath);
            strcpy(EngineOptions->EGTB_PATH, syzygypath);
            printf("Using Syzygy Path: %s\n", syzygypath);
            continue;
        }

        if(!strcmp(command, "usetb")) {
            EngineOptions->use_TBs = 1;
            InitTBs(EngineOptions->EGTB_PATH);
            continue;
        }

        move = ParseMove(inBuf, pos);
        if(move == NOMOVE) {
            printf("Command unknown:%s\n",inBuf);
            continue;
        }
        char *sanMove = PrMoveSAN(pos, move);
        if(EngineOptions->SanMode == 1) {
            SanLog(sanMove, pos->side, pos->hisPly);
        }
        if(EngineOptions->summary == 1) {
            GameSummary(sanMove, pos->side, pos->hisPly);
        }
        MakeMove(pos, move);
        checkresult(pos);
        if(EngineOptions->texLog == 1) {
            WriteMoveTEX(inBuf, pos->hisPly);
        };
        //WriteMoveTEX(inBuf, pos->hisPly);
        PrintBoard(pos);
        printf("Standard Algebraic Notation: %s\n\n", sanMove);
        pos->ply=0;
    }
}





















