#include <iostream>
#include <map>
#include <utility>
#include <stdio.h>
#include <assert.h>

using namespace std;

int sgn(int d) {
  return d > 0 ? +1 : (d < 0 ? -1 : 0);
}

struct vents {
  map<pair<int, int>, int> num_vents;
  int num_overlaps{0};

  void add(int x, int y);
  void add(int x1, int y1, int x2, int y2);
};

void vents::add(int x, int y) {
  if (num_vents.find({x, y}) == num_vents.end())
    num_vents[{x, y}] = 0;
  if (++num_vents[{x, y}] == 2)
    ++num_overlaps;
}

void vents::add(int x1, int y1, int x2, int y2) {
  int dx = x2 - x1;
  int dy = y2 - y1;
  assert(dx == 0 || dy == 0 || abs(dx) == abs(dy));
  dx = sgn(dx);
  dy = sgn(dy);
  while (x1 != x2 || y1 != y2) {
    add(x1, y1);
    x1 += dx;
    y1 += dy;
  }
  add(x2, y2);
}

int main(int argc, char **argv) {
  int x1, y1, x2, y2;
  vents ocean_floor;
  while (cin >> x1 >> y1 >> x2 >> y2)
    ocean_floor.add(x1, y1, x2, y2);
  printf("%d overlaps\n", ocean_floor.num_overlaps);
  return 0;
}
