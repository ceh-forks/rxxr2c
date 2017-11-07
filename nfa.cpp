#include<parsingdata.h>
#include<nfa.h>

void decorate_regex(regex r, int flags) {
  exp *e = r->expression;
  meta *m = r->metadata;
  switch(e->type) {
    case Zero || Dot || Atom:
      m->scount = 1;
      m->nullable = 0;
      m->cflags = flags;
      break;
    case One || Pred || Backref:
      m->scount = 1;
      m->nullable = 1;
      m->cflags = flags;
      break;
    case Group:
      switch(m->c->type) {
        MODS:
          int flags2 = (flags | 



