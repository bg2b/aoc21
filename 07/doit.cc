// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <algorithm>
#include <vector>
#include <cassert>

// For part 1 with n crabs the best position is at the position of the
// n/2 crab.  To see that this gives the minimum fuel, consider the
// "derivative".  One position farther to the right, half the crabs
// have to move one extra step, and half have to move one less step,
// so the derivative is zero.

// For part 2, the function giving the fuel is unimodal, but the best
// position need not align with any crab.  Brute force is OK since the
// crabs only span about 1000 positions.

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
  int fuel = 0;
  for (auto i : crabs) {
    int nsteps = abs(i - pos);
    fuel += nsteps * (nsteps + 1) / 2;
  }
  return fuel;
}

void part2() {
  auto crabs = read_crabs();
  int min_fuel = fuel_for(crabs, crabs.front());
  for (int pos = crabs.front() + 1; pos <= crabs.back(); ++pos)
    min_fuel = min(min_fuel, fuel_for(crabs, pos));
  cout << min_fuel << '\n';
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
