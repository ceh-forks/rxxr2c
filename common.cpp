#include<cctype>

#define ZMIN '\x00'
#define ZMAX '\x7f'

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

