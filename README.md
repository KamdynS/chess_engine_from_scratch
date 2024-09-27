# chess_engine_from_scratch
A chess engine I built from scratch to refine my C++ skills.

# Current progress
This repo is a work in progress that aims to show my skills as I develop. If this message is still here, the engine is not complete yet.

# OOP Refactor progress
All rules are implemented. I've added support for bitboards and refactored the entire engine to follow a Singleton design pattern. This introduced some bugs that I'm yet to be able to solve. I merged that branch regardless because I do like the code. I just need to get it working. 

The big issue atm is getting bitboards fully functional. These have been a thorn in my side as of late. 

# Engine plans

After getting the basic functionality working, I will start on the engine. The plan for this is to start with a simple piece score + MCTS. Later on I'll add DL to the MCTS.

## Goals for engine

I want to create an engine in the style of AlphaZero, with my own modifications. Most projects I see use stockfish, but I will be implementing something different

Eventually I will connect this to the lichess API and register it as a bot.

That's all for now! If you have any questions or inquiries, reach me at my twitter: @kamdynshaeffer Cheers! :)