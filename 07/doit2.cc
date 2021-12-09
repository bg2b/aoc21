#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <assert.h>

using namespace std;

int main(int argc, char **argv) {
  vector<int> crabs;
  int c;
  while (cin >> c)
    crabs.push_back(c);
  sort(crabs.begin(), crabs.end());
  int min_fuel = 0;
  for (auto i : crabs)
    min_fuel += i * (i + 1) / 2;
  int best_pos = 0;
  for (int pos = crabs.front(); pos <= crabs.back(); ++pos) {
    int fuel = 0;
    for (auto i : crabs) {
      int nsteps = abs(i - pos);
      fuel += nsteps * (nsteps + 1) / 2;
    }
    if (fuel < min_fuel) {
      min_fuel = fuel;
      best_pos = pos;
    }
  }
  printf("%d fuel at %d\n", min_fuel, best_pos);
  return 0;
}
