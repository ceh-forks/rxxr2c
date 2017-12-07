#include "baselib.h"

struct cword;

class Word {
  struct llist<pair<char> *> *;

  public:
    short is_empty(void);
    void extend(struct pair<char> *);
    void append(struct word *);
    int length(void);
    struct cword *tail(void);
    Word suffix(int);
    struct llist<char> *select(struct llist<struct pair<char> *> *);
    char *print(void);
    char *print_select(struct llist<struct pair<char> *> *);
};

struct cword {
  struct llist<struct pair<char> *> cl;
  Word t;
};

