#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct token {
  int type;
  bool closing;

  token(char c);

  bool is_mismatch(token const &t) const;
  int points() const;
};

token::token(char c) {
  static string valid("()[]{}<>");
  auto p = valid.find(c);
  assert(p != string::npos);
  type = p / 2;
  closing = (p % 2 != 0);
}

bool token::is_mismatch(token const &t) const {
  return (type != t.type || closing == t.closing);
}

int token::points() const {
  static int pts[4] = { 3, 57, 1197, 25137 };
  return pts[type];
}

int score(string const &line) {
  vector<token> open;
  for (auto c : line) {
    token t(c);
    if (t.closing) {
      if (open.empty() || t.is_mismatch(open.back()))
	return t.points();
      open.pop_back();
    } else
      open.push_back(t);
  }
  return 0;
}

int main(int argc, char **argv) {
  int total_score = 0;
  string line;
  while (cin >> line)
    total_score += score(line);
  printf("%d\n", total_score);
  return 0;
}
