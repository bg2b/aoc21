#include <iostream>
#include <sstream>
#include <set>
#include <vector>
#include <utility>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct board {
  set<int> unmarked;
  set<int> rows[5];
  set<int> cols[5];

  board(istream &in);

  int mark(int n);

  // Step where the card wins and score
  pair<unsigned, int> play(vector<int> const &numbers);
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

pair<unsigned, int> board::play(vector<int> const &numbers) {
  for (unsigned i = 0; i < numbers.size(); ++i) {
    int score = mark(numbers[i]);
    if (score)
      return make_pair(i, score);
  }
  return make_pair(0, 0);
}

int main(int argc, char **argv) {
  string line;
  getline(cin, line);
  stringstream cnums(line);
  vector<int> numbers;
  int num;
  while (cnums >> num)
    numbers.push_back(num);
  unsigned last_winner = 0;
  int winner_score = 0;
  while (getline(cin, line) && cin.peek() != EOF) {
    assert(line == "");
    auto this_card = board(cin).play(numbers);
    if (this_card.first > last_winner) {
      last_winner = this_card.first;
      winner_score = this_card.second;
    }
  }
  printf("Score %d\n", winner_score);
  return 0;
}
