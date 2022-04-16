#include <chrono>
#include <iostream>
#include <memory>
#include <optional>
using namespace std;

enum class NodeType {
  Value,
  Add,
  Star,
  Or,
  Brackets,
};

class Expression {
  NodeType _type;
  shared_ptr<Expression> _left;
  shared_ptr<Expression> _right;
  char _value;

 public:
  // constructor for concat and or
  Expression(NodeType type, shared_ptr<Expression> left, shared_ptr<Expression> right)
      : _type(type), _left(std::move(left)), _right(std::move(right)) {}

  // constructor for star
  Expression(NodeType type, shared_ptr<Expression> left) : _type(type), _left(std::move(left)) {}
  // constructor for value
  Expression(char value) : _type(NodeType::Value), _value(std::move(value)) {}

  const Expression getLeft() { return *_left.get(); }
  const Expression getRight() { return *_right.get(); }
  NodeType getType() { return _type; }
  void starRightSide() { _right = std::make_shared<Expression>(NodeType::Star, std::move(_right)); }
  void printTree(const std::string& prefix = "", bool is_right = false) {
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
};

typedef shared_ptr<Expression> SPExpression;

struct Error {
  std::string msg;

  Error() = default;

  Error(std::string msg) : msg(std::move(msg)) {}
};

typedef std::optional<Error> OErr;

template <typename T>
struct ErrOr {
  OErr err;
  std::optional<T> data;

  ErrOr() = default;
  ErrOr(const Error& err) : err(err) {}
  ErrOr(const T& data) : data(data) {}
  ErrOr(T&& data) : data(std::move(data)) {}
};

#define ERROR_WITH_FILE(msg) Error(msg + " at "s + __FILE__ + ":" + std::to_string(__LINE__))

class RegExpParser {
 public:
  RegExpParser() {}
  ErrOr<std::pair<std::string, SPExpression>> parseExpression(std::string expression) {
    shared_ptr<Expression> curr;
    while (expression.size()) {
      switch (expression.at(0)) {
        case '*': {
          if (curr == nullptr) return ERROR_WITH_FILE("star called without anything before");
          switch (curr->getType()) {
            case NodeType::Star: {
              return ERROR_WITH_FILE("star operators cannot be nested");
            }
            case NodeType::Brackets:
            case NodeType::Value: {
              curr = make_shared<Expression>(NodeType::Star, std::move(curr));
              break;
            }
            case NodeType::Add:
            case NodeType::Or: {
              curr->starRightSide();
              break;
            }
            default: {
              return ERROR_WITH_FILE("unknown node type, error");
            }
          }
          break;
        }
        case '(': {
          expression.erase(0, 1);
          auto ret = parseExpression(expression);
          if (ret.err) return *ret.err;
          expression = ret.data.value().first;
          if (expression.size() == 0 || expression.at(0) != ')') {
            return ERROR_WITH_FILE("bracket not closed");
          }
          auto temp = std::make_shared<Expression>(NodeType::Brackets, std::move(ret.data.value().second));
          curr == nullptr ? curr = std::move(temp)
                          : curr = std::make_shared<Expression>(NodeType::Add, std::move(curr), std::move(temp));

          break;
        }
        case ')': {
          return std::make_pair(expression, std::move(curr));
        }
        case '|': {
          expression.erase(0, 1);
          auto ret = parseExpression(expression);
          if (ret.err) return *ret.err;
          expression = ret.data.value().first;
          if (expression.size() != 0 && expression.at(0) != ')') {
            return ERROR_WITH_FILE("or expression not parsed correctly");
          }

          auto rhs = std::move(ret.data.value().second);
          if (rhs == nullptr) {
            return ERROR_WITH_FILE("or expression rhs is empty");
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
};

int main(int argc, char** argv) {
  if (argc != 2) return 0;

  string expression = argv[1];
  RegExpParser parser;
  auto ret = parser.parseExpression(expression);
  if (ret.err) {
    cout << "error :" << ret.err.value().msg << endl;
  } else {
    cout << "no errors\n";
    ret.data.value().second->printTree();
  }

  return 1;
}
