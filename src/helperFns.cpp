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
#include "helperFns.h"
#include <sstream>
#include <unistd.h>

int stuff[] = {'#', static_cast<int>(ACS_ULCORNER), static_cast<int>(ACS_LLCORNER), static_cast<int>(ACS_URCORNER), static_cast<int>(ACS_LRCORNER),
	static_cast<int>(ACS_LTEE), static_cast<int>(ACS_RTEE), static_cast<int>(ACS_BTEE), static_cast<int>(ACS_TTEE), static_cast<int>(ACS_HLINE), static_cast<int>(ACS_VLINE), static_cast<int>(ACS_PLUS)};
std::set<int> WALLS(stuff, stuff + 12);

// Return the character at x, y
chtype charAt(int x, int y) {
	//if(!mtx.try_lock())
	//	return false;
	//std::lock_guard<std::mutex> lock (mtx);
	//mtx.lock();
	// check bounds
	if(x < 0 || y < 0)
		return 0;

	int curX, curY; // store current position; we will return to it

	// get value
	getyx(stdscr, curY, curX);
	chtype value = mvinch(y, x);

	// move back + return
	mvinch(curY, curX);
	//mtx.unlock();
	return value;
}

// Return the letter ignoring colour at x, y
chtype letterAt(int x, int y) {
  return charAt(x, y) & A_CHARTEXT;
}

bool writeAt(int x, int y, chtype letter) {
	//if(!mtx.try_lock())
		//return false;
	//std::lock_guard<std::mutex> lock(mtx);
	//mtx.lock();
	// Check bounds
	if(x < 0 || y < 0)
		return false;

	int curX, curY;
	getyx(stdscr, curY, curX);


	mvinch(y, x);

	addch(letter);
	mvinch(curY, curX);
	//mtx.unlock();
	return true;
}

bool writeAt(int x, int y, chtype letter, int color) {
	//mtx.lock();
	// Check bounds
	if(x < 0 || y < 0)
		return false;

	int curX, curY;
	getyx(stdscr, curY, curX);


	mvinch(y, x);
	attron(COLOR_PAIR(color));
	addch(letter);
	attroff(COLOR_PAIR(color));
	mvinch(curY, curX);

	//mtx.unlock();
	return true;
}

void writeError(std::string msg) {
	std::ofstream fs;
	fs.open("errors.log", std::fstream::app);
	fs << msg;
	fs << "\n";
	fs.close();
}

void printAtBottomChar(char msg) {
	//mtx.lock();
	std::string x;
	x += msg;
	mvprintw(MAP_END+5, 0, "%s", (x).c_str());
	//mtx.unlock();
}
void printAtBottom(std::string msg) {
	//mtx.lock();
	int x, y;
	getyx(stdscr, y, x);
	mvprintw(MAP_END+1, 1, "%s", msg.c_str());
	mvinch(y,x);
	move(y,x);

	//mtx.unlock();
}




// Game state
void winGame() {
	clear();
	writeError("YOU WIN");

  if (IN_TUTORIAL) {
	  printAtBottom("Now you know the basics, let's begin...");
	} else {
	  if((CURRENT_LEVEL % 3) == 0) {
		  printAtBottom("YOU WIN THE GAME!\nGAIN A LIFE!");
	  }
	  else {
		  printAtBottom("YOU WIN THE GAME!");
	  }
	}

	refresh();
	GAME_WON = 1;
	READY = false;
	sleep(1);
}

void loseGame() {
	clear();
	writeError("YOU LOSE");
	if (IN_TUTORIAL) {
	  printAtBottom("Hint: jump away quickly with gg or 10G");
	} else {
	  printAtBottom("YOU LOSE THE GAME!\nLOST 1 LIFE");
	  LIVES--;
	}
	refresh();
	GAME_WON = -1;
	READY = false;

	sleep(1);
}


bool isWall(chtype character) {
  return WALLS.find(character) != WALLS.end();
}


// check to see if the player can move there
bool isValid(int x, int y) {
	// Within range of board
	if(y < 0 || x < 0 || x >= WIDTH)
		return false;
	

	// Move cursor, check character, move cursor back
	int curX, curY;
	getyx(stdscr, curY, curX);
	chtype testPos= mvinch(y, x);
	mvinch(curY, curX);


	// Now see if it's a valid spot
	if(testPos >= 4000000 || isWall(testPos))
	{
		return false;
	}
	return true;	
}
