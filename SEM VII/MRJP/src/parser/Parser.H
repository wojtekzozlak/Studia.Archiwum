#ifndef PARSER_HEADER_FILE
#define PARSER_HEADER_FILE

#include<vector>
#include<string>


class LProgram;
class TopDef;
class ListTopDef;
class Ext;
class InDef;
class ListInDef;
class Function;
class FArg;
class ListFArg;
class ListIdent;
class SBlock;
class ListStmt;
class Stmt;
class Item;
class ListItem;
class Type;
class Expr;
class ListExpr;
class AddOp;
class MulOp;
class RelOp;

typedef union
{
  int int_;
  char char_;
  double double_;
  char* string_;
  LProgram* lprogram_;
  TopDef* topdef_;
  ListTopDef* listtopdef_;
  Ext* ext_;
  InDef* indef_;
  ListInDef* listindef_;
  Function* function_;
  FArg* farg_;
  ListFArg* listfarg_;
  ListIdent* listident_;
  SBlock* sblock_;
  ListStmt* liststmt_;
  Stmt* stmt_;
  Item* item_;
  ListItem* listitem_;
  Type* type_;
  Expr* expr_;
  ListExpr* listexpr_;
  AddOp* addop_;
  MulOp* mulop_;
  RelOp* relop_;
} YYSTYPE;

LProgram* pLProgram(FILE *inp);
LProgram* pLProgram(const char *str);



#define _ERROR_ 258
#define _SYMB_0 259
#define _SYMB_1 260
#define _SYMB_2 261
#define _SYMB_3 262
#define _SYMB_4 263
#define _SYMB_5 264
#define _SYMB_6 265
#define _SYMB_7 266
#define _SYMB_8 267
#define _SYMB_9 268
#define _SYMB_10 269
#define _SYMB_11 270
#define _SYMB_12 271
#define _SYMB_13 272
#define _SYMB_14 273
#define _SYMB_15 274
#define _SYMB_16 275
#define _SYMB_17 276
#define _SYMB_18 277
#define _SYMB_19 278
#define _SYMB_20 279
#define _SYMB_21 280
#define _SYMB_22 281
#define _SYMB_23 282
#define _SYMB_24 283
#define _SYMB_25 284
#define _SYMB_26 285
#define _SYMB_27 286
#define _SYMB_28 287
#define _SYMB_29 288
#define _SYMB_30 289
#define _SYMB_31 290
#define _SYMB_32 291
#define _SYMB_33 292
#define _SYMB_34 293
#define _SYMB_35 294
#define _SYMB_36 295
#define _SYMB_37 296
#define _SYMB_38 297
#define _SYMB_39 298
#define _SYMB_40 299
#define _SYMB_41 300
#define _SYMB_42 301
#define _SYMB_43 302
#define _STRING_ 303
#define _INTEGER_ 304
#define _IDENT_ 305

extern YYSTYPE yylval;

#endif
