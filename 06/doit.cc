// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <array>
#include <iostream>
#include <numeric>

using namespace std;

using nfish = long;

struct fish {
  // Number of fish
  array<nfish, 9> next_spawn{0};

  // Construct from cin
  fish();

  // Total number of fish
  nfish num_fish() const;

  // Simulate one day
  void day();
};

fish::fish() {
  int until_spawn;
  while (cin >> until_spawn) {
    ++next_spawn[until_spawn];
    char comma;
    cin >> comma;
  }
}

nfish fish::num_fish() const {
  return accumulate(next_spawn.begin(), next_spawn.end(), nfish(0));
}

void fish::day() {
  // nns = next next_spawn
  auto nns = decltype(next_spawn){0};
  // Count down
  for (size_t i = 1; i < next_spawn.size(); ++i)
    nns[i - 1] += next_spawn[i];
  // 0's reset to 6 and make an 8
  nns[6] += next_spawn[0];
  nns[8] += next_spawn[0];
  next_spawn = nns;
}

void solve(int num_days) {
  fish f;
  for (int _ = 0; _ < num_days; ++_)
    f.day();
  cout << f.num_fish() << '\n';
}

void part1() { solve(80); }
void part2() { solve(256); }

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
