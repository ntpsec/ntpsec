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
     T_Auth = 261,
     T_Autokey = 262,
     T_Automax = 263,
     T_Average = 264,
     T_Bclient = 265,
     T_Beacon = 266,
     T_Broadcast = 267,
     T_Broadcastclient = 268,
     T_Broadcastdelay = 269,
     T_Burst = 270,
     T_Calibrate = 271,
     T_Ceiling = 272,
     T_Clockstats = 273,
     T_Cohort = 274,
     T_ControlKey = 275,
     T_Crypto = 276,
     T_Cryptostats = 277,
     T_Day = 278,
     T_Default = 279,
     T_Digest = 280,
     T_Disable = 281,
     T_Discard = 282,
     T_Dispersion = 283,
     T_Double = 284,
     T_Driftfile = 285,
     T_Drop = 286,
     T_Ellipsis = 287,
     T_Enable = 288,
     T_End = 289,
     T_False = 290,
     T_File = 291,
     T_Filegen = 292,
     T_Flag1 = 293,
     T_Flag2 = 294,
     T_Flag3 = 295,
     T_Flag4 = 296,
     T_Flake = 297,
     T_Floor = 298,
     T_Freq = 299,
     T_Fudge = 300,
     T_Host = 301,
     T_Huffpuff = 302,
     T_Iburst = 303,
     T_Ident = 304,
     T_Ignore = 305,
     T_Incalloc = 306,
     T_Incmem = 307,
     T_Initalloc = 308,
     T_Initmem = 309,
     T_Includefile = 310,
     T_Integer = 311,
     T_Interface = 312,
     T_Intrange = 313,
     T_Ipv4 = 314,
     T_Ipv4_flag = 315,
     T_Ipv6 = 316,
     T_Ipv6_flag = 317,
     T_Kernel = 318,
     T_Key = 319,
     T_Keys = 320,
     T_Keysdir = 321,
     T_Kod = 322,
     T_Mssntp = 323,
     T_Leapfile = 324,
     T_Limited = 325,
     T_Link = 326,
     T_Listen = 327,
     T_Logconfig = 328,
     T_Logfile = 329,
     T_Loopstats = 330,
     T_Lowpriotrap = 331,
     T_Manycastclient = 332,
     T_Manycastserver = 333,
     T_Mask = 334,
     T_Maxage = 335,
     T_Maxclock = 336,
     T_Maxdepth = 337,
     T_Maxdist = 338,
     T_Maxmem = 339,
     T_Maxpoll = 340,
     T_Minclock = 341,
     T_Mindepth = 342,
     T_Mindist = 343,
     T_Minimum = 344,
     T_Minpoll = 345,
     T_Minsane = 346,
     T_Mode = 347,
     T_Monitor = 348,
     T_Month = 349,
     T_Mru = 350,
     T_Multicastclient = 351,
     T_Nic = 352,
     T_Nolink = 353,
     T_Nomodify = 354,
     T_None = 355,
     T_Nonvolatile = 356,
     T_Nopeer = 357,
     T_Noquery = 358,
     T_Noselect = 359,
     T_Noserve = 360,
     T_Notrap = 361,
     T_Notrust = 362,
     T_Ntp = 363,
     T_Ntpport = 364,
     T_NtpSignDsocket = 365,
     T_Orphan = 366,
     T_Orphanwait = 367,
     T_Panic = 368,
     T_Peer = 369,
     T_Peerstats = 370,
     T_Phone = 371,
     T_Pid = 372,
     T_Pidfile = 373,
     T_Pool = 374,
     T_Port = 375,
     T_Preempt = 376,
     T_Prefer = 377,
     T_Protostats = 378,
     T_Pw = 379,
     T_Qos = 380,
     T_Randfile = 381,
     T_Rawstats = 382,
     T_Refid = 383,
     T_Requestkey = 384,
     T_Restrict = 385,
     T_Revoke = 386,
     T_Saveconfigdir = 387,
     T_Server = 388,
     T_Setvar = 389,
     T_Source = 390,
     T_Statistics = 391,
     T_Stats = 392,
     T_Statsdir = 393,
     T_Step = 394,
     T_Stepout = 395,
     T_Stratum = 396,
     T_String = 397,
     T_Sysstats = 398,
     T_Tick = 399,
     T_Time1 = 400,
     T_Time2 = 401,
     T_Timingstats = 402,
     T_Tinker = 403,
     T_Tos = 404,
     T_Trap = 405,
     T_True = 406,
     T_Trustedkey = 407,
     T_Ttl = 408,
     T_Type = 409,
     T_Unconfig = 410,
     T_Unpeer = 411,
     T_Version = 412,
     T_WanderThreshold = 413,
     T_Week = 414,
     T_Wildcard = 415,
     T_Xleave = 416,
     T_Year = 417,
     T_Flag = 418,
     T_EOC = 419,
     T_Simulate = 420,
     T_Beep_Delay = 421,
     T_Sim_Duration = 422,
     T_Server_Offset = 423,
     T_Duration = 424,
     T_Freq_Offset = 425,
     T_Wander = 426,
     T_Jitter = 427,
     T_Prop_Delay = 428,
     T_Proc_Delay = 429
   };
