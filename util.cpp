#include "baselib.hpp"
#include "nfa.hpp"
#include "set.hpp"
#include "util.hpp"
#include <string>

typedef boost::unordered_map<std::string, struct pair<char> *> hashtbl_cpair;

struct pair<struct llist<int> *> *find_kleene_rec(struct nfa *nfa, int i, struct pair<struct llist<int> *> *lst) {
  if (listMem<int>(i, lst->a))
    return lst;
  struct state *swt_state = get_state(nfa, i);
  lst->a = intset_add(i, lst->a);
  switch(swt_state->type) {
    case End:
    case Kill:
      return lst;
    case Pass:
    case MakeB:
    case EvalB:
    case Match:
    case CheckPred:
      return find_kleene_rec(nfa, swt_state->i, lst);
    case CheckBackref:
    case BeginCap:
    case EndCap:
      return find_kleene_rec(nfa, swt_state->pi->b, lst);
    case BranchAlt:
      return find_kleene_rec(nfa, swt_state->pi->a, find_kleene_rec(nfa, swt_state->pi->b, lst));
    case BranchKln:
      lst->b = addListNode<int>(i, lst->b);
      return find_kleene_rec(nfa, swt_state->pi->a, find_kleene_rec(nfa, swt_state->pi->b, lst));
  }
}

struct llist<int> *find_kleene(struct nfa *nfa) {
  struct pair<struct llist<int> *> *t = makePair<struct llist<int> *>(NULL, NULL);
  t = find_kleene_rec(nfa, nfa->root, t);
  deleteList<int>(t->a);
  struct llist<int> *r = t->b;
  delete t;
  return r;
}

struct branch_struct {
  int i;
  struct pair<int> *p;
};

//collect all the branch points within a given kleene expression
struct twople<struct llist<int> *, struct llist<struct branch_struct *> *> *find_branches_rec(struct nfa *nfa, int i, struct twople<struct llist<int> *, struct llist<struct branch_struct *> *> *lst) {
  if (listMem<int>(i, lst->a))
    return lst;
  lst->a = intset_add(i, lst->a);
  struct state *swt_state = get_state(nfa, i);
  switch(swt_state->type) {
    case End:
    case Kill:
      return lst;
    case Pass:
    case MakeB:
    case EvalB:
    case Match:
    case CheckPred:
      return find_branches_rec(nfa, swt_state->i, lst);
    case CheckBackref:
    case BeginCap:
    case EndCap:
      return find_branches_rec(nfa, swt_state->pi->b, lst);
    case BranchAlt:
    case BranchKln:
      struct branch_struct *t = new branch_struct;
      t->i = i;
      t->p = makePair<int>(swt_state->pi->a, swt_state->pi->b);
      lst->b = addListNode<struct branch_struct *>(t, lst->b);
      return find_branches_rec(nfa, swt_state->pi->a, find_branches_rec(nfa, swt_state->pi->b, lst));
  }
}

struct llist<struct branch_struct *> *find_branches(struct nfa *nfa, int ik) {
  struct state *s = get_state(nfa, ik);
  if (s->type == BranchKln) {
    //all the sub-expression wire back to ik, so we will never explore outside the kleene expression
    struct twople<struct llist<int> *, struct llist<struct branch_struct *> *> *t = find_branches_rec(nfa, s->pi->a, makeTwople<struct llist<int> *, struct llist<struct branch_struct *> *>(addListNode<int>(ik, NULL), NULL));
    deleteList<int>(t->a);
    struct llist<struct branch_struct *> *r = t->b;
    delete t;
    return r;
  }
  else
    return NULL;
}

bool ier_rec(int i, struct llist<int> *st, struct nfa *nfa, word *w, int dst) {
  if (i == dst) {
    deleteList<int>(st);
    return true;
  }
  else if (listMem<int>(i, st)) {
    deleteList<int>(st);
    return false;
  }
  st = intset_add(i, st);
  struct state *swt_state = get_state(nfa, i);
  switch(swt_state->type) {
    case End:
    case Kill:
    case Match:
      deleteList<int>(st);
      return false;
    case Pass:
    case MakeB:
    case EvalB:
      return ier_rec(swt_state->i, st, nfa, w, dst);
    case CheckPred:
      if (swt_state->p->type == P_BOI) {
        if (w == NULL)
          return ier_rec(swt_state->i, st, nfa, w, dst);
        else {
          deleteList<int>(st);
          return false;
        }
      }
      else if (swt_state->p->type == P_BOL) {
        if (w == NULL)
          return ier_rec(swt_state->i, st, nfa, w, dst);
        else if (w->head->a <= '\n' && '\n' <= w->head->b)
          return ier_rec(swt_state->i, st, nfa, w, dst);
        else if (swt_state->p->value == 1 && w->head->a <= '\r' && '\r' <= w->head->b)
          return ier_rec(swt_state->i, st, nfa, w, dst);
        else {
          deleteList<int>(st);
          return false;
        }
      }
      else {
        deleteList<int>(st);
        return false;
      }
    case CheckBackref:
      //Not supported
      deleteList<int>(st);
      return false;
    case BeginCap:
    case EndCap:
      return ier_rec(swt_state->pi->b, st, nfa, w, dst);
    case BranchAlt:
    case BranchKln:
      return ier_rec(swt_state->pi->a, st, nfa, w, dst) || ier_rec(swt_state->pi->b, st, nfa, w, dst);
  }
}

