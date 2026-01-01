// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cassert>

using namespace std;

struct cave_system {
  // Successor caves
  map<string, vector<string>> tunnels;

  // Construct from cin
  cave_system();

  // Is a cave small?
  bool is_small(string const &cave) const;

  // Find the number of ways to extend path and reach the end.  If
  // small_once is false, two visits to one small cave are allowed.
  int extend(vector<string> &path, bool small_once) const;
  // Return the number of paths from start to end
  int find_paths(bool small_once) const {
    vector<string> path{"start"};
    return extend(path, small_once);
  }
};

cave_system::cave_system() {
  string connection;
  while (cin >> connection) {
    auto pos = connection.find('-');
    assert(pos != string::npos);
    auto cave1 = connection.substr(0, pos);
    auto cave2 = connection.substr(pos + 1);
    assert(cave1 != cave2);
    tunnels[cave1].push_back(cave2);
    tunnels[cave2].push_back(cave1);
  }
}

bool cave_system::is_small(string const &cave) const {
  assert(!cave.empty());
  return islower(cave[0]);
}

int cave_system::extend(vector<string> &path, bool small_once) const {
  assert(!path.empty());
  auto const &current = path.back();
  if (current == "end")
    return 1;
  int num_paths = 0;
  for (auto const &cave : tunnels.at(current)) {
    if (cave == "start")
      // start can't be revisited
      continue;
    bool next_small_once = small_once;
    if (is_small(cave)) {
      if (find(path.begin(), path.end(), cave) != path.end()) {
        // Visited before
        if (small_once)
          // Only single visits allowed
          continue;
        // Use the single 2x visit for this cave
        next_small_once = true;
      }
    }
    path.push_back(cave);
    num_paths += extend(path, next_small_once);
    path.pop_back();
  }
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
