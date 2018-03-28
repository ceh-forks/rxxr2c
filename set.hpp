#ifndef SET_H
#define SET_H

#include "baselib.hpp"

//Intsets are represented by llist<int>

//Adds a value to an intset
llist<int> *intset_add(int, llist<int> *);

//Creates the union of two intsets
llist<int> *intset_union(llist<int> *, llist<int> *);

//checks if first set is subset of the second
bool intset_subset(struct llist<int> *, struct llist<int> *);

int compare_intlist(struct llist<int> *, struct llist<int> *);

struct llist<struct pair<int> *> *intpairset_add(struct pair<int> *, struct llist<struct pair<int> *> *);
bool intpairset_mem(struct pair<int> *, struct llist<struct pair<int> *> *);

#endif
