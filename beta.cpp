#include "parsingdata.hpp"
#include <iostream>

struct llist<int> *make(int i) {
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

struct btree<int> *elems(struct llist<int> *l) {
  struct btree<int> *t = NULL;
  while(l) {
    btins(t, l->head);
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
    tr->node->l = llappend<int>(_l, tr->node->l);
    tr->lt = _ltr;
    tr->rt = _rtr;
    return tr;
  }
}

otr_collect(struct btree<struct beta> *tr, w, lst) {
  if (tr == NULL)
    return lst;
  else {
    

int main(void) {
  std::cout << "hello\n";
  return 0;
}

