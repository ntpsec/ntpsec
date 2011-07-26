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
     T_U_int = 417,
     T_Unconfig = 418,
     T_Unpeer = 419,
     T_Version = 420,
     T_WanderThreshold = 421,
     T_Week = 422,
     T_Wildcard = 423,
     T_Xleave = 424,
     T_Year = 425,
     T_Flag = 426,
     T_EOC = 427,
     T_Simulate = 428,
     T_Beep_Delay = 429,
     T_Sim_Duration = 430,
     T_Server_Offset = 431,
     T_Duration = 432,
     T_Freq_Offset = 433,
     T_Wander = 434,
     T_Jitter = 435,
     T_Prop_Delay = 436,
     T_Proc_Delay = 437
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
#define T_U_int 417
#define T_Unconfig 418
#define T_Unpeer 419
#define T_Version 420
#define T_WanderThreshold 421
#define T_Week 422
#define T_Wildcard 423
#define T_Xleave 424
#define T_Year 425
#define T_Flag 426
#define T_EOC 427
#define T_Simulate 428
#define T_Beep_Delay 429
#define T_Sim_Duration 430
#define T_Server_Offset 431
#define T_Duration 432
#define T_Freq_Offset 433
#define T_Wander 434
#define T_Jitter 435
#define T_Prop_Delay 436
#define T_Proc_Delay 437




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
#line 526 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 538 "ntp_parser.c"

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
#define YYLAST   563

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  188
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  99
/* YYNRULES -- Number of rules.  */
#define YYNRULES  295
/* YYNRULES -- Number of states.  */
#define YYNSTATES  396

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   437

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     184,   185,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   183,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   186,     2,   187,     2,     2,     2,     2,
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
     175,   176,   177,   178,   179,   180,   181,   182
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
     332,   334,   336,   338,   340,   342,   344,   346,   348,   351,
     353,   356,   358,   360,   362,   365,   367,   370,   372,   374,
     376,   378,   380,   382,   384,   386,   390,   393,   395,   398,
     401,   404,   407,   409,   411,   413,   415,   417,   419,   422,
     425,   428,   430,   432,   434,   436,   438,   440,   442,   444,
     446,   449,   452,   454,   457,   459,   461,   463,   465,   467,
     469,   471,   473,   475,   478,   481,   484,   488,   490,   493,
     496,   499,   502,   506,   509,   511,   513,   515,   517,   519,
     521,   523,   525,   527,   529,   532,   533,   538,   540,   541,
     542,   545,   548,   551,   554,   556,   558,   562,   566,   568,
     570,   572,   574,   576,   578,   580,   582,   584,   587,   590,
     592,   594,   596,   598,   600,   602,   604,   606,   609,   611,
     614,   616,   618,   620,   626,   629,   631,   634,   636,   638,
     640,   642,   644,   646,   652,   654,   658,   661,   665,   667,
     669,   672,   674,   680,   685,   689,   692,   694,   701,   705,
     708,   712,   714,   716,   718,   720
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     189,     0,    -1,   190,    -1,   190,   191,   172,    -1,   191,
     172,    -1,     1,   172,    -1,    -1,   192,    -1,   205,    -1,
     207,    -1,   208,    -1,   217,    -1,   225,    -1,   212,    -1,
     234,    -1,   239,    -1,   243,    -1,   247,    -1,   273,    -1,
     193,   194,   197,    -1,   138,    -1,   123,    -1,   118,    -1,
      13,    -1,    80,    -1,   195,    -1,   196,   147,    -1,   147,
      -1,    63,    -1,    65,    -1,    -1,   197,   198,    -1,   199,
      -1,   201,    -1,   203,    -1,   200,    -1,     8,    -1,    16,
      -1,    50,    -1,   108,    -1,   125,    -1,   126,    -1,   158,
      -1,   169,    -1,   202,    58,    -1,   202,   162,    -1,    67,
      -1,    94,    -1,    88,    -1,   160,    -1,    96,    -1,   165,
      -1,   204,   147,    -1,    51,    -1,   206,   194,    -1,   163,
      -1,   164,    -1,    14,    -1,    81,   270,    -1,   100,   270,
      -1,     9,    58,    -1,    21,    58,    -1,    22,   209,    -1,
      68,   147,    -1,    69,   147,    -1,   133,    58,    -1,   136,
      58,    -1,   159,   266,    -1,   114,   147,    -1,    -1,   209,
     210,    -1,   211,   147,    -1,   136,    58,    -1,    48,    -1,
      51,    -1,   128,    -1,   130,    -1,    27,    -1,   156,   213,
      -1,   213,   214,    -1,   214,    -1,   215,    58,    -1,   216,
     272,    -1,    20,   271,    -1,    18,    -1,    45,    -1,   115,
      -1,   116,    -1,    95,    -1,    12,    -1,    92,    -1,    86,
      -1,    90,    -1,    84,    -1,   141,   218,    -1,   143,   147,
      -1,    39,   219,   220,    -1,   218,   219,    -1,   219,    -1,
      19,    -1,    23,    -1,    78,    -1,   119,    -1,   131,    -1,
     149,    -1,   154,    -1,   127,    -1,    -1,   220,   221,    -1,
      38,   147,    -1,   161,   224,    -1,   222,    -1,   223,    -1,
      74,    -1,   102,    -1,    35,    -1,    28,    -1,   104,    -1,
     121,    -1,    25,    -1,   167,    -1,    98,    -1,   170,    -1,
       3,    -1,    29,   228,    -1,    99,   231,    -1,   135,   194,
     226,    -1,   135,   195,    82,   195,   226,    -1,   135,    26,
     226,    -1,   135,    63,    26,   226,    -1,   135,    65,    26,
     226,    -1,   135,   140,   226,    -1,    -1,   226,   227,    -1,
      44,    -1,    52,    -1,    70,    -1,    71,    -1,    73,    -1,
      79,    -1,   103,    -1,   106,    -1,   107,    -1,   109,    -1,
     110,    -1,   111,    -1,   113,    -1,   165,    -1,   228,   229,
      -1,   229,    -1,   230,    58,    -1,    10,    -1,    93,    -1,
      97,    -1,   231,   232,    -1,   232,    -1,   233,    58,    -1,
      53,    -1,    54,    -1,    55,    -1,    56,    -1,    83,    -1,
      85,    -1,    87,    -1,    91,    -1,    47,   194,   235,    -1,
     235,   236,    -1,   236,    -1,   237,   272,    -1,   238,   271,
      -1,   146,    58,    -1,   132,   147,    -1,   151,    -1,   152,
      -1,    40,    -1,    41,    -1,    42,    -1,    43,    -1,    35,
     240,    -1,    28,   240,    -1,   240,   241,    -1,   241,    -1,
     242,    -1,   142,    -1,     7,    -1,    11,    -1,    17,    -1,
      66,    -1,    97,    -1,   112,    -1,   155,   244,    -1,   244,
     245,    -1,   245,    -1,   246,   272,    -1,     5,    -1,    30,
      -1,    46,    -1,    49,    -1,   117,    -1,   144,    -1,   145,
      -1,   258,    -1,   262,    -1,   248,   272,    -1,   249,   147,
      -1,   250,   147,    -1,    57,   147,   191,    -1,    36,    -1,
      32,   251,    -1,    76,   256,    -1,   120,   269,    -1,   139,
     252,    -1,   157,   195,   254,    -1,   160,   265,    -1,    15,
      -1,   105,    -1,   150,    -1,    51,    -1,    72,    -1,   122,
      -1,   129,    -1,    77,    -1,   137,    -1,   147,    -1,   147,
      31,    -1,    -1,   147,   183,   147,   253,    -1,    26,    -1,
      -1,    -1,   254,   255,    -1,   124,    58,    -1,    59,   195,
      -1,   256,   257,    -1,   257,    -1,   147,    -1,   259,   261,
     260,    -1,   259,   261,   147,    -1,    59,    -1,   101,    -1,
       4,    -1,    62,    -1,    64,    -1,   168,    -1,    75,    -1,
      52,    -1,    33,    -1,   134,   263,    -1,   263,   264,    -1,
     264,    -1,     6,    -1,     7,    -1,    24,    -1,    61,    -1,
      89,    -1,   148,    -1,   153,    -1,   265,    58,    -1,    58,
      -1,   266,   267,    -1,   267,    -1,    58,    -1,   268,    -1,
     184,    58,    34,    58,   185,    -1,   269,   147,    -1,   147,
      -1,   270,   194,    -1,   194,    -1,    58,    -1,   158,    -1,
      37,    -1,    58,    -1,    31,    -1,   274,   186,   275,   278,
     187,    -1,   173,    -1,   275,   276,   172,    -1,   276,   172,
      -1,   277,   183,   272,    -1,   174,    -1,   175,    -1,   278,
     279,    -1,   279,    -1,   281,   186,   280,   282,   187,    -1,
     176,   183,   272,   172,    -1,   138,   183,   194,    -1,   282,
     283,    -1,   283,    -1,   177,   183,   272,   186,   284,   187,
      -1,   284,   285,   172,    -1,   285,   172,    -1,   286,   183,
     272,    -1,   178,    -1,   179,    -1,   180,    -1,   181,    -1,
     182,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   348,   348,   352,   353,   354,   368,   369,   370,   371,
     372,   373,   374,   375,   376,   377,   378,   379,   380,   388,
     398,   399,   400,   401,   402,   406,   407,   412,   417,   419,
     425,   426,   434,   435,   436,   440,   445,   446,   447,   448,
     449,   450,   451,   452,   456,   458,   463,   464,   465,   466,
     467,   468,   472,   477,   486,   496,   497,   507,   509,   511,
     522,   529,   531,   536,   538,   540,   542,   544,   546,   552,
     553,   561,   563,   575,   576,   577,   578,   579,   588,   593,
     598,   606,   608,   610,   615,   616,   617,   618,   619,   620,
     624,   625,   626,   627,   636,   638,   647,   657,   662,   670,
     671,   672,   673,   674,   675,   676,   677,   682,   683,   691,
     701,   710,   725,   730,   731,   735,   736,   740,   741,   742,
     743,   744,   745,   746,   755,   759,   763,   771,   779,   787,
     802,   817,   830,   831,   839,   840,   841,   842,   843,   844,
     845,   846,   847,   848,   849,   850,   851,   852,   856,   861,
     869,   874,   875,   876,   880,   885,   893,   898,   899,   900,
     901,   902,   903,   904,   905,   913,   923,   928,   936,   938,
     940,   942,   947,   948,   952,   953,   954,   955,   963,   965,
     970,   975,   983,   985,   997,   998,   999,  1000,  1001,  1002,
    1010,  1015,  1020,  1028,  1033,  1034,  1035,  1036,  1037,  1038,
    1039,  1048,  1049,  1050,  1057,  1064,  1080,  1099,  1104,  1106,
    1108,  1110,  1112,  1119,  1124,  1125,  1126,  1130,  1131,  1132,
    1133,  1137,  1138,  1142,  1149,  1159,  1168,  1173,  1175,  1180,
    1181,  1189,  1191,  1199,  1204,  1212,  1237,  1244,  1254,  1255,
    1259,  1260,  1261,  1262,  1266,  1267,  1268,  1272,  1277,  1282,
    1290,  1291,  1292,  1293,  1294,  1295,  1296,  1306,  1311,  1319,
    1324,  1332,  1334,  1338,  1343,  1348,  1356,  1361,  1369,  1378,
    1379,  1383,  1384,  1393,  1411,  1415,  1420,  1428,  1433,  1434,
    1438,  1443,  1451,  1456,  1461,  1466,  1471,  1479,  1484,  1489,
    1497,  1502,  1503,  1504,  1505,  1506
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
  "T_Ttl", "T_Type", "T_U_int", "T_Unconfig", "T_Unpeer", "T_Version",
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
     435,   436,   437,    61,    40,    41,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   188,   189,   190,   190,   190,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   192,
     193,   193,   193,   193,   193,   194,   194,   195,   196,   196,
     197,   197,   198,   198,   198,   199,   200,   200,   200,   200,
     200,   200,   200,   200,   201,   201,   202,   202,   202,   202,
     202,   202,   203,   204,   205,   206,   206,   207,   207,   207,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   209,
     209,   210,   210,   211,   211,   211,   211,   211,   212,   213,
     213,   214,   214,   214,   215,   215,   215,   215,   215,   215,
     216,   216,   216,   216,   217,   217,   217,   218,   218,   219,
     219,   219,   219,   219,   219,   219,   219,   220,   220,   221,
     221,   221,   221,   222,   222,   223,   223,   224,   224,   224,
     224,   224,   224,   224,   225,   225,   225,   225,   225,   225,
     225,   225,   226,   226,   227,   227,   227,   227,   227,   227,
     227,   227,   227,   227,   227,   227,   227,   227,   228,   228,
     229,   230,   230,   230,   231,   231,   232,   233,   233,   233,
     233,   233,   233,   233,   233,   234,   235,   235,   236,   236,
     236,   236,   237,   237,   238,   238,   238,   238,   239,   239,
     240,   240,   241,   241,   242,   242,   242,   242,   242,   242,
     243,   244,   244,   245,   246,   246,   246,   246,   246,   246,
     246,   247,   247,   247,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   248,   248,   248,   249,   249,   249,
     249,   250,   250,   251,   251,   251,   252,   253,   253,   254,
     254,   255,   255,   256,   256,   257,   258,   258,   259,   259,
     260,   260,   260,   260,   261,   261,   261,   262,   263,   263,
     264,   264,   264,   264,   264,   264,   264,   265,   265,   266,
     266,   267,   267,   268,   269,   269,   270,   270,   271,   271,
     271,   272,   272,   273,   274,   275,   275,   276,   277,   277,
     278,   278,   279,   280,   281,   282,   282,   283,   284,   284,
     285,   286,   286,   286,   286,   286
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
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     1,     1,     1,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     1,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     2,     3,     1,     2,     2,
       2,     2,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     0,     4,     1,     0,     0,
       2,     2,     2,     2,     1,     1,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       1,     1,     1,     5,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     5,     1,     3,     2,     3,     1,     1,
       2,     1,     5,     4,     3,     2,     1,     6,     3,     2,
       3,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,    23,    57,   214,     0,    69,     0,     0,
     225,     0,   207,     0,     0,   217,     0,   238,     0,     0,
     218,     0,   221,    24,     0,     0,     0,   239,   215,     0,
      22,     0,   219,    21,   220,     0,     0,     0,     0,   222,
      20,     0,     0,     0,   216,     0,     0,     0,     0,     0,
      55,    56,   274,     0,     2,     0,     7,     0,     8,     0,
       9,    10,    13,    11,    12,    14,    15,    16,    17,     0,
       0,     0,   201,     0,   202,    18,     0,     5,    60,    61,
      62,   184,   185,   186,   187,   188,   189,   183,   179,   181,
     182,   151,   152,   153,   124,   149,     0,   223,   208,   178,
      99,   100,   101,   102,   106,   103,   104,   105,   107,    28,
      29,    27,     0,    25,     0,     6,    63,    64,   235,   209,
     234,   267,    58,   157,   158,   159,   160,   161,   162,   163,
     164,   125,   155,     0,    59,    68,   265,   210,    65,   250,
     251,   252,   253,   254,   255,   256,   247,   249,   132,    28,
      29,   132,   132,    25,    66,     0,   211,    94,    98,    95,
     194,   195,   196,   197,   198,   199,   200,   190,   192,     0,
      89,    84,     0,    85,    93,    91,    92,    90,    88,    86,
      87,    78,    80,     0,     0,   229,   261,     0,    67,   260,
     262,   258,   213,     1,     0,     4,    30,    54,   272,   271,
     203,   204,   205,   246,   245,   244,     0,     0,    77,    73,
      74,    75,    76,     0,    70,     0,   180,   148,   150,   224,
      96,   174,   175,   176,   177,     0,     0,   172,   173,   165,
     167,     0,     0,    26,   206,   233,   266,   154,   156,   264,
     248,   128,   132,   132,   131,   126,     0,     0,    97,   191,
     193,   270,   268,   269,    83,    79,    81,    82,   212,     0,
     259,   257,     3,    19,   240,   241,   242,   237,   243,   236,
     278,   279,     0,     0,     0,    72,    71,   116,   115,     0,
     113,   114,     0,   108,   111,   112,   171,   170,   166,   168,
     169,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   133,   129,   130,   132,   228,
       0,     0,   230,     0,    36,    37,    38,    53,    46,    48,
      47,    50,    39,    40,    41,    42,    49,    51,    43,    31,
      32,    35,    33,     0,    34,     0,     0,     0,     0,   281,
       0,   276,     0,   109,   123,   119,   121,   117,   118,   120,
     122,   110,   127,   227,   226,   232,   231,     0,    44,    45,
      52,     0,   275,   273,   280,     0,   277,   263,   284,     0,
       0,     0,     0,     0,   286,     0,     0,   282,   285,   283,
       0,     0,   291,   292,   293,   294,   295,     0,     0,     0,
     287,     0,   289,     0,   288,   290
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
     338,   339,   370,   340,   373,   374,   387,   388,   389
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -168
static const yytype_int16 yypact[] =
{
      26,  -139,    -8,  -168,  -168,  -168,     2,  -168,   128,    18,
     -88,   128,  -168,    69,   -43,  -168,   -84,  -168,   -80,   -78,
    -168,   -77,  -168,  -168,   -43,   359,   -43,  -168,  -168,   -76,
    -168,   -75,  -168,  -168,  -168,    23,    25,   -19,    24,  -168,
    -168,   -71,    69,   -63,  -168,    88,   447,   -60,   -55,    31,
    -168,  -168,  -168,    90,   199,   -81,  -168,   -43,  -168,   -43,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,    -2,
     -51,   -50,  -168,     5,  -168,  -168,   -74,  -168,  -168,  -168,
     188,  -168,  -168,  -168,  -168,  -168,  -168,  -168,   128,  -168,
    -168,  -168,  -168,  -168,    18,  -168,    51,    85,  -168,   128,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,   254,  -168,   -34,   352,  -168,  -168,  -168,   -77,
    -168,  -168,   -43,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,   359,  -168,    59,   -43,  -168,  -168,   -28,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,    25,  -168,  -168,    97,
      98,  -168,  -168,    48,  -168,   -47,  -168,    69,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,    88,  -168,    -2,
    -168,  -168,   -16,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,   447,  -168,    74,    -2,  -168,  -168,    83,   -55,  -168,
    -168,  -168,    92,  -168,   -29,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,     4,  -138,  -168,  -168,
    -168,  -168,  -168,    94,  -168,     6,  -168,  -168,  -168,  -168,
     -23,  -168,  -168,  -168,  -168,     7,    99,  -168,  -168,   254,
    -168,    -2,   -16,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,   178,  -168,  -168,   178,   178,   -60,     9,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,   -46,   124,
    -168,  -168,  -168,   388,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -122,     8,   -13,  -168,  -168,  -168,  -168,    27,
    -168,  -168,     1,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,   178,   178,  -168,   151,
     -60,   121,  -168,   126,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,   -52,  -168,    40,    10,    19,  -113,  -168,
      11,  -168,    -2,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,   178,  -168,  -168,  -168,  -168,    16,  -168,  -168,
    -168,   -43,  -168,  -168,  -168,    30,  -168,  -168,  -168,    12,
      15,    -2,    20,  -153,  -168,    35,    -2,  -168,  -168,  -168,
      33,    63,  -168,  -168,  -168,  -168,  -168,    82,    39,    34,
    -168,    44,  -168,    -2,  -168,  -168
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -168,  -168,  -168,   -40,  -168,  -168,   -14,   -36,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,    21,  -168,  -168,  -168,
    -168,   -24,  -168,  -168,  -168,  -168,  -168,  -168,  -142,  -168,
    -168,   130,  -168,  -168,   106,  -168,  -168,  -168,     0,  -168,
    -168,  -168,   236,   -69,  -168,  -168,  -168,    86,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,   133,
    -168,  -168,  -168,  -168,  -168,  -168,   108,  -168,  -168,    67,
    -168,  -168,   233,    41,  -167,  -168,  -168,  -168,    -7,  -168,
    -168,   -72,  -168,  -168,  -168,  -101,  -168,  -110,  -168
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     112,   153,   250,   186,   344,   277,   358,   148,   264,   244,
     245,   185,   278,   310,   194,   279,   336,   257,   158,   216,
     109,   251,   110,   152,   372,   336,   345,     1,    91,   198,
     216,   139,   140,    77,   377,     2,   270,   271,   203,     3,
       4,     5,   252,   196,   149,   197,   150,     6,     7,   141,
      78,   280,   270,   271,     8,     9,   199,   204,    10,    97,
      79,    11,    12,   115,   289,    13,   265,   116,   266,   117,
     118,   135,   136,    14,   363,   234,   155,    15,   311,   281,
     205,   138,   154,    16,   159,    17,   142,   111,   100,   191,
     193,   195,   101,   160,    18,    19,   201,   202,    20,   346,
     306,   307,    21,    22,   111,   347,    23,    24,   236,   218,
     359,    92,   207,   233,   143,    93,   219,   238,   161,   239,
     236,   151,   348,   242,   243,    25,    26,    27,   111,   187,
     246,    28,   256,   248,   162,    81,   247,   163,   282,    82,
      29,   259,   253,   262,    30,    83,    31,   102,    32,    33,
     261,   267,   275,   276,   286,    34,   309,   287,   313,    35,
      36,    37,    38,    39,    40,    41,   352,    42,   349,    43,
     342,   350,   268,   144,   343,   366,    44,   353,   145,   356,
     341,    45,    46,    47,   357,    48,    49,   360,   103,    50,
      51,   362,   372,   361,    84,   371,   104,   365,    -6,    52,
     105,   367,   255,   376,   375,   164,   369,   379,     2,   380,
     308,   392,     3,     4,     5,   208,   394,   393,   106,   381,
       6,     7,   291,   107,   217,    85,   395,     8,     9,   288,
     292,    10,   165,   166,    11,    12,   209,   237,    13,   210,
      86,   382,   383,   384,   385,   386,    14,    99,   293,   294,
      15,   295,   235,   249,   240,   260,    16,   296,    17,   134,
     382,   383,   384,   385,   386,   337,   364,    18,    19,   390,
      87,    20,   378,   290,   355,    21,    22,   391,     0,    23,
      24,   297,     0,     0,   298,   299,     0,   300,   301,   302,
       0,   303,     0,     0,   221,   222,   223,   224,    25,    26,
      27,     0,     0,     0,    28,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,   211,    30,   212,    31,
       0,    32,    33,     0,   213,     0,     0,     0,    34,     0,
       0,     0,    35,    36,    37,    38,    39,    40,    41,     0,
      42,     0,    43,   304,     0,     0,     0,   368,     0,    44,
       0,     0,     0,     0,    45,    46,    47,     0,    48,    49,
       0,     2,    50,    51,     0,     3,     4,     5,     0,     0,
       0,    -6,    52,     6,     7,     0,     0,     0,     0,     0,
       8,     9,     0,     0,    10,     0,   225,    11,    12,     0,
       0,    13,     0,     0,     0,     0,   314,     0,     0,    14,
     226,     0,     0,    15,   315,   227,   228,     0,     0,    16,
       0,    17,   123,   124,   125,   126,     0,     0,     0,     0,
      18,    19,     0,     0,    20,     0,     0,     0,    21,    22,
       0,     0,    23,    24,     0,     0,     0,     0,   316,   317,
       0,     0,   127,     0,   128,     0,   129,     0,     0,     0,
     130,    25,    26,    27,     0,   318,     0,    28,     0,   170,
       0,     0,     0,     0,     0,   171,    29,   172,     0,     0,
      30,     0,    31,     0,    32,    33,   319,     0,     0,     0,
       0,    34,   320,     0,   321,    35,    36,    37,    38,    39,
      40,    41,   173,    42,     0,    43,   322,     0,     0,     0,
       0,     0,    44,     0,     0,     0,     0,    45,    46,    47,
       0,    48,    49,   323,   324,    50,    51,     0,     0,     0,
       0,     0,     0,     0,     0,    52,     0,     0,     0,     0,
       0,   174,     0,   175,     0,     0,     0,   176,     0,   177,
       0,     0,   178,     0,     0,     0,   325,     0,   326,     0,
       0,     0,     0,   327,     0,     0,     0,   328,     0,     0,
       0,     0,   179,   180
};

