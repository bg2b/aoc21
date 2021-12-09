#include <iostream>
#include <array>
#include <stdio.h>
#include <assert.h>

using namespace std;

typedef long nfish;

struct fish {
  array<nfish, 9> next_spawn{0};

  fish(istream &in);

  nfish num_fish() const;

  void day();
};

fish::fish(istream &in) {
  unsigned until_spawn;
  while (cin >> until_spawn) {
    ++next_spawn[until_spawn];
  }
}

nfish fish::num_fish() const {
  nfish result = 0;
  for (nfish n : next_spawn)
    result += n;
  return result;
}

void fish::day() {
  // nns = next next_spawn
  auto nns = decltype(next_spawn){0};
  // Count down
  for (unsigned i = 1; i < next_spawn.size(); ++i)
    nns[i - 1] += next_spawn[i];
  // 0's reset to 6 and make an 8
  nns[6] += next_spawn[0];
  nns[8] += next_spawn[0];
  next_spawn = nns;
}

int main(int argc, char **argv) {
  fish f(cin);
  for (int i = 0; i <= 256; ++i) {
    printf("day %d, number of fish %ld\n", i, f.num_fish());
    f.day();
  }
  return 0;
}
