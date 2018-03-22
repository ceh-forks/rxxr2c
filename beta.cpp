#include "baselib.hpp"
#include "nfa.hpp"
#include "set.hpp"
#include "error.hpp"
#include "word.hpp"
#include "flags.hpp"

struct llist<int> *beta_make(int i) {
  struct llist<int> *r = new struct llist<int>;
  r->head = i;
  r->tail = NULL;
  return r;
}

struct beta {
  char c1;
  char c2;
  struct llist<int> *l;
};

struct beta *makeBeta(char c1, char c2, struct llist<int> *l) {
  struct beta *r = new struct beta;
  r->c1 = c1;
  r->c2 = c2;
  r->l = l;
}

void btins(struct btree<int> *t, int i) {
  if(t == NULL)
    t = makeTree<int>(i, NULL, NULL);
  else if(i < t->node) {
    if(t->lt != NULL)
      btins(t->lt, i);
    else
      t->lt = makeTree<int>(i, NULL, NULL);
  }
  else {
    if(t->rt != NULL)
      btins(t->rt, i);
    else {
      t->rt = makeTree<int>(i, NULL, NULL);
    }
  }
}

struct llist<int> *elems(struct llist<int> *l) {
  struct llist<int> *t = NULL;
  while(l) {
    intset_add(l->head, t);
    l = l->tail;
  }
  return t;
}

struct btree<struct beta *> *otr_add(struct btree<struct beta *> *tr, char _u, char _v, llist<int> *_l) {
  if (tr == NULL)
    return makeTree<struct beta *>(makeBeta(_u, _v, _l), NULL, NULL);
  else if (_u < tr->node->c1) {
    tr->lt = otr_add(tr->lt, _u, _v, _l);
    return tr;
  }
  else if (tr->node->c2 < _u) {
    tr->rt = otr_add(tr->rt, _u, _v, _l);
    return tr;
  }
  else {
    btree<struct beta *> *_ltr, *_rtr;
    if (_u == tr->node->c1)
      _ltr = tr->lt;
    else if (tr->node->c1 < _u)
      _ltr = otr_add(tr->lt, tr->node->c1, zprev(_u), tr->node->l);
    else
      _ltr = otr_add(tr->lt, _u, zprev(tr->node->c1), _l);
    
    if (_v == tr->node->c2)
      _rtr = tr->rt;
    else if (_v < tr->node->c2)
      _rtr = otr_add(tr->rt, znext(_v), tr->node->c2, tr->node->l);
    else
      _rtr = otr_add(tr->rt, znext(tr->node->c2), _v, _l);
    
    //Make new beta
    tr->node->c1 = max(tr->node->c1, _u);
    tr->node->c2 = min(tr->node->c2, _v);
    tr->node->l = listAppend<int>(_l, tr->node->l);
    tr->lt = _ltr;
    tr->rt = _rtr;
    return tr;
  }
}

struct pair<struct llist<int> *> *otr_collect_fold_func(int i, struct pair<struct llist<int> *> *p) {
  if (listMem(i, p->a) == 1)
    return p;
  else {
    p->a = intset_add(i, p->a);
    p->b = addListNode<int>(i, p->b);
    return p;
  }
}

struct pair<struct llist<int> *> *otr_collect_fold(struct llist<int> *l) {
  if(l == NULL)
    raise(NULL_ERROR);
  if(l->tail == NULL)
    return otr_collect_fold_func(l->head, makePair<struct llist<int> *>(NULL, NULL));
  else
    return otr_collect_fold_func(l->head, otr_collect_fold(l->tail));
}

struct llist<struct twople<word *, struct llist<int> *> *> *otr_collect(struct btree<struct beta *> *tr, word *w, struct llist<struct twople<word *, struct llist<int> *> *> *lst) {
  if (tr == NULL)
    return lst;
  struct pair<struct llist<int> *> *p = otr_collect_fold(tr->node->l);
  word *tw = word_extend(w, makePair<char>(tr->node->c1, tr->node->c2));
  struct llist<struct twople<word *, struct llist<int> *> *> *tlst = addListNode<struct twople<word *, struct llist<int> *> *>(makeTwople<word *, struct llist<int> *>(tw, p->b), lst);
  delete p->a;
  delete p;
  return otr_collect(tr->rt, w, otr_collect(tr->lt, w, tlst));
}

struct btree<struct beta *> *advance_fold_left(struct btree<struct beta *> *tr, struct llist<struct transition *> *l) {
  if (l == NULL)
    raise(NULL_ERROR);
  struct btree<struct beta *> *t = otr_add(tr, l->head->a, l->head->b, addListNode<int>(l->head->c, NULL));
  if (l->tail == NULL)
    return t;
  else
    return advance_fold_left(t, l->tail);
}

