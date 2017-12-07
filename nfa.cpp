#include<ctype.h>
#include "parsingdata.h"
#include "nfa.h"

struct pred *get_pcond(enum predt p, int flags) {
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
      switch(e->gk->type) {
        case MODS:
          flags2 = (flags | e->gk->m_on) & (!e->gk->m_off);
          m->scount = 0;
          m->nullable = 1;
          m->cflags = flags2;
          break;
        case CAP:
          decorate_regex(e->gk->r1, flags);
          m->scount = 2 + e->gk->r1->metadata->scount;
          m->nullable = e->gk->r1->metadata->nullable;
          m->cflags = flags;
          break;
        case NOCAP:
          flags2 = (flags | e->gk->m_on) & (!e->gk->m_off);
          decorate_regex(e->gk->r1, flags2);
          m->scount = e->gk->r1->metadata->scount;
          m->nullable = e->gk->r1->metadata->nullable;
          m->cflags = flags;
        //default:
          //TODO Unsupported grouping construct
      }
      break;
    case Conc:
      decorate_regex(e->ca->r1, flags);
      decorate_regex(e->ca->r2, e->ca->r1->metadata->cflags);
      m->scount = 1 + e->ca->r1->metadata->scount + e->ca->r2->metadata->scount;
      m->nullable = e->ca->r1->metadata->nullable && e->ca->r2->metadata->nullable;
      m->cflags = e->ca->r2->metadata->cflags;
      break;
    case Alt:
      decorate_regex(e->ca->r1, flags);
      decorate_regex(e->ca->r2, e->ca->r1->metadata->cflags);
      m->scount = 1 + e->ca->r1->metadata->scount + e->ca->r2->metadata->scount;
      m->nullable = e->ca->r1->metadata->nullable || e->ca->r2->metadata->nullable;
      m->cflags = e->ca->r2->metadata->cflags;
      break;
    case Kleene:
      int n;
      decorate_regex(e->kl->r, flags);
      if(e->kl->r->metadata->nullable == 1)
        n = 3;
      else
        n = 1;
      m->scount = n + e->kl->r->metadata->scount;
      m->nullable = 1;
      m->cflags = e->kl->r->metadata->cflags;
      break;
  }
}

//TODO: make sure arrays are fully initialised