static const yytype_int16 yycheck[] =
{
      14,    37,   169,    58,     3,    28,    58,    26,     4,   151,
     152,    47,    35,    59,    54,    38,   138,   184,    42,    88,
      63,    37,    65,    37,   177,   138,    25,     1,    10,    31,
      99,     6,     7,   172,   187,     9,   174,   175,    33,    13,
      14,    15,    58,    57,    63,    59,    65,    21,    22,    24,
      58,    74,   174,   175,    28,    29,    58,    52,    32,   147,
      58,    35,    36,   147,   231,    39,    62,   147,    64,   147,
     147,   147,   147,    47,   187,   115,   147,    51,   124,   102,
      75,    58,    58,    57,   147,    59,    61,   147,    19,    58,
       0,   172,    23,     5,    68,    69,   147,   147,    72,    98,
     242,   243,    76,    77,   147,   104,    80,    81,   122,    58,
     162,    93,   186,   147,    89,    97,    31,    58,    30,   147,
     134,   140,   121,    26,    26,    99,   100,   101,   147,   184,
      82,   105,    58,   157,    46,     7,   183,    49,   161,    11,
     114,    58,   158,   172,   118,    17,   120,    78,   122,   123,
      58,   147,    58,   147,   147,   129,   147,    58,    34,   133,
     134,   135,   136,   137,   138,   139,   308,   141,   167,   143,
     183,   170,   168,   148,   147,   342,   150,    26,   153,    58,
     172,   155,   156,   157,    58,   159,   160,   147,   119,   163,
     164,   172,   177,   183,    66,   183,   127,   186,   172,   173,
     131,   185,   181,   183,   371,   117,   176,   172,     9,   376,
     246,   172,    13,    14,    15,    27,   172,   183,   149,   186,
      21,    22,    44,   154,    94,    97,   393,    28,    29,   229,
      52,    32,   144,   145,    35,    36,    48,   131,    39,    51,
     112,   178,   179,   180,   181,   182,    47,    11,    70,    71,
      51,    73,   119,   167,   146,   188,    57,    79,    59,    26,
     178,   179,   180,   181,   182,   272,   338,    68,    69,   187,
     142,    72,   373,   232,   310,    76,    77,   387,    -1,    80,
      81,   103,    -1,    -1,   106,   107,    -1,   109,   110,   111,
      -1,   113,    -1,    -1,    40,    41,    42,    43,    99,   100,
     101,    -1,    -1,    -1,   105,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   114,    -1,    -1,   128,   118,   130,   120,
      -1,   122,   123,    -1,   136,    -1,    -1,    -1,   129,    -1,
      -1,    -1,   133,   134,   135,   136,   137,   138,   139,    -1,
     141,    -1,   143,   165,    -1,    -1,    -1,   361,    -1,   150,
      -1,    -1,    -1,    -1,   155,   156,   157,    -1,   159,   160,
      -1,     9,   163,   164,    -1,    13,    14,    15,    -1,    -1,
      -1,   172,   173,    21,    22,    -1,    -1,    -1,    -1,    -1,
      28,    29,    -1,    -1,    32,    -1,   132,    35,    36,    -1,
      -1,    39,    -1,    -1,    -1,    -1,     8,    -1,    -1,    47,
     146,    -1,    -1,    51,    16,   151,   152,    -1,    -1,    57,
      -1,    59,    53,    54,    55,    56,    -1,    -1,    -1,    -1,
      68,    69,    -1,    -1,    72,    -1,    -1,    -1,    76,    77,
      -1,    -1,    80,    81,    -1,    -1,    -1,    -1,    50,    51,
      -1,    -1,    83,    -1,    85,    -1,    87,    -1,    -1,    -1,
      91,    99,   100,   101,    -1,    67,    -1,   105,    -1,    12,
      -1,    -1,    -1,    -1,    -1,    18,   114,    20,    -1,    -1,
     118,    -1,   120,    -1,   122,   123,    88,    -1,    -1,    -1,
      -1,   129,    94,    -1,    96,   133,   134,   135,   136,   137,
     138,   139,    45,   141,    -1,   143,   108,    -1,    -1,    -1,
      -1,    -1,   150,    -1,    -1,    -1,    -1,   155,   156,   157,
      -1,   159,   160,   125,   126,   163,   164,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   173,    -1,    -1,    -1,    -1,
      -1,    84,    -1,    86,    -1,    -1,    -1,    90,    -1,    92,
      -1,    -1,    95,    -1,    -1,    -1,   158,    -1,   160,    -1,
      -1,    -1,    -1,   165,    -1,    -1,    -1,   169,    -1,    -1,
      -1,    -1,   115,   116
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
     163,   164,   173,   189,   190,   191,   192,   193,   205,   206,
     207,   208,   212,   217,   225,   234,   239,   243,   247,   248,
     249,   250,   258,   259,   262,   273,   274,   172,    58,    58,
     209,     7,    11,    17,    66,    97,   112,   142,   240,   241,
     242,    10,    93,    97,   228,   229,   230,   147,   251,   240,
      19,    23,    78,   119,   127,   131,   149,   154,   219,    63,
      65,   147,   194,   195,   196,   147,   147,   147,   147,   256,
     257,   194,   270,    53,    54,    55,    56,    83,    85,    87,
      91,   231,   232,   233,   270,   147,   147,   269,    58,     6,
       7,    24,    61,    89,   148,   153,   263,   264,    26,    63,
      65,   140,   194,   195,    58,   147,   252,   218,   219,   147,
       5,    30,    46,    49,   117,   144,   145,   244,   245,   246,
      12,    18,    20,    45,    84,    86,    90,    92,    95,   115,
     116,   213,   214,   215,   216,   195,    58,   184,   266,   267,
     268,    58,   265,     0,   191,   172,   194,   194,    31,    58,
     272,   147,   147,    33,    52,    75,   261,   186,    27,    48,
      51,   128,   130,   136,   210,   211,   241,   229,    58,    31,
     220,    40,    41,    42,    43,   132,   146,   151,   152,   235,
     236,   237,   238,   147,   191,   257,   194,   232,    58,   147,
     264,   226,    26,    26,   226,   226,    82,   183,   219,   245,
     272,    37,    58,   158,   271,   214,    58,   272,   254,    58,
     267,    58,   172,   197,     4,    62,    64,   147,   168,   260,
     174,   175,   275,   276,   277,    58,   147,    28,    35,    38,
      74,   102,   161,   221,   222,   223,   147,    58,   236,   272,
     271,    44,    52,    70,    71,    73,    79,   103,   106,   107,
     109,   110,   111,   113,   165,   227,   226,   226,   195,   147,
      59,   124,   255,    34,     8,    16,    50,    51,    67,    88,
      94,    96,   108,   125,   126,   158,   160,   165,   169,   198,
     199,   200,   201,   202,   203,   204,   138,   276,   278,   279,
     281,   172,   183,   147,     3,    25,    98,   104,   121,   167,
     170,   224,   226,    26,   253,   195,    58,    58,    58,   162,
     147,   183,   172,   187,   279,   186,   272,   185,   194,   176,
     280,   183,   177,   282,   283,   272,   183,   187,   283,   172,
     272,   186,   178,   179,   180,   181,   182,   284,   285,   286,
     187,   285,   172,   183,   172,   272
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
#line 355 "ntp_parser.y"
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
#line 389 "ntp_parser.y"
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.peers, my_node);
		}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 408 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), (yyvsp[(1) - (2)].Integer)); }
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 413 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_UNSPEC); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 418 "ntp_parser.y"
    { (yyval.Integer) = AF_INET; }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 420 "ntp_parser.y"
    { (yyval.Integer) = AF_INET6; }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 425 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 427 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 441 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 457 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 459 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_uval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 473 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 487 "ntp_parser.y"
    {
			unpeer_node *my_node;
			
			my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
		}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 508 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 510 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.manycastserver, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 512 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.multicastclient, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 523 "ntp_parser.y"
    {
			attr_val *atrv;
			
			atrv = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			APPEND_G_FIFO(cfgt.vars, atrv);
		}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 530 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 532 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			CONCAT_G_FIFOS(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 537 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 539 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 541 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 543 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 545 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Attr_val_fifo); }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 547 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 552 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 554 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 562 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 564 "ntp_parser.y"
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
#line 589 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.orphan_cmds, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 594 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 599 "ntp_parser.y"
    {	
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 607 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 609 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 611 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 637 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.stats_list, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 639 "ntp_parser.y"
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
#line 648 "ntp_parser.y"
    {
			filegen_node *fgn;
			
			fgn = create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.filegen_opts, fgn);
		}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 658 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 663 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 682 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 684 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 692 "ntp_parser.y"
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
#line 702 "ntp_parser.y"
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
#line 711 "ntp_parser.y"
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
#line 726 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 756 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.discard_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 760 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.mru_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 764 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 772 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 780 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 788 "ntp_parser.y"
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
#line 803 "ntp_parser.y"
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
#line 818 "ntp_parser.y"
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
#line 830 "ntp_parser.y"
    { (yyval.Int_fifo) = NULL; }
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 832 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 857 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 862 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 870 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 881 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 886 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 894 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 914 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.fudge, aon);
		}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 924 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 929 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 937 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 939 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 941 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 943 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 964 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.enable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 966 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.disable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 971 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 976 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 984 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 986 "ntp_parser.y"
    { 
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				yyerror("enable/disable stats remote configuration ignored");
			}
		}
    break;

  case 190:

