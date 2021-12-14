#include <iostream>
#include <string>
#include <map>
#include <limits>
#include <stdio.h>
#include <assert.h>

using namespace std;

using element = char;

struct polymerization {
  map<pair<element, element>, element> rules;

  polymerization(istream &in);

  string grow(string const &tmplt) const;
};

polymerization::polymerization(istream &in) {
  string line;
  while (getline(in, line)) {
    assert(line.size() == 4 && line[2] == ' ');
    rules[make_pair(line[0], line[1])] = line[3];
  }
}

string polymerization::grow(string const &tmplt) const {
  auto n = tmplt.size();
  if (n < 2)
    return tmplt;
  string result;
  result.reserve(2 * n - 1);
  for (size_t i = 0; i + 1 < n; ++i) {
    result.push_back(tmplt[i]);
    pair<element, element> adjacent(tmplt[i], tmplt[i + 1]);
    auto matching = rules.find(adjacent);
    if (matching != rules.end())
      result.push_back(matching->second);
  }
  result.push_back(tmplt.back());
  return result;
}

size_t most_minus_least(string const &poly) {
  map<element, size_t> counts;
  for (auto c : poly) {
    if (counts.find(c) == counts.end())
      counts[c] = 0;
    ++counts[c];
  }
  size_t most_common = 0;
  size_t least_common = numeric_limits<size_t>::max();
  for (auto const &count : counts) {
    most_common = max(most_common, count.second);
    least_common = min(least_common, count.second);
  }
  return most_common - least_common;
}

int main(int argc, char **argv) {
  string current_polymer;
  getline(cin, current_polymer);
  polymerization poly_rules(cin);
  for (int step = 0; step < 10; ++step) {
    current_polymer = poly_rules.grow(current_polymer);
    printf("Length %zu after %d steps\n", current_polymer.size(), step + 1);
   }
  printf("Difference between most and least common elements %zu\n",
	 most_minus_least(current_polymer));
  return 0;
}
