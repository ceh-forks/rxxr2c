enum prede {P_BOI, P_EOI, P_EOIX, P_BOL, P_EOL, P_EOM, P_WB, P_NWB};

typedef struct pred {
  enum prede type;
  short value;
} pred;

enum statee {End, Kill, Pass, MakeB, EvalB, Match, CheckPred, CheckBackref, BeginCap, EndCap, BranchAlt, BranchKln};

typedef struct state {
  enum statee type;
  int *list;
} state;

typedef struct transition {
  char a;
  char b;
  int c;
} transition;

typedef struct position {
  int a;
  int b;
} position;

typedef struct t {
  llist<state> states;
  llist<transition> transitions;
  llist<position> positions;
  int root;
} t;

t make(

