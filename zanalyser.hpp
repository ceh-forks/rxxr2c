#include "baselib.hpp"
#include "word.hpp"
#include "phi.hpp"

struct zanalyser_struct {
  struct nfa *nfa;
  word *w;
  struct llist<struct llist<int> *> *cache;
  struct llist<phi_w_prefix *> *evolve;
  struct llist<phi_w_prefix *> *advance;
  int flgs;
};

struct zanalyser_struct *zanalyser_init(struct nfa *, word *, struct llist<int> *);
phi_w_prefix *zanalyser_next(struct zanalyser_struct *);
