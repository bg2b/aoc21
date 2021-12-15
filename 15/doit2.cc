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

  string shift(string const &s) const;
  void expand_map();

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

string cave::shift(string const &s) const {
  string result(s);
  for (auto &c : result) {
    if (c == '9')
      c = '1';
    else
      ++c;
  }
  return result;
}

void cave::expand_map() {
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
  cave c(cin);
  c.expand_map();
  printf("Risk %d\n", c.total_risk());
  return 0;
}