bool is_epsilon_reachable(struct nfa *nfa, word *w, int src, int dst) {
  return ier_rec(src, NULL, nfa, w, dst);
}

struct xtree {
  char u;
  char v;
  struct llist<int> *l1;
  struct llist<int> *l2;
  struct xtree *ltr;
  struct xtree *rtr;

  ~xtree() {
    deleteList<int>(l1);
    deleteList<int>(l2);
    if (ltr != NULL)
      delete ltr;
    if (rtr != NULL)
      delete rtr;
  }
};

struct xtree *xtr_add(struct xtree *tr, char u, char v, struct llist<int> *l1, struct llist<int> *l2) {
  if (tr == NULL) {
    struct xtree *r = new struct xtree;
    r->u = u;
    r->v = v;
    r->l1 = l1;
    r->l2 = l2;
    r->ltr = NULL;
    r->rtr = NULL;
    return r;
  }
  else if (v < tr->u) {
    tr->ltr = xtr_add(tr->ltr, u, v, l1, l2);
    return tr;
  }
  else if (tr->v < u) {
    tr->rtr = xtr_add(tr->rtr, u, v, l1, l2);
    return tr;
  }
  else {
    struct xtree *_ltr, *_rtr;
    if (tr->u == u)
      _ltr = tr->ltr;
    else if (tr->u < u)
      _ltr = xtr_add(tr->ltr, tr->u, zprev(u), tr->l1, tr->l2);
    else
      _ltr = xtr_add(tr->ltr, u, zprev(tr->u), l1, l2);
    if (tr->v == v)
      _rtr = tr->rtr;
    else if (v < tr->v)
      _rtr = xtr_add(tr->rtr, znext(v), tr->v, tr->l1, tr->l2);
    else
      _rtr = xtr_add(tr->rtr, znext(tr->v), v, l1, l2);
    tr->u = max(tr->u, u);
    tr->v = min(tr->v, v);
    tr->l1 = listAppend<int>(tr->l1, l1);
    tr->l2 = listAppend<int>(tr->l2, l2);
    tr->ltr = _ltr;
    tr->rtr = _rtr;
    return tr;
  }
}

std::string utilHashFunc(int i, int j) {
  std::string r = std::to_string(min(i, j)) + "x" + std::to_string(max(i, j)) + "e";
}

struct pair<int> *utilHashFuncReverse(std::string s) {
  int i = 0;
  while (s.at(i) != 'x')
    i++;
  std::string s1 = s.substr(0, i);
  i++;
  int j = 0;
  while (s.at(i + j) != 'e')
    j++;
  std::string s2 = s.substr(i, j-i);
  return makePair<int>(stoi(s1), stoi(s2));
}

void xtr_collect_rec(xtree *tr, hashtbl_cpair *table) {
  if (tr = NULL)
    return;
  xtr_collect_rec(tr->ltr, table);
  xtr_collect_rec(tr->rtr, table);
  struct llist<int> *l1, *l2;
  l1 = tr->l1;
  while(l1) {
    l2 = tr->l2;
    while(l2) {
      table->insert( { utilHashFunc(l1->head, l2->head), makePair<char>(tr->u, tr->v) } );
      l2 = l2->tail;
    }
    l1 = l1->tail;
  }
  return;
}

//compute all possible parallel transitions
hashtbl_cpair *xtr_collect(xtree *tr) {
  hashtbl_cpair *table = new hashtbl_cpair;
  xtr_collect_rec(tr, table);
  return table;
}

