/* Lexing / parsing structures & functions */
#include "baselib.hpp"
//Flags for regex lexer
#define FLAG_UNIX_LINES 1
#define FLAG_NO_CASE 2
#define FLAG_MULTILINE 4
#define FLAG_DOTALL 8

//parsing metadata
struct meta {
  int spos;
  int epos;
  int scount; //no. of states required
  short nullable;
  int cflags;
};

enum expe {Zero, //Null
  One, //empty
  Dot, //any
  Pred, //predicate
  Atom, //atom
  Group, //group
  Backref, //backreference
  Conc, //concatenation
  Alt, //alternation
  Kleene //repetition
};

//expressions matching just a single character
struct atom {
  short isList; //Either single character (c1) or character class
  char c1;
  char c2;
  struct atom *next;
};

//predicate expressions - anchors
enum predt {Bol, //beginning of line
  Eol, //end of line
  Wordb, //word boundary
  NWordb, //not word boundary
  Boi, //beginning of input
  Eom, //end of previous match
  Eoi1, //end of input except final terminator
  Eoi2 //end of input
};

//grouping construct
enum gkind {CAP, //capturing group with group id
  NOCAP, //non-capturing group
  MODS, //inline modifiers
  PLA, //positive look-ahead
  NLA, //negative look-ahead
  PLB, //positive look-behind
  NLB, //negatve look-behind
  ATOMIC //atomic group
};

struct regex;

struct group {
  enum gkind type;
  int m_on;
  int m_off;
  int cap_gid; //Group ID for capturing group
  struct regex *r1;
};

enum qfier {Gq, Rq}; //Greedy, Reluctant

struct kleene {
  enum qfier q;
  struct regex *r;
};

struct conalt {
  struct regex *r1;
  struct regex *r2;
};

//expression
struct exp {
  enum expe type;
  union {
    int i;
    struct atom *at;
    enum predt pr;
    struct group *gk;
    struct kleene *kl;
    struct conalt *ca;
  };
};

//Regex = expression + metadata
struct regex {
  struct exp *expression;
  struct meta *metadata;
};

//Pattern = regex + global modifiers
struct pattern {
  struct regex *expression;
  int flags;
};

struct regex *make_r(struct exp *, int, int); //create regex with default metadata
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

