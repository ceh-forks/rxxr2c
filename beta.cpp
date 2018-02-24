#include "nfa.hpp"
#include "set.hpp"
#include "error.hpp"
#include "word.hpp"
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

struct llist<struct twople<word *, struct llist<int> *> *> *advance(struct nfa *nfa, word *w, llist<int> *b) {
  return otr_collect(advance_fold_right(nfa, b, NULL), w, NULL);
}

int main(void) {
  std::cout << "hello\n";
  return 0;
}

