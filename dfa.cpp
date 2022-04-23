#include "dfa.h"

DfaState DfaState::move(const char& symbol) {
  DfaState state;
  for (const auto& node : _nodes) {
    if (!node->isEpsilon() && node->getSymbol() == symbol) {
      state._nodes.insert(node->getLeft());
    }
  }

  _possible_moves.emplace_back(symbol, state.epsilonClosure().getIds());
  return state;
}
bool DFA::containsState(const DfaState& state) {
  for (const auto& it : _all) {
    if (it->getIds() == state.getIds()) return true;
  }
  return false;
}
bool DfaState::isFinal() const { return _is_final; }
void DfaState::setFinal(bool val) { _is_final = val; }
char DfaState::getName() const { return _state_name; }
void DfaState::setName(char val) { _state_name = val; }
DfaState DfaState::epsilonClosure() {
  DfaState state = *this;
  while (true) {
    DfaState temp = state;
    for (const auto& node : state._nodes) {
      if (node == nullptr) continue;

      if (node->isEpsilon()) {
        state._nodes.insert(node->getLeft());
        if (node->getRight()) state._nodes.insert(node->getRight());
      }
    }
    if (temp._nodes.size() == state._nodes.size()) break;
  }
  return state;
}
std::set<char> DfaState::possibleMoves() {
  std::set<char> moves;
  for (const auto& node : _nodes) {
    if (node == nullptr) continue;
    if (!node->isEpsilon() && node->getLeft() != nullptr) {
      moves.insert(node->getSymbol());
    }
  }
  return moves;
}
bool DfaState::contains(const SPNfaNode& node) {
  const auto& ret = _nodes.find(node);
  return ret != _nodes.end();
}
void DfaState::printIds() const {
  std::cout << "{";
  for (const auto& state : _nodes) {
    std::cout << " " << state->getId();
  }
  std::cout << " }";
}
std::set<size_t> DfaState::getIds() const {
  std::set<size_t> ids;
  for (const auto& state : _nodes) {
    ids.insert(state->getId());
  }
  return ids;
}
bool DfaState::insert(const SPNfaNode& node) { return _nodes.insert(node).second; }
std::vector<std::pair<char, std::set<size_t>>> DfaState::getMoves() const { return _possible_moves; }
void DFA::insert(const SPDfaState& state) {
  *state = state->epsilonClosure();
  if (containsState(*state)) return;
  _all.insert(state);
  state->setName(_current_name++);
  if (state->contains(_final_node)) {
    state->setFinal(true);
  }
  auto moves = state->possibleMoves();
  _all_moves.insert(moves.begin(), moves.end());
  for (const auto& symbol : moves) {
    auto move_state = state->move(symbol);
    if (!containsState(move_state)) {
      insert(std::make_shared<DfaState>(move_state));
    }
  }
}
char DFA::getStateName(const std::set<size_t>& ids) {
  for (const auto& state : _all) {
    if (ids == state->getIds()) return state->getName();
  }
  return ' ';
}
DFA DFA::generateDfaFromNfa(const NfaStructure& nfa) {
  DFA dfa;
  if (nfa.getStart() == nullptr) return dfa;
  dfa._final_node = nfa.getFinal();
  auto temp = std::make_shared<DfaState>();
  temp->insert(nfa.getStart());
  *temp = temp->epsilonClosure();
  dfa._start = std::move(temp);
  dfa.insert(dfa._start);
  return dfa;
}
ErrOr<DFA> DFA::generateDfaFromRE(const std::string& expression) {
  auto nfa = NfaStructure::generateNfaFromRE(expression);
  if (nfa.err) {
    std::cout << "NFA error :" << nfa.err.value().msg << std::endl;
    return *nfa.err;
  }
  return generateDfaFromNfa(nfa.data.value());
}
void DFA::print() {
  std::cout << "in the first row, all possible moves are printed\nin the first column all states are listed. State A "
               "is the starting state, and all states written with color \033[1;31mred\033[0m are the final states\n";
  bool trap_state = false;
  std::cout << "   |";
  for (const auto& val : _all_moves) {
    std::cout << " "
              << "\033[1;31m" << val << "\033[0m"
              << " |";
  }
  std::cout << std::endl;
  std::cout << "---|";
  for (const auto& val : _all_moves) {
    std::cout << "---|";
  }
  std::cout << std::endl;
  for (const auto& state : _all) {
    if (state->isFinal()) {
      std::cout << " "
                << "\033[1;31m" << state->getName() << "\033[0m"
                << " |";
    } else {
      std::cout << " " << state->getName() << " |";
    }

    for (const auto& val : _all_moves) {
      bool was_set = false;
      for (const auto& mv : state->getMoves()) {
        if (mv.first == val) {
          std::cout << " " << getStateName(mv.second) << " |";
          was_set = true;
          break;
        }
      }
      if (!was_set) {
        trap_state = true;
        std::cout << " " << _current_name << " |";
      }
    }
    std::cout << std::endl;
    std::cout << "---|";
    for (const auto& val : _all_moves) {
      std::cout << "---|";
    }
    std::cout << std::endl;
  }
  if (trap_state) {
    std::cout << " " << _current_name << " |";
    for (const auto& val : _all_moves) {
      std::cout << " " << _current_name << " |";
    }
    std::cout << std::endl;
    std::cout << "---|";
    for (const auto& val : _all_moves) {
      std::cout << "---|";
    }
    std::cout << std::endl;
  }
}
