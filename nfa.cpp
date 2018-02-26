#include<ctype.h>
#include "parsingdata.hpp"
#include "nfa.hpp"
#include "set.hpp"

struct pred *get_pcond(enum predtype p, int flags) {
  struct pred *r = new struct pred;
  switch(p) {
    case Boi:
      r->type = P_BOI;
      break;
    case Eom:
      r->type = P_EOM;
      break;
    case Eoi1:
      r->type = P_EOIX;
      r->value = (flags & FLAG_UNIX_LINES) != 0;
      break;
    case Eoi2:
      r->type = P_EOI;
      break;
    case Bol:
      if((flags & FLAG_UNIX_LINES) != 0) {
        r->type = P_BOL;
        r->value = (flags & FLAG_UNIX_LINES);
      }
      else
        r->type = P_BOI;
      break;
    case Eol:
      if((flags & FLAG_UNIX_LINES) != 0) {
        r->type = P_EOL;
        r->value = (flags & FLAG_UNIX_LINES);
      }
      else
        r->type = P_EOI;
      break;
    case Wordb:
      r->type = P_WB;
      break;
    case NWordb:
      r->type = P_NWB;
      break;
  }
  return r;
}

void decorate_regex(struct regex *r, int flags) {
  struct exp *e = r->expression;
  struct meta *m = r->metadata;
  switch(e->type) {
    case Zero:
    case Dot:
    case Atom:
      m->scount = 1;
      m->nullable = 0;
      m->cflags = flags;
      break;
    case One:
    case Pred:
    case Backref:
      m->scount = 1;
      m->nullable = 1;
      m->cflags = flags;
      break;
    case Group:
      int flags2;
      switch(e->group->type) {
        case MODS:
          flags2 = (flags | e->group->m_on) & (!e->group->m_off);
          m->scount = 0;
          m->nullable = 1;
          m->cflags = flags2;
          break;
        case CAP:
          decorate_regex(e->group->r1, flags);
          m->scount = 2 + e->group->r1->metadata->scount;
          m->nullable = e->group->r1->metadata->nullable;
          m->cflags = flags;
          break;
        case NOCAP:
          flags2 = (flags | e->group->m_on) & (!e->group->m_off);
          decorate_regex(e->group->r1, flags2);
          m->scount = e->group->r1->metadata->scount;
          m->nullable = e->group->r1->metadata->nullable;
          m->cflags = flags;
        //default:
          //TODO Unsupported grouping construct
      }
      break;
    case Conc:
      decorate_regex(e->conalt->r1, flags);
      decorate_regex(e->conalt->r2, e->conalt->r1->metadata->cflags);
      m->scount = 1 + e->conalt->r1->metadata->scount + e->conalt->r2->metadata->scount;
      m->nullable = e->conalt->r1->metadata->nullable && e->conalt->r2->metadata->nullable;
      m->cflags = e->conalt->r2->metadata->cflags;
      break;
    case Alt:
      decorate_regex(e->conalt->r1, flags);
      decorate_regex(e->conalt->r2, e->conalt->r1->metadata->cflags);
      m->scount = 1 + e->conalt->r1->metadata->scount + e->conalt->r2->metadata->scount;
      m->nullable = e->conalt->r1->metadata->nullable || e->conalt->r2->metadata->nullable;
      m->cflags = e->conalt->r2->metadata->cflags;
      break;
    case Kleene:
      int n;
      decorate_regex(e->kleene->r, flags);
      if(e->kleene->r->metadata->nullable == 1)
        n = 3;
      else
        n = 1;
      m->scount = n + e->kleene->r->metadata->scount;
      m->nullable = 1;
      m->cflags = e->kleene->r->metadata->cflags;
      break;
  }
}