//find all parallel transitions for the specified state pair
hashtbl_cpair *get_parallel_transitions(struct nfa *nfa, int ik, int i1, int i2) {
  struct llist<struct transition *> *i1_lst, *i2_lst, *iter1, *iter2;
  i1_lst = get_transitions(nfa, i1);
  i2_lst = get_transitions(nfa, i2);
  iter1 = i1_lst;
  iter2 = i2_lst;
  xtree *r = NULL;
  //we're only interested in transitions within the current kleene expression
  while(iter1) {
    if (iter1->head->c <= ik)
      r = xtr_add(r, iter1->head->a, iter1->head->b, addListNode<int>(iter1->head->c, NULL), NULL);
    iter1 = iter1->tail;
  }
  while(iter2) {
    if (iter2->head->c <= ik)
      r = xtr_add(r, iter2->head->a, iter2->head->b, NULL, addListNode<int>(iter2->head->c, NULL));
    iter2 = iter2->tail;
  }
  deleteListWithPointers<struct transition *>(i1_lst);
  deleteListWithPointers<struct transition *>(i2_lst);
  hashtbl_cpair *table = xtr_collect(r);
  delete r;
  return table;
}

struct fc_struct {
  struct llist<struct pair<int> *> *pcache;
  struct llist<struct pair<int> *> *nlist;
  struct llist<struct pair<int> *> *clist;
};

//collect converging products and eliminate redundancies
struct fc_struct *filter_convergences(struct llist<struct pair<int> *> *plist, struct llist<struct pair<int> *> *pcache, struct llist<struct pair<int> *> *nlist, struct llist<struct pair<int> *> *clist, struct nfa *nfa) {
  if (plist == NULL) {
    struct fc_struct *r = new fc_struct;
    r->pcache = pcache;
    r->nlist = nlist;
    r->clist = clist;
    return r;
  }
  else if (plist->head->a == plist->head->b) {
    struct llist<struct pair<int> *> *t = plist;
    plist = plist->tail;
    t->tail = clist;
    clist = t;
    return filter_convergences(plist, pcache, nlist, clist, nfa);
  }
  else {
    if (intpairset_mem(plist->head, pcache)) {
      //product already encountered, ignore
      //TODO: memory
      return filter_convergences(plist->tail, pcache, nlist, clist, nfa);
    }
    else {
      //new product, needs to be simulated further
      struct llist<struct pair<int> *> *t = plist;
      plist = plist->tail;
      pcache = intpairset_add(t->head, pcache);
      nlist = addListNode<struct pair<int> *>(t->head, nlist);
      delete t;
      return filter_convergences(plist, pcache, nlist, clist, nfa);
    }
  }
}

bool check_convergence(int ik, struct llist<struct pair<int> *> *plist, struct llist<struct pair<int> *> *pcache, struct nfa *nfa) {
  if (plist == NULL)
    return false;
  //calculate the new list of products reachable from the current product
  struct llist<struct pair<int> *> *nprods = NULL;
  hashtbl_cpair *table = get_parallel_transitions(nfa, ik, plist->head->a, plist->head->b);
  hashtbl_cpair::iterator iter = table->begin;
  while(iter != table->end) {
    nprods = addListNode<struct pair<int> *>(utilHashFuncReverse(iter->first), nprods);
    delete iter->second;
  }
  table->clear;
  delete table;
  //check to see if there are any converging products
  struct fc_struct *fc = filter_convergences(nprods, pcache, plist->tail, NULL, nfa);
    if (fc->clist == NULL) {
      bool r = check_convergence(ik, fc->nlist, pcache, nfa);
      delete fc;
      return r;
    }
    else {
      delete fc;
      return true;
    }
}

//filter out innner branches with parallel paths leading to a common state
struct llist<int> *filter_converging_branches(int ik, struct llist<struct branch_struct *> *l, struct nfa *nfa) {
  if (l == NULL)
    return NULL;
  struct llist<int> *tf = filter_converging_branches(ik, l->tail, nfa);
  struct llist<struct pair<int> *> *p = addListNode<struct pair<int> *>(makePair<int>(l->head->p->a, l->head->p->b), NULL);
  if (check_convergence(ik, p, NULL, nfa))
    return addListNode<int>(l->head->i, tf);
  else
    return tf;
}

//analyse each kleene for pumpability
void fpk_explore(struct llist<int> *klns, struct nfa *nfa, hashtbl_kleene *table) {
  if (klns == NULL)
    return;
  //collect pumpable kleene states along with the corresponding branch points
  struct llist<int> *brset, *t, *iter;
  brset = NULL;
  t = filter_converging_branches(klns->head, find_branches(nfa, klns->head), nfa);
  iter = t;
  while(iter) {
    intset_add(iter->head, brset);
    iter = iter->tail;
  }
  deleteList<int>(t);
  if (brset != NULL)
    table->insert( { klns->head, brset } );
  fpk_explore(klns->tail, nfa, table);
}

hashtbl_kleene *find_pumpable_kleene(struct nfa *nfa) {
  hashtbl_kleene *result = new hashtbl_kleene;
  fpk_explore(find_kleene(nfa), nfa, result);
  return result;
}
