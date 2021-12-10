#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
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
  return type + 1;
}

long completion_score(string const &line) {
  vector<token> open;
  for (auto c : line) {
    token t(c);
    if (t.closing) {
      if (open.empty() || t.is_mismatch(open.back()))
	// Not incomplete
	return -1;
      open.pop_back();
    } else
      open.push_back(t);
  }
  long result = 0;
  while (!open.empty()) {
    result *= 5;
    result += open.back().points();
    open.pop_back();
  }
  return result;
}

int main(int argc, char **argv) {
  vector<long> scores;
  string line;
  while (cin >> line) {
    long score = completion_score(line);
    if (score >= 0)
      scores.push_back(score);
  }
  sort(scores.begin(), scores.end());
  assert(!scores.empty());
  printf("%ld\n", scores[scores.size() / 2]);
  return 0;
}
