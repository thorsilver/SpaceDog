# SpaceDog
Chess engine based on VICE, written in C.

Added features include an enhanced evaluation function, PV search, a variety of game recording and logging functions, and Syzygy tablebase support.

This is a learning experience for me, so please watch out for bugs!  My main goal is to develop a fully-functional engine with all the main features we expect from a modern alpha-beta engine, taking inspiration from open-source titans like Stockfish, Crafty, etc.  As time goes on I'll add my own unique wrinkles.  Eventually I hope to branch out and add support for neural network evaluations, and support for my favourite chess variants.

-------
Tablebase support is made possible with the Fathom API, based on Ronald de Man's original Syzygy TB probing code and subsequently modified by Basil and Jon Dart.  All the code is provided under permissive licenses - Ronald de Man's original code can be "redistributed and/or modified without restrictions", and Jon Dart and Basil's modifications are released under the MIT License.

