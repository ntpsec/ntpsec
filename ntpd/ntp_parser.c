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


  struct FILE_INFO *ip_file;	/* configuration file stream */

  #define YYMALLOC	emalloc
  #define YYFREE	free
  #define YYERROR_VERBOSE
  #define YYMAXDEPTH	1000	/* stop the madness sooner */
  void yyerror(const char *msg);
  extern int input_from_file;	/* else from ntpq :config */


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
     T_Mode7 = 352,
     T_Monitor = 353,
     T_Month = 354,
     T_Mru = 355,
     T_Multicastclient = 356,
     T_Nic = 357,
     T_Nolink = 358,
     T_Nomodify = 359,
     T_Nomrulist = 360,
     T_None = 361,
     T_Nonvolatile = 362,
     T_Nopeer = 363,
     T_Noquery = 364,
     T_Noselect = 365,
     T_Noserve = 366,
     T_Notrap = 367,
     T_Notrust = 368,
     T_Ntp = 369,
     T_Ntpport = 370,
     T_NtpSignDsocket = 371,
     T_Orphan = 372,
     T_Orphanwait = 373,
     T_Panic = 374,
     T_Peer = 375,
     T_Peerstats = 376,
     T_Phone = 377,
     T_Pid = 378,
     T_Pidfile = 379,
     T_Pool = 380,
     T_Port = 381,
     T_Preempt = 382,
     T_Prefer = 383,
     T_Protostats = 384,
     T_Pw = 385,
     T_Qos = 386,
     T_Randfile = 387,
     T_Rawstats = 388,
     T_Refid = 389,
     T_Requestkey = 390,
     T_Reset = 391,
     T_Restrict = 392,
     T_Revoke = 393,
     T_Saveconfigdir = 394,
     T_Server = 395,
     T_Setvar = 396,
     T_Source = 397,
     T_Statistics = 398,
     T_Stats = 399,
     T_Statsdir = 400,
     T_Step = 401,
     T_Stepout = 402,
     T_Stratum = 403,
     T_String = 404,
     T_Sys = 405,
     T_Sysstats = 406,
     T_Tick = 407,
     T_Time1 = 408,
     T_Time2 = 409,
     T_Timer = 410,
     T_Timingstats = 411,
     T_Tinker = 412,
     T_Tos = 413,
     T_Trap = 414,
     T_True = 415,
     T_Trustedkey = 416,
     T_Ttl = 417,
     T_Type = 418,
     T_U_int = 419,
     T_Unconfig = 420,
     T_Unpeer = 421,
     T_Version = 422,
     T_WanderThreshold = 423,
     T_Week = 424,
     T_Wildcard = 425,
     T_Xleave = 426,
     T_Year = 427,
     T_Flag = 428,
     T_EOC = 429,
     T_Simulate = 430,
     T_Beep_Delay = 431,
     T_Sim_Duration = 432,
     T_Server_Offset = 433,
     T_Duration = 434,
     T_Freq_Offset = 435,
     T_Wander = 436,
     T_Jitter = 437,
     T_Prop_Delay = 438,
     T_Proc_Delay = 439
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
#define T_Mode7 352
#define T_Monitor 353
#define T_Month 354
#define T_Mru 355
#define T_Multicastclient 356
#define T_Nic 357
#define T_Nolink 358
#define T_Nomodify 359
#define T_Nomrulist 360
#define T_None 361
#define T_Nonvolatile 362
#define T_Nopeer 363
#define T_Noquery 364
#define T_Noselect 365
#define T_Noserve 366
#define T_Notrap 367
#define T_Notrust 368
#define T_Ntp 369
#define T_Ntpport 370
#define T_NtpSignDsocket 371
#define T_Orphan 372
#define T_Orphanwait 373
#define T_Panic 374
#define T_Peer 375
#define T_Peerstats 376
#define T_Phone 377
#define T_Pid 378
#define T_Pidfile 379
#define T_Pool 380
#define T_Port 381
#define T_Preempt 382
#define T_Prefer 383
#define T_Protostats 384
#define T_Pw 385
#define T_Qos 386
#define T_Randfile 387
#define T_Rawstats 388
#define T_Refid 389
#define T_Requestkey 390
#define T_Reset 391
#define T_Restrict 392
#define T_Revoke 393
#define T_Saveconfigdir 394
#define T_Server 395
#define T_Setvar 396
#define T_Source 397
#define T_Statistics 398
#define T_Stats 399
#define T_Statsdir 400
#define T_Step 401
#define T_Stepout 402
#define T_Stratum 403
#define T_String 404
#define T_Sys 405
#define T_Sysstats 406
#define T_Tick 407
#define T_Time1 408
#define T_Time2 409
#define T_Timer 410
#define T_Timingstats 411
#define T_Tinker 412
#define T_Tos 413
#define T_Trap 414
#define T_True 415
#define T_Trustedkey 416
#define T_Ttl 417
#define T_Type 418
#define T_U_int 419
#define T_Unconfig 420
#define T_Unpeer 421
#define T_Version 422
#define T_WanderThreshold 423
#define T_Week 424
#define T_Wildcard 425
#define T_Xleave 426
#define T_Year 427
#define T_Flag 428
#define T_EOC 429
#define T_Simulate 430
#define T_Beep_Delay 431
#define T_Sim_Duration 432
#define T_Server_Offset 433
#define T_Duration 434
#define T_Freq_Offset 435
#define T_Wander 436
#define T_Jitter 437
#define T_Prop_Delay 438
#define T_Proc_Delay 439




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 49 "ntp_parser.y"

	char *			String;
	double			Double;
	int			Integer;
	unsigned		U_int;
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
#line 530 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 542 "ntp_parser.c"

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
#define YYFINAL  196
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   620

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  190
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  100
/* YYNRULES -- Number of rules.  */
#define YYNRULES  299
/* YYNRULES -- Number of states.  */
#define YYNSTATES  400

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   439

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     186,   187,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   185,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   188,     2,   189,     2,     2,     2,     2,
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
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184
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
      85,    87,    89,    91,    93,    96,    99,   101,   103,   105,
     107,   109,   111,   114,   116,   119,   121,   123,   125,   128,
     131,   134,   137,   140,   143,   146,   149,   152,   155,   158,
     159,   162,   165,   168,   170,   172,   174,   176,   178,   181,
     184,   186,   189,   192,   195,   197,   199,   201,   203,   205,
     207,   209,   211,   213,   215,   218,   221,   225,   228,   230,
     232,   234,   236,   238,   240,   242,   244,   246,   247,   250,
     253,   256,   258,   260,   262,   264,   266,   268,   270,   272,
     274,   276,   278,   280,   282,   285,   288,   292,   298,   302,
     307,   312,   316,   317,   320,   322,   324,   326,   328,   330,
     332,   334,   336,   338,   340,   342,   344,   346,   348,   350,
     353,   355,   358,   360,   362,   364,   367,   369,   372,   374,
     376,   378,   380,   382,   384,   386,   388,   392,   395,   397,
     400,   403,   406,   409,   411,   413,   415,   417,   419,   421,
     424,   427,   430,   432,   434,   436,   438,   440,   442,   444,
     446,   448,   450,   452,   455,   458,   460,   463,   465,   467,
     469,   471,   473,   475,   477,   479,   481,   483,   486,   489,
     492,   496,   498,   501,   504,   507,   510,   514,   517,   519,
     521,   523,   525,   527,   529,   531,   533,   535,   537,   540,
     541,   546,   548,   549,   550,   553,   556,   559,   562,   564,
     566,   570,   574,   576,   578,   580,   582,   584,   586,   588,
     590,   592,   595,   598,   600,   602,   604,   606,   608,   610,
     612,   614,   617,   619,   622,   624,   626,   628,   634,   637,
     639,   642,   644,   646,   648,   650,   652,   654,   660,   662,
     666,   669,   673,   675,   677,   680,   682,   688,   693,   697,
     700,   702,   709,   713,   716,   720,   722,   724,   726,   728
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     191,     0,    -1,   192,    -1,   192,   193,   174,    -1,   193,
     174,    -1,     1,   174,    -1,    -1,   194,    -1,   207,    -1,
     209,    -1,   210,    -1,   219,    -1,   227,    -1,   214,    -1,
     236,    -1,   241,    -1,   246,    -1,   250,    -1,   276,    -1,
     195,   196,   199,    -1,   140,    -1,   125,    -1,   120,    -1,
      13,    -1,    80,    -1,   197,    -1,   198,   149,    -1,   149,
      -1,    63,    -1,    65,    -1,    -1,   199,   200,    -1,   201,
      -1,   203,    -1,   205,    -1,   202,    -1,     8,    -1,    16,
      -1,    50,    -1,   110,    -1,   127,    -1,   128,    -1,   160,
      -1,   171,    -1,   204,    58,    -1,   204,   164,    -1,    67,
      -1,    94,    -1,    88,    -1,   162,    -1,    96,    -1,   167,
      -1,   206,   149,    -1,    51,    -1,   208,   196,    -1,   165,
      -1,   166,    -1,    14,    -1,    81,   273,    -1,   101,   273,
      -1,     9,    58,    -1,    21,    58,    -1,    22,   211,    -1,
      68,   149,    -1,    69,   149,    -1,   135,    58,    -1,   138,
      58,    -1,   161,   269,    -1,   116,   149,    -1,    -1,   211,
     212,    -1,   213,   149,    -1,   138,    58,    -1,    48,    -1,
      51,    -1,   130,    -1,   132,    -1,    27,    -1,   158,   215,
      -1,   215,   216,    -1,   216,    -1,   217,    58,    -1,   218,
     275,    -1,    20,   274,    -1,    18,    -1,    45,    -1,   117,
      -1,   118,    -1,    95,    -1,    12,    -1,    92,    -1,    86,
      -1,    90,    -1,    84,    -1,   143,   220,    -1,   145,   149,
      -1,    39,   221,   222,    -1,   220,   221,    -1,   221,    -1,
      19,    -1,    23,    -1,    78,    -1,   121,    -1,   133,    -1,
     151,    -1,   156,    -1,   129,    -1,    -1,   222,   223,    -1,
      38,   149,    -1,   163,   226,    -1,   224,    -1,   225,    -1,
      74,    -1,   103,    -1,    35,    -1,    28,    -1,   106,    -1,
     123,    -1,    25,    -1,   169,    -1,    99,    -1,   172,    -1,
       3,    -1,    29,   230,    -1,   100,   233,    -1,   137,   196,
     228,    -1,   137,   197,    82,   197,   228,    -1,   137,    26,
     228,    -1,   137,    63,    26,   228,    -1,   137,    65,    26,
     228,    -1,   137,   142,   228,    -1,    -1,   228,   229,    -1,
      44,    -1,    52,    -1,    70,    -1,    71,    -1,    73,    -1,
      79,    -1,   104,    -1,   105,    -1,   108,    -1,   109,    -1,
     111,    -1,   112,    -1,   113,    -1,   115,    -1,   167,    -1,
     230,   231,    -1,   231,    -1,   232,    58,    -1,    10,    -1,
      93,    -1,    98,    -1,   233,   234,    -1,   234,    -1,   235,
      58,    -1,    53,    -1,    54,    -1,    55,    -1,    56,    -1,
      83,    -1,    85,    -1,    87,    -1,    91,    -1,    47,   196,
     237,    -1,   237,   238,    -1,   238,    -1,   239,   275,    -1,
     240,   274,    -1,   148,    58,    -1,   134,   149,    -1,   153,
      -1,   154,    -1,    40,    -1,    41,    -1,    42,    -1,    43,
      -1,    35,   242,    -1,    28,   242,    -1,   242,   243,    -1,
     243,    -1,   244,    -1,   245,    -1,     7,    -1,    11,    -1,
      17,    -1,    66,    -1,    98,    -1,   114,    -1,    97,    -1,
     144,    -1,   157,   247,    -1,   247,   248,    -1,   248,    -1,
     249,   275,    -1,     5,    -1,    30,    -1,    46,    -1,    49,
      -1,   119,    -1,   146,    -1,   147,    -1,   152,    -1,   261,
      -1,   265,    -1,   251,   275,    -1,   252,   149,    -1,   253,
     149,    -1,    57,   149,   193,    -1,    36,    -1,    32,   254,
      -1,    76,   259,    -1,   122,   272,    -1,   141,   255,    -1,
     159,   197,   257,    -1,   162,   268,    -1,    15,    -1,   107,
      -1,   152,    -1,    51,    -1,    72,    -1,   124,    -1,   131,
      -1,    77,    -1,   139,    -1,   149,    -1,   149,    31,    -1,
      -1,   149,   185,   149,   256,    -1,    26,    -1,    -1,    -1,
     257,   258,    -1,   126,    58,    -1,    59,   197,    -1,   259,
     260,    -1,   260,    -1,   149,    -1,   262,   264,   263,    -1,
     262,   264,   149,    -1,    59,    -1,   102,    -1,     4,    -1,
      62,    -1,    64,    -1,   170,    -1,    75,    -1,    52,    -1,
      33,    -1,   136,   266,    -1,   266,   267,    -1,   267,    -1,
       6,    -1,     7,    -1,    24,    -1,    61,    -1,    89,    -1,
     150,    -1,   155,    -1,   268,    58,    -1,    58,    -1,   269,
     270,    -1,   270,    -1,    58,    -1,   271,    -1,   186,    58,
      34,    58,   187,    -1,   272,   149,    -1,   149,    -1,   273,
     196,    -1,   196,    -1,    58,    -1,   160,    -1,    37,    -1,
      58,    -1,    31,    -1,   277,   188,   278,   281,   189,    -1,
     175,    -1,   278,   279,   174,    -1,   279,   174,    -1,   280,
     185,   275,    -1,   176,    -1,   177,    -1,   281,   282,    -1,
     282,    -1,   284,   188,   283,   285,   189,    -1,   178,   185,
     275,   174,    -1,   140,   185,   196,    -1,   285,   286,    -1,
     286,    -1,   179,   185,   275,   188,   287,   189,    -1,   287,
     288,   174,    -1,   288,   174,    -1,   289,   185,   275,    -1,
     180,    -1,   181,    -1,   182,    -1,   183,    -1,   184,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   351,   351,   355,   356,   357,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   391,
     401,   402,   403,   404,   405,   409,   410,   415,   420,   422,
     428,   429,   437,   438,   439,   443,   448,   449,   450,   451,
     452,   453,   454,   455,   459,   461,   466,   467,   468,   469,
     470,   471,   475,   480,   489,   499,   500,   510,   512,   514,
     525,   532,   534,   539,   541,   543,   545,   547,   549,   555,
     556,   564,   566,   578,   579,   580,   581,   582,   591,   596,
     601,   609,   611,   613,   618,   619,   620,   621,   622,   623,
     627,   628,   629,   630,   639,   641,   650,   660,   665,   673,
     674,   675,   676,   677,   678,   679,   680,   685,   686,   694,
     704,   713,   728,   733,   734,   738,   739,   743,   744,   745,
     746,   747,   748,   749,   758,   762,   766,   774,   782,   790,
     805,   820,   833,   834,   842,   843,   844,   845,   846,   847,
     848,   849,   850,   851,   852,   853,   854,   855,   856,   860,
     865,   873,   878,   879,   880,   884,   889,   897,   902,   903,
     904,   905,   906,   907,   908,   909,   917,   927,   932,   940,
     942,   944,   946,   951,   952,   956,   957,   958,   959,   967,
     969,   974,   979,   987,   989,  1006,  1007,  1008,  1009,  1010,
    1011,  1015,  1016,  1024,  1029,  1034,  1042,  1047,  1048,  1049,
    1050,  1051,  1052,  1053,  1054,  1063,  1064,  1065,  1072,  1079,
    1095,  1114,  1119,  1121,  1123,  1125,  1127,  1134,  1139,  1140,
    1141,  1145,  1146,  1147,  1148,  1152,  1153,  1157,  1164,  1174,
    1183,  1188,  1190,  1195,  1196,  1204,  1206,  1214,  1219,  1227,
    1252,  1259,  1269,  1270,  1274,  1275,  1276,  1277,  1281,  1282,
    1283,  1287,  1292,  1297,  1305,  1306,  1307,  1308,  1309,  1310,
    1311,  1321,  1326,  1334,  1339,  1347,  1349,  1353,  1358,  1363,
    1371,  1376,  1384,  1393,  1394,  1398,  1399,  1408,  1426,  1430,
    1435,  1443,  1448,  1449,  1453,  1458,  1466,  1471,  1476,  1481,
    1486,  1494,  1499,  1504,  1512,  1517,  1518,  1519,  1520,  1521
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
  "T_Minpoll", "T_Minsane", "T_Mode", "T_Mode7", "T_Monitor", "T_Month",
  "T_Mru", "T_Multicastclient", "T_Nic", "T_Nolink", "T_Nomodify",
  "T_Nomrulist", "T_None", "T_Nonvolatile", "T_Nopeer", "T_Noquery",
  "T_Noselect", "T_Noserve", "T_Notrap", "T_Notrust", "T_Ntp", "T_Ntpport",
  "T_NtpSignDsocket", "T_Orphan", "T_Orphanwait", "T_Panic", "T_Peer",
  "T_Peerstats", "T_Phone", "T_Pid", "T_Pidfile", "T_Pool", "T_Port",
  "T_Preempt", "T_Prefer", "T_Protostats", "T_Pw", "T_Qos", "T_Randfile",
  "T_Rawstats", "T_Refid", "T_Requestkey", "T_Reset", "T_Restrict",
  "T_Revoke", "T_Saveconfigdir", "T_Server", "T_Setvar", "T_Source",
  "T_Statistics", "T_Stats", "T_Statsdir", "T_Step", "T_Stepout",
  "T_Stratum", "T_String", "T_Sys", "T_Sysstats", "T_Tick", "T_Time1",
  "T_Time2", "T_Timer", "T_Timingstats", "T_Tinker", "T_Tos", "T_Trap",
  "T_True", "T_Trustedkey", "T_Ttl", "T_Type", "T_U_int", "T_Unconfig",
  "T_Unpeer", "T_Version", "T_WanderThreshold", "T_Week", "T_Wildcard",
  "T_Xleave", "T_Year", "T_Flag", "T_EOC", "T_Simulate", "T_Beep_Delay",
  "T_Sim_Duration", "T_Server_Offset", "T_Duration", "T_Freq_Offset",
  "T_Wander", "T_Jitter", "T_Prop_Delay", "T_Proc_Delay", "'='", "'('",
  "')'", "'{'", "'}'", "$accept", "configuration", "command_list",
  "command", "server_command", "client_type", "address", "ip_address",
  "address_fam", "option_list", "option", "option_flag",
  "option_flag_keyword", "option_int", "option_int_keyword", "option_str",
  "option_str_keyword", "unpeer_command", "unpeer_keyword",
  "other_mode_command", "authentication_command", "crypto_command_list",
  "crypto_command", "crypto_str_keyword", "orphan_mode_command",
  "tos_option_list", "tos_option", "tos_option_int_keyword",
  "tos_option_dbl_keyword", "monitoring_command", "stats_list", "stat",
  "filegen_option_list", "filegen_option", "link_nolink", "enable_disable",
  "filegen_type", "access_control_command", "ac_flag_list",
  "access_control_flag", "discard_option_list", "discard_option",
  "discard_option_keyword", "mru_option_list", "mru_option",
  "mru_option_keyword", "fudge_command", "fudge_factor_list",
  "fudge_factor", "fudge_factor_dbl_keyword", "fudge_factor_bool_keyword",
  "system_option_command", "system_option_list", "system_option",
  "system_option_flag_keyword", "system_option_local_flag_keyword",
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
     435,   436,   437,   438,   439,    61,    40,    41,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   190,   191,   192,   192,   192,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   194,
     195,   195,   195,   195,   195,   196,   196,   197,   198,   198,
     199,   199,   200,   200,   200,   201,   202,   202,   202,   202,
     202,   202,   202,   202,   203,   203,   204,   204,   204,   204,
     204,   204,   205,   206,   207,   208,   208,   209,   209,   209,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   211,
     211,   212,   212,   213,   213,   213,   213,   213,   214,   215,
     215,   216,   216,   216,   217,   217,   217,   217,   217,   217,
     218,   218,   218,   218,   219,   219,   219,   220,   220,   221,
     221,   221,   221,   221,   221,   221,   221,   222,   222,   223,
     223,   223,   223,   224,   224,   225,   225,   226,   226,   226,
     226,   226,   226,   226,   227,   227,   227,   227,   227,   227,
     227,   227,   228,   228,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   230,
     230,   231,   232,   232,   232,   233,   233,   234,   235,   235,
     235,   235,   235,   235,   235,   235,   236,   237,   237,   238,
     238,   238,   238,   239,   239,   240,   240,   240,   240,   241,
     241,   242,   242,   243,   243,   244,   244,   244,   244,   244,
     244,   245,   245,   246,   247,   247,   248,   249,   249,   249,
     249,   249,   249,   249,   249,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   251,   251,
     251,   252,   252,   252,   252,   253,   253,   254,   254,   254,
     255,   256,   256,   257,   257,   258,   258,   259,   259,   260,
     261,   261,   262,   262,   263,   263,   263,   263,   264,   264,
     264,   265,   266,   266,   267,   267,   267,   267,   267,   267,
     267,   268,   268,   269,   269,   270,   270,   271,   272,   272,
     273,   273,   274,   274,   274,   275,   275,   276,   277,   278,
     278,   279,   280,   280,   281,   281,   282,   283,   284,   285,
     285,   286,   287,   287,   288,   289,   289,   289,   289,   289
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     2,     1,     2,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     0,
       2,     2,     2,     1,     1,     1,     1,     1,     2,     2,
       1,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     3,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     3,     5,     3,     4,
       4,     3,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     2,     1,     1,     1,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     2,     1,     2,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     2,
       3,     1,     2,     2,     2,     2,     3,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     0,
       4,     1,     0,     0,     2,     2,     2,     2,     1,     1,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     2,     1,     1,     1,     5,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     5,     1,     3,
       2,     3,     1,     1,     2,     1,     5,     4,     3,     2,
       1,     6,     3,     2,     3,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,    23,    57,   218,     0,    69,     0,     0,
     229,     0,   211,     0,     0,   221,     0,   242,     0,     0,
     222,     0,   225,    24,     0,     0,     0,   243,   219,     0,
      22,     0,   223,    21,   224,     0,     0,     0,     0,   226,
      20,     0,     0,     0,   220,     0,     0,     0,     0,     0,
      55,    56,   278,     0,     2,     0,     7,     0,     8,     0,
       9,    10,    13,    11,    12,    14,    15,    16,    17,     0,
       0,     0,   205,     0,   206,    18,     0,     5,    60,    61,
      62,   185,   186,   187,   188,   191,   189,   190,   192,   180,
     182,   183,   184,   152,   153,   154,   124,   150,     0,   227,
     212,   179,    99,   100,   101,   102,   106,   103,   104,   105,
     107,    28,    29,    27,     0,    25,     0,     6,    63,    64,
     239,   213,   238,   271,    58,   158,   159,   160,   161,   162,
     163,   164,   165,   125,   156,     0,    59,    68,   269,   214,
      65,   254,   255,   256,   257,   258,   259,   260,   251,   253,
     132,    28,    29,   132,   132,    25,    66,     0,   215,    94,
      98,    95,   197,   198,   199,   200,   201,   202,   203,   204,
     193,   195,     0,    89,    84,     0,    85,    93,    91,    92,
      90,    88,    86,    87,    78,    80,     0,     0,   233,   265,
       0,    67,   264,   266,   262,   217,     1,     0,     4,    30,
      54,   276,   275,   207,   208,   209,   250,   249,   248,     0,
       0,    77,    73,    74,    75,    76,     0,    70,     0,   181,
     149,   151,   228,    96,   175,   176,   177,   178,     0,     0,
     173,   174,   166,   168,     0,     0,    26,   210,   237,   270,
     155,   157,   268,   252,   128,   132,   132,   131,   126,     0,
       0,    97,   194,   196,   274,   272,   273,    83,    79,    81,
      82,   216,     0,   263,   261,     3,    19,   244,   245,   246,
     241,   247,   240,   282,   283,     0,     0,     0,    72,    71,
     116,   115,     0,   113,   114,     0,   108,   111,   112,   172,
     171,   167,   169,   170,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   133,
     129,   130,   132,   232,     0,     0,   234,     0,    36,    37,
      38,    53,    46,    48,    47,    50,    39,    40,    41,    42,
      49,    51,    43,    31,    32,    35,    33,     0,    34,     0,
       0,     0,     0,   285,     0,   280,     0,   109,   123,   119,
     121,   117,   118,   120,   122,   110,   127,   231,   230,   236,
     235,     0,    44,    45,    52,     0,   279,   277,   284,     0,
     281,   267,   288,     0,     0,     0,     0,     0,   290,     0,
       0,   286,   289,   287,     0,     0,   295,   296,   297,   298,
     299,     0,     0,     0,   291,     0,   293,     0,   292,   294
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    53,    54,    55,    56,    57,   123,   115,   116,   266,
     333,   334,   335,   336,   337,   338,   339,    58,    59,    60,
      61,    80,   217,   218,    62,   184,   185,   186,   187,    63,
     159,   110,   223,   286,   287,   288,   355,    64,   244,   309,
      96,    97,    98,   133,   134,   135,    65,   232,   233,   234,
     235,    66,    89,    90,    91,    92,    67,   170,   171,   172,
      68,    69,    70,    71,   100,   158,   358,   261,   316,   121,
     122,    72,    73,   272,   209,    74,   148,   149,   195,   191,
     192,   193,   139,   124,   257,   203,    75,    76,   275,   276,
     277,   342,   343,   374,   344,   377,   378,   391,   392,   393
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -171
static const yytype_int16 yypact[] =
{
      12,  -136,   -12,  -171,  -171,  -171,    -1,  -171,   280,     6,
    -125,   280,  -171,   121,   -55,  -171,   -88,  -171,   -87,   -83,
    -171,   -81,  -171,  -171,   -55,   274,   -55,  -171,  -171,   -74,
    -171,   -73,  -171,  -171,  -171,    24,    29,   -11,    27,  -171,
    -171,   -63,   121,   -62,  -171,    93,   490,   -58,   -53,    42,
    -171,  -171,  -171,    97,   179,   -71,  -171,   -55,  -171,   -55,
    -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,    -9,
     -48,   -44,  -171,    -2,  -171,  -171,   -82,  -171,  -171,  -171,
     158,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,   280,
    -171,  -171,  -171,  -171,  -171,  -171,     6,  -171,    50,    80,
    -171,   280,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,
    -171,  -171,  -171,  -171,   226,  -171,   -33,   334,  -171,  -171,
    -171,   -81,  -171,  -171,   -55,  -171,  -171,  -171,  -171,  -171,
    -171,  -171,  -171,   274,  -171,    59,   -55,  -171,  -171,   -29,
    -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,    29,  -171,
    -171,    95,    99,  -171,  -171,    45,  -171,   -56,  -171,   121,
    -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,
      93,  -171,    -9,  -171,  -171,     0,  -171,  -171,  -171,  -171,
    -171,  -171,  -171,  -171,   490,  -171,    72,    -9,  -171,  -171,
      77,   -53,  -171,  -171,  -171,    83,  -171,   -28,  -171,  -171,
    -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,    10,
    -147,  -171,  -171,  -171,  -171,  -171,    87,  -171,     5,  -171,
    -171,  -171,  -171,     4,  -171,  -171,  -171,  -171,     7,   100,
    -171,  -171,   226,  -171,    -9,     0,  -171,  -171,  -171,  -171,
    -171,  -171,  -171,  -171,   453,  -171,  -171,   453,   453,   -58,
      14,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,
    -171,   -47,   127,  -171,  -171,  -171,   388,  -171,  -171,  -171,
    -171,  -171,  -171,  -171,  -171,  -121,    -8,   -20,  -171,  -171,
    -171,  -171,    19,  -171,  -171,     3,  -171,  -171,  -171,  -171,
    -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,
    -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,
     453,   453,  -171,   155,   -58,   124,  -171,   125,  -171,  -171,
    -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,
    -171,  -171,  -171,  -171,  -171,  -171,  -171,   -49,  -171,    40,
      11,    16,  -122,  -171,     9,  -171,    -9,  -171,  -171,  -171,
    -171,  -171,  -171,  -171,  -171,  -171,   453,  -171,  -171,  -171,
    -171,     8,  -171,  -171,  -171,   -55,  -171,  -171,  -171,    13,
    -171,  -171,  -171,    17,    25,    -9,    18,  -119,  -171,    43,
      -9,  -171,  -171,  -171,    28,    51,  -171,  -171,  -171,  -171,
    -171,    39,    55,    52,  -171,    67,  -171,    -9,  -171,  -171
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -171,  -171,  -171,   -34,  -171,  -171,   -14,   -36,  -171,  -171,
    -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,
    -171,  -171,  -171,  -171,  -171,  -171,    41,  -171,  -171,  -171,
    -171,   -35,  -171,  -171,  -171,  -171,  -171,  -171,  -150,  -171,
    -171,   102,  -171,  -171,    91,  -171,  -171,  -171,    20,  -171,
    -171,  -171,   232,   -24,  -171,  -171,  -171,  -171,    74,  -171,
    -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,  -171,
     128,  -171,  -171,  -171,  -171,  -171,  -171,    98,  -171,  -171,
      62,  -171,  -171,   231,    23,  -170,  -171,  -171,  -171,   -13,
    -171,  -171,   -79,  -171,  -171,  -171,  -116,  -171,  -127,  -171
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     114,   155,   253,   247,   248,   189,   348,   160,   111,   362,
     112,   188,   314,     1,   267,   150,    93,   260,   340,   340,
     197,     2,   201,   154,    99,     3,     4,     5,   349,   273,
     274,   206,   280,     6,     7,   141,   142,   254,    77,   281,
       8,     9,   282,   199,    10,   200,    78,    11,    12,   202,
     207,    13,   151,   143,   152,   273,   274,    79,   255,    14,
     376,   117,   118,    15,   292,   219,   119,   367,   120,    16,
     381,    17,   268,   208,   269,   137,   138,   219,   283,   315,
      18,    19,   140,   237,    20,   156,   157,   161,    21,    22,
     144,   113,    23,    24,   113,   310,   311,   196,   162,    94,
     194,   204,   350,   198,    95,   205,   210,   284,   221,   351,
     239,   222,    25,    26,    27,   363,   236,   241,   145,    28,
     242,   245,   239,   163,   251,   246,   352,   249,    29,   250,
     259,   153,    30,   190,    31,   262,    32,    33,   113,   164,
     102,   264,   165,    34,   103,   278,   265,    35,    36,    37,
      38,    39,    40,    41,   279,    42,   289,    43,   290,   270,
     256,   317,   356,   313,    44,   346,   345,   285,   347,    45,
      46,    47,   353,    48,    49,   354,   370,    50,    51,   146,
     271,   357,   360,   361,   147,   211,    -6,    52,     2,   364,
     366,   373,     3,     4,     5,   371,   365,   369,   220,   104,
       6,     7,   375,   380,   376,   379,   212,     8,     9,   213,
     384,    10,   166,   312,    11,    12,   385,   383,    13,   386,
     387,   388,   389,   390,   240,   258,    14,   399,   394,   396,
      15,   386,   387,   388,   389,   390,    16,   397,    17,   167,
     168,   398,   105,   101,   252,   169,   243,    18,    19,   238,
     106,    20,   291,   263,   107,    21,    22,   136,   293,    23,
      24,   382,   341,   368,   395,     0,   224,   225,   226,   227,
       0,     0,   108,     0,     0,     0,     0,   109,   359,    25,
      26,    27,     0,     0,     0,     0,    28,    81,   214,     0,
     215,    82,     0,     0,     0,    29,   216,    83,     0,    30,
       0,    31,     0,    32,    33,     0,     0,     0,     0,     0,
      34,     0,     0,     0,    35,    36,    37,    38,    39,    40,
      41,     0,    42,     0,    43,     0,     0,   125,   126,   127,
     128,    44,     0,     0,     0,     0,    45,    46,    47,     0,
      48,    49,     0,     2,    50,    51,    84,     3,     4,     5,
       0,   372,     0,    -6,    52,     6,     7,   129,     0,   130,
     228,   131,     8,     9,     0,   132,    10,     0,     0,    11,
      12,     0,     0,    13,   229,     0,     0,    85,    86,   230,
     231,    14,     0,     0,     0,    15,     0,     0,     0,     0,
       0,    16,     0,    17,    87,     0,   318,     0,     0,     0,
       0,     0,    18,    19,   319,     0,    20,     0,     0,     0,
      21,    22,     0,     0,    23,    24,     0,     0,     0,     0,
       0,     0,     0,     0,    88,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    25,    26,    27,     0,   320,   321,
       0,    28,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,     0,    30,   322,    31,     0,    32,    33,
       0,     0,     0,     0,     0,    34,     0,     0,     0,    35,
      36,    37,    38,    39,    40,    41,   323,    42,     0,    43,
       0,     0,   324,     0,   325,     0,    44,     0,     0,     0,
       0,    45,    46,    47,     0,    48,    49,   294,   326,    50,
      51,     0,   173,     0,     0,   295,     0,     0,   174,    52,
     175,     0,     0,     0,     0,   327,   328,     0,     0,     0,
       0,     0,     0,   296,   297,     0,   298,     0,     0,     0,
       0,     0,   299,     0,     0,   176,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   329,     0,
     330,     0,     0,     0,     0,   331,     0,   300,   301,   332,
       0,   302,   303,     0,   304,   305,   306,     0,   307,     0,
       0,     0,     0,     0,   177,     0,   178,     0,     0,     0,
     179,     0,   180,     0,     0,   181,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   182,   183,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     308
};

