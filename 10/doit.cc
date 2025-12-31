// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

using namespace std;

struct token {
  // 0 for (), 1 for [], etc.
  int type;
  // Was this a closing token?
  bool closing;

  token(char c);

  int corruption() const { return vector<int>{3, 57, 1197, 25137}[type]; }
  int completion() const { return type + 1; }
};

token::token(char c) {
  static string valid = "()[]{}<>";
  auto p = valid.find(c);
  assert(p != string::npos);
  type = p / 2;
  closing = (p % 2 != 0);
}

// Compute corruption and completion scores for a line.  Corruption
// score is 0 for a line that is not corrupt.  Completion score is 0
// for a line that is corrupt.
pair<long, long> scores(string const &line) {
  vector<token> open;
  for (auto c : line) {
    token t(c);
    if (t.closing) {
      if (open.empty() || t.type != open.back().type)
        return {t.corruption(), 0};
      open.pop_back();
    } else
      open.push_back(t);
  }
  long completion = 0;
  while (!open.empty()) {
    completion *= 5;
    completion += open.back().completion();
    open.pop_back();
  }
  return {0, completion};
}

void solve(bool part1) {
  string line;
  long total_corruption = 0;
  vector<long> completions;
  while (cin >> line) {
    auto [corruption, completion] = scores(line);
    total_corruption += corruption;
    if (corruption == 0)
      completions.push_back(completion);
  }
  if (part1)
    cout << total_corruption << '\n';
  else {
    assert(!completions.empty());
    sort(completions.begin(), completions.end());
    cout << completions[completions.size() / 2] << '\n';
  }
}

void part1() { solve(true); }
void part2() { solve(false); }

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <part>\n";
    return 1;
  }
  if (string(argv[1]) == "1")
    part1();
  else
    part2();
  return 0;
}
