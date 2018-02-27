#include "nfa.hpp"
#include "baselib.hpp"
#include "word.cpp"
#include "set.cpp"

struct phi_w_prefix { // Phis with prefixes
  word *w;
  llist<int> *p;
};

struct inode { // Tree of phis
  char c1;
  char c2;
  struct llist<int> *t;
  struct inode *n1;
  struct inode *n2;
};

struct inode *makeInode(char c1, char c2, struct llist<int> *t, struct inode *n1, struct inode *n2) {
  struct inode *r = new struct inode;
  r->c1 = c1;
  r->c2 = c2;
  r->t = t;
  r->n1 = n1;
  r->n2 = n2;
  return r;
}

struct inode *itr_add(struct inode *tr, char _u, char _v, struct llist<int> *_s) {
  if(tr == NULL)
    return makeInode(_u, _v, _s, NULL, NULL);

  if(_v < tr->c1) {
    tr->n1 = itr_add(tr->n1, _u, _v, _s);
    return tr;
  }
  else if(tr->c2 < _u) {
    tr->n2 = itr_add(tr->n2, _u, _v, _s);
    return tr;
  }
  else {
    struct inode *_ltr, *_rtr;
    if(tr->c1 == _u)
      _ltr = tr->n1;
    else if(tr->c1 < _u)
      _ltr = itr_add(tr->n1, tr->c1, zprev(_u), tr->t);
    else
      _ltr = itr_add(tr->n1, _u, zprev(tr->c1), _s);

    if(tr->c2 == _v)
      _rtr = tr->n2;
    else if(_v < tr->c2)
      _rtr = itr_add(tr->n2, znext(_v), tr->c2, tr->t);
    else
      _rtr = itr_add(tr->n2, znext(tr->c2), _v, _s);

    llist<int> *uni = intset_union(_s, tr->t);
    delete tr->t;
    tr->c1 = max(tr->c1, _u);
    tr->c2 = min(tr->c2, _v);
    tr->t = uni;
    tr->n1 = _ltr;
    tr->n2 = _rtr;
    return tr;
  }
}

struct llist<struct phi_w_prefix *> *itr_collect(struct inode *tr, word *w, struct llist<struct phi_w_prefix *> *lst) {
  if (tr == NULL)
    return lst;
  crange *c = new crange;
  c->a = tr->c1;
  c->b = tr->c2;
  struct phi_w_prefix *phi = new phi_w_prefix;
  phi->w = word_extend(w, c);
  phi->p = tr->t;
  struct llist<struct phi_w_prefix *> *philist = addListNode<struct phi_w_prefix *>(phi, lst);
  return itr_collect(tr->n2, w, itr_collect(tr->n2, w, philist));
}

struct nme {
  char next;
  word *lst;
};

struct nme *nomatch_explore(struct inode *tr, struct nme *nm) {
  if (tr == NULL)
    return nm;
  struct nme *right = nomatch_explore(tr->n2, nm);
  delete nm;
  struct nme *left = new struct nme;
  left->next = tr->c1;
  if (tr->c2 != zprev(right->next)) {
    crange *ct = new crange;
    ct->a = znext(tr->c2);
    ct->b = zprev(right->next);
    left->lst = addListNode<crange *>(ct, right->lst);
  }
  else
    left->lst = right->lst;
  delete right;
  return nomatch_explore(tr->n1, left);
}

word *itr_find_nomatch(struct inode *tr) {
  struct nme *nm = new struct nme;
  nm->next = '\x80';
  nm->lst = NULL;
  nm = nomatch_explore(tr, nm);
  if ('\x00' < nm->next) {
    crange *t = new crange;
    t->a = '\x00';
    t->b = zprev(nm->next);
    nm->lst = addListNode<crange *>(t, nm->lst);
  }
  word *lst = nm->lst;
  delete nm;
  return lst;
}

struct inode *explore_fold_left(struct inode *tr, struct llist<struct transition *> *trans) {
  if (trans == NULL)
    return tr;
  else
    return explore_fold_left(itr_add(tr, trans->head->a, trans->head->b, addListNode<int>(trans->head->c, NULL)), trans->tail);
}

struct inode *explore_intset_fold(struct llist<int> *i, inode *tr, struct nfa *nfa) {
  if (i == NULL)
    return tr;
  else {
    struct llist<struct transition *> *trans = get_transitions(nfa, i->head);
    struct inode *r = explore_intset_fold(i->tail, explore_fold_left(tr, trans), nfa);
    deleteListWithPointers<struct transition *>(trans);
    return r;
  }
}

struct twople <struct llist<struct phi_w_prefix *> *, word *> *explore(struct nfa *nfa, word *w, struct llist<int> *p) {
  struct inode *tr = explore_intset_fold(p, NULL, nfa);
  struct llist<struct phi_w_prefix *> *r1 = itr_collect(tr, w, NULL);
  word *r2 = itr_find_nomatch(tr);
  return makeTwople<struct llist<struct phi_w_prefix *> *, word *>(r1, r2);
}
