#include "reg_exp.h"

Expression::Expression(char value) : _type(NodeType::Value), _value(value) {}
const std::shared_ptr<Expression>& Expression::getLeft() { return _left; }
const std::shared_ptr<Expression>& Expression::getRight() { return _right; }
void Expression::starRightSide() { _right = std::make_shared<Expression>(NodeType::Star, std::move(_right)); }
void Expression::printTree(const std::string& prefix, bool is_right) {
  std::cout << prefix;
  std::cout << (is_right ? "|--" : "L--");
  switch (_type) {
    case NodeType::Value: {
      std::cout << _value << std::endl;
      break;
    }
    case NodeType::Add: {
      std::cout << '+' << std::endl;
      _left->printTree(prefix + (is_right ? "|   " : "    "), true);
      _right->printTree(prefix + (is_right ? "|   " : "    "), false);
      break;
    }
    case NodeType::Star: {
      std::cout << '*' << std::endl;
      _left->printTree(prefix + (is_right ? "|   " : "    "), false);
      break;
    }
    case NodeType::Brackets: {
      std::cout << "()" << std::endl;
      _left->printTree(prefix + (is_right ? "|   " : "    "), false);
      break;
    }
    case NodeType::Or: {
      std::cout << '|' << std::endl;
      _left->printTree(prefix + (is_right ? "|   " : "    "), true);
      _right->printTree(prefix + (is_right ? "|   " : "    "), false);
      break;
    }
    default: {
      std::cout << std::endl << "error while printing tree, unsupported node type" << std::endl;
      return;
    }
  }
}

ErrOr<std::pair<std::string, SPExpression>> RegExpParser::parseExpression(std::string expression) {
  std::shared_ptr<Expression> curr;
  while (!expression.empty()) {
    switch (expression.at(0)) {
      case '*': {
        if (curr == nullptr) return ERROR_WITH_FILE("KLEENE CLOSURE called without anything before");
        switch (curr->getType()) {
          case NodeType::Star: {
            return ERROR_WITH_FILE("KLEENE CLOSURE operators cannot be nested");
          }
          case NodeType::Brackets:
          case NodeType::Value: {
            curr = std::make_shared<Expression>(NodeType::Star, std::move(curr));
            break;
          }
          case NodeType::Add:
          case NodeType::Or: {
            curr->starRightSide();
            break;
          }
          default: {
            return ERROR_WITH_FILE("unknown node type in kleene closure, error");
          }
        }
        break;
      }
      case '(': {
        expression.erase(0, 1);
        _open_bracets++;
        auto ret = parseExpression(expression);
        if (ret.err) return *ret.err;
        expression = ret.data.value().first;
        if (expression.empty() || expression.at(0) != ')') {
          return ERROR_WITH_FILE("bracket not closed");
        }
        auto temp = std::make_shared<Expression>(NodeType::Brackets, std::move(ret.data.value().second));
        curr == nullptr ? curr = std::move(temp)
                        : curr = std::make_shared<Expression>(NodeType::Add, std::move(curr), std::move(temp));

        break;
      }
      case ')': {
        if (!_open_bracets--) return ERROR_WITH_FILE("closing bracket doesn't have an opening bracked");
        return std::make_pair(expression, std::move(curr));
      }
      case '|': {
        if (curr == nullptr) return ERROR_WITH_FILE("OR called without anything before");
        expression.erase(0, 1);
        auto ret = parseExpression(expression);
        if (ret.err) return *ret.err;
        expression = ret.data.value().first;
        if (!expression.empty() && expression.at(0) != ')') {
          return ERROR_WITH_FILE("OR expression not parsed correctly");
        }

        auto rhs = std::move(ret.data.value().second);
        if (rhs == nullptr) {
          return ERROR_WITH_FILE("OR expression rhs is empty");
        }
        curr = std::make_shared<Expression>(NodeType::Or, std::move(curr), std::move(rhs));
        return std::make_pair(expression, std::move(curr));
      }
      default: {
        auto temp = std::make_shared<Expression>(expression.at(0));
        curr == nullptr ? curr = std::move(temp)
                        : curr = std::make_shared<Expression>(NodeType::Add, std::move(curr), std::move(temp));
        break;
      }
    }
    expression.erase(0, 1);
  }
  return std::make_pair(expression, std::move(curr));
}
