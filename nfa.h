enum prede {P_BOI, P_EOI, P_EOIX, P_BOL, P_EOL, P_EOM, P_WB, P_NWB};

struct pred {
  enum prede type;
  short value;
};

enum statet {End, Kill, Pass, MakeB, EvalB, Match, CheckPred, CheckBackref, BeginCap, EndCap, BranchAlt, BranchKln};

struct state {
  enum statet type;
  int i;
  short klnBool;
  union {
    struct pair<int> *pi;
    struct llist<struct pair<char> *> *cl;
    struct pred p;
    char *c;
  };
};

struct transition {
  char a;
  char b;
  int c;
};

struct position {
  int a;
  int b;
};

template <int i>
struct t {
  struct state *states[i];
  struct transition *transitions[i];
  struct position *positions[i];
  int root;
};

template <int i>
struct t<i> make(struct pattern);
//int root(struct t);
//int size(struct t);
//struct llist<transition> get_state(struct t, int);
//struct position get_subexp_location(struct t, int);

