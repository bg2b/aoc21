#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct heightmap {
  vector<string> height;

  heightmap(istream &in);

  int H() const { return height.size(); }
  int W() const { return height[0].size(); }

  char at(int i, int j) const;
  bool is_low_point(int i, int j) const;

  int risk(int i, int j) const;

  int total_risk() const;
};

heightmap::heightmap(istream &in) {
  string line;
  while (getline(in, line)) {
    height.push_back(line);
    assert(height.back().size() == height.front().size());
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

int heightmap::risk(int i, int j) const {
  return is_low_point(i, j) ? at(i, j) - '0' + 1 : 0;
}

int heightmap::total_risk() const {
  int result = 0;
  for (int i = 0; i < H(); ++i)
    for (int j = 0; j < W(); ++j)
      if (is_low_point(i, j))
	result += risk(i, j);
  return result;
}

int main(int argc, char **argv) {
  heightmap map(cin);
  printf("%d\n", map.total_risk());
  return 0;
}
