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
#line 105 "../../ntpd/ntp_parser.c"

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
     T_Abbrev = 258,
     T_Age = 259,
     T_All = 260,
     T_Allan = 261,
     T_Allpeers = 262,
     T_Auth = 263,
     T_Autokey = 264,
     T_Automax = 265,
     T_Average = 266,
     T_Bclient = 267,
     T_Beacon = 268,
     T_Broadcast = 269,
     T_Broadcastclient = 270,
     T_Broadcastdelay = 271,
     T_Burst = 272,
     T_Calibrate = 273,
     T_Ceiling = 274,
     T_Clockstats = 275,
     T_Cohort = 276,
     T_ControlKey = 277,
     T_Crypto = 278,
     T_Cryptostats = 279,
     T_Ctl = 280,
     T_Day = 281,
     T_Default = 282,
     T_Digest = 283,
     T_Disable = 284,
     T_Discard = 285,
     T_Dispersion = 286,
     T_Double = 287,
     T_Driftfile = 288,
     T_Drop = 289,
     T_Ellipsis = 290,
     T_Enable = 291,
     T_End = 292,
     T_False = 293,
     T_File = 294,
     T_Filegen = 295,
     T_Flag1 = 296,
     T_Flag2 = 297,
     T_Flag3 = 298,
     T_Flag4 = 299,
     T_Flake = 300,
     T_Floor = 301,
     T_Freq = 302,
     T_Fudge = 303,
     T_Host = 304,
     T_Huffpuff = 305,
     T_Iburst = 306,
     T_Ident = 307,
     T_Ignore = 308,
     T_Incalloc = 309,
     T_Incmem = 310,
     T_Initalloc = 311,
     T_Initmem = 312,
     T_Includefile = 313,
     T_Integer = 314,
     T_Interface = 315,
     T_Intrange = 316,
     T_Io = 317,
     T_Ipv4 = 318,
     T_Ipv4_flag = 319,
     T_Ipv6 = 320,
     T_Ipv6_flag = 321,
     T_Kernel = 322,
     T_Key = 323,
     T_Keys = 324,
     T_Keysdir = 325,
     T_Kod = 326,
     T_Mssntp = 327,
     T_Leapfile = 328,
     T_Limited = 329,
     T_Link = 330,
     T_Listen = 331,
     T_Logconfig = 332,
     T_Logfile = 333,
     T_Loopstats = 334,
     T_Lowpriotrap = 335,
     T_Manycastclient = 336,
     T_Manycastserver = 337,
     T_Mask = 338,
     T_Maxage = 339,
     T_Maxclock = 340,
     T_Maxdepth = 341,
     T_Maxdist = 342,
     T_Maxmem = 343,
     T_Maxpoll = 344,
     T_Mem = 345,
     T_Minclock = 346,
     T_Mindepth = 347,
     T_Mindist = 348,
     T_Minimum = 349,
     T_Minpoll = 350,
     T_Minsane = 351,
     T_Mode = 352,
     T_Mode7 = 353,
     T_Monitor = 354,
     T_Month = 355,
     T_Mru = 356,
     T_Multicastclient = 357,
     T_Nic = 358,
     T_Nolink = 359,
     T_Nomodify = 360,
     T_Nomrulist = 361,
     T_None = 362,
     T_Nonvolatile = 363,
     T_Nopeer = 364,
     T_Noquery = 365,
     T_Noselect = 366,
     T_Noserve = 367,
     T_Notrap = 368,
     T_Notrust = 369,
     T_Ntp = 370,
     T_Ntpport = 371,
     T_NtpSignDsocket = 372,
     T_Orphan = 373,
     T_Orphanwait = 374,
     T_Panic = 375,
     T_Peer = 376,
     T_Peerstats = 377,
     T_Phone = 378,
     T_Pid = 379,
     T_Pidfile = 380,
     T_Pool = 381,
     T_Port = 382,
     T_Preempt = 383,
     T_Prefer = 384,
     T_Protostats = 385,
     T_Pw = 386,
     T_Qos = 387,
     T_Randfile = 388,
     T_Rawstats = 389,
     T_Refid = 390,
     T_Requestkey = 391,
     T_Reset = 392,
     T_Restrict = 393,
     T_Revoke = 394,
     T_Saveconfigdir = 395,
     T_Server = 396,
     T_Setvar = 397,
     T_Source = 398,
     T_Statistics = 399,
     T_Stats = 400,
     T_Statsdir = 401,
     T_Step = 402,
     T_Stepout = 403,
     T_Stratum = 404,
     T_String = 405,
     T_Sys = 406,
     T_Sysstats = 407,
     T_Tick = 408,
     T_Time1 = 409,
     T_Time2 = 410,
     T_Timer = 411,
     T_Timingstats = 412,
     T_Tinker = 413,
     T_Tos = 414,
     T_Trap = 415,
     T_True = 416,
     T_Trustedkey = 417,
     T_Ttl = 418,
     T_Type = 419,
     T_U_int = 420,
     T_Unconfig = 421,
     T_Unpeer = 422,
     T_Version = 423,
     T_WanderThreshold = 424,
     T_Week = 425,
     T_Wildcard = 426,
     T_Xleave = 427,
     T_Year = 428,
     T_Flag = 429,
     T_EOC = 430,
     T_Simulate = 431,
     T_Beep_Delay = 432,
     T_Sim_Duration = 433,
     T_Server_Offset = 434,
     T_Duration = 435,
     T_Freq_Offset = 436,
     T_Wander = 437,
     T_Jitter = 438,
     T_Prop_Delay = 439,
     T_Proc_Delay = 440
   };
