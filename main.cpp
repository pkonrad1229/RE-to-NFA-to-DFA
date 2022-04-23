
#include <iostream>

#include "dfa.h"
#include "nfa.h"
#include "reg_exp.h"
using namespace std;

int main(int argc, char** argv) {
  if (argc != 2) return 0;

  string expression = argv[1];

  auto final = DFA::generateDfaFromRE(expression);
  if (!final.err) {
    final.data.value().print();
  }

  return 1;
}
