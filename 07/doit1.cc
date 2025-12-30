// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit1 doit1.cc
// ./doit1 1 < input  # part 1
// ./doit1 2 < input  # part 2

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <numbers>
#include <cassert>

// This uses a golden section search for part 2

using namespace std;

// Read the list of crabs and sort
vector<int> read_crabs() {
  vector<int> crabs;
  int c;
  while (cin >> c) {
    crabs.push_back(c);
    char comma;
    cin >> comma;
  }
  sort(crabs.begin(), crabs.end());
  assert(!crabs.empty());
  return crabs;
}

void part1() {
  auto crabs = read_crabs();
  int pos = crabs[crabs.size() / 2];
  int fuel = 0;
  for (auto i : crabs)
    fuel += abs(i - pos);
  cout << fuel << '\n';
}

// Part 2 fuel for all crabs to move to pos
int fuel_for(vector<int> const &crabs, int pos) {
  // This is cached since the golden search often reuses function
  // evaluations (though since positions must be integers, it's not as
  // much as in the continuous case)
  static map<int, int> cache;
  if (!cache.contains(pos)) {
    int &fuel = cache[pos];
    for (auto i : crabs) {
      int nsteps = abs(i - pos);
      fuel += nsteps * (nsteps + 1) / 2;
    }
  }
  return cache.at(pos);
}

void part2() {
  auto crabs = read_crabs();
  // Golden section search
  int a = crabs.front();
  int b = crabs.back();
  int fuel_a = fuel_for(crabs, a);
  int fuel_b = fuel_for(crabs, b);
  // Refine down to at most 3 consecutive positions
  while (b - a >= 3) {
    double const alpha = numbers::phi - 1.0;
    int c = alpha * a + (1.0 - alpha) * b;
    int d = (1.0 - alpha) * a + alpha * b;
    int fuel_c = fuel_for(crabs, c);
    int fuel_d = fuel_for(crabs, d);
    if (fuel_c >= fuel_d) {
      // min is in [c, b]
      a = c;
      fuel_a = fuel_c;
    } else {
      // min is in [a, d]
      b = d;
      fuel_b = fuel_d;
    }
  }
  // Minimum of the at-most-3
  int c = (a + b) / 2;
  int fuel_c = fuel_for(crabs, c);
  cout << min(min(fuel_a, fuel_b), fuel_c) << '\n';
}

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
