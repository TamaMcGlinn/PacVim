# :q to quit

Just to be consistent with vim, :q rather than q should be to quit.

# better difficulty settings

There should ideally be a screen first to select the difficulty, and we want both easier and harder difficulties than currently.

# A* type ghost

A* pathfinding ghost type could be added only in nightmare difficulty.

# Allow keybindings

Maybe there should be a way to define keybindings, ideally with vimscript syntax.

# Boss key

There should be a key (F1?) that shows stuff compiling, as a throwback to other 90's games

# Multiplayer support

To achieve this, we will need to investigate a way to communicate between two instances of pacvim, either on the same machine
like in wemux, or full-blown networking. Oof... don't expect this any time soon!

Each player has their own color, and cooperatively turn all the characters in the level non-white to complete the level.
If one player gets caught, the other can still complete the level. Scores are separate, and reflect how many tokens that player walked across.
Lives are shared, but do you lose one when any player is caught or only once all players are caught?