#endif
/* Tokens.  */
#define T_Abbrev 258
#define T_Age 259
#define T_All 260
#define T_Allan 261
#define T_Allpeers 262
#define T_Auth 263
#define T_Autokey 264
#define T_Automax 265
#define T_Average 266
#define T_Bclient 267
#define T_Beacon 268
#define T_Broadcast 269
#define T_Broadcastclient 270
#define T_Broadcastdelay 271
#define T_Burst 272
#define T_Calibrate 273
#define T_Ceiling 274
#define T_Clockstats 275
#define T_Cohort 276
#define T_ControlKey 277
#define T_Crypto 278
#define T_Cryptostats 279
#define T_Ctl 280
#define T_Day 281
#define T_Default 282
#define T_Digest 283
#define T_Disable 284
#define T_Discard 285
#define T_Dispersion 286
#define T_Double 287
#define T_Driftfile 288
#define T_Drop 289
#define T_Ellipsis 290
#define T_Enable 291
#define T_End 292
#define T_False 293
#define T_File 294
#define T_Filegen 295
#define T_Flag1 296
#define T_Flag2 297
#define T_Flag3 298
#define T_Flag4 299
#define T_Flake 300
#define T_Floor 301
#define T_Freq 302
#define T_Fudge 303
#define T_Host 304
#define T_Huffpuff 305
#define T_Iburst 306
#define T_Ident 307
#define T_Ignore 308
#define T_Incalloc 309
#define T_Incmem 310
#define T_Initalloc 311
#define T_Initmem 312
#define T_Includefile 313
#define T_Integer 314
#define T_Interface 315
#define T_Intrange 316
#define T_Io 317
#define T_Ipv4 318
#define T_Ipv4_flag 319
#define T_Ipv6 320
#define T_Ipv6_flag 321
#define T_Kernel 322
#define T_Key 323
#define T_Keys 324
#define T_Keysdir 325
#define T_Kod 326
#define T_Mssntp 327
#define T_Leapfile 328
#define T_Limited 329
#define T_Link 330
#define T_Listen 331
#define T_Logconfig 332
#define T_Logfile 333
#define T_Loopstats 334
#define T_Lowpriotrap 335
#define T_Manycastclient 336
#define T_Manycastserver 337
#define T_Mask 338
#define T_Maxage 339
#define T_Maxclock 340
#define T_Maxdepth 341
#define T_Maxdist 342
#define T_Maxmem 343
#define T_Maxpoll 344
#define T_Mem 345
#define T_Minclock 346
#define T_Mindepth 347
#define T_Mindist 348
#define T_Minimum 349
#define T_Minpoll 350
#define T_Minsane 351
#define T_Mode 352
#define T_Mode7 353
#define T_Monitor 354
#define T_Month 355
#define T_Mru 356
#define T_Multicastclient 357
#define T_Nic 358
#define T_Nolink 359
#define T_Nomodify 360
#define T_Nomrulist 361
#define T_None 362
#define T_Nonvolatile 363
#define T_Nopeer 364
#define T_Noquery 365
#define T_Noselect 366
#define T_Noserve 367
#define T_Notrap 368
#define T_Notrust 369
#define T_Ntp 370
#define T_Ntpport 371
#define T_NtpSignDsocket 372
#define T_Orphan 373
#define T_Orphanwait 374
#define T_Panic 375
#define T_Peer 376
#define T_Peerstats 377
#define T_Phone 378
#define T_Pid 379
#define T_Pidfile 380
#define T_Pool 381
#define T_Port 382
#define T_Preempt 383
#define T_Prefer 384
#define T_Protostats 385
#define T_Pw 386
#define T_Qos 387
#define T_Randfile 388
#define T_Rawstats 389
#define T_Refid 390
#define T_Requestkey 391
#define T_Reset 392
#define T_Restrict 393
#define T_Revoke 394
#define T_Saveconfigdir 395
#define T_Server 396
#define T_Setvar 397
#define T_Source 398
#define T_Statistics 399
#define T_Stats 400
#define T_Statsdir 401
#define T_Step 402
#define T_Stepout 403
#define T_Stratum 404
#define T_String 405
#define T_Sys 406
#define T_Sysstats 407
#define T_Tick 408
#define T_Time1 409
#define T_Time2 410
#define T_Timer 411
#define T_Timingstats 412
#define T_Tinker 413
#define T_Tos 414
#define T_Trap 415
#define T_True 416
#define T_Trustedkey 417
#define T_Ttl 418
#define T_Type 419
#define T_U_int 420
#define T_Unconfig 421
#define T_Unpeer 422
#define T_Version 423
#define T_WanderThreshold 424
#define T_Week 425
#define T_Wildcard 426
#define T_Xleave 427
#define T_Year 428
#define T_Flag 429
#define T_EOC 430
#define T_Simulate 431
#define T_Beep_Delay 432
#define T_Sim_Duration 433
#define T_Server_Offset 434
#define T_Duration 435
#define T_Freq_Offset 436
#define T_Wander 437
#define T_Jitter 438
#define T_Prop_Delay 439
#define T_Proc_Delay 440




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
#line 532 "../../ntpd/ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 544 "../../ntpd/ntp_parser.c"

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
#define YYLAST   598

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  191
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  100
/* YYNRULES -- Number of rules.  */
#define YYNRULES  300
/* YYNRULES -- Number of states.  */
#define YYNSTATES  402

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   440

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     187,   188,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   186,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   189,     2,   190,     2,     2,     2,     2,
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
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185
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
     400,   403,   406,   409,   412,   414,   416,   418,   420,   422,
     424,   427,   430,   433,   435,   437,   439,   441,   443,   445,
     447,   449,   451,   453,   455,   458,   461,   463,   466,   468,
     470,   472,   474,   476,   478,   480,   482,   484,   486,   489,
     492,   495,   499,   501,   504,   507,   510,   513,   517,   520,
     522,   524,   526,   528,   530,   532,   534,   536,   538,   540,
     543,   544,   549,   551,   552,   553,   556,   559,   562,   565,
     567,   569,   573,   577,   579,   581,   583,   585,   587,   589,
     591,   593,   595,   598,   601,   603,   605,   607,   609,   611,
     613,   615,   617,   620,   622,   625,   627,   629,   631,   637,
     640,   642,   645,   647,   649,   651,   653,   655,   657,   663,
     665,   669,   672,   676,   678,   680,   683,   685,   691,   696,
     700,   703,   705,   712,   716,   719,   723,   725,   727,   729,
     731
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     192,     0,    -1,   193,    -1,   193,   194,   175,    -1,   194,
     175,    -1,     1,   175,    -1,    -1,   195,    -1,   208,    -1,
     210,    -1,   211,    -1,   220,    -1,   228,    -1,   215,    -1,
     237,    -1,   242,    -1,   247,    -1,   251,    -1,   277,    -1,
     196,   197,   200,    -1,   141,    -1,   126,    -1,   121,    -1,
      14,    -1,    81,    -1,   198,    -1,   199,   150,    -1,   150,
      -1,    64,    -1,    66,    -1,    -1,   200,   201,    -1,   202,
      -1,   204,    -1,   206,    -1,   203,    -1,     9,    -1,    17,
      -1,    51,    -1,   111,    -1,   128,    -1,   129,    -1,   161,
      -1,   172,    -1,   205,    59,    -1,   205,   165,    -1,    68,
      -1,    95,    -1,    89,    -1,   163,    -1,    97,    -1,   168,
      -1,   207,   150,    -1,    52,    -1,   209,   197,    -1,   166,
      -1,   167,    -1,    15,    -1,    82,   274,    -1,   102,   274,
      -1,    10,    59,    -1,    22,    59,    -1,    23,   212,    -1,
      69,   150,    -1,    70,   150,    -1,   136,    59,    -1,   139,
      59,    -1,   162,   270,    -1,   117,   150,    -1,    -1,   212,
     213,    -1,   214,   150,    -1,   139,    59,    -1,    49,    -1,
      52,    -1,   131,    -1,   133,    -1,    28,    -1,   159,   216,
      -1,   216,   217,    -1,   217,    -1,   218,    59,    -1,   219,
     276,    -1,    21,   275,    -1,    19,    -1,    46,    -1,   118,
      -1,   119,    -1,    96,    -1,    13,    -1,    93,    -1,    87,
      -1,    91,    -1,    85,    -1,   144,   221,    -1,   146,   150,
      -1,    40,   222,   223,    -1,   221,   222,    -1,   222,    -1,
      20,    -1,    24,    -1,    79,    -1,   122,    -1,   134,    -1,
     152,    -1,   157,    -1,   130,    -1,    -1,   223,   224,    -1,
      39,   150,    -1,   164,   227,    -1,   225,    -1,   226,    -1,
      75,    -1,   104,    -1,    36,    -1,    29,    -1,   107,    -1,
     124,    -1,    26,    -1,   170,    -1,   100,    -1,   173,    -1,
       4,    -1,    30,   231,    -1,   101,   234,    -1,   138,   197,
     229,    -1,   138,   198,    83,   198,   229,    -1,   138,    27,
     229,    -1,   138,    64,    27,   229,    -1,   138,    66,    27,
     229,    -1,   138,   143,   229,    -1,    -1,   229,   230,    -1,
      45,    -1,    53,    -1,    71,    -1,    72,    -1,    74,    -1,
      80,    -1,   105,    -1,   106,    -1,   109,    -1,   110,    -1,
     112,    -1,   113,    -1,   114,    -1,   116,    -1,   168,    -1,
     231,   232,    -1,   232,    -1,   233,    59,    -1,    11,    -1,
      94,    -1,    99,    -1,   234,   235,    -1,   235,    -1,   236,
      59,    -1,    54,    -1,    55,    -1,    56,    -1,    57,    -1,
      84,    -1,    86,    -1,    88,    -1,    92,    -1,    48,   197,
     238,    -1,   238,   239,    -1,   239,    -1,   240,   276,    -1,
     241,   275,    -1,   149,    59,    -1,     3,   150,    -1,   135,
     150,    -1,   154,    -1,   155,    -1,    41,    -1,    42,    -1,
      43,    -1,    44,    -1,    36,   243,    -1,    29,   243,    -1,
     243,   244,    -1,   244,    -1,   245,    -1,   246,    -1,     8,
      -1,    12,    -1,    18,    -1,    67,    -1,    99,    -1,   115,
      -1,    98,    -1,   145,    -1,   158,   248,    -1,   248,   249,
      -1,   249,    -1,   250,   276,    -1,     6,    -1,    31,    -1,
      47,    -1,    50,    -1,   120,    -1,   147,    -1,   148,    -1,
     153,    -1,   262,    -1,   266,    -1,   252,   276,    -1,   253,
     150,    -1,   254,   150,    -1,    58,   150,   194,    -1,    37,
      -1,    33,   255,    -1,    77,   260,    -1,   123,   273,    -1,
     142,   256,    -1,   160,   198,   258,    -1,   163,   269,    -1,
      16,    -1,   108,    -1,   153,    -1,    52,    -1,    73,    -1,
     125,    -1,   132,    -1,    78,    -1,   140,    -1,   150,    -1,
     150,    32,    -1,    -1,   150,   186,   150,   257,    -1,    27,
      -1,    -1,    -1,   258,   259,    -1,   127,    59,    -1,    60,
     198,    -1,   260,   261,    -1,   261,    -1,   150,    -1,   263,
     265,   264,    -1,   263,   265,   150,    -1,    60,    -1,   103,
      -1,     5,    -1,    63,    -1,    65,    -1,   171,    -1,    76,
      -1,    53,    -1,    34,    -1,   137,   267,    -1,   267,   268,
      -1,   268,    -1,     7,    -1,     8,    -1,    25,    -1,    62,
      -1,    90,    -1,   151,    -1,   156,    -1,   269,    59,    -1,
      59,    -1,   270,   271,    -1,   271,    -1,    59,    -1,   272,
      -1,   187,    59,    35,    59,   188,    -1,   273,   150,    -1,
     150,    -1,   274,   197,    -1,   197,    -1,    59,    -1,   161,
      -1,    38,    -1,    59,    -1,    32,    -1,   278,   189,   279,
     282,   190,    -1,   176,    -1,   279,   280,   175,    -1,   280,
     175,    -1,   281,   186,   276,    -1,   177,    -1,   178,    -1,
     282,   283,    -1,   283,    -1,   285,   189,   284,   286,   190,
      -1,   179,   186,   276,   175,    -1,   141,   186,   197,    -1,
     286,   287,    -1,   287,    -1,   180,   186,   276,   189,   288,
     190,    -1,   288,   289,   175,    -1,   289,   175,    -1,   290,
     186,   276,    -1,   181,    -1,   182,    -1,   183,    -1,   184,
      -1,   185,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   352,   352,   356,   357,   358,   372,   373,   374,   375,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   392,
     402,   403,   404,   405,   406,   410,   411,   416,   421,   423,
     429,   430,   438,   439,   440,   444,   449,   450,   451,   452,
     453,   454,   455,   456,   460,   462,   467,   468,   469,   470,
     471,   472,   476,   481,   490,   500,   501,   511,   513,   515,
     526,   533,   535,   540,   542,   544,   546,   548,   550,   556,
     557,   565,   567,   579,   580,   581,   582,   583,   592,   597,
     602,   610,   612,   614,   619,   620,   621,   622,   623,   624,
     628,   629,   630,   631,   640,   642,   651,   661,   666,   674,
     675,   676,   677,   678,   679,   680,   681,   686,   687,   695,
     705,   714,   729,   734,   735,   739,   740,   744,   745,   746,
     747,   748,   749,   750,   759,   763,   767,   775,   783,   791,
     806,   821,   834,   835,   843,   844,   845,   846,   847,   848,
     849,   850,   851,   852,   853,   854,   855,   856,   857,   861,
     866,   874,   879,   880,   881,   885,   890,   898,   903,   904,
     905,   906,   907,   908,   909,   910,   918,   928,   933,   941,
     943,   945,   947,   949,   954,   955,   959,   960,   961,   962,
     970,   972,   977,   982,   990,   992,  1009,  1010,  1011,  1012,
    1013,  1014,  1018,  1019,  1027,  1032,  1037,  1045,  1050,  1051,
    1052,  1053,  1054,  1055,  1056,  1057,  1066,  1067,  1068,  1075,
    1082,  1098,  1117,  1122,  1124,  1126,  1128,  1130,  1137,  1142,
    1143,  1144,  1148,  1149,  1150,  1151,  1155,  1156,  1160,  1167,
    1177,  1186,  1191,  1193,  1198,  1199,  1207,  1209,  1217,  1222,
    1230,  1255,  1262,  1272,  1273,  1277,  1278,  1279,  1280,  1284,
    1285,  1286,  1290,  1295,  1300,  1308,  1309,  1310,  1311,  1312,
    1313,  1314,  1324,  1329,  1337,  1342,  1350,  1352,  1356,  1361,
    1366,  1374,  1379,  1387,  1396,  1397,  1401,  1402,  1411,  1429,
    1433,  1438,  1446,  1451,  1452,  1456,  1461,  1469,  1474,  1479,
    1484,  1489,  1497,  1502,  1507,  1515,  1520,  1521,  1522,  1523,
    1524
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_Abbrev", "T_Age", "T_All", "T_Allan",
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
     435,   436,   437,   438,   439,   440,    61,    40,    41,   123,
     125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   191,   192,   193,   193,   193,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   195,
     196,   196,   196,   196,   196,   197,   197,   198,   199,   199,
     200,   200,   201,   201,   201,   202,   203,   203,   203,   203,
     203,   203,   203,   203,   204,   204,   205,   205,   205,   205,
     205,   205,   206,   207,   208,   209,   209,   210,   210,   210,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   212,
     212,   213,   213,   214,   214,   214,   214,   214,   215,   216,
     216,   217,   217,   217,   218,   218,   218,   218,   218,   218,
     219,   219,   219,   219,   220,   220,   220,   221,   221,   222,
     222,   222,   222,   222,   222,   222,   222,   223,   223,   224,
     224,   224,   224,   225,   225,   226,   226,   227,   227,   227,
     227,   227,   227,   227,   228,   228,   228,   228,   228,   228,
     228,   228,   229,   229,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   231,
     231,   232,   233,   233,   233,   234,   234,   235,   236,   236,
     236,   236,   236,   236,   236,   236,   237,   238,   238,   239,
     239,   239,   239,   239,   240,   240,   241,   241,   241,   241,
     242,   242,   243,   243,   244,   244,   245,   245,   245,   245,
     245,   245,   246,   246,   247,   248,   248,   249,   250,   250,
     250,   250,   250,   250,   250,   250,   251,   251,   251,   251,
     251,   251,   251,   251,   251,   251,   251,   251,   251,   252,
     252,   252,   253,   253,   253,   253,   254,   254,   255,   255,
     255,   256,   257,   257,   258,   258,   259,   259,   260,   260,
     261,   262,   262,   263,   263,   264,   264,   264,   264,   265,
     265,   265,   266,   267,   267,   268,   268,   268,   268,   268,
     268,   268,   269,   269,   270,   270,   271,   271,   272,   273,
     273,   274,   274,   275,   275,   275,   276,   276,   277,   278,
     279,   279,   280,   281,   281,   282,   282,   283,   284,   285,
     286,   286,   287,   288,   288,   289,   290,   290,   290,   290,
     290
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
       2,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     3,     1,     2,     2,     2,     2,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       0,     4,     1,     0,     0,     2,     2,     2,     2,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     2,     1,     1,     1,     5,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     5,     1,
       3,     2,     3,     1,     1,     2,     1,     5,     4,     3,
       2,     1,     6,     3,     2,     3,     1,     1,     1,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,    23,    57,   219,     0,    69,     0,     0,
     230,     0,   212,     0,     0,   222,     0,   243,     0,     0,
     223,     0,   226,    24,     0,     0,     0,   244,   220,     0,
      22,     0,   224,    21,   225,     0,     0,     0,     0,   227,
      20,     0,     0,     0,   221,     0,     0,     0,     0,     0,
      55,    56,   279,     0,     2,     0,     7,     0,     8,     0,
       9,    10,    13,    11,    12,    14,    15,    16,    17,     0,
       0,     0,   206,     0,   207,    18,     0,     5,    60,    61,
      62,   186,   187,   188,   189,   192,   190,   191,   193,   181,
     183,   184,   185,   152,   153,   154,   124,   150,     0,   228,
     213,   180,    99,   100,   101,   102,   106,   103,   104,   105,
     107,    28,    29,    27,     0,    25,     0,     6,    63,    64,
     240,   214,   239,   272,    58,   158,   159,   160,   161,   162,
     163,   164,   165,   125,   156,     0,    59,    68,   270,   215,
      65,   255,   256,   257,   258,   259,   260,   261,   252,   254,
     132,    28,    29,   132,   132,    25,    66,     0,   216,    94,
      98,    95,   198,   199,   200,   201,   202,   203,   204,   205,
     194,   196,     0,    89,    84,     0,    85,    93,    91,    92,
      90,    88,    86,    87,    78,    80,     0,     0,   234,   266,
       0,    67,   265,   267,   263,   218,     1,     0,     4,    30,
      54,   277,   276,   208,   209,   210,   251,   250,   249,     0,
       0,    77,    73,    74,    75,    76,     0,    70,     0,   182,
     149,   151,   229,    96,     0,   176,   177,   178,   179,     0,
       0,   174,   175,   166,   168,     0,     0,    26,   211,   238,
     271,   155,   157,   269,   253,   128,   132,   132,   131,   126,
       0,     0,    97,   195,   197,   275,   273,   274,    83,    79,
      81,    82,   217,     0,   264,   262,     3,    19,   245,   246,
     247,   242,   248,   241,   283,   284,     0,     0,     0,    72,
      71,   116,   115,     0,   113,   114,     0,   108,   111,   112,
     172,   173,   171,   167,   169,   170,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   133,   129,   130,   132,   233,     0,     0,   235,     0,
      36,    37,    38,    53,    46,    48,    47,    50,    39,    40,
      41,    42,    49,    51,    43,    31,    32,    35,    33,     0,
      34,     0,     0,     0,     0,   286,     0,   281,     0,   109,
     123,   119,   121,   117,   118,   120,   122,   110,   127,   232,
     231,   237,   236,     0,    44,    45,    52,     0,   280,   278,
     285,     0,   282,   268,   289,     0,     0,     0,     0,     0,
     291,     0,     0,   287,   290,   288,     0,     0,   296,   297,
     298,   299,   300,     0,     0,     0,   292,     0,   294,     0,
     293,   295
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    53,    54,    55,    56,    57,   123,   115,   116,   267,
     335,   336,   337,   338,   339,   340,   341,    58,    59,    60,
      61,    80,   217,   218,    62,   184,   185,   186,   187,    63,
     159,   110,   223,   287,   288,   289,   357,    64,   245,   311,
      96,    97,    98,   133,   134,   135,    65,   233,   234,   235,
     236,    66,    89,    90,    91,    92,    67,   170,   171,   172,
      68,    69,    70,    71,   100,   158,   360,   262,   318,   121,
     122,    72,    73,   273,   209,    74,   148,   149,   195,   191,
     192,   193,   139,   124,   258,   203,    75,    76,   276,   277,
     278,   344,   345,   376,   346,   379,   380,   393,   394,   395
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -172
static const yytype_int16 yypact[] =
{
       4,  -171,   -21,  -172,  -172,  -172,   -13,  -172,   222,    38,
    -135,   222,  -172,   114,    23,  -172,  -100,  -172,   -92,   -91,
    -172,   -87,  -172,  -172,    23,   316,    23,  -172,  -172,   -80,
    -172,   -75,  -172,  -172,  -172,    10,     5,   120,    19,  -172,
    -172,   -71,   114,   -70,  -172,    70,   479,   -60,   -56,    39,
    -172,  -172,  -172,    99,   173,   -72,  -172,    23,  -172,    23,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,    -8,
     -46,   -42,  -172,     8,  -172,  -172,   -78,  -172,  -172,  -172,
     251,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,   222,
    -172,  -172,  -172,  -172,  -172,  -172,    38,  -172,    54,    84,
    -172,   222,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,
    -172,  -172,  -172,  -172,    50,  -172,   -32,   328,  -172,  -172,
    -172,   -87,  -172,  -172,    23,  -172,  -172,  -172,  -172,  -172,
    -172,  -172,  -172,   316,  -172,    60,    23,  -172,  -172,   -27,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,     5,  -172,
    -172,   112,   122,  -172,  -172,    45,  -172,   -34,  -172,   114,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,
      70,  -172,    -8,  -172,  -172,   -28,  -172,  -172,  -172,  -172,
    -172,  -172,  -172,  -172,   479,  -172,    92,    -8,  -172,  -172,
      95,   -56,  -172,  -172,  -172,    96,  -172,   -17,  -172,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,     3,
    -123,  -172,  -172,  -172,  -172,  -172,   100,  -172,    15,  -172,
    -172,  -172,  -172,    -4,    18,  -172,  -172,  -172,  -172,    26,
     110,  -172,  -172,    50,  -172,    -8,   -28,  -172,  -172,  -172,
    -172,  -172,  -172,  -172,  -172,   307,  -172,  -172,   307,   307,
     -60,    27,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,
    -172,  -172,   -44,   146,  -172,  -172,  -172,   382,  -172,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,   -81,    16,     6,  -172,
    -172,  -172,  -172,    44,  -172,  -172,     2,  -172,  -172,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,
    -172,  -172,   307,   307,  -172,   170,   -60,   139,  -172,   141,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,   -30,
    -172,    51,    22,    36,  -102,  -172,    30,  -172,    -8,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,   307,  -172,
    -172,  -172,  -172,    28,  -172,  -172,  -172,    23,  -172,  -172,
    -172,    41,  -172,  -172,  -172,    29,    42,    -8,    40,  -133,
    -172,    49,    -8,  -172,  -172,  -172,    43,   101,  -172,  -172,
    -172,  -172,  -172,    75,    52,    53,  -172,    62,  -172,    -8,
    -172,  -172
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -172,  -172,  -172,   -45,  -172,  -172,   -14,   -36,  -172,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,    57,  -172,  -172,  -172,
    -172,   -35,  -172,  -172,  -172,  -172,  -172,  -172,  -132,  -172,
    -172,   132,  -172,  -172,   102,  -172,  -172,  -172,    12,  -172,
    -172,  -172,   227,   -53,  -172,  -172,  -172,  -172,    77,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,
     128,  -172,  -172,  -172,  -172,  -172,  -172,   104,  -172,  -172,
      71,  -172,  -172,   235,    17,  -170,  -172,  -172,  -172,   -12,
    -172,  -172,   -77,  -172,  -172,  -172,  -111,  -172,  -124,  -172
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     114,   155,   254,   189,    77,     1,   350,   160,   268,   197,
     255,   188,   141,   142,     2,    99,   316,   261,     3,     4,
       5,   248,   249,   154,   201,   281,     6,     7,   351,   364,
     143,   256,   282,     8,     9,   283,   219,    10,    78,   342,
      11,    12,   206,   199,    13,   200,    79,   378,   219,    93,
     117,   202,    14,   224,   274,   275,    15,   383,   118,   119,
     342,   207,    16,   120,    17,   294,   269,   144,   270,   140,
     137,   284,   238,    18,    19,   138,   162,    20,   156,   157,
     161,    21,    22,   317,   208,    23,    24,   111,   369,   112,
     113,   225,   226,   227,   228,   145,   274,   275,   194,   196,
     285,   163,   352,   198,   204,    25,    26,    27,   205,   353,
     240,   210,    28,   221,   312,   313,   222,   164,   237,   242,
     165,    29,   240,   243,   252,    30,   354,    31,   250,    32,
      33,   190,    94,   257,   102,   365,    34,    95,   103,   246,
      35,    36,    37,    38,    39,    40,    41,   150,    42,   247,
      43,   260,   251,   271,   263,   265,   146,    44,   266,   279,
     286,   147,    45,    46,    47,   280,    48,    49,   290,   292,
      50,    51,   355,   113,   272,   356,   291,   315,   372,    -6,
      52,   319,   358,     2,   151,   229,   152,     3,     4,     5,
     166,   347,   348,   104,   349,     6,     7,   359,   362,   230,
     363,   366,     8,     9,   231,   232,    10,   381,   367,    11,
      12,   368,   386,    13,   314,   377,   373,   167,   168,   371,
     375,    14,   378,   169,   385,    15,   382,   398,   220,   401,
      81,    16,   387,    17,    82,   241,   105,   400,   101,   399,
      83,   259,    18,    19,   106,   293,    20,   253,   107,   239,
      21,    22,   244,   295,    23,    24,   388,   389,   390,   391,
     392,   136,   264,   153,   343,   396,   108,   370,   384,   397,
     113,   109,     0,     0,    25,    26,    27,     0,     0,   211,
     361,    28,   388,   389,   390,   391,   392,     0,     0,    84,
      29,     0,     0,     0,    30,     0,    31,     0,    32,    33,
     212,     0,     0,   213,     0,    34,     0,     0,     0,    35,
      36,    37,    38,    39,    40,    41,     0,    42,     0,    43,
      85,    86,     0,     0,     0,     0,    44,     0,     0,     0,
       0,    45,    46,    47,     0,    48,    49,    87,     2,    50,
      51,     0,     3,     4,     5,     0,     0,     0,    -6,    52,
       6,     7,   296,   374,     0,     0,     0,     8,     9,     0,
     297,    10,     0,     0,    11,    12,     0,    88,    13,     0,
     125,   126,   127,   128,     0,     0,    14,     0,   298,   299,
      15,   300,   214,     0,   215,     0,    16,   301,    17,     0,
     216,   320,     0,     0,     0,     0,     0,    18,    19,   321,
     129,    20,   130,     0,   131,    21,    22,     0,   132,    23,
      24,     0,   302,   303,     0,     0,   304,   305,     0,   306,
     307,   308,     0,   309,     0,     0,     0,     0,     0,    25,
      26,    27,     0,   322,   323,     0,    28,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,     0,    30,
     324,    31,     0,    32,    33,     0,     0,     0,     0,     0,
      34,     0,     0,     0,    35,    36,    37,    38,    39,    40,
      41,   325,    42,     0,    43,   310,     0,   326,     0,   327,
       0,    44,     0,     0,     0,     0,    45,    46,    47,     0,
      48,    49,   173,   328,    50,    51,     0,     0,   174,     0,
     175,     0,     0,     0,    52,     0,     0,     0,     0,     0,
     329,   330,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   176,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   331,     0,   332,     0,     0,     0,     0,
     333,     0,     0,     0,   334,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   177,     0,   178,     0,     0,     0,
     179,     0,   180,     0,     0,   181,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   182,   183
};

