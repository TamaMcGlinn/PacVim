/*

Copyright 2015 Jamal Moon

PacVim is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License (LGPL) as 
published by the Free Software Foundation, either version 3 of the 
License, or (at your option) any later version.

PacVim program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */
#ifdef __APPLE__
#include <curses.h>
#elif __FreeBSD__
#include <curses.h>
#else
#include <cstddef>
#include <ncurses.h>
#endif



#ifndef GLOBALS_H
#define GLOBALS_H

#include <set>
#include <mutex>
#include "reachableMap.h"

//#include <cursesw.h>
extern int TOTAL_POINTS;
extern int GAME_WON; // 0 = in progress, 1 = won, -1 = lose
extern int FREEZE_GHOSTS; // 0 = moving, 1 = frozen
extern std::string INPUT; // keyboard characters
extern int CURRENT_LEVEL;
extern bool IN_TUTORIAL;
extern ReachableMap reachability_map;
extern int LIVES;
extern const int NUM_OF_LEVELS;

extern bool READY;
extern double THINK_MULTIPLIER; // all the think times for the AI are multipled by this

extern int MAP_BEGIN; // the first row in which a play can move in (near the *top* of the map file!)
extern int MAP_END;   // the last row with any character on it at all (not necessarily inside the map)
extern int WIDTH;
#define HEIGHT (MAP_END - MAP_BEGIN)

extern std::mutex mtx;
#endif

#ifndef MAPS_LOCATION
#define MAPS_LOCATION "maps"
#endif
