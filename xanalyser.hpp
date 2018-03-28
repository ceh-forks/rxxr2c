#include "baselib.hpp"
#include "word.hpp"

struct xanalyserstruct {
  struct nfa *nfa;
  struct llist<int> *kset;
  word *w;
  struct llist<struct llist<int> *> *bcache;
  struct llist<struct llist<int> *> *pcache;
  struct llist<struct twople<int, struct llist<int> *> *> *hits;
  struct llist<struct twople<word *, struct llist<int> *> *> *evolve;
  struct llist<struct twople<word *, struct llist<int> *> *> *advance;
  int flgs;
};

struct xanalyserstruct *xanalyser_init(struct nfa *, struct llist<int> *);

struct xanalyser_next_struct {
  int i;
  word *w;
  struct llist<int> *phi;
};

struct xanalyser_next_struct *xanalyser_next(struct xanalyserstruct *);
