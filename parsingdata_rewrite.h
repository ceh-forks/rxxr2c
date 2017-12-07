/* Lexing / parsing structures & functions */
#include "baselib.h"
//Flags for regex lexer
#define FLAG_UNIX_CASE 1;
#define FLAG_NO_CASE 2;
#define FLAG_MULTILINE 4;
#define FLAG_DOTALL 8;

//parsing metadata
struct meta {
  int spos;
  int epos;
  int scount; //no. of states required
  bool nullable;
  int cflags;
};

struct regex;

enum pred {Bol, //beginning of line
  Eol, //end of line
  Wordb, //word boundary
  NWordb, //not word boundary
  Boi, //beginning of input
  Eom, //end of previous match
  Eoi1, //end of input except final terminator
  Eoi2 //end of input
};
enum gkind {CAP, //capturing group with group id
  NOCAP, //non-capturing group
  MODS, //inline modifiers
  PLA, //positive look-ahead
  NLA, //negative look-ahead
  PLB, //positive look-behind
  NLB, //negatve look-behind
  ATOMIC //atomic group
};
enum qfier {Gq, Rq}; //Greedy, Reluctant

//expression
class exp {
  public:
    static void decorate_regex(int flags);
};
class exp_zero : public exp {};
class exp_one : public exp {};
//predicate expressions - anchors
class exp_pred : public exp {
  public:
    enum prede type;
};
//expressions matching just a single character
class exp_atom : public exp {
  public:
    bool isClass; //Either single character (c) or character class (list)
    union {
      char c;
      struct llist<char> *list;
    };
};
//grouping construct
class exp_group : public exp {
  public:
    enum gkind type;
    int m_on;
    int m_off;
    int cap_gid; //Group ID for capturing group
};
class exp_backref : public exp {
  public:
    int i;
}
class exp_concalt : public exp {
  public:
    regex r1;
    regex r2;
}
class exp_kleene : public exp {
  public:
    enum qfier type;
    regex r;
}

//Regex = expression + metadata
struct regex {
  exp *expression;
  struct meta *metadata;
};

//Pattern = regex + global modifiers
struct pattern {
  struct regex *expression;
  int flags;
};

struct regex *make_r(exp *, int, int); //create regex with default metadata
struct ctr *make_ctr(int, int); //initialise default ctr
struct ctr *ctr_add(struct ctr, char, char); //insert new range into character class
struct llist<pair<char> > *ctr_positive(struct ctr); //read out character class
struct llist<pair<char> > *ctr_negative(struct ctr); //read out inverse of character class

//tree to store sorted character classes
struct ctr {
  char a;
  char b;
  struct ctr *lt;
  struct ctr *rt;
};

