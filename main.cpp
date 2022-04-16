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

  const std::shared_ptr<Expression>& getLeft() { return _left; }
  const std::shared_ptr<Expression>& getRight() { return _right; }
  NodeType getType() { return _type; }
  const char& getValue() { return _value; }
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

// enum class NfaNodeType {

// };

class NfaNode {
  std::shared_ptr<NfaNode> _left;
  std::shared_ptr<NfaNode> _right;
  bool _eps;
  char _symbol;

 public:
  NfaNode(std::shared_ptr<NfaNode> left, const char& symbol) : _left(std::move(left)), _symbol(symbol), _eps(false) {}
  NfaNode(std::shared_ptr<NfaNode> left) : _left(std::move(left)), _eps(true) {}
  NfaNode(std::shared_ptr<NfaNode> left, std::shared_ptr<NfaNode> right)
      : _left(std::move(left)), _right(std::move(right)), _eps(true) {}
  NfaNode() : _eps(false) {}

  const std::shared_ptr<NfaNode>& getLeft() { return _left; }
  const std::shared_ptr<NfaNode>& getRight() { return _right; }
  bool isEpsilon() { return _eps; }
  const char getSymbol() { return _symbol; }

  void setLeft(const std::shared_ptr<NfaNode>& node) { _left = node; }
  void setRight(const std::shared_ptr<NfaNode>& node) { _right = node; }
  void setEpsilon(bool eps) { _eps = eps; }

  void setNode(const NfaNode& t) {
    if (this != &t) {
      _left = t._left;
      _right = t._right;
      _eps = t._eps;
      _symbol = t._symbol;
    }
  }

  // NfaNode& operator=(const NfaNode& t) {
  //   if (this != &t) {
  //     _left = t._left;
  //     _right = t._right;
  //     _eps = t._eps;
  //     _symbol = t._symbol;
  //   }
  //   return *this;
  // }
};

typedef std::shared_ptr<NfaNode> SPNfaNode;

class NfaStructure {
  SPNfaNode _start;
  SPNfaNode _final;

 public:
  NfaStructure() {}
  const SPNfaNode& getStart() { return _start; }
  const SPNfaNode& getFinal() { return _final; }
  bool isFinal(const SPNfaNode& node) { return _final == node; }
  void setStart(const SPNfaNode& node) { _start = node; }
  void setFinal(const SPNfaNode& node) { _final = node; }
};

class TransformReToNfa {
 public:
  TransformReToNfa() {}
  /**
   * Function that recursivly transforms a tree
   * @param expr proto message which specifies the type of the writer, and provides needed data
   * @return a unique pointer to table writer, or error
   */
  ErrOr<NfaStructure> transform(const SPExpression& expr) {
    if (expr->getType() == NodeType::Value) {
      NfaStructure nfa;
      auto temp = std::make_shared<NfaNode>(std::make_shared<NfaNode>(), expr->getValue());
      nfa.setStart(temp);
      nfa.setFinal(temp->getLeft());
      return std::move(nfa);
    }
    auto ret = transform(expr->getLeft());
    if (ret.err) return *ret.err;
    auto nfa = *ret.data;
    switch (expr->getType()) {
      case NodeType::Add: {
        ret = transform(expr->getRight());
        if (ret.err) return *ret.err;
        auto rhs = *ret.data;
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

        nfa.getFinal()->setEpsilon(true);
        nfa.getFinal()->setLeft(nfa.getStart());
        nfa.getFinal()->setRight(final);

        nfa.setStart(start);
        nfa.setFinal(final);

        return std::move(nfa);
      }
      case ::NodeType::Or: {
        ret = transform(expr->getRight());
        if (ret.err) return *ret.err;
        auto rhs = *ret.data;

        auto start = std::make_shared<NfaNode>(nfa.getStart(), rhs.getStart());
        auto final = std::make_shared<NfaNode>();

        const auto& left_final = nfa.getFinal();
        left_final->setEpsilon(true);
        left_final->setLeft(final);

        const auto& right_final = rhs.getFinal();
        right_final->setEpsilon(true);
        right_final->setLeft(final);

        nfa.setStart(start);
        nfa.setFinal(final);

        return std::move(nfa);
      }
      default: {
        return ERROR_WITH_FILE("unknown node type");
      }
    }
  }
};

int main(int argc, char** argv) {
  if (argc != 2) return 0;

  string expression = argv[1];
  RegExpParser parser;
  auto ret = parser.parseExpression(expression);
  if (ret.err) {
    cout << "error :" << ret.err.value().msg << endl;
    return 0;
  }
  cout << "no errors\n";
  ret.data.value().second->printTree();
  TransformReToNfa transform;
  auto nfa = transform.transform(ret.data.value().second);
  if (nfa.err) {
    cout << "error :" << nfa.err.value().msg << endl;
    return 0;
  }
  cout << "no error in nfa\n";
  auto tree = *nfa.data;

  return 1;
}
