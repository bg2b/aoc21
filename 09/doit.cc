// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include <utility>
#include <vector>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return {c1.first + c2.first, c1.second + c2.second};
}

vector<coord> adjacent{{-1, 0}, {+1, 0}, {0, -1}, {0, +1}};

struct heightmap {
  // Heights are just represented directly from the input
  vector<string> height;

  // Construct from cin
  heightmap();

  int H() const { return height.size(); }
  int W() const { return height[0].size(); }
  // Out-of-bounds gives a value greater than '9'
  char at(coord const &c) const;

  // Is c (strictly) lower than all adjacent locations?
  bool is_low_point(coord const &c) const;

  // Return the risk associated with all the low points
  int total_risk() const;

  // If c is a low point, return the basin that drains to that point.
  // Return {} if not a low point
  set<coord> basin_at(coord const &c) const;
  // Return the product of the largest basin sizes
  int largest_basins() const;
};

heightmap::heightmap() {
  string line;
  while (getline(cin, line)) {
    height.push_back(line);
    assert(height.back().size() == height.front().size());
  }
}

char heightmap::at(coord const &c) const {
  char const maxval = '9' + 1;
  if (c.first < 0 || c.first >= W() || c.second < 0 || c.second >= H())
    return maxval;
  return height[c.second][c.first];
}

bool heightmap::is_low_point(coord const &c) const {
  auto h = at(c);
  for (auto const &delta : adjacent)
    if (h >= at(c + delta))
      return false;
  return true;
}

int heightmap::total_risk() const {
  auto risk = [&](coord const &c) {
    return is_low_point(c) ? at(c) - '0' + 1 : 0;
  };
  int result = 0;
  for (int i = 0; i < W(); ++i)
    for (int j = 0; j < H(); ++j)
      result += risk({i, j});
  return result;
}

set<coord> heightmap::basin_at(coord const &c) const {
  if (!is_low_point(c))
    return {};
  set<coord> result{c};
  vector<coord> frontier{c};
  while (!frontier.empty()) {
    auto c = frontier.back();
    frontier.pop_back();
    auto h = at(c);
    // Frontier locations pull in any adjacent locations that are
    // greater but that are not 9 (or off the edge)
    for (auto const &delta : adjacent) {
      auto c1 = c + delta;
      if (result.contains(c1))
        // Already in the basin
        continue;
      if (auto h1 = at(c1); h1 > h && h1 < '9') {
        // New
        result.emplace(c1);
        frontier.emplace_back(c1);
      }
    }
  }
  return result;
}

int heightmap::largest_basins() const {
  set<coord> all_basins;
  vector<int> basin_sizes;
  for (int i = 0; i < W(); ++i)
    for (int j = 0; j < H(); ++j) {
      coord c{i, j};
      if (all_basins.contains(c))
        // Already counted
        continue;
      if (auto basin = basin_at(c); !basin.empty()) {
        // There's a basin with low point c
        all_basins.insert(basin.begin(), basin.end());
        basin_sizes.push_back(basin.size());
      }
    }
  sort(basin_sizes.begin(), basin_sizes.end());
  int nbasins = basin_sizes.size();
  int result = 1;
  for (int i = 0; i < min(3, nbasins); ++i)
    result *= basin_sizes[nbasins - 1 - i];
  return result;
}

void part1() { cout << heightmap().total_risk() << '\n'; }
void part2() { cout << heightmap().largest_basins() << '\n'; }

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " partnum < input\n";
    exit(1);
  }
  if (*argv[1] == '1')
    part1();
  else
    part2();
  return 0;
}