#endif
/* Tokens.  */
#define T_Age 258
#define T_All 259
#define T_Allan 260
#define T_Auth 261
#define T_Autokey 262
#define T_Automax 263
#define T_Average 264
#define T_Bclient 265
#define T_Beacon 266
#define T_Broadcast 267
#define T_Broadcastclient 268
#define T_Broadcastdelay 269
#define T_Burst 270
#define T_Calibrate 271
#define T_Ceiling 272
#define T_Clockstats 273
#define T_Cohort 274
#define T_ControlKey 275
#define T_Crypto 276
#define T_Cryptostats 277
#define T_Day 278
#define T_Default 279
#define T_Digest 280
#define T_Disable 281
#define T_Discard 282
#define T_Dispersion 283
#define T_Double 284
#define T_Driftfile 285
#define T_Drop 286
#define T_Ellipsis 287
#define T_Enable 288
#define T_End 289
#define T_False 290
#define T_File 291
#define T_Filegen 292
#define T_Flag1 293
#define T_Flag2 294
#define T_Flag3 295
#define T_Flag4 296
#define T_Flake 297
#define T_Floor 298
#define T_Freq 299
#define T_Fudge 300
#define T_Host 301
#define T_Huffpuff 302
#define T_Iburst 303
#define T_Ident 304
#define T_Ignore 305
#define T_Incalloc 306
#define T_Incmem 307
#define T_Initalloc 308
#define T_Initmem 309
#define T_Includefile 310
#define T_Integer 311
#define T_Interface 312
#define T_Intrange 313
#define T_Ipv4 314
#define T_Ipv4_flag 315
#define T_Ipv6 316
#define T_Ipv6_flag 317
#define T_Kernel 318
#define T_Key 319
#define T_Keys 320
#define T_Keysdir 321
#define T_Kod 322
#define T_Mssntp 323
#define T_Leapfile 324
#define T_Limited 325
#define T_Link 326
#define T_Listen 327
#define T_Logconfig 328
#define T_Logfile 329
#define T_Loopstats 330
#define T_Lowpriotrap 331
#define T_Manycastclient 332
#define T_Manycastserver 333
#define T_Mask 334
#define T_Maxage 335
#define T_Maxclock 336
#define T_Maxdepth 337
#define T_Maxdist 338
#define T_Maxmem 339
#define T_Maxpoll 340
#define T_Minclock 341
#define T_Mindepth 342
#define T_Mindist 343
#define T_Minimum 344
#define T_Minpoll 345
#define T_Minsane 346
#define T_Mode 347
#define T_Monitor 348
#define T_Month 349
#define T_Mru 350
#define T_Multicastclient 351
#define T_Nic 352
#define T_Nolink 353
#define T_Nomodify 354
#define T_None 355
#define T_Nonvolatile 356
#define T_Nopeer 357
#define T_Noquery 358
#define T_Noselect 359
#define T_Noserve 360
#define T_Notrap 361
#define T_Notrust 362
#define T_Ntp 363
#define T_Ntpport 364
#define T_NtpSignDsocket 365
#define T_Orphan 366
#define T_Orphanwait 367
#define T_Panic 368
#define T_Peer 369
#define T_Peerstats 370
#define T_Phone 371
#define T_Pid 372
#define T_Pidfile 373
#define T_Pool 374
#define T_Port 375
#define T_Preempt 376
#define T_Prefer 377
#define T_Protostats 378
#define T_Pw 379
#define T_Qos 380
#define T_Randfile 381
#define T_Rawstats 382
#define T_Refid 383
#define T_Requestkey 384
#define T_Restrict 385
#define T_Revoke 386
#define T_Saveconfigdir 387
#define T_Server 388
#define T_Setvar 389
#define T_Source 390
#define T_Statistics 391
#define T_Stats 392
#define T_Statsdir 393
#define T_Step 394
#define T_Stepout 395
#define T_Stratum 396
#define T_String 397
#define T_Sysstats 398
#define T_Tick 399
#define T_Time1 400
#define T_Time2 401
#define T_Timingstats 402
#define T_Tinker 403
#define T_Tos 404
#define T_Trap 405
#define T_True 406
#define T_Trustedkey 407
#define T_Ttl 408
#define T_Type 409
#define T_Unconfig 410
#define T_Unpeer 411
#define T_Version 412
#define T_WanderThreshold 413
#define T_Week 414
#define T_Wildcard 415
#define T_Xleave 416
#define T_Year 417
#define T_Flag 418
#define T_EOC 419
#define T_Simulate 420
#define T_Beep_Delay 421
#define T_Sim_Duration 422
#define T_Server_Offset 423
#define T_Duration 424
#define T_Freq_Offset 425
#define T_Wander 426
#define T_Jitter 427
#define T_Prop_Delay 428
#define T_Proc_Delay 429




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
#line 509 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 521 "ntp_parser.c"

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
#define YYFINAL  181
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   510

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  180
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  96
/* YYNRULES -- Number of rules.  */
#define YYNRULES  282
/* YYNRULES -- Number of states.  */
#define YYNSTATES  382

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   429

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     176,   177,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   175,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   178,     2,   179,     2,     2,     2,     2,
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
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174
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
     468,   470,   473,   476,   479,   483,   485,   488,   491,   494,
     497,   501,   504,   506,   508,   510,   512,   514,   516,   518,
     520,   522,   525,   526,   531,   533,   534,   535,   538,   541,
     544,   547,   549,   551,   555,   559,   561,   563,   565,   567,
     569,   571,   573,   575,   577,   580,   582,   585,   587,   589,
     591,   597,   600,   602,   605,   607,   609,   611,   613,   615,
     617,   623,   625,   629,   632,   636,   638,   640,   643,   645,
     651,   656,   660,   663,   665,   672,   676,   679,   683,   685,
     687,   689,   691
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     181,     0,    -1,   182,    -1,   182,   183,   164,    -1,   183,
     164,    -1,     1,   164,    -1,    -1,   184,    -1,   197,    -1,
     199,    -1,   200,    -1,   209,    -1,   217,    -1,   204,    -1,
     226,    -1,   231,    -1,   235,    -1,   239,    -1,   262,    -1,
     185,   186,   189,    -1,   133,    -1,   119,    -1,   114,    -1,
      12,    -1,    77,    -1,   187,    -1,   188,   142,    -1,   142,
      -1,    60,    -1,    62,    -1,    -1,   189,   190,    -1,   191,
      -1,   193,    -1,   195,    -1,   192,    -1,     7,    -1,    15,
      -1,    48,    -1,   104,    -1,   121,    -1,   122,    -1,   151,
      -1,   161,    -1,   194,    56,    -1,    64,    -1,    90,    -1,
      85,    -1,   153,    -1,    92,    -1,   157,    -1,   196,   142,
      -1,    49,    -1,   198,   186,    -1,   155,    -1,   156,    -1,
      13,    -1,    78,   259,    -1,    96,   259,    -1,     8,    56,
      -1,    20,    56,    -1,    21,   201,    -1,    65,   142,    -1,
      66,   142,    -1,   129,    56,    -1,   131,    56,    -1,   152,
     255,    -1,   110,   142,    -1,    -1,   201,   202,    -1,   203,
     142,    -1,   131,    56,    -1,    46,    -1,    49,    -1,   124,
      -1,   126,    -1,    25,    -1,   149,   205,    -1,   205,   206,
      -1,   206,    -1,   207,    56,    -1,   208,   261,    -1,    19,
     260,    -1,    17,    -1,    43,    -1,   111,    -1,   112,    -1,
      91,    -1,    11,    -1,    88,    -1,    83,    -1,    86,    -1,
      81,    -1,   136,   210,    -1,   138,   142,    -1,    37,   211,
     212,    -1,   210,   211,    -1,   211,    -1,    18,    -1,    22,
      -1,    75,    -1,   115,    -1,   127,    -1,   143,    -1,   147,
      -1,   123,    -1,    -1,   212,   213,    -1,    36,   142,    -1,
     154,   216,    -1,   214,    -1,   215,    -1,    71,    -1,    98,
      -1,    33,    -1,    26,    -1,   100,    -1,   117,    -1,    23,
      -1,   159,    -1,    94,    -1,   162,    -1,     3,    -1,    27,
     220,    -1,    95,   223,    -1,   130,   186,   218,    -1,   130,
     187,    79,   187,   218,    -1,   130,    24,   218,    -1,   130,
      60,    24,   218,    -1,   130,    62,    24,   218,    -1,   130,
     135,   218,    -1,    -1,   218,   219,    -1,    42,    -1,    50,
      -1,    67,    -1,    68,    -1,    70,    -1,    76,    -1,    99,
      -1,   102,    -1,   103,    -1,   105,    -1,   106,    -1,   107,
      -1,   109,    -1,   157,    -1,   220,   221,    -1,   221,    -1,
     222,    56,    -1,     9,    -1,    89,    -1,    93,    -1,   223,
     224,    -1,   224,    -1,   225,    56,    -1,    51,    -1,    52,
      -1,    53,    -1,    54,    -1,    80,    -1,    82,    -1,    84,
      -1,    87,    -1,    45,   186,   227,    -1,   227,   228,    -1,
     228,    -1,   229,   261,    -1,   230,   260,    -1,   141,    56,
      -1,   128,   142,    -1,   145,    -1,   146,    -1,    38,    -1,
      39,    -1,    40,    -1,    41,    -1,    33,   232,    -1,    26,
     232,    -1,   232,   233,    -1,   233,    -1,   234,    -1,   137,
      -1,     6,    -1,    10,    -1,    16,    -1,    63,    -1,    93,
      -1,   108,    -1,   148,   236,    -1,   236,   237,    -1,   237,
      -1,   238,   261,    -1,     5,    -1,    28,    -1,    44,    -1,
      47,    -1,   113,    -1,   139,    -1,   140,    -1,   250,    -1,
     240,   261,    -1,   241,   142,    -1,   242,   142,    -1,    55,
     142,   183,    -1,    34,    -1,    30,   243,    -1,    73,   248,
      -1,   116,   258,    -1,   134,   244,    -1,   150,   187,   246,
      -1,   153,   254,    -1,    14,    -1,   101,    -1,   144,    -1,
      69,    -1,   118,    -1,   125,    -1,    74,    -1,   132,    -1,
     142,    -1,   142,    29,    -1,    -1,   142,   175,   142,   245,
      -1,    24,    -1,    -1,    -1,   246,   247,    -1,   120,    56,
      -1,    57,   187,    -1,   248,   249,    -1,   249,    -1,   142,
      -1,   251,   253,   252,    -1,   251,   253,   142,    -1,    57,
      -1,    97,    -1,     4,    -1,    59,    -1,    61,    -1,   160,
      -1,    72,    -1,    50,    -1,    31,    -1,   254,    56,    -1,
      56,    -1,   255,   256,    -1,   256,    -1,    56,    -1,   257,
      -1,   176,    56,    32,    56,   177,    -1,   258,   142,    -1,
     142,    -1,   259,   186,    -1,   186,    -1,    56,    -1,   151,
      -1,    35,    -1,    56,    -1,    29,    -1,   263,   178,   264,
     267,   179,    -1,   165,    -1,   264,   265,   164,    -1,   265,
     164,    -1,   266,   175,   261,    -1,   166,    -1,   167,    -1,
     267,   268,    -1,   268,    -1,   270,   178,   269,   271,   179,
      -1,   168,   175,   261,   164,    -1,   133,   175,   186,    -1,
     271,   272,    -1,   272,    -1,   169,   175,   261,   178,   273,
     179,    -1,   273,   274,   164,    -1,   274,   164,    -1,   275,
     175,   261,    -1,   170,    -1,   171,    -1,   172,    -1,   173,
      -1,   174,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   336,   336,   340,   341,   342,   356,   357,   358,   359,
     360,   361,   362,   363,   364,   365,   366,   367,   368,   376,
     386,   387,   388,   389,   390,   394,   395,   400,   405,   407,
     413,   414,   422,   423,   424,   428,   433,   434,   435,   436,
     437,   438,   439,   440,   444,   449,   450,   451,   452,   453,
     454,   458,   463,   472,   482,   483,   493,   495,   497,   508,
     515,   517,   522,   524,   526,   528,   530,   532,   538,   539,
     547,   549,   561,   562,   563,   564,   565,   574,   579,   584,
     592,   594,   596,   601,   602,   603,   604,   605,   606,   610,
     611,   612,   613,   622,   624,   633,   643,   648,   656,   657,
     658,   659,   660,   661,   662,   663,   668,   669,   677,   687,
     696,   711,   716,   717,   721,   722,   726,   727,   728,   729,
     730,   731,   732,   741,   745,   749,   757,   765,   773,   788,
     803,   816,   817,   825,   826,   827,   828,   829,   830,   831,
     832,   833,   834,   835,   836,   837,   838,   842,   847,   855,
     860,   861,   862,   866,   871,   879,   884,   885,   886,   887,
     888,   889,   890,   891,   899,   909,   914,   922,   924,   926,
     928,   933,   934,   938,   939,   940,   941,   949,   951,   956,
     961,   969,   971,   983,   984,   985,   986,   987,   988,   996,
    1001,  1006,  1014,  1019,  1020,  1021,  1022,  1023,  1024,  1025,
    1034,  1035,  1042,  1049,  1065,  1084,  1089,  1091,  1093,  1095,
    1097,  1104,  1109,  1110,  1111,  1115,  1116,  1117,  1121,  1122,
    1126,  1133,  1143,  1152,  1157,  1159,  1164,  1165,  1173,  1175,
    1183,  1188,  1196,  1221,  1228,  1238,  1239,  1243,  1244,  1245,
    1246,  1250,  1251,  1252,  1261,  1266,  1274,  1279,  1287,  1289,
    1293,  1298,  1303,  1311,  1316,  1324,  1333,  1334,  1338,  1339,
    1348,  1366,  1370,  1375,  1383,  1388,  1389,  1393,  1398,  1406,
    1411,  1416,  1421,  1426,  1434,  1439,  1444,  1452,  1457,  1458,
    1459,  1460,  1461
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_Age", "T_All", "T_Allan", "T_Auth",
  "T_Autokey", "T_Automax", "T_Average", "T_Bclient", "T_Beacon",
  "T_Broadcast", "T_Broadcastclient", "T_Broadcastdelay", "T_Burst",
  "T_Calibrate", "T_Ceiling", "T_Clockstats", "T_Cohort", "T_ControlKey",
  "T_Crypto", "T_Cryptostats", "T_Day", "T_Default", "T_Digest",
  "T_Disable", "T_Discard", "T_Dispersion", "T_Double", "T_Driftfile",
  "T_Drop", "T_Ellipsis", "T_Enable", "T_End", "T_False", "T_File",
  "T_Filegen", "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4", "T_Flake",
  "T_Floor", "T_Freq", "T_Fudge", "T_Host", "T_Huffpuff", "T_Iburst",
  "T_Ident", "T_Ignore", "T_Incalloc", "T_Incmem", "T_Initalloc",
  "T_Initmem", "T_Includefile", "T_Integer", "T_Interface", "T_Intrange",
  "T_Ipv4", "T_Ipv4_flag", "T_Ipv6", "T_Ipv6_flag", "T_Kernel", "T_Key",
  "T_Keys", "T_Keysdir", "T_Kod", "T_Mssntp", "T_Leapfile", "T_Limited",
  "T_Link", "T_Listen", "T_Logconfig", "T_Logfile", "T_Loopstats",
  "T_Lowpriotrap", "T_Manycastclient", "T_Manycastserver", "T_Mask",
  "T_Maxage", "T_Maxclock", "T_Maxdepth", "T_Maxdist", "T_Maxmem",
  "T_Maxpoll", "T_Minclock", "T_Mindepth", "T_Mindist", "T_Minimum",
  "T_Minpoll", "T_Minsane", "T_Mode", "T_Monitor", "T_Month", "T_Mru",
  "T_Multicastclient", "T_Nic", "T_Nolink", "T_Nomodify", "T_None",
  "T_Nonvolatile", "T_Nopeer", "T_Noquery", "T_Noselect", "T_Noserve",
  "T_Notrap", "T_Notrust", "T_Ntp", "T_Ntpport", "T_NtpSignDsocket",
  "T_Orphan", "T_Orphanwait", "T_Panic", "T_Peer", "T_Peerstats",
  "T_Phone", "T_Pid", "T_Pidfile", "T_Pool", "T_Port", "T_Preempt",
  "T_Prefer", "T_Protostats", "T_Pw", "T_Qos", "T_Randfile", "T_Rawstats",
  "T_Refid", "T_Requestkey", "T_Restrict", "T_Revoke", "T_Saveconfigdir",
  "T_Server", "T_Setvar", "T_Source", "T_Statistics", "T_Stats",
  "T_Statsdir", "T_Step", "T_Stepout", "T_Stratum", "T_String",
  "T_Sysstats", "T_Tick", "T_Time1", "T_Time2", "T_Timingstats",
  "T_Tinker", "T_Tos", "T_Trap", "T_True", "T_Trustedkey", "T_Ttl",
  "T_Type", "T_Unconfig", "T_Unpeer", "T_Version", "T_WanderThreshold",
  "T_Week", "T_Wildcard", "T_Xleave", "T_Year", "T_Flag", "T_EOC",
  "T_Simulate", "T_Beep_Delay", "T_Sim_Duration", "T_Server_Offset",
  "T_Duration", "T_Freq_Offset", "T_Wander", "T_Jitter", "T_Prop_Delay",
  "T_Proc_Delay", "'='", "'('", "')'", "'{'", "'}'", "$accept",
  "configuration", "command_list", "command", "server_command",
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
  "nic_rule_action", "integer_list", "integer_list_range",
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
     425,   426,   427,   428,   429,    61,    40,    41,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   180,   181,   182,   182,   182,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   184,
     185,   185,   185,   185,   185,   186,   186,   187,   188,   188,
     189,   189,   190,   190,   190,   191,   192,   192,   192,   192,
     192,   192,   192,   192,   193,   194,   194,   194,   194,   194,
     194,   195,   196,   197,   198,   198,   199,   199,   199,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   201,   201,
     202,   202,   203,   203,   203,   203,   203,   204,   205,   205,
     206,   206,   206,   207,   207,   207,   207,   207,   207,   208,
     208,   208,   208,   209,   209,   209,   210,   210,   211,   211,
     211,   211,   211,   211,   211,   211,   212,   212,   213,   213,
     213,   213,   214,   214,   215,   215,   216,   216,   216,   216,
     216,   216,   216,   217,   217,   217,   217,   217,   217,   217,
     217,   218,   218,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   220,   220,   221,
     222,   222,   222,   223,   223,   224,   225,   225,   225,   225,
     225,   225,   225,   225,   226,   227,   227,   228,   228,   228,
     228,   229,   229,   230,   230,   230,   230,   231,   231,   232,
     232,   233,   233,   234,   234,   234,   234,   234,   234,   235,
     236,   236,   237,   238,   238,   238,   238,   238,   238,   238,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   240,   240,   240,   241,   241,   241,   242,   242,
     243,   243,   243,   244,   245,   245,   246,   246,   247,   247,
     248,   248,   249,   250,   250,   251,   251,   252,   252,   252,
     252,   253,   253,   253,   254,   254,   255,   255,   256,   256,
     257,   258,   258,   259,   259,   260,   260,   260,   261,   261,
     262,   263,   264,   264,   265,   266,   266,   267,   267,   268,
     269,   270,   271,   271,   272,   273,   273,   274,   275,   275,
     275,   275,   275
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
       1,     2,     2,     2,     3,     1,     2,     2,     2,     2,
       3,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     0,     4,     1,     0,     0,     2,     2,     2,
       2,     1,     1,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     2,     1,     1,     1,
       5,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       5,     1,     3,     2,     3,     1,     1,     2,     1,     5,
       4,     3,     2,     1,     6,     3,     2,     3,     1,     1,
       1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,    23,    56,   212,     0,    68,     0,     0,
     222,     0,   205,     0,     0,     0,   235,     0,     0,   215,
       0,   218,    24,     0,     0,     0,   236,   213,     0,    22,
       0,   216,    21,   217,     0,     0,     0,   219,    20,     0,
       0,     0,   214,     0,     0,     0,     0,     0,    54,    55,
     261,     0,     2,     0,     7,     0,     8,     0,     9,    10,
      13,    11,    12,    14,    15,    16,    17,     0,     0,     0,
     200,     0,    18,     0,     5,    59,    60,    61,   183,   184,
     185,   186,   187,   188,   182,   178,   180,   181,   150,   151,
     152,   123,   148,     0,   220,   206,   177,    98,    99,   100,
     101,   105,   102,   103,   104,   106,    28,    29,    27,     0,
      25,     0,     6,    62,    63,   232,   207,   231,   254,    57,
     156,   157,   158,   159,   160,   161,   162,   163,   124,   154,
       0,    58,    67,   252,   208,    64,   131,    28,    29,   131,
     131,    25,    65,     0,   209,    93,    97,    94,   193,   194,
     195,   196,   197,   198,   199,   189,   191,     0,    88,    83,
       0,    84,    92,    90,    91,    89,    87,    85,    86,    77,
      79,     0,     0,   226,   248,     0,    66,   247,   249,   245,
     211,     1,     0,     4,    30,    53,   259,   258,   201,   202,
     203,   243,   242,   241,     0,     0,    76,    72,    73,    74,
      75,     0,    69,     0,   179,   147,   149,   221,    95,   173,
     174,   175,   176,     0,     0,   171,   172,   164,   166,     0,
       0,    26,   204,   230,   253,   153,   155,   251,   127,   131,
     131,   130,   125,     0,     0,    96,   190,   192,   257,   255,
     256,    82,    78,    80,    81,   210,     0,   246,   244,     3,
      19,   237,   238,   239,   234,   240,   233,   265,   266,     0,
       0,     0,    71,    70,   115,   114,     0,   112,   113,     0,
     107,   110,   111,   170,   169,   165,   167,   168,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   132,   128,   129,   131,   225,     0,     0,   227,
       0,    36,    37,    38,    52,    45,    47,    46,    49,    39,
      40,    41,    42,    48,    50,    43,    31,    32,    35,    33,
       0,    34,     0,     0,     0,     0,   268,     0,   263,     0,
     108,   122,   118,   120,   116,   117,   119,   121,   109,   126,
     224,   223,   229,   228,     0,    44,    51,     0,   262,   260,
     267,     0,   264,   250,   271,     0,     0,     0,     0,     0,
     273,     0,     0,   269,   272,   270,     0,     0,   278,   279,
     280,   281,   282,     0,     0,     0,   274,     0,   276,     0,
     275,   277
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    51,    52,    53,    54,    55,   118,   110,   111,   250,
     316,   317,   318,   319,   320,   321,   322,    56,    57,    58,
      59,    77,   202,   203,    60,   169,   170,   171,   172,    61,
     145,   105,   208,   270,   271,   272,   338,    62,   228,   292,
      91,    92,    93,   128,   129,   130,    63,   217,   218,   219,
     220,    64,    85,    86,    87,    65,   155,   156,   157,    66,
      67,    68,    69,    95,   144,   341,   245,   299,   116,   117,
      70,    71,   256,   194,   180,   176,   177,   178,   134,   119,
     241,   188,    72,    73,   259,   260,   261,   325,   326,   356,
     327,   359,   360,   373,   374,   375
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -156
static const yytype_int16 yypact[] =
{
       2,  -151,   -29,  -156,  -156,  -156,   -26,  -156,    78,    -3,
     -87,    78,  -156,   107,   -53,   -86,  -156,   -84,   -76,  -156,
     -69,  -156,  -156,   -53,   305,   -53,  -156,  -156,   -68,  -156,
     -64,  -156,  -156,  -156,   -23,   -16,    27,  -156,  -156,   -57,
     107,   -50,  -156,    49,   398,   -47,   -52,    45,  -156,  -156,
    -156,   102,   171,   -60,  -156,   -53,  -156,   -53,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,     9,   -36,   -35,
    -156,    19,  -156,   -70,  -156,  -156,  -156,   168,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,    78,  -156,  -156,  -156,  -156,
    -156,    -3,  -156,    55,    84,  -156,    78,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   235,
    -156,   -27,   317,  -156,  -156,  -156,   -69,  -156,  -156,   -53,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   305,  -156,
      74,   -53,  -156,  -156,    -5,  -156,  -156,   115,   118,  -156,
    -156,    65,  -156,   -28,  -156,   107,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,    49,  -156,     9,  -156,  -156,
       5,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   398,
    -156,    92,     9,  -156,  -156,    93,   -52,  -156,  -156,  -156,
      97,  -156,    -4,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,     1,  -141,  -156,  -156,  -156,  -156,
    -156,   103,  -156,    21,  -156,  -156,  -156,  -156,    16,  -156,
    -156,  -156,  -156,    22,   109,  -156,  -156,   235,  -156,     9,
       5,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   153,  -156,
    -156,   153,   153,   -47,    30,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,   -20,   137,  -156,  -156,  -156,
     161,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   -85,
      13,    -2,  -156,  -156,  -156,  -156,    36,  -156,  -156,    28,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,   153,   153,  -156,   156,   -47,   125,  -156,
     138,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
     140,  -156,    58,    31,    47,  -109,  -156,    34,  -156,     9,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   153,
    -156,  -156,  -156,  -156,    41,  -156,  -156,   -53,  -156,  -156,
    -156,    51,  -156,  -156,  -156,    38,    62,     9,    52,  -116,
    -156,    68,     9,  -156,  -156,  -156,    57,   226,  -156,  -156,
    -156,  -156,  -156,   194,    69,    63,  -156,    75,  -156,     9,
    -156,  -156
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -156,  -156,  -156,   -40,  -156,  -156,   -14,   -34,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,    72,  -156,  -156,  -156,
    -156,   -22,  -156,  -156,  -156,  -156,  -156,  -156,  -120,  -156,
    -156,   151,  -156,  -156,   119,  -156,  -156,  -156,    26,  -156,
    -156,  -156,   246,   -51,  -156,  -156,  -156,   106,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   148,
    -156,  -156,  -156,  -156,  -156,  -156,    94,  -156,  -156,   244,
      59,  -155,  -156,  -156,  -156,    12,  -156,  -156,   -48,  -156,
    -156,  -156,   -81,  -156,   -93,  -156
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     109,   141,   237,     1,   174,   251,    88,   106,   136,   107,
       2,   173,   182,    74,     3,     4,     5,   244,   146,   231,
     232,   140,     6,     7,   323,   257,   258,    75,     8,     9,
      76,   331,    10,   135,   204,    11,    12,   297,   186,    13,
     238,   184,   264,   185,   137,   204,   138,    14,   323,   265,
     191,   332,   266,   358,   148,    94,   112,    15,   113,    16,
     252,   239,   253,   363,   276,   187,   114,    17,    18,   192,
     349,    19,   222,   115,   132,    20,    21,   149,   133,    22,
      23,   257,   258,   142,    78,   143,    89,   267,    79,   108,
      90,   193,   147,   150,    80,   108,   151,    24,    25,    26,
     298,   179,   181,    27,   183,   224,   189,   190,   195,   293,
     294,   206,    28,   207,   268,   221,    29,   224,    30,   139,
      31,    32,   333,   235,   175,    97,   108,    33,   334,    98,
     226,    34,    35,    36,    37,    38,    39,   227,    40,   229,
      41,    81,   230,   254,   233,   335,    42,   234,   243,   246,
      43,    44,    45,   248,    46,    47,   240,    48,    49,   262,
     249,   255,   152,   263,   273,   274,    -6,    50,   301,   300,
     269,    82,   296,   329,   352,   339,   302,   328,   330,     2,
     340,   343,    99,     3,     4,     5,    83,   336,   153,   154,
     337,     6,     7,   196,   344,   278,   345,     8,     9,   295,
     346,    10,   361,   279,    11,    12,   347,   366,    13,   303,
     304,   348,   351,   357,   197,    84,    14,   198,   353,   355,
     280,   281,   100,   282,   381,   305,    15,   362,    16,   283,
     101,   358,   365,   378,   102,   367,    17,    18,   379,   380,
      19,   242,   205,   275,    20,    21,   306,   225,    22,    23,
     103,   307,   284,   308,   104,   285,   286,    96,   287,   288,
     289,   236,   290,   342,   223,   309,    24,    25,    26,   131,
     247,   324,    27,   209,   210,   211,   212,   350,   364,   277,
     377,    28,   310,   311,     0,    29,     0,    30,     0,    31,
      32,     0,   199,     0,   200,     0,    33,     0,     0,   201,
      34,    35,    36,    37,    38,    39,     0,    40,     0,    41,
     291,     0,   312,     0,   313,    42,     0,     0,   314,    43,
      44,    45,   315,    46,    47,     2,    48,    49,     0,     3,
       4,     5,     0,   354,     0,    -6,    50,     6,     7,     0,
       0,     0,     0,     8,     9,     0,     0,    10,     0,     0,
      11,    12,     0,     0,    13,     0,   120,   121,   122,   123,
       0,     0,    14,   213,   368,   369,   370,   371,   372,     0,
       0,     0,    15,   376,    16,     0,   214,     0,     0,     0,
     215,   216,    17,    18,     0,   124,    19,   125,     0,   126,
      20,    21,   127,     0,    22,    23,   368,   369,   370,   371,
     372,     0,     0,     0,     0,     0,     0,     0,     0,   158,
       0,     0,    24,    25,    26,   159,     0,   160,    27,     0,
       0,     0,     0,     0,     0,     0,     0,    28,     0,     0,
       0,    29,     0,    30,     0,    31,    32,     0,     0,     0,
       0,   161,    33,     0,     0,     0,    34,    35,    36,    37,
      38,    39,     0,    40,     0,    41,     0,     0,     0,     0,
       0,    42,     0,     0,     0,    43,    44,    45,     0,    46,
      47,     0,    48,    49,     0,     0,     0,     0,     0,   162,
       0,   163,    50,     0,   164,     0,   165,     0,     0,   166,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   167,
     168
};

