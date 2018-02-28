#include "flags.hpp"
#include<stdio.h>

int is_empty(int f) {
  return (f==0);
}
int is_interrupted(int f) {
  return (f & INTERRUPTED);
}
int is_accepting(int f) {
  return (f & ACCEPTING);
}
int is_klnhit(int f) {
  return (f & KLNHIT);
}
int is_eoihit(int f) {
  return (f & EOIHIT);
}
int is_pruned(int f) {
  return (f & PRUNED);
}

int set_interrupted(int f) {
  return (f | INTERRUPTED);
}
int set_accepting(int f) {
  return (f | ACCEPTING);
}
int set_klnhit(int f) {
  return (f | KLNHIT);
}
int set_eoihit(int f) {
  return (f | EOIHIT);
}
int set_pruned(int f) {
  return (f | PRUNED);
}

int uni(int f1, int f2) {
  return (f1 | f2);
}

