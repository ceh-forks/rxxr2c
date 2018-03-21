#include "baselib.hpp"
#include "phi.hpp"
#include "product.hpp"
#include "triple.hpp"
#include "flags.hpp"

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

struct twople<word *, struct triple *> *y1analyser_next(struct y1analyser_struct *m) {
  if (m->tpls != NULL) {
    struct llist<struct twople<word *, struct triple *> *> *t = m->tpls;
    m->tpls = t->tail;
    if (!tripleset_mem(t->head->b, m->tcache)) {
      //never seen before triple, return
      m->tcache = tripleset_add(t->head->b, m->tcache);
      struct twople<word *, struct triple *> *r = makeTwople<word *, struct triple *>(word_suffix(t->head->a, listLength<crange *>(m->w)), t->head->b);
      delete t->head;
      delete t;
      return r;
    }
    else {
      delete t->head;
      delete t;
      return y1analyser_next(m);
    }
  }
  else if (m->evolve != NULL) {
    struct llist<struct twople<word *, struct product *> *> *t = m->evolve;
    m->evolve = t->tail;
    //evolve product looking for triples
    struct product_evolve_struct *ev = product_evolve(m->nfa, t->head->a, t->head->b, m->brset);
    //ignore any paths that also lead to acceptance
    if (!is_accepting(ev->flags)) {
      m->flgs = uni(ev->flags, m->flgs);
      struct llist<struct triple *> *iter = ev->triple;
      struct llist<struct twople<word *, struct triple *> *> *build = new struct llist<struct twople<word *, struct triple *> *>;
      build->head = makeTwople<word *, struct triple *>(t->head->a, iter->head);
      struct llist<struct twople<word *, struct triple *> *> *buildIter = build;
      while(iter) {
        buildIter->tail = addListNode<struct twople<word *, struct triple *> *>(makeTwople<word *, struct triple *>(t->head->a, iter->head), NULL);
        iter = iter->tail;
        buildIter = buildIter->tail;
      }
      buildIter->tail = NULL;
      m->tpls = build;
      //Resulting product needs to be further advanced
      m->advance = addListNode<struct twople<word *, struct product *> *>(makeTwople<word *, struct product *>(t->head->a, ev->product), m->advance);
    }
    delete t->head;
    delete t;
    return y1analyser_next(m);
  }
  else if (m->advance != NULL) {
    struct llist<struct twople<word *, struct product *> *> *t = m->advance;
    m->advance = t->tail;
    if(!productset_mem(t->head->b, m->pcache)) {
      //never see before product, advance
      m->pcache = productset_add(t->head->b, m->pcache);
      m->evolve = product_advance(m->nfa, t->head->a, t->head->b);
    }
    delete t->head;
    delete t;
    return y1analyser_next(m);
  }
  else
    return NULL;
}
