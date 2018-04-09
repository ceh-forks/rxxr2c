#include "baselib.hpp"
#include "triple.hpp"
#include "phi.hpp"
#include "flags.hpp"
#include "util.hpp"

struct y2analyser_struct {
  struct nfa *nfa;
  int ik;
  word *w;
  struct llist<struct triple *> *cache;
  struct llist<struct twople<word *, struct triple *> *> *evolve;
  struct llist<struct twople<word *, struct triple *> *> *advance;
  int flgs;
};

struct y2analyser_struct *y2analyser_init(struct nfa *nfa, word *w, struct triple *tpl, int ik) {
  struct y2analyser_struct *r = new y2analyser_struct;
  r->nfa = nfa;
  r->ik = ik;
  r->w = w;
  r->cache = NULL;
  r->evolve = NULL;
  //start with advancing the given triple
  //y2 can't be empty since it also includes the first branching character ('a' in y1ay2)
  r->advance = addListNode<struct twople<word *, struct triple *> *>(makeTwople<word *, struct triple *>(w, tpl), NULL);
  r->flgs = EMPTY;
}

struct twople<word *, struct llist<int> *> *y2analyser_next(y2analyser_struct *m) {
  if(m->evolve != NULL) {
    struct llist<struct twople<word *, struct triple *> *> *t = m->evolve;
    m->evolve = t->tail;
    struct triple *tpl = t->head->b;
    //evolve the phi component
    struct phi_evolve_struct *phe = phi_evolve(m->nfa, t->head->a, tpl->phi, 0);
    struct triple *etpl = triple_make(tpl->i1, tpl->i2, phe->lst);
    m->flgs = uni(phe->flgs, m->flgs);
    //ignore any paths that also lead to acceptance
    if (!is_accepting(phe->flgs) || !tripleset_mem(etpl, m->cache)) {
      //never seen before triple, check for convergence
      m->cache = tripleset_add(etpl, m->cache);
      if (is_epsilon_reachable(m->nfa, t->head->a, tpl->i1, m->ik) && is_epsilon_reachable(m->nfa, t->head->a, tpl->i2, m->ik)) {
        //Converges, return
        struct twople<word *, struct llist<int> *> *r = makeTwople<word *, struct llist<int> *>(word_suffix(t->head->a, listLength<crange *>(m->w)), phe->lst);
        delete phe;
        delete t->head;
        delete t;
        return r;
      }
      else {
        m->advance = addListNode<struct twople<word *, struct triple *> *>(makeTwople<word *, struct triple *>(t->head->a, etpl), m->advance);
        delete phe;
        delete t->head;
        delete t;
        return y2analyser_next(m);
      }
    }
    else {
      delete phe;
      delete t->head;
      delete t;
      return y2analyser_next(m);
    }
  }
  else if (m->advance != NULL) {
    struct llist<struct twople<word *, struct triple *> *> *t = m->advance;
    m->advance = t->tail;
    m->evolve = triple_advance(m->nfa, t->head->a, t->head->b);
    delete t->head;
    delete t;
    return y2analyser_next(m);
  }
  else
    return NULL;
}
