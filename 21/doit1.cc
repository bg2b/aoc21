#include <iostream>
#include <algorithm>
#include <stdio.h>

using namespace std;

int roll(int &die, int &num_rolls) {
  ++num_rolls;
  int result = die++;
  if (die > 100)
    die = 1;
  return result;
}

int play(int p1, int p2) {
  int space[2] = { p1, p2 };
  int points[2] = { 0, 0 };
  int turn = 0;
  int die = 1;
  int num_rolls = 0;
  while (points[0] < 1000 && points[1] < 1000) {
    space[turn] += roll(die, num_rolls);
    space[turn] += roll(die, num_rolls);
    space[turn] += roll(die, num_rolls);
    space[turn] = (space[turn] - 1) % 10 + 1;
    points[turn] += space[turn];
    turn = 1 - turn;
  }
  return min(points[0], points[1]) * num_rolls;
}

int main(int argc, char **argv) {
  int p1, p2;
  cin >> p1 >> p2;
  printf("%d\n", play(p1, p2));
  return 0;
}
