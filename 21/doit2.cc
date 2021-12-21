#include <iostream>
#include <map>
#include <array>
#include <algorithm>
#include <stdio.h>
#include <assert.h>

using namespace std;

unsigned const min_die = 3;
unsigned const max_die = 9;
unsigned const win = 21;

struct possible_rolls {
  array<unsigned, max_die - min_die + 1> total_counts;

  possible_rolls();

  // In how many universes will the die produce a given total?
  unsigned occurrences_of(unsigned total) const;
};

possible_rolls::possible_rolls() {
  for (auto &count : total_counts)
    count = 0;
  for (unsigned r1 : { 1, 2, 3 })
    for (unsigned r2 : { 1, 2, 3 })
      for (unsigned r3 : { 1, 2, 3 })
	++total_counts[r1 + r2 + r3 - min_die];
}

unsigned possible_rolls::occurrences_of(unsigned total) const {
  assert(min_die <= total && total <= max_die);
  return total_counts[total - min_die];
}

possible_rolls rolls;

struct state {
  // The spaces the players are on
  unsigned space[2];
  // The points they have
  unsigned points[2];
  // The player that goes next
  int turn;

  // Starting state
  state(int p1, int p2);
  // Update with rolling a given total
  state(state const &s, unsigned total);

  // Is this an ending state?  And if so, who won?
  bool is_win() const { return points[0] >= win || points[1] >= win; }
  int winner() const { assert(is_win()); return points[0] >= win ? 0 : 1; }

  // Used for comparing states.  The priority is such that if s1 has
  // fewer points for the player whose turn it is than s2, then
  // s1.priority() < s2.priority().  Also, s1.priority() !=
  // s2.priority() unless space, points, and turn all match.
  unsigned priority() const;
};

state::state(int p1, int p2) : turn(0) {
  space[0] = p1;
  space[1] = p2;
  points[0] = 0;
  points[1] = 0;
}

state::state(state const &s, unsigned total) {
  space[0] = s.space[0];
  space[1] = s.space[1];
  points[0] = s.points[0];
  points[1] = s.points[1];
  turn = s.turn;
  space[turn] += total;
  space[turn] = (space[turn] - 1) % 10 + 1;
  points[turn] += space[turn];
  turn = 1 - turn;
}

unsigned state::priority() const {
  assert(!is_win());
  // 100 makes it easy to see what state the priority corresponds to
  unsigned result = points[turn];
  result = 100 * result + points[1 - turn];
  result = 100 * result + space[turn];
  result = 100 * result + space[1 - turn];
  result = 100 * result + turn;
  return result;
}

bool operator<(state const &s1, state const &s2) {
  return s1.priority() < s2.priority();
}

size_t play(int p1, int p2) {
  size_t wins_for[2] = { 0, 0 };
  // Maps states to the number of occurrences of the state.  States
  // are searched in order of increasing points for the player whose
  // turn it is.
  map<state, size_t> states;
  states[state(p1, p2)] = 1;
  while (!states.empty()) {
    // State to search
    auto s = states.begin()->first;
    auto count = states.begin()->second;
    states.erase(states.begin());
    assert(!s.is_win());
    // Expand successors
    for (unsigned die = min_die; die <= max_die; ++die) {
      unsigned num_rolls = rolls.occurrences_of(die);
      state next(s, die);
      if (next.is_win())
	wins_for[next.winner()] += num_rolls * count;
      else
	states[next] += num_rolls * count;
      }
  }
  return max(wins_for[0], wins_for[1]);
}

int main(int argc, char **argv) {
  int p1, p2;
  cin >> p1 >> p2;
  printf("%zu wins\n", play(p1, p2));
  return 0;
}
