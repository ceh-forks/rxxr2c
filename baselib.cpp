#include "baselib.h"
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

template<typename T>
struct pair<T> *makePair(T i, T j) {
  struct pair<T> *t = new struct pair<T>;
  t->a = i;
  t->b = j;
  return t;
}

