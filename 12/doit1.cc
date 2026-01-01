// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit1 doit1.cc
// ./doit1 1 < input  # part 1
// ./doit1 2 < input  # part 2

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cctype>
#include <cassert>

using namespace std;

// This encodes cave names to ints during reading for speed

struct cave_system {
  // Start and end get fixed numbers
  static constexpr int start = 0;
  static constexpr int end = 1;
  // Successor caves
  vector<vector<int>> tunnels;
  // is_small[i] is true if the cave with index i is small
  vector<bool> is_small;

  // Construct from cin
  cave_system();

  // Find the number of ways to extend path from current and reach the
  // end.  The path is represented as a count of the number of times
  // each cave has been visited (with current not incremented yet).
  // If small_once is false, two visits to one small cave are allowed.
  int extend(int current, vector<int> &path, bool small_once) const;
  // Return the number of paths from start to end
  int find_paths(bool small_once) const {
    vector<int> path(tunnels.size(), 0);
    return extend(start, path, small_once);
  }
};

cave_system::cave_system() {
  map<string, int> indexes;
  // Find or create the index for a cave name
  auto index = [&](string const &cave) {
    if (auto p = indexes.find(cave); p != indexes.end())
      return p->second;
    int result = tunnels.size();
    indexes.emplace(cave, result);
    tunnels.resize(result + 1);
    is_small.resize(result + 1);
    is_small[result] = islower(cave.front());
    return result;
  };
  // I want fixed start and end indexes
  (void)index("start");
  (void)index("end");
  assert(index("start") == start && index("end") == end);
  assert(is_small[start] && is_small[end]);
  // Read the connections
  string connection;
  while (cin >> connection) {
    auto pos = connection.find('-');
    assert(pos != string::npos);
    auto cave1 = connection.substr(0, pos);
    auto cave2 = connection.substr(pos + 1);
    assert(cave1 != cave2);
    auto i1 = index(cave1);
    auto i2 = index(cave2);
    tunnels[i1].push_back(i2);
    tunnels[i2].push_back(i1);
  }
}

int cave_system::extend(int current, vector<int> &path, bool small_once) const {
  if (current == end)
    // Reached the end
    return 1;
  // Visit the current cave
  ++path[current];
  int num_paths = 0;
  for (auto next : tunnels[current]) {
    if (next == start)
      // start can't be revisited
      continue;
    bool next_small_once = small_once;
    if (is_small[next]) {
      if (path[next] > 0) {
        // Visited before
        if (small_once)
          // Only single visits allowed
          continue;
        // Use the single 2x visit for this cave
        next_small_once = true;
      }
    }
    num_paths += extend(next, path, next_small_once);
  }
  // Unvisit the current cave
  --path[current];
  return num_paths;
}

void part1() { cout << cave_system().find_paths(true) << '\n'; }
void part2() { cout << cave_system().find_paths(false) << '\n'; }

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
