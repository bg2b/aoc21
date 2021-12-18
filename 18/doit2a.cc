#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct sfnum {
  int val{-1};
  vector<sfnum> children;

  sfnum(istream &in);
  sfnum(int val_) : val(val_) {}

  // Unreduced addition
  sfnum(sfnum const &sfn1, sfnum const &sfn2);

  bool is_leaf() const { return children.empty(); }

  // Back up, move to sibling, descend to a leaf, add v.  Use dir 0
  // for augmenting left and dir 1 for right.
  void augment(vector<pair<int, sfnum *>> path, int dir, int v);
  // Path has choice of child and pointer to parent
  bool explode_aux(vector<pair<int, sfnum *>> &path);
  // Do an explode if possible, return whether one was done
  bool explode();

  // Split leftmost number >= 10 (if any), return whether a split was
  // done
  bool split();

  // Do explodes and splits to reduce a number as much as possible
  void reduce();

  unsigned magnitude() const;

  string str() const;
  void print() const { printf("%s\n", str().c_str()); }
};

sfnum::sfnum(istream &in) {
  auto read1 = [&](char expected) {
    char c = in.get();
    assert(c == expected);
  };
  if (in.peek() == '[') {
    read1('[');
    children.push_back(sfnum(in));
    read1(',');
    children.push_back(sfnum(in));
    read1(']');
    if (in.peek() == '\n')
      read1('\n');
  } else
    in >> val;
}

sfnum::sfnum(sfnum const &sfn1, sfnum const &sfn2) {
  children.push_back(sfn1);
  children.push_back(sfn2);
}

void sfnum::augment(vector<pair<int, sfnum *>> path, int dir, int v) {
  while (!path.empty() && path.back().first == dir)
    path.pop_back();
  if (!path.empty()) {
    auto adj = &path.back().second->children[dir];
    while (!adj->is_leaf())
      adj = &adj->children[1 - dir];
    adj->val += v;
  }
}

bool sfnum::explode_aux(vector<pair<int, sfnum *>> &path) {
  if (is_leaf())
    return false;
  for (int dir : { 0, 1 }) {
    path.emplace_back(dir, this);
    if (children[dir].explode_aux(path))
      return true;
    path.pop_back();
  }
  if (path.size() < 4)
    return false;
  // This node explodes
  assert(children[0].is_leaf() && children[1].is_leaf());
  augment(path, 0, children[0].val);
  augment(path, 1, children[1].val);
  val = 0;
  children.clear();
  return true;
}

bool sfnum::explode() {
  vector<pair<int, sfnum *>> path;
  return explode_aux(path);
}

bool sfnum::split() {
  if (is_leaf()) {
    if (val < 10)
      return false;
    children.push_back(sfnum(val / 2));
    children.push_back(sfnum((val + 1) / 2));
    return true;
  }
  return children[0].split() || children[1].split();
}

void sfnum::reduce() {
  while (explode() || split())
    ;
}

unsigned sfnum::magnitude() const {
  if (is_leaf())
    return val;
  return 3 * children[0].magnitude() + 2 * children[1].magnitude();
}

string sfnum::str() const {
  if (is_leaf())
    return to_string(val);
  return "[" + children[0].str() + "," + children[1].str() + "]";
}

int main(int argc, char **argv) {
  vector<sfnum> sfns;
  while (cin.peek() == '[')
    sfns.push_back(sfnum(cin));
  unsigned max_mag = 0;
  for (size_t i = 0; i < sfns.size(); ++i)
    for (size_t j = 0; j < sfns.size(); ++j)
      if (j != i) {
	sfnum sum(sfns[i], sfns[j]);
	sum.reduce();
	max_mag = max(max_mag, sum.magnitude());
      }
  printf("Maximum magnitude %u\n", max_mag);
  return 0;
}
