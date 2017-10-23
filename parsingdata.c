#include<stdlib.h>
#include<stdio.h>
#include "parsingdata.h"

struct meta *make_r(int i, int j) {
	struct meta *r;
	r = malloc(sizeof(struct meta));
	r->spos = i;
	r->epos = j;
	r->scount = 0;
	r->nullable = 0;
	return r;
}

int main() {
	printf("Is it working?\n");
	return 0;
}
