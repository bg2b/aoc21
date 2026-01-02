// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <map>
#include <array>
#include <cassert>

using namespace std;

// Part 1: Deterministic dice

int roll(int &die, int &num_rolls) {
  ++num_rolls;
  int result = die++;
  if (die > 100)
    die = 1;
  return result;
}

void part1() {
  int p1, p2;
  cin >> p1 >> p2;
  int space[2] = {p1, p2};
  int points[2] = {0, 0};
  int turn = 0;
  int die = 1;
  int num_rolls = 0;
  while (points[0] < 1000 && points[1] < 1000) {
    space[turn] += roll(die, num_rolls);
    space[turn] += roll(die, num_rolls);
    space[turn] += roll(die, num_rolls);
    space[turn] = (space[turn] - 1) % 10 + 1;
    points[turn] += space[turn];
    turn = 1 - turn;
  }
  cout << min(points[0], points[1]) * num_rolls << '\n';
}

// Part 2: Quantum dice with memoization

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
  for (unsigned r1 : {1, 2, 3})
    for (unsigned r2 : {1, 2, 3})
      for (unsigned r3 : {1, 2, 3})
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
  unsigned points[2]{0, 0};
  // The player that goes next
  int turn{0};

  // Starting state
  state(int p1, int p2);
  // Update with rolling a given total
  state(state const &s, unsigned total);

  // Is this an ending state?  And if so, who won?
  bool is_win() const { return points[0] >= win || points[1] >= win; }
  int winner() const {
    assert(is_win());
    return points[0] >= win ? 0 : 1;
  }
};

state::state(int p1, int p2) {
  space[0] = p1;
  space[1] = p2;
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

// Some random total order on states for use in a map
bool operator<(state const &s1, state const &s2) {
  for (int i = 0; i < 2; ++i) {
    if (s1.space[i] != s2.space[i])
      return s1.space[i] < s2.space[i];
    if (s1.points[i] != s2.points[i])
      return s1.points[i] < s2.points[i];
  }
  return s1.turn < s2.turn;
}

using wins = pair<size_t, size_t>;

wins play(state const &s, map<state, wins> &cache) {
  if (s.is_win())
    // Immediate win
    return {s.winner() == 0, s.winner() == 1};
  if (auto p = cache.find(s); p != cache.end())
    // Already computed
    return p->second;
  wins result(0, 0);
  for (unsigned die = min_die; die <= max_die; ++die) {
    // Add up wins for all successor states
    unsigned num_rolls = rolls.occurrences_of(die);
    wins next_wins = play(state(s, die), cache);
    result.first += num_rolls * next_wins.first;
    result.second += num_rolls * next_wins.second;
  }
  return cache[s] = result;
}

void part2() {
  int p1, p2;
  cin >> p1 >> p2;
  map<state, wins> cache;
  wins w = play(state(p1, p2), cache);
  cout << max(w.first, w.second) << '\n';
}

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