static const yytype_int16 yycheck[] =
{
      14,    37,   172,    59,   175,     1,     4,    42,     5,    54,
      38,    47,     7,     8,    10,   150,    60,   187,    14,    15,
      16,   153,   154,    37,    32,    29,    22,    23,    26,    59,
      25,    59,    36,    29,    30,    39,    89,    33,    59,   141,
      36,    37,    34,    57,    40,    59,    59,   180,   101,    11,
     150,    59,    48,     3,   177,   178,    52,   190,   150,   150,
     141,    53,    58,   150,    60,   235,    63,    62,    65,    59,
     150,    75,   117,    69,    70,   150,     6,    73,    59,   150,
     150,    77,    78,   127,    76,    81,    82,    64,   190,    66,
     150,    41,    42,    43,    44,    90,   177,   178,    59,     0,
     104,    31,   100,   175,   150,   101,   102,   103,   150,   107,
     124,   189,   108,    59,   246,   247,    32,    47,   150,    59,
      50,   117,   136,   150,   159,   121,   124,   123,    83,   125,
     126,   187,    94,   161,    20,   165,   132,    99,    24,    27,
     136,   137,   138,   139,   140,   141,   142,    27,   144,    27,
     146,    59,   186,   150,    59,    59,   151,   153,   175,    59,
     164,   156,   158,   159,   160,   150,   162,   163,   150,    59,
     166,   167,   170,   150,   171,   173,   150,   150,   348,   175,
     176,    35,   314,    10,    64,   135,    66,    14,    15,    16,
     120,   175,   186,    79,   150,    22,    23,    27,    59,   149,
      59,   150,    29,    30,   154,   155,    33,   377,   186,    36,
      37,   175,   382,    40,   250,   186,   188,   147,   148,   189,
     179,    48,   180,   153,   175,    52,   186,   175,    96,   399,
       8,    58,   189,    60,    12,   133,   122,   175,    11,   186,
      18,   184,    69,    70,   130,   233,    73,   170,   134,   121,
      77,    78,   148,   236,    81,    82,   181,   182,   183,   184,
     185,    26,   191,   143,   276,   190,   152,   344,   379,   393,
     150,   157,    -1,    -1,   101,   102,   103,    -1,    -1,    28,
     316,   108,   181,   182,   183,   184,   185,    -1,    -1,    67,
     117,    -1,    -1,    -1,   121,    -1,   123,    -1,   125,   126,
      49,    -1,    -1,    52,    -1,   132,    -1,    -1,    -1,   136,
     137,   138,   139,   140,   141,   142,    -1,   144,    -1,   146,
      98,    99,    -1,    -1,    -1,    -1,   153,    -1,    -1,    -1,
      -1,   158,   159,   160,    -1,   162,   163,   115,    10,   166,
     167,    -1,    14,    15,    16,    -1,    -1,    -1,   175,   176,
      22,    23,    45,   367,    -1,    -1,    -1,    29,    30,    -1,
      53,    33,    -1,    -1,    36,    37,    -1,   145,    40,    -1,
      54,    55,    56,    57,    -1,    -1,    48,    -1,    71,    72,
      52,    74,   131,    -1,   133,    -1,    58,    80,    60,    -1,
     139,     9,    -1,    -1,    -1,    -1,    -1,    69,    70,    17,
      84,    73,    86,    -1,    88,    77,    78,    -1,    92,    81,
      82,    -1,   105,   106,    -1,    -1,   109,   110,    -1,   112,
     113,   114,    -1,   116,    -1,    -1,    -1,    -1,    -1,   101,
     102,   103,    -1,    51,    52,    -1,   108,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,    -1,   121,
      68,   123,    -1,   125,   126,    -1,    -1,    -1,    -1,    -1,
     132,    -1,    -1,    -1,   136,   137,   138,   139,   140,   141,
     142,    89,   144,    -1,   146,   168,    -1,    95,    -1,    97,
      -1,   153,    -1,    -1,    -1,    -1,   158,   159,   160,    -1,
     162,   163,    13,   111,   166,   167,    -1,    -1,    19,    -1,
      21,    -1,    -1,    -1,   176,    -1,    -1,    -1,    -1,    -1,
     128,   129,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   161,    -1,   163,    -1,    -1,    -1,    -1,
     168,    -1,    -1,    -1,   172,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    87,    -1,    -1,    -1,
      91,    -1,    93,    -1,    -1,    96,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   118,   119
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     1,    10,    14,    15,    16,    22,    23,    29,    30,
      33,    36,    37,    40,    48,    52,    58,    60,    69,    70,
      73,    77,    78,    81,    82,   101,   102,   103,   108,   117,
     121,   123,   125,   126,   132,   136,   137,   138,   139,   140,
     141,   142,   144,   146,   153,   158,   159,   160,   162,   163,
     166,   167,   176,   192,   193,   194,   195,   196,   208,   209,
     210,   211,   215,   220,   228,   237,   242,   247,   251,   252,
     253,   254,   262,   263,   266,   277,   278,   175,    59,    59,
     212,     8,    12,    18,    67,    98,    99,   115,   145,   243,
     244,   245,   246,    11,    94,    99,   231,   232,   233,   150,
     255,   243,    20,    24,    79,   122,   130,   134,   152,   157,
     222,    64,    66,   150,   197,   198,   199,   150,   150,   150,
     150,   260,   261,   197,   274,    54,    55,    56,    57,    84,
      86,    88,    92,   234,   235,   236,   274,   150,   150,   273,
      59,     7,     8,    25,    62,    90,   151,   156,   267,   268,
      27,    64,    66,   143,   197,   198,    59,   150,   256,   221,
     222,   150,     6,    31,    47,    50,   120,   147,   148,   153,
     248,   249,   250,    13,    19,    21,    46,    85,    87,    91,
      93,    96,   118,   119,   216,   217,   218,   219,   198,    59,
     187,   270,   271,   272,    59,   269,     0,   194,   175,   197,
     197,    32,    59,   276,   150,   150,    34,    53,    76,   265,
     189,    28,    49,    52,   131,   133,   139,   213,   214,   244,
     232,    59,    32,   223,     3,    41,    42,    43,    44,   135,
     149,   154,   155,   238,   239,   240,   241,   150,   194,   261,
     197,   235,    59,   150,   268,   229,    27,    27,   229,   229,
      83,   186,   222,   249,   276,    38,    59,   161,   275,   217,
      59,   276,   258,    59,   271,    59,   175,   200,     5,    63,
      65,   150,   171,   264,   177,   178,   279,   280,   281,    59,
     150,    29,    36,    39,    75,   104,   164,   224,   225,   226,
     150,   150,    59,   239,   276,   275,    45,    53,    71,    72,
      74,    80,   105,   106,   109,   110,   112,   113,   114,   116,
     168,   230,   229,   229,   198,   150,    60,   127,   259,    35,
       9,    17,    51,    52,    68,    89,    95,    97,   111,   128,
     129,   161,   163,   168,   172,   201,   202,   203,   204,   205,
     206,   207,   141,   280,   282,   283,   285,   175,   186,   150,
       4,    26,   100,   107,   124,   170,   173,   227,   229,    27,
     257,   198,    59,    59,    59,   165,   150,   186,   175,   190,
     283,   189,   276,   188,   197,   179,   284,   186,   180,   286,
     287,   276,   186,   190,   287,   175,   276,   189,   181,   182,
     183,   184,   185,   288,   289,   290,   190,   289,   175,   186,
     175,   276
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
#line 359 "ntp_parser.y"
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
#line 393 "ntp_parser.y"
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.peers, my_node);
		}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 412 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), (yyvsp[(1) - (2)].Integer)); }
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 417 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_UNSPEC); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 422 "ntp_parser.y"
    { (yyval.Integer) = AF_INET; }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 424 "ntp_parser.y"
    { (yyval.Integer) = AF_INET6; }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 429 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 431 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 445 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 461 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 463 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_uval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 477 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 491 "ntp_parser.y"
    {
			unpeer_node *my_node;
			
			my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
		}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 512 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 514 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.manycastserver, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 516 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.multicastclient, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 527 "ntp_parser.y"
    {
			attr_val *atrv;
			
			atrv = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			APPEND_G_FIFO(cfgt.vars, atrv);
		}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 534 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 536 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			CONCAT_G_FIFOS(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 541 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 543 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 545 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 547 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 549 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Attr_val_fifo); }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 551 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 556 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 558 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 566 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 568 "ntp_parser.y"
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
#line 593 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.orphan_cmds, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 598 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 603 "ntp_parser.y"
    {	
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 611 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 613 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 615 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 641 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.stats_list, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 643 "ntp_parser.y"
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
#line 652 "ntp_parser.y"
    {
			filegen_node *fgn;
			
			fgn = create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.filegen_opts, fgn);
		}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 662 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 667 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 686 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 688 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 696 "ntp_parser.y"
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
#line 706 "ntp_parser.y"
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
#line 715 "ntp_parser.y"
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
#line 730 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 760 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.discard_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 764 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.mru_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 768 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 776 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 784 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 792 "ntp_parser.y"
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
#line 807 "ntp_parser.y"
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
#line 822 "ntp_parser.y"
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
#line 834 "ntp_parser.y"
    { (yyval.Int_fifo) = NULL; }
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 836 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 862 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 867 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 875 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 886 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 891 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 157:

