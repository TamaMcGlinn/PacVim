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

#ifndef GHOST1_H
#define GHOST1_H

#include "avatar.h"
#include <time.h>
#include <map>

enum class Ghost_Species {
  Seeker, // goes towards the player
  Lemming, // picks random direction, continued until hitting wall
  Clockwise_Lemming, // like Lemming but always travels clockwise
  AntiClockwise_Lemming, // like Lemming but always travels anti-clockwise
};

enum class Direction {
  North, East, South, West
};

struct offset {
  int x;
  int y;
};

class Ghost1 : public avatar {
	private:
	  Ghost_Species species;
    std::chrono::time_point<std::chrono::steady_clock> last_think_time;

    // previous direction of travel for lemmings
    Direction current_direction;
    int previous_xoffset = 0;
    int previous_yoffset = 0;

		double sleepTime;
    bool ghost_at_position(int x, int y);
    bool direction_valid(Direction dir);
    void pick_direction(Direction dir);
    Direction get_most_pronounced_direction();
    Direction get_next_dir(Direction previous_dir);
    Direction get_next_valid_dir(Direction direction);
    double eval(int positionX, int positionY, int playerX, int playerY);
    void pick_random_direction();
    void pick_clockwise_next_direction();
    void seeker_think();
    void lemming_think();
    void lemming_pickdir();
	public:
		void spawnGhost();
		Ghost1(Ghost_Species s, int theX, int theY, double c) : species(s), avatar(theX, theY, false, 'G', COLOR_RED) {
		  sleepTime = c;
		}
		void think();
};
#endif
