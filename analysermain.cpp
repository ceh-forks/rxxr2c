#include "set.hpp"
#include "nfa.hpp"
#include "word.hpp"
#include "phi.hpp"
#include "util.hpp"
#include "xanalyser.hpp"
#include "y1analyser.hpp"
#include "y2analyser.hpp"
#include "zanalyser.hpp"
#include <iostream>

bool is_saturated(struct nfa *nfa, word *y, struct llist<int> *p) {
  word *l = addListNode<crange *>(makePair<char>(ZMIN, ZMAX), NULL);
  struct llist<char> *pumpable = word_select(y, l);
  phi_simulate_struct *phisim = phi_simulate(nfa, y, p, pumpable);
  bool r = intset_subset(phisim->lst, p);
}

void search_z(struct nfa *nfa, word *lead, struct llist<int> *y2p) {
  struct zanalyser_struct *zs = zanalyser_init(nfa, lead, y2p);
  while(zanalyser_next(zs) != NULL) {
    std::cout << "> Z : %s : %s\n"; //TODO
  }
}

void search_y2(struct nfa *nfa, int ik, word *lead, struct triple *y1_tpl, struct llist<int> *xp) {
  struct y2analyser_struct *y2s = y2analyser_init(nfa, lead, y1_tpl, ik);
  while(y2analyser_next(y2s) != NULL) {
    
  }
}

void enumerate_verbose(struct nfa *nfa) {
  hashtbl_kleene *ktbl = find_pumpable_kleene(nfa);
  struct llist<int> *kset;
  hashtbl_kleene::iterator iter = ktbl->begin();
  while(iter != ktbl->end()) {
    kset = intset_add(iter->first, kset);
    iter++;
  }

}
