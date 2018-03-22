#ifndef BETA_H
#define BETA_H

#include "baselib.hpp"
#include "word.hpp"

struct rec_evolve_struct {
  int flgs;
  struct llist<int> *eb;
  struct llist<struct twople<int, struct llist<int> *> *> *hits;
};

struct llist<int> *beta_make(int);
struct llist<int> *beta_elems(struct llist<int> *);
struct rec_evolve_struct *beta_evolve(struct nfa *nfa, word *w, struct llist<int> *b, struct llist<int> *kset);
struct llist<int> *betaset_add(int, struct llist<int> *);
struct llist<struct twople<word *, struct llist<int> *> *> *beta_advance(struct nfa *nfa, word *w, llist<int> *b);

#endif
