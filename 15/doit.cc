// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cassert>

using namespace std;

struct cave {
  // Risk level at each tile
  vector<string> risk_level;

  // Construct from cin
  cave();

  int H() const { return risk_level.size(); }
  int W() const { return risk_level[0].size(); }
  int risk_at(int i, int j) const { return risk_level[i][j] - '0'; }

  // Expand the map as required for part 2, return *this
  cave &expand_map();

  // Compute risk of safest path
  int lowest_risk() const;
};

cave::cave() {
  string line;
  while (getline(cin, line)) {
    risk_level.push_back(line);
    assert(risk_level.back().size() == risk_level.front().size());
  }
}

cave &cave::expand_map() {
  auto shift = [](string const &s) {
    string result(s);
    for (auto &c : result)
      c = c == '9' ? '1' : c + 1;
    return result;
  };
  int const num_times = 5 - 1;
  for (auto &s : risk_level) {
    string next = shift(s);
    for (int k = 0; k < num_times; ++k) {
      s += next;
      next = shift(next);
    }
  }
  int const original_H = H();
  for (int k = 0; k < num_times; ++k)
    for (int i = 0; i < original_H; ++i)
      risk_level.push_back(shift(risk_level[k * original_H + i]));
  return *this;
}

int cave::lowest_risk() const {
  // Worst case risk is no more than this...
  int const max_risk = W() * H() * 10;
  // Minimal risk path found so far from (0, 0)
  vector<vector<int>> minimal_risk(H(), vector<int>(W(), max_risk));
  minimal_risk[0][0] = 0;
  // Tiles whose minimal risk is known
  vector<vector<bool>> visited(H(), vector<bool>(W(), false));
  // Risk comparison
  auto less_risky = [&](pair<int, int> const &p1, pair<int, int> const &p2) {
    auto [i1, j1] = p1;
    auto [i2, j2] = p2;
    if (minimal_risk[i1][j1] != minimal_risk[i2][j2])
      // Prefer lower risk
      return minimal_risk[i1][j1] < minimal_risk[i2][j2];
    // Same risk, just some total order
    return p1 < p2;
  };
  // Frontier (ideally a priority queue, but for convenience I'll just
  // use a set)
  set<pair<int, int>, decltype(less_risky)> Q(less_risky);
  Q.emplace(0, 0);
  // Q updating, consider reaching (i, j) with some candidate risk
  // from an adjacent tile
  auto consider = [&](int i, int j, int candidate_risk) {
    if (i < 0 || i >= H() || j < 0 || j >= W())
      // Out of bounds
      return;
    if (visited[i][j])
      // The minimal risk for (i, j) is already determined
      return;
    int risk = candidate_risk + risk_at(i, j);
    if (minimal_risk[i][j] <= risk)
      // The risk along this path is not better than a previously
      // found path
      return;
    // Because I'm using a set and the ordering depends on
    // minimal_risk, I have to remove any existing entry in Q
    Q.erase({i, j});
    minimal_risk[i][j] = risk;
    Q.emplace(i, j);
  };
  // Dijklmnopqrstra
  while (!Q.empty()) {
    auto [i, j] = *Q.begin();
    Q.erase(Q.begin());
    // This tile's risk is now known
    visited[i][j] = true;
    int current_risk = minimal_risk[i][j];
    consider(i + 1, j, current_risk);
    consider(i - 1, j, current_risk);
    consider(i, j + 1, current_risk);
    consider(i, j - 1, current_risk);
  }
  return minimal_risk[H() - 1][W() - 1];
}

void part1() { cout << cave().lowest_risk() << '\n'; }
void part2() { cout << cave().expand_map().lowest_risk() << '\n'; }

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
