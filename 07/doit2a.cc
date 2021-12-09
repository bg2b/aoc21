#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <assert.h>

using namespace std;

int fuel_for(vector<int> const &crabs, int pos) {
  int fuel = 0;
  for (auto i : crabs) {
    int nsteps = abs(i - pos);
    fuel += nsteps * (nsteps + 1) / 2;
  }
  return fuel;
}

int golden_section_search(vector<int> const &crabs) {
  int a = 0;
  int b = crabs.size() - 1;
  int fuel_a = fuel_for(crabs, a);
  int fuel_b = fuel_for(crabs, b);
  while (b - a >= 3) {
    int c = 0.382 * b + 0.618 * a;
    int d = 0.382 * a + 0.618 * b;
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
  int c = (a + b) / 2;
  int fuel_c = fuel_for(crabs, c);
  if (fuel_a < fuel_b && fuel_a < fuel_c)
    return a;
  if (fuel_b < fuel_c)
    return b;
  return c;
}

int main(int argc, char **argv) {
  vector<int> crabs;
  int c;
  while (cin >> c)
    crabs.push_back(c);
  sort(crabs.begin(), crabs.end());
  int best_pos = golden_section_search(crabs);
  int min_fuel = fuel_for(crabs, best_pos);
  printf("%d fuel at %d\n", min_fuel, best_pos);
  return 0;
}
