#ifndef PHI_H
#define PHI_H

typedef struct twople<word *, struct llist<int> *> phi_w_prefix; // Phis with prefixes

struct phi_evolve_struct {
  int flgs;
  struct llist<int> *lst;
};

struct phi_evolve_struct *phi_evolve(struct nfa *nfa, word *w, struct llist<int> *p, int iopt);
struct twople <struct llist<phi_w_prefix *> *, word *> *phi_explore(struct nfa *, word *, struct llist<int> *);

struct llist<struct llist<int> *> *phiset_add(struct llist<int> *l, struct llist<struct llist<int> *> *phiset);
bool phiset_mem(struct llist<int> *, struct llist<struct llist<int> *> *);

#endif
