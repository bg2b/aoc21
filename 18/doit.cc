// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doitx doitx.cc
// ./doitx 1 < input  # part 1
// ./doitx 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

struct sfnum {
  // >= 0 means leaf
  int val{-1};
  // children
  sfnum *child[2]{nullptr, nullptr};
  // non-null if nested
  sfnum *parent{nullptr};

  // Construct from string
  sfnum(stringstream &in, sfnum *parent_ = nullptr);
  // Copy constructor
  sfnum(sfnum const &sfn, sfnum *parent_ = nullptr);
  // Make a leaf
  sfnum(int val_, sfnum *parent_) : val(val_), parent(parent_) {}
  ~sfnum();

  sfnum &operator=(sfnum const &sfn) = delete;

  bool is_leaf() const { return val >= 0; }

  // Find leftmost exploding pair, null if none
  sfnum *explode_pos(int level = 0);
  // Add v to the rightmost/leftmost number to the left/right of c (if
  // any)
  void augment(sfnum *c, int v, int which);
  // Do an explode if possible, return whether one was done
  bool explode();

  // Split leftmost number >= 10, return whether a split was done
  bool split();

  // Do explodes and splits to reduce a number as much as possible
  void reduce();

  // Add another sfnum and reduce the result
  void add(sfnum const &sfn);

  unsigned magnitude() const;
};

sfnum::sfnum(stringstream &in, sfnum *parent_) : parent(parent_) {
  auto read1 = [&](char expected) {
    char c = in.get();
    assert(c == expected);
  };
  if (in.peek() == '[') {
    read1('[');
    val = -1;
    child[0] = new sfnum(in, this);
    read1(',');
    child[1] = new sfnum(in, this);
    read1(']');
  } else
    in >> val;
}

sfnum::sfnum(sfnum const &sfn, sfnum *parent_) : parent(parent_) {
  if (sfn.is_leaf())
    val = sfn.val;
  else {
    child[0] = new sfnum(*sfn.child[0], this);
    child[1] = new sfnum(*sfn.child[1], this);
  }
}

sfnum::~sfnum() {
  if (!is_leaf()) {
    delete child[0];
    delete child[1];
  }
}

sfnum *sfnum::explode_pos(int level) {
  if (is_leaf())
    return nullptr;
  for (int i = 0; i < 2; ++i)
    if (auto pos = child[i]->explode_pos(level + 1); pos)
      return pos;
  if (level < 4)
    return nullptr;
  assert(child[0]->is_leaf() && child[1]->is_leaf());
  return this;
}

void sfnum::augment(sfnum *c, int v, int which) {
  assert(!is_leaf() && (child[0] == c || child[1] == c));
  if (child[which] == c) {
    sfnum *sibling = child[1 - which];
    while (!sibling->is_leaf())
      sibling = sibling->child[which];
    sibling->val += v;
  } else if (parent)
    parent->augment(this, v, which);
}

bool sfnum::explode() {
  auto pos = explode_pos();
  if (!pos)
    return false;
  assert(pos->parent);
  pos->parent->augment(pos, pos->child[0]->val, 1);
  pos->parent->augment(pos, pos->child[1]->val, 0);
  if (pos->parent->child[0] == pos)
    pos->parent->child[0] = new sfnum(0, pos->parent);
  else {
    assert(pos->parent->child[1] == pos);
    pos->parent->child[1] = new sfnum(0, pos->parent);
  }
  delete pos;
  return true;
}

bool sfnum::split() {
  if (is_leaf()) {
    if (val < 10)
      return false;
    child[0] = new sfnum(val / 2, this);
    child[1] = new sfnum((val + 1) / 2, this);
    val = -1;
    return true;
  }
  return child[0]->split() || child[1]->split();
}

void sfnum::reduce() {
  while (explode() || split())
    ;
}

void sfnum::add(sfnum const &sfn) {
  assert(!is_leaf());
  sfnum *new0 = new sfnum(*this, this);
  delete child[0];
  delete child[1];
  child[0] = new0;
  child[1] = new sfnum(sfn, this);
  reduce();
}

unsigned sfnum::magnitude() const {
  if (is_leaf())
    return val;
  return 3 * child[0]->magnitude() + 2 * child[1]->magnitude();
}

vector<sfnum> read_nums() {
  vector<sfnum> result;
  string line;
  while (getline(cin, line)) {
    stringstream in(line);
    result.push_back(in);
  }
  return result;
}

void part1() {
  auto sfns = read_nums();
  sfnum total = sfns.front();
  for (size_t i = 1; i < sfns.size(); ++i)
    total.add(sfns[i]);
  cout << total.magnitude() << '\n';
}

void part2() {
  auto sfns = read_nums();
  unsigned max_mag = 0;
  for (size_t i = 0; i < sfns.size(); ++i)
    for (size_t j = 0; j < sfns.size(); ++j)
      if (j != i) {
        sfnum sum(sfns[i]);
        sum.add(sfns[j]);
        max_mag = max(max_mag, sum.magnitude());
      }
  cout << max_mag << '\n';
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " partnum < input\n";
    exit(1);
  }
  if (*argv[1] == '1')
    part1();
  else
    part2();
  return 0;
}
