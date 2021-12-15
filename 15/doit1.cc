#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <set>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct cave {
  vector<string> risk_level;

  cave(istream &in);

  int H() const { return risk_level.size(); }
  int W() const { return risk_level[0].size(); }

  int risk_at(int i, int j) const;

  int total_risk() const;
};

cave::cave(istream &in) {
  string line;
  while (getline(in, line)) {
    risk_level.push_back(line);
    assert(risk_level.back().size() == risk_level.front().size());
  }
}

int cave::risk_at(int i, int j) const {
  assert(i >= 0 && i < H() && j >= 0 && j < W());
  return risk_level[i][j] - '0';
}

int cave::total_risk() const {
  // Minimal risk path from (0, 0)
  int const max_risk = numeric_limits<int>::max();
  vector<vector<int>> minimal_risk(H(), vector<int>(W(), max_risk));
  minimal_risk[0][0] = 0;
  // Visited squares
  set<pair<int, int>> visited;
  // Frontier (ideally a priority queue, but whatevs)
  set<pair<int, int>> Q;
  Q.emplace(0, 0);
  // Risk comparison
  auto less_risky = [&](pair<int, int> const &p1, pair<int, int> const &p2) {
    int i1 = p1.first;
    int j1 = p1.second;
    int i2 = p2.first;
    int j2 = p2.second;
    return minimal_risk[i1][j1] < minimal_risk[i2][j2];
  };
  // Q updating, consider reaching (i, j) with some candidate risk
  // from an adjacent square
  auto consider = [&](int i, int j, int candidate_risk) {
    if (i < 0 || i >= H() || j < 0 || j >= W())
      // Out of bounds
      return;
    if (visited.find(make_pair(i, j)) != visited.end())
      // This node has already been done
      return;
    int risk = candidate_risk + risk_at(i, j);
    if (minimal_risk[i][j] <= risk)
      // The risk along this path is not better than a previously
      // found path
      return;
    minimal_risk[i][j] = risk;
    Q.emplace(i, j);
  };
  // Dijklmnopqrstra
  while (!Q.empty()) {
    auto current = *Q.begin();
    for (auto const &next : Q)
      if (less_risky(next, current))
	current = next;
    Q.erase(current);
    visited.insert(current);
    int i = current.first;
    int j = current.second;
    int current_risk = minimal_risk[i][j];
    consider(i + 1, j, current_risk);
    consider(i - 1, j, current_risk);
    consider(i, j + 1, current_risk);
    consider(i, j - 1, current_risk);
  }
  return minimal_risk[H() - 1][W() - 1];
}

int main(int argc, char **argv) {
  printf("Risk %d\n", cave(cin).total_risk());
  return 0;
}
