#include <iostream>
#include <sstream>
#include <array>
#include <map>
#include <algorithm>
#include <string>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct entry {
  array<string, 10> patterns;
  array<string, 4> output;

  static map<string, int> digits;

  entry(string const &line);

  string decode1(string const &pattern, string const &wiring) const;
  bool valid(string const &wiring) const;
  int decoded_digit(string const &pattern, string const &wiring) const;

  int decode() const;
};

map<string, int> entry::digits = {
  { "abcefg", 0 }, { "cf", 1 }, { "acdeg", 2 },
  { "acdfg", 3 }, { "bcdf", 4 }, { "abdfg", 5 },
  { "abdefg", 6 }, { "acf", 7 }, { "abcdefg", 8 },
  { "abcdfg", 9 }
};

entry::entry(string const &line) {
  stringstream in(line);
  for (size_t i = 0; i < patterns.size(); ++i)
    in >> patterns[i];
  for (size_t i = 0; i < output.size(); ++i)
    in >> output[i];
}

string entry::decode1(string const &pattern, string const &wiring) const {
  string result(pattern);
  for (auto &c : result)
    c = wiring[c - 'a'];
  sort(result.begin(), result.end());
  return result;
}

bool entry::valid(string const &wiring) const {
  for (auto pattern : patterns)
    if (digits.find(decode1(pattern, wiring)) == digits.end())
      return false;
  return true;
}

int entry::decoded_digit(string const &pattern, string const &wiring) const {
  return digits.find(decode1(pattern, wiring))->second;
}

int entry::decode() const {
  string wiring("abcdefg");
  while (!valid(wiring)) {
    bool has_next = next_permutation(wiring.begin(), wiring.end());
    assert(has_next);
  }
  return (decoded_digit(output[0], wiring) * 1000 +
	  decoded_digit(output[1], wiring) * 100 +
	  decoded_digit(output[2], wiring) * 10 +
	  decoded_digit(output[3], wiring) * 1);
}

int main(int argc, char **argv) {
  string line;
  int total = 0;
  while (getline(cin, line))
    total += entry(line).decode();
  printf("%d\n", total);
  return 0;
}
