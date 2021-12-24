#include <iostream>
#include <string>
#include <stdio.h>
#include <assert.h>

using namespace std;

// Compiles the input to produce a checking program.  Supply model
// number as the first command line argument (14 digits).  Prints
// values of w, x, y, and z for each step.

void dumpreg() {
  printf("printf(\"%%2d: w=%%ld x=%%ld y=%%ld z=%%ld\\n\", step++, w, x, y, z);\n");
}

int main(int argc, char **argv) {
  printf("#include <stdio.h>\n");
  printf("#include <assert.h>\n");
  printf("int main(int argc, char **argv) {\n");
  printf("char *next_input = argv[1];\n");
  printf("int step = 0;\n");
  printf("long w = 0, x = 0, y = 0, z = 0;\n");
  string cmd;
  char reg;
  while (cin >> cmd >> reg) {
    assert(reg == 'w' || reg == 'x' || reg == 'y' || reg == 'z');
    if (cmd == "inp") {
      dumpreg();
      printf("%c = *next_input++ - '0';\n", reg);
      continue;
    }
    string op;
    cin >> op;
    char const *opstr = op.c_str();
    if (cmd == "add")
      printf("%c += %s;\n", reg, opstr);
    else if (cmd == "mul")
      printf("%c *= %s;\n", reg, opstr);
    else if (cmd == "div")
      printf("%c /= %s;\n", reg, opstr);
    else if (cmd == "mod")
      printf("%c %%= %s;\n", reg, opstr);
    else {
      assert(cmd == "eql");
      printf("%c = (%c == %s);\n", reg, reg, opstr);
    }
  }
  printf("assert(*next_input == '\\0');\n");
  dumpreg();
  printf("}\n");
  return 0;
}