/* Line 1464 of yacc.c  */
#line 1011 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.tinker, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 191:

/* Line 1464 of yacc.c  */
#line 1016 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 192:

/* Line 1464 of yacc.c  */
#line 1021 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 193:

/* Line 1464 of yacc.c  */
#line 1029 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 203:

/* Line 1464 of yacc.c  */
#line 1051 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 204:

/* Line 1464 of yacc.c  */
#line 1058 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 1065 "ntp_parser.y"
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

  case 206:

/* Line 1464 of yacc.c  */
#line 1081 "ntp_parser.y"
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

  case 207:

/* Line 1464 of yacc.c  */
#line 1100 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 1105 "ntp_parser.y"
    { /* see drift_parm below for actions */ }
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 1107 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.logconfig, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 1109 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.phone, (yyvsp[(2) - (2)].String_fifo)); }
    break;

  case 211:

/* Line 1464 of yacc.c  */
#line 1111 "ntp_parser.y"
    { APPEND_G_FIFO(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 212:

/* Line 1464 of yacc.c  */
#line 1113 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.trap, aon);
		}
    break;

  case 213:

/* Line 1464 of yacc.c  */
#line 1120 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.ttl, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 1143 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 224:

/* Line 1464 of yacc.c  */
#line 1150 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
			av = create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 1159 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, "");
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 1169 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1175 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 1180 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 1182 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 1190 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 1192 "ntp_parser.y"
    {
			(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), estrdup((yyvsp[(2) - (2)].Address_node)->address));
			destroy_address_node((yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 1200 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 1205 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 1213 "ntp_parser.y"
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

  case 236:

/* Line 1464 of yacc.c  */
#line 1238 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 1245 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 247:

/* Line 1464 of yacc.c  */
#line 1273 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.reset_counters, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 1278 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 1283 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 257:

/* Line 1464 of yacc.c  */
#line 1307 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 1312 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 1320 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 1325 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 1333 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[(1) - (1)].Integer)); }
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 1339 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_rangeval('-', (yyvsp[(2) - (5)].Integer), (yyvsp[(4) - (5)].Integer)); }
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 1344 "ntp_parser.y"
    {
			(yyval.String_fifo) = (yyvsp[(1) - (2)].String_fifo);
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(2) - (2)].String)));
		}
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 1349 "ntp_parser.y"
    {
			(yyval.String_fifo) = NULL;
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(1) - (1)].String)));
		}
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 1357 "ntp_parser.y"
    {
			(yyval.Address_fifo) = (yyvsp[(1) - (2)].Address_fifo);
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 1362 "ntp_parser.y"
    {
			(yyval.Address_fifo) = NULL;
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(1) - (1)].Address_node));
		}
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 1370 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			} else {
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
			}
		}
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 1378 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 1379 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 1383 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 1394 "ntp_parser.y"
    {
			sim_node *sn;
			
			sn =  create_sim_node((yyvsp[(3) - (5)].Attr_val_fifo), (yyvsp[(4) - (5)].Sim_server_fifo));
			APPEND_G_FIFO(cfgt.sim_details, sn);

			/* Revert from ; to \n for end-of-command */
			old_config_style = 1;
		}
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 1411 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 1416 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 1421 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 277:

/* Line 1464 of yacc.c  */
#line 1429 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 280:

/* Line 1464 of yacc.c  */
#line 1439 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = (yyvsp[(1) - (2)].Sim_server_fifo);
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(2) - (2)].Sim_server));
		}
    break;

  case 281:

/* Line 1464 of yacc.c  */
#line 1444 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(1) - (1)].Sim_server));
		}
    break;

  case 282:

/* Line 1464 of yacc.c  */
#line 1452 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Sim_script_fifo)); }
    break;

  case 283:

/* Line 1464 of yacc.c  */
#line 1457 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 284:

/* Line 1464 of yacc.c  */
#line 1462 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 285:

/* Line 1464 of yacc.c  */
#line 1467 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = (yyvsp[(1) - (2)].Sim_script_fifo);
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(2) - (2)].Sim_script));
		}
    break;

  case 286:

/* Line 1464 of yacc.c  */
#line 1472 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(1) - (1)].Sim_script));
		}
    break;

  case 287:

/* Line 1464 of yacc.c  */
#line 1480 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Attr_val_fifo)); }
    break;

  case 288:

/* Line 1464 of yacc.c  */
#line 1485 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 289:

/* Line 1464 of yacc.c  */
#line 1490 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 290:

/* Line 1464 of yacc.c  */
#line 1498 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1464 of yacc.c  */
#line 3591 "ntp_parser.c"
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
#line 1509 "ntp_parser.y"


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


