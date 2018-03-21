#include "baselib.hpp"
#include "beta.hpp"
#include "phi.hpp"
#include "word.hpp"
#include "flags.hpp"

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

struct xanalyserstruct *xanalyser_init(struct nfa *nfa, struct llist<int> *kset) {
  struct xanalyserstruct *r = new xanalyserstruct;
  r->nfa = nfa;
  r->kset = kset;
  r->w = NULL;
  r->bcache = NULL;
  r->pcache = NULL;
  //Start evolving the root state
  r->hits = NULL;
  r->evolve = addListNode<struct twople<word *, struct llist<int> *> *>(makeTwople<word *, struct llist<int> *>(NULL, beta_make(nfa->root)), NULL);
}

struct xanalyser_next_struct {
  int i;
  word *w;
  struct llist<int> *phi;
};

struct xanalyser_next_struct *nextmake(int i, word *w, struct llist<int> *phi) {
  struct xanalyser_next_struct *r = new xanalyser_next_struct;
  r->i = i;
  r->w = w;
  r->phi = phi;
}

struct xanalyser_next_struct *xanalyser_next(struct xanalyserstruct *m) {
  if (m->hits != NULL) {
    struct llist<struct twople<int, struct llist<int> *> *> *t = m->hits;
    m->hits = t->tail;
    struct llist<int> *p = beta_elems(t->head->b);
    phi_evolve_struct *ep = phi_evolve(m->nfa, m->w, p, NULL);
    deleteList<int>(p);
    struct llist<struct llist<int> *> *iter = m->pcache;
    while(iter) {
      if (!listSame<int>(ep->lst, iter->head)) {
        //Never seen before phi, return
        m->pcache = phiset_add(ep->lst, m->pcache);
        struct xanalyser_next_struct *r = nextmake(m->hits->head->a, m->w, ep->lst);
        delete ep;
        deleteList<int>(t->head->b);
        delete t->head;
        delete t;
        return r;
      }
      else
        iter = iter->tail;
    }
    delete ep;
    deleteList<int>(t->head->b);
    delete t->head;
    delete t;
    return xanalyser_next(m);
  }
  else if (m->evolve != NULL) {
    struct llist<struct twople<word *, struct llist<int> *> *> *t = m->evolve;
    m->evolve = t->tail;
    word *tw = m->w;
    deleteList<crange *>(tw); //TODO: If there's double frees / mem errors, here it is
    m->w = t->head->a;
    //Look for hits
    struct rec_evolve_struct *b = beta_evolve(m->nfa, t->head->a, t->head->b, m->kset);
    m->hits = b->hits;
    //New beta to be advanced
    m->advance = addListNode<struct twople<word *, struct llist<int> *> *>(t->head, m->advance);
    m->flgs = uni(b->flgs, m->flgs);
    deleteList<int>(b->eb);
    delete b;
    delete t->head;
    delete t;
    return xanalyser_next(m);
  }
  else if (m->advance != NULL) {
    struct llist<struct twople<word *, struct llist<int> *> *> *t = m->advance;
    m->advance = t->tail;
    struct llist<struct llist<int> *> *iter = m->bcache;
    while(iter) {
      if (!listSame<int>(t->head->b, iter->head)) {
        //Never seen before beta, advance
        m->pcache = phiset_add(t->head->b, m->pcache); //betaset works same as phi
        m->evolve = beta_advance(m->nfa, t->head->a, t->head->b);
        delete t;
        return xanalyser_next(m);
      }
      else
        iter = iter->tail;
    }
    delete t->head;
    delete t;
    return xanalyser_next(m);
  }
  else
    return NULL;
}
