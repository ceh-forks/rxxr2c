#include "baselib.hpp"
#include "word.hpp"

struct y1analyser_struct {
  struct nfa *nfa;
  int ik;
  struct llist<int> *brset;
  word *w;
  struct llist<struct product *> *pcache;
  struct llist<struct triple *> *tcache;
  struct llist<struct twople<word *, struct triple *> *> *tpls;
  struct llist<struct twople<word *, struct product *> *> *evolve;
  struct llist<struct twople<word *, struct product *> *> *advance;
  int flgs;
};

struct y1analyser_struct *y1analyser_init(struct nfa *, word *, struct llist<int> *, int, struct llist<int> *);
struct twople<word *, struct triple *> *y1analyser_next(struct y1analyser_struct *);
