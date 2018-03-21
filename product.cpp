#include "phi.hpp"
#include "set.hpp"
#include "flags.hpp"
#include "triple.hpp"

struct product {
  int i;
  struct llist<int> *phi; //Sorted Int set
};

struct product *product_make(int i, struct llist<int> *phi) {
  struct product *r = new struct product;
  r->i = i;
  r->phi = phi;
  return r;
}

int productset_compare(struct product *p1, struct product *p2) {
  if (p1->i > p2->i)
    return 1;
  else if (p1->i < p2->i)
    return -1;
  else
    return compare_intlist(p1->phi, p2->phi);
}

struct llist<struct product *> *productset_add(struct product *p, struct llist<struct product *> *l) {
  if (l == NULL)
    return addListNode<struct product *>(p, l);
  else if (productset_compare(p, l->head) == 1) {
    l->tail = productset_add(p, l->tail);
    return l;
  }
  else
    return addListNode<struct product *>(p, l);
}

struct ptnode {
  char c1;
  char c2;
  struct llist<int> *l1;
  struct llist<int> *l2;
  struct ptnode *t1;
  struct ptnode *t2;
};

struct ptnode *ptr_add(struct ptnode *tr, char u, char v, struct llist<int> *s1, struct llist<int> *s2) {
  struct ptnode *r;
  if (tr == NULL) {
    r = new struct ptnode;
    r->c1 = u;
    r->c2 = v;
    r->l1 = s1;
    r->l2 = s2;
    r->t1 = NULL;
    r->t2 = NULL;
  }
  else if (v < tr->c1) {
    r = tr;
    r->t1 = ptr_add(r->t1, u, v, s1, s2);
  }
  else if (tr->c2 < u) {
    r = tr;
    r->t2 = ptr_add(r->t2, u, v, s1, s2);
  }
  else {
    r = tr;
    struct ptnode *ltr, *rtr;
    if (r->c1 == u)
      ltr = r->t1;
    else if (r->c1 < u)
      ltr = ptr_add(r->t1, tr->c1, zprev(u), r->l1, r->l2);
    else
      ltr = ptr_add(r->t1, u, zprev(r->c1), s1, s2);
    if (r->c2 == v)
      rtr = r->t2;
    else if (v < r->c2)
      rtr = ptr_add(r->t2, znext(v), r->c2, r->l1, r->l2);
    else
      rtr = ptr_add(r->t2, znext(r->c2), v, s1, s2);
    r->c1 = max(r->c1, u);
    r->c2 = min(r->c2, v);
    struct llist<int> *d1, *d2;
    d1 = r->l1; d2 = r->l2;
    r->l1 = intset_union(r->l1, s1);
    r->l2 = intset_union(r->l2, s2);
    deleteList<int>(s1);
    deleteList<int>(s2);
    deleteList<int>(d1);
    deleteList<int>(d2);
    r->t1 = ltr;
    r->t2 = rtr;
  }
  return r;
}

struct llist<struct twople<word *, struct product *> *> *ptr_collect_fold(struct llist<int> *i, struct llist<struct twople<word *, struct product *> *> *l, word *w, char u, char v, struct llist<int> *p) {
  if (i == NULL)
    return l;
  crange *cr = makePair<char>(u, v);
  struct product *pr = new struct product;
  pr->i = i->head;
  pr->phi = p;
  l = addListNode<struct twople<word *, struct product *> *>(makeTwople<word *, struct product *>(word_extend(w, cr), pr), l);
  return ptr_collect_fold(i->tail, l, w, u, v, p);
}

struct llist<struct twople<word *, struct product *> *> *ptr_collect(struct ptnode *tr, word *w, struct llist<struct twople<word *, struct product *> *> *lst) {
  if (tr == NULL)
    return lst;
  struct llist<int> *p = tr->l2;
  lst = ptr_collect_fold(tr->l1, lst, w, tr->c1, tr->c2, p);
  return ptr_collect(tr->t2, w, ptr_collect(tr->t1, w, lst));
}

struct ptnode *product_advance_foldleft1(struct ptnode *tr, struct llist<struct transition *> *tl) {
  if (tl == NULL)
    return tr;
  else {
    struct ptnode *r = product_advance_foldleft1(ptr_add(tr, tl->head->a, tl->head->b, intset_add(tl->head->c, NULL), NULL), tl->tail);
    delete tl->head;
    delete tl;
    return r;
  }
}

struct ptnode *product_advance_foldleft2(struct ptnode *tr, struct llist<struct transition *> *tl) {
  if (tl == NULL)
    return tr;
  else {
    struct ptnode *r = product_advance_foldleft2(ptr_add(tr, tl->head->a, tl->head->b, NULL, intset_add(tl->head->c, NULL)), tl->tail);
    delete tl->head;
    delete tl;
    return r;
  }
}

