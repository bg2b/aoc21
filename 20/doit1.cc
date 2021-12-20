#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <assert.h>

using namespace std;

string algorithm;

struct image {
  char outside{'.'};
  vector<string> pixels;

  image(istream &in);

  int H() const { return pixels.size(); }
  int W() const { return pixels[0].size(); }

  // Returns outside for anything beyond the explicit range
  char at(int i, int j) const;

  char enhanced_pixel(int i, int j) const;

  // Said in loud voice to computer
  void Enhance(bool excited);

  int num_lit() const;
};

image::image(istream &in) {
  string line;
  while (getline(in, line)) {
    pixels.push_back(line);
    assert(pixels.back().size() == pixels.front().size());
  }
}

char image::at(int i, int j) const {
  if (i < 0 || i >= H())
    return outside;
  if (j < 0 || j >= W())
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

void image::Enhance(bool) {
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

int main(int argc, char **argv) {
  getline(cin, algorithm);
  assert(algorithm.size() == 512);
  string blank;
  getline(cin, blank);
  assert(blank.empty());
  image img(cin);
  img.Enhance(!!1);
  img.Enhance(!!1);
  printf("%d lit pixels\n", img.num_lit());
  return 0;
}
