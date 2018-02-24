%{

struct range {
  int m;
  int n;
  enum qfier qf;
};

#include "parsingdata.hpp"

make_seq(int r, int n, struct pair<int> *cpos) {
  if (n == 1)
    return r;
  else if (n > 1)
    return make_r(makeConc(r, make_seq(r, n-1, cpos)), cpos);
  else
    //TODO: Error handling
}

make_greedy_zton(int r, int n, int cpos) {
  if (n == -1)
    return make_r(makeKleene(Gq, r), cpos);
  else if (n == 1)
    return make_r(makeAlt(r, make_r(makeOne(), cpos)), cpos);
  else if (n > 1)
    return make_r(makeAlt(make_seq(r, n, cpos), make_greedy_zton(r, n-1, cpos)), cpos);
  else
    //TODO: Error handling
}

make_reluctant_zton(int r, int n, int cpos) {
  if (n == -1)
    return make_r(makeKleene(Rq, r), cpos);
  else if (n == 1)
    return make_r(makeAlt(make_r(makeOne(), cpos), r), cpos);
  else if (n > 1)
    return make_r(makeAlt(make_reluctant_zton(r, n-1, cpos), make_seq(r, n, cpos)), cpos);
  else
    //TODO: Error handling
}

make_range(int r, struct range *rng, struct pair<int> *cpos) {
  if (rng->m == 0) {
    if (rng->n == 0)
      return make_r(makeOne(), cpos);
    else if (rng->n == -1)
      return make_r(makeKleene(qf, r), cpos);
    else if (rng->qf == Gq)
      return make_greedy_zton(r, n, cpos);
    else if (rng->qf == Rq)
      return make_reluctant_zton(r, n, cpos);
    else
      //TODO: Error handling
  }
  else if (rng->n == -1) {
    struct range *t;
    t = new struct range;
    t->m = 0;
    t->n = -1;
    t->qf = rng->qf;
    struct regex *r = make_r(makeConc(make_seq(r, m, cpos), make_range(r, t, cpos)), cpos);
    delete t;
    return r;
  }
  else if (rng->m == rng->n)
    return make_seq(r, m, cpos);
  else if (rng->m < rng->n) {
  struct range *t;
    t = new struct range;
    t->m = 0;
    t->n = rng->n - rng->m;
    t->qf = rng->qf;
    struct regex *r = make_r(makeConc(make_seq(r, m, cpos), make_range(r, t, cpos)), cpos);
    delete t;
    return r;
  }
  else
    //TODO: Error handling
}

%}

%token <(int * int) * char> Literal
%token <(int * int) * ParsingData.pred> Anchor
%token <int * ParsingData.gkind> GrpOpen
%token <(int * int)> BeginQuote EndQuote
%token <int> TkDot ModsGrpOpen Mod GrpClose ClsClose 
%token <(int * int) * int> TkBackref
%token <int * bool> ClsOpen
%token <char * char> ClsRange
%token <(int * int) * ((char * char) list)> ClsNamed
%token <int * (int * int * ParsingData.qfier)> Repetition
%token VBar NegMods EndMods Eos

%start parse
%%

parse: Eos { make_r(makeOne(), 0, 0); } 
  |expr Eos { $1 };

expr: conc { $1 } 
  |conc VBar expr { make_r(makeAlt ($1, $3), r_spos($1), r_epos($3)); }

conc: factor { $1 }
  |factor conc { make_r(makeConc($1, $2) r_spos($1), r_epos($2)); }

factor: atom { $1 }
  |factor Repetition { make_range($1, $2->b, makePair<int>(r_spos($1), $2->a)); }

atom: literal { $1 }
  |Anchor { make_r(makePred($1->b), $1->a->a, $1->a->b); }
  |TkDot { make_r(makeDot(), $1, $1); }
  |BeginQuote EndQuote { make_r(makeOne(), $1->a, $2->b); }
  |BeginQuote quote_body EndQuote { $2 }
  |GrpOpen GrpClose { make_r(makeGroup($1->b, 0, 0, make_r(makeOne ($1->a, $2))), $1->a, $2); }
  |GrpOpen expr GrpClose { make_r(makeGroup($1->b, 0, 0, $2), $1->a, $3); }
  |TkBackref { make_r(makeBackref($1->b), $1->a); }
  |ModsGrpOpen mods GrpClose { make_r(makeGroup(MODS, $2->a, $2->b, make_r(makeOne(), $1, $3)), $1, $3); }
  |ModsGrpOpen mods EndMods GrpClose { make_r(makeOne(), $1, $4); }
  |ModsGrpOpen mods EndMods expr GrpClose { make_r(makeGroup(NOCAP, $2->a, $2->b, $4), $1, $5); }
  |ClsOpen ch_range_list ClsClose {
      if ($1->b)
        make_r(makeAtomClass(ctr_negative($2)), $1->a, $3);
      else 
        make_r(makeAtomClass(ctr_positive($2)), $1->a, $3);
  }
  |ClsNamed { make_r(makeAtomClass($1->b), $1->a->a, $1->a->b); }

quote_body: literal { $1 }
  | literal quote_body { make_r(makeConc($1, $2), r_spos $1, r_epos $2);  }

literal: Literal { make_r(makeAtomChar($1->b), $1->a->a, $1->a->b); }

mods: mod_list { ($1, 0) }
  |mod_list NegMods mod_list { ($1, $3) }

mod_list: { 0 }
  |Mod mod_list { $1 || $2 }

ch_range_list: ch_range { ctr_add_cls CTNull $1 }
  |ch_range ch_range_list { ctr_add_cls $2 $1 }

ch_range: Literal { [($1->b, $1->b)] }
  |ClsRange { [$1] }
  |ClsNamed { $1->b }