/* Line 1464 of yacc.c  */
#line 899 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 919 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.fudge, aon);
		}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 929 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 934 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 942 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 944 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 946 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 948 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 950 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 971 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.enable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 973 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.disable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 978 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 983 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 184:

/* Line 1464 of yacc.c  */
#line 991 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 185:

/* Line 1464 of yacc.c  */
#line 993 "ntp_parser.y"
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

  case 194:

/* Line 1464 of yacc.c  */
#line 1028 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.tinker, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 195:

/* Line 1464 of yacc.c  */
#line 1033 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 196:

/* Line 1464 of yacc.c  */
#line 1038 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 197:

/* Line 1464 of yacc.c  */
#line 1046 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 1069 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 1076 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 1083 "ntp_parser.y"
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

  case 211:

/* Line 1464 of yacc.c  */
#line 1099 "ntp_parser.y"
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

  case 212:

/* Line 1464 of yacc.c  */
#line 1118 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 213:

/* Line 1464 of yacc.c  */
#line 1123 "ntp_parser.y"
    { /* see drift_parm below for actions */ }
    break;

  case 214:

/* Line 1464 of yacc.c  */
#line 1125 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.logconfig, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 215:

/* Line 1464 of yacc.c  */
#line 1127 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.phone, (yyvsp[(2) - (2)].String_fifo)); }
    break;

  case 216:

