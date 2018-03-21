#include "baselib.hpp"
#include "word.hpp"

//NFA state pair + phi
struct triple {
  int i1;
  int i2;
  struct llist<int> *phi;
};

struct triple *triple_make(int, int, struct llist<int> *);
struct llist<struct twople<word *, struct triple *> *> *triple_advance(struct nfa *, word *, struct triple *);
struct llist<struct triple *> *tripleset_add(struct triple *t, struct llist<struct triple *> *tl);
bool tripleset_mem(struct triple *, struct llist<struct triple *> *);
