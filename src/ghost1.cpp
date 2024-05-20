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

#include "ghost1.h"

double Ghost1::eval(int positionX, int positionY, int playerX, int playerY) {
	if(!isValid(positionX,positionY))
		return 1000;
	return sqrt(pow(playerY-positionY, 2.0) + pow(playerX-positionX, 2.0));
}

void Ghost1::think() {
	if(GAME_WON != 0) {
	  return;
	}
	if (FREEZE_GHOSTS != 0) {
	  return;
	}

  auto now = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = now - last_think_time;
	if (elapsed_seconds.count() < sleepTime) {
	    return;
	}
	last_think_time = now;

	// evaluate the four potential paths and move accordingly
	int playerX, playerY;
	getyx(stdscr, playerY, playerX);
	double up = eval(x, y-1, playerX, playerY);
	double down = eval(x, y+1, playerX, playerY);
	double left = eval(x-1, y, playerX, playerY);
	double right = eval(x+1, y, playerX, playerY);

	if(up <= down && up <= left && up <= right) {
		moveTo(x, y-1);
	} else if(down <= left && down <= right && down <= up) {
		moveTo(x, y+1);
	} else if(left <= right && left <= up && left <= down) {
		moveTo(x-1, y);
	} else if(right <= up && right <= down && right <= left) {
		moveTo(x+1, y);
	}
}

void Ghost1::spawnGhost() { 
  last_think_time = std::chrono::steady_clock::now();
	if(!moveTo(x, y)) {
	  writeError("UNABLE TO SPAWN");
	}
}

