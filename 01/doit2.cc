#include <iostream>
#include <vector>
#include <stdio.h>

using namespace std;

int main(int argc, char **argv) {
  vector<int> depths;
  int depth;
  while (cin >> depth)
    depths.push_back(depth);
  int increases = 0;
  for (size_t i = 0; i + 3 < depths.size(); ++i)
    if (depths[i + 3] > depths[i])
      ++increases;
  printf("%d increases\n", increases);
  return 0;
}
