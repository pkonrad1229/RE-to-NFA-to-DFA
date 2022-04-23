#pragma once

#include <memory>

#include "errors.h"

enum class NodeType {
  Value,
  Add,
  Star,
  Or,
  Brackets,
};

class Expression {
  NodeType _type;
  std::shared_ptr<Expression> _left;
  std::shared_ptr<Expression> _right;
  char _value{};

 public:
  // constructor for concat and or
  Expression(NodeType type, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
      : _type(type), _left(std::move(left)), _right(std::move(right)) {}

  // constructor for star
  Expression(NodeType type, std::shared_ptr<Expression> left) : _type(type), _left(std::move(left)) {}
  // constructor for value
  explicit Expression(char value);

  const std::shared_ptr<Expression>& getLeft();
  const std::shared_ptr<Expression>& getRight();
  NodeType getType() { return _type; }
  [[nodiscard]] const char& getValue() const { return _value; }
  void starRightSide();
  void printTree(const std::string& prefix = "", bool is_right = false);
};

typedef std::shared_ptr<Expression> SPExpression;

class RegExpParser {
  size_t _open_bracets{0};

 public:
  ErrOr<std::pair<std::string, SPExpression>> parseExpression(std::string expression);
};
