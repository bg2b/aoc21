#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <assert.h>

using namespace std;

int xmin, xmax, ymin, ymax;
int no_solution;

int terminal_x(int vx) {
  return vx * (vx + 1) / 2;
}

void step(int &x, int &y, int &vx, int &vy, int &highest_y) {
  x += vx;
  --vx;
  y += vy;
  --vy;
  highest_y = max(highest_y, y);
}

int highest_y(int vx, int vy) {
  assert(vx > 0);
  if (terminal_x(vx) < xmin)
    return no_solution;
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
  return no_solution;
}

int main(int argc, char **argv) {
  cin >> xmin >> xmax >> ymin >> ymax;
  // Below worst possible highest y
  no_solution = -ymax - 1;
  assert(xmin > 0 && xmin < xmax);
  assert(ymin < 0 && ymin < ymax);
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
  for (int vy = vy_max; vy >= vy_min; --vy)
    for (int vx = vx_min; vx <= vx_max; ++vx) {
      int height = highest_y(vx, vy);
      if (height != no_solution) {
	printf("Highest y is %d\n", height);
	return 0;
      }
    }
  assert(false && "Some solution should have been found");
  return 0;
}
