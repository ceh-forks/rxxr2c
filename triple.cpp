#include "nfa.hpp"
#include "set.hpp"
#include "word.hpp"

//NFA state pair + phi
struct triple {
  int i1;
  int i2;
  struct llist<int> *phi;
};

struct triple *triple_make(int i1, int i2, struct llist<int> *phi) {
  struct triple *r = new struct triple;
  r->i1 = i1;
  r->i2 = i2;
  r->phi = phi;
  return r;
}

int tripleset_compare(struct triple *t1, struct triple *t2) {
  if (t1->i1 > t2->i1)
    return 1;
  else if (t1->i1 < t2->i1)
    return -1;
  else {
    if (t1->i2 > t2->i2)
      return 1;
    else if (t1->i2 < t2->i2)
      return -1;
    else
      return compare_intlist(t1->phi, t2->phi);
  }
}

struct llist<struct triple *> *tripleset_add(struct triple *t, struct llist<struct triple *> *tl) {
  if (tl == NULL)
    return addListNode<struct triple *>(t, tl);
  else if (tripleset_compare(t, tl->head) == 1) {
    tl->tail = tripleset_add(t, tl->tail);
    return tl;
  }
  else
    return addListNode<struct triple *>(t, tl);
}

struct ttnode {
  char c1;
  char c2;
  struct llist<int> *l1;
  struct llist<int> *l2;
  struct llist<int> *l3;
  struct ttnode *tl;
  struct ttnode *tr;
};

struct ttnode *ttr_add(struct ttnode *tr, char u, char v, struct llist<int> *s1, struct llist<int> *s2, struct llist<int> *s3) {
  if (tr == NULL) {
    struct ttnode *r = new ttnode;
    r->c1 = u;
    r->c2 = v;
    r->l1 = s1;
    r->l2 = s2;
    r->l3 = s3;
    r->tl = NULL;
    r->tr = NULL;
    return r;
  }
  else if (v < tr->c1) {
    tr->tl = ttr_add(tr->tl, u, v, s1, s2, s3);
    return tr;
  }
  else if (tr->c2 < u) {
    tr->tr = ttr_add(tr->tr, u, v, s1, s2, s3);
    return tr;
  }
  else {
    struct ttnode *ltr, *rtr;
    if (tr->c1 == u)
      ltr = tr->tl;
    else if (tr->c1 < u)
      ltr = ttr_add(tr->tl, tr->c1, zprev(u), tr->l1, tr->l2, tr->l3);
    else
      ltr = ttr_add(tr->tl, u, zprev(tr->c1), s1, s2, s3);
    if (tr->c2 == v)
      rtr = tr->tr;
  }
}

struct llist<struct twople<word *, struct triple *> *> *ttr_collect_fold1(struct llist<int> *js, struct llist<struct twople<word *, struct triple *> *> *l, struct ttnode *tr, word *w, int i, struct llist<int> *p) {
  if (js == NULL)
    return l;
  else {
    word *rw = word_extend(w, makePair<char>(tr->c1, tr->c2));
    struct triple *rt = triple_make(min(i, js->head), max(i, js->head), p);
    struct llist<struct twople<word *, struct triple *> *> *r = addListNode<struct twople<word *, struct triple *> *>(makeTwople<word *, struct triple *>(rw, rt), l);
    return ttr_collect_fold1(js->tail, r, tr, w, i, p);
  }
}

struct llist<struct twople<word *, struct triple *> *> *ttr_collect_fold2(struct llist<int> *is, struct llist<struct twople<word *, struct triple *> *> *l, struct ttnode *tr, word *w, struct llist<int> *p) {
  if (is == NULL)
    return l;
  else
    return ttr_collect_fold1(tr->l2, l, tr, w, is->head, p);
}

struct llist<struct twople<word *, struct triple *> *> *ttr_collect(ttnode *tr, word *w, struct llist<struct twople<word *, struct triple *> *> *lst) {
  if (tr == NULL)
    return lst;
  struct llist<int> *p = tr->l3;
  struct llist<struct twople<word *, struct triple *> *> *lst = ttr_collect_fold2(tr->l1, lst, tr, w, p);
  return ttr_collect(tr->tr, w, ttr_collect(tr->tl, w, lst));
}
