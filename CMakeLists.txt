cmake_minimum_required(VERSION 3.10)
project(SpaceDog_TBs)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)
include_directories(/fathom/src/)

add_executable(SpaceDog_TBs
        fathom/src/tbconfig.h
        fathom/src/tbprobe.c
        fathom/src/tbprobe.h
        fathom/src/tbcore.h
        attack.c
        bitboards.c
        board.c
        data.c
        defs.h
        evaluate.c
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
        xboard.c syzygy.c syzygy.h)
