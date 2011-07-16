/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 11 "ntp_parser.y"

  #ifdef HAVE_CONFIG_H
  # include <config.h>
  #endif

  #include "ntp.h"
  #include "ntpd.h"
  #include "ntp_machine.h"
  #include "ntp_stdlib.h"
  #include "ntp_filegen.h"
  #include "ntp_scanner.h"
  #include "ntp_config.h"
  #include "ntp_crypto.h"

  #include "ntpsim.h"		/* HMS: Do we really want this all the time? */
				/* SK: It might be a good idea to always
				   include the simulator code. That way
				   someone can use the same configuration file
				   for both the simulator and the daemon
				*/


  struct FILE_INFO *ip_file;   /* Pointer to the configuration file stream */

  #define YYMALLOC	emalloc
  #define YYFREE	free
  #define YYERROR_VERBOSE
  #define YYMAXDEPTH	1000   /* stop the madness sooner */
  void yyerror(const char *msg);
  extern int input_from_file;  /* 0=input from ntpq :config */


/* Line 189 of yacc.c  */
#line 105 "ntp_parser.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 1
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_Age = 258,
     T_All = 259,
     T_Allan = 260,
     T_Allpeers = 261,
     T_Auth = 262,
     T_Autokey = 263,
     T_Automax = 264,
     T_Average = 265,
     T_Bclient = 266,
     T_Beacon = 267,
     T_Broadcast = 268,
     T_Broadcastclient = 269,
     T_Broadcastdelay = 270,
     T_Burst = 271,
     T_Calibrate = 272,
     T_Ceiling = 273,
     T_Clockstats = 274,
     T_Cohort = 275,
     T_ControlKey = 276,
     T_Crypto = 277,
     T_Cryptostats = 278,
     T_Ctl = 279,
     T_Day = 280,
     T_Default = 281,
     T_Digest = 282,
     T_Disable = 283,
     T_Discard = 284,
     T_Dispersion = 285,
     T_Double = 286,
     T_Driftfile = 287,
     T_Drop = 288,
     T_Ellipsis = 289,
     T_Enable = 290,
     T_End = 291,
     T_False = 292,
     T_File = 293,
     T_Filegen = 294,
     T_Flag1 = 295,
     T_Flag2 = 296,
     T_Flag3 = 297,
     T_Flag4 = 298,
     T_Flake = 299,
     T_Floor = 300,
     T_Freq = 301,
     T_Fudge = 302,
     T_Host = 303,
     T_Huffpuff = 304,
     T_Iburst = 305,
     T_Ident = 306,
     T_Ignore = 307,
     T_Incalloc = 308,
     T_Incmem = 309,
     T_Initalloc = 310,
     T_Initmem = 311,
     T_Includefile = 312,
     T_Integer = 313,
     T_Interface = 314,
     T_Intrange = 315,
     T_Io = 316,
     T_Ipv4 = 317,
     T_Ipv4_flag = 318,
     T_Ipv6 = 319,
     T_Ipv6_flag = 320,
     T_Kernel = 321,
     T_Key = 322,
     T_Keys = 323,
     T_Keysdir = 324,
     T_Kod = 325,
     T_Mssntp = 326,
     T_Leapfile = 327,
     T_Limited = 328,
     T_Link = 329,
     T_Listen = 330,
     T_Logconfig = 331,
     T_Logfile = 332,
     T_Loopstats = 333,
     T_Lowpriotrap = 334,
     T_Manycastclient = 335,
     T_Manycastserver = 336,
     T_Mask = 337,
     T_Maxage = 338,
     T_Maxclock = 339,
     T_Maxdepth = 340,
     T_Maxdist = 341,
     T_Maxmem = 342,
     T_Maxpoll = 343,
     T_Mem = 344,
     T_Minclock = 345,
     T_Mindepth = 346,
     T_Mindist = 347,
     T_Minimum = 348,
     T_Minpoll = 349,
     T_Minsane = 350,
     T_Mode = 351,
     T_Monitor = 352,
     T_Month = 353,
     T_Mru = 354,
     T_Multicastclient = 355,
     T_Nic = 356,
     T_Nolink = 357,
     T_Nomodify = 358,
     T_None = 359,
     T_Nonvolatile = 360,
     T_Nopeer = 361,
     T_Noquery = 362,
     T_Noselect = 363,
     T_Noserve = 364,
     T_Notrap = 365,
     T_Notrust = 366,
     T_Ntp = 367,
     T_Ntpport = 368,
     T_NtpSignDsocket = 369,
     T_Orphan = 370,
     T_Orphanwait = 371,
     T_Panic = 372,
     T_Peer = 373,
     T_Peerstats = 374,
     T_Phone = 375,
     T_Pid = 376,
     T_Pidfile = 377,
     T_Pool = 378,
     T_Port = 379,
     T_Preempt = 380,
     T_Prefer = 381,
     T_Protostats = 382,
     T_Pw = 383,
     T_Qos = 384,
     T_Randfile = 385,
     T_Rawstats = 386,
     T_Refid = 387,
     T_Requestkey = 388,
     T_Reset = 389,
     T_Restrict = 390,
     T_Revoke = 391,
     T_Saveconfigdir = 392,
     T_Server = 393,
     T_Setvar = 394,
     T_Source = 395,
     T_Statistics = 396,
     T_Stats = 397,
     T_Statsdir = 398,
     T_Step = 399,
     T_Stepout = 400,
     T_Stratum = 401,
     T_String = 402,
     T_Sys = 403,
     T_Sysstats = 404,
     T_Tick = 405,
     T_Time1 = 406,
     T_Time2 = 407,
     T_Timer = 408,
     T_Timingstats = 409,
     T_Tinker = 410,
     T_Tos = 411,
     T_Trap = 412,
     T_True = 413,
     T_Trustedkey = 414,
     T_Ttl = 415,
     T_Type = 416,
     T_Unconfig = 417,
     T_Unpeer = 418,
     T_Version = 419,
     T_WanderThreshold = 420,
     T_Week = 421,
     T_Wildcard = 422,
     T_Xleave = 423,
     T_Year = 424,
     T_Flag = 425,
     T_EOC = 426,
     T_Simulate = 427,
     T_Beep_Delay = 428,
     T_Sim_Duration = 429,
     T_Server_Offset = 430,
     T_Duration = 431,
     T_Freq_Offset = 432,
     T_Wander = 433,
     T_Jitter = 434,
     T_Prop_Delay = 435,
     T_Proc_Delay = 436
   };