struct btree<struct beta *> *advance_fold_right(struct nfa *nfa, struct llist<int> *l, struct btree<struct beta *> *tr) {
  if(l == NULL)
    raise(NULL_ERROR);
  if(l->tail == NULL)
    return advance_fold_left(tr, get_transitions(nfa, l->head));
  else
    return advance_fold_right(nfa, l->tail, advance_fold_left(tr, get_transitions(nfa, l->head)));
}

struct llist<struct twople<word *, struct llist<int> *> *> *beta_advance(struct nfa *nfa, word *w, llist<int> *b) {
  return otr_collect(advance_fold_right(nfa, b, NULL), w, NULL);
}

struct rec_evolve_struct {
  int flgs;
  struct llist<int> *eb;
  struct llist<struct twople<int, struct llist<int> *> *> *hits;
};

struct evolve_struct {
  struct nfa *nfa;
  word *w;
  struct llist<int> *b;
  struct llist<int> *kset;
};

struct rec_evolve_struct *evolve_rec(struct llist<int> *rb, struct llist<int> *st, struct rec_evolve_struct *revst, struct evolve_struct *evst) {
  if (rb == NULL) {
    deleteList<int>(st);
    delete evst;
    return revst;
  }
  else if (listMem<int>(rb->head, st))
    return evolve_rec(rb->tail, st, revst, evst); //Already explored left-most occurence
  struct llist<int> *st2 = intset_add(rb->head, st);
  struct state *swt_state = get_state(evst->nfa, rb->head);
  switch(swt_state->type) {
    case End:
    case Kill: {
      revst->eb = addListNode<int>(rb->head, revst->eb);
      return evolve_rec(rb->tail, st, revst, evst);
    }
    case Pass:
    case MakeB:
    case EvalB: {
      return evolve_rec(addListNode<int>(swt_state->i, rb->tail), st, revst, evst);
    }
    case BeginCap:
    case EndCap: {
      return evolve_rec(addListNode<int>(swt_state->pi->b, rb->tail), st, revst, evst);
    }
    case Match: {
      revst->eb = addListNode<int>(rb->head, revst->eb);
      return evolve_rec(rb->tail, st, revst, evst);
    }
    case CheckPred: {
      if (swt_state->p->type == P_BOI) {
        if (evst->w == NULL)
          return evolve_rec(addListNode<int>(swt_state->i, rb->tail), st, revst, evst);
        else
          return evolve_rec(rb->tail, st, revst, evst);
      }
      else if (swt_state->p->type == P_BOL) {
        if (evst->w == NULL || (evst->w->head->a <= '\n' && '\n' <= evst->w->head->b) || (swt_state->p->value == 1 && evst->w->head->a <= '\r' && '\r' <= evst->w->head->b))
          return evolve_rec(addListNode<int>(swt_state->i, rb->tail), st, revst, evst);
        else
          return evolve_rec(rb->tail, st, revst, evst);
      }
      else if (swt_state->p->type == P_EOI) {
        revst->eb = addListNode<int>(rb->head, revst->eb);
        return evolve_rec(rb->tail, st, revst, evst);
      }
      else {
        revst->flgs = set_interrupted(revst->flgs);
        return evolve_rec(rb->tail, st, revst, evst);
      }
    }
    case CheckBackref: {
      revst->flgs = set_interrupted(revst->flgs);
      return evolve_rec(rb->tail, st, revst, evst);
    }
    case BranchAlt: {
      struct llist<int> *r = addListNode<int>(swt_state->pi->b, rb->tail);
      r = addListNode<int>(swt_state->pi->a, r);
      return evolve_rec(r, st, revst, evst);
    }
    case BranchKln: {
      struct llist<int> *r;
      if (listMem<int>(rb->head, evst->kset))
        revst->hits = addListNode<struct twople<int, struct llist<int> *> *>(makeTwople<int, struct llist<int> *>(rb->head, addListNode<int>(rb->head, listCpy<int>(revst->eb))), revst->hits);
      if (swt_state->klnBool == 1) {
        r = addListNode<int>(swt_state->pi->b, rb->tail);
        r = addListNode<int>(swt_state->pi->a, rb->tail);
      }
      else {
        r = addListNode<int>(swt_state->pi->a, rb->tail);
        r = addListNode<int>(swt_state->pi->b, rb->tail);
      }
      return evolve_rec(r, st, revst, evst);
    }
  }
}

struct rec_evolve_struct *beta_evolve(struct nfa *nfa, word *w, struct llist<int> *b, struct llist<int> *kset) {
  struct rec_evolve_struct *revst = new rec_evolve_struct;
  revst->flgs = EMPTY;
  revst->eb = NULL;
  revst->hits = NULL;
  struct evolve_struct *evst = new evolve_struct;
  evst->nfa = nfa;
  evst->w = w;
  evst->b = b;
  evst->kset = kset;
  return evolve_rec(listRev(b), NULL, revst, evst);
}
