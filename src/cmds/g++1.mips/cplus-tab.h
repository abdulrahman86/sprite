typedef union {long itype; tree ttype; enum tree_code code; } YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#define	YYACCEPT	return(0)
#define	YYABORT	return(1)
#define	YYERROR	goto yyerrlab
#define	IDENTIFIER	258
#define	TYPENAME	259
#define	SCSPEC	260
#define	TYPESPEC	261
#define	TYPE_QUAL	262
#define	CONSTANT	263
#define	STRING	264
#define	ELLIPSIS	265
#define	SIZEOF	266
#define	ENUM	267
#define	IF	268
#define	ELSE	269
#define	WHILE	270
#define	DO	271
#define	FOR	272
#define	SWITCH	273
#define	CASE	274
#define	DEFAULT	275
#define	BREAK	276
#define	CONTINUE	277
#define	RETURN	278
#define	GOTO	279
#define	ASM	280
#define	TYPEOF	281
#define	ALIGNOF	282
#define	ATTRIBUTE	283
#define	AGGR	284
#define	DELETE	285
#define	NEW	286
#define	OVERLOAD	287
#define	PRIVATE	288
#define	PUBLIC	289
#define	PROTECTED	290
#define	THIS	291
#define	OPERATOR	292
#define	DYNAMIC	293
#define	POINTSAT_LEFT_RIGHT	294
#define	LEFT_RIGHT	295
#define	SCOPE	296
#define	EMPTY	297
#define	TYPENAME_COLON	298
#define	ASSIGN	299
#define	RANGE	300
#define	OROR	301
#define	ANDAND	302
#define	MIN_MAX	303
#define	EQCOMPARE	304
#define	ARITHCOMPARE	305
#define	LSHIFT	306
#define	RSHIFT	307
#define	UNARY	308
#define	PLUSPLUS	309
#define	MINUSMINUS	310
#define	HYPERUNARY	311
#define	PAREN_STAR_PAREN	312
#define	PAREN_X_SCOPE_STAR_PAREN	313
#define	PAREN_X_SCOPE_REF_PAREN	314
#define	POINTSAT	315
#define	RAISE	316
#define	RAISES	317
#define	RERAISE	318
#define	TRY	319
#define	EXCEPT	320
#define	CATCH	321
#define	TYPENAME_SCOPE	322
#define	TYPENAME_ELLIPSIS	323
#define	PRE_PARSED_FUNCTION_DECL	324
#define	EXTERN_LANG_STRING	325
#define	ALL	326

