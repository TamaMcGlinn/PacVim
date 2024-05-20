/*

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

#ifndef SECTION_H
#define SECTION_H

// the point of this datastructure is to determine for every position in the
// map, whether it is directly connected to a non-whitespace, non-wall; i.e.
// a character which gives you a point, and which must be reached in order to
// complete the map. For example, consider:
//
// ###     ###
// #a##    #
// #  #    ###
// ####
//
// a is the only character, and this character also makes the spaces below it
// connected to a character. The C shape on the right does not create any
// reachable space, nor would it if it were an enclosed space, unless it
// had a character inside. The idea being that only places that can reach a
// character should be accessible to the player, and by this definition and
// algorithm, we can establish for every line which x positions can be jumped 
// to safely without ending up outside the map.

#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include <algorithm>

// sections are parts of a line, defined by the indexes of the first
// and last reachable characters on the line. For passages 1-char wide,
// the x_start and x_end are the same
struct Section {
  int x_start, x_end;
  bool isInside; // whether it contains a non-space char

  bool overlaps(const Section & otherSection) {
    bool overlaps = (otherSection.x_start <= x_end) && (otherSection.x_end >= x_start);
    return overlaps;
  }
};

// a (horizontal) line, contains sections and can be built up from a string
struct Line {
  std::vector<Section> sections;

  void addChar(int index, char c) {
    bool isInside = c != ' ';
    if (sections.empty()) {
      sections.push_back({index, index, isInside});
    } else {
      Section last_section = sections[sections.size() - 1];
      if (last_section.x_end != index - 1) {
        // discontinuous index means new section
        sections.push_back({index, index, isInside});
      } else {
        // continuous index means expand last section to include index
        last_section.x_end = index;
        if (isInside) {
          last_section.isInside = true;
        }
        // replace last element with modified copy
        sections.pop_back();
        sections.push_back(last_section);
      }
    }
  }

  Line(std::string str) {
    for(int i = 0; i < str.length(); i++) {
      char const c = str[i];
      if (c == '#') {
        continue;
      }
      addChar(i, c);
    }
  }

  Section section_at(int index) {
    for(Section & s : sections) {
      if (s.x_start <= index && s.x_end >= index) {
        return s;
      }
    }
    return {-1, -1, false};
  }
};

// Represents a set of sections that are connected to each other
// if one has a non-space character, they are all marked "inside"
// and are therefore reachable by jumps
class SectionGroup {
  bool is_inside;
  std::vector<Section *> sections;
  std::vector<Section *> bottom;
  std::vector<Section *> next_bottom;

public:
  SectionGroup() : is_inside(false) {
  }

  SectionGroup(Section * s) : is_inside(false) {
    addSection(s);
  }

  void addSection(Section * s, bool addBottom = true){
    if (s->isInside) {
      is_inside = true;
      for (auto existing_s : sections) {
        existing_s->isInside = true;
      }
    } else if (is_inside) {
      s->isInside = true;
    }
    sections.push_back(s);
    if (addBottom) {
      bottom.push_back(s);
    }
  }

  bool bottom_touches(Section s) {
    for(Section * b : bottom) {
      if (b->overlaps(s)) {
        return true;
      }
    }
    return false;
  }

  void addToNextBottom(Section * s){
    addSection(s, false);
    next_bottom.push_back(s);
  }

  void line_finished(){
    bottom.swap(next_bottom);
    next_bottom.clear();
  }

  void merge(SectionGroup * g){
    is_inside |= g->is_inside;
    for (auto s : g->sections) {
      addSection(s, false);
    }
    for(auto s : g->bottom) {
      if (std::find(bottom.begin(), bottom.end(), s) == bottom.end()) {
        bottom.push_back(s);
      }
    }
  }
};

class ReachableMap {
  std::vector<Line> lines;

  // the frontline contains all open-ended sectiongroups (bottoms), which we might still add sections to
  std::vector<SectionGroup> frontline;

public:
  void addLine(std::string str) {
    std::vector<std::vector<SectionGroup *>> frontline_groups_to_join;
    lines.push_back(Line(str));
    Line & newline = lines[lines.size()-1];
    // sections in the new line that are not connected to the previous
    // line at all are added as new frontline SectionGroups,
    // each containing 1 section
    std::vector<Section *> new_frontline_sections;
    // iterate over all the new line's sections
    for(Section & s : newline.sections) {
      // the list of bottoms section s touches
      std::vector<SectionGroup *> bottoms_touching;
      for (SectionGroup & g : frontline) {
        if (g.bottom_touches(s)) {
          bottoms_touching.push_back(&g);
          // potentially add multiple sections to the next bottom for g
          g.addToNextBottom(&s);
        }
      }
      if (bottoms_touching.empty()) {
        // since it doesn't touch any existing ones, this is a new SectionGroup
        new_frontline_sections.push_back(&s);
      } else if (bottoms_touching.size() > 1) {
        // going to merge SectionGroups
        frontline_groups_to_join.push_back(bottoms_touching);
      }
    }
    for (SectionGroup & g : frontline) {
      g.line_finished();
    }
    for (auto join_vector : frontline_groups_to_join) {
      // merging SectionGroups
      SectionGroup newGroup;
      // join the groups in this vector
      for (SectionGroup * sg : join_vector) {
        newGroup.merge(sg);
      }
      // remove them all from frontline
      frontline.erase(std::remove_if(frontline.begin(), frontline.end(),
        [&](SectionGroup & s) { 
          return std::find(join_vector.begin(), join_vector.end(), &s) != join_vector.end();
        }), frontline.end());
      // add the result again
      frontline.push_back(newGroup);
    }
    for (Section * s : new_frontline_sections) {
      // new SectionGroups, each containing 1 section
      frontline.push_back(SectionGroup(s));
    }
  }

  bool reachable(int x, int y) {
    if (lines.empty() || y < 0 || y > lines.size() - 1) {
      return false;
    }
    Section s = lines[y].section_at(x);
    return s.isInside;
  }

  int first_reachable_index_on_line(int y) {
    if (lines.empty() || y < 0 || y > lines.size() - 1) {
      return -1;
    }
    Line l = lines[y];
    if (l.sections.empty()) {
      return -1;
    }
    for (int section_index = 0; section_index < l.sections.size(); ++section_index) {
      if (l.sections[section_index].isInside) {
        return l.sections[section_index].x_start + 2;
      }
    }
    return -1;
  }

  int last_reachable_index_on_line(int y) {
    if (lines.empty() || y < 0 || y > lines.size() - 1) {
      return -1;
    }
    Line l = lines[y];
    if (l.sections.empty()) {
      return -1;
    }
    for (int section_index = l.sections.size() - 1; section_index >= 0; --section_index) {
      if (l.sections[section_index].isInside) {
        return l.sections[section_index].x_end + 2;
      }
    }
    return -1;
  }

  void clear() {
    lines.clear();
    frontline.clear();
  }
};

#endif
