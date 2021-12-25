#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct cucumbers {
  vector<string> sea_floor;

  int H() const { return sea_floor.size(); }
  int W() const { return sea_floor[0].size(); }

  cucumbers(istream &in);

  // Wrap coordinates beyond H() or W()
  void maybe_wrap(int &i, int &j) const;

  // Does wrapping, so access at H() or W() is OK
  bool is_empty(int i, int j) const;

  // Try to do one step of cucumbers of the given type, return true if
  // any moved
  bool step(char which);

  // Note | not ||
  bool step() { return step('>') | step('v'); }

  void print() const;
};

cucumbers::cucumbers(istream &in) {
  string line;
  while (getline(in, line)) {
    sea_floor.push_back(line);
    assert(sea_floor.back().size() == sea_floor.front().size());
  }
}

void cucumbers::maybe_wrap(int &i, int &j) const {
  if (i >= H())
    i -= H();
  if (j >= W())
    j -= W();
}

bool cucumbers::is_empty(int i, int j) const {
  maybe_wrap(i, j);
  return sea_floor[i][j] == '.';
}

bool cucumbers::step(char type) {
  int delta_i = (type == 'v' ? +1 : 0);
  int delta_j = (type == '>' ? +1 : 0);
  list<pair<int, int>> moving;
  for (int i = 0; i < H(); ++i)
    for (int j = 0; j < W(); ++j)
      if (sea_floor[i][j] == type && is_empty(i + delta_i, j + delta_j))
	moving.emplace_back(i, j);
  for (auto const &p : moving) {
    int i = p.first;
    int j = p.second;
    sea_floor[i][j] = '.';
    i += delta_i;
    j += delta_j;
    maybe_wrap(i, j);
    sea_floor[i][j] = type;
  }
  return !moving.empty();
}

void cucumbers::print() const {
  for (auto const &row : sea_floor)
    printf(" %s\n", row.c_str());
  printf("\n");
}

int main(int argc, char **argv) {
  cucumbers cucs(cin);
  int steps = 1;
  if (argc > 1)
    cucs.print();
  while (cucs.step())
    ++steps;
  if (argc > 1)
    cucs.print();
  printf("%d steps\n", steps);
  return 0;
}