static const yytype_int16 yycheck[] =
{
      14,    35,   157,     1,    56,     4,     9,    60,    24,    62,
       8,    45,    52,   164,    12,    13,    14,   172,    40,   139,
     140,    35,    20,    21,   133,   166,   167,    56,    26,    27,
      56,     3,    30,    56,    85,    33,    34,    57,    29,    37,
      35,    55,    26,    57,    60,    96,    62,    45,   133,    33,
      31,    23,    36,   169,     5,   142,   142,    55,   142,    57,
      59,    56,    61,   179,   219,    56,   142,    65,    66,    50,
     179,    69,   112,   142,   142,    73,    74,    28,   142,    77,
      78,   166,   167,    56,     6,   142,    89,    71,    10,   142,
      93,    72,   142,    44,    16,   142,    47,    95,    96,    97,
     120,    56,     0,   101,   164,   119,   142,   142,   178,   229,
     230,    56,   110,    29,    98,   142,   114,   131,   116,   135,
     118,   119,    94,   145,   176,    18,   142,   125,   100,    22,
      56,   129,   130,   131,   132,   133,   134,   142,   136,    24,
     138,    63,    24,   142,    79,   117,   144,   175,    56,    56,
     148,   149,   150,    56,   152,   153,   151,   155,   156,    56,
     164,   160,   113,   142,   142,    56,   164,   165,     7,    32,
     154,    93,   142,   175,   329,   295,    15,   164,   142,     8,
      24,    56,    75,    12,    13,    14,   108,   159,   139,   140,
     162,    20,    21,    25,    56,    42,    56,    26,    27,   233,
     142,    30,   357,    50,    33,    34,   175,   362,    37,    48,
      49,   164,   178,   175,    46,   137,    45,    49,   177,   168,
      67,    68,   115,    70,   379,    64,    55,   175,    57,    76,
     123,   169,   164,   164,   127,   178,    65,    66,   175,   164,
      69,   169,    91,   217,    73,    74,    85,   128,    77,    78,
     143,    90,    99,    92,   147,   102,   103,    11,   105,   106,
     107,   155,   109,   297,   116,   104,    95,    96,    97,    25,
     176,   259,   101,    38,    39,    40,    41,   325,   359,   220,
     373,   110,   121,   122,    -1,   114,    -1,   116,    -1,   118,
     119,    -1,   124,    -1,   126,    -1,   125,    -1,    -1,   131,
     129,   130,   131,   132,   133,   134,    -1,   136,    -1,   138,
     157,    -1,   151,    -1,   153,   144,    -1,    -1,   157,   148,
     149,   150,   161,   152,   153,     8,   155,   156,    -1,    12,
      13,    14,    -1,   347,    -1,   164,   165,    20,    21,    -1,
      -1,    -1,    -1,    26,    27,    -1,    -1,    30,    -1,    -1,
      33,    34,    -1,    -1,    37,    -1,    51,    52,    53,    54,
      -1,    -1,    45,   128,   170,   171,   172,   173,   174,    -1,
      -1,    -1,    55,   179,    57,    -1,   141,    -1,    -1,    -1,
     145,   146,    65,    66,    -1,    80,    69,    82,    -1,    84,
      73,    74,    87,    -1,    77,    78,   170,   171,   172,   173,
     174,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,
      -1,    -1,    95,    96,    97,    17,    -1,    19,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,    -1,
      -1,   114,    -1,   116,    -1,   118,   119,    -1,    -1,    -1,
      -1,    43,   125,    -1,    -1,    -1,   129,   130,   131,   132,
     133,   134,    -1,   136,    -1,   138,    -1,    -1,    -1,    -1,
      -1,   144,    -1,    -1,    -1,   148,   149,   150,    -1,   152,
     153,    -1,   155,   156,    -1,    -1,    -1,    -1,    -1,    81,
      -1,    83,   165,    -1,    86,    -1,    88,    -1,    -1,    91,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,
     112
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     1,     8,    12,    13,    14,    20,    21,    26,    27,
      30,    33,    34,    37,    45,    55,    57,    65,    66,    69,
      73,    74,    77,    78,    95,    96,    97,   101,   110,   114,
     116,   118,   119,   125,   129,   130,   131,   132,   133,   134,
     136,   138,   144,   148,   149,   150,   152,   153,   155,   156,
     165,   181,   182,   183,   184,   185,   197,   198,   199,   200,
     204,   209,   217,   226,   231,   235,   239,   240,   241,   242,
     250,   251,   262,   263,   164,    56,    56,   201,     6,    10,
      16,    63,    93,   108,   137,   232,   233,   234,     9,    89,
      93,   220,   221,   222,   142,   243,   232,    18,    22,    75,
     115,   123,   127,   143,   147,   211,    60,    62,   142,   186,
     187,   188,   142,   142,   142,   142,   248,   249,   186,   259,
      51,    52,    53,    54,    80,    82,    84,    87,   223,   224,
     225,   259,   142,   142,   258,    56,    24,    60,    62,   135,
     186,   187,    56,   142,   244,   210,   211,   142,     5,    28,
      44,    47,   113,   139,   140,   236,   237,   238,    11,    17,
      19,    43,    81,    83,    86,    88,    91,   111,   112,   205,
     206,   207,   208,   187,    56,   176,   255,   256,   257,    56,
     254,     0,   183,   164,   186,   186,    29,    56,   261,   142,
     142,    31,    50,    72,   253,   178,    25,    46,    49,   124,
     126,   131,   202,   203,   233,   221,    56,    29,   212,    38,
      39,    40,    41,   128,   141,   145,   146,   227,   228,   229,
     230,   142,   183,   249,   186,   224,    56,   142,   218,    24,
      24,   218,   218,    79,   175,   211,   237,   261,    35,    56,
     151,   260,   206,    56,   261,   246,    56,   256,    56,   164,
     189,     4,    59,    61,   142,   160,   252,   166,   167,   264,
     265,   266,    56,   142,    26,    33,    36,    71,    98,   154,
     213,   214,   215,   142,    56,   228,   261,   260,    42,    50,
      67,    68,    70,    76,    99,   102,   103,   105,   106,   107,
     109,   157,   219,   218,   218,   187,   142,    57,   120,   247,
      32,     7,    15,    48,    49,    64,    85,    90,    92,   104,
     121,   122,   151,   153,   157,   161,   190,   191,   192,   193,
     194,   195,   196,   133,   265,   267,   268,   270,   164,   175,
     142,     3,    23,    94,   100,   117,   159,   162,   216,   218,
      24,   245,   187,    56,    56,    56,   142,   175,   164,   179,
     268,   178,   261,   177,   186,   168,   269,   175,   169,   271,
     272,   261,   175,   179,   272,   164,   261,   178,   170,   171,
     172,   173,   174,   273,   274,   275,   179,   274,   164,   175,
     164,   261
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
#line 343 "ntp_parser.y"
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
#line 377 "ntp_parser.y"
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.peers, my_node);
		}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 396 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), (yyvsp[(1) - (2)].Integer)); }
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 401 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_UNSPEC); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 406 "ntp_parser.y"
    { (yyval.Integer) = AF_INET; }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 408 "ntp_parser.y"
    { (yyval.Integer) = AF_INET6; }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 413 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 415 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 429 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 445 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 459 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 473 "ntp_parser.y"
    {
			unpeer_node *my_node;
			
			my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
		}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 494 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 496 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.manycastserver, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 498 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.multicastclient, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 509 "ntp_parser.y"
    {
			attr_val *atrv;
			
			atrv = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			APPEND_G_FIFO(cfgt.vars, atrv);
		}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 516 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 518 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			CONCAT_G_FIFOS(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 523 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 525 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 527 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 529 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 531 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Attr_val_fifo); }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 533 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 538 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 540 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 548 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 550 "ntp_parser.y"
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
#line 575 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.orphan_cmds, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 580 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 585 "ntp_parser.y"
    {	
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 593 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 595 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 597 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 623 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.stats_list, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 625 "ntp_parser.y"
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
#line 634 "ntp_parser.y"
    {
			filegen_node *fgn;
			
			fgn = create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.filegen_opts, fgn);
		}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 644 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 649 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 668 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 670 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 678 "ntp_parser.y"
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
#line 688 "ntp_parser.y"
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
#line 697 "ntp_parser.y"
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
#line 712 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 742 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.discard_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 746 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.mru_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 750 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 758 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 766 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 774 "ntp_parser.y"
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
#line 789 "ntp_parser.y"
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
#line 804 "ntp_parser.y"
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
#line 816 "ntp_parser.y"
    { (yyval.Int_fifo) = NULL; }
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 818 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 843 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 848 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 856 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 867 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 872 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 880 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 900 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.fudge, aon);
		}
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 910 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 915 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 923 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 925 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 927 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 929 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 177:

