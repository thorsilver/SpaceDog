cmake_minimum_required(VERSION 3.10)
project(SpaceDog-FRC)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)
include_directories(/fathom/src/)

add_executable(SpaceDog-FRC
        fathom/src/tbconfig.h
        fathom/src/tbprobe.c
        fathom/src/tbprobe.h
        fathom/src/tbcore.h
        attack.c
        bitboards.c
        board.c
        chess960.c
        chess960.h
        data.c
        defs.h
        endgame.c
        endgame.h
        hashkeys.c
        init.c
        io.c
        main.c
        makemove.c
        misc.c
        movegen.c
        perft.c
        polybook.c
        polykeys.c
        polykeys.h
        pvtable.c
        search.c
        uci.c
        validate.c
        xboard.c syzygy.c syzygy.h eval.c eval.h chess960.h chess960.c endgame.c endgame.h)
