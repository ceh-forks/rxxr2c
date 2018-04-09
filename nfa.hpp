#ifndef NFA_H
#define NFA_H

#include "baselib.hpp"

enum prede {P_BOI, P_EOI, P_EOIX, P_BOL, P_EOL, P_EOM, P_WB, P_NWB};

struct pred {
  enum prede type;
  short value;
};

enum statetype {End, Kill, Pass, MakeB, EvalB, Match, CheckPred, CheckBackref, BeginCap, EndCap, BranchAlt, BranchKln};

struct state {
  enum statetype type;
  int i;
  short klnBool;
  union {
    struct pair<int> *pi;
    struct llist<struct pair<char> *> *cl;
    struct pred *p;
    char *c;
  };
};

struct transition {
  char a;
  char b;
  int c;
};

struct nfa {
  struct llist<struct state *> *states;
  struct llist<struct llist<struct transition *> *> *transitions;
  struct llist<struct pair<int> *> *positions;
  int root;
};

struct state *get_state(struct nfa *, int);
struct nfa *make(struct regex *, int);
void delete_nfa(struct nfa *);
//int root(struct t);
//int size(struct t);
//struct llist<transition> get_state(struct t, int);
//struct position get_subexp_location(struct t, int);

struct llist<struct transition *> *get_transitions(struct nfa *nfa, int i);

#endif
