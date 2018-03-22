#ifndef FLAGS_H
#define FLAGS_H

#define EMPTY 0
#define INTERRUPTED 1
#define ACCEPTING 2
#define KLNHIT 4
#define EOIHIT 8
#define PRUNED 16

int is_empty(int);
int is_interrupted(int);
int is_accepting(int);
int is_klnhit(int);
int is_eoihit(int);
int is_pruned(int);

int set_empty(int);
int set_interrupted(int);
int set_accepting(int);
int set_klnhit(int);
int set_eoihit(int);
int set_pruned(int);

int uni(int, int);

#endif
