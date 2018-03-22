#include "word.hpp"

struct y2analyser_struct {
  struct nfa *nfa;
  int ik;
  word *w;
  struct llist<struct triple *> *cache;
  struct llist<struct twople<word *, struct triple *> *> *evolve;
  struct llist<struct twople<word *, struct triple *> *> *advance;
  int flgs;
};

struct y2analyser_struct y2analyser_init(struct nfa *, word *, struct triple *, int);
struct twople<word *, struct llist<int> *> *y2analyser_next(y2analyser_struct *);
