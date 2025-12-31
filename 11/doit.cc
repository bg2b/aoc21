// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return {c1.first + c2.first, c1.second + c2.second};
}

// clang-format off
vector<coord> adjacent{
  {-1, -1}, {0, -1}, {+1, -1},
  {-1,  0},          {+1,  0},
  {-1, +1}, {0, +1}, {+1, +1}};
// clang-format on

struct grid {
  vector<string> octopi;

  // Construct from cin
  grid();

  // Accessing
  int H() const { return octopi.size(); }
  int W() const { return octopi[0].size(); }
  bool valid(coord const &c) const {
    return c.first >= 0 && c.first < W() && c.second >= 0 && c.second < H();
  }
  char &at(coord const &c) { return octopi[c.second][c.first]; }

  // Is an octopus going to flash?
  bool fully_charged(char octopus) const { return octopus > '9'; }

  // Increase charge on octopus; if it becomes fully charged then it
  // will flash and charge the octopi around it
  void charge(coord const &c);

  // Simulate a step, return number of flashes
  int step();
};

grid::grid() {
  string line;
  while (getline(cin, line)) {
    octopi.push_back(line);
    assert(octopi.back().size() == octopi.front().size());
  }
}

void grid::charge(coord const &c) {
  if (!valid(c))
    return;
  auto &octopus = at(c);
  if (fully_charged(octopus))
    // Already saw the flash of this octopus
    return;
  if (fully_charged(++octopus))
    // The octopus will flash and charge the adjacent octopi
    for (auto const &delta : adjacent)
      charge(c + delta);
}

int grid::step() {
  for (int i = 0; i < W(); ++i)
    for (int j = 0; j < H(); ++j)
      charge({i, j});
  int num_flashes = 0;
  for (auto &row : octopi)
    for (auto &octopus : row)
      if (fully_charged(octopus)) {
        octopus = '0';
        ++num_flashes;
      }
  return num_flashes;
}

void part1() {
  grid g;
  int total_flashes = 0;
  for (int _ = 0; _ < 100; ++_)
    total_flashes += g.step();
  cout << total_flashes << '\n';
}

void part2() {
  grid g;
  int const max_flashes = g.H() * g.W();
  for (int step = 1;; ++step)
    if (g.step() == max_flashes) {
      cout << step << '\n';
      return;
    }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <part>\n";
    return 1;
  }
  if (*argv[1] == '1')
    part1();
  else
    part2();
  return 0;
}
