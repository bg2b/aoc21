// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

// The image enhancement algorithm
string algorithm;

struct image {
  // The infinite outside set of pixels has this value
  char outside{'.'};
  // The non-infinite portion
  vector<string> pixels;

  // Read from cin
  image();

  int H() const { return pixels.size(); }
  int W() const { return pixels[0].size(); }

  // Returns outside for anything beyond the explicit range
  char at(int i, int j) const;

  char enhanced_pixel(int i, int j) const;

  // Said in loud voice to computer
  void Enhance();

  // How many pixels are lit?
  int num_lit() const;
};

image::image() {
  string line;
  while (getline(cin, line)) {
    pixels.push_back(line);
    assert(pixels.back().size() == pixels.front().size());
  }
}

char image::at(int i, int j) const {
  if (i < 0 || i >= H() || j < 0 || j >= W())
    return outside;
  return pixels[i][j];
}

char image::enhanced_pixel(int i, int j) const {
  // Offset (-1, -1) is MSB
  unsigned index = 0;
  for (int di = -1; di <= 1; ++di)
    for (int dj = -1; dj <= 1; ++dj) {
      index *= 2;
      if (at(i + di, j + dj) != '.')
        ++index;
    }
  assert(index < algorithm.size());
  return algorithm[index];
}

void image::Enhance() {
  // How does the outside transform?
  char new_outside = enhanced_pixel(-10, -10);
  // Transform the explicit part
  vector<string> new_pixels;
  for (int i = -1; i <= H(); ++i) {
    string row;
    for (int j = -1; j <= W(); ++j)
      row.push_back(enhanced_pixel(i, j));
    new_pixels.push_back(row);
  }
  outside = new_outside;
  pixels = new_pixels;
}

int image::num_lit() const {
  if (outside != '.')
    // Infinite lit pixels
    return -1;
  int result = 0;
  for (int i = 0; i < H(); ++i)
    for (int j = 0; j < W(); ++j)
      if (at(i, j) != '.')
        ++result;
  return result;
}

void solve(bool part1) {
  getline(cin, algorithm);
  assert(algorithm.size() == 512);
  string blank;
  getline(cin, blank);
  assert(blank.empty());
  image img;
  int iterations = part1 ? 2 : 50;
  for (int _ = 0; _ < iterations; ++_)
    img.Enhance();
  cout << img.num_lit() << '\n';
}

void part1() { solve(true); }
void part2() { solve(false); }

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