struct pair<int> *compile(struct regex *r, struct llist<struct state *> *sv, struct llist<struct pair<int> *> *pv, int next, int kont, int flags) {
  exp *e = r->expression;
  meta *m = r->metadata;
  switch(e->type) {
    case Zero: {
      struct pair<int> *p = listAdvance<struct pair<int> *>(pv, next)->head;
      p->a = m->spos;
      p->b = m->epos;
      listAdvance<struct state *>(sv, next)->head->type = Kill;
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case One: {
      struct pair<int> *p = listAdvance<struct pair<int> *>(pv, next)->head;
      p->a = m->spos;
      p->b = m->epos;
      struct state *s = listAdvance<struct state *>(sv, next)->head;
      s->type = Pass;
      s->i = kont;
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case Dot: {
      struct pair<int> *p = listAdvance<struct pair<int> *>(pv, next)->head;
      struct state *s = listAdvance<struct state *>(sv, next)->head;
      p->a = m->spos;
      p->b = m->epos;
      s->type = Match;
      s->i = kont;
      if((flags & FLAG_DOTALL) != 0) {
        struct llist<struct pair<char> *> *t = new struct llist<struct pair<char> *>;
        t->head = new struct pair<char>;
        t->head->a = '\x00';
        t->head->b = '\x7f';
        t->tail = NULL;
        s->cl = t;
      }
      else if((flags & FLAG_UNIX_LINES) != 0) {
        struct llist<struct pair<char> *> *t = new struct llist<struct pair<char> *>;
        t->head = new struct pair<char>;
        t->head->a = '\x00';
        t->head->b = '\x09';
        t->tail = new struct llist<struct pair<char> *>;
        t->tail->head = new struct pair<char>;
        t->tail->head->a = '\x0b';
        t->tail->head->b = '\x7f';
        t->tail->tail = NULL;
        s->cl = t;
      }
      else {
        struct llist<struct pair<char> *> *t = new struct llist<struct pair<char> *>;
        t->head = new struct pair<char>;
        t->head->a = '\x00';
        t->head->b = '\x09';
        t->tail = new struct llist<struct pair<char> *>;
        t->tail->head = new struct pair<char>;
        t->tail->head->a = '\x0b';
        t->tail->head->b = '\x0c';
        t->tail->tail = new struct llist<struct pair<char> *>;
        t->tail->tail->head = new struct pair<char>;
        t->tail->tail->head->a = '\x0e';
        t->tail->tail->head->b = '\x7f';
        t->tail->tail->tail = NULL;
        s->cl = t;
      }
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case Atom: {
      struct pair<int> *p = listAdvance<struct pair<int> *>(pv, next)->head;
      struct state *s = listAdvance<struct state *>(sv, next)->head;
      p->a = m->spos;
      p->b = m->epos;
      s->type = Match;
      s->i = kont;
      if(e->atom->isList == 0) {
        if((flags & FLAG_NO_CASE) != 0) {
          s->c = new char[20];
          *(s->c) = tolower(e->atom->c);
        }
        else {
          s->cl = new struct llist<struct pair<char> *>;
          s->cl->head = new struct pair<char>;
          s->cl->head->a = e->atom->cls->head->a;
          s->cl->head->b = e->atom->cls->head->b;
          s->cl->tail = NULL;
        }
      }
      else {
        if((flags & FLAG_NO_CASE) != 0) {
          s->cl = new struct llist<struct pair<char> *>;
          s->cl->head = new struct pair<char>;
          s->cl->head->a = tolower(e->atom->cls->head->a);
          s->cl->head->b = tolower(e->atom->cls->head->b);
          s->cl->tail = NULL;
        }
        else {
          s->cl = new struct llist<struct pair<char> *>;
          s->cl->head = new struct pair<char>;
          s->cl->head->a = e->atom->cls->head->a;
          s->cl->head->b = e->atom->cls->head->b;
          s->cl->tail = NULL;
        }
      }
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case Pred: {
      struct pair<int> *p = listAdvance<struct pair<int> *>(pv, next)->head;
      struct state *s = listAdvance<struct state *>(sv, next)->head;
      p->a = m->spos;
      p->b = m->epos;
      s->type = CheckPred;
      s->p = get_pcond(e->pred, flags);
      s->i = kont;
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case Backref: {
      struct pair<int> *p = listAdvance<struct pair<int> *>(pv, next)->head;
      struct state *s = listAdvance<struct state *>(sv, next)->head;
      p->a = m->spos;
      p->b = m->epos;
      s->type = CheckBackref;
      s->pi = new struct pair<int>;
      s->pi->a = e->i;
      s->pi->b = kont;
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case Group: {
      struct pair<int> *p = listAdvance<struct pair<int> *>(pv, next)->head;
      struct state *s = listAdvance<struct state *>(sv, next)->head;
      switch(e->group->type) {
        case CAP: {
          int i = e->group->cap_gid;
          p->a = m->epos;
          p->b = m->epos;
          s->type = EndCap;
          if(!(s->pi))
            s->pi = new struct pair<int>;
          s->pi->a = i;
          s->pi->b = kont;
          struct pair<int> *t = compile(e->group->r1, sv, pv, (next-1), next, flags);
          struct pair<int> *p2 = listAdvance<struct pair<int> *>(pv, t->a)->head;
          struct state *s2 = listAdvance<struct state *>(sv, t->a)->head;
          p2->a = m->epos;
          p2->b = m->epos;
          s2->type = BeginCap;
          if(!(s2->pi))
            s2->pi = new struct pair<int>;
          s2->pi->a = e->group->cap_gid;
          s2->pi->b = t->b;
          struct pair<int> *r = new struct pair<int>;
          r->a = next-1;
          r->b = next;
          return r;
        }
        case MODS: {
          struct pair<int> *r = new struct pair<int>;
          r->a = next;
          r->b = kont;
          return r;
        }
        case NOCAP: {
          int flags2 = (flags | e->group->m_on) & (~(e->group->m_off));
          return compile(e->group->r1, sv, pv, next, kont, flags2);
        }
        //default:
          //TODO: Exceptions
      }
    }
    case Conc: {
      struct pair<int> *p = compile(e->conalt->r2, sv, pv, next, kont, flags);
      return compile(e->conalt->r1, sv, pv, p->a, p->b, flags);
    }
    case Alt: {
      struct pair<int> *pos = compile(e->conalt->r2, sv, pv, next, kont, e->conalt->r1->metadata->cflags);
      struct pair<int> *pos2 = compile(e->conalt->r2, sv, pv, pos->a, kont, e->conalt->r1->metadata->cflags);
      struct pair<int> *p = listAdvance<struct pair<int> *>(pv, pos2->a)->head;
      struct state *s = listAdvance<struct state *>(sv, pos2->a)->head;
      p->a = m->spos;
      p->b = m->epos;
      s->type = BranchAlt;
      if(!(s->pi))
        s->pi = new struct pair<int>;
      s->pi->a = pos2->b;
      s->pi->b = pos->b;
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      delete pos;
      delete pos2;
      return r;
    }
    case Kleene: {
      struct pair<int> *p = listAdvance<struct pair<int> *>(pv, next-1)->head;
      struct state *s = listAdvance<struct state *>(sv, next-1)->head;
      int next_eb, kont_eb;
      if(e->kleene->r->metadata->nullable == 1) {
        p->a = e->kleene->r->metadata->epos;
        p->b = e->kleene->r->metadata->epos;
        s->type = EvalB;
        s->i = next;
        next_eb = next-2;
        kont_eb = next-1;
      }
      else {
        next_eb = next-1;
        kont_eb = next;
      }
      int next_mb, kont_mb;
      struct pair<int> *pos = compile(e->kleene->r, sv, pv, next_eb, kont_eb, flags);
      if(e->kleene->r->metadata->nullable == 1) {
        struct pair<int> *p2 = listAdvance<struct pair<int> *>(pv, pos->a)->head;
        struct state *s2 = listAdvance<struct state *>(sv, pos->a)->head;
        p2->a = e->kleene->r->metadata->spos;
        p2->b = e->kleene->r->metadata->spos;
        s2->type = MakeB;
        s2->i = pos->b;
        next_mb = pos->a-1;
        kont_mb = pos->a;
      }
      else {
        next_mb = pos->a;
        kont_mb = pos->b;
      }
      p->a = m->spos;
      p->b = m->epos;
      s->type = BranchKln;
      s->klnBool = e->kleene->q == Gq;
      s->pi = new struct pair<int>;
      s->pi->a = kont_mb;
      s->pi->b = kont;
      delete pos;
      struct pair<int> *r = new struct pair<int>;
      r->a = next_mb;
      r->b = next;
      return r;
    }
  }
}

struct nfa *make(regex *r, int flags) {
  decorate_regex(r, flags);
  int state_count = r->metadata->scount + 1;
  struct nfa *nfa = new struct nfa;
  //Initialise states
  nfa->states = listInitialise<struct state *>(NULL, state_count);
  struct llist<struct state *> *ts = nfa->states;
  for(int i=0; i < state_count; i++) {
    ts->head = new struct state;
    ts->head->type = End;
    ts = ts->tail;
  }
  //Initialise transitions
  nfa->transitions = listInitialise<struct transition *>(NULL, state_count);
  //Initialise positions
  nfa->positions = listInitialise<struct pair<int> *>(NULL, state_count);
  struct llist<struct pair<int> *> *tp = nfa->positions;
  for(int i; i < state_count; i++) {
    tp->head = new struct pair<int>;
    tp->head->a = r->metadata->epos;
    tp->head->b = r->metadata->epos;
  }
  struct pair<int> *p = compile(r, nfa->states, nfa->positions, (state_count-2), (state_count-1), flags);
  nfa->root = p->b;
  return nfa;
}

struct state *get_state(struct nfa *nfa, int i) {
  return listAdvance<struct state *>(nfa->states, i)->head;
}

struct twople<struct llist<int> *, struct llist<struct transition *> *> *e_get_transitions_explore(int i, struct twople<struct llist<int> *, struct llist<struct transition *> *> *lst, struct nfa *nfa) {
  if(listMem<int>(i, lst->a))
    return lst;
  struct llist<int> *st = intset_add(i, lst->a);
  struct state *swt = get_state(nfa, i);
  switch (swt->type) {
    case Kill:
      return lst;
    case End:
      struct transition *trans = new struct transition;
      trans->a = ZMIN;
      trans->b = ZMAX;
      trans->c = i;
      lst->b = addListNode<struct transition *>(trans, lst->b);
      return lst;
    case Pass:
    case MakeB:
    case EvalB:
      return e_get_transitions_explore(swt->i, lst, nfa);
    case Match:
      struct llist<struct transition *> *itr = lst->b;
      struct llist<struct pair<char> *> *foldlist = swt->cl;
      while(foldlist) {
        struct transition *trans = new struct transition;
        trans->a = foldlist->head->a;
        trans->b = foldlist->head->b;
        trans->c = swt->i;
        itr = addListNode<struct transition *>(trans, itr);
        foldlist = foldlist->tail;
      }
      lst->b = itr;
      return lst;
    case CheckPred:
    case CheckBackref:
      return lst; //Not supported
    case BeginCap:
    case EndCap:
      return e_get_transitions_explore(swt->pi->b, lst, nfa);
    case BranchAlt:
      return e_get_transitions_explore(swt->pi->a, e_get_transitions_explore(swt->pi->b, lst, nfa), nfa);
    case BranchKln:
      if (swt->klnBool)
        return e_get_transitions_explore(swt->pi->b, e_get_transitions_explore(swt->pi->a, lst, nfa), nfa);
      else
        return e_get_transitions_explore(swt->pi->a, e_get_transitions_explore(swt->pi->b, lst, nfa), nfa);
  }
}

struct llist<struct transition *> *get_transitions(struct nfa *nfa, int i) {
  struct llist<struct llist<struct transition *> *> *node = listAdvance<struct llist<struct transition *> *>(nfa->transitions, i);
  if (node->head != NULL)
    return node->head;
  struct twople<struct llist<int> *, struct llist<struct transition *> *> *lst = e_get_transitions_explore(i, makeTwople<struct llist<int> *, struct llist<struct transition *> *>(NULL, NULL), nfa);
  node->head = lst->b;
  delete lst->a;
  delete lst;
  return node->head;
}
