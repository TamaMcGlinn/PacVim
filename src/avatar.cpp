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
#include "avatar.h"
#include <sstream>

#include "globals.h"

avatar::avatar() {
	x = 1;
	y = 1;
	lives = 3;
	points = 0;
	portrait = 'G';
	isPlayer = false;
	color = COLOR_WHITE;
}

avatar::avatar(int a, int b) {
	x = a;
	y = b;
	lives = 3;
	points = 0;
	portrait = 'G';
	isPlayer = false;
	color = COLOR_WHITE;
	letterUnder = charAt(a, b);
}


avatar::avatar(int a, int b, bool human) {
	x = a;
	y = b;
	lives = 3;
	points = 0;
	isPlayer = human;
	if(human)
		portrait = ' '; // default for player
	else
		portrait = 'G';
	letterUnder = charAt(a, b);
	moveTo(a, b);
}

avatar::avatar(int a, int b, bool human, int c) {
	avatar(a, b, human);
	color = c;
}

int avatar::getPoints() { return points; }
bool avatar::getPlayer() { return isPlayer; }
int avatar::getX() { return x; }
int avatar::getY() { return y; }
char avatar::getPortrait() { return portrait; }

bool avatar::setPos(int theX, int theY) { 
	x = theX;
	y = theY;
	return true;
}

bool avatar::moveTo(int a, int b) {
	if(!isValid(a, b)) {
    // movement invalid, staying still
		return false;
	}

	// character at destination 
	chtype curChar = charAt(a, b);
	if(isPlayer) {
		if((curChar & COLOR_PAIR(6)) == COLOR_PAIR(6) ) {
			GAME_WON = -1;
		  // player hit a ~
			return false;
		}
		// hit a ghost.. red color
		if((curChar & COLOR_PAIR(1)) == COLOR_PAIR(1)) {
			GAME_WON = -1;
			// player hit a ghost!
			return false;
		}
		
		// points
		if(curChar != ' ' && !(curChar & COLOR_PAIR(2))) {
			points++;
		}
		// move
		x = a;
		y = b;
		writeAt(x, y, curChar, COLOR_GREEN); // make it green
		letterUnder = charAt(x, y);
		move(b, a);

		if(points >= TOTAL_POINTS) {
			GAME_WON = 1;
		}
	}
	else { // it is a ghost

		// get player position, see if we stepped on the player
		int playerX, playerY;
		getyx(stdscr, playerY, playerX);
		if(playerY == b && playerX == a) {
			GAME_WON = -1; // hit the player, end the game
		}
		// check if we are hitting a ghost-- if so, it's an invalid location
		if( (curChar & COLOR_PAIR(1)) == COLOR_PAIR(1) ) {
			return false;
		}
		writeAt(x, y, letterUnder);
		letterUnder = charAt(a, b);

		writeAt(a, b, portrait,  color); 
		x = a;
		y = b;
	}
	refresh();
	return true;

}
bool avatar::moveRight() {
	if(!isValid(x+1, y)) 
		return false;
	
	moveTo(x+1, y+0);
	return true;
}

bool avatar::moveLeft() {
	if(!isValid(x-1, y))
		return false;
	
	moveTo(x-1, y);
	return true;
}

bool avatar::moveUp() {
	if(!isValid(x,y-1)) 
		return false;
	
	moveTo(x, y-1);
	return true;
}

bool avatar::moveDown() {
	if(!isValid(x,y+1))
		return false;
	moveTo(x, y+1);
	return true;
}

bool avatar::parseWordEnd(bool isWord) {
	// Formula: Get next char, is it alphanumeric? If so, loop & break
	//		on nonalpha-, and viceversa. 
	// 2nd case: if you are not at the end of a word, loop until you
	//		reach a space
	
	if(charAt(x+1, y) == ' ') {
		moveRight();
	}
	// store the current character type
	char curChar = charAt(x, y);
	bool isAlpha = isalnum(curChar);
	char nextChar = charAt(x+1,y);

	while(nextChar == ' ') {
		if(!moveTo(x+1, y))
			return false;
		nextChar = charAt(x+1, y);
		curChar = charAt(x, y);
		isAlpha = isalnum(curChar);
	}
	// breakOnSpace = true if the current character isn't the end of a word
	bool breakOnSpace = (nextChar != ' ' && curChar != ' ');
	bool breakOnAlpha = !isalnum(nextChar) && nextChar != ' ';
	while(true) { // no definite loop #; break when we reach conditions
		if((!breakOnAlpha == !isalnum(nextChar))  && isWord) {
			break;
		}
		else if(breakOnSpace && (nextChar == ' ')) {
			break; 
		}
		else if(nextChar == '#') { // not allowed to go on # so break
			break;
		}
		else { // iterate
			if(!moveTo(x+1, y))
				return false;
			if(nextChar != ' ')
				breakOnSpace = true;
			nextChar = charAt(x+1, y);
			curChar = charAt(x, y);
		}
	}
	return true;
}
				
