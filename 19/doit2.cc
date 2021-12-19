#include <iostream>
#include <sstream>
#include <array>
#include <list>
#include <set>
#include <stdio.h>
#include <assert.h>

using namespace std;

// Orientation indexes are from 0 to 23:
// 1. index / 8 denotes the axis to point along (reducing to 8
//    remaining orientations index')
// 2. index' >= 4 indicates to negate along that axis (4 remaining
//    index'')
// 3. index'' indicates number of times to rotate along the axis

struct displacement {
  array<int, 3> xyz;

  displacement(array<int, 3> const &xyz_) : xyz(xyz_) {}
  // Copy with a new orientation
  displacement(displacement const &d, int orient);
};

displacement::displacement(displacement const &d, int orient) : xyz(d.xyz) {
  // This routine is fairly subtle.  Do not muck with it without
  // testing carefully
  int axis = orient / 8;
  orient %= 8;
  assert(0 <= axis && axis < 3);
  while (axis > 0) {
    // Shift
    swap(xyz[0], xyz[1]);
    swap(xyz[1], xyz[2]);
    --axis;
  }
  if (orient >= 4) {
    // Flip
    xyz[0] = -xyz[0];
    swap(xyz[1], xyz[2]);
    orient -= 4;
  }
  assert(orient < 4);
  // Rotate
  while (orient > 0) {
    int y1 = xyz[1], z1 = xyz[2];
    xyz[1] = -z1;
    xyz[2] = y1;
    --orient;
  }
}

displacement operator+(displacement const &d1, displacement const &d2) {
  auto new_xyz = d1.xyz;
  new_xyz[0] += d2.xyz[0];
  new_xyz[1] += d2.xyz[1];
  new_xyz[2] += d2.xyz[2];
  return displacement(new_xyz);
}

displacement operator-(displacement const &d1, displacement const &d2) {
  auto new_xyz = d1.xyz;
  new_xyz[0] -= d2.xyz[0];
  new_xyz[1] -= d2.xyz[1];
  new_xyz[2] -= d2.xyz[2];
  return displacement(new_xyz);
}

// Lexicographic ordering
bool operator<(displacement const &d1, displacement const &d2) {
  for (int i : { 0, 1, 2 }) {
    if (d1.xyz[i] < d2.xyz[i])
      return true;
    if (d2.xyz[i] < d1.xyz[i])
      return false;
  }
  // Equal
  return false;
}

int manhattan(displacement const &d1, displacement const &d2) {
  auto d = d2 - d1;
  return abs(d.xyz[0]) + abs(d.xyz[1]) + abs(d.xyz[2]);
}

struct region {
  set<displacement> sensed;
  set<displacement> scanners;

  region(istream &in);
  // Copy with a new orientation
  region(region const &r, int orient);
  // Copy while shifting
  region(region const &r, displacement const &shift);

  // Pairwise offsets of items in sensed.  Used for a quick test of
  // whether there can be sufficient overlap between regions
  multiset<displacement> signature() const;

  // Try to merge r into the region.  Flips r through all possible
  // orientations, and for each orientation tries to shift it to one
  // of the positions in sensed.  If at least twelve displacements
  // match in the result, then it's a successful merge.  Returns true
  // on success, false if there's not enough overlap
  bool merge(region const &r);
};

region::region(istream &in) {
  string line;
  while (getline(in, line) && line != "") {
    array<int, 3> xyz;
    stringstream(line) >> xyz[0] >> xyz[1] >> xyz[2];
    sensed.emplace(displacement(xyz));
  }
  scanners.emplace(displacement({ 0, 0, 0 }));
}

region::region(region const &r, int orient) {
  for (auto const &d : r.sensed)
    sensed.emplace(displacement(d, orient));
  for (auto const &d : r.scanners)
    scanners.emplace(displacement(d, orient));
}

region::region(region const &r, displacement const &shift) {
  for (auto const &d : r.sensed)
    sensed.emplace(d + shift);
  for (auto const &d : r.scanners)
    scanners.emplace(d + shift);
}

multiset<displacement> region::signature() const {
  multiset<displacement> result;
  for (auto const &d1 : sensed)
    for (auto const &d2 : sensed)
      // Only need one of (d1, d2) and (d2, d1), so use lexicographic
      // order
      if (d1 < d2)
	result.emplace(d2 - d1);
  return result;
}

bool insufficient_overlap(multiset<displacement> const &sig1,
			  multiset<displacement> const &sig2)
{
  int num_overlaps = 0;
  for (auto const &s2 : sig2)
    if (sig1.find(s2) != sig1.end())
      ++num_overlaps;
  // A set of 12 matching probes is the minimum.  The signature will
  // contain displacements between each of those 12 with one of the 11
  // others, but pairs are canonically ordered so / 2.
  return num_overlaps < 12 * 11 / 2;
}

bool region::merge(region const &r) {
  auto sig = signature();
  for (int orient = 0; orient < 24; ++orient) {
    region r1(r, orient);
    auto sig1 = r1.signature();
    if (insufficient_overlap(sig, sig1))
      // No possible match
      continue;
    for (auto const &d : sensed)
      for (auto const &d1 : r1.sensed) {
	// Try shifting d1 on top of d and see if we get a match
	displacement shift = d - d1;
	region r2(r1, shift);
	int num_overlaps = 0;
	for (auto const &d2 : r2.sensed)
	  if (sensed.find(d2) != sensed.end())
	    ++num_overlaps;
	if (num_overlaps >= 12) {
	  sensed.insert(r2.sensed.begin(), r2.sensed.end());
	  scanners.insert(r2.scanners.begin(), r2.scanners.end());
	  return true;
	}
      }
  }
  return false;
}

int main(int argc, char **argv) {
  list<region> regions;
  string line;
  while (getline(cin, line)) {
    int scanner_num = stoi(line);
    assert(scanner_num == int(regions.size()));
    regions.push_back(region(cin));
  }
  while (regions.size() > 1) {
    auto i = regions.begin();
    ++i;
    while (i != regions.end()) {
      if (regions.begin()->merge(*i))
	regions.erase(i++);
      else
	++i;
    }
  }
  assert(!regions.empty());
  region const &r = *regions.begin();
  printf("%zu probes\n", r.sensed.size());
  int max_manhattan = 0;
  for (auto const &d1 : r.scanners)
    for (auto const &d2 : r.scanners)
      max_manhattan = max(max_manhattan, manhattan(d1, d2));
  printf("Maximum Manhattan distance %d\n", max_manhattan);
  return 0;
}
