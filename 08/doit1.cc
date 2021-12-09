#include <iostream>
#include <sstream>
#include <array>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct entry {
  array<string, 10> patterns;
  array<string, 4> output;

  entry(string const &line);

  int count1478() const;
};

entry::entry(string const &line) {
  stringstream in(line);
  for (size_t i = 0; i < patterns.size(); ++i)
    in >> patterns[i];
  for (size_t i = 0; i < output.size(); ++i)
    in >> output[i];
}

int entry::count1478() const {
  int result = 0;
  for (auto digit : output)
    switch (digit.size()) {
    case 2: // 1
    case 4: // 4
    case 3: // 7
    case 7: // 8
      ++result;
      break;
    default:
      ;
    }
  return result;
}

int main(int argc, char **argv) {
  string line;
  int total = 0;
  while (getline(cin, line))
    total += entry(line).count1478();
  printf("%d\n", total);
  return 0;
}
