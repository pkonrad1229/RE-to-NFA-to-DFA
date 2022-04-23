#include "nfa.h"

void NfaStructure::increaseIds(const SPNfaNode& root, const size_t& num) {
  if (!root || root->getWasSet()) return;
  root->getId() += num;
  root->getWasSet() = true;
  if (root->getLeft()) {
    increaseIds(root->getLeft(), num);
  }
  if (root->getRight()) {
    increaseIds(root->getRight(), num);
  }
}

void NfaStructure::setWasIncreased(const SPNfaNode& root) {
  if (!root || !root->getWasSet()) return;
  root->getWasSet() = false;
  if (root->getLeft()) {
    setWasIncreased(root->getLeft());
  }
  if (root->getRight()) {
    setWasIncreased(root->getRight());
  }
}

ErrOr<NfaStructure> NfaStructure::generateNfaFromExpression(const SPExpression& expr) {
  if (expr->getType() == NodeType::Value) {
    NfaStructure nfa;
    auto temp = std::make_shared<NfaNode>(std::make_shared<NfaNode>(), expr->getValue());
    temp->setId(1);
    temp->getLeft()->setId(2);
    nfa.getSize() = 2;
    nfa.setStart(temp);
    nfa.setFinal(temp->getLeft());
    return std::move(nfa);
  }
  if (!expr->getLeft()) return ERROR_WITH_FILE("Pointer to node expected to exist, but is nullptr");
  auto ret = generateNfaFromExpression(expr->getLeft());
  if (ret.err) return *ret.err;
  auto nfa = *ret.data;
  switch (expr->getType()) {
    case NodeType::Add: {
      ret = generateNfaFromExpression(expr->getRight());
      if (ret.err) return *ret.err;
      auto rhs = *ret.data;
      rhs.increaseAllIds(nfa.getSize() - 1);
      nfa.getSize() += rhs.getSize() - 1;
      nfa.getFinal()->setNode(*rhs.getStart().get());
      nfa.setFinal(rhs.getFinal());
      return std::move(nfa);
    }
    case NodeType::Brackets: {
      return std::move(nfa);
    }
    case NodeType::Star: {
      auto final = std::make_shared<NfaNode>();
      auto start = std::make_shared<NfaNode>(nfa.getStart(), final);

      nfa.increaseAllIds(1);
      start->setId(1);
      final->setId(nfa.getSize() += 2);

      nfa.getFinal()->setEpsilon(true);
      nfa.getFinal()->setLeft(nfa.getStart());
      nfa.getFinal()->setRight(final);

      nfa.setStart(start);
      nfa.setFinal(final);

      return std::move(nfa);
    }
    case ::NodeType::Or: {
      ret = generateNfaFromExpression(expr->getRight());
      if (ret.err) return *ret.err;
      auto rhs = *ret.data;

      auto start = std::make_shared<NfaNode>(nfa.getStart(), rhs.getStart());
      start->setId(1);
      nfa.increaseAllIds(1);
      auto final = std::make_shared<NfaNode>();

      const auto& left_final = nfa.getFinal();
      left_final->setEpsilon(true);
      left_final->setLeft(final);

      rhs.increaseAllIds(++nfa.getSize());
      nfa.getSize() += rhs.getSize();

      const auto& right_final = rhs.getFinal();
      right_final->setEpsilon(true);
      right_final->setLeft(final);

      final->setId(++nfa.getSize());
      nfa.setStart(start);
      nfa.setFinal(final);

      return std::move(nfa);
    }
    default: {
      return ERROR_WITH_FILE("unknown node type");
    }
  }
}

ErrOr<NfaStructure> NfaStructure::generateNfaFromRE(const std::string& expression) {
  RegExpParser parser;
  auto ret = parser.parseExpression(expression);
  if (ret.err) {
    std::cout << "RE error :" << ret.err.value().msg << std::endl;
    return *ret.err;
  }
  return generateNfaFromExpression(ret.data.value().second);
}

void NfaStructure::print(const SPNfaNode& root) {
  if (!root || root->getWasSet() || root == _final) return;
  root->getWasSet() = true;
  std::cout << "id: " << root->getId() << " ,";
  if (root->isEpsilon()) {
    std::cout << "epsilon transition to node " << root->getLeft()->getId();
    if (root->getRight()) {
      std::cout << " and " << root->getRight()->getId();
    }
    std::cout << std::endl;
  } else {
    std::cout << "transition on character '" << root->getSymbol() << "' to node " << root->getLeft()->getId()
              << std::endl;
  }
  print(root->getLeft());
  if (root->getRight()) {
    print(root->getRight());
  }
}
void NfaStructure::print() {
  setWasIncreased(_start);
  std::cout << "STARTING NODE ";
  print(_start);
  std::cout << "FINAL NODE id: " << _final->getId() << std::endl;
}
void NfaStructure::increaseAllIds(const size_t& num) {
  setWasIncreased(_start);
  increaseIds(_start, num);
}
void NfaNode::setNode(const NfaNode& t) {
  if (this != &t) {
    _left = t._left;
    _right = t._right;
    _eps = t._eps;
    _symbol = t._symbol;
  }
}
