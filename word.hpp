#include "baselib.hpp"

typedef struct pair<char> crange; //Character range
typedef struct llist<pair<char> *> word; //List of character ranges stored in reverse order

//append a character (or class)
word *word_extend(word *, crange *);
word *word_append(word *, word *);
word *word_suffix(word *, int);

crange *word_intersect(crange *c1, crange *c2);

struct llist<char> *word_select(struct llist<struct pair<char> *> *);
char *word_print(void);
char *word_print_select(word);