struct pair<int> *compile(struct regex *r, struct state *sv[], struct pair<int> *pv[], int next, int kont, int flags) {
  exp *e = r->expression;
  meta *m = r->metadata;
  switch(e->type) {
    case Zero: {
      pv[next]->a = m->spos;
      pv[next]->b = m->epos;
      sv[next]->type = Kill;
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case One: {
      pv[next]->a = m->spos;
      pv[next]->b = m->epos;
      sv[next]->type = Pass;
      sv[next]->i = kont;
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case Dot: {
      pv[next]->a = m->spos;
      pv[next]->b = m->epos;
      sv[next]->type = Match;
      sv[next]->i = kont;
      if((flags & FLAG_DOTALL) != 0) {
        struct llist<struct pair<char> *> *t = new struct llist<struct pair<char> *>;
        (*t).head = new struct pair<char>;
        t->head->a = '\x00';
        t->head->b = '\x7f';
        t->tail = NULL;
        sv[next]->cl = t;
      }
      else if((flags & FLAG_UNIX_LINES) != 0) {
        struct llist<struct pair<char> *> *t = new struct llist<struct pair<char> *>;
        (*t).head = new struct pair<char>;
        t->head->a = '\x00';
        t->head->b = '\x09';
        (*t).tail = new struct llist<struct pair<char> *>;
        (*t).tail->head = new struct pair<char>;
        t->tail->head->a = '\x0b';
        t->tail->head->b = '\x7f';
        t->tail->tail = NULL;
        sv[next]->cl = t;
      }
      else {
        struct llist<struct pair<char> *> *t = new struct llist<struct pair<char> *>;
        (*t).head = new struct pair<char>;
        t->head->a = '\x00';
        t->head->b = '\x09';
        (*t).tail = new struct llist<struct pair<char> *>;
        (*t).tail->head = new struct pair<char>;
        t->tail->head->a = '\x0b';
        t->tail->head->b = '\x0c';
        (*t).tail->tail = new struct llist<struct pair<char> *>;
        (*t).tail->tail->head = new struct pair<char>;
        t->tail->tail->head->a = '\x0e';
        t->tail->tail->head->b = '\x7f';
        t->tail->tail->tail = NULL;
        sv[next]->cl = t;
      }
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case Atom: {
      pv[next]->a = m->spos;
      pv[next]->b = m->epos;
      sv[next]->type = Match;
      sv[next]->i = kont;
      if(e->at->isList == 0) {
        if((flags & FLAG_NO_CASE) != 0) {
          sv[next]->c = new char[20];
          sv[next]->c = tolower(e->at->c1);
        }
        else {
          sv[next]->cl = new struct llist<struct pair<char> *>;
          sv[next]->cl->head = new struct pair<char>;
          sv[next]->cl->head->a = e->at->c1;
          sv[next]->cl->head->b = e->at->c1;
          sv[next]->cl->tail = NULL;
        }
      }
      else {
        if((flags & FLAG_NO_CASE) != 0) {
          sv[next]->cl = new struct llist<struct pair<char> *>;
          sv[next]->cl->head = new struct pair<char>;
          sv[next]->cl->head->a = tolower(e->at->c1);
          sv[next]->cl->head->b = tolower(e->at->c2);
          sv[next]->cl->tail = NULL;
        }
        else {
          sv[next]->cl = new struct llist<struct pair<char> *>;
          sv[next]->cl->head = new struct pair<char>;
          sv[next]->cl->head->a = e->at->c1;
          sv[next]->cl->head->b = e->at->c2;
          sv[next]->cl->tail = NULL;
        }
      }
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case Pred: {
      pv[next]->a = m->spos;
      pv[next]->b = m->epos;
      sv[next]->type = CheckPred;
      sv[next]->p = get_pcond(e->pr, flags);
      sv[next]->i = kont;
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case Backref: {
      pv[next]->a = m->spos;
      pv[next]->b = m->epos;
      sv[next]->type = CheckBackref;
      sv[next]->pi = new struct pair<int>;
      sv[next]->pi->a = e->i;
      sv[next]->pi->b = kont;
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      return r;
    }
    case Group: {
      switch(e->gk->type) {
        case CAP: {
          int i = e->gk->cap_gid;
          pv[next]->a = m->epos;
          pv[next]->b = m->epos;
          sv[next]->type = EndCap;
          if(!(sv[next]->pi))
            sv[next]->pi = new struct pair<int>;
          sv[next]->pi->a = i;
          sv[next]->pi->b = kont;
          struct pair<int> *t = compile(e->gk->r1, sv, pv, (next-1), next, flags);
          pv[t->a]->a = m->epos;
          pv[t->a]->b = m->epos;
          sv[t->a]->type = BeginCap;
          if(!sv[t->a]->pi)
            sv[t->a]->pi = new struct pair<int>;
          sv[t->a]->pi->a = e->gk->cap_gid;
          sv[t->a]->pi->b = t->b;
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
          int flags2 = (flags | e->gk->m_on) & (~(e->gk->m_off));
          return compile(e->gk->r1, sv, pv, next, kont, flags2);
        }
        //default:
          //TODO: Exceptions
      }
    }
    case Conc: {
      struct pair<int> *p = compile(e->ca->r2, sv, pv, next, kont, flags);
      return compile(e->ca->r1, sv, pv, p->a, p->b, flags);
    }
    case Alt: {
      struct pair<int> *p = compile(e->ca->r2, sv, pv, next, kont, e->ca->r1->metadata->cflags);
      //TODO memory management
      return compile(e->ca->r1, sv, pv, p->a, p->b, flags);
    case Alt:
      struct pair<int> *p = compile(e->ca->r2, sv, pv, next, kont, e->ca->r1->metadata->cflags);
      struct pair<int> *p2 = compile(e->ca->r2, sv, pv, p->a, kont, e->ca->r1->metadata->cflags);
      pv[p2->a]->a = m->spos;
      pv[p2->a]->b = m->epos;
      sv[p2->a]->type = BranchAlt;
      if(!(sv[p2->a]->pi))
        sv[p2->a]->pi = new struct pair<int>;
      sv[p2->a]->pi->a = p2->b;
      sv[p2->a]->pi->b = p->b;
      struct pair<int> *r = new struct pair<int>;
      r->a = next-1;
      r->b = next;
      delete p;
      delete p2;
      return r;
    }
    case Kleene: {
      int next_eb, kont_eb;
      if(e->kl->r->metadata->nullable == 1) {
        pv[next-1]->a = e->kl->r->metadata->epos;
        pv[next-1]->b = e->kl->r->metadata->epos;
        sv[next-1]->type = EvalB;
        sv[next-1]->i = next;
        next_eb = next-2;
        kont_eb = next-1;
      }
      else {
        next_eb = next-1;
        kont_eb = next;
      }
      int next_mb, kont_mb;
      struct pair<int> p = compile(e->kl->r, sv, pv, next_eb, kont_eb, flags);
      if(e->kl->r->metadata->nullable == 1) {
        pv[p->a]->a = e->kl->r->metadata->spos;
        pv[p->a]->b = e->kl->r->metadata->spos;
        sv[p->a]->type = MakeB;
        sv[p->a]->i = p->b;
        next_mb = p->a-1;
        kont_mb = p->a;
      }
      else {
        next_mb = p->a;
        kont_mb = p->b;
      }
      pv[next]->a = m->spos;
      pv[next]->b = m->epos;
      sv[next]->type = BranchKln;
      sv[next]->klnBool = e->kl->qf == Gq;
      sv[next]->pi = new struct pair<int>;
      sv[next]->pi->a = kont_mb;
      sv[next]->pi->b = kont;
      delete p;
      struct pair<int> *r = new struct pair<int>;
      r->a = next_mb;
      r->b = next;
      return r;
    }
  }
}

template <int i>
struct t<i> make(regex *r, int flags) {
  decorate_regex(r, flags);
  int state_count = r->metadata->scount + 1;
  struct t<state_count> *r = new struct t<state_count>;
  struct state *sv[] = r->states;
  for(int i; i < state_count; i++) {
    sv[state_count] = new struct state;
    sv[state_count]->type = End;
  }
  struct transition *tv[] = r->transitions;
  for(int i; i < state_count; i++) {
    tv[state_count] = NULL;
  }
  struct pair<int> *pv[] = r->positions;
  for(int i; i < state_count; i++) {
    pv[state_count] = new struct pair<int>;
    pv[state_count]->a = r->metadata->epos;
    pv[state_count]->b = r->metadata->epos;
  }
  struct pair<int> *p = compile(r, sv, pv, (state_count-2), (state_count-1), flags);
  r->root = p->b;
  return r;
}

int main(void) {
  return 0;
}
