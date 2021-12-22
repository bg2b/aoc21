#include <iostream>
#include <map>
#include <stdio.h>
#include <assert.h>

using namespace std;

int const dim = 3;

struct cube {
  int l[dim];
  int h[dim];

  cube(istream &in);
  cube(int max_coord);

  // Do two cubes overlap?
  bool overlaps(cube const &c) const;

  cube intersection(cube const &c) const;

  size_t volume() const;
};

cube::cube(istream &in) {
  for (int i = 0; i < dim; ++i) {
    in >> l[i];
    in >> h[i];
    assert(l[i] <= h[i]);
  }
  char eol = in.get();
  assert(eol == '\n');
}

cube::cube(int max_coord) {
  for (int i = 0; i < dim; ++i) {
    l[i] = -max_coord;
    h[i] = max_coord;
  }
}

bool cube::overlaps(cube const &c) const {
  for (int i = 0; i < dim; ++i)
    if (h[i] < c.l[i] || l[i] > c.h[i])
      return false;
  return true;
}

cube cube::intersection(cube const &c) const {
  assert(overlaps(c));
  cube result(*this);
  for (int i = 0; i < dim; ++i) {
    result.l[i] = max(result.l[i], c.l[i]);
    result.h[i] = min(result.h[i], c.h[i]);
  }
  return result;
}

size_t cube::volume() const {
  size_t result = 1;
  for (int i = 0; i < dim; ++i)
    result *= h[i] - l[i] + 1;
  return result;
}

// Some total order
bool operator<(cube const &c1, cube const &c2) {
  for (int i = 0; i < dim; ++i) {
    if (c1.l[i] < c2.l[i])
      return true;
    if (c2.l[i] < c1.l[i])
      return false;
    if (c1.h[i] < c2.h[i])
      return true;
    if (c2.h[i] < c1.h[i])
      return false;
  }
  // Equal
  return false;
}

int main(int argc, char **argv) {
  // Map from cube to number of times points in the cube appear (can
  // be negative for subtracting overlaps)
  map<cube, int> cubes;
  string cmd;
  while (cin >> cmd) {
    assert(cmd == "on" || cmd == "off");
    cube c(cin);
    map<cube, int> new_cubes(cubes);
    for (auto const &existing : cubes)
      if (existing.first.overlaps(c)) {
	// Subtle... why does this work?  Consider some point that's
	// within the cube c.  Assume by induction that the sum of
	// existing cube * count for cubes containing the point is
	// either 1 or 0, depending on whether the point is on or not.
	//
	// 1. Suppose the point is on already.  The sum of existing
	//    counts is +1 by the induction hypothesis.  The negation
	//    here will then produce a -1.  That'll cancel the
	//    existing 1 to give 0.
	//
	// 2. Suppose the point is currently off.  The existing sum is
	//    0, so the negation won't produce a net change, and it'll
	//    stay 0.
	//
	// In both cases, the count of any point in c will be 0 after
	// this loop.  If the command is "off", that's the right
	// answer, and if the command is "on" then ++new_cubes[c] will
	// give 1.  QED
	cube overlap = existing.first.intersection(c);
	new_cubes[overlap] -= existing.second;
      }
    if (cmd == "on")
      ++new_cubes[c];
    // cubes = new_cubes; is OK, but may as well try to save a little
    // time and space
    cubes.clear();
    for (auto const &c : new_cubes)
      if (c.second != 0)
	cubes.insert(c);
  }
  ssize_t total_set = 0;
  for (auto const &c : cubes)
    total_set += c.first.volume() * c.second;
  printf("%zu cubes, total set %zd\n", cubes.size(), total_set);
  return 0;
}