/* Line 1464 of yacc.c  */
#line 950 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.enable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 952 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.disable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 957 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 962 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 970 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 972 "ntp_parser.y"
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
#line 997 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.tinker, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 190:

/* Line 1464 of yacc.c  */
#line 1002 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 191:

/* Line 1464 of yacc.c  */
#line 1007 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 192:

/* Line 1464 of yacc.c  */
#line 1015 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 201:

/* Line 1464 of yacc.c  */
#line 1036 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 202:

/* Line 1464 of yacc.c  */
#line 1043 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 203:

/* Line 1464 of yacc.c  */
#line 1050 "ntp_parser.y"
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

  case 204:

/* Line 1464 of yacc.c  */
#line 1066 "ntp_parser.y"
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

  case 205:

/* Line 1464 of yacc.c  */
#line 1085 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 1090 "ntp_parser.y"
    { /* see drift_parm below for actions */ }
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 1092 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.logconfig, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 1094 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.phone, (yyvsp[(2) - (2)].String_fifo)); }
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 1096 "ntp_parser.y"
    { APPEND_G_FIFO(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 1098 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.trap, aon);
		}
    break;

  case 211:

/* Line 1464 of yacc.c  */
#line 1105 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.ttl, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 220:

/* Line 1464 of yacc.c  */
#line 1127 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 221:

