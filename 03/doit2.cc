#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <assert.h>

using namespace std;

int to_int(string const &bits) {
  int result = 0;
  for (auto d : bits) {
    result *= 2;
    if (d == '1')
      ++result;
  }
  return result;
}

string filter(vector<string> values, bool use_most_common) {
  unsigned pos = 0;
  while (values.size() > 1) {
    assert(pos < values[0].size());
    unsigned num_ones = 0;
    for (auto const &value : values) {
      if (value[pos] == '1')
	++num_ones;
    }
    unsigned num_zeros = values.size() - num_ones;
    char most_common = (num_ones >= num_zeros ? '1' : '0');
    char least_common = (num_zeros <= num_ones ? '0' : '1');
    char wanted = (use_most_common ? most_common : least_common);
    vector<string> next_values;
    for (auto const &value : values)
      if (value[pos] == wanted)
	next_values.push_back(value);
    values = next_values;
    ++pos;
  }
  assert(!values.empty());
  return values[0];
}


int main(int argc, char **argv) {
  vector<string> lines;
  string line;
  while (getline(cin, line)) {
    lines.push_back(line);
    assert(line.size() == lines[0].size());
  }
  auto oxygen = to_int(filter(lines, true));
  auto co2 = to_int(filter(lines, false));
  printf("%d %d %d\n", oxygen, co2, oxygen * co2);
  return 0;
}
