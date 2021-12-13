#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct paper {
  using marks = set<pair<unsigned, unsigned>>;
  marks dots;

  paper(istream &in);

  int num_dots() const { return dots.size(); }

  // Swap x, y so I only have to deal with folds along y
  void transpose();

  // Return overlap of two lines
  string overlap(string const &line1, string const &line2) const;
  // Fold the paper
  void fold_up(unsigned y);

  // Since the set is lexicographic by first coordinate, printing is
  // only simple column-by-column.  Do transpose() first for normal
  // printing, or just rotate your head while looking in the mirror.
  void print_sideways() const;
};

paper::paper(istream &in) {
  string line;
  while (getline(in, line)) {
    unsigned x, y;
    char comma;
    if (!(stringstream(line) >> x >> comma >> y))
      break;
    assert(comma == ',');
    dots.emplace(x, y);
  }
}

void paper::transpose() {
  marks xdots;
  for (auto const &dot : dots)
    xdots.emplace(dot.second, dot.first);
  dots = xdots;
}

void paper::fold_up(unsigned pos) {
  marks new_dots;
  for (auto const &dot : dots) {
    // Nothing on the fold line
    assert(dot.second != pos);
    if (dot.second < pos)
      new_dots.insert(dot);
    else {
      // New position must not be below position 0
      assert(pos >= dot.second - pos);
      unsigned new_y = pos - (dot.second - pos);
      new_dots.emplace(dot.first, new_y);
    }
  }
  dots = new_dots;
}

void paper::print_sideways() const {
  unsigned next_x = 0;
  unsigned next_y = 0;
  for (auto const &dot : dots) {
    while (next_x < dot.first) {
      fputc('\n', stdout);
      ++next_x;
      next_y = 0;
    }
    while (next_y < dot.second) {
      fputc(' ', stdout);
      ++next_y;
    }
    fputc('o', stdout);
    ++next_y;
  }
  fputc('\n', stdout);
}

int main(int argc, char **argv) {
  paper instructions(cin);
  string line;
  while (getline(cin, line)) {
    string fold, along, where;
    if (!(stringstream(line) >> fold >> along >> where))
      break;
    printf("%d dots before fold\n", instructions.num_dots());
    assert(fold == "fold");
    assert(along == "along");
    assert(where.size() >= 3);
    assert((where[0] == 'x' || where[0] == 'y') && where[1] == '=');
    int pos = atoi(&where[2]);
    if (where[0] == 'x')
      instructions.transpose();
    instructions.fold_up(pos);
    if (where[0] == 'x')
      instructions.transpose();
  }
  printf("%d dots after all folds\n", instructions.num_dots());
  instructions.transpose();
  instructions.print_sideways();
  return 0;
}
