# SpaceDog
Chess engine based on VICE, written in C.

Added features include an enhanced evaluation function, PV search, a variety of game recording and logging functions, and Syzygy tablebase support.

This is a learning experience for me, so please watch out for bugs!  My main goal is to endow SpaceDog with all the main features we expect from a modern alpha-beta engine, taking inspiration from open-source titans like Stockfish, Crafty, etc., to whom I owe a great debt for inspiring and educating me.  As time goes on I'll keep adding my own unique wrinkles.  

My ultimate goal for SpaceDog isn't to compete with the big engines, but to make a strong and useful chess partner for myself, and hopefully for other people learning the game.  With that in mind I've spent some effort adding game recording and logging functions that I find helpful for learning from my games with SpaceDog, and I'm working on some useful analysis functions too.  
Eventually I hope to branch out and add support for neural network evaluations, and support for my favourite chess variants.

-------
Tablebase support is made possible with the Fathom API, based on Ronald de Man's original Syzygy TB probing code and subsequently modified by Basil and Jon Dart.  All the code is provided under permissive licenses - Ronald de Man's original code can be "redistributed and/or modified without restrictions", and Jon Dart and Basil's modifications are released under the MIT License.

