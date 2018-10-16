//
// Created by Eric Silverman on 14/10/2018.
//

#ifndef SPACEDOG_BOOK_POLYKEYS_H
#define SPACEDOG_BOOK_POLYKEYS_H

#ifdef _MSC_VER
#  define U64_POLY(u) (u##ui64)
#else
#  define U64_POLY(u) (u##ULL)
#endif

extern const U64 Random64Poly[781];

#endif //SPACEDOG_BOOK_POLYKEYS_H