#endif
/* Tokens.  */
#define T_Age 258
#define T_All 259
#define T_Allan 260
#define T_Allpeers 261
#define T_Auth 262
#define T_Autokey 263
#define T_Automax 264
#define T_Average 265
#define T_Bclient 266
#define T_Beacon 267
#define T_Broadcast 268
#define T_Broadcastclient 269
#define T_Broadcastdelay 270
#define T_Burst 271
#define T_Calibrate 272
#define T_Ceiling 273
#define T_Clockstats 274
#define T_Cohort 275
#define T_ControlKey 276
#define T_Crypto 277
#define T_Cryptostats 278
#define T_Ctl 279
#define T_Day 280
#define T_Default 281
#define T_Digest 282
#define T_Disable 283
#define T_Discard 284
#define T_Dispersion 285
#define T_Double 286
#define T_Driftfile 287
#define T_Drop 288
#define T_Ellipsis 289
#define T_Enable 290
#define T_End 291
#define T_False 292
#define T_File 293
#define T_Filegen 294
#define T_Flag1 295
#define T_Flag2 296
#define T_Flag3 297
#define T_Flag4 298
#define T_Flake 299
#define T_Floor 300
#define T_Freq 301
#define T_Fudge 302
#define T_Host 303
#define T_Huffpuff 304
#define T_Iburst 305
#define T_Ident 306
#define T_Ignore 307
#define T_Incalloc 308
#define T_Incmem 309
#define T_Initalloc 310
#define T_Initmem 311
#define T_Includefile 312
#define T_Integer 313
#define T_Interface 314
#define T_Intrange 315
#define T_Io 316
#define T_Ipv4 317
#define T_Ipv4_flag 318
#define T_Ipv6 319
#define T_Ipv6_flag 320
#define T_Kernel 321
#define T_Key 322
#define T_Keys 323
#define T_Keysdir 324
#define T_Kod 325
#define T_Mssntp 326
#define T_Leapfile 327
#define T_Limited 328
#define T_Link 329
#define T_Listen 330
#define T_Logconfig 331
#define T_Logfile 332
#define T_Loopstats 333
#define T_Lowpriotrap 334
#define T_Manycastclient 335
#define T_Manycastserver 336
#define T_Mask 337
#define T_Maxage 338
#define T_Maxclock 339
#define T_Maxdepth 340
#define T_Maxdist 341
#define T_Maxmem 342
#define T_Maxpoll 343
#define T_Mem 344
#define T_Minclock 345
#define T_Mindepth 346
#define T_Mindist 347
#define T_Minimum 348
#define T_Minpoll 349
#define T_Minsane 350
#define T_Mode 351
#define T_Monitor 352
#define T_Month 353
#define T_Mru 354
#define T_Multicastclient 355
#define T_Nic 356
#define T_Nolink 357
#define T_Nomodify 358
#define T_None 359
#define T_Nonvolatile 360
#define T_Nopeer 361
#define T_Noquery 362
#define T_Noselect 363
#define T_Noserve 364
#define T_Notrap 365
#define T_Notrust 366
#define T_Ntp 367
#define T_Ntpport 368
#define T_NtpSignDsocket 369
#define T_Orphan 370
#define T_Orphanwait 371
#define T_Panic 372
#define T_Peer 373
#define T_Peerstats 374
#define T_Phone 375
#define T_Pid 376
#define T_Pidfile 377
#define T_Pool 378
#define T_Port 379
#define T_Preempt 380
#define T_Prefer 381
#define T_Protostats 382
#define T_Pw 383
#define T_Qos 384
#define T_Randfile 385
#define T_Rawstats 386
#define T_Refid 387
#define T_Requestkey 388
#define T_Reset 389
#define T_Restrict 390
#define T_Revoke 391
#define T_Saveconfigdir 392
#define T_Server 393
#define T_Setvar 394
#define T_Source 395
#define T_Statistics 396
#define T_Stats 397
#define T_Statsdir 398
#define T_Step 399
#define T_Stepout 400
#define T_Stratum 401
#define T_String 402
#define T_Sys 403
#define T_Sysstats 404
#define T_Tick 405
#define T_Time1 406
#define T_Time2 407
#define T_Timer 408
#define T_Timingstats 409
#define T_Tinker 410
#define T_Tos 411
#define T_Trap 412
#define T_True 413
#define T_Trustedkey 414
#define T_Ttl 415
#define T_Type 416
#define T_Unconfig 417
#define T_Unpeer 418
#define T_Version 419
#define T_WanderThreshold 420
#define T_Week 421
#define T_Wildcard 422
#define T_Xleave 423
#define T_Year 424
#define T_Flag 425
#define T_EOC 426
#define T_Simulate 427
#define T_Beep_Delay 428
#define T_Sim_Duration 429
#define T_Server_Offset 430
#define T_Duration 431
#define T_Freq_Offset 432
#define T_Wander 433
#define T_Jitter 434
#define T_Prop_Delay 435
#define T_Proc_Delay 436




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 49 "ntp_parser.y"

	char *			String;
	double			Double;
	int			Integer;
	gen_fifo *		Generic_fifo;
	attr_val *		Attr_val;
	attr_val_fifo *		Attr_val_fifo;
	int_fifo *		Int_fifo;
	string_fifo *		String_fifo;
	address_node *		Address_node;
	address_fifo *		Address_fifo;
	setvar_node *		Set_var;
	server_info *		Sim_server;
	server_info_fifo *	Sim_server_fifo;
	script_info *		Sim_script;
	script_info_fifo *	Sim_script_fifo;



