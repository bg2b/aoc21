#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <utility>
#include <stdio.h>
#include <assert.h>

using namespace std;

int const dim = 3;

struct reactor {
  // Points used in [start, end) intervals along each axis
  map<int, unsigned> breakpoints[dim];
  // For stepping along each axis in the corners array
  size_t stride[dim];
  // State of each cell
  vector<bool> corners;

  // Collect breakpoints for a cube
  void set_breakpoints(array<int, 2 * dim> const &cube);

  // Execute a command, setting or clearing things in corners
  void execute(bool on, array<int, 2 * dim> const &cube);

  // How many individual points are set?
  size_t count_on() const;
};

void reactor::set_breakpoints(array<int, 2 * dim> const &cube) {
  for (int i = 0; i < dim; ++i) {
    // It's convenient to use [start, end) rather than [start, end]
    breakpoints[i][cube[2 * i]] = 0;
    breakpoints[i][cube[2 * i + 1] + 1] = 0;
  }
}

void reactor::execute(bool on, array<int, 2 * dim> const &cube) {
  if (corners.empty()) {
    // First time after set_breakpoints; initialize
    size_t sz = 1;
    for (int i = 0; i < dim; ++i) {
      unsigned index = 0;
      for (auto &p : breakpoints[i])
	p.second = index++;
      sz *= breakpoints[i].size() - 1;
    }
    corners.resize(sz, false);
    for (int i = 0; i < dim; ++i) {
      stride[i] = corners.size();
      for (int j = 0; j <= i; ++j)
	stride[i] /= breakpoints[j].size() - 1;
    }
    assert(stride[dim - 1] == 1);
  }
  // Find start and end for each dimension
  size_t start[dim];
  size_t end[dim];
  for (int i = 0; i < dim; ++i) {
    start[i] = breakpoints[i][cube[2 * i]] * stride[i];
    end[i] = breakpoints[i][cube[2 * i + 1] + 1] * stride[i];
  }
  // Critical loop, specialized for dim 3
  assert(dim == 3);
  for (auto xi = start[0]; xi != end[0]; xi += stride[0])
    for (auto yi = start[1]; yi != end[1]; yi += stride[1])
      for (auto zi = start[2]; zi != end[2]; ++zi)
	corners[xi + yi + zi] = on;
}

size_t reactor::count_on() const {
  // Volumes along each dimension
  vector<unsigned> vols[dim];
  for (int i = 0; i < dim; ++i)
    for (auto q = breakpoints[i].begin(), p = q++;
	 q != breakpoints[i].end();
	 ++p, ++q)
      vols[i].push_back(q->first - p->first);
  assert(dim == 3);
  // Critical loop, specialized for dim 3
  size_t result = 0;
  size_t xi = 0;
  for (auto vx : vols[0]) {
    size_t yi = 0;
    for (auto vy : vols[1]) {
      size_t zi = 0;
      for (auto vz : vols[2]) {
	if (corners[xi + yi + zi])
	  result += size_t(vx) * vy * vz;
	++zi;
      }
      yi += stride[1];
    }
    xi += stride[0];
  }
  return result;
}

int main(int argc, char **argv) {
  reactor reactr;
  vector<pair<string, array<int, 6>>> commands;
  string cmd;
  while (cin >> cmd) {
    assert(cmd == "on" || cmd == "off");
    array<int, 2 * dim> xyz;
    cin >> xyz[0] >> xyz[1] >> xyz[2] >> xyz[3] >> xyz[4] >> xyz[5];
    commands.emplace_back(cmd, xyz);
    reactr.set_breakpoints(commands.back().second);
  }
  for (auto const &command : commands)
    reactr.execute(command.first == "on", command.second);
  printf("%zu bits, total set %zu\n",
	 reactr.corners.size(), reactr.count_on());
  return 0;
}
