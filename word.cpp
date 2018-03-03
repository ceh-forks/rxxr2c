#include "baselib.hpp"

typedef struct pair<char> crange; //Character range
typedef struct llist<struct pair<char> *> word;

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

llist<char> *word_select_fold(word *w, word *l, llist<char> *acc) {
  if(w == NULL)
    return acc;
  else {
    addListNode<char>(chr_select(w->head, l), acc);
    return word_select_fold(w->tail, l, acc);
  }
}

llist<char> *word_select(word *w, word *l) {
  llist<char> *t = new llist<char>;
  t = NULL;
  return word_select_fold(w, l, t);
}

/*char *word_print(llist<char *> *l, word *w) {
  if (u == v)

}
char *word_print_select(word);*/