/* Line 214 of yacc.c  */
#line 523 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 535 "ntp_parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  193
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   606

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  187
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  99
/* YYNRULES -- Number of rules.  */
#define YYNRULES  294
/* YYNRULES -- Number of states.  */
#define YYNSTATES  395

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   436

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     183,   184,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   182,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   185,     2,   186,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    15,    16,    18,    20,
      22,    24,    26,    28,    30,    32,    34,    36,    38,    40,
      44,    46,    48,    50,    52,    54,    56,    59,    61,    63,
      65,    66,    69,    71,    73,    75,    77,    79,    81,    83,
      85,    87,    89,    91,    93,    96,    98,   100,   102,   104,
     106,   108,   111,   113,   116,   118,   120,   122,   125,   128,
     131,   134,   137,   140,   143,   146,   149,   152,   155,   156,
     159,   162,   165,   167,   169,   171,   173,   175,   178,   181,
     183,   186,   189,   192,   194,   196,   198,   200,   202,   204,
     206,   208,   210,   212,   215,   218,   222,   225,   227,   229,
     231,   233,   235,   237,   239,   241,   243,   244,   247,   250,
     253,   255,   257,   259,   261,   263,   265,   267,   269,   271,
     273,   275,   277,   279,   282,   285,   289,   295,   299,   304,
     309,   313,   314,   317,   319,   321,   323,   325,   327,   329,
     331,   333,   335,   337,   339,   341,   343,   345,   348,   350,
     353,   355,   357,   359,   362,   364,   367,   369,   371,   373,
     375,   377,   379,   381,   383,   387,   390,   392,   395,   398,
     401,   404,   406,   408,   410,   412,   414,   416,   419,   422,
     425,   427,   429,   431,   433,   435,   437,   439,   441,   443,
     446,   449,   451,   454,   456,   458,   460,   462,   464,   466,
     468,   470,   472,   475,   478,   481,   485,   487,   490,   493,
     496,   499,   503,   506,   508,   510,   512,   514,   516,   518,
     520,   522,   524,   526,   529,   530,   535,   537,   538,   539,
     542,   545,   548,   551,   553,   555,   559,   563,   565,   567,
     569,   571,   573,   575,   577,   579,   581,   584,   587,   589,
     591,   593,   595,   597,   599,   601,   603,   606,   608,   611,
     613,   615,   617,   623,   626,   628,   631,   633,   635,   637,
     639,   641,   643,   649,   651,   655,   658,   662,   664,   666,
     669,   671,   677,   682,   686,   689,   691,   698,   702,   705,
     709,   711,   713,   715,   717
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     188,     0,    -1,   189,    -1,   189,   190,   171,    -1,   190,
     171,    -1,     1,   171,    -1,    -1,   191,    -1,   204,    -1,
     206,    -1,   207,    -1,   216,    -1,   224,    -1,   211,    -1,
     233,    -1,   238,    -1,   242,    -1,   246,    -1,   272,    -1,
     192,   193,   196,    -1,   138,    -1,   123,    -1,   118,    -1,
      13,    -1,    80,    -1,   194,    -1,   195,   147,    -1,   147,
      -1,    63,    -1,    65,    -1,    -1,   196,   197,    -1,   198,
      -1,   200,    -1,   202,    -1,   199,    -1,     8,    -1,    16,
      -1,    50,    -1,   108,    -1,   125,    -1,   126,    -1,   158,
      -1,   168,    -1,   201,    58,    -1,    67,    -1,    94,    -1,
      88,    -1,   160,    -1,    96,    -1,   164,    -1,   203,   147,
      -1,    51,    -1,   205,   193,    -1,   162,    -1,   163,    -1,
      14,    -1,    81,   269,    -1,   100,   269,    -1,     9,    58,
      -1,    21,    58,    -1,    22,   208,    -1,    68,   147,    -1,
      69,   147,    -1,   133,    58,    -1,   136,    58,    -1,   159,
     265,    -1,   114,   147,    -1,    -1,   208,   209,    -1,   210,
     147,    -1,   136,    58,    -1,    48,    -1,    51,    -1,   128,
      -1,   130,    -1,    27,    -1,   156,   212,    -1,   212,   213,
      -1,   213,    -1,   214,    58,    -1,   215,   271,    -1,    20,
     270,    -1,    18,    -1,    45,    -1,   115,    -1,   116,    -1,
      95,    -1,    12,    -1,    92,    -1,    86,    -1,    90,    -1,
      84,    -1,   141,   217,    -1,   143,   147,    -1,    39,   218,
     219,    -1,   217,   218,    -1,   218,    -1,    19,    -1,    23,
      -1,    78,    -1,   119,    -1,   131,    -1,   149,    -1,   154,
      -1,   127,    -1,    -1,   219,   220,    -1,    38,   147,    -1,
     161,   223,    -1,   221,    -1,   222,    -1,    74,    -1,   102,
      -1,    35,    -1,    28,    -1,   104,    -1,   121,    -1,    25,
      -1,   166,    -1,    98,    -1,   169,    -1,     3,    -1,    29,
     227,    -1,    99,   230,    -1,   135,   193,   225,    -1,   135,
     194,    82,   194,   225,    -1,   135,    26,   225,    -1,   135,
      63,    26,   225,    -1,   135,    65,    26,   225,    -1,   135,
     140,   225,    -1,    -1,   225,   226,    -1,    44,    -1,    52,
      -1,    70,    -1,    71,    -1,    73,    -1,    79,    -1,   103,
      -1,   106,    -1,   107,    -1,   109,    -1,   110,    -1,   111,
      -1,   113,    -1,   164,    -1,   227,   228,    -1,   228,    -1,
     229,    58,    -1,    10,    -1,    93,    -1,    97,    -1,   230,
     231,    -1,   231,    -1,   232,    58,    -1,    53,    -1,    54,
      -1,    55,    -1,    56,    -1,    83,    -1,    85,    -1,    87,
      -1,    91,    -1,    47,   193,   234,    -1,   234,   235,    -1,
     235,    -1,   236,   271,    -1,   237,   270,    -1,   146,    58,
      -1,   132,   147,    -1,   151,    -1,   152,    -1,    40,    -1,
      41,    -1,    42,    -1,    43,    -1,    35,   239,    -1,    28,
     239,    -1,   239,   240,    -1,   240,    -1,   241,    -1,   142,
      -1,     7,    -1,    11,    -1,    17,    -1,    66,    -1,    97,
      -1,   112,    -1,   155,   243,    -1,   243,   244,    -1,   244,
      -1,   245,   271,    -1,     5,    -1,    30,    -1,    46,    -1,
      49,    -1,   117,    -1,   144,    -1,   145,    -1,   257,    -1,
     261,    -1,   247,   271,    -1,   248,   147,    -1,   249,   147,
      -1,    57,   147,   190,    -1,    36,    -1,    32,   250,    -1,
      76,   255,    -1,   120,   268,    -1,   139,   251,    -1,   157,
     194,   253,    -1,   160,   264,    -1,    15,    -1,   105,    -1,
     150,    -1,    51,    -1,    72,    -1,   122,    -1,   129,    -1,
      77,    -1,   137,    -1,   147,    -1,   147,    31,    -1,    -1,
     147,   182,   147,   252,    -1,    26,    -1,    -1,    -1,   253,
     254,    -1,   124,    58,    -1,    59,   194,    -1,   255,   256,
      -1,   256,    -1,   147,    -1,   258,   260,   259,    -1,   258,
     260,   147,    -1,    59,    -1,   101,    -1,     4,    -1,    62,
      -1,    64,    -1,   167,    -1,    75,    -1,    52,    -1,    33,
      -1,   134,   262,    -1,   262,   263,    -1,   263,    -1,     6,
      -1,     7,    -1,    24,    -1,    61,    -1,    89,    -1,   148,
      -1,   153,    -1,   264,    58,    -1,    58,    -1,   265,   266,
      -1,   266,    -1,    58,    -1,   267,    -1,   183,    58,    34,
      58,   184,    -1,   268,   147,    -1,   147,    -1,   269,   193,
      -1,   193,    -1,    58,    -1,   158,    -1,    37,    -1,    58,
      -1,    31,    -1,   273,   185,   274,   277,   186,    -1,   172,
      -1,   274,   275,   171,    -1,   275,   171,    -1,   276,   182,
     271,    -1,   173,    -1,   174,    -1,   277,   278,    -1,   278,
      -1,   280,   185,   279,   281,   186,    -1,   175,   182,   271,
     171,    -1,   138,   182,   193,    -1,   281,   282,    -1,   282,
      -1,   176,   182,   271,   185,   283,   186,    -1,   283,   284,
     171,    -1,   284,   171,    -1,   285,   182,   271,    -1,   177,
      -1,   178,    -1,   179,    -1,   180,    -1,   181,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   346,   346,   350,   351,   352,   366,   367,   368,   369,
     370,   371,   372,   373,   374,   375,   376,   377,   378,   386,
     396,   397,   398,   399,   400,   404,   405,   410,   415,   417,
     423,   424,   432,   433,   434,   438,   443,   444,   445,   446,
     447,   448,   449,   450,   454,   459,   460,   461,   462,   463,
     464,   468,   473,   482,   492,   493,   503,   505,   507,   518,
     525,   527,   532,   534,   536,   538,   540,   542,   548,   549,
     557,   559,   571,   572,   573,   574,   575,   584,   589,   594,
     602,   604,   606,   611,   612,   613,   614,   615,   616,   620,
     621,   622,   623,   632,   634,   643,   653,   658,   666,   667,
     668,   669,   670,   671,   672,   673,   678,   679,   687,   697,
     706,   721,   726,   727,   731,   732,   736,   737,   738,   739,
     740,   741,   742,   751,   755,   759,   767,   775,   783,   798,
     813,   826,   827,   835,   836,   837,   838,   839,   840,   841,
     842,   843,   844,   845,   846,   847,   848,   852,   857,   865,
     870,   871,   872,   876,   881,   889,   894,   895,   896,   897,
     898,   899,   900,   901,   909,   919,   924,   932,   934,   936,
     938,   943,   944,   948,   949,   950,   951,   959,   961,   966,
     971,   979,   981,   993,   994,   995,   996,   997,   998,  1006,
    1011,  1016,  1024,  1029,  1030,  1031,  1032,  1033,  1034,  1035,
    1044,  1045,  1046,  1053,  1060,  1076,  1095,  1100,  1102,  1104,
    1106,  1108,  1115,  1120,  1121,  1122,  1126,  1127,  1128,  1129,
    1133,  1134,  1138,  1145,  1155,  1164,  1169,  1171,  1176,  1177,
    1185,  1187,  1195,  1200,  1208,  1233,  1240,  1250,  1251,  1255,
    1256,  1257,  1258,  1262,  1263,  1264,  1268,  1273,  1278,  1286,
    1287,  1288,  1289,  1290,  1291,  1292,  1302,  1307,  1315,  1320,
    1328,  1330,  1334,  1339,  1344,  1352,  1357,  1365,  1374,  1375,
    1379,  1380,  1389,  1407,  1411,  1416,  1424,  1429,  1430,  1434,
    1439,  1447,  1452,  1457,  1462,  1467,  1475,  1480,  1485,  1493,
    1498,  1499,  1500,  1501,  1502
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_Age", "T_All", "T_Allan",
  "T_Allpeers", "T_Auth", "T_Autokey", "T_Automax", "T_Average",
  "T_Bclient", "T_Beacon", "T_Broadcast", "T_Broadcastclient",
  "T_Broadcastdelay", "T_Burst", "T_Calibrate", "T_Ceiling",
  "T_Clockstats", "T_Cohort", "T_ControlKey", "T_Crypto", "T_Cryptostats",
  "T_Ctl", "T_Day", "T_Default", "T_Digest", "T_Disable", "T_Discard",
  "T_Dispersion", "T_Double", "T_Driftfile", "T_Drop", "T_Ellipsis",
  "T_Enable", "T_End", "T_False", "T_File", "T_Filegen", "T_Flag1",
  "T_Flag2", "T_Flag3", "T_Flag4", "T_Flake", "T_Floor", "T_Freq",
  "T_Fudge", "T_Host", "T_Huffpuff", "T_Iburst", "T_Ident", "T_Ignore",
  "T_Incalloc", "T_Incmem", "T_Initalloc", "T_Initmem", "T_Includefile",
  "T_Integer", "T_Interface", "T_Intrange", "T_Io", "T_Ipv4",
  "T_Ipv4_flag", "T_Ipv6", "T_Ipv6_flag", "T_Kernel", "T_Key", "T_Keys",
  "T_Keysdir", "T_Kod", "T_Mssntp", "T_Leapfile", "T_Limited", "T_Link",
  "T_Listen", "T_Logconfig", "T_Logfile", "T_Loopstats", "T_Lowpriotrap",
  "T_Manycastclient", "T_Manycastserver", "T_Mask", "T_Maxage",
  "T_Maxclock", "T_Maxdepth", "T_Maxdist", "T_Maxmem", "T_Maxpoll",
  "T_Mem", "T_Minclock", "T_Mindepth", "T_Mindist", "T_Minimum",
  "T_Minpoll", "T_Minsane", "T_Mode", "T_Monitor", "T_Month", "T_Mru",
  "T_Multicastclient", "T_Nic", "T_Nolink", "T_Nomodify", "T_None",
  "T_Nonvolatile", "T_Nopeer", "T_Noquery", "T_Noselect", "T_Noserve",
  "T_Notrap", "T_Notrust", "T_Ntp", "T_Ntpport", "T_NtpSignDsocket",
  "T_Orphan", "T_Orphanwait", "T_Panic", "T_Peer", "T_Peerstats",
  "T_Phone", "T_Pid", "T_Pidfile", "T_Pool", "T_Port", "T_Preempt",
  "T_Prefer", "T_Protostats", "T_Pw", "T_Qos", "T_Randfile", "T_Rawstats",
  "T_Refid", "T_Requestkey", "T_Reset", "T_Restrict", "T_Revoke",
  "T_Saveconfigdir", "T_Server", "T_Setvar", "T_Source", "T_Statistics",
  "T_Stats", "T_Statsdir", "T_Step", "T_Stepout", "T_Stratum", "T_String",
  "T_Sys", "T_Sysstats", "T_Tick", "T_Time1", "T_Time2", "T_Timer",
  "T_Timingstats", "T_Tinker", "T_Tos", "T_Trap", "T_True", "T_Trustedkey",
  "T_Ttl", "T_Type", "T_Unconfig", "T_Unpeer", "T_Version",
  "T_WanderThreshold", "T_Week", "T_Wildcard", "T_Xleave", "T_Year",
  "T_Flag", "T_EOC", "T_Simulate", "T_Beep_Delay", "T_Sim_Duration",
  "T_Server_Offset", "T_Duration", "T_Freq_Offset", "T_Wander", "T_Jitter",
  "T_Prop_Delay", "T_Proc_Delay", "'='", "'('", "')'", "'{'", "'}'",
  "$accept", "configuration", "command_list", "command", "server_command",
  "client_type", "address", "ip_address", "address_fam", "option_list",
  "option", "option_flag", "option_flag_keyword", "option_int",
  "option_int_keyword", "option_str", "option_str_keyword",
  "unpeer_command", "unpeer_keyword", "other_mode_command",
  "authentication_command", "crypto_command_list", "crypto_command",
  "crypto_str_keyword", "orphan_mode_command", "tos_option_list",
  "tos_option", "tos_option_int_keyword", "tos_option_dbl_keyword",
  "monitoring_command", "stats_list", "stat", "filegen_option_list",
  "filegen_option", "link_nolink", "enable_disable", "filegen_type",
  "access_control_command", "ac_flag_list", "access_control_flag",
  "discard_option_list", "discard_option", "discard_option_keyword",
  "mru_option_list", "mru_option", "mru_option_keyword", "fudge_command",
  "fudge_factor_list", "fudge_factor", "fudge_factor_dbl_keyword",
  "fudge_factor_bool_keyword", "system_option_command",
  "system_option_list", "system_option", "system_option_flag_keyword",
  "tinker_command", "tinker_option_list", "tinker_option",
  "tinker_option_keyword", "miscellaneous_command", "misc_cmd_dbl_keyword",
  "misc_cmd_str_keyword", "misc_cmd_str_lcl_keyword", "drift_parm",
  "variable_assign", "t_default_or_zero", "trap_option_list",
  "trap_option", "log_config_list", "log_config_command",
  "interface_command", "interface_nic", "nic_rule_class",
  "nic_rule_action", "reset_command", "counter_set_list",
  "counter_set_keyword", "integer_list", "integer_list_range",
  "integer_list_range_elt", "integer_range", "string_list", "address_list",
  "boolean", "number", "simulate_command", "sim_conf_start",
  "sim_init_statement_list", "sim_init_statement", "sim_init_keyword",
  "sim_server_list", "sim_server", "sim_server_offset", "sim_server_name",
  "sim_act_list", "sim_act", "sim_act_stmt_list", "sim_act_stmt",
  "sim_act_keyword", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,    61,    40,    41,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   187,   188,   189,   189,   189,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   191,
     192,   192,   192,   192,   192,   193,   193,   194,   195,   195,
     196,   196,   197,   197,   197,   198,   199,   199,   199,   199,
     199,   199,   199,   199,   200,   201,   201,   201,   201,   201,
     201,   202,   203,   204,   205,   205,   206,   206,   206,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   208,   208,
     209,   209,   210,   210,   210,   210,   210,   211,   212,   212,
     213,   213,   213,   214,   214,   214,   214,   214,   214,   215,
     215,   215,   215,   216,   216,   216,   217,   217,   218,   218,
     218,   218,   218,   218,   218,   218,   219,   219,   220,   220,
     220,   220,   221,   221,   222,   222,   223,   223,   223,   223,
     223,   223,   223,   224,   224,   224,   224,   224,   224,   224,
     224,   225,   225,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   226,   226,   226,   226,   226,   227,   227,   228,
     229,   229,   229,   230,   230,   231,   232,   232,   232,   232,
     232,   232,   232,   232,   233,   234,   234,   235,   235,   235,
     235,   236,   236,   237,   237,   237,   237,   238,   238,   239,
     239,   240,   240,   241,   241,   241,   241,   241,   241,   242,
     243,   243,   244,   245,   245,   245,   245,   245,   245,   245,
     246,   246,   246,   246,   246,   246,   246,   246,   246,   246,
     246,   246,   246,   247,   247,   247,   248,   248,   248,   248,
     249,   249,   250,   250,   250,   251,   252,   252,   253,   253,
     254,   254,   255,   255,   256,   257,   257,   258,   258,   259,
     259,   259,   259,   260,   260,   260,   261,   262,   262,   263,
     263,   263,   263,   263,   263,   263,   264,   264,   265,   265,
     266,   266,   267,   268,   268,   269,   269,   270,   270,   270,
     271,   271,   272,   273,   274,   274,   275,   276,   276,   277,
     277,   278,   279,   280,   281,   281,   282,   283,   283,   284,
     285,   285,   285,   285,   285
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     2,     1,     2,     1,     1,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     0,     2,
       2,     2,     1,     1,     1,     1,     1,     2,     2,     1,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     3,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     3,     5,     3,     4,     4,
       3,     0,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       1,     1,     1,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     2,     1,     2,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     3,     1,     2,     2,     2,
       2,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     0,     4,     1,     0,     0,     2,
       2,     2,     2,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     1,
       1,     1,     5,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     5,     1,     3,     2,     3,     1,     1,     2,
       1,     5,     4,     3,     2,     1,     6,     3,     2,     3,
       1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,    23,    56,   213,     0,    68,     0,     0,
     224,     0,   206,     0,     0,   216,     0,   237,     0,     0,
     217,     0,   220,    24,     0,     0,     0,   238,   214,     0,
      22,     0,   218,    21,   219,     0,     0,     0,     0,   221,
      20,     0,     0,     0,   215,     0,     0,     0,     0,     0,
      54,    55,   273,     0,     2,     0,     7,     0,     8,     0,
       9,    10,    13,    11,    12,    14,    15,    16,    17,     0,
       0,     0,   200,     0,   201,    18,     0,     5,    59,    60,
      61,   183,   184,   185,   186,   187,   188,   182,   178,   180,
     181,   150,   151,   152,   123,   148,     0,   222,   207,   177,
      98,    99,   100,   101,   105,   102,   103,   104,   106,    28,
      29,    27,     0,    25,     0,     6,    62,    63,   234,   208,
     233,   266,    57,   156,   157,   158,   159,   160,   161,   162,
     163,   124,   154,     0,    58,    67,   264,   209,    64,   249,
     250,   251,   252,   253,   254,   255,   246,   248,   131,    28,
      29,   131,   131,    25,    65,     0,   210,    93,    97,    94,
     193,   194,   195,   196,   197,   198,   199,   189,   191,     0,
      88,    83,     0,    84,    92,    90,    91,    89,    87,    85,
      86,    77,    79,     0,     0,   228,   260,     0,    66,   259,
     261,   257,   212,     1,     0,     4,    30,    53,   271,   270,
     202,   203,   204,   245,   244,   243,     0,     0,    76,    72,
      73,    74,    75,     0,    69,     0,   179,   147,   149,   223,
      95,   173,   174,   175,   176,     0,     0,   171,   172,   164,
     166,     0,     0,    26,   205,   232,   265,   153,   155,   263,
     247,   127,   131,   131,   130,   125,     0,     0,    96,   190,
     192,   269,   267,   268,    82,    78,    80,    81,   211,     0,
     258,   256,     3,    19,   239,   240,   241,   236,   242,   235,
     277,   278,     0,     0,     0,    71,    70,   115,   114,     0,
     112,   113,     0,   107,   110,   111,   170,   169,   165,   167,
     168,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   132,   128,   129,   131,   227,
       0,     0,   229,     0,    36,    37,    38,    52,    45,    47,
      46,    49,    39,    40,    41,    42,    48,    50,    43,    31,
      32,    35,    33,     0,    34,     0,     0,     0,     0,   280,
       0,   275,     0,   108,   122,   118,   120,   116,   117,   119,
     121,   109,   126,   226,   225,   231,   230,     0,    44,    51,
       0,   274,   272,   279,     0,   276,   262,   283,     0,     0,
       0,     0,     0,   285,     0,     0,   281,   284,   282,     0,
       0,   290,   291,   292,   293,   294,     0,     0,     0,   286,
       0,   288,     0,   287,   289
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    53,    54,    55,    56,    57,   121,   113,   114,   263,
     329,   330,   331,   332,   333,   334,   335,    58,    59,    60,
      61,    80,   214,   215,    62,   181,   182,   183,   184,    63,
     157,   108,   220,   283,   284,   285,   351,    64,   241,   305,
      94,    95,    96,   131,   132,   133,    65,   229,   230,   231,
     232,    66,    88,    89,    90,    67,   167,   168,   169,    68,
      69,    70,    71,    98,   156,   354,   258,   312,   119,   120,
      72,    73,   269,   206,    74,   146,   147,   192,   188,   189,
     190,   137,   122,   254,   200,    75,    76,   272,   273,   274,
     338,   339,   369,   340,   372,   373,   386,   387,   388
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -168
static const yytype_int16 yypact[] =
{
      26,  -151,   -20,  -168,  -168,  -168,   -16,  -168,   136,    -4,
    -122,   136,  -168,     3,   -35,  -168,  -103,  -168,   -97,   -94,
    -168,   -90,  -168,  -168,   -35,   332,   -35,  -168,  -168,   -84,
    -168,   -76,  -168,  -168,  -168,    16,    25,   107,    18,  -168,
    -168,   -67,     3,   -63,  -168,    67,   490,   -60,   -55,    30,
    -168,  -168,  -168,    90,   192,   -80,  -168,   -35,  -168,   -35,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,   -24,
     -51,   -48,  -168,     0,  -168,  -168,   -93,  -168,  -168,  -168,
     208,  -168,  -168,  -168,  -168,  -168,  -168,  -168,   136,  -168,
    -168,  -168,  -168,  -168,    -4,  -168,    46,    74,  -168,   136,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,   150,  -168,   -37,   344,  -168,  -168,  -168,   -90,
    -168,  -168,   -35,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,   332,  -168,    53,   -35,  -168,  -168,   -30,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,    25,  -168,  -168,    92,
      95,  -168,  -168,    41,  -168,   -58,  -168,     3,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,    67,  -168,   -24,
    -168,  -168,   -21,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,   490,  -168,    71,   -24,  -168,  -168,    77,   -55,  -168,
    -168,  -168,    78,  -168,   -32,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,     4,  -104,  -168,  -168,
    -168,  -168,  -168,    83,  -168,    -5,  -168,  -168,  -168,  -168,
     -23,  -168,  -168,  -168,  -168,    -2,    96,  -168,  -168,   150,
    -168,   -24,   -21,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,   419,  -168,  -168,   419,   419,   -60,     9,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,   -46,   116,
    -168,  -168,  -168,   273,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -114,   -13,    -8,  -168,  -168,  -168,  -168,    21,
    -168,  -168,    11,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,   419,   419,  -168,   153,
     -60,   129,  -168,   137,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,   138,  -168,    47,    17,    29,  -119,  -168,
      19,  -168,   -24,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,   419,  -168,  -168,  -168,  -168,    39,  -168,  -168,
     -35,  -168,  -168,  -168,    34,  -168,  -168,  -168,    40,    50,
     -24,    48,  -130,  -168,    58,   -24,  -168,  -168,  -168,    49,
      38,  -168,  -168,  -168,  -168,  -168,   228,    61,    54,  -168,
      66,  -168,   -24,  -168,  -168
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -168,  -168,  -168,   -33,  -168,  -168,   -14,   -36,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,    57,  -168,  -168,  -168,
    -168,   -38,  -168,  -168,  -168,  -168,  -168,  -168,  -142,  -168,
    -168,   146,  -168,  -168,   110,  -168,  -168,  -168,    13,  -168,
    -168,  -168,   233,   -70,  -168,  -168,  -168,    79,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,   126,
    -168,  -168,  -168,  -168,  -168,  -168,   104,  -168,  -168,    64,
    -168,  -168,   227,    23,  -167,  -168,  -168,  -168,   -15,  -168,
    -168,   -75,  -168,  -168,  -168,  -110,  -168,  -128,  -168
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     112,   153,   250,   186,   158,   277,    91,   198,   264,   244,
     245,   185,   278,   310,   344,   279,   251,   257,   216,   336,
      77,   194,   100,   152,   336,    97,   101,     1,   109,   216,
     110,   139,   140,   203,   199,     2,   345,   252,    78,     3,
       4,     5,    79,   196,   115,   197,   371,     6,     7,   141,
     116,   280,   204,   117,     8,     9,   376,   118,    10,   270,
     271,    11,    12,   135,   289,    13,   265,   362,   266,   270,
     271,   136,   160,    14,   138,   205,   154,    15,   311,   281,
     155,   102,   234,    16,   159,    17,   142,   111,   191,    92,
     193,   195,   207,    93,    18,    19,   201,   161,    20,   202,
     306,   307,    21,    22,   218,   219,    23,    24,   236,   346,
     233,   238,   111,   162,   143,   347,   163,   239,   242,   248,
     236,   243,   103,   246,   247,    25,    26,    27,   187,   256,
     104,    28,   348,   148,   105,   259,   261,   253,   282,   262,
      29,   275,   276,    81,    30,   286,    31,    82,    32,    33,
     313,   267,   106,    83,   287,    34,   309,   107,   341,    35,
      36,    37,    38,    39,    40,    41,   352,    42,   343,    43,
     149,   268,   150,   144,   342,   365,    44,   349,   145,   353,
     350,    45,    46,    47,   164,    48,    49,   356,    50,    51,
     221,   222,   223,   224,   359,   357,   358,    -6,    52,   360,
     361,     2,    84,   374,   364,     3,     4,     5,   379,   368,
     308,   165,   166,     6,     7,   381,   382,   383,   384,   385,
       8,     9,   370,   366,    10,   394,   371,    11,    12,   378,
     375,    13,   391,    85,   380,   208,   392,   393,   255,    14,
     217,   237,   288,    15,    99,   235,   249,   151,    86,    16,
     240,    17,   260,   134,   111,   290,   209,   337,   390,   210,
      18,    19,   377,   363,    20,     0,     0,     0,    21,    22,
       0,     0,    23,    24,   355,     0,     0,     0,    87,     0,
       0,   314,   225,     0,     0,     0,     0,     0,     0,   315,
       0,    25,    26,    27,     0,     0,   226,    28,     0,     0,
       0,   227,   228,     0,     0,     0,    29,     0,     0,     0,
      30,     0,    31,     0,    32,    33,     0,     0,     0,     0,
       0,    34,     0,   316,   317,    35,    36,    37,    38,    39,
      40,    41,     0,    42,     0,    43,   211,     0,   212,     0,
     318,     0,    44,     0,   213,     0,   367,    45,    46,    47,
       0,    48,    49,     2,    50,    51,     0,     3,     4,     5,
       0,   319,     0,    -6,    52,     6,     7,   320,     0,   321,
       0,     0,     8,     9,     0,     0,    10,     0,     0,    11,
      12,   322,     0,    13,     0,   123,   124,   125,   126,     0,
       0,    14,     0,     0,     0,    15,     0,     0,   323,   324,
       0,    16,     0,    17,     0,   381,   382,   383,   384,   385,
       0,     0,    18,    19,   389,   127,    20,   128,     0,   129,
      21,    22,     0,   130,    23,    24,     0,     0,     0,     0,
       0,   325,     0,   326,     0,     0,     0,   327,     0,     0,
       0,   328,     0,    25,    26,    27,     0,     0,     0,    28,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,     0,    30,   291,    31,     0,    32,    33,     0,     0,
       0,   292,     0,    34,     0,     0,     0,    35,    36,    37,
      38,    39,    40,    41,     0,    42,     0,    43,     0,   293,
     294,     0,   295,     0,    44,     0,     0,     0,   296,    45,
      46,    47,   170,    48,    49,     0,    50,    51,   171,     0,
     172,     0,     0,     0,     0,     0,    52,     0,     0,     0,
       0,     0,   297,     0,     0,   298,   299,     0,   300,   301,
     302,     0,   303,     0,     0,   173,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   174,     0,   175,     0,     0,     0,
     176,     0,   177,   304,     0,   178,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   179,   180
};

