#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <utility>
#include <stdio.h>
#include <assert.h>

using namespace std;

using basin = set<pair<int, int>>;

struct heightmap {
  vector<string> height;

  heightmap(istream &in);

  int H() const { return height.size(); }
  int W() const { return height[0].size(); }

  char at(int i, int j) const;
  bool is_low_point(int i, int j) const;

  basin basin_at(int i, int j) const;
};

heightmap::heightmap(istream &in) {
  string line;
  while (getline(in, line)) {
    height.push_back(line);
    assert(int(height.back().size()) == W());
  }
}

char heightmap::at(int i, int j) const {
  char const maxval = '9' + 1;
  if (i < 0 || i >= H())
    return maxval;
  if (j < 0 || j >= W())
    return maxval;
  return height[i][j];
}

bool heightmap::is_low_point(int i, int j) const {
  auto h = at(i, j);
  return (h < at(i - 1, j) && h < at(i + 1, j) &&
	  h < at(i, j - 1) && h < at(i, j + 1));
}

basin heightmap::basin_at(int i, int j) const {
  basin result;
  if (!is_low_point(i, j))
    return result;
  result.emplace(i, j);
  vector<pair<int, int>> frontier{{ i, j }};
  while (!frontier.empty()) {
    auto ij = frontier.back();
    frontier.pop_back();
    int i = ij.first;
    int j = ij.second;
    auto h_ij = at(i, j);
    // Frontier locations pull in any adjacent locations that are
    // greater but that are not 9 (or off the edge)
    for (auto dij : vector<pair<int, int>>{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}) {
      int di = dij.first;
      int dj = dij.second;
      if (result.find(make_pair(i + di, j + dj)) != result.end())
	// Already in the basin
	continue;
      auto h_dij = at(i + di, j + dj);
      if (h_dij > h_ij && h_dij < '9') {
	// New
	result.emplace(i + di, j + dj);
	frontier.emplace_back(i + di, j + dj);
      }
    }
  }
  return result;
}

int main(int argc, char **argv) {
  heightmap map(cin);
  basin all_basins;
  vector<int> basin_sizes;
  for (int i = 0; i < map.H(); ++i)
    for (int j = 0; j < map.W(); ++j)
      if (all_basins.find(make_pair(i, j)) == all_basins.end()) {
	basin b = map.basin_at(i, j);
	if (b.empty())
	  continue;
	all_basins.insert(b.begin(), b.end());
	basin_sizes.push_back(b.size());
      }
  sort(basin_sizes.begin(), basin_sizes.end());
  int nbasins = basin_sizes.size();
  int result = 1;
  for (int i = 0; i < min(3, nbasins); ++i)
    result *= basin_sizes[nbasins - 1 - i];
  printf("%d\n", result);
  return 0;
}
