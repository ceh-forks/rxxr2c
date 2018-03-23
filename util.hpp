#ifndef UTIL_H
#define UTIL_H

#include "word.hpp"
#include <boost/unordered_map.hpp>

typedef boost::unordered_map<int, struct llist<int> *> hashtbl_kleene;

bool is_epsilon_reachable(struct nfa *nfa, word *w, int src, int dst);
hashtbl_kleene *find_pumpable_kleene(struct nfa *nfa)

#endif
