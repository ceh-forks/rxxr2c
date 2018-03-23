#include "baselib.hpp"
#include "set.hpp"
#include "error.hpp"

struct llist<int> *intset_add(int i, struct llist<int> *l) {
  if (l == NULL)
    return addListNode<int>(i, l);
  else if (i > l->head) {
    l->tail = intset_add(i, l->tail);
    return l;
  }
  else
    return addListNode<int>(i, l);
}

struct llist<int> *intset_union(struct llist<int> *l1, struct llist<int> *l2) {
  struct llist<int> *t1 = listCpy<int>(l1);
  struct llist<int> *t2 = l2;
  while(t2) {
    t1 = intset_add(t2->head, t1);
    t2 = t2->tail;
  }
  return t1;
}

int compare_intpair(struct pair<int> *x, struct pair<int> *y) {
  if(x->a < y->a)
    return -1;
  else if(x->a > y->a)
    return 1;
  else {
    if (x->b < y->b)
      return -1;
    else if (x->b > y->b)
      return 1;
    else
      return 0;
  }
}

struct llist<struct pair<int> *> *intpairset_add(struct pair<int> *p, struct llist<struct pair<int> *> *l) {
  if (l == NULL)
    return addListNode<struct pair<int> *>(p, l);
  else if (compare_intpair(p, l->head) == 1) {
    l->tail = intpairset_add(p, l->tail);
    return l;
  }
  else
    return addListNode<struct pair<int> *>(p, l);
}

bool intpairset_mem(struct pair<int> *p, struct llist<struct pair<int> *> *l) {
  while (l) {
    if (p->a == l->head->a && p->b == l->head->b)
      return true;
    else
      l = l->tail;
  }
  return false;
}

int compare_intlist(struct llist<int> *x, struct llist<int> *y) {
  if (x == NULL && y == NULL)
    return 0;
  else if (x == NULL || y == NULL)
    raise(COMPARE_INPUT_ERROR);
  else if (x->head < y->head)
    return -1;
  else if (x->head > y->head)
    return 1;
  else
    return compare_intlist(x->tail, y->tail);
}


