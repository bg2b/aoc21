#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <assert.h>

using namespace std;

int main(int argc, char **argv) {
  int nlines = 0;
  vector<int> counts0;
  string line;
  while (getline(cin, line)) {
    ++nlines;
    auto l = line.size();
    if (counts0.empty())
      counts0=vector<int>(l, 0);
    assert(counts0.size() == l);
    for (size_t i = 0; i < l; ++i)
      if (line[i] == '0')
	++counts0[i];
  }
  int gamma = 0;
  int epsilon = 0;
  int digit = 1 << (counts0.size() - 1);
  for (int c0 : counts0) {
    int most_common;
    if (c0 > nlines - c0)
      most_common = 0;
    else
      most_common = 1;
    int least_common = !most_common;
    gamma += digit * most_common;
    epsilon += digit * least_common;
    assert(digit > 0);
    digit /= 2;
  }
  assert(digit == 0);
  printf("%d %d %d\n", gamma, epsilon, gamma * epsilon);
  return 0;
}
