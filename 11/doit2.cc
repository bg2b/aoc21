#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct grid {
  vector<string> octopi;

  grid(istream &in);

  int H() const { return octopi.size(); }
  int W() const { return octopi[0].size(); }

  bool valid(int i, int j) const;
  bool fully_charged(char octopus) const { return octopus > '9'; }

  // Increase charge on octopus; if it becomes fully charged then it
  // will flash and charge the octopi around it
  void charge(int i, int j);

  // Simulate a step, return number of flashes
  int step();
};

grid::grid(istream &in) {
  string line;
  while (getline(in, line)) {
    octopi.push_back(line);
    assert(octopi.back().size() == octopi.front().size());
  }
}

bool grid::valid(int i, int j) const {
  return (i >= 0 && i < W() && j >= 0 && j < H());
}

void grid::charge(int i, int j) {
  if (!valid(i, j))
    return;
  bool fully_charged_before = fully_charged(octopi[i][j]);
  bool fully_charged_now = fully_charged(++octopi[i][j]);
  if (fully_charged_now && !fully_charged_before)
    for (int di = -1; di <= +1; ++di)
      for (int dj = -1; dj <= +1; ++dj)
	if (di != 0 || dj != 0)
	  charge(i + di, j + dj);
}

int grid::step() {
  for (int i = 0; i < H(); ++i)
    for (int j = 0; j < W(); ++j)
      charge(i, j);
  int num_flashes = 0;
  for (auto &row : octopi)
    for (auto &octopus : row)
      if (fully_charged(octopus)) {
	octopus = '0';
	++num_flashes;
      }
  return num_flashes;
}

int main(int argc, char **argv) {
  grid g(cin);
  int const max_flashes = g.H() * g.W();
  for (int step = 1; ; ++step)
    if (g.step() == max_flashes) {
      printf("step %d\n", step);
      break;
    }
  return 0;
}
