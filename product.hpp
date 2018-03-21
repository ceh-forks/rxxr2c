#include "baselib.hpp"

struct product {
  int i;
  struct llist<int> *phi; //Sorted Int set
};

struct product *product_make(int i, struct llist<int> *phi);
int productset_compare(struct product *p1, struct product *p2);
struct llist<struct product *> *productset_add(struct product *p, struct llist<struct product *> *l);
bool productset_mem(struct product *, struct llist<struct product *> *);

struct llist<struct twople<word *, struct product *> *> *product_advance(struct nfa *nfa, word *w, struct product *p);

struct product_evolve_struct {
  int flags;
  struct product *product;
  struct llist<struct triple *> *triple;
};

struct product_evolve_struct *product_evolve(struct nfa *nfa, word *w, struct product *p, struct llist<int> *brset);