/* Line 1464 of yacc.c  */
#line 1134 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
			av = create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 222:

/* Line 1464 of yacc.c  */
#line 1143 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, "");
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 1153 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 1159 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 1164 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 1166 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1174 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 1176 "ntp_parser.y"
    {
			(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), estrdup((yyvsp[(2) - (2)].Address_node)->address));
			destroy_address_node((yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 1184 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 1189 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 1197 "ntp_parser.y"
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

  case 233:

/* Line 1464 of yacc.c  */
#line 1222 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 1229 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 1262 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 245:

/* Line 1464 of yacc.c  */
#line 1267 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 246:

/* Line 1464 of yacc.c  */
#line 1275 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 247:

/* Line 1464 of yacc.c  */
#line 1280 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 1288 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[(1) - (1)].Integer)); }
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 1294 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_rangeval('-', (yyvsp[(2) - (5)].Integer), (yyvsp[(4) - (5)].Integer)); }
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 1299 "ntp_parser.y"
    {
			(yyval.String_fifo) = (yyvsp[(1) - (2)].String_fifo);
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(2) - (2)].String)));
		}
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 1304 "ntp_parser.y"
    {
			(yyval.String_fifo) = NULL;
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(1) - (1)].String)));
		}
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 1312 "ntp_parser.y"
    {
			(yyval.Address_fifo) = (yyvsp[(1) - (2)].Address_fifo);
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 1317 "ntp_parser.y"
    {
			(yyval.Address_fifo) = NULL;
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(1) - (1)].Address_node));
		}
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 1325 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			} else {
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
			}
		}
    break;

  case 256:

