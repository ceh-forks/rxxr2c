#include "beta.hpp"

struct phi_evolve_struct {
  int flgs;
  struct llist<int> *lst;
};

struct phi_evolve_struct *phi_evolve(struct nfa *nfa, word *w, struct llist<int> *p, int iopt);
