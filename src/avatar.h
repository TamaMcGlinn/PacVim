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

#ifndef AVATAR_H
#define AVATAR_H

#include <math.h>
#include <sstream>
#include <unistd.h>
#include "helperFns.h"
// Avatar Class -- can be a ghost or player
class avatar {
	public:
    avatar(bool human, char p, int c);
    virtual void spawn(int theX, int theY);
	protected:
		chtype letterUnder;
		int x;
		int y;
		bool isPlayer;
		int points;
		char portrait;
		int lives;
		int color;
	public:	
    bool moveTo(int a, int b, bool ignoreWalls = false);
		//bool moveTo(int, int, bool);
		bool moveRight(int repeats);
		bool moveLeft(int repeats);
		bool moveUp(int repeats);
		bool moveDown(int repeats);
    bool parse(bool uppercase, int offset, bool stop_at_word_start);
		bool parseWordForward(bool, int repeats);
    bool parseSingleWordForward(bool uppercase);
		bool parseWordBackward(bool, int repeats);
    bool parseSingleWordBackward(bool uppercase);
		bool parseWordEnd(bool, int repeats);
    bool parseSingleWordEnd(bool uppercase);
		bool jumpToBeginning();
		bool jumpToEnd(int repeats);
	  bool percentJump();
		bool jumpToChar(char, bool, bool, bool, int repeats);
		bool jumpForward(char, bool, bool, int repeats);
		bool jumpBackward(char, bool, bool, int repeats);

		int getPoints();
		bool getPlayer();
		int getX();
		int getY();
		bool setPos(int, int);
		char getPortrait();

		void setLetterUnder(char);
};
	
#endif
