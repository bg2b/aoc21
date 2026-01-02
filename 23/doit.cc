// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <cassert>

using namespace std;

// Extra lines for part 2
string extra[2] = {"#D#C#B#A#", "#D#B#A#C#"};

// The state of the burrow
struct burrow {
  // ..x.x.x.x..
  // x = forbidden outside-of-room space, but it's convenient to have
  // an index for those even though they can't be used
  array<char, 11> hallway;
  // 4 rooms, variable depth, top spot is index 0
  array<vector<char>, 4> rooms;

  // Construct initial state from cin
  burrow(bool part2);

  bool is_clear(char c) const { return c == '.'; }
  bool is_amphipod(char c) const { return c >= 'A' && c <= 'D'; }

  // How much does each step for the amphipod cost?
  unsigned move_cost(char amphipod) const;

  // The hall position just outside the door for the given room
  unsigned outside_door(unsigned r) const;

  // The room an amphipod wants to get into
  unsigned target_room(char amphipod) const { return amphipod - 'A'; }

  // Does a room contain only the desired types of amphipods
  bool all_cozy(unsigned r) const;

  // Is a hall position forbidden?
  bool is_forbidden(unsigned h) const;

  // Possible moves out of a room and associated cost
  list<pair<burrow, unsigned>> move_out() const;
  // Possible moves into a room and associated cost
  list<pair<burrow, unsigned>> move_in() const;
  // All successors and costs
  list<pair<burrow, unsigned>> successors() const;

  // Everyone in their room?
  bool all_home() const;
};

burrow::burrow(bool part2) {
  string line;
  cin >> line;
  assert(line == string(hallway.size() + 2, '#'));
  cin >> line;
  assert(line.size() == hallway.size() + 2);
  line = line.substr(1);
  for (auto &c : hallway) {
    c = line[0];
    line = line.substr(1);
  }
  vector<string> lines;
  while (cin >> line)
    lines.push_back(line);
  assert(lines.size() == 3);
  if (part2)
    lines = {lines[0], extra[0], extra[1], lines[1], lines[2]};
  // Read room rows until we hit the bottom
  for (auto line : lines) {
    while (!line.empty() && line[0] == '#')
      line = line.substr(1);
    if (line.empty() || line[0] == '#')
      break;
    assert(line.size() >= 2 * rooms.size());
    for (unsigned r = 0; r < rooms.size(); ++r)
      rooms[r].push_back(line[2 * r]);
  }
}

unsigned burrow::move_cost(char amphipod) const {
  assert(is_amphipod(amphipod));
  unsigned cost = 1;
  for (char c = 'A'; c != amphipod; ++c)
    cost *= 10;
  return cost;
}

unsigned burrow::outside_door(unsigned r) const {
  unsigned const end_sz = (hallway.size() - (2 * rooms.size() - 1)) / 2;
  return 2 * r + end_sz;
}

bool burrow::all_cozy(unsigned r) const {
  char amphipod = 'A' + r;
  for (unsigned i = 0; i < rooms[r].size(); ++i)
    if (!is_clear(rooms[r][i]) && rooms[r][i] != amphipod)
      return false;
  return true;
}

bool burrow::is_forbidden(unsigned h) const {
  for (unsigned r = 0; r < rooms.size(); ++r)
    if (outside_door(r) == h)
      return true;
  return false;
}

list<pair<burrow, unsigned>> burrow::move_out() const {
  list<pair<burrow, unsigned>> result;
  for (unsigned r = 0; r < rooms.size(); ++r) {
    if (all_cozy(r))
      // No point in moving out of this room
      continue;
    unsigned i = 0;
    while (i < rooms[r].size() && is_clear(rooms[r][i]))
      ++i;
    if (i >= rooms[r].size())
      // Room is empty
      continue;
    char amphipod = rooms[r][i];
    assert(is_amphipod(amphipod));
    // Move into the hall
    unsigned steps = i + 1;
    unsigned hall_pos = outside_door(r);
    assert(is_clear(hallway[hall_pos]));
    // Walk left or right
    for (int dir : {-1, +1}) {
      int h = hall_pos + dir;
      unsigned s = steps + 1;
      while (h >= 0 && h < int(hallway.size())) {
        if (!is_clear(hallway[h]))
          // Someone is in the way
          break;
        if (!is_forbidden(h)) {
          burrow next(*this);
          next.rooms[r][i] = '.';
          next.hallway[h] = amphipod;
          result.emplace_back(next, s * move_cost(amphipod));
        }
        h += dir;
        ++s;
      }
    }
  }
  return result;
}

list<pair<burrow, unsigned>> burrow::move_in() const {
  list<pair<burrow, unsigned>> result;
  for (unsigned h = 0; h < hallway.size(); ++h) {
    if (is_clear(hallway[h]))
      continue;
    char amphipod = hallway[h];
    // Room the amphipod wants
    unsigned r = target_room(amphipod);
    // Room must be empty or only have the same amphipod
    if (!all_cozy(r))
      continue;
    // Check the steps along the hallway
    unsigned hr = outside_door(r);
    assert(h != hr);
    int dir = h < hr ? +1 : -1;
    unsigned hc = h + dir;
    unsigned steps = 1;
    while (hc != hr && is_clear(hallway[hc])) {
      hc += dir;
      ++steps;
    }
    if (!is_clear(hallway[hc]))
      // Hallway blocked
      continue;
    // Move in
    unsigned i = 0;
    ++steps;
    // All the way down
    while (i + 1 < rooms[r].size() && is_clear(rooms[r][i + 1])) {
      ++i;
      ++steps;
    }
    burrow next(*this);
    next.hallway[h] = '.';
    next.rooms[r][i] = amphipod;
    result.emplace_back(next, steps * move_cost(amphipod));
  }
  return result;
}

list<pair<burrow, unsigned>> burrow::successors() const {
  // If someone can be moved in, that takes precendence (moving out
  // first can't possibly help)
  auto in = move_in();
  if (!in.empty())
    return in;
  return move_out();
}

bool burrow::all_home() const {
  for (auto c : hallway)
    if (!is_clear(c))
      return false;
  for (unsigned r = 0; r < rooms.size(); ++r)
    if (!all_cozy(r))
      return false;
  return true;
}

// Some random total order
bool operator<(burrow const &b1, burrow const &b2) {
  if (b1.hallway != b2.hallway)
    return b1.hallway < b2.hallway;
  return b1.rooms < b2.rooms;
}

unsigned search(burrow const &start) {
  // States to explore, sorted by cost
  set<pair<unsigned, burrow>> to_search;
  to_search.emplace(0, start);
  // Best cost to reach each state
  map<burrow, unsigned> best_cost;
  best_cost[start] = 0;
  // Search from minimum cost
  while (!to_search.empty()) {
    auto [cost, b] = *to_search.begin();
    to_search.erase(to_search.begin());
    // Skip if we've found a better path
    if (auto it = best_cost.find(b); it != best_cost.end() && it->second < cost)
      continue;
    // Done?
    if (b.all_home())
      return cost;
    for (auto const &[next, move_cost] : b.successors()) {
      unsigned next_cost = cost + move_cost;
      auto it = best_cost.find(next);
      if (it == best_cost.end() || next_cost < it->second) {
        best_cost[next] = next_cost;
        to_search.emplace(next_cost, next);
      }
    }
  }
  assert(false && "no solution found");
}

void solve(bool part2) {
  burrow b(part2);
  cout << search(b) << '\n';
}

void part1() { solve(false); }
void part2() { solve(true); }

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
