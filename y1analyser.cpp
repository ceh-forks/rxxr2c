#include "baselib.hpp"
#include "phi.hpp"
#include "product.hpp"
#include "triple.hpp"

struct y1analyser_struct {
  struct nfa *nfa;
  int ik;
  struct llist<int> *brset;
  word *w;
  struct llist<struct product *> *pcache;
}
