// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cassert>

using namespace std;

int num_bits;
vector<int> report;

void read_report() {
  string line;
  while (getline(cin, line)) {
    assert(report.empty() || num_bits == int(line.length()));
    num_bits = line.length();
    report.push_back(stoi(line, nullptr, 2));
  }
}

// Split values into two parts according to the bit at the given
// position
array<vector<int>, 2> split(vector<int> const &values, int pos) {
  array<vector<int>, 2> result;
  for (auto v : values)
    result[(v & (1 << pos)) == 0 ? 0 : 1].push_back(v);
  return result;
}

void part1() {
  int gamma = 0;
  int epsilon = 0;
  for (int pos = 0; pos < num_bits; ++pos) {
    auto [v0, v1] = split(report, pos);
    int digit = 1 << pos;
    int most_common = v0.size() > v1.size() ? 0 : 1;
    gamma += digit * most_common;
    epsilon += digit * !most_common;
  }
  cout << gamma * epsilon << '\n';
}

void part2() {
  auto filter = [](vector<int> values, bool most_common) {
    int next_pos = num_bits;
    while (values.size() > 1) {
      auto [v0, v1] = split(values, --next_pos);
      if (most_common)
        values = v1.size() >= v0.size() ? v1 : v0;
      else
        values = v0.size() <= v1.size() ? v0 : v1;
    }
    assert(values.size() == 1);
    return values.front();
  };
  auto oxygen = filter(report, true);
  auto co2 = filter(report, false);
  cout << oxygen * co2 << '\n';
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " partnum < input\n";
    exit(1);
  }
  read_report();
  if (*argv[1] == '1')
    part1();
  else
    part2();
  return 0;
}
