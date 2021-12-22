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

// Some total order on states for use in a map
bool operator<(state const &s1, state const &s2) {
  for (int i = 0; i < 2; ++i) {
    if (s1.space[i] < s2.space[i])
      return true;
    if (s2.space[i] < s1.space[i])
      return false;
    if (s1.points[i] < s2.points[i])
      return true;
    if (s2.points[i] < s1.points[i])
      return false;
  }
  return s1.turn < s2.turn;
}

using wins = pair<size_t, size_t>;

wins play(state const &s, map<state, wins> &cache) {
  if (s.is_win())
    // Immediate win
    return { s.winner() == 0, s.winner() == 1 };
  if (cache.find(s) != cache.end())
    // Already computed
    return cache[s];
  wins result(0, 0);
  for (unsigned die = min_die; die <= max_die; ++die) {
    // Add up wins for all successor states
    unsigned num_rolls = rolls.occurrences_of(die);
    wins next_wins = play(state(s, die), cache);
    result.first += num_rolls * next_wins.first;
    result.second += num_rolls * next_wins.second;
  }
  cache[s] = result;
  return result;
}

size_t play(int p1, int p2) {
  map<state, wins> cache;
  wins w = play(state(p1, p2), cache);
  return max(w.first, w.second);
}

int main(int argc, char **argv) {
  int p1, p2;
  cin >> p1 >> p2;
  printf("%zu wins\n", play(p1, p2));
  return 0;
}
