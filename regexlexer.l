%{
#include "parsingdata.hpp"
#include "error.hpp"
#include "regexparser.hpp"
#include<cstring>
#include<regex>

// resolve pre-define character classes into struct llist<struct pair<char> *> * representation
struct llist<struct pair<char> *> *resolve_class(char *cls_str_o) {
  //Makes sure string is null-terminated for strcmp to work
  //TODO: is this neccessary?
  char cls_str[strlen(cls_str_o)];
  strcpy(cls_str, cls_str_o);
  if (strcmp(cls_str, "\\d") == 0 || strcmp(cls_str, "\\p{Digit}") == 0)
    return addListNode<struct pair<char> *>(makePair<char>('\x30', '\x39'), NULL);
  else if (strcmp(cls_str, "\\D") == 0) {
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x3a', '\x7f'), NULL);
    return addListNode<struct pair<char> *>(makePair<char>('\x00', '\x2f'), r);
  }
  else if (strcmp(cls_str, "\\s") == 0 || strcmp(cls_str, "\\p{Space}") == 0) {
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x20', '\x20'), NULL);
    return addListNode<struct pair<char> *>(makePair<char>('\x09', '\x0d'), r);
  }
  else if (strcmp(cls_str, "\\S") == 0) {
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x21', '\x7f'), NULL);
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x0e', '\x1f'), r);
    return addListNode<struct pair<char> *>(makePair<char>('\x00', '\x08'), r);
  }
  else if (strcmp(cls_str, "\\w") == 0) {
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x61', '\x7a'), NULL);
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x5f', '\x5f'), r);
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x41', '\x5a'), r);
    return addListNode<struct pair<char> *>(makePair<char>('\x30', '\x39'), r);
  }
  else if (strcmp(cls_str, "\\W") == 0) {
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x7b', '\x7f'), NULL);
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x60', '\x60'), r);
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x5b', '\x5e'), r);
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x3a', '\x40'), r);
    return addListNode<struct pair<char> *>(makePair<char>('\x00', '\x2f'), r);
  }
  else if (strcmp(cls_str, "\\p{Lower}") == 0)
    return addListNode<struct pair<char> *>(makePair<char>('\x61', '\x7a'), NULL);
  else if (strcmp(cls_str, "\\p{Upper}") == 0)
    return addListNode<struct pair<char> *>(makePair<char>('\x41', '\x5a'), NULL);
  else if (strcmp(cls_str, "\\p{ASCII}") == 0)
    return addListNode<struct pair<char> *>(makePair<char>('\x00', '\x7f'), NULL):
  else if (strcmp(cls_str, "\\p{Alpha}") == 0) {
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x61', '\x7a'), NULL);
    return addListNode<struct pair<char> *>(makePair<char>('\x41', '\x5a'), r);
  }
  else if (strcmp(cls_str, "\\p{Alnum}") == 0) {
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x61', '\x7a'), NULL);
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x41', '\x5a'), r);
    return addListNode<struct pair<char> *>(makePair<char>('\x30', '\x39'), r);
  }
  else if (strcmp(cls_str, "\\p{Punct}") == 0) {
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x7b', '\x7e'), NULL);
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x5b', '\x60'), r);
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x3a', '\x40'), r);
    return addListNode<struct pair<char> *>(makePair<char>('\x21', '\x2f'), r);
  }
  else if (strcmp(cls_str, "\\p{Graph}") == 0 || strcmp(cls_str, "\\p{Print}") == 0)
    return addListNode<struct pair<char> *>(makePair<char>('\x21', '\x7e'), NULL);
  else if (strcmp(cls_str, "\\p{Blank}") == 0) {
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x20', '\x20'), NULL);
    return addListNode<struct pair<char> *>(makePair<char>('\x09', '\x09'), r);
  }
  else if (strcmp(cls_str, "\\p{Cntrl}") == 0) {
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x7f', '\x7f'), NULL);
    return addListNode<struct pair<char> *>(makePair<char>('\x00', '\x1f'), r);
  }
  else if (strcmp(cls_str, "\\p{XDigit}") == 0) {
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x61', '\x66'), NULL);
    struct llist<struct pair<char> *> *r = addListNode<struct pair<char> *>(makePair<char>('\x41', '\x46'), r);
    return addListNode<struct pair<char> *>(makePair<char>('\x30', '\x39'), r);
  }
  else
    raise(InternalLexingError);
}

//disallow non-ascii characters
char resolve_char(char c, int spos) {
  if (c < '\x00' || c > '\x7f')
    raise(1); //NonAsciiInput(spos, c);
  else
    return c;
}

//handle character escapes and numerals
char resolve_literal(char *lit_str, int spos) {
  regex lit_switch = regex("[\]x\|[\]0\|[\]c\|[\].\|."); //TODO: conversion
  cmatch m;
  if (!regex_match(lit_str, m, lit_switch))
    raise(InternalLexingError);
  char *lit_head = m[0];
  char c;
  if (strcmp(lit_head, "\\x") == 0) {
    char *rep[] = regex_replace(lit_str, lit_switch, "0x", regex_constants::format_first_only);
    int code = int_of_string (Str.replace_first lit_switch "0x" lit_str) in
    Char.chr code
  }
    |"\\0" -> 
      code = int_of_string (Str.replace_first lit_switch "0o" lit_str) in
      Char.chr code
    |"\\c" ->
      begin
        match lit_str.[2] with
          '?' -> '\x7f'|
          c -> Char.chr (Char.code c - Char.code '@')
      end
    |"\\t" -> '\t'
    |"\\n" -> '\n'
    |"\\r" -> '\r'
    |"\\f" -> '\x0c'
    |"\\a" -> '\x07'
    |"\\e" -> '\x1b'
    |_ -> lit_str.[0] in
  resolve_char c spos;
}

%}

escape_char '\\'

hex_literal escape_char 'x' ['0' - '9' 'a' - 'f' 'A' - 'F'] ['0' - '9' 'a' - 'f' 'A' - 'F']
oct_literal escape_char '0' (['0' - '7'] | ['0' - '7']['0' - '7'] | ['0' - '3']['0' - '7']['0' - '7'])
ctrl_literal escape_char 'c' ['?' '@' - '_']
nmd_literal escape_char ['t' 'n' 'r' 'f' 'a' 'e']

encoded_literals hex_literal | oct_literal | ctrl_literal | nmd_literal

normal_literal encoded_literals | [^'\\']
chead_literal encoded_literals | [^'[' '\\']
cbody_literal encoded_literals | [^'[' ']' '\\']

plain_chead_u [^'[' '\n' '\\']
plain_chead_v [^'[' ']' '\n' '\\']
plain_cbody_u [^'[' ']' '\n' '\\']
plain_cbody_v [^'[' ']' '\n' '\\']

range_chead_u encoded_literals | plain_chead_u as u
range_chead_v encoded_literals | plain_chead_v as v
range_cbody_u encoded_literals | plain_cbody_u as u 
range_cbody_v encoded_literals | plain_cbody_v as v

range_chead range_chead_u '-' range_chead_v 
range_cbody range_cbody_u '-' range_cbody_v

posix_names "Lower" | "Upper" | "ASCII" | "Alpha" | "Digit" | "Alnum" | "Punct" | "Graph" | "Print" | "Blank" | "Cntrl" | "XDigit" | "Space"
predefined_cls escape_char (['d' 'D' 's' 'S' 'w' 'W'] | 'p' '{' posix_names '}')

%s tk_normal

%%

<tk_normal>'\n' return makeEos();
  | '|' return makeVbar();
  | '*''+' raise(1);

%%

main() {
  yylex();
}
