#pragma once
#include <set>
#include <vector>

#include "errors.h"
#include "nfa.h"
class DfaState {
  std::set<SPNfaNode> _nodes;  // set of all NFA nodes that this state is made of
  std::vector<std::pair<char, std::set<size_t>>>
      _possible_moves;    // vector containing all possible characters and a set of ids that the transition will move to
  bool _is_final{false};  // set to true if that state is a final state
  char _state_name{' '};  // character representing the name of the node

 public:
  DfaState() = default;

  [[nodiscard]] bool isFinal() const;
  void setFinal(bool val);
  [[nodiscard]] char getName() const;
  void setName(char val);

  /**
   * Function that returns a DFA state consisting of all the nodes, that this state can move to, using given symbol
   * @param symbol character representing the symbol of transition
   * @return DfaState state consisting of all the nodes, that this state can move to
   */
  DfaState move(const char& symbol);

  /**
   * Function that performs epsilonClosure for a DfaState
   * @return DfaState state representing epsilon closure of this state
   */
  DfaState epsilonClosure();

  /**
   * Function that shows all possible moves for this state
   * @return set of all characters, that are a possible move for NFA nodes of this state
   */
  std::set<char> possibleMoves();

  /**
   * Function that checks is this state contains given node
   * @param node shared pointer to a NFA node
   * @return true if this state contains given node
   */
  bool contains(const SPNfaNode& node);

  /**
   * Function that prints ids of NFA nodes of this state
   */
  void printIds() const;

  /**
   * Function that returns a set of all NFA nodes ids contained by this state
   * @return std::set<size_t> of NFA ids
   */
  [[nodiscard]] std::set<size_t> getIds() const;

  /**
   * Function that inserts a node to the state
   * @param node shared pointer to a NFA node
   * @return true if node was inserted, false if the node was already contained by the state
   */
  bool insert(const SPNfaNode& node);

  /**
   * Function that returns the _possible_moves member
   * @return _possible_moves
   */
  [[nodiscard]] std::vector<std::pair<char, std::set<size_t>>> getMoves() const;
};

typedef std::shared_ptr<DfaState> SPDfaState;

class DFA {
  SPDfaState _start;
  SPNfaNode _final_node;
  SPNfaNode _trap_state;
  std::set<SPDfaState> _all;
  std::set<char> _all_moves;
  char _current_name = 'A';

  DFA() = default;

  /**
   * Function that checks if given state is in the DFA
   * @param state DFA state
   * @return true if node was inserted, false if the node was already contained by the state
   */
  bool containsState(const DfaState& state);

  /**
   * Function that inserts a state to the DFA
   * @param state shared pointer to a DFA state
   */
  void insert(const SPDfaState& state);

  /**
   * Function that returns the name of the state
   * @param ids sts::set of NFA node ids
   * @return name of the state
   */
  char getStateName(const std::set<size_t>& ids);

 public:
  /**
   * Function that generates a DFA from NFA
   * @param nfa NfaStructure object
   * @return DFA
   */
  static DFA generateDfaFromNfa(const NfaStructure& nfa);

  /**
   * Function that generates a DFA from a string
   * @param expression string with the expression
   * @return DFA or error
   */
  static ErrOr<DFA> generateDfaFromRE(const std::string& expression);

  void print();
};
