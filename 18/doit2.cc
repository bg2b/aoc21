#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct sfnum {
  // >= 0 means leaf
  int val{-1};
  // children
  sfnum *left{nullptr}, *right{nullptr};
  // non-null if nested
  sfnum *parent;

  sfnum(istream &in, sfnum *parent_ = nullptr);
  sfnum(sfnum const &sfn, sfnum *parent_ = nullptr);
  sfnum(int val_, sfnum *parent_) : val(val_), parent(parent_) {}
  ~sfnum();

  sfnum &operator=(sfnum const &sfn) = delete;

  bool is_leaf() const { return val >= 0; }

  // Find leftmost exploding pair, null if none
  sfnum *explode_pos(int level = 0);
  // Add v to the rightmost number to the left of child (if any)
  void augment_left(sfnum *child, int v);
  // Add v to the leftmost number to the right of child (if any)
  void augment_right(sfnum *child, int v);
  // Do an explode if possible, return whether one was done
  bool explode();

  // Split leftmost number >= 10 (if any), return whether a split was
  // done
  bool split();

  // Do explodes and splits to reduce a number as much as possible
  void reduce();

  // Add another sfnum and reduce the result
  void add(sfnum const &sfn);

  unsigned magnitude() const;

  string str() const;
  void print() const { printf("%s\n", str().c_str()); }
};

sfnum::sfnum(istream &in, sfnum *parent_) : parent(parent_) {
  auto read1 = [&](char expected) {
    char c = in.get();
    assert(c == expected);
  };
  if (in.peek() == '[') {
    read1('[');
    val = -1;
    left = new sfnum(in, this);
    read1(',');
    right = new sfnum(in, this);
    read1(']');
  } else {
    in >> val;
  }
  if (!parent)
    read1('\n');
}

sfnum::sfnum(sfnum const &sfn, sfnum *parent_) : parent(parent_) {
  if (sfn.is_leaf()) {
    val = sfn.val;
  } else {
    left = new sfnum(*sfn.left, this);
    right = new sfnum(*sfn.right, this);
  }
}

sfnum::~sfnum() {
  if (!is_leaf()) {
    delete left;
    delete right;
  }
}

sfnum *sfnum::explode_pos(int level) {
  if (is_leaf())
    return nullptr;
  else {
    auto pos = left->explode_pos(level + 1);
    if (pos)
      return pos;
    pos = right->explode_pos(level + 1);
    if (pos)
      return pos;
    if (level < 4)
      return nullptr;
    assert(left->is_leaf() && right->is_leaf());
    return this;
  }
}

void sfnum::augment_left(sfnum *child, int v) {
  assert(!is_leaf() && (left == child || right == child));
  if (right == child) {
    sfnum *sibling = left;
    while (!sibling->is_leaf())
      sibling = sibling->right;
    sibling->val += v;
  } else if (parent)
    parent->augment_left(this, v);
}

void sfnum::augment_right(sfnum *child, int v) {
  assert(!is_leaf() && (left == child || right == child));
  if (left == child) {
    sfnum *sibling = right;
    while (!sibling->is_leaf())
      sibling = sibling->left;
    sibling->val += v;
  } else if (parent)
    parent->augment_right(this, v);
}

bool sfnum::explode() {
  auto pos = explode_pos();
  if (!pos)
    return false;
  assert(pos->parent);
  pos->parent->augment_left(pos, pos->left->val);
  pos->parent->augment_right(pos, pos->right->val);
  if (pos->parent->left == pos)
    pos->parent->left = new sfnum(0, pos->parent);
  else {
    assert(pos->parent->right == pos);
    pos->parent->right = new sfnum(0, pos->parent);
  }
  delete pos;
  return true;
}

bool sfnum::split() {
  if (is_leaf()) {
    if (val < 10)
      return false;
    left = new sfnum(val / 2, this);
    right = new sfnum((val + 1) / 2, this);
    val = -1;
    return true;
  }
  return left->split() || right->split();
}

void sfnum::reduce() {
  while (explode() || split())
    ;
}

void sfnum::add(sfnum const &sfn) {
  assert(!is_leaf());
  sfnum *new_left = new sfnum(*this, this);
  delete left;
  delete right;
  left = new_left;
  right = new sfnum(sfn, this);
  reduce();
}

unsigned sfnum::magnitude() const {
  if (is_leaf())
    return val;
  return 3 * left->magnitude() + 2 * right->magnitude();
}

string sfnum::str() const {
  if (is_leaf())
    return to_string(val);
  else
    return "[" + left->str() + "," + right->str() + "]";
}

int main(int argc, char **argv) {
  vector<sfnum> sfns;
  while (cin.peek() == '[')
    sfns.push_back(sfnum(cin));
  unsigned max_mag = 0;
  for (size_t i = 0; i < sfns.size(); ++i)
    for (size_t j = 0; j < sfns.size(); ++j)
      if (j != i) {
	sfnum sum(sfns[i]);
	sum.add(sfns[j]);
	max_mag = max(max_mag, sum.magnitude());
      }
  printf("Maximum magnitude %u\n", max_mag);
  return 0;
}
