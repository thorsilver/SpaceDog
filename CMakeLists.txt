cmake_minimum_required(VERSION 3.10)
project(SpaceDog-book C)

set(CMAKE_C_STANDARD 99)

add_executable(SpaceDog-book main.c defs.h init.c bitboards.c hashkeys.c board.c data.c attack.c io.c movegen.c validate.c makemove.c perft.c search.c misc.c pvtable.c evaluate.c uci.c xboard.c polybook.c polykeys.c polykeys.h)