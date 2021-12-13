#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct paper {
  vector<string> lines;

  paper(istream &in);

  // Set a dot, called from constructor
  void set(unsigned x, unsigned y);
  // Final step of constructor, makes sure all lines are the same
  // length
  void even_up();

  int num_dots() const;

  // Swap x, y so I only have to deal with folds along y
  void transpose();

  // Return overlap of two lines
  string overlap(string const &line1, string const &line2) const;
  // Fold the paper
  void fold_up(unsigned y);

  void print() const;
};

paper::paper(istream &in) {
  string line;
  while (getline(in, line)) {
    unsigned x, y;
    char comma;
    if (!(stringstream(line) >> x >> comma >> y))
      break;
    assert(comma == ',');
    set(x, y);
  }
  even_up();
}

void paper::set(unsigned x, unsigned y) {
  while (y >= lines.size())
    lines.push_back("");
  while (x >= lines[y].size())
    lines[y].push_back(' ');
  lines[y][x] = '*';
}

void paper::even_up() {
  size_t max_length = 0;
  for (auto const &line : lines)
    max_length = max(max_length, line.size());
  for (auto &line : lines)
    line.resize(max_length, ' ');
}

int paper::num_dots() const {
  int result = 0;
  for (auto const &line : lines)
    for (auto pos : line)
      result += (pos != ' ');
  return result;
}

void paper::transpose() {
  assert(!lines.empty());
  auto n = lines[0].size();
  vector<string> xlines(n);
  for (auto const &line : lines)
    for (size_t i = 0; i < n; ++i)
      xlines[i].push_back(line[i]);
  lines = xlines;
}

string paper::overlap(string const &line1, string const &line2) const {
  assert(line1.size() == line2.size());
  string result(line1);
  for (size_t i = 0; i < line2.size(); ++i)
    if (line2[i] != ' ')
      result[i] = line2[i];
  return result;
}

void paper::fold_up(unsigned pos) {
  // The fold may not leave two equal size pieces, but the bottom part
  // should not extend beyond the top after folding
  assert(pos >= lines.size() / 2);
  // The fold line should be empty
  assert(lines[pos] == string(lines[pos].size(), ' '));
  // Line at pos + k folds up and overlaps onto pos - k
  for (unsigned k = 1; pos + k < lines.size(); ++k)
    lines[pos - k] = overlap(lines[pos - k], lines[pos + k]);
  lines.resize(pos);
}

void paper::print() const {
  for (auto const &line : lines)
    printf("%s\n", line.c_str());
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
  instructions.print();
  return 0;
}
