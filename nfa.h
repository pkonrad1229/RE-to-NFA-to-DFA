#pragma once

#include "reg_exp.h"

class NfaNode {
  std::shared_ptr<NfaNode> _left;   // pointer to next (or previous in case of kleene closure) node
  std::shared_ptr<NfaNode> _right;  // pointer to the second transition (only possible in the case of eps transition)
  bool _eps;                        // true if the node has epsilon transition(s)
  char _symbol{};                   // symbol of the transition, if transition is not eps
  size_t _id{};                     // id of a node, node with id = 1 is the starting node
  bool _was_set{true};  // this boolean is used in setting node ids and printing, to ensure that a single node isn't
                        // affected more than once

 public:
  NfaNode(std::shared_ptr<NfaNode> left, const char& symbol) : _left(std::move(left)), _symbol(symbol), _eps(false) {}
  explicit NfaNode(std::shared_ptr<NfaNode> left) : _left(std::move(left)), _eps(true) {}
  NfaNode(std::shared_ptr<NfaNode> left, std::shared_ptr<NfaNode> right)
      : _left(std::move(left)), _right(std::move(right)), _eps(true) {}
  NfaNode() : _eps(false) {}

  const std::shared_ptr<NfaNode>& getLeft() { return _left; }
  const std::shared_ptr<NfaNode>& getRight() { return _right; }
  [[nodiscard]] bool isEpsilon() const { return _eps; }
  [[nodiscard]] char getSymbol() const { return _symbol; }
  size_t& getId() { return _id; }
  bool& getWasSet() { return _was_set; }

  void setLeft(const std::shared_ptr<NfaNode>& node) { _left = node; }
  void setRight(const std::shared_ptr<NfaNode>& node) { _right = node; }
  void setEpsilon(bool eps) { _eps = eps; }
  void setId(const size_t& id) { _id = id; }

  void setNode(const NfaNode& t);
};

typedef std::shared_ptr<NfaNode> SPNfaNode;

class NfaStructure {
  SPNfaNode _start;
  SPNfaNode _final;
  size_t _num_of_nodes;

  NfaStructure() : _num_of_nodes(0) {}

  bool isFinal(const SPNfaNode& node) { return _final == node; }
  void setStart(const SPNfaNode& node) { _start = node; }
  void setFinal(const SPNfaNode& node) { _final = node; }
  size_t& getSize() { return _num_of_nodes; }
  void increaseAllIds(const size_t& num);
  static ErrOr<NfaStructure> generateNfaFromExpression(const SPExpression& expr);
  void increaseIds(const SPNfaNode& root, const size_t& num);
  void setWasIncreased(const SPNfaNode& root);

  void print(const SPNfaNode& root);

 public:
  /**
   * Function that prints the Nfa Structure
   */
  void print();

  [[nodiscard]] const SPNfaNode& getStart() const { return _start; }
  [[nodiscard]] const SPNfaNode& getFinal() const { return _final; }

  /**
   * Function that recursively transforms a tree
   * @param expression string with the RE
   * @return a NfaStructure, or error
   */
  static ErrOr<NfaStructure> generateNfaFromRE(const std::string& expression, bool print = false);
};
