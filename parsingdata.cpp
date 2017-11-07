#include<stdlib.h>
#include "baselib.h"
#include "parsingdata.h"

//Create regex with default metadata
struct meta *make_r(int i, int j) {
  struct meta *r;
  r = malloc(sizeof(struct meta));
  r->spos = i;
  r->epos = j;
  r->scount = 0;
  r->nullable = 0;
  return r;
}

struct ctr *make_ctr(int i, int j, struct ctr *c, struct ctr *d) {
  struct ctr *r = malloc(sizeof(struct ctr));
  r->a = i;
  r->b = j;
  r->lt = c;
  r->rt = d;
  return r;
}

struct ctr *ctr_add(struct ctr *node, int u, int v) {
  if(node == NULL)
    return make_ctr(u, v, NULL, NULL);
  else if(v < node->a) {
    struct ctr *t = ctr_add(node->lt, u, v);
    node->lt = n;
    return node;
  }
  else if(node->b < u) {
    struct ctr *t = ctr_add(node->rt, u, v);
    node->rt = n;
    return node;
  }
  else {
    if(node->a != u) {
      struct ctr *t = ctr_add(node->lt, min(node->a, u), zprev(max(node->a, u));
      node->lt = t;
    }
    if(node->b != v) {
      struct ctr *t = ctr_add(node->rt, znext(min(node->b, v)), max(node->b, v);
      node->rt = t;
    }
    struct ctr *t = make_ctr(max(node->a, u), min(node->b, v), node->lt, node->rt);
    if(node->a > node->b)
      return ctr_add(node, v, u);
    else
      return ctr_add(node, u, v);
  }
}

struct llist<struct pair<int>> *ctr_positive_r(struct ctr node, struct llist<struct pair<int>> *lst) {
  if(node == NULL)
    return lst;
  struct llist<struct pair<int>> *t = ctr_positive_r(node->rt, lst);
  if(t == NULL) {
    struct pair<int> *p = malloc(sizeof(struct pair<int>));
    return ctr_positive_r(node->lt,
