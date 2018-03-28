#include <iostream>
#include <cassert>
#include "baselib.hpp"
#include "parsingdata.hpp"
#include "nfa.hpp"
#include "flags.hpp"

int main() {
  std::cout << "ParsingData test: ";
  struct regex *r = make_r(makeAtomChar('c'), ZMAX, ZMAX);
  assert(r->metadata->spos == ZMAX);
  struct ctr *ctr = make_ctr(2, 4, NULL, NULL);
  ctr = ctr_add(ctr, 'x', 'v');
  assert(ctr->rt->a == 'x');
  std::cout << "Passed\n";
  std::cout << "nfa test: ";
  
  return 0;
}
