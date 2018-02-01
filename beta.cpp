#include "parsingdata.h"
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

void otr_add(struct btree<struct beta> *tr, char _u, char _v, llist<int> *_l) {
  if (tr == NULL)
    return makeTree<struct beta>(makeBeta(_u, _v, _l), NULL, NULL);
  else if (_u < tr->beta->c1)
    return makeTree<struct beta>(makeBeta(tr->beta->c1, tr->beta->c2, listCpy<int>(tr->beta->l)), otr_add(tr->lt, _u, _v, _l), treeCpy<int>(tr->rt));
  else if (tr->beta->c2 < _u)
    return makeTree<struct beta>(makeBeta(tr->beta->c1, tr->beta->c2, listCpy<int>(tr->beta->l)), treeCpy<int>(tr->lt), otr_add(tr->rt, _u, _v, _l));
  else {
    btree<struct beta> _ltr, _rtr;
    if (_u == tr->beta->c1)
      _ltr = tr->lt
    else if (tr->beta->c1 < _u)
      _ltr = otr_add(tr->lt, tr->beta->c1, zprev(_u), tr->beta->l);
    else
      _ltr = otr_add(tr->lt, _u, zprev(tr->beta->c1), _l);
    
    if (_v == tr->beta->c2)
      _rtr = tr->rt;
    else if (_v < tr->beta->c2)
      _rtr = otr_add(tr->rt, znext(_v), tr->beta->c2, tr->beta->l);
    else
      _rtr = otr_add(tr->rt, znext(tr->beta->c2), _v, _l);
    return makeTree<struct beta>(makeBeta(max(tr->beta->c1, _u), min(tr->beta->c2, _v), append<int>(_l, tr->beta->l)), _ltr, _rtr);
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

