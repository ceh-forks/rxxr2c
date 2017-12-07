#include "parsingdata.h"
#include <iostream>

struct llist<int> *make(int i) {
  struct llist<int> *r = new struct llist<int>;
  r->head = i;
  r->tail = NULL;
  return r;
}

void btins(struct btree<int> *t, int i) {
  if(t == NULL) {
    t = new struct btree<int>;
    t->node = i;
    t->lt = NULL;
    t->rt = NULL;
  }
  else if(i < t->node) {
    if(t->lt != NULL)
      btins(t->lt, i);
    else {
      t->lt = new struct btree<int>;
      t->lt->node = i;
      t->lt->lt = NULL;
      t->lt->rt = NULL;
    }  
  }
  else {
    if(t->rt != NULL)
      btins(t->rt, i);
    else {
      t->rt = new struct btree<int>;
      t->rt->node = i;
      t->rt->lt = NULL;
      t->rt->rt = NULL;
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

otr_add(

int main(void) {
  std::cout << "hello\n";
  return 0;
}

