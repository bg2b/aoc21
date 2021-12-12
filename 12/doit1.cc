#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct cave_system {
  map<string, vector<string>> tunnels;

  cave_system(istream &in);

  bool is_small(string const &cave) const;
  int extend(vector<string> &path) const;
  int find_paths() const;
};

cave_system::cave_system(istream &in) {
  string cave1, cave2;
  while (in >> cave1 >> cave2) {
    assert(cave1 != cave2);
    tunnels[cave1].push_back(cave2);
    tunnels[cave2].push_back(cave1);
  }
}

bool cave_system::is_small(string const &cave) const {
  assert(!cave.empty());
  return islower(cave[0]);
}

int cave_system::extend(vector<string> &path) const {
  assert(!path.empty());
  auto const &current = path.back();
  if (current == "end")
    return 1;
  auto successors = tunnels.find(current);
  if (successors == tunnels.end())
    return 0;
  int num_paths = 0;
  for (auto const &cave : successors->second) {
    if (is_small(cave) && find(path.begin(), path.end(), cave) != path.end())
      // No repeated visits to small caves
      continue;
    path.push_back(cave);
    num_paths += extend(path);
    path.pop_back();
  }
  return num_paths;
}

int cave_system::find_paths() const {
  vector<string> path(1, "start");
  return extend(path);
}

int main(int argc, char **argv) {
  cave_system caves(cin);
  printf("%d\n", caves.find_paths());
  return 0;
}