static const yytype_int16 yycheck[] =
{
      14,    37,   172,   153,   154,    58,     3,    42,    63,    58,
      65,    47,    59,     1,     4,    26,    10,   187,   140,   140,
      54,     9,    31,    37,   149,    13,    14,    15,    25,   176,
     177,    33,    28,    21,    22,     6,     7,    37,   174,    35,
      28,    29,    38,    57,    32,    59,    58,    35,    36,    58,
      52,    39,    63,    24,    65,   176,   177,    58,    58,    47,
     179,   149,   149,    51,   234,    89,   149,   189,   149,    57,
     189,    59,    62,    75,    64,   149,   149,   101,    74,   126,
      68,    69,    58,   117,    72,    58,   149,   149,    76,    77,
      61,   149,    80,    81,   149,   245,   246,     0,     5,    93,
      58,   149,    99,   174,    98,   149,   188,   103,    58,   106,
     124,    31,   100,   101,   102,   164,   149,    58,    89,   107,
     149,    26,   136,    30,   159,    26,   123,    82,   116,   185,
      58,   142,   120,   186,   122,    58,   124,   125,   149,    46,
      19,    58,    49,   131,    23,    58,   174,   135,   136,   137,
     138,   139,   140,   141,   149,   143,   149,   145,    58,   149,
     160,    34,   312,   149,   152,   185,   174,   163,   149,   157,
     158,   159,   169,   161,   162,   172,   346,   165,   166,   150,
     170,    26,    58,    58,   155,    27,   174,   175,     9,   149,
     174,   178,    13,    14,    15,   187,   185,   188,    96,    78,
      21,    22,   185,   185,   179,   375,    48,    28,    29,    51,
     380,    32,   119,   249,    35,    36,   188,   174,    39,   180,
     181,   182,   183,   184,   133,   184,    47,   397,   189,   174,
      51,   180,   181,   182,   183,   184,    57,   185,    59,   146,
     147,   174,   121,    11,   170,   152,   148,    68,    69,   121,
     129,    72,   232,   191,   133,    76,    77,    26,   235,    80,
      81,   377,   275,   342,   391,    -1,    40,    41,    42,    43,
      -1,    -1,   151,    -1,    -1,    -1,    -1,   156,   314,   100,
     101,   102,    -1,    -1,    -1,    -1,   107,     7,   130,    -1,
     132,    11,    -1,    -1,    -1,   116,   138,    17,    -1,   120,
      -1,   122,    -1,   124,   125,    -1,    -1,    -1,    -1,    -1,
     131,    -1,    -1,    -1,   135,   136,   137,   138,   139,   140,
     141,    -1,   143,    -1,   145,    -1,    -1,    53,    54,    55,
      56,   152,    -1,    -1,    -1,    -1,   157,   158,   159,    -1,
     161,   162,    -1,     9,   165,   166,    66,    13,    14,    15,
      -1,   365,    -1,   174,   175,    21,    22,    83,    -1,    85,
     134,    87,    28,    29,    -1,    91,    32,    -1,    -1,    35,
      36,    -1,    -1,    39,   148,    -1,    -1,    97,    98,   153,
     154,    47,    -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    57,    -1,    59,   114,    -1,     8,    -1,    -1,    -1,
      -1,    -1,    68,    69,    16,    -1,    72,    -1,    -1,    -1,
      76,    77,    -1,    -1,    80,    81,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   144,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   100,   101,   102,    -1,    50,    51,
      -1,   107,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     116,    -1,    -1,    -1,   120,    67,   122,    -1,   124,   125,
      -1,    -1,    -1,    -1,    -1,   131,    -1,    -1,    -1,   135,
     136,   137,   138,   139,   140,   141,    88,   143,    -1,   145,
      -1,    -1,    94,    -1,    96,    -1,   152,    -1,    -1,    -1,
      -1,   157,   158,   159,    -1,   161,   162,    44,   110,   165,
     166,    -1,    12,    -1,    -1,    52,    -1,    -1,    18,   175,
      20,    -1,    -1,    -1,    -1,   127,   128,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    71,    -1,    73,    -1,    -1,    -1,
      -1,    -1,    79,    -1,    -1,    45,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   160,    -1,
     162,    -1,    -1,    -1,    -1,   167,    -1,   104,   105,   171,
      -1,   108,   109,    -1,   111,   112,   113,    -1,   115,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    86,    -1,    -1,    -1,
      90,    -1,    92,    -1,    -1,    95,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     167
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     1,     9,    13,    14,    15,    21,    22,    28,    29,
      32,    35,    36,    39,    47,    51,    57,    59,    68,    69,
      72,    76,    77,    80,    81,   100,   101,   102,   107,   116,
     120,   122,   124,   125,   131,   135,   136,   137,   138,   139,
     140,   141,   143,   145,   152,   157,   158,   159,   161,   162,
     165,   166,   175,   191,   192,   193,   194,   195,   207,   208,
     209,   210,   214,   219,   227,   236,   241,   246,   250,   251,
     252,   253,   261,   262,   265,   276,   277,   174,    58,    58,
     211,     7,    11,    17,    66,    97,    98,   114,   144,   242,
     243,   244,   245,    10,    93,    98,   230,   231,   232,   149,
     254,   242,    19,    23,    78,   121,   129,   133,   151,   156,
     221,    63,    65,   149,   196,   197,   198,   149,   149,   149,
     149,   259,   260,   196,   273,    53,    54,    55,    56,    83,
      85,    87,    91,   233,   234,   235,   273,   149,   149,   272,
      58,     6,     7,    24,    61,    89,   150,   155,   266,   267,
      26,    63,    65,   142,   196,   197,    58,   149,   255,   220,
     221,   149,     5,    30,    46,    49,   119,   146,   147,   152,
     247,   248,   249,    12,    18,    20,    45,    84,    86,    90,
      92,    95,   117,   118,   215,   216,   217,   218,   197,    58,
     186,   269,   270,   271,    58,   268,     0,   193,   174,   196,
     196,    31,    58,   275,   149,   149,    33,    52,    75,   264,
     188,    27,    48,    51,   130,   132,   138,   212,   213,   243,
     231,    58,    31,   222,    40,    41,    42,    43,   134,   148,
     153,   154,   237,   238,   239,   240,   149,   193,   260,   196,
     234,    58,   149,   267,   228,    26,    26,   228,   228,    82,
     185,   221,   248,   275,    37,    58,   160,   274,   216,    58,
     275,   257,    58,   270,    58,   174,   199,     4,    62,    64,
     149,   170,   263,   176,   177,   278,   279,   280,    58,   149,
      28,    35,    38,    74,   103,   163,   223,   224,   225,   149,
      58,   238,   275,   274,    44,    52,    70,    71,    73,    79,
     104,   105,   108,   109,   111,   112,   113,   115,   167,   229,
     228,   228,   197,   149,    59,   126,   258,    34,     8,    16,
      50,    51,    67,    88,    94,    96,   110,   127,   128,   160,
     162,   167,   171,   200,   201,   202,   203,   204,   205,   206,
     140,   279,   281,   282,   284,   174,   185,   149,     3,    25,
      99,   106,   123,   169,   172,   226,   228,    26,   256,   197,
      58,    58,    58,   164,   149,   185,   174,   189,   282,   188,
     275,   187,   196,   178,   283,   185,   179,   285,   286,   275,
     185,   189,   286,   174,   275,   188,   180,   181,   182,   183,
     184,   287,   288,   289,   189,   288,   174,   185,   174,   275
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
#line 358 "ntp_parser.y"
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
#line 392 "ntp_parser.y"
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.peers, my_node);
		}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 411 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), (yyvsp[(1) - (2)].Integer)); }
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 416 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_UNSPEC); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 421 "ntp_parser.y"
    { (yyval.Integer) = AF_INET; }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 423 "ntp_parser.y"
    { (yyval.Integer) = AF_INET6; }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 428 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 430 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 444 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 460 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 462 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_uval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 476 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 490 "ntp_parser.y"
    {
			unpeer_node *my_node;
			
			my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
		}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 511 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 513 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.manycastserver, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 515 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.multicastclient, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 526 "ntp_parser.y"
    {
			attr_val *atrv;
			
			atrv = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			APPEND_G_FIFO(cfgt.vars, atrv);
		}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 533 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 535 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			CONCAT_G_FIFOS(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 540 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 542 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 544 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 546 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 548 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Attr_val_fifo); }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 550 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 555 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 557 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 565 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 567 "ntp_parser.y"
    {
			(yyval.Attr_val) = NULL;
			cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer);
			msyslog(LOG_WARNING,
				"'crypto revoke %d' is deprecated, "
				"please use 'revoke %d' instead.",
				cfgt.auth.revoke, cfgt.auth.revoke);
		}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 592 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.orphan_cmds, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 597 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 602 "ntp_parser.y"
    {	
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 610 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 612 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 614 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 640 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.stats_list, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 642 "ntp_parser.y"
    {
			if (input_from_file) {
				cfgt.stats_dir = (yyvsp[(2) - (2)].String);
			} else {
				YYFREE((yyvsp[(2) - (2)].String));
				yyerror("statsdir remote configuration ignored");
			}
		}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 651 "ntp_parser.y"
    {
			filegen_node *fgn;
			
			fgn = create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.filegen_opts, fgn);
		}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 661 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 666 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 685 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 687 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 695 "ntp_parser.y"
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

  case 110:

