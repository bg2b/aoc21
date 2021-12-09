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
  int pos = crabs[crabs.size()/2];
  int fuel = 0;
  for (auto i : crabs)
    fuel += abs(i - pos);
  printf("%d fuel at %d\n", fuel, pos);
  return 0;
}