static const yytype_int16 yycheck[] =
{
      14,    37,   169,    58,    42,    28,    10,    31,     4,   151,
     152,    47,    35,    59,     3,    38,    37,   184,    88,   138,
     171,    54,    19,    37,   138,   147,    23,     1,    63,    99,
      65,     6,     7,    33,    58,     9,    25,    58,    58,    13,
      14,    15,    58,    57,   147,    59,   176,    21,    22,    24,
     147,    74,    52,   147,    28,    29,   186,   147,    32,   173,
     174,    35,    36,   147,   231,    39,    62,   186,    64,   173,
     174,   147,     5,    47,    58,    75,    58,    51,   124,   102,
     147,    78,   115,    57,   147,    59,    61,   147,    58,    93,
       0,   171,   185,    97,    68,    69,   147,    30,    72,   147,
     242,   243,    76,    77,    58,    31,    80,    81,   122,    98,
     147,    58,   147,    46,    89,   104,    49,   147,    26,   157,
     134,    26,   119,    82,   182,    99,   100,   101,   183,    58,
     127,   105,   121,    26,   131,    58,    58,   158,   161,   171,
     114,    58,   147,     7,   118,   147,   120,    11,   122,   123,
      34,   147,   149,    17,    58,   129,   147,   154,   171,   133,
     134,   135,   136,   137,   138,   139,   308,   141,   147,   143,
      63,   167,    65,   148,   182,   342,   150,   166,   153,    26,
     169,   155,   156,   157,   117,   159,   160,    58,   162,   163,
      40,    41,    42,    43,   147,    58,    58,   171,   172,   182,
     171,     9,    66,   370,   185,    13,    14,    15,   375,   175,
     246,   144,   145,    21,    22,   177,   178,   179,   180,   181,
      28,    29,   182,   184,    32,   392,   176,    35,    36,   171,
     182,    39,   171,    97,   185,    27,   182,   171,   181,    47,
      94,   131,   229,    51,    11,   119,   167,   140,   112,    57,
     146,    59,   188,    26,   147,   232,    48,   272,   386,    51,
      68,    69,   372,   338,    72,    -1,    -1,    -1,    76,    77,
      -1,    -1,    80,    81,   310,    -1,    -1,    -1,   142,    -1,
      -1,     8,   132,    -1,    -1,    -1,    -1,    -1,    -1,    16,
      -1,    99,   100,   101,    -1,    -1,   146,   105,    -1,    -1,
      -1,   151,   152,    -1,    -1,    -1,   114,    -1,    -1,    -1,
     118,    -1,   120,    -1,   122,   123,    -1,    -1,    -1,    -1,
      -1,   129,    -1,    50,    51,   133,   134,   135,   136,   137,
     138,   139,    -1,   141,    -1,   143,   128,    -1,   130,    -1,
      67,    -1,   150,    -1,   136,    -1,   360,   155,   156,   157,
      -1,   159,   160,     9,   162,   163,    -1,    13,    14,    15,
      -1,    88,    -1,   171,   172,    21,    22,    94,    -1,    96,
      -1,    -1,    28,    29,    -1,    -1,    32,    -1,    -1,    35,
      36,   108,    -1,    39,    -1,    53,    54,    55,    56,    -1,
      -1,    47,    -1,    -1,    -1,    51,    -1,    -1,   125,   126,
      -1,    57,    -1,    59,    -1,   177,   178,   179,   180,   181,
      -1,    -1,    68,    69,   186,    83,    72,    85,    -1,    87,
      76,    77,    -1,    91,    80,    81,    -1,    -1,    -1,    -1,
      -1,   158,    -1,   160,    -1,    -1,    -1,   164,    -1,    -1,
      -1,   168,    -1,    99,   100,   101,    -1,    -1,    -1,   105,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   114,    -1,
      -1,    -1,   118,    44,   120,    -1,   122,   123,    -1,    -1,
      -1,    52,    -1,   129,    -1,    -1,    -1,   133,   134,   135,
     136,   137,   138,   139,    -1,   141,    -1,   143,    -1,    70,
      71,    -1,    73,    -1,   150,    -1,    -1,    -1,    79,   155,
     156,   157,    12,   159,   160,    -1,   162,   163,    18,    -1,
      20,    -1,    -1,    -1,    -1,    -1,   172,    -1,    -1,    -1,
      -1,    -1,   103,    -1,    -1,   106,   107,    -1,   109,   110,
     111,    -1,   113,    -1,    -1,    45,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    86,    -1,    -1,    -1,
      90,    -1,    92,   164,    -1,    95,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   115,   116
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     1,     9,    13,    14,    15,    21,    22,    28,    29,
      32,    35,    36,    39,    47,    51,    57,    59,    68,    69,
      72,    76,    77,    80,    81,    99,   100,   101,   105,   114,
     118,   120,   122,   123,   129,   133,   134,   135,   136,   137,
     138,   139,   141,   143,   150,   155,   156,   157,   159,   160,
     162,   163,   172,   188,   189,   190,   191,   192,   204,   205,
     206,   207,   211,   216,   224,   233,   238,   242,   246,   247,
     248,   249,   257,   258,   261,   272,   273,   171,    58,    58,
     208,     7,    11,    17,    66,    97,   112,   142,   239,   240,
     241,    10,    93,    97,   227,   228,   229,   147,   250,   239,
      19,    23,    78,   119,   127,   131,   149,   154,   218,    63,
      65,   147,   193,   194,   195,   147,   147,   147,   147,   255,
     256,   193,   269,    53,    54,    55,    56,    83,    85,    87,
      91,   230,   231,   232,   269,   147,   147,   268,    58,     6,
       7,    24,    61,    89,   148,   153,   262,   263,    26,    63,
      65,   140,   193,   194,    58,   147,   251,   217,   218,   147,
       5,    30,    46,    49,   117,   144,   145,   243,   244,   245,
      12,    18,    20,    45,    84,    86,    90,    92,    95,   115,
     116,   212,   213,   214,   215,   194,    58,   183,   265,   266,
     267,    58,   264,     0,   190,   171,   193,   193,    31,    58,
     271,   147,   147,    33,    52,    75,   260,   185,    27,    48,
      51,   128,   130,   136,   209,   210,   240,   228,    58,    31,
     219,    40,    41,    42,    43,   132,   146,   151,   152,   234,
     235,   236,   237,   147,   190,   256,   193,   231,    58,   147,
     263,   225,    26,    26,   225,   225,    82,   182,   218,   244,
     271,    37,    58,   158,   270,   213,    58,   271,   253,    58,
     266,    58,   171,   196,     4,    62,    64,   147,   167,   259,
     173,   174,   274,   275,   276,    58,   147,    28,    35,    38,
      74,   102,   161,   220,   221,   222,   147,    58,   235,   271,
     270,    44,    52,    70,    71,    73,    79,   103,   106,   107,
     109,   110,   111,   113,   164,   226,   225,   225,   194,   147,
      59,   124,   254,    34,     8,    16,    50,    51,    67,    88,
      94,    96,   108,   125,   126,   158,   160,   164,   168,   197,
     198,   199,   200,   201,   202,   203,   138,   275,   277,   278,
     280,   171,   182,   147,     3,    25,    98,   104,   121,   166,
     169,   223,   225,    26,   252,   194,    58,    58,    58,   147,
     182,   171,   186,   278,   185,   271,   184,   193,   175,   279,
     182,   176,   281,   282,   271,   182,   186,   282,   171,   271,
     185,   177,   178,   179,   180,   181,   283,   284,   285,   186,
     284,   171,   182,   171,   271
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:

/* Line 1464 of yacc.c  */
#line 353 "ntp_parser.y"
    {
			/* I will need to incorporate much more fine grained
			 * error messages. The following should suffice for
			 * the time being.
			 */
			msyslog(LOG_ERR, 
				"syntax error in %s line %d, column %d",
				ip_file->fname,
				ip_file->err_line_no,
				ip_file->err_col_no);
		}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 387 "ntp_parser.y"
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.peers, my_node);
		}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 406 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), (yyvsp[(1) - (2)].Integer)); }
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 411 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_UNSPEC); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 416 "ntp_parser.y"
    { (yyval.Integer) = AF_INET; }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 418 "ntp_parser.y"
    { (yyval.Integer) = AF_INET6; }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 423 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 425 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 439 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 455 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 469 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 483 "ntp_parser.y"
    {
			unpeer_node *my_node;
			
			my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
		}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 504 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 506 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.manycastserver, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 508 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.multicastclient, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 519 "ntp_parser.y"
    {
			attr_val *atrv;
			
			atrv = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			APPEND_G_FIFO(cfgt.vars, atrv);
		}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 526 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 528 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			CONCAT_G_FIFOS(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 533 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 535 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 537 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 539 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 541 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Attr_val_fifo); }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 543 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 548 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 550 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 558 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 560 "ntp_parser.y"
    {
			(yyval.Attr_val) = NULL;
			cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer);
			msyslog(LOG_WARNING,
				"'crypto revoke %d' is deprecated, "
				"please use 'revoke %d' instead.",
				cfgt.auth.revoke, cfgt.auth.revoke);
		}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 585 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.orphan_cmds, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 590 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 595 "ntp_parser.y"
    {	
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 603 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 605 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 607 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 633 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.stats_list, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 635 "ntp_parser.y"
    {
			if (input_from_file) {
				cfgt.stats_dir = (yyvsp[(2) - (2)].String);
			} else {
				YYFREE((yyvsp[(2) - (2)].String));
				yyerror("statsdir remote configuration ignored");
			}
		}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 644 "ntp_parser.y"
    {
			filegen_node *fgn;
			
			fgn = create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.filegen_opts, fgn);
		}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 654 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 659 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 678 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 680 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 688 "ntp_parser.y"
    {
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			} else {
				(yyval.Attr_val) = NULL;
				YYFREE((yyvsp[(2) - (2)].String));
				yyerror("filegen file remote config ignored");
			}
		}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 698 "ntp_parser.y"
    {
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen type remote config ignored");
			}
		}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 707 "ntp_parser.y"
    {
			const char *err;
			
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				if (T_Link == (yyvsp[(1) - (1)].Integer))
					err = "filegen link remote config ignored";
				else
					err = "filegen nolink remote config ignored";
				yyerror(err);
			}
		}
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 722 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 752 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.discard_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 756 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.mru_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 760 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 768 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 776 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 784 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node(
				create_address_node(
					estrdup("0.0.0.0"), 
					AF_INET),
				create_address_node(
					estrdup("0.0.0.0"), 
					AF_INET),
				(yyvsp[(4) - (4)].Int_fifo), 
				ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 799 "ntp_parser.y"
    {
			restrict_node *rn;
			
			rn = create_restrict_node(
				create_address_node(
					estrdup("::"), 
					AF_INET6),
				create_address_node(
					estrdup("::"), 
					AF_INET6),
				(yyvsp[(4) - (4)].Int_fifo), 
				ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 814 "ntp_parser.y"
    {
			restrict_node *	rn;

			APPEND_G_FIFO((yyvsp[(3) - (3)].Int_fifo), create_int_node((yyvsp[(2) - (3)].Integer)));
			rn = create_restrict_node(
				NULL, NULL, (yyvsp[(3) - (3)].Int_fifo), ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 826 "ntp_parser.y"
    { (yyval.Int_fifo) = NULL; }
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 828 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 853 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 858 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 866 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 877 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 882 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 890 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 910 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.fudge, aon);
		}
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 920 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 925 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 933 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 935 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 937 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 939 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 177:

/* Line 1464 of yacc.c  */
#line 960 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.enable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 962 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.disable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 967 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 972 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 980 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 982 "ntp_parser.y"
    { 
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				yyerror("enable/disable stats remote configuration ignored");
			}
		}
    break;

  case 189:

/* Line 1464 of yacc.c  */
#line 1007 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.tinker, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 190:

/* Line 1464 of yacc.c  */
#line 1012 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 191:

/* Line 1464 of yacc.c  */
#line 1017 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 192:

/* Line 1464 of yacc.c  */
#line 1025 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 202:

/* Line 1464 of yacc.c  */
#line 1047 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 203:

/* Line 1464 of yacc.c  */
#line 1054 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 204:

/* Line 1464 of yacc.c  */
#line 1061 "ntp_parser.y"
    {
			char error_text[64];
			attr_val *av;

			if (input_from_file) {
				av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
				APPEND_G_FIFO(cfgt.vars, av);
			} else {
				YYFREE((yyvsp[(2) - (2)].String));
				snprintf(error_text, sizeof(error_text),
					 "%s remote config ignored",
					 keyword((yyvsp[(1) - (2)].Integer)));
				yyerror(error_text);
			}
		}
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 1077 "ntp_parser.y"
    {
			if (!input_from_file) {
				yyerror("remote includefile ignored");
				break;
			}
			if (curr_include_level >= MAXINCLUDELEVEL) {
				fprintf(stderr, "getconfig: Maximum include file level exceeded.\n");
				msyslog(LOG_ERR, "getconfig: Maximum include file level exceeded.\n");
			} else {
				fp[curr_include_level + 1] = F_OPEN(FindConfig((yyvsp[(2) - (3)].String)), "r");
				if (fp[curr_include_level + 1] == NULL) {
					fprintf(stderr, "getconfig: Couldn't open <%s>\n", FindConfig((yyvsp[(2) - (3)].String)));
					msyslog(LOG_ERR, "getconfig: Couldn't open <%s>\n", FindConfig((yyvsp[(2) - (3)].String)));
				} else {
					ip_file = fp[++curr_include_level];
				}
			}
		}
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 1096 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 1101 "ntp_parser.y"
    { /* see drift_parm below for actions */ }
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 1103 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.logconfig, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 1105 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.phone, (yyvsp[(2) - (2)].String_fifo)); }
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 1107 "ntp_parser.y"
    { APPEND_G_FIFO(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 211:

/* Line 1464 of yacc.c  */
#line 1109 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.trap, aon);
		}
    break;

  case 212:

/* Line 1464 of yacc.c  */
#line 1116 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.ttl, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 222:

/* Line 1464 of yacc.c  */
#line 1139 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 1146 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
			av = create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 224:

/* Line 1464 of yacc.c  */
#line 1155 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, "");
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 1165 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 1171 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1176 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 1178 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 1186 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 1188 "ntp_parser.y"
    {
			(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), estrdup((yyvsp[(2) - (2)].Address_node)->address));
			destroy_address_node((yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 1196 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 1201 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 1209 "ntp_parser.y"
    {
			char	prefix;
			char *	type;
			
			switch ((yyvsp[(1) - (1)].String)[0]) {
			
			case '+':
			case '-':
			case '=':
				prefix = (yyvsp[(1) - (1)].String)[0];
				type = (yyvsp[(1) - (1)].String) + 1;
				break;
				
			default:
				prefix = '=';
				type = (yyvsp[(1) - (1)].String);
			}	
			
			(yyval.Attr_val) = create_attr_sval(prefix, estrdup(type));
			YYFREE((yyvsp[(1) - (1)].String));
		}
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 1234 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 236:

/* Line 1464 of yacc.c  */
#line 1241 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 246:

/* Line 1464 of yacc.c  */
#line 1269 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.reset_counters, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 247:

/* Line 1464 of yacc.c  */
#line 1274 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 1279 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 256:

/* Line 1464 of yacc.c  */
#line 1303 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 257:

/* Line 1464 of yacc.c  */
#line 1308 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 1316 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 1321 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 1329 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[(1) - (1)].Integer)); }
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 1335 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_rangeval('-', (yyvsp[(2) - (5)].Integer), (yyvsp[(4) - (5)].Integer)); }
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 1340 "ntp_parser.y"
    {
			(yyval.String_fifo) = (yyvsp[(1) - (2)].String_fifo);
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(2) - (2)].String)));
		}
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 1345 "ntp_parser.y"
    {
			(yyval.String_fifo) = NULL;
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(1) - (1)].String)));
		}
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 1353 "ntp_parser.y"
    {
			(yyval.Address_fifo) = (yyvsp[(1) - (2)].Address_fifo);
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 1358 "ntp_parser.y"
    {
			(yyval.Address_fifo) = NULL;
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(1) - (1)].Address_node));
		}
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 1366 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			} else {
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
			}
		}
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 1374 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 1375 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 1379 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 1390 "ntp_parser.y"
    {
			sim_node *sn;
			
			sn =  create_sim_node((yyvsp[(3) - (5)].Attr_val_fifo), (yyvsp[(4) - (5)].Sim_server_fifo));
			APPEND_G_FIFO(cfgt.sim_details, sn);

			/* Revert from ; to \n for end-of-command */
			old_config_style = 1;
		}
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 1407 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 1412 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 1417 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 1425 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 279:

/* Line 1464 of yacc.c  */
#line 1435 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = (yyvsp[(1) - (2)].Sim_server_fifo);
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(2) - (2)].Sim_server));
		}
    break;

  case 280:

/* Line 1464 of yacc.c  */
#line 1440 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(1) - (1)].Sim_server));
		}
    break;

  case 281:

/* Line 1464 of yacc.c  */
#line 1448 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Sim_script_fifo)); }
    break;

  case 282:

/* Line 1464 of yacc.c  */
#line 1453 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 283:

/* Line 1464 of yacc.c  */
#line 1458 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 284:

/* Line 1464 of yacc.c  */
#line 1463 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = (yyvsp[(1) - (2)].Sim_script_fifo);
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(2) - (2)].Sim_script));
		}
    break;

  case 285:

/* Line 1464 of yacc.c  */
#line 1468 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(1) - (1)].Sim_script));
		}
    break;

  case 286:

/* Line 1464 of yacc.c  */
#line 1476 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Attr_val_fifo)); }
    break;

  case 287:

/* Line 1464 of yacc.c  */
#line 1481 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 288:

/* Line 1464 of yacc.c  */
#line 1486 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 289:

/* Line 1464 of yacc.c  */
#line 1494 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1464 of yacc.c  */
#line 3588 "ntp_parser.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1684 of yacc.c  */
#line 1505 "ntp_parser.y"


