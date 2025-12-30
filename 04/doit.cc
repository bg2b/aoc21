// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <set>
#include <sstream>
#include <utility>
#include <vector>
#include <cassert>

using namespace std;

// Boards get updated during play
struct board {
  // All the unmarked numbers
  set<int> unmarked;
  // Unmarked numbers in rows and columns
  set<int> rows[5];
  set<int> cols[5];

  // Construct from a row-major list of numbers
  board(vector<int> const &nums);

  // Mark the nubmer n, return score, or 0 if not yet winning
  int mark(int n);

  // Step where the card wins, and the corresponding score.  The step
  // is numbers.size() if the card doesn't win.
  pair<int, int> play(vector<int> const &numbers);
};

board::board(vector<int> const &nums) {
  assert(nums.size() == 25);
  for (int i = 0; i < 25; ++i) {
    auto n = nums[i];
    unmarked.insert(n);
    rows[i / 5].insert(n);
    cols[i % 5].insert(n);
  }
}

int board::mark(int n) {
  if (!unmarked.contains(n))
    // Not on this board
    return 0;
  unmarked.erase(n);
  bool bingo = false;
  for (int i = 0; i < 5; ++i) {
    rows[i].erase(n);
    cols[i].erase(n);
    bingo = bingo || rows[i].empty() || cols[i].empty();
  }
  int score = 0;
  if (bingo) {
    for (auto i : unmarked)
      score += i;
    score *= n;
  }
  return score;
}

pair<int, int> board::play(vector<int> const &numbers) {
  for (unsigned i = 0; i < numbers.size(); ++i) {
    int score = mark(numbers[i]);
    if (score)
      return {i, score};
  }
  return {numbers.size(), 0};
}

void solve(bool find_first) {
  string line;
  getline(cin, line);
  line.push_back(',');
  stringstream ss(line);
  vector<int> drawn;
  int num;
  char comma;
  while (ss >> num >> comma)
    drawn.push_back(num);
  int best_step = find_first ? drawn.size() : 0;
  int best_score = 0;
  vector<int> nums(25, 0);
  while (cin >> nums[0]) {
    for (int i = 1; i < 25; ++i)
      cin >> nums[i];
    auto [step, score] = board(nums).play(drawn);
    bool dominated = find_first ? step < best_step : step > best_step;
    if (dominated) {
      best_step = step;
      best_score = score;
    }
  }
  cout << best_score << '\n';
}

void part1() { solve(true); }
void part2() { solve(false); }

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
