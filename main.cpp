#include <iostream>

#include "dfa.h"

void testParsingV1() {
  std::cout << "\033[1;33m--- Parsing test for RE \033[1;36m(a|b)*abb\033[1;33m ---\033[0m\n";
  auto ret = DFA::generateDfaFromRE("(a|b)*abb");
  if (ret.err) {
    std::cout << "\033[1;31mERROR, DFA could not be created\033[0m\n";
  }
  auto dfa = *ret.data;
  auto str = "abb";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "abababb";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "aaaabb";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "bbaabbabb";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "abba";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "123";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
}

void testParsingV2() {
  std::cout << "\033[1;33m--- Parsing test for RE \033[1;36m(ab*|123)|bba*\033[1;33m ---\033[0m\n";
  auto ret = DFA::generateDfaFromRE("(ab*|123)|bba*");
  if (ret.err) {
    std::cout << "\033[1;31mERROR, DFA could not be created\033[0m\n";
  }
  auto dfa = *ret.data;
  auto str = "123";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "abbb";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "a";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "bb";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "bbbaaa";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "b";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
}
void generatingTest(const std::string& expression) {
  std::cout << "\033[1;33m--- Generating DFA for RE \033[1;36m" << expression << "\033[1;33m ---\033[0m\n";
  auto ret = DFA::generateDfaFromRE(expression);
  if (ret.err) {
    std::cout << "\033[1;31mERROR, DFA could not be created becasue: \033[0;31m" << (*ret.err).msg << "\033[0m\n";
    return;
  }
  std::cout << "\033[1;32m--- DFA generated correctly ---\033[0m\n";
}

void testAll() {
  testParsingV1();
  testParsingV2();
  generatingTest("(aa|b*a)*|(123|bc*d)");
  generatingTest("(aa)*|");
  generatingTest("|abcd");
  generatingTest("*aa");
  generatingTest("(aa)*|()");
}

void printHelp() {
  std::cout << "\t-h -- prints help\n\t-test -- runs all tests\n\t-run <expression> <string> -- generates a DFA from "
               "the first argument, if possible, and checks if the second argument can be accepted by the dfa. "
               "Expression and string parameters must be passed in apostrophe\n";
}
int main(int argc, char** argv) {
  if (argc == 1) {
    std::cout << "no parameter was passed, printing help\n";
    printHelp();
    return 0;
  }
  auto flag = std::string(argv[1]);
  if (flag == "-run") {
    if (argc != 4) {
      std::cout << " Incorrect number of parameters!\n";
      return 0;
    }
    std::string expression = argv[2];
    std::cout << "\033[1;33m--- Generating DFA for RE \033[1;36m" << expression << "\033[1;33m ---\033[0m\n";
    auto ret = DFA::generateDfaFromRE(expression);
    if (ret.err) {
      std::cout << "\033[1;31mERROR, DFA could not be created becasue: \033[0;31m" << (*ret.err).msg << "\033[0m\n";
      return 0;
    }
    auto dfa = *ret.data;
    dfa.print();
    std::string str(argv[3]);
    std::cout << "string '" << str
              << (dfa.parseExpression(str) ? "' is\033[1;32m correct\033[0m\n" : "' is \033[1;31mincorrect\033[0m\n");
  } else if (flag == "-test") {
    if (argc != 2) {
      std::cout << " Incorrect number of parameters!\n";
      return 0;
    }
    testAll();
  } else if (flag == "-h") {
    printHelp();
  } else {
    std::cout << " Incorrect argument passed compile with '-h' to see help\n";
  }

  return 1;
}
