#include <iostream>
#include <string>
#include <map>
#include <set>
#include <stdio.h>
#include <assert.h>

using namespace std;

using element = char;

struct polymerization {
  map<pair<element, element>, element> rules;

  polymerization(istream &in);

  // Return element to insert or null if none
  element insertion(element element1, element element2) const;
};

polymerization::polymerization(istream &in) {
  string line;
  while (getline(in, line)) {
    assert(line.size() == 4 && line[2] == ' ');
    rules[make_pair(line[0], line[1])] = line[3];
  }
}

element polymerization::insertion(element element1, element element2) const {
  auto match = rules.find(make_pair(element1, element2));
  if (match == rules.end())
    return element(0);
  return match->second;
}

struct polymer {
  // First element
  element start;
  // (element1, element2) => number of occurrences of that pair
  map<pair<element, element>, size_t> adjacent_counts;

  // A single element polymer
  polymer(element start_) : start(start_)  {}
  // Starting from a template
  polymer(string const &tmplt);

  // Add num_times occurrences of (element1, element2)
  void add_pair(element element1, element element2, size_t num_times = 1);

  // Number of elements in total
  size_t size() const;
  // How often does a certain element occur?
  size_t occurrences_of(element element) const;

  size_t most_minus_least() const;

  // Get the result after one step of growth
  polymer grow(polymerization const &poly_rules);
};

polymer::polymer(string const &tmplt) {
  assert(!tmplt.empty());
  start = tmplt[0];
  for (size_t i = 0; i + 1 < tmplt.size(); ++i)
    add_pair(tmplt[i], tmplt[i + 1]);
}

void polymer::add_pair(element element1, element element2, size_t num_times) {
  pair<element, element> adjacent(element1, element2);
  if (adjacent_counts.find(adjacent) == adjacent_counts.end())
    adjacent_counts[adjacent] = 0;
  adjacent_counts[adjacent] += num_times;
}

size_t polymer::size() const {
  // Initial element ...
  size_t result = 1;
  // ... plus the second element in each pair
  for (auto const &count : adjacent_counts)
    result += count.second;
  return result;
}

size_t polymer::occurrences_of(element element) const {
  size_t result = 0;
  // One extra occurrence of the first element in the polymer
  if (element == start)
    ++result;
  // Plus how many times the element appears as the second element of
  // a pair
  for (auto const &count : adjacent_counts)
    if (element == count.first.second)
      result += count.second;
  return result;
}

size_t polymer::most_minus_least() const {
  set<pair<size_t, element>> occurrences;
  occurrences.emplace(occurrences_of(start), start);
  for (auto const &count : adjacent_counts) {
    element element2 = count.first.second;
    occurrences.emplace(occurrences_of(element2), element2);
  }
  auto const &least_common = *occurrences.begin();
  auto const &most_common = *occurrences.rbegin();
  return most_common.first - least_common.first;
}

polymer polymer::grow(polymerization const &poly_rules) {
  // Start from the same initial element
  polymer result(start);
  for (auto const &count : adjacent_counts) {
    element element1 = count.first.first;
    element element2 = count.first.second;
    size_t num_times = count.second;
    element insertion = poly_rules.insertion(element1, element2);
    if (insertion) {
      // An insertion splits (element1, element2) into two pairs
      result.add_pair(element1, insertion, num_times);
      result.add_pair(insertion, element2, num_times);
    } else
      // Nothing inserts between (element1, element2)
      result.add_pair(element1, element2, num_times);
  }
  return result;
}

int main(int argc, char **argv) {
  string tmplt;
  getline(cin, tmplt);
  polymer current_polymer(tmplt);
  polymerization poly_rules(cin);
  for (int step = 0; step < 40; ++step) {
    current_polymer = current_polymer.grow(poly_rules);
    printf("Length %zu after %d steps\n", current_polymer.size(), step + 1);
  }
  printf("Difference between most and least common elements %zu\n",
	 current_polymer.most_minus_least());
  return 0;
}
