#include "set.hpp"
#include "nfa.hpp"
#include "word.hpp"
#include "phi.hpp"
#include "xanalyser.hpp"
#include "y1analyser.hpp"
#include "y2analyser.hpp"
#include "zanalyser.hpp"

bool is_saturated(struct nfa *nfa, word *y, struct llist<int> *p) {
  word *l = addListNode<crange *>(makePair<char>(ZMIN, ZMAX), NULL);
  struct llist<char> *pumpable = word_select(y, l);
  phi_simulate_struct *phisim = phi_simulate(nfa, y, p, pumpable);
  bool r = intset_subset(phisim->lst, p);
}

void enumerate_verbose(struct nfa *nfa) {
  
}
