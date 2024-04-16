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

#include "globals.h"
#include "reachableMap.h"
#include <string>

int TOTAL_POINTS = 0;
int GAME_WON = 0;
int FREEZE_GHOSTS = 0;
std::string INPUT = "";
bool READY = false;
int LIVES = 3;
const int NUM_OF_LEVELS = 12;

double THINK_MULTIPLIER = 1.0;

int MAP_BEGIN = 0;
int MAP_END = 0;
int WIDTH = 0;
int CURRENT_LEVEL = 0;
bool IN_TUTORIAL = true;

bool lastJumpWasForwards;
bool lastJumpIncludedTarget;
char lastJumpChar = '\0';

ReachableMap reachability_map;

std::mutex mtx;
