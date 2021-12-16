#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
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

  size_t eval() const;
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

size_t packet::eval() const {
  vector<size_t> args;
  for (auto const &subpacket : subpackets)
    args.push_back(subpacket.eval());
  switch (type_id) {
  case 0:
    return accumulate(args.begin(), args.end(), size_t(0));
  case 1:
    return accumulate(args.begin(), args.end(),
		      size_t(1), multiplies<size_t>());
  case 2:
    assert(!args.empty());
    return *min_element(args.begin(), args.end());
  case 3:
    assert(!args.empty());
    return *max_element(args.begin(), args.end());
  case 4:
    return literal;
  case 5:
    assert(args.size() == 2);
    return args[0] > args[1] ? 1 : 0;
  case 6:
    assert(args.size() == 2);
    return args[0] < args[1] ? 1 : 0;
  default:
    assert(type_id == 7);
    assert(args.size() == 2);
    return args[0] == args[1] ? 1 : 0;
  }
}

int main(int argc, char **argv) {
  string line;
  getline(cin, line);
  transmission xmission(line);
  printf("Packet value %zu\n", packet(xmission).eval());
  return 0;
}
