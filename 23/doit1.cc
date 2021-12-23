#include <iostream>
#include <string>
#include <array>
#include <list>
#include <map>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct burrow {
  // ..x.x.x.x..
  // x = forbidden outside-of-room space, but it's convenient to have
  // an index for those even though they can't be used
  array<char, 11> hallway;
  // 4 rooms, each two spots, top spot is index 0
  array<array<char, 2>, 4> rooms;

  burrow(istream &in);

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

  void print() const;
};

burrow::burrow(istream &in) {
  string line;
  in >> line;
  assert(line == string(hallway.size() + 2, '#'));
  in >> line;
  assert(line.size() == hallway.size() + 2);
  line = line.substr(1);
  for (auto &c : hallway) {
    c = line[0];
    line = line.substr(1);
  }
  for (unsigned i = 0; i < rooms[0].size(); ++i) {
    in >> line;
    while (!line.empty() && line[0] == '#')
      line = line.substr(1);
    assert(line.size() >= 2 * rooms.size());
    for (unsigned r = 0; r < rooms.size(); ++r)
      rooms[r][i] = line[2 * r];
  }
  in >> line;
  assert(line == string(line.size(), '#'));
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
    for (int dir : { -1, +1 }) {
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

void burrow::print() const {
  printf(" #############\n");
  printf(" #");
  for (unsigned i = 0; i < hallway.size(); ++i)
    fputc(hallway[i], stdout);
  printf("#\n");
  printf(" ###%c#%c#%c#%c###\n",
	 rooms[0][0], rooms[1][0], rooms[2][0], rooms[3][0]);
  for (unsigned i = 1; i < rooms[0].size(); ++i)
    printf("   #%c#%c#%c#%c#\n",
	   rooms[0][i], rooms[1][i], rooms[2][i], rooms[3][i]);
  printf("   #########\n\n");
}

// Some random total order
bool operator<(burrow const &b1, burrow const &b2) {
  if (b1.hallway < b2.hallway)
    return true;
  if (b2.hallway < b1.hallway)
    return false;
  return b1.rooms < b2.rooms;
}

pair<unsigned, list<burrow>> search(burrow const &start) {
  // Things to explore
  multimap<unsigned, list<burrow>> to_search;
  to_search.emplace(0, list<burrow>(1, start));
  // Pointers from final burrow state to the corresponding entry in
  // to_search
  map<burrow, decltype(to_search)::iterator> path_to;
  path_to[start] = to_search.begin();
  // Search from minimum cost
  while (!to_search.empty()) {
    auto cb = *to_search.begin();
    unsigned cost = cb.first;
    list<burrow> const &bs = cb.second;
    burrow const &b = bs.back();
    // Remove from path_to
    auto pt = path_to.find(b);
    assert(pt != path_to.end() && pt->second == to_search.begin());
    path_to.erase(pt);
    to_search.erase(to_search.begin());
    // Done?
    if (b.all_home())
      return cb;
    for (auto const &next : b.successors()) {
      unsigned next_cost = cost + next.second;
      auto pt = path_to.find(next.first);
      if (pt != path_to.end()) {
	if (pt->second->first <= next_cost)
	  // Already reached this state via a path that's at least as
	  // good
	  continue;
	// Existing solution is worse, drop it
	to_search.erase(pt->second);
	path_to.erase(pt);
      }
      auto next_bs(bs);
      next_bs.push_back(next.first);
      path_to[next.first] = to_search.emplace(next_cost, next_bs);
    }
  }
  assert(false && "no solution found");
}

int main(int argc, char **argv) {
  burrow b(cin);
  auto cost_and_steps = search(b);
  if (argc > 1)
    for (auto const &step : cost_and_steps.second)
      step.print();
  printf("Cost %u\n", cost_and_steps.first);
  return 0;
}
