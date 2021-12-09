#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

int main(int argc, char **argv) {
  int horiz = 0;
  int depth = 0;
  int aim = 0;
  string dir;
  int amnt;
  while (cin >> dir && cin >> amnt) {
    if (dir == "down")
      aim += amnt;
    else if (dir == "up")
      aim -= amnt;
    else {
      assert(dir == "forward");
      horiz += amnt;
      depth += amnt * aim;
    }
  }
  printf("%ld\n", long(horiz) * depth);
  return 0;
}