bool avatar::parseWordBackward(bool isWord) {
	// Formula: Get next char, is it alphanumeric? If so, loop & break
	//		on nonalpha-, and viceversa. 
	// 2nd case: if you are not at the end of a word, loop until you
	//		reach a space

	// store the current character type
	char curChar = charAt(x, y); 
	bool isAlpha = isalnum(curChar);
	char nextChar = charAt(x-1, y); 

	// breakOnSpace = true if the current character isn't the end of a word
	bool breakOnSpace = (nextChar != ' ' && curChar != ' ');
	bool breakOnAlpha = breakOnSpace && (!isAlpha && !isalnum(nextChar) && isWord); 
	bool breakOnNonAlpha = breakOnSpace && ((isAlpha && isalnum(nextChar))
	 		|| (!isAlpha && isalnum(nextChar)))&& isWord;


	while(true) { // no definite loop #; break when we reach conditions
		if(curChar != ' ' && ((isalnum(nextChar) && breakOnAlpha) || (!isalnum(nextChar) && breakOnNonAlpha))) {
			break;
		}
		else if(breakOnSpace && nextChar == ' ') {
			break; 
		}
		else if(nextChar == '#') { // not allowed to go on # so break
			return false;
		}
		else { // iterate

			if(!moveTo(x-1, y))
				return false;
			if(nextChar != ' ' && !breakOnSpace) {
				if(isWord) {	
					breakOnAlpha = !isalnum(nextChar);
					breakOnNonAlpha = isalnum(nextChar);
				}
				breakOnSpace = true;
			}
			nextChar = charAt(x-1, y); 
			curChar = charAt(x, y);
		}
	}
	return true;
}

bool avatar::parseWordForward(bool isWord) {
	char curChar = charAt(x, y); 
	bool isAlpha = isalnum(curChar);
	char lastChar= 'X';

	bool breakOnAlpha = !isalnum(curChar);
	
	while(true) {
		if(curChar != ' ' && (!isalnum(curChar) == !breakOnAlpha) && isWord ) { // if they are the same
			break;
		}
		else if(lastChar == ' ' && curChar != ' ') {
			break;
		}
		else if(lastChar == '#' || curChar == '#') {
			moveTo(x-1, y);
			return false;
		}
		else {
			lastChar = curChar;
			if(!moveTo(x+1,y))
				return false;
			curChar = charAt(x,y);
		}
	}
	return true;
}

bool avatar::jumpToEnd() {
  int x = reachability_map.last_reachable_index_on_line(y);
  if (x != -1) {
    moveTo(x, y);
	  return true;
  }
  return false;
}

bool avatar::jumpToBeginning() { 
  int x = reachability_map.first_reachable_index_on_line(y);
	if (x != -1) {
	  moveTo(x,y);
	  return true;
	}
	return false;
}

bool avatar::percentJump() {
  int source_x = x;
  chtype letter;
  char opposite = 'x'; bool forward = true;
  while (opposite == 'x' && source_x < WIDTH) {
    letter = letterAt(source_x,y);
    if(letter == '(') {
	    opposite = ')';
    }
    else if(letter == '{') {
	    opposite = '}';
    }
    else if(letter == '[') {
	    opposite = ']';
    }
    else if(letter == ')') {
	    opposite = '(';
	    forward = false;
    }
    else if(letter == '}') {
	    opposite = '{';
	    forward = false;
    }
    else if(letter == ']') {
	    opposite = '[';
	    forward = false;
    }
    if (opposite == 'x') {
      source_x += 1;
    }
  }
  if (opposite == 'x') {
    // no bracket char found
    return false;
  }
  int offset = forward ? 1 : -1;
  for(int target_y = y; target_y >= 0 && target_y < HEIGHT; target_y += offset) {
    int start_x = target_y == y ? source_x + offset : (forward ? 0 : WIDTH - 1);
    for(int target_x = start_x; target_x >= 0 && target_x < WIDTH; target_x += offset) {
      chtype letter = letterAt(target_x,target_y);
      if(letter == opposite){
        return moveTo(target_x,target_y);
      }
    }
  }
  return false;
}

bool avatar::jumpToChar(char targetChar, bool forward, bool includingTarget, bool acrossWalls) {
  int offset = forward ? 1 : -1;
  for(int target_x = x + offset; target_x >= 0 && target_x < WIDTH + 1; target_x += offset) {
    if (!acrossWalls && charAt(target_x, y) > 4000000) {
      return false;
    }
    chtype letter = letterAt(target_x,y);
    if(letter == targetChar){
      if (!includingTarget) {
        target_x -= offset;
      }
      return moveTo(target_x,y);
    }
  }
  return false;
}

bool avatar::jumpForward(char targetChar, bool includingTarget, bool acrossWalls) {
  return jumpToChar(targetChar, true, includingTarget, acrossWalls);
}

bool avatar::jumpBackward(char targetChar, bool includingTarget, bool acrossWalls) {
  return jumpToChar(targetChar, false, includingTarget, acrossWalls);
}
