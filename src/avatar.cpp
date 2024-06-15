/*

Copyright 2015 Jamal Moon
Copyright 2024 Tama McGlinn

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

avatar::avatar(bool human, char p, int c) {
	lives = 3;
	isPlayer = human;
	portrait = p;
	color = c;
}

void avatar::spawn(int theX, int theY) {
  points = 0;
	x = theX;
	y = theY;
	letterUnder = charAt(x, y);
	moveTo(x, y);
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

bool avatar::moveTo(int a, int b, bool ignoreWalls) {
  if (GAME_WON == 1){
    // this prevents losing from doing repeated action like 88e
    // that first wins the game and then runs into an enemy or ~
    return false;
  }
	if(!isValid(a, b, ignoreWalls)) {
    // movement invalid, staying still
		return false;
	}

	// character at destination 
	chtype curChar = charAt(a, b);
	if(isPlayer) {
    if (PAIR_NUMBER(curChar & A_COLOR) == 6) {
			GAME_WON = -1;
		  // player hit a ~
			return false;
		}
		// hit a ghost.. red color
    if (PAIR_NUMBER(curChar & A_COLOR) == 1) {
			GAME_WON = -1;
			// player hit a ghost!
			return false;
		}
		
		// points
		if(curChar != ' ' && PAIR_NUMBER(curChar & A_COLOR) != 2) {
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
    if (PAIR_NUMBER(curChar & A_COLOR) == 1) {
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
bool avatar::moveRight(int repeats) {
  for (int i = 0; i < repeats; ++i) {
	  if(!isValid(x+1, y))
		  return false;
	
	  moveTo(x+1, y+0);
	}
	return true;
}

bool avatar::moveLeft(int repeats) {
	for (int i = 0; i < repeats; ++i) {
	  if(!isValid(x-1, y))
		  return false;
	
	  moveTo(x-1, y);
	}
	return true;
}

bool avatar::moveUp(int repeats) {
	for (int i = 0; i < repeats; ++i) {
	  if(!isValid(x,y-1)) 
		  return false;
	
	  moveTo(x, y-1);
	}
	return true;
}

bool avatar::moveDown(int repeats) {
	for (int i = 0; i < repeats; ++i) {
	  if(!isValid(x,y+1))
		  return false;
	  moveTo(x, y+1);
	}
	return true;
}

bool avatar::parseWordEnd(bool uppercase, int repeats) {
  for (int i = 0; i < repeats; ++i) {
    if (!parseSingleWordEnd(uppercase)) {
      return false;
    }
  }
  return true;
}

bool avatar::parseSingleWordEnd(bool uppercase) {
  return parse(uppercase, 1, false);
}
				
bool avatar::parseWordBackward(bool uppercase, int repeats) {
  for (int i = 0; i < repeats; ++i) {
    if (!parseSingleWordBackward(uppercase)) {
      return false;
    }
  }
  return true;
}

// for b, e, B, E parsing,
// and for w and W parsing, signified by stop_at_word_start == true
// 1. If this or next char is a space,
//    move past all the spaces and start from first non-space
// 2. If step 1 didn't move us, and
//    uppercase == false, and
//    the next character's alphanumericity is different,
//      move one character over.
// 3. If we have moved, and stop_at_word_start == true, terminate here and return true.
// 4. Now keep moving until either:
//    - the next char will be a space, or
//    - uppercase == false and
//      the next char's alphanumericity differs from that of the current character
// 5. If stop_at_word_start,
//    - move one character over, and then
//    - if this character is a space, move to the first non-space
// 6. Assert that we've moved at least one space
bool avatar::parse(bool uppercase, int offset, bool stop_at_word_start) {
  bool moved = false;
	char curChar = charAt(x, y); 
	char nextChar = charAt(x+offset, y);
  auto move_over = [&]() {
    moved = true;
    if (!moveTo(x+offset, y)) {
      return false;
    }
	  curChar = charAt(x, y); 
	  nextChar = charAt(x+offset, y);
	  return true;
	};
	// to ensure we always return when moveTo returns false
	#define MOVE if (!move_over()) { return false; }

  // 1. If this or next char is a space,
  //    move past all the spaces and start from first non-space
	if (curChar == ' ' || nextChar == ' ') {
	  MOVE;
	  while(curChar == ' ') {
	    MOVE;
	  }
  }

  assert(curChar != ' ' && "curChar still a space after moving past spaces");

  // 2. If step 1 didn't move us, and
  //    uppercase == false, and
  //    the next character's alphanumericity is different,
  if (!moved && uppercase == false && (isalnum(curChar) != isalnum(nextChar))) {
    //      move one character over.
    MOVE;
  }

  // 3. If we have moved, and stop_at_word_start == true, terminate here and return true.
  if (moved && stop_at_word_start) {
    return true;
  }

  // 4. Now keep moving until either:
  //    - the next char will be a space, or
  //    - uppercase == false and
  //      the next char's alphanumericity differs from that of the current character
  while(true) {
    if (nextChar == ' ') {
      break;
    }
    if (uppercase == false && (isalnum(curChar) != isalnum(nextChar))) {
      break;
    }
    MOVE;
  }

  // 5. If stop_at_word_start,
  if (stop_at_word_start) {
    //    - move one character over, and then
    MOVE;
    //    - if this character is a space, move to the first non-space
	  while(curChar == ' ') {
	    MOVE;
	  }
  }

	#undef MOVE

  // 6. Assert that we've moved at least one space
  assert(moved && "parse did not move us");

  return true;
}

bool avatar::parseSingleWordBackward(bool uppercase) {
  return parse(uppercase, -1, false);
}

bool avatar::parseWordForward(bool uppercase, int repeats) {
  for (int i = 0; i < repeats; ++i) {
    if (!parseSingleWordForward(uppercase)) {
      return false;
    }
  }
  return true;
}

bool avatar::parseSingleWordForward(bool uppercase) {
  return parse(uppercase, 1, true);
}

bool avatar::jumpToEnd(int repeats) {
  int specified_line = y + (repeats - 1);
  int target_line = find_reachable_line(specified_line, false);
  if (target_line == -1) {
    writeError("Unable to find reachable line backward from ");
    writeError(std::to_string(specified_line));
    return false;
  }
  int x = reachability_map.last_reachable_index_on_line(target_line);
  if (x != -1) {
    moveTo(x, target_line);
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
    if (charAt(source_x, y) > 4000000) {
      return false; // don't allow walljump for finding opening bracket
    }
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

bool avatar::jumpToChar(char targetChar, bool forward, bool includingTarget, bool acrossWalls, int repeats) {
  std::vector<int> target_list;
  int offset = forward ? 1 : -1;
  for(int target_x = x + offset; target_x >= 0 && target_x < WIDTH + 1; target_x += offset) {
    if (!acrossWalls && charAt(target_x, y) > 4000000) {
      return false;
    }
    chtype letter = letterAt(target_x,y);
    if(letter == targetChar){
      int real_target = target_x;
      if (!includingTarget) {
        real_target -= offset;
      }
      target_list.push_back(real_target);
      --repeats;
      if (repeats == 0) {
        for (int tx : target_list) {
          moveTo(tx,y);
        }
        return true;
      }
    }
  }
  return false;
}

bool avatar::jumpForward(char targetChar, bool includingTarget, bool acrossWalls, int repeats) {
  return jumpToChar(targetChar, true, includingTarget, acrossWalls, repeats);
}

bool avatar::jumpBackward(char targetChar, bool includingTarget, bool acrossWalls, int repeats) {
  return jumpToChar(targetChar, false, includingTarget, acrossWalls, repeats);
}

