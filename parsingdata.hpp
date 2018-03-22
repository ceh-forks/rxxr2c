#ifndef PDATA_H
#define PDATA_H

#include "baselib.hpp"

/* Lexing / parsing structures & functions */
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
  short isList; //Either single character (c) or character class
  union {
    char c;
    struct llist<struct pair<char> *> *cls;
  };
};

//predicate expressions - anchors
enum predtype {Bol, //beginning of line
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
    struct atom *atom;
    enum predtype pred;
    struct group *group;
    struct kleene *kleene;
    struct conalt *conalt;
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
struct llist<struct pair<char> *> *ctr_positive(struct ctr); //read out character class
struct llist<struct pair<char> *> *ctr_negative(struct ctr); //read out inverse of character class

//tree to store sorted character classes
struct ctr {
  char a;
  char b;
  struct ctr *lt;
  struct ctr *rt;
};

struct exp *makeZero();
struct exp *makeOne();
struct exp *makeDot();
struct exp *makePred(enum predtype);
struct exp *makeAtomChar(char);
struct exp *makeAtomClass(llist<pair<char> *> *);
struct exp *makeGroup(enum gkind, int, int, int, struct regex *);
struct exp *makeBackref(int);
struct exp *makeConc(struct regex *, struct regex *);
struct exp *makeAlt(struct regex *, struct regex *);
struct exp *makeKleene(enum qfier, struct regex *);

#endif
