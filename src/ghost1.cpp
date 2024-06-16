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

#include "ghost1.h"
#include <random>

double Ghost1::eval(int positionX, int positionY, int playerX, int playerY, bool ignoreWalls) {
	if(!isValid(positionX,positionY,ignoreWalls))
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

	switch (species) {
	  case Ghost_Species::Seeker:
	    seeker_think();
	    break;
	  case Ghost_Species::Lemming:
	  case Ghost_Species::Clockwise_Lemming:
	  case Ghost_Species::AntiClockwise_Lemming:
	    lemming_think();
	    break;
	  case Ghost_Species::Agent_Smith:
	    smith_think();
	    break;
	  default:
	    writeError("Invalid species!");
	}
}

void Ghost1::seeker_think() {
  bool ignoreWalls = can_ignore_walls ? rare_ability(50) : false;

	// evaluate the four potential paths and move accordingly
	int playerX, playerY;
	getyx(stdscr, playerY, playerX);

	double up = eval(x, y-1, playerX, playerY, ignoreWalls);
	double down = eval(x, y+1, playerX, playerY, ignoreWalls);
	double left = eval(x-1, y, playerX, playerY, ignoreWalls);
	double right = eval(x+1, y, playerX, playerY, ignoreWalls);

	if(up <= down && up <= left && up <= right) {
		moveTo(x, y-1, ignoreWalls);
	} else if(down <= left && down <= right && down <= up) {
		moveTo(x, y+1, ignoreWalls);
	} else if(left <= right && left <= up && left <= down) {
		moveTo(x-1, y, ignoreWalls);
	} else if(right <= up && right <= down && right <= left) {
		moveTo(x+1, y, ignoreWalls);
	}
}

Direction Ghost1::get_most_pronounced_direction() {
  int vertical_middle = (MAP_END - MAP_BEGIN) / 2;
  int horizontal_middle = WIDTH / 2;

  int northness = vertical_middle - y;
  int southness = y - vertical_middle;
  int eastness = x - horizontal_middle;
  int westness = horizontal_middle - x;

  if (northness > southness && northness > eastness && northness > westness) {
    return Direction::North;
  }
  if (southness > northness && southness > eastness && southness > westness) {
    return Direction::South;
  }
  if (eastness > northness && eastness > southness && eastness > westness) {
    return Direction::East;
  }
  if (westness > northness && westness > southness && westness > eastness) {
    return Direction::West;
  }
  writeError("Error: get_most_pronounced_direction has some impossible error");
  return Direction::North;
}

Direction Ghost1::get_next_dir(Direction previous_dir) {
  bool clockwise = species == Ghost_Species::Clockwise_Lemming;
  switch (previous_dir) {
    case Direction::North:
      if (clockwise) {
        return Direction::East;
      } else {
        return Direction::West;
      }
    case Direction::East:
        if (clockwise) {
          return Direction::South;
        } else {
          return Direction::North;
        }
      case Direction::South:
        if (clockwise) {
          return Direction::West;
        } else {
          return Direction::East;
        }
      case Direction::West:
        if (clockwise) {
          return Direction::North;
        } else {
          return Direction::South;
        }
      default:
        writeError("Invalid direction!");
        return previous_dir;
  }
}

bool Ghost1::ghost_at_position(int x, int y) {
	chtype curChar = charAt(x, y);
  return PAIR_NUMBER(curChar & A_COLOR) == 1;
}

bool Ghost1::direction_valid(Direction dir) {
  switch(dir) {
    case Direction::North:
      return isValid(x, y-1) && !ghost_at_position(x, y-1);
    case Direction::East:
      return isValid(x+1, y) && !ghost_at_position(x+1, y);
    case Direction::South:
      return isValid(x, y+1) && !ghost_at_position(x, y+1);
    case Direction::West:
      return isValid(x-1, y) && !ghost_at_position(x-1, y);
    default:
      writeError("Invalid direction when checking for walls");
      return false;
  }
}

bool Ghost1::rare_ability(int rarity) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, rarity);
  int pick_random = dist(gen);
  return pick_random == 0;
}

Direction Ghost1::get_next_valid_dir(Direction direction) {
  int directions_tried = 0;
  for(int directions_tried = 0; !direction_valid(direction); direction = get_next_dir(direction), ++directions_tried) {
    // intentionally empty, except for error that shouldn't ever occur
    if (directions_tried > 4) { // maybe this should be 3 but I don't care right now
      writeError("(Anti)Clockwise lemming is going around in circles! (could be a map error)");
      return Direction::North;
    }
  }
  if (direction == current_direction) {
    if (rare_ability(200)) {
      // very rarely change direction when not hitting wall
      return get_next_valid_dir(get_next_dir(direction));
    }
  }
  return direction;
}

void Ghost1::smith_think() {
  int playerX = player.getX();
  int playerY = player.getY();
  float distance = sqrt( (x - playerX) * (x - playerX) +
                         (y - playerY) * (y - playerY) );
  std::stringstream ss;
  if (distance < 4.1 and distance >= 0.1) {
    species = Ghost_Species::Seeker;
    spawn(x, y);
  }
}

void Ghost1::lemming_think() {
  lemming_pickdir();
  moveTo(x + previous_xoffset, y + previous_yoffset);
}

void Ghost1::lemming_pickdir() {
  if (previous_xoffset == 0 && previous_yoffset == 0) {
    if (species == Ghost_Species::Lemming) {
      pick_random_direction();
    } else {
      Direction current_area = get_most_pronounced_direction();
      writeError("Most pronounced dir:");
      writeError(std::to_string(static_cast<int>(current_area)));
      Direction turned_dir = get_next_dir(current_area); // do one turn for sure,
      Direction next_dir = get_next_valid_dir(turned_dir); // then as many as necessary
      pick_direction(next_dir);
    }
    return;
  }
  Direction next_dir = get_next_valid_dir(current_direction);
  pick_direction(next_dir);
}

void Ghost1::spawn(int theX, int theY) {
  if (species == Ghost_Species::Agent_Smith) {
    x = theX;
    y = theY;
	  letterUnder = charAt(x, y);
    can_ignore_walls = true;
  } else {
    avatar::spawn(theX, theY);
  }
  last_think_time = std::chrono::steady_clock::now();
}

void Ghost1::pick_direction(Direction dir) {
    current_direction = dir;
    switch(dir) {
      case Direction::North:
        previous_xoffset = 0;
        previous_yoffset = -1;
        return;
      case Direction::East:
        previous_xoffset = 1;
        previous_yoffset = 0;
        return;
      case Direction::South:
        previous_xoffset = 0;
        previous_yoffset = 1;
        return;
      case Direction::West:
        previous_xoffset = -1;
        previous_yoffset = 0;
        return;
    }
}

void Ghost1::pick_random_direction() {
  std::vector<Direction> valid_dirs;
  if (isValid(x, y-1)) {
    valid_dirs.push_back(Direction::North);
  }
  if (isValid(x, y+1)) {
    valid_dirs.push_back(Direction::South);
  }
  if (isValid(x-1, y)) {
    valid_dirs.push_back(Direction::West);
  }
  if (isValid(x+1, y)) {
    valid_dirs.push_back(Direction::East);
  }
  if (valid_dirs.empty()) {
    writeError("Lemming has no place to go!");
    previous_xoffset = 0;
    previous_yoffset = 0;
    return;
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, valid_dirs.size() - 1);
  int random_index = dist(gen);
  Direction random_dir = valid_dirs[random_index];
  pick_direction(random_dir);
}
