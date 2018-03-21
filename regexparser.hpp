#include "baselib.hpp"

enum regextokentype {Literal,
  Anchor,
  GrpOpen,
  BeginQuote,
  EndQuote,
  TkDot,
  ModsGrpOpen,
  Mod,
  GrpClose,
  ClsClose,
  TkBackref,
  ClsOpen,
  ClsRange,
  ClsNamed,
  Repetition,
  VBar,
  NegMods,
  EndMods,
  Eos
};

struct regextoken {
  enum regextokentype type;
  union {
    struct literal *literal;
    struct anchor *anchor;
    struct grpopen *grpopen;
    struct pair<int> *beginquote;
    struct pair<int> *endquote;
    int *tkdot;
    int modsgrpopen;
    int mod;
    int grpclose;
    int clsclose;
    struct tkbackref *tkbackref;
    struct twople<int, bool> *clsopen;
    struct pair<char> *clsrange;
    struct clsnamed *clsnamed;
    struct repetition *repetition;
  };
};
