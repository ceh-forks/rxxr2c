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
};

struct inode *itr_add(struct inode *tr, char _u, char _v, struct llist<int> *_s) {
  if(tr == NULL) {
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
  struct phi_w_prefix *phi = new phi_w_prefix;
  phi->w = word_extend(w, tr->c1, tr->c2);
  phi->p = tr->s;
  struct llist<struct phi_w_prefix *> *philist = addListNode<struct phi_w_prefix *>(phi, lst)
  return itr_collect(tr->n2, w, itr_collect(tr->n2, w, philist));
}
