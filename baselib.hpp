/* Standard general functions, structures and constants */
#define NULL 0
//First/last character of alphabet in ASCII
#define ZMIN '\x00'
#define ZMAX '\x7f'

//Fairly self-explanitory.
//You press the on button to turn it on. You can figure out the rest of it yourself
int min(int, int);
int max(int, int);

//Calculate previous/next character in ASCII
char zprev(char);
char znext(char);

void strLower(char *);

//Linked list
template<typename T>
struct llist {
  T head;
  struct llist<T> *tail;
};

template<typename T>
struct llist<T> *addListNode(T, struct llist<T> *);

//Creates a new list which is a copy of a given one
template<typename T>
struct llist<T> *listCpy(llist<T> *list);

//Pair of a type
template<typename T>
struct pair {
  T a;
  T b;
};

template <typename T>
struct btree {
  T node;
  struct btree *lt;
  struct btree *rt;
};

template <typename T>
struct btree<T> *makeTree(T, struct btree<T> *, struct btree<T> *);

template <typename T>
struct llist<T> *llappend(struct llist<T> *, struct llist<T> *);

template <typename T>
struct pair<T> *makePair(T i, T j);

//Returns a copy of a list
template<typename T>
struct llist<T> *listCpy(struct llist<T> *);

//Returns a copy of a btree
template<typename T>
struct btree<T> *treeCpy(struct btree<T> *);
