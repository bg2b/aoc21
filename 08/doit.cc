// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <array>
#include <cassert>

using namespace std;

// Part 2 here is just a brute-force search through all wiring
// permutations

// How the digits are displayed (assuming no mixed up segments):
//
//   0:      1:      2:      3:      4:
//  aaaa    ....    aaaa    aaaa    ....
// b    c  .    c  .    c  .    c  b    c
// b    c  .    c  .    c  .    c  b    c
//  ....    ....    dddd    dddd    dddd
// e    f  .    f  e    .  .    f  .    f
// e    f  .    f  e    .  .    f  .    f
//  gggg    ....    gggg    gggg    ....
//
//   5:      6:      7:      8:      9:
//  aaaa    aaaa    aaaa    aaaa    aaaa
// b    .  b    .  .    c  b    c  b    c
// b    .  b    .  .    c  b    c  b    c
//  dddd    dddd    ....    dddd    dddd
// .    f  e    f  .    f  e    f  .    f
// .    f  e    f  .    f  e    f  .    f
//  gggg    gggg    ....    gggg    gggg

// Lit segments for all the digits above
map<string, int> digits = {
    {"abcefg", 0}, {"cf", 1},     {"acdeg", 2}, {"acdfg", 3},   {"bcdf", 4},
    {"abdfg", 5},  {"abdefg", 6}, {"acf", 7},   {"abcdefg", 8}, {"abcdfg", 9}};

struct display {
  // The observed patterns
  array<string, 10> patterns;
  // What's being displayed
  array<string, 4> output;

  display(string const &line);

  // Part 1
  int count1478() const;

  // Part 2: find the wiring permutation and decode the displayed
  // number
  int decode() const;
};

display::display(string const &line) {
  stringstream ss(line);
  for (size_t i = 0; i < patterns.size(); ++i)
    ss >> patterns[i];
  char bar;
  ss >> bar;
  assert(bar == '|');
  for (size_t i = 0; i < output.size(); ++i)
    ss >> output[i];
}

int display::count1478() const {
  int result = 0;
  for (auto const &digit : output)
    switch (digit.length()) {
    case 2:  // 1
    case 4:  // 4
    case 3:  // 7
    case 7:  // 8
      ++result;
      break;
    default:;
    }
  return result;
}

int display::decode() const {
  string wiring("abcdefg");
  // Apply the wiring permutation to a pattern of segments,
  // returning the (sorted) segments that result
  auto decode1 = [&](string const &pattern) {
    string result(pattern);
    for (auto &c : result)
      c = wiring[c - 'a'];
    sort(result.begin(), result.end());
    return result;
  };
  // Are all observed patterns valid under the wiring permutation?
  auto valid = [&]() {
    for (auto const &pattern : patterns)
      if (!digits.contains(decode1(pattern)))
        return false;
    return true;
  };
  // Try all permutations of the wires
  while (!valid()) {
    bool has_next = next_permutation(wiring.begin(), wiring.end());
    assert(has_next);
  }
  auto decode_digit = [&](string const &pattern) {
    return digits.at(decode1(pattern));
  };
  return decode_digit(output[0]) * 1000 + decode_digit(output[1]) * 100 +
         decode_digit(output[2]) * 10 + decode_digit(output[3]) * 1;
}

void solve(int (display::*fn)() const) {
  string line;
  int total = 0;
  while (getline(cin, line))
    total += (display(line).*fn)();
  cout << total << '\n';
}

void part1() { solve(&display::count1478); }
void part2() { solve(&display::decode); }

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
