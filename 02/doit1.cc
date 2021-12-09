#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

int main(int argc, char **argv) {
  int horiz = 0;
  int depth = 0;
  string dir;
  int amnt;
  while (cin >> dir && cin >> amnt) {
    if (dir == "forward")
      horiz += amnt;
    else if (dir == "down")
      depth += amnt;
    else {
      assert(dir == "up");
      depth -= amnt;
    }
  }
  printf("%d\n", horiz * depth);
  return 0;
}
