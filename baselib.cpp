#include "baselib.hpp"
#include <iostream>

int min(int i, int j) {
  if(i<j) return i;
  else return j;
}

int max(int i, int j) {
  if(i>j) return i;
  else return j;
}

char zprev(char u) {
  if(u <= ZMIN)
    return ZMIN;
  else
    return (u-1);
}

char znext(char u) {
  if(u >= ZMAX)
    return ZMAX;
  else
    return (u+1);
}

//Ignore case of string
void strLower(char *p) {
	for ( ; *p; ++p) *p = tolower(*p);
}