/* Line 1464 of yacc.c  */
#line 705 "ntp_parser.y"
    {
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen type remote config ignored");
			}
		}
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 714 "ntp_parser.y"
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

  case 112:

/* Line 1464 of yacc.c  */
#line 729 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 759 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.discard_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 763 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.mru_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 767 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 775 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 783 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 791 "ntp_parser.y"
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

  case 130:

/* Line 1464 of yacc.c  */
#line 806 "ntp_parser.y"
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

  case 131:

/* Line 1464 of yacc.c  */
#line 821 "ntp_parser.y"
    {
			restrict_node *	rn;

			APPEND_G_FIFO((yyvsp[(3) - (3)].Int_fifo), create_int_node((yyvsp[(2) - (3)].Integer)));
			rn = create_restrict_node(
				NULL, NULL, (yyvsp[(3) - (3)].Int_fifo), ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 833 "ntp_parser.y"
    { (yyval.Int_fifo) = NULL; }
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 835 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 861 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 866 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 874 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 885 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 890 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 157:

/* Line 1464 of yacc.c  */
#line 898 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 918 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.fudge, aon);
		}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 928 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 933 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 941 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 943 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 945 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 947 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 968 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.enable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 970 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.disable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 975 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 980 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 988 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 184:

/* Line 1464 of yacc.c  */
#line 990 "ntp_parser.y"
    { 
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			} else {
				char err_str[128];
				
				(yyval.Attr_val) = NULL;
				snprintf(err_str, sizeof(err_str),
					 "enable/disable %s remote configuration ignored",
					 keyword((yyvsp[(1) - (1)].Integer)));
				yyerror(err_str);
			}
		}
    break;

  case 193:

/* Line 1464 of yacc.c  */
#line 1025 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.tinker, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 194:

/* Line 1464 of yacc.c  */
#line 1030 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 195:

/* Line 1464 of yacc.c  */
#line 1035 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 196:

/* Line 1464 of yacc.c  */
#line 1043 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 1066 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 1073 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 1080 "ntp_parser.y"
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

  case 210:

/* Line 1464 of yacc.c  */
#line 1096 "ntp_parser.y"
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

  case 211:

/* Line 1464 of yacc.c  */
#line 1115 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 212:

/* Line 1464 of yacc.c  */
#line 1120 "ntp_parser.y"
    { /* see drift_parm below for actions */ }
    break;

  case 213:

/* Line 1464 of yacc.c  */
#line 1122 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.logconfig, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 214:

/* Line 1464 of yacc.c  */
#line 1124 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.phone, (yyvsp[(2) - (2)].String_fifo)); }
    break;

  case 215:

/* Line 1464 of yacc.c  */
#line 1126 "ntp_parser.y"
    { APPEND_G_FIFO(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 216:

/* Line 1464 of yacc.c  */
#line 1128 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.trap, aon);
		}
    break;

  case 217:

/* Line 1464 of yacc.c  */
#line 1135 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.ttl, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 1158 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1165 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
			av = create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 1174 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, "");
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 1184 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 1190 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 1195 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 1197 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 1205 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 236:

/* Line 1464 of yacc.c  */
#line 1207 "ntp_parser.y"
    {
			(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), estrdup((yyvsp[(2) - (2)].Address_node)->address));
			destroy_address_node((yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 1215 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 238:

/* Line 1464 of yacc.c  */
#line 1220 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 239:

/* Line 1464 of yacc.c  */
#line 1228 "ntp_parser.y"
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

  case 240:

/* Line 1464 of yacc.c  */
#line 1253 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 241:

/* Line 1464 of yacc.c  */
#line 1260 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 1288 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.reset_counters, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 1293 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 1298 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 1322 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 1327 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 1335 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 1340 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 1348 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[(1) - (1)].Integer)); }
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 1354 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_rangeval('-', (yyvsp[(2) - (5)].Integer), (yyvsp[(4) - (5)].Integer)); }
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 1359 "ntp_parser.y"
    {
			(yyval.String_fifo) = (yyvsp[(1) - (2)].String_fifo);
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(2) - (2)].String)));
		}
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 1364 "ntp_parser.y"
    {
			(yyval.String_fifo) = NULL;
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(1) - (1)].String)));
		}
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 1372 "ntp_parser.y"
    {
			(yyval.Address_fifo) = (yyvsp[(1) - (2)].Address_fifo);
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 1377 "ntp_parser.y"
    {
			(yyval.Address_fifo) = NULL;
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(1) - (1)].Address_node));
		}
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 1385 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			} else {
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
			}
		}
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 1393 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 1394 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 1398 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 277:

/* Line 1464 of yacc.c  */
#line 1409 "ntp_parser.y"
    {
			sim_node *sn;
			
			sn =  create_sim_node((yyvsp[(3) - (5)].Attr_val_fifo), (yyvsp[(4) - (5)].Sim_server_fifo));
			APPEND_G_FIFO(cfgt.sim_details, sn);

			/* Revert from ; to \n for end-of-command */
			old_config_style = 1;
		}
    break;

  case 278:

/* Line 1464 of yacc.c  */
#line 1426 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 279:

/* Line 1464 of yacc.c  */
#line 1431 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 280:

/* Line 1464 of yacc.c  */
#line 1436 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 281:

/* Line 1464 of yacc.c  */
#line 1444 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 284:

/* Line 1464 of yacc.c  */
#line 1454 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = (yyvsp[(1) - (2)].Sim_server_fifo);
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(2) - (2)].Sim_server));
		}
    break;

  case 285:

/* Line 1464 of yacc.c  */
#line 1459 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(1) - (1)].Sim_server));
		}
    break;

  case 286:

/* Line 1464 of yacc.c  */
#line 1467 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Sim_script_fifo)); }
    break;

  case 287:

/* Line 1464 of yacc.c  */
#line 1472 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 288:

/* Line 1464 of yacc.c  */
#line 1477 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 289:

/* Line 1464 of yacc.c  */
#line 1482 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = (yyvsp[(1) - (2)].Sim_script_fifo);
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(2) - (2)].Sim_script));
		}
    break;

  case 290:

/* Line 1464 of yacc.c  */
#line 1487 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(1) - (1)].Sim_script));
		}
    break;

  case 291:

/* Line 1464 of yacc.c  */
#line 1495 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Attr_val_fifo)); }
    break;

  case 292:

/* Line 1464 of yacc.c  */
#line 1500 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 293:

/* Line 1464 of yacc.c  */
#line 1505 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 294:

/* Line 1464 of yacc.c  */
#line 1513 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1464 of yacc.c  */
#line 3613 "ntp_parser.c"
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
#line 1524 "ntp_parser.y"


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