/* Line 1464 of yacc.c  */
#line 1129 "ntp_parser.y"
    { APPEND_G_FIFO(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 217:

/* Line 1464 of yacc.c  */
#line 1131 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.trap, aon);
		}
    break;

  case 218:

/* Line 1464 of yacc.c  */
#line 1138 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.ttl, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1161 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 1168 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
			av = create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 1177 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, "");
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 1187 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 1193 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 1198 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 1200 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 236:

/* Line 1464 of yacc.c  */
#line 1208 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 1210 "ntp_parser.y"
    {
			(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), estrdup((yyvsp[(2) - (2)].Address_node)->address));
			destroy_address_node((yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 238:

/* Line 1464 of yacc.c  */
#line 1218 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 239:

/* Line 1464 of yacc.c  */
#line 1223 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 240:

/* Line 1464 of yacc.c  */
#line 1231 "ntp_parser.y"
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

  case 241:

/* Line 1464 of yacc.c  */
#line 1256 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 242:

/* Line 1464 of yacc.c  */
#line 1263 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 1291 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.reset_counters, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 1296 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 1301 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 1325 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 1330 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 1338 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 1343 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 1351 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[(1) - (1)].Integer)); }
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 1357 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_rangeval('-', (yyvsp[(2) - (5)].Integer), (yyvsp[(4) - (5)].Integer)); }
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 1362 "ntp_parser.y"
    {
			(yyval.String_fifo) = (yyvsp[(1) - (2)].String_fifo);
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(2) - (2)].String)));
		}
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 1367 "ntp_parser.y"
    {
			(yyval.String_fifo) = NULL;
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(1) - (1)].String)));
		}
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 1375 "ntp_parser.y"
    {
			(yyval.Address_fifo) = (yyvsp[(1) - (2)].Address_fifo);
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 1380 "ntp_parser.y"
    {
			(yyval.Address_fifo) = NULL;
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(1) - (1)].Address_node));
		}
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 1388 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			} else {
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
			}
		}
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 1396 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 1397 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 1401 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 278:

