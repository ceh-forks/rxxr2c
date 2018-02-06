#include "nfa.hpp"
#include "baselib.hpp"

struct inode {
  char c1;
  char c2;
  struct inttree *t;
  struct inode *n1;
  struct inode *n2;
};

struct inode *makeInode(char c1, char c2, struct inttree *t, struct inode *n1, struct inode *n2) {
  struct inode *r = new struct inode;
  r->c1 = c1;
  r->c2 = c2;
  r->t = t;
  r->n1 = n1;
  r->n2 = n2;
  return r;
}

struct inode *itr_add(inode *tr, char _u, char _v, struct inttree *_s) {
  if(tr == NULL) {
    return makeInode(_u, _v, _s, NULL, NULL);
  }
  else {
    if(_v < tr->c1) {
      return makeInode(tr->c1, tr->c2, tr->t, itr_add(tr->n1, _u, _v, _s), tr->n2);
    }
    else if(tr->c2 < _u) {
      return makeInode(tr->c1, tr->c2, tr->t, tr->n1, itr_add(tr->n2, _u, _v, _s));
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
        _rtr = itr_add(tr->n2, znex(_v), tr->c2, tr->t);
      else
        _rtr = itr_add(tr->n2, znext(tr->c2), _v, _s);
      return makeInode(max(tr->c1, _u), min(tr->c2, _v), intset_union(_s, tr->t), _ltr, _rtr);
    }
  }
}
