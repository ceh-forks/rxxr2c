#include "nfa.h"

struct inode {
  char c1;
  char c2;
  struct inttree *t;
  struct inode *n1;
  struct inode *n2;
};

struct inode *itr_add(inode *tr, char _u, char _v, struct inttree *_s) {
  if(tr == NULL) {
    struct inode *r = new struct inode;
    r->c1 = _u;
    r->c2 = _v;
    r->t = _s;
    t->n1 = NULL;
    t->n2 = NULL;
  }
  else {
    if(_v < tr->c1) {
      struct inode *r = new struct inode;
      r->c1 = tr->c1;
      r->c2 = tr->c2;
      r->t = tr->t;
      r->n1 = itr_add(tr->n1, _u, _v, _s);
      r->n2 = tr->n2;
      return r;
    }
    else if(tr->c2 < _u) {
      struct inode *r = new struct inode;
      r->c1 = tr->c1;
      r->c2 = tr->c2;
      r->t = tr->t;
      r->n1 = tr->n1;
      r->n2 = itr_add(tr->n2, _u, _v, _s);
      return r;
    }
    else {
      struct inode *_ltr;
      if(tr->c1 == _u)
        _ltr = tr->n1;
      else if(tr->c1 < _u) {
        
