#include "baselib.hpp"
#include <iostream>

int min(int i, int j) {
  if(i<j) return i;
  else return j;
}

int max(int i, int j) {
  if(i>j) return i;
  else return j;
}

char zprev(char u) {
  if(u <= ZMIN)
    return ZMIN;
  else
    return (u-1);
}

char znext(char u) {
  if(u >= ZMAX)
    return ZMAX;
  else
    return (u+1);
}

//Ignore case of string
void strLower(char *p) {
	for ( ; *p; ++p) *p = tolower(*p);
}

template<typename T>
struct llist<T> *addListNode(T item, struct llist<T> *node) {
  struct llist<T> *t = new struct llist<T>;
  t->head = item;
  t->tail = node;
  return t;
}

template <typename T>
void deleteList(struct llist<T> *l) {
  if (l == NULL)
    return;
  struct llist<T> *t = l->tail;
  delete l;
  deleteList(t);
}

template <typename T>
void deleteListWithPointers(struct llist<T> *l) {
  if (l == NULL)
    return;
  struct llist<T> *t = l->tail;
  delete l->head;
  delete l;
  deleteListWithPointers(t);
}

template<typename T>
struct llist<T> *listRev2(struct llist<T> *l, struct llist<T> *prev) {
  if (l == NULL)
    return prev;
  else {
    struct llist<T> *r = addListNode<T>(l->head, prev);
    return listRev2(l->tail, r);
  }
}

template<typename T>
struct llist<T> *listRev(struct llist<T> *l) {
  return listRev2(l, NULL);
}

template<typename T>
int listMem(T item, struct llist<T> *node) {
  struct llist<T> *t = node;
  while(t) {
    if (t->head == item)
      return 1;
    else
      t = t->tail;
  }
  return 0;
}

template<typename T>
struct llist<T> *listInitialise(T item, int i) {
  if (i == 1)
    return addListNode<T>(item, NULL);
  else
    return addListNode<T>(item, listInitialise(item, i-1));
}

template<typename T>
struct llist<T> *listAdvance(struct llist<T> *l, int i) {
  if (i == 0)
    return l;
  if (l == NULL)
    l = addListNode<T>(NULL, NULL);
  if (l->tail == NULL)
    l->tail = addListNode<T>(NULL, NULL);
  return listAdvance<T>(l->tail, i-1);
}

template <typename T>
struct llist<T> *listAppend(struct llist<T> *l1, struct llist<T> *l2) {
  if (l1 == NULL)
    return l2;
  else {
    struct llist<T> *t = l1;
    while(t->tail)
      t = t->tail;
    t->tail = l2;
    return l1;
  }
}

template <typename T>
int listLength(struct llist<T> *l) {
  int i = 0;
  while(l) {
    l = l->tail;
    i++;
  }
  return i;
}

template<typename T>
struct llist<T> *listCpy(struct llist<T> *olist) {
  if (olist == NULL)
    return NULL;
  else {
    struct llist<T> *r = new struct llist<T>;
    r->head = olist->head;
    r->tail = listCpy(olist->tail);
    return r;
  }
}

template<typename T>
struct btree<T> *treeCpy(struct btree<T> *otree) {
  if (otree == NULL)
    return NULL;
  else {
    struct btree<T> *r = new struct btree<T>;
    r->node = otree->node;
    r->lt = treeCpy(otree->lt);
    r->rt = treeCpy(otree->rt);
    return r;
  }
}

template<typename T>
struct pair<T> *makePair(T i, T j) {
  struct pair<T> *t = new struct pair<T>;
  t->a = i;
  t->b = j;
  return t;
}

template<typename T1, T2>
struct twople<T1, T2> *makeTwople(T1 a, T2 b) {
  struct twople<T1, T2> *r = new struct twople<T1, T2>;
  r->a = a;
  r->b = b;
  return r;
}

template <typename T>
struct btree<T> *makeTree(T node, struct btree<T> *l, struct btree<T> *r) {
  struct btree<T> *r = new struct btree<T>;
  r->node = node;
  r->lt = l;
  r->rt = r;
  return r;
}
