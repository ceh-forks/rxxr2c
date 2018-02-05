#include "baselib.hpp"

typedef struct pair<char> crange; //Character range
typedef struct llist<pair<char> *> word;

word *word_extend(word *w, crange *cc) {
  return addListNode<crange *>(cc, w);
}

word *word_append(word *w1, word *w2) {
  return listAppend<crange *>(w2, w1);
}

word *word_suffix2(word *w, int flen, int i) {
  if (i == flen)
    return NULL;
  else if (w && w->tail) {
    w->tail = word_suffix2(w->tail, flen, i-1);
  }
  else
    return NULL;
}

word *word_suffix(word *w, int flen) {
  int tlen = listLength(w);
  if (flen >= tlen)
    return NULL;
  else
    return word_suffix2(w, flen, tlen);
}

//Calculate the intersection of two ranges
crange *word_intersect(crange *c1, crange *c2) {
  if(c1->b < c2->a || c2->b < c1->a)
    return NULL;
  else
    return makePair<char>(max(c1->a, c2->a), min(c1->b, c2->b));
}

//Select a character in the preferred range if possible
char chr_select(crange *c, word *w) {
  if(w == NULL)
    return c->a;
  else {
    crange *t = word_intersect(c, w->head);
    if (t == NULL)
      return chr_select(c, w->tail);
    else {
      char r = t->a;
      delete t;
      return r;
    }
  }
}
char *word_print(void);
char *word_print_select(word);