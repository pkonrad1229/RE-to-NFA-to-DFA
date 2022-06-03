#include <iostream>

#include "dfa.h"

void testParsingV1() {
  std::cout << YELLOW << "--- Parsing test for RE " << CYAN << "(a|b)*abb" << YELLOW << " ---" << RESET << "\n";
  auto ret = DFA::generateDfaFromRE("(a|b)*abb");
  if (ret.err) {
    std::cout << RED << "ERROR, DFA could not be created" << RESET << "\n";
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
  std::cout << YELLOW << "--- Parsing test for RE " << CYAN << "(ab*|123)|bba*" << YELLOW << " ---" << RESET << "\n";
  auto ret = DFA::generateDfaFromRE("(ab*|123)|bba*");
  if (ret.err) {
    std::cout << RED << "ERROR, DFA could not be created" << RESET << "\n";
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

void testParsingV3() {
  std::cout << YELLOW
            << "--- Parsing test for RE "
               ""
            << CYAN
            << "(1|2|3|4|5|6|7|8|9|0)(1|2|3|4|5|6|7|8|9|0)-(1|2|3|4|5|6|7|8|9|0)(1|2|3|4|5|6|7|8|9|0)-(1|"
               "2|3|4|5|6|7|8|9|0)*"
            << YELLOW << " ---" << RESET << "\n";
  auto ret = DFA::generateDfaFromRE(
      "(1|2|3|4|5|6|7|8|9|0)(1|2|3|4|5|6|7|8|9|0)-(1|2|3|4|5|6|7|8|9|0)(1|2|3|4|5|6|7|8|9|0)-(1|"
      "2|3|4|5|6|7|8|9|0)*");
  if (ret.err) {
    std::cout << RED << "ERROR, DFA could not be created" << RESET << "\n";
  }
  auto dfa = *ret.data;
  auto str = "05-12-1999";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "05-11-123";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "01-02-3";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "00-00-00";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "11-2-3";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "1-22-33";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "11-22--33";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
  str = "a1-22-33";
  std::cout << str << (dfa.parseExpression(str) ? " correct\n" : " incorrect\n");
}
void generatingTest(const std::string& expression) {
  std::cout << YELLOW << "--- Generating DFA for RE " << CYAN << "" << expression << YELLOW << " ---" << RESET << "\n";
  auto ret = DFA::generateDfaFromRE(expression);
  if (ret.err) {
    std::cout << RED << "ERROR, DFA could not be created becasue: " << SMALLRED << (*ret.err).msg << "" << RESET
              << "\n";
    return;
  }
  std::cout << GREEN << "--- DFA generated correctly ---" << RESET << "\n";
}

void creationTest(const std::string& expression) {
  std::cout << YELLOW << "--- Step-by-step DFA creation for RE " << CYAN << "" << expression << YELLOW << " ---"
            << RESET << "\n";
  auto ret = DFA::generateDfaFromRE(expression, true);
  if (ret.err) {
    std::cout << RED << "ERROR, DFA could not be created becasue: " << SMALLRED << (*ret.err).msg << "" << RESET
              << "\n";
    return;
  }
  auto dfa = *ret.data;
  dfa.print();
}

void testAll() {
  testParsingV1();
  testParsingV2();
  testParsingV3();
  generatingTest("(aa|b*a)*|(123|bc*d)");
  generatingTest("(((a|b)*)*)*|1*2(1*|2*)*");
  generatingTest("(((a|b)*)*)*");
  generatingTest("a**");
  generatingTest("(((a*|(bc)*d)|123*)OR(E*F*g|hi(Jk)*)lMnOp)*qrS (PuVW|xYz)*");
  generatingTest("((((((a))))*|(((((d)))*))))");
  generatingTest(
      "(1|2|3|4|5|6|7|8|9|0)*(1|2|3|4|5|6|7|8|9|0)*(1|2|3|4|5|6|7|8|9|0)*(1|2|3|4|5|6|7|8|9|0)*(1|2|3|4|5|6|7|8|9|0)*");

  generatingTest("(aa)*|");
  generatingTest("|abcd");
  generatingTest("*aa");
  generatingTest("(aa)*|()");
  generatingTest("()*");
  generatingTest("((ab|cd*)ef|g12(123)*");
  generatingTest("ab(|123)");
  generatingTest("ab(123|)");
  generatingTest("ab|(123|456)*||d");
  generatingTest("ab*|*");

  creationTest("(a|bc)*|12*3");
  creationTest("((123)*4*|aBc)*");
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
    std::cout << YELLOW << "--- Generating DFA for RE " << CYAN << "" << expression << YELLOW << " ---" << RESET
              << "\n";
    auto ret = DFA::generateDfaFromRE(expression, true);
    if (ret.err) {
      std::cout << RED << "ERROR, DFA could not be created becasue: " << SMALLRED << (*ret.err).msg << "" << RESET
                << "\n";
      return 0;
    }
    auto dfa = *ret.data;
    dfa.print();
    std::string str(argv[3]);
    std::cout << "string '" << str
              << (dfa.parseExpression(str) ? std::string("' is") + GREEN + " correct" + RESET + "\n"
                                           : std::string("' is ") + RED + "incorrect" + RESET + "\n");
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
