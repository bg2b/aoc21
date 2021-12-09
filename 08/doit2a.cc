#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include <algorithm>
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

using namespace std;

typedef uint8_t byte;

struct entry {
  array<string, 10> patterns;
  array<string, 4> output;

  entry(string const &line);

  byte numeric(string const &pattern) const;
  vector<byte> of_length(size_t n) const;
  inline byte unique_of_length(size_t n) const;
  int to_digit(string const &pattern, array<byte, 10> const &digits) const;

  int decode() const;
};

entry::entry(string const &line) {
  stringstream in(line);
  for (size_t i = 0; i < patterns.size(); ++i)
    in >> patterns[i];
  for (size_t i = 0; i < output.size(); ++i)
    in >> output[i];
}

byte entry::numeric(string const &pattern) const {
  byte result = 0;
  for (auto c : pattern)
    result |= 1 << (c - 'a');
  return result;
}

vector<byte> entry::of_length(size_t n) const {
  vector<byte> result;
  for (auto const &pattern : patterns)
    if (pattern.size() == n)
      result.push_back(numeric(pattern));
  return result;
}

byte entry::unique_of_length(size_t n) const {
  auto v = of_length(n);
  assert(v.size() == 1);
  return (v.front());
}

int entry::to_digit(string const &pattern, array<byte, 10> const &digits) const {
  byte b = numeric(pattern);
  auto p = find(digits.begin(), digits.end(), b);
  assert(p != digits.end());
  return p - digits.begin();
}

inline int popcount(byte b) {
  return __builtin_popcount(b);
}

int entry::decode() const {
  byte _1 = unique_of_length(2);
  byte _4 = unique_of_length(4);
  byte _7 = unique_of_length(3);
  byte _8 = unique_of_length(7);
  // Find 0, 6, 9
  auto v069 = of_length(6);
  assert(v069.size() == 3);
  byte _0{0}, _6{0}, _9{0};
  for (int i = 0; i < 3; ++i)
    if (popcount(v069[i] & ~(_4 | _7)) == 1)
      _9 = v069[i];
    else if (popcount(v069[i] & _1) == 1)
      _6 = v069[i];
    else
      _0 = v069[i];
  // Find 2, 3, 5
  auto v235 = of_length(5);
  assert(v235.size() == 3);
  byte _2{0}, _3{0}, _5{0};
  for (int i = 0; i < 3; ++i)
    if (popcount(v235[i] & _1) == 2)
      _3 = v235[i];
    else if (popcount(v235[i] & _4) == 3)
      _5 = v235[i];
    else
      _2 = v235[i];
  // Convert output
  array<byte, 10> digits{ _0, _1, _2, _3, _4, _5, _6, _7, _8, _9 };
  return (to_digit(output[0], digits) * 1000 +
	  to_digit(output[1], digits) * 100 +
	  to_digit(output[2], digits) * 10 +
	  to_digit(output[3], digits) * 1);
}

int main(int argc, char **argv) {
  string line;
  int total = 0;
  while (getline(cin, line))
    total += entry(line).decode();
  printf("%d\n", total);
  return 0;
}
