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

struct ctr *ctr_add_r(struct ctr *node, int u, int v) {
  if(node == NULL)
    return make_ctr(u, v, NULL, NULL);
  else if(v < node->a) {
    struct ctr *t = ctr_add_r(node->lt, u, v);
    node->lt = n;
    return node;
  }
  else if(node->b < u) {
    struct ctr *t = ctr_add_r(node->rt, u, v);
    node->rt = n;
    return node;
  }
  else {
    if(node->a != u) {
      struct ctr *t = ctr_add_r(node->lt, min(node->a, u), zprev(max(node->a, u));
      node->lt = t;
    }
    if(node->b != v) {
      struct ctr *t = ctr_add_r(node->rt, znext(min(node->b, v)), max(node->b, v);
      node->rt = t;
    }
    struct ctr *t = make_ctr(max(node->a, u), min(node->b, v), node->lt, node->rt);
    return t;
  }
}

struct ctr *ctr_add(struct ctr *node, int u, int v) {
  if(node->a > node->b)
    return ctr_add_r(node, v, u);
  else
    return ctr_add_r(node, u, v);
}

struct llist<struct pair<char> *> *ctr_positive_r(struct ctr *node, struct llist<struct pair<char> *> *lst) {
  if(node == NULL)
    return lst;
  struct llist<struct pair<char> *> *t = ctr_positive_r(node->rt, lst);
  if(t == NULL) {
    struct pair<char> *p = makePair<char>(node->a, node->b);
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(p, NULL);
    return ctr_positive_r(node->lt, r);
  }
  else {
    if(znext(node->b) == t->head->a) {
      t->head->a = node->a;
      return ctr_positive_r(node->lt, t);
    }
    else {
      struct pair<char> *p = makePair<char>(node->a, node->b);
      t = addListNode<struct pair<char> *>(p, t);
      return ctr_positive_r(node->lt, t);
    }
  }
}

struct llist<struct pair<char> *> *ctr_positive(struct ctr *node) {
  return ctr_positive_r(node, NULL);
}

struct neg_r {
  struct llist<struct pair<char> *> *a;
  char b;
};

struct neg_r *mkNegR(struct llist<struct pair<char> *> *i, char j) {
  struct neg_r *r = malloc(sizeof(struct neg_r));
  r->a = i;
  r->b = j;
}

struct neg_r *ctr_negative_r(struct ctr *node, struct llist<struct pair<char> *> neg, char next) {
  if(node == NULL)
    return mkNegR(neg, next);
  struct neg_r *t = ctr_negative_r(node->rt, neg, next);
  if(node->b == zprev(t->b))
    return ctr_negative_r(node->lt, t->a, node->a);
  else {
    struct pair<char> *p = makePair<char>(znext(node->b), zprev(t->b));
    neg = addListNode<struct pair<char> *>(p, neg);
    return ctr_negative_r(node->lt, neg, node->a);
  }
}

struct llist<struct pair<char> *> *ctr_negative(struct ctr *node) {
  struct neg_r *r = ctr_negative_r(node, NULL, '\x80');
  if(ZMIN < r->b) {
    struct pair<char> *t = makePair(ZMIN, zprev(r->b));
    return addListNode(t, r->a);
  }
  else
    return r->a;
}