/* Line 1464 of yacc.c  */
#line 1333 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 257:

/* Line 1464 of yacc.c  */
#line 1334 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 1338 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 1349 "ntp_parser.y"
    {
			sim_node *sn;
			
			sn =  create_sim_node((yyvsp[(3) - (5)].Attr_val_fifo), (yyvsp[(4) - (5)].Sim_server_fifo));
			APPEND_G_FIFO(cfgt.sim_details, sn);

			/* Revert from ; to \n for end-of-command */
			old_config_style = 1;
		}
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 1366 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 1371 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 1376 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 1384 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 1394 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = (yyvsp[(1) - (2)].Sim_server_fifo);
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(2) - (2)].Sim_server));
		}
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 1399 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(1) - (1)].Sim_server));
		}
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 1407 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Sim_script_fifo)); }
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 1412 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 1417 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 1422 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = (yyvsp[(1) - (2)].Sim_script_fifo);
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(2) - (2)].Sim_script));
		}
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 1427 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(1) - (1)].Sim_script));
		}
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 1435 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Attr_val_fifo)); }
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 1440 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 1445 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 277:

/* Line 1464 of yacc.c  */
#line 1453 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1464 of yacc.c  */
#line 3516 "ntp_parser.c"
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
#line 1464 "ntp_parser.y"


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


