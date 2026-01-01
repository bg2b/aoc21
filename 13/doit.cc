// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <cassert>

using namespace std;

using marks = set<pair<int, int>>;

// The current set of dots on the paper
marks paper;

void read() {
  string line;
  while (getline(cin, line)) {
    int x, y;
    char comma;
    if (!(stringstream(line) >> x >> comma >> y))
      break;
    assert(comma == ',');
    paper.emplace(x, y);
  }
}

// Fold the bottom part of paper up to the top
void fold_up(int pos) {
  marks new_paper;
  for (auto const &dot : paper) {
    // Nothing is allowed on the fold line
    assert(dot.second != pos);
    if (dot.second < pos)
      new_paper.insert(dot);
    else {
      // New position must not be below position 0
      assert(pos >= dot.second - pos);
      int new_y = pos - (dot.second - pos);
      new_paper.emplace(dot.first, new_y);
    }
  }
  paper = new_paper;
}

// I want to avoid separate horizontal and vertical folds.  For
// folding the right part of the paper to left, just flip x and y,
// fold bottom to top, and then flip back.
void transpose() {
  marks xpaper;
  for (auto const &dot : paper)
    xpaper.emplace(dot.second, dot.first);
  paper = xpaper;
}

void fold(char axis, int pos) {
  if (axis == 'x')
    transpose();
  fold_up(pos);
  if (axis == 'x')
    transpose();
}

// Attempt to read and do one fold.  Return whether or not the fold
// was successful.
bool fold_once() {
  string line;
  if (!getline(cin, line))
    return false;
  string fold_cmd, along;
  char axis, equals;
  int pos;
  stringstream(line) >> fold_cmd >> along >> axis >> equals >> pos;
  assert(fold_cmd == "fold" && along == "along");
  fold(axis, pos);
  return true;
}

void print() {
  // The natural way to print is line-by-line, so flip first
  transpose();
  int next_x = 0;
  int next_y = 0;
  for (auto const &dot : paper) {
    while (next_x < dot.first) {
      cout << '\n';
      ++next_x;
      next_y = 0;
    }
    while (next_y < dot.second) {
      cout << ' ';
      ++next_y;
    }
    cout << 'o';
    ++next_y;
  }
  cout << '\n';
}

void part1() {
  fold_once();
  cout << paper.size() << '\n';
}

void part2() {
  while (fold_once())
    ;
  print();
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " partnum < input\n";
    exit(1);
  }
  read();
  if (*argv[1] == '1')
    part1();
  else
    part2();
  return 0;
}
