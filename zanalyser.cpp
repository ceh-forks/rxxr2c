#include "baselib.hpp"
#include "phi.hpp"
#include "flags.hpp"

struct zanalyser_struct {
  struct nfa *nfa;
  word *w;
  struct llist<struct llist<int> *> *cache;
  struct llist<phi_w_prefix *> *evolve;
  struct llist<phi_w_prefix *> *advance;
  int flgs;
};

struct zanalyser_struct *zanalyser_init(struct nfa *nfa, word *w, struct llist<int> *p) {
  struct zanalyser_struct *r = new zanalyser_struct;
  r->nfa = nfa;
  r->w = w;
  r->cache = NULL;
  r->evolve = addListNode<phi_w_prefix *>(makeTwople<word *, struct llist<int> *>(w, p), NULL);
  r->advance = NULL;
  r->flgs = EMPTY;
  return r;
}

phi_w_prefix *zanalyser_next(struct zanalyser_struct *m) {
  if (m->evolve != NULL) {
    struct llist<phi_w_prefix *> *t = m->evolve;
    m->evolve = t->tail;
    struct phi_evolve_struct *phe = phi_evolve(m->nfa, t->head->a, t->head->b, 0);
    if (is_accepting(phe->flgs)) {
      //we cannot advance this phi - prefix matching takes over
      deleteList<int>(phe->lst);
      delete phe;
      //TODO: Possibly delete head->?
      delete t->head;
      delete t;
      return zanalyser_next(m);
    }
    else if (is_eoihit(phe->flgs)) {
      //we may be able to get rid of the EOI/END with a character
      m->advance = addListNode<phi_w_prefix *>(makeTwople<word *, struct llist<int> *>(t->head->a, phe->lst), m->evolve);
      delete phe;
      delete t->head;
      delete t;
      return zanalyser_next(m);
    }
    else {
      phi_w_prefix *r = makeTwople<word *, struct llist<int> *>(word_suffix(t->head->a, listLength<crange *>(m->w)), phe->lst);
      delete phe;
      delete t->head;
      delete t;
      return r;
    }
  }
  else if (m->advance != NULL) {
    struct llist<phi_w_prefix *> *t = m->advance;
    m->advance = t->tail;
    if (!phiset_mem(t->head->b, m->cache)) {
      //never seen before phi, need to be explored
      m->cache = phiset_add(t->head->b, m->cache);
      struct twople <struct llist<phi_w_prefix *> *, word *> *phe = phi_explore(m->nfa, t->head->a, t->head->b);
      //schedule the resulting phis to be checked for non-acceptance
      m->evolve = phe->a;
      //check if there are no-match characters
      if (phe->b == NULL) {
        delete phe;
        delete t->head;
        delete t;
        return zanalyser_next(m);
      }
      else {
        phi_w_prefix *r = makeTwople<word *, struct llist<int> *>(word_suffix(word_extend(t->head->a, phe->b->head), listLength<crange *>(m->w)), NULL);
        delete phe;
        delete t->head;
        delete t;
        return r;
      }
    }
    else {
      delete t->head;
      delete t;
      return zanalyser_next(m);
    }
  }
  else
    return zanalyser_next(m);
}
