#include <iostream>
#include <set>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct board {
  set<int> unmarked;
  set<int> rows[5];
  set<int> cols[5];

  board(istream &in);

  int mark(int n);
};

board::board(istream &in) {
  for (int i = 0; i < 25; ++i) {
    int n;
    in >> n;
    assert(!in.fail());
    unmarked.insert(n);
    rows[i / 5].insert(n);
    cols[i % 5].insert(n);
  }
}

int board::mark(int n) {
  if (unmarked.find(n) != unmarked.end()) {
    unmarked.erase(n);
    bool bingo = false;
    for (int i = 0; i < 5; ++i) {
      rows[i].erase(n);
      if (rows[i].empty())
	bingo = true;
      cols[i].erase(n);
      if (cols[i].empty())
	bingo = true;
    }
    if (!bingo)
      return 0;
    int score = 0;
    for (auto i : unmarked)
      score += i;
    score *= n;
    return score;
  }
  return 0;
}

int main(int argc, char **argv) {
  board b(cin);
  int steps = 0;
  while (true) {
    ++steps;
    int n;
    cin >> n;
    if (cin.fail())
      break;
    int score = b.mark(n);
    if (score) {
      printf("%d %d\n", steps, score);
      break;
    }
  }
  return 0;
}