/* Line 1464 of yacc.c  */
#line 1412 "ntp_parser.y"
    {
			sim_node *sn;
			
			sn =  create_sim_node((yyvsp[(3) - (5)].Attr_val_fifo), (yyvsp[(4) - (5)].Sim_server_fifo));
			APPEND_G_FIFO(cfgt.sim_details, sn);

			/* Revert from ; to \n for end-of-command */
			old_config_style = 1;
		}
    break;

  case 279:

/* Line 1464 of yacc.c  */
#line 1429 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 280:

/* Line 1464 of yacc.c  */
#line 1434 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 281:

/* Line 1464 of yacc.c  */
#line 1439 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 282:

/* Line 1464 of yacc.c  */
#line 1447 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 285:

/* Line 1464 of yacc.c  */
#line 1457 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = (yyvsp[(1) - (2)].Sim_server_fifo);
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(2) - (2)].Sim_server));
		}
    break;

  case 286:

/* Line 1464 of yacc.c  */
#line 1462 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(1) - (1)].Sim_server));
		}
    break;

  case 287:

/* Line 1464 of yacc.c  */
#line 1470 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Sim_script_fifo)); }
    break;

  case 288:

/* Line 1464 of yacc.c  */
#line 1475 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 289:

/* Line 1464 of yacc.c  */
#line 1480 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 290:

/* Line 1464 of yacc.c  */
#line 1485 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = (yyvsp[(1) - (2)].Sim_script_fifo);
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(2) - (2)].Sim_script));
		}
    break;

  case 291:

/* Line 1464 of yacc.c  */
#line 1490 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(1) - (1)].Sim_script));
		}
    break;

  case 292:

/* Line 1464 of yacc.c  */
#line 1498 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Attr_val_fifo)); }
    break;

  case 293:

/* Line 1464 of yacc.c  */
#line 1503 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 294:

/* Line 1464 of yacc.c  */
#line 1508 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 295:

/* Line 1464 of yacc.c  */
#line 1516 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1464 of yacc.c  */
#line 3626 "../../ntpd/ntp_parser.c"
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
#line 1527 "ntp_parser.y"


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


