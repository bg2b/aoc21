#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <limits>
#include <algorithm>
#include <memory>
#include <stdio.h>
#include <assert.h>

using namespace std;

// And now for something completely different...
//
// This uses a (naive) implementation of BDDs.  Probably it would be
// reasonably competitive time-wise with a real library.  This version
// is about 5x slower than doit2a.cc on the large example in input.
//
// For a quick intro to BDDs, see
// https://en.wikipedia.org/wiki/Binary_decision_diagram

struct bdd;

using BDD = shared_ptr<bdd const>;
using wBDD = weak_ptr<bdd const>;

struct bdd {
  // Variable number
  unsigned index;
  // Unique ID, used in the unique table and caches to avoid pointer
  // hashing and ref count updating
  unsigned id;
  // children[0] = if 0..., children[1] = if 1...
  BDD children[2];

  bdd(unsigned index_, BDD const &f0, BDD const &f1);
};

bdd::bdd(unsigned index_, BDD const &f0, BDD const &f1) :
  index(index_), children({f0, f1}) {
  static unsigned next_id = 0;
  id = next_id++;
}

map<tuple<unsigned, unsigned, unsigned>, wBDD> unique_bdds;

// Find (or make) a BDD for "if var_index then f0 else f1"; index
// should be less than f0->index and f1->index
BDD bdd_find(unsigned index, BDD const &f0, BDD const &f1) {
  if (f0 == f1)
    return f0;
  auto key = make_tuple(index, f0->id, f1->id);
  auto p = unique_bdds.find(key);
  if (p != unique_bdds.end()) {
    if (auto result = p->second.lock())
      return result;
  }
  BDD const result(new bdd(index, f0, f1));
  unique_bdds[key] = result;
  return result;
}

// Split on variable number index1
BDD cofactor(BDD f, unsigned index1, int which) {
  if (f->index != index1)
    return f;
  return f->children[which];
}

BDD bdd_zero(new bdd(numeric_limits<unsigned>::max(), nullptr, nullptr));
BDD bdd_one(new bdd(numeric_limits<unsigned>::max(), nullptr, nullptr));

BDD bdd_var(unsigned index) {
  return bdd_find(index, bdd_zero, bdd_one);
}

// Real implementations use pointer flags for negation
BDD bdd_not(BDD const &f) {
  map<unsigned, BDD> cache;
  auto not_cached = [&](BDD f_, auto &not_ref) -> BDD {
    if (f_ == bdd_zero)
      return bdd_one;
    if (f_ == bdd_one)
      return bdd_zero;
    auto p = cache.find(f_->id);
    if (p != cache.end())
      return p->second;
    auto result = bdd_find(f_->index,
			   not_ref(f_->children[0], not_ref),
			   not_ref(f_->children[1], not_ref));
    cache[f_->id] = result;
    return result;
  };
  return not_cached(f, not_cached);
}

BDD bdd_and(BDD const &f, BDD const &g) {
  // Real implementations use a hash table as a single (persistent)
  // operation cache
  map<pair<unsigned, unsigned>, BDD> cache;
  auto and_cached = [&](BDD f_, BDD g_, auto &and_ref) -> BDD {
    if (f_ == bdd_zero || g_ == bdd_zero)
      return bdd_zero;
    if (f_ == bdd_one || f_ == g_)
      return g_;
    if (g_ == bdd_one)
      return f_;
    auto key = make_pair(f_->id, g_->id);
    auto p = cache.find(key);
    if (p != cache.end())
      return p->second;
    unsigned index = min(f_->index, g_->index);
    BDD c0 = and_ref(cofactor(f_, index, 0), cofactor(g_, index, 0), and_ref);
    BDD c1 = and_ref(cofactor(f_, index, 1), cofactor(g_, index, 1), and_ref);
    auto result = bdd_find(index, c0, c1);
    cache[key] = result;
    return result;
  };
  return and_cached(f, g, and_cached);
}

BDD bdd_or(BDD const &f, BDD const &g) {
  map<pair<unsigned, unsigned>, BDD> cache;
  auto or_cached = [&](BDD f_, BDD g_, auto &or_ref) -> BDD {
    if (f_ == bdd_one || g_ == bdd_one)
      return bdd_one;
    if (f_ == bdd_zero || f_ == g_)
      return g_;
    if (g_ == bdd_zero)
      return f_;
    auto key = make_pair(f_->id, g_->id);
    auto p = cache.find(key);
    if (p != cache.end())
      return p->second;
    unsigned index = min(f_->index, g_->index);
    BDD c0 = or_ref(cofactor(f_, index, 0), cofactor(g_, index, 0), or_ref);
    BDD c1 = or_ref(cofactor(f_, index, 1), cofactor(g_, index, 1), or_ref);
    auto result = bdd_find(index, c0, c1);
    cache[key] = result;
    return result;
  };
  return or_cached(f, g, or_cached);
}

size_t bdd_size(BDD const &f) {
  set<unsigned> cache;
  auto size_cached = [&](BDD f_, auto &size_ref) {
    if (cache.find(f_->id) != cache.end())
      return;
    cache.insert(f_->id);
    if (f_ == bdd_zero || f_ == bdd_one)
      return;
    size_ref(f_->children[0], size_ref);
    size_ref(f_->children[1], size_ref);
  };
  size_cached(f, size_cached);
  return cache.size();
}

