#include "baselib.hpp"

//Intsets are represented by llist<int>

//Adds a value to an intset
llist<int> *intset_add(int, llist<int> *);

//Creates the union of two intsets
llist<int> *intset_union(llist<int> *l1, llist<int> *l2);
