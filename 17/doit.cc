// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <optional>
#include <cassert>

using namespace std;

// Simulating explicitly is a little silly, but it's easy to get it
// right and reasonably fast.  It should be possible to take just the
// vy though and compute a range of times where the probe y will be in
// the target zone, then check the x's directly at those times.

// The target area
int xmin, xmax, ymin, ymax;

// Final x coordinate if launched with velocity vx
int terminal_x(int vx) { return vx * (vx + 1) / 2; }

void step(int &x, int &y, int &vx, int &vy, int &highest_y) {
  x += vx;
  --vx;
  y += vy;
  --vy;
  highest_y = max(highest_y, y);
}

// Simulate a probe launch.  Return highest y value if the probe
// reaches the target area, or nullopt if it does not.
optional<int> highest_y(int vx, int vy) {
  assert(vx > 0);
  if (terminal_x(vx) < xmin)
    // Will not reach the desired minimum x
    return nullopt;
  int x = 0;
  int y = 0;
  int result = 0;
  while (x < xmin)
    step(x, y, vx, vy, result);
  while (x <= xmax && y >= ymin) {
    if (ymin <= y && y <= ymax)
      // In the target area
      return result;
    step(x, y, vx, vy, result);
  }
  return nullopt;
}

void read_input() {
  string target, area;
  cin >> target >> area;
  assert(target == "target" && area == "area:");
  char x, y, equals, dot, comma;
  cin >> x >> equals >> xmin >> dot >> dot >> xmax >> comma;
  assert(x == 'x' && equals == '=' && dot == '.' && comma == ',');
  cin >> y >> equals >> ymin >> dot >> dot >> ymax;
  assert(y == 'y' && equals == '=' && dot == '.');
  assert(xmin > 0 && xmin < xmax);
  assert(ymin < 0 && ymin < ymax);
}

// Return the highest y of any solution (part1), or the total number
// of solutions (!part1)
int solve(bool part1) {
  // vx needs to be at least enough to reach the target
  int vx_min = 1;
  while (terminal_x(vx_min) < xmin)
    ++vx_min;
  // vx > xmax will be beyond the target immediately
  int vx_max = xmax;
  // vy < ymin will be below the target immediately
  int vy_min = ymin;
  // Launching upwards is symmetric.  If the probe is sent up at
  // velocity vy > 0, then coming back down there will be a step with
  // y = 0 and traveling at velocity -vy.  That means -ymin is the
  // maximum possible vy, since otherwise the next step from y = 0
  // when going down would take the probe below the target.
  int vy_max = -ymin;
  // For greatest height, start searching at largest upwards velocity
  int num_solutions = 0;
  for (int vy = vy_max; vy >= vy_min; --vy)
    for (int vx = vx_min; vx <= vx_max; ++vx)
      if (auto height = highest_y(vx, vy); height.has_value()) {
        if (part1)
          return *height;
        ++num_solutions;
      }
  return num_solutions;
}

void part1() { cout << solve(true) << '\n'; }
void part2() { cout << solve(false) << '\n'; }

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " partnum < input\n";
    exit(1);
  }
  read_input();
  if (*argv[1] == '1')
    part1();
  else
    part2();
  return 0;
}
