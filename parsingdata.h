struct meta {
	int spos;
	int epos;
	int scount;
	short nullable;
	int cflags;
};

enum expe {Zero, One, Dot, Pred, Atom, Group, Backref, Conc, Alt, Kleene};

struct atom {
	short isList;
	char c1;
	char c2;
	struct atom *next;
};

enum pred {Bol, Eol, Wordb, NWordb, Boi, Eom, Eoi1, Eoi2};

enum gkinde {CAP, NOCAP, MODS, PLA, NLA, PLB, NLB, ATOMIC};

struct gkind {
	enum gkinde type;
	int cap;
};

enum qfier {Gq, Rq};

struct exp {
	enum expe type;
	union {
		struct atom a;
		enum pred b;
		struct gkind c;
		enum qfier d;
	};
};

struct meta *make_r(int i, int j);

enum ctre {CTNull, CTNode};

struct ctr {
	enum ctre type;
	char a;
	char b;
	struct ctr *c;
	struct ctr *d;
};
	
