// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;

using element = char;
using adjacent = pair<element, element>;

struct polymerization {
  // Rules for how pairs of adjacent elements expand
  map<adjacent, element> rules;

  // Construct from cin
  polymerization();

  // Return element to insert or nullopt if none
  optional<element> insertion(adjacent const &adj) const;
};

polymerization::polymerization() {
  element a, b, c;
  string to;
  while (cin >> a >> b >> to >> c) {
    assert(to == "->");
    rules.emplace(adjacent{a, b}, c);
  }
}

optional<element> polymerization::insertion(adjacent const &adj) const {
  if (auto match = rules.find(adj); match != rules.end())
    return match->second;
  return nullopt;
}

struct polymer {
  // First element
  element start;
  // Number of occurrences of adjacent elements
  map<adjacent, long> adjacent_counts;

  // Start from a single element
  polymer(element e) : start(e) {}
  // Construct from a template
  polymer(string const &tmplt);

  // Add num_times occurrences of the adjacent pair
  void add_pair(adjacent const &adj, long num_times) {
    adjacent_counts[adj] += num_times;
  }

  // Get the result after one step of growth
  polymer grow(polymerization const &poly_rules) const;

  // How often does a certain element occur?
  long occurrences_of(element e) const;

  // Difference of abundances
  long most_minus_least() const;
};

polymer::polymer(string const &tmplt) {
  assert(!tmplt.empty());
  start = tmplt[0];
  for (size_t i = 0; i + 1 < tmplt.size(); ++i)
    add_pair({tmplt[i], tmplt[i + 1]}, 1);
}

polymer polymer::grow(polymerization const &poly_rules) const {
  // Start from the same initial element
  polymer result(start);
  for (auto const &[adj, num_times] : adjacent_counts)
    if (auto ins = poly_rules.insertion(adj); ins) {
      // An insertion splits (element1, element2) into two pairs
      result.add_pair({adj.first, *ins}, num_times);
      result.add_pair({*ins, adj.second}, num_times);
    } else
      // Nothing inserts between the pair
      result.add_pair(adj, num_times);
  return result;
}

long polymer::occurrences_of(element e) const {
  long result = 0;
  // One extra occurrence of the first element in the polymer
  if (e == start)
    ++result;
  // Plus how many times the element appears as the second element of a pair
  for (auto const &[adj, num_times] : adjacent_counts)
    if (e == adj.second)
      result += num_times;
  return result;
}

long polymer::most_minus_least() const {
  set<element> elements{start};
  for (auto const &[adj, _] : adjacent_counts)
    elements.insert(adj.second);
  vector<long> occurrences;
  for (auto e : elements)
    occurrences.push_back(occurrences_of(e));
  auto [mn, mx] = minmax_element(occurrences.begin(), occurrences.end());
  return *mx - *mn;
}

void solve(int steps) {
  string tmplt;
  cin >> tmplt;
  polymerization poly_rules;
  polymer current_polymer(tmplt);
  for (int _ = 0; _ < steps; ++_)
    current_polymer = current_polymer.grow(poly_rules);
  cout << current_polymer.most_minus_least() << '\n';
}

void part1() { solve(10); }
void part2() { solve(40); }

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