// Fraction of valuations for which the function is true.  Multiply by
// 2^num_vars for the number of valuations of the variables that make
// the formula true.
double bdd_satfrac(BDD const &f) {
  map<unsigned, double> cache;
  auto satfrac_cached = [&](BDD f_, auto &satfrac_ref) -> double {
    if (f_ == bdd_zero)
      return 0.0;
    if (f_ == bdd_one)
      return 1.0;
    auto p = cache.find(f_->id);
    if (p != cache.end())
      return p->second;
    auto result = 0.5 * (satfrac_ref(f_->children[0], satfrac_ref) +
			 satfrac_ref(f_->children[1], satfrac_ref));
    cache[f_->id] = result;
    return result;
  };
  return satfrac_cached(f, satfrac_cached);
}

// Formula for when a variable (encoded in binary) is >= c
BDD bdd_geq(vector<BDD> const &vars, unsigned num_bits, unsigned c) {
  if (num_bits == 0) {
    assert(c == 0);
    return bdd_one;
  }
  unsigned bit = 1 << (num_bits - 1);
  if (c & bit)
    return bdd_and(vars[num_bits - 1],
		   bdd_geq(vars, num_bits - 1, c - bit));
  else
    return bdd_or(vars[num_bits - 1],
		  bdd_geq(vars, num_bits - 1, c));
}

BDD bdd_leq(vector<BDD> const &vars, unsigned num_bits, unsigned c) {
  // The c + 1 may require one extra bit than would be otherwise
  // necessary; could be avoided with a specialized bdd_leq
  return bdd_not(bdd_geq(vars, num_bits, c + 1));
}

// Formula for when a variable (encoded in binary) is >= l and <= h
BDD bdd_range(vector<BDD> const &vars, unsigned l, unsigned h) {
  return bdd_and(bdd_geq(vars, vars.size(), l),
		 bdd_leq(vars, vars.size(), h));
}

// Formula expressing a cube
BDD bdd_cube(vector<BDD> const &xvars, unsigned xl, unsigned xh,
	     vector<BDD> const &yvars, unsigned yl, unsigned yh,
	     vector<BDD> const &zvars, unsigned zl, unsigned zh) {
  return bdd_and(bdd_and(bdd_range(yvars, yl, yh),
			 bdd_range(zvars, zl, zh)),
		 bdd_range(xvars, xl, xh));
}

int main(int argc, char **argv) {
  vector<pair<string, vector<int>>> commands;
  string cmd;
  while (cin >> cmd) {
    assert(cmd == "on" || cmd == "off");
    int xl, xh, yl, yh, zl, zh;
    cin >> xl >> xh >> yl >> yh >> zl >> zh;
    commands.emplace_back(cmd, vector<int>({ xl, xh, yl, yh, zl, zh }));
  }
  // Shift to nonnegative integers and figure out number of bits
  // needed for encoding
  assert(!commands.empty());
  int l = commands[0].second[0];
  for (auto const &command : commands)
    l = min(l, *min_element(command.second.begin(), command.second.end()));
  for (auto &command : commands)
    for (auto &c : command.second)
      c -= l;
  unsigned h = commands[0].second[0];
  for (auto const &command : commands)
    h = max(h, unsigned(*max_element(command.second.begin(), command.second.end())));
  unsigned num_bits = 1;
  while ((1u << num_bits) <= h)
    ++num_bits;
  // Make BDD variables; ordering is MSB-to-LSB for each variable, and
  // from trying it appears that x>y>z is better than an interleaved
  // ordering.  _vars[0] is LSB
  vector<BDD> xvars;
  vector<BDD> yvars;
  vector<BDD> zvars;
  unsigned index = 3 * num_bits;
  for (unsigned i = 0; i < num_bits; ++i)
    zvars.push_back(bdd_var(--index));
  for (unsigned i = 0; i < num_bits; ++i)
    yvars.push_back(bdd_var(--index));
  for (unsigned i = 0; i < num_bits; ++i)
    xvars.push_back(bdd_var(--index));
  assert(index == 0);
  // All cubes off
  BDD reactor = bdd_zero;
  for (size_t i = 0; i < commands.size(); ) {
    // Collect sequences of the same command.  The reactor BDD tends
    // to be the biggest, so it's best to avoid combining it
    // repeatedly with tiny BDDs.  First combining small BDDs for the
    // same command with an OR is faster.
    BDD cubes = bdd_zero;
    size_t j = i;
    while (j < commands.size() && commands[i].first == commands[j].first) {
      auto const &command = commands[j];
      BDD cube = bdd_cube(xvars, command.second[0], command.second[1],
			  yvars, command.second[2], command.second[3],
			  zvars, command.second[4], command.second[5]);
      cubes = bdd_or(cubes, cube);
      ++j;
    }
    if (commands[i].first == "on")
      reactor = bdd_or(reactor, cubes);
    else
      reactor = bdd_and(reactor, bdd_not(cubes));
    if (argc > 1)
      printf("[%zu, %zu) %.16g %zu\n",
	     i, j,
	     bdd_satfrac(reactor) * (1ul << (3 * num_bits)),
	     bdd_size(reactor));
    i = j;
  }
  printf("BDD size %zu, total set %.16g\n",
	 bdd_size(reactor),
	 bdd_satfrac(reactor) * (1ul << (3 * num_bits)));
  return 0;
}