void 
yyerror(
	const char *msg
	)
{
	int retval;

	ip_file->err_line_no = ip_file->prev_token_line_no;
	ip_file->err_col_no = ip_file->prev_token_col_no;
	
	msyslog(LOG_ERR, 
		"line %d column %d %s", 
		ip_file->err_line_no,
		ip_file->err_col_no,
		msg);
	if (!input_from_file) {
		/* Save the error message in the correct buffer */
		retval = snprintf(remote_config.err_msg + remote_config.err_pos,
				  MAXLINE - remote_config.err_pos,
				  "column %d %s",
				  ip_file->err_col_no, msg);

		/* Increment the value of err_pos */
		if (retval > 0)
			remote_config.err_pos += retval;

		/* Increment the number of errors */
		++remote_config.no_errors;
	}
}


/*
 * token_name - convert T_ token integers to text
 *		example: token_name(T_Server) returns "T_Server"
 */
const char *
token_name(
	int token
	)
{
	return yytname[YYTRANSLATE(token)];
}


/* Initial Testing function -- ignore */
#if 0
int main(int argc, char *argv[])
{
	ip_file = FOPEN(argv[1], "r");
	if (!ip_file)
		fprintf(stderr, "ERROR!! Could not open file: %s\n", argv[1]);
	yyparse();
	return 0;
}
#endif