struct ptnode *product_advance_setfold(struct llist<int> *is, struct ptnode *tr, struct nfa *nfa) {
  if (is == NULL)
    return tr;
  else
    return product_advance_setfold(is->tail, product_advance_foldleft2(tr, get_transitions(nfa, is->head)), nfa);
}

struct llist<struct twople<word *, struct product *> *> *product_advance(struct nfa *nfa, word *w, struct product *p) {
  struct ptnode *tr = product_advance_foldleft1(NULL, get_transitions(nfa, p->i));
  tr = product_advance_setfold(p->phi, tr, nfa);
  return ptr_collect(tr, w, NULL);
}

bool productset_mem(struct product *p, struct llist<struct product *> *l) {
  if (p == NULL)
    return false;
  while(l) {
    if(p->i == l->head->i && listSame<int>(p->phi, l->head->phi))
      return true;
    else
      l = l->tail;
  }
  return false;
}

struct product_evolve_struct {
  int flags;
  struct product *product;
  struct llist<struct triple *> *triple;
};

struct product_evolve_struct *product_evolve_rec(struct llist<int> *pl, struct llist<int> *st, struct llist<struct triple *> *tpl, int flags, int i, struct llist<int> *ep, struct nfa *nfa, word *w, struct llist<int> *brset) {
  if (pl == NULL) {
    struct product_evolve_struct *r = new product_evolve_struct;
    r->flags = flags;
    r->product = product_make(i, ep);
    r->triple = tpl;
    deleteList<int>(ep);
    deleteList<int>(st);
    return r;
  }
  else if (listMem<int>(i, st))
    return product_evolve_rec(pl->tail, st, tpl, flags, i, ep, nfa, w, brset);
  st = intset_add(pl->head, st);
  struct state *swt_state = get_state(nfa, pl->head);
  switch(swt_state->type) {
    case End:
    case Kill:
    case Match:
      struct llist<int> *pl2 = pl->tail;
      delete pl;
      return product_evolve_rec(pl2, st, tpl, flags, i, ep, nfa, w, brset);
    case Pass:
    case MakeB:
    case EvalB:
      struct llist<int> *pl2 = addListNode<int>(swt_state->i, pl->tail);
      delete pl;
      return product_evolve_rec(pl2, st, tpl, flags, i, ep, nfa, w, brset);
    case BeginCap:
    case EndCap:
      struct llist<int> *pl2 = addListNode<int>(swt_state->pi->b, pl->tail);
      delete pl;
      return product_evolve_rec(pl2, st, tpl, flags, i, ep, nfa, w, brset);
    case CheckPred:
      if (swt_state->p->type == P_BOI || swt_state->p->type == P_BOL || swt_state->p->type == P_EOI) {
        if (swt_state->p->type != P_EOI && (w == NULL || (swt_state->p->type == P_BOL && ((w->head->a <= '\n' && '\n' <= w->head->b) || (swt_state->p->value && w->head->a <= '\r' && '\r' <= w->head->b))))) {
          struct llist<int> *pl2 = addListNode<int>(swt_state->i, pl->tail);
          delete pl;
          return product_evolve_rec(pl2, st, tpl, flags, i, ep, nfa, w, brset);
        }
        else {
          struct llist<int> *pl2 = pl->tail;
          delete pl;
          return product_evolve_rec(pl2, st, tpl, flags, i, ep, nfa, w, brset);
        }
      }
      else {
        flags = set_interrupted(flags);
        struct llist<int> *pl2 = pl->tail;
        delete pl;
        return product_evolve_rec(pl2, st, tpl, flags, i, ep, nfa, w, brset);
      }
    case CheckBackref:
      flags = set_interrupted(flags);
      struct llist<int> *pl2 = pl->tail;
      delete pl;
      return product_evolve_rec(pl2, st, tpl, flags, i, ep, nfa, w, brset);
    case BranchAlt:
    case BranchKln:
      if (listMem<int>(i, brset))
        tpl = addListNode<struct triple *>(triple_make(swt_state->pi->a, swt_state->pi->b, ep), tpl);
        struct llist<int> *pl2 = addListNode<int>(swt_state->pi->b, pl->tail);
        pl2 = addListNode<int>(swt_state->pi->a, pl2);
        delete pl;
        return product_evolve_rec(pl2, st, tpl, flags, i, ep, nfa, w, brset);
  }
}

struct product_evolve_struct *product_evolve(struct nfa *nfa, word *w, struct product *p, struct llist<int> *brset) {
  struct phi_evolve_struct *phi_evolved = phi_evolve(nfa, w, p->phi, NULL);
  int flgs = phi_evolved->flgs;
  struct llist<int> *ep = phi_evolved->lst;
  delete phi_evolved;
  return product_evolve_rec(addListNode<int>(p->i, NULL), NULL, NULL, flgs, p->i, ep, nfa, w, brset);
}
