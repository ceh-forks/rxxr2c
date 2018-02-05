#include "baselib.hpp"

struct cword;

typedef struct pair<char> crange; //Character range
typedef struct llist<pair<char> *> word; //List of character ranges stored in reverse order

//check for empty word
short word_is_empty(void);
//append a character (or class)
void word_extend(crange *);
void word_append(word *, word *);
int word_length(word *);
struct cword *word_tail(void);
word *word_suffix(word *, int);
struct llist<char> *word_select(struct llist<struct pair<char> *> *);
char *word_print(void);
char *word_print_select(word);


struct cword {
  struct llist<struct pair<char> *> cl;
  word t;
};
