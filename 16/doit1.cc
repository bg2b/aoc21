#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <assert.h>

using namespace std;

struct transmission {
  vector<bool> bits;
  unsigned read_pos{0};

  transmission(string const &BITS);

  // Get the next bit, advance read_pos
  bool read_bit();

  // Read a bunch of bits as a number
  size_t read_bits(unsigned num_bits);
};

transmission::transmission(string const &BITS) {
  for (auto c : BITS) {
    if (c >= '0' && c <= '9')
      c -= '0';
    else {
      assert(c >= 'A' && c <= 'F');
      c -= 'A';
      c += 10;
    }
    for (int i = 0; i < 4; ++i)
      bits.push_back((c & (0x8 >> i)) != 0);
  }
}

bool transmission::read_bit() {
  assert(read_pos < bits.size());
  return bits[read_pos++];
}

size_t transmission::read_bits(unsigned num_bits) {
  size_t result = 0;
  assert(num_bits <= 8 * sizeof(result));
  // MSB first
  while (num_bits--) {
    result <<= 1;
    if (read_bit())
      ++result;
  }
  return result;
}

struct packet {
  unsigned version;
  unsigned type_id;
  size_t literal{0};
  vector<packet> subpackets;

  packet(transmission &xmission);
  // Used by constructor for reading a literal
  void read_literal(transmission &xmission);
  // Used by constructor for reading an operator
  void read_operator(transmission &xmission);

  unsigned version_sum() const;
};

packet::packet(transmission &xmission) {
  version = xmission.read_bits(3);
  type_id = xmission.read_bits(3);
  if (type_id == 4)
    read_literal(xmission);
  else
    read_operator(xmission);
}

void packet::read_literal(transmission &xmission) {
  for (bool has_next_nybble = true; has_next_nybble; ) {
    unsigned nybble = xmission.read_bits(5);
    has_next_nybble = (nybble & 0x10) != 0;
    literal <<= 4;
    literal += nybble & ~0x10;
  }
}

void packet::read_operator(transmission &xmission) {
  if (xmission.read_bit()) {
    // Subpacket count
    unsigned num_subpackets = xmission.read_bits(11);
    while (num_subpackets--)
      subpackets.push_back(packet(xmission));
  } else {
    // Sum of subpacket lengths
    unsigned sum_of_lengths = xmission.read_bits(15);
    unsigned end_pos = xmission.read_pos + sum_of_lengths;
    while (xmission.read_pos < end_pos)
      subpackets.push_back(packet(xmission));
    assert(xmission.read_pos == end_pos);
  }
}

unsigned packet::version_sum() const {
  unsigned result = version;
  for (auto const &subpacket : subpackets)
    result += subpacket.version_sum();
  return result;
}

int main(int argc, char **argv) {
  string line;
  getline(cin, line);
  transmission xmission(line);
  printf("Version sum %u\n", packet(xmission).version_sum());
  return 0;
}
