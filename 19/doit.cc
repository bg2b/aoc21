// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <array>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <cassert>

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

// set/map operations are a big part of the time, so I'll take the
// trouble to use the unordered versions
template <> struct std::hash<displacement> {
  size_t operator()(displacement const &d) const {
    return d.xyz[0] + 1000 * (d.xyz[1] + 1000 * d.xyz[2]);
  }
};

bool operator==(displacement const &d1, displacement const &d2) {
  return d1.xyz == d2.xyz;
}

bool operator<(displacement const &d1, displacement const &d2) {
  return d1.xyz < d2.xyz;
}

int manhattan(displacement const &d1, displacement const &d2) {
  auto d = d2 - d1;
  return abs(d.xyz[0]) + abs(d.xyz[1]) + abs(d.xyz[2]);
}

// A region with some scanners and the beacons that they are sensing
struct region {
  // The sensed beacons
  unordered_set<displacement> sensed;
  // The scanners
  list<displacement> scanners;

  // Construct from cin
  region();
  // Copy with a new orientation
  region(region const &r, int orient);
  // Copy while shifting
  region(region const &r, displacement const &shift);

  // Pairwise offsets of items in sensed.  Used for a quick test of
  // whether there can be sufficient overlap between regions
  unordered_map<displacement, int> signature() const;

  // Try to merge r into the region.  Flips r through all possible
  // orientations, and for each orientation tries to shift it to one
  // of the positions in sensed.  If at least twelve displacements
  // match in the result, then it's a successful merge.  Returns true
  // on success, false if there's not enough overlap
  bool merge(region const &r);
};

region::region() {
  string line;
  while (getline(cin, line) && !line.empty()) {
    array<int, 3> xyz;
    char comma;
    stringstream(line) >> xyz[0] >> comma >> xyz[1] >> comma >> xyz[2];
    assert(comma == ',');
    sensed.emplace(displacement(xyz));
  }
  // There's a single scanner at the origin
  scanners.emplace_back(displacement({0, 0, 0}));
}

region::region(region const &r, int orient) {
  for (auto const &d : r.sensed)
    sensed.emplace(displacement(d, orient));
  for (auto const &d : r.scanners)
    scanners.emplace_back(displacement(d, orient));
}

region::region(region const &r, displacement const &shift) {
  for (auto const &d : r.sensed)
    sensed.emplace(d + shift);
  for (auto const &d : r.scanners)
    scanners.emplace_back(d + shift);
}

unordered_map<displacement, int> region::signature() const {
  unordered_map<displacement, int> result;
  for (auto const &d1 : sensed)
    for (auto const &d2 : sensed)
      // Only need one of (d1, d2) and (d2, d1), so use lexicographic
      // order
      if (d1 < d2)
        ++result[d2 - d1];
  return result;
}

bool insufficient_overlap(unordered_map<displacement, int> const &sig1,
                          unordered_map<displacement, int> const &sig2) {
  int num_overlaps = 0;
  for (auto const &[disp, cnt2] : sig2)
    if (auto p = sig1.find(disp); p != sig1.end())
      num_overlaps += min(p->second, cnt2);
  // A set of 12 matching beacons is the minimum.  The signature will
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
          if (sensed.contains(d2))
            ++num_overlaps;
        if (num_overlaps >= 12) {
          sensed.insert(r2.sensed.begin(), r2.sensed.end());
          scanners.insert(scanners.end(), r2.scanners.begin(),
                          r2.scanners.end());
          return true;
        }
      }
  }
  return false;
}

void solve(bool part1) {
  list<region> regions;
  string line;
  while (getline(cin, line)) {
    string dash, scanner;
    int scanner_num;
    stringstream(line) >> dash >> scanner >> scanner_num >> dash;
    assert(dash == "---");
    assert(scanner_num == int(regions.size()));
    regions.push_back(region());
  }
  while (regions.size() > 1)
    for (auto i = next(regions.begin()); i != regions.end();)
      if (regions.begin()->merge(*i))
        regions.erase(i++);
      else
        ++i;
  assert(!regions.empty());
  region const &r = *regions.begin();
  if (part1)
    cout << r.sensed.size() << '\n';
  else {
    int max_manhattan = 0;
    for (auto const &d1 : r.scanners)
      for (auto const &d2 : r.scanners)
        max_manhattan = max(max_manhattan, manhattan(d1, d2));
    cout << max_manhattan << '\n';
  }
}

void part1() { solve(true); }
void part2() { solve(false); }

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
