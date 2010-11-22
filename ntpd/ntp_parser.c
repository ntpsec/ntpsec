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
#define YYLAST   584

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  180
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  94
/* YYNRULES -- Number of rules.  */
#define YYNRULES  279
/* YYNRULES -- Number of states.  */
#define YYNSTATES  378

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
      85,    87,    89,    91,    94,    96,    98,   100,   102,   104,
     106,   109,   111,   113,   115,   118,   121,   124,   127,   130,
     133,   136,   139,   142,   145,   148,   149,   152,   155,   158,
     160,   162,   164,   166,   168,   171,   174,   176,   179,   182,
     185,   187,   189,   191,   193,   195,   197,   199,   201,   203,
     205,   208,   211,   215,   218,   220,   222,   224,   226,   228,
     230,   232,   234,   236,   237,   240,   243,   246,   248,   250,
     252,   254,   256,   258,   260,   262,   264,   266,   268,   270,
     272,   275,   278,   282,   288,   292,   297,   302,   306,   307,
     310,   312,   314,   316,   318,   320,   322,   324,   326,   328,
     330,   332,   334,   336,   338,   341,   343,   346,   348,   350,
     352,   355,   357,   360,   362,   364,   366,   368,   370,   372,
     374,   376,   380,   383,   385,   388,   391,   394,   397,   399,
     401,   403,   405,   407,   409,   412,   415,   418,   420,   422,
     424,   426,   428,   430,   432,   434,   436,   439,   442,   444,
     447,   449,   451,   453,   455,   457,   459,   461,   463,   466,
     469,   472,   476,   478,   481,   484,   487,   490,   494,   497,
     499,   501,   503,   505,   507,   509,   511,   513,   515,   518,
     519,   524,   526,   527,   528,   531,   534,   537,   540,   542,
     544,   548,   552,   554,   556,   558,   560,   562,   564,   566,
     568,   570,   573,   575,   578,   580,   582,   584,   590,   593,
     595,   598,   600,   602,   604,   606,   608,   610,   616,   618,
     622,   625,   629,   631,   633,   636,   638,   644,   649,   653,
     656,   658,   665,   669,   672,   676,   678,   680,   682,   684
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     181,     0,    -1,   182,    -1,   182,   183,   164,    -1,   183,
     164,    -1,     1,   164,    -1,    -1,   184,    -1,   195,    -1,
     197,    -1,   198,    -1,   207,    -1,   215,    -1,   202,    -1,
     224,    -1,   229,    -1,   233,    -1,   237,    -1,   260,    -1,
     185,   186,   189,    -1,   133,    -1,   119,    -1,   114,    -1,
      12,    -1,    77,    -1,   187,    -1,   188,   142,    -1,   142,
      -1,    60,    -1,    62,    -1,    -1,   189,   190,    -1,   191,
      -1,   193,    -1,   192,    -1,     7,    -1,    15,    -1,    48,
      -1,   104,    -1,   121,    -1,   122,    -1,   151,    -1,   161,
      -1,   194,    56,    -1,    64,    -1,    90,    -1,    85,    -1,
     153,    -1,    92,    -1,   157,    -1,   196,   186,    -1,   155,
      -1,   156,    -1,    13,    -1,    78,   257,    -1,    96,   257,
      -1,     8,    56,    -1,    20,    56,    -1,    21,   199,    -1,
      65,   142,    -1,    66,   142,    -1,   129,    56,    -1,   131,
      56,    -1,   152,   253,    -1,   110,   142,    -1,    -1,   199,
     200,    -1,   201,   142,    -1,   131,    56,    -1,    46,    -1,
      49,    -1,   124,    -1,   126,    -1,    25,    -1,   149,   203,
      -1,   203,   204,    -1,   204,    -1,   205,    56,    -1,   206,
     259,    -1,    19,   258,    -1,    17,    -1,    43,    -1,   111,
      -1,   112,    -1,    91,    -1,    11,    -1,    88,    -1,    83,
      -1,    86,    -1,    81,    -1,   136,   208,    -1,   138,   142,
      -1,    37,   209,   210,    -1,   208,   209,    -1,   209,    -1,
      18,    -1,    22,    -1,    75,    -1,   115,    -1,   127,    -1,
     143,    -1,   147,    -1,   123,    -1,    -1,   210,   211,    -1,
      36,   142,    -1,   154,   214,    -1,   212,    -1,   213,    -1,
      71,    -1,    98,    -1,    33,    -1,    26,    -1,   100,    -1,
     117,    -1,    23,    -1,   159,    -1,    94,    -1,   162,    -1,
       3,    -1,    27,   218,    -1,    95,   221,    -1,   130,   186,
     216,    -1,   130,   187,    79,   187,   216,    -1,   130,    24,
     216,    -1,   130,    60,    24,   216,    -1,   130,    62,    24,
     216,    -1,   130,   135,   216,    -1,    -1,   216,   217,    -1,
      42,    -1,    50,    -1,    67,    -1,    68,    -1,    70,    -1,
      76,    -1,    99,    -1,   102,    -1,   103,    -1,   105,    -1,
     106,    -1,   107,    -1,   109,    -1,   157,    -1,   218,   219,
      -1,   219,    -1,   220,    56,    -1,     9,    -1,    89,    -1,
      93,    -1,   221,   222,    -1,   222,    -1,   223,    56,    -1,
      51,    -1,    52,    -1,    53,    -1,    54,    -1,    80,    -1,
      82,    -1,    84,    -1,    87,    -1,    45,   186,   225,    -1,
     225,   226,    -1,   226,    -1,   227,   259,    -1,   228,   258,
      -1,   141,    56,    -1,   128,   142,    -1,   145,    -1,   146,
      -1,    38,    -1,    39,    -1,    40,    -1,    41,    -1,    33,
     230,    -1,    26,   230,    -1,   230,   231,    -1,   231,    -1,
     232,    -1,   137,    -1,     6,    -1,    10,    -1,    16,    -1,
      63,    -1,    93,    -1,   108,    -1,   148,   234,    -1,   234,
     235,    -1,   235,    -1,   236,   259,    -1,     5,    -1,    28,
      -1,    44,    -1,    47,    -1,   113,    -1,   139,    -1,   140,
      -1,   248,    -1,   238,   259,    -1,   239,   142,    -1,   240,
     142,    -1,    55,   142,   183,    -1,    34,    -1,    30,   241,
      -1,    73,   246,    -1,   116,   256,    -1,   134,   242,    -1,
     150,   187,   244,    -1,   153,   252,    -1,    14,    -1,   101,
      -1,   144,    -1,    69,    -1,   118,    -1,   125,    -1,    74,
      -1,   132,    -1,   142,    -1,   142,    29,    -1,    -1,   142,
     175,   142,   243,    -1,    24,    -1,    -1,    -1,   244,   245,
      -1,   120,    56,    -1,    57,   187,    -1,   246,   247,    -1,
     247,    -1,   142,    -1,   249,   251,   250,    -1,   249,   251,
     142,    -1,    57,    -1,    97,    -1,     4,    -1,    59,    -1,
      61,    -1,   160,    -1,    72,    -1,    50,    -1,    31,    -1,
     252,    56,    -1,    56,    -1,   253,   254,    -1,   254,    -1,
      56,    -1,   255,    -1,   176,    56,    32,    56,   177,    -1,
     256,   142,    -1,   142,    -1,   257,   186,    -1,   186,    -1,
      56,    -1,   151,    -1,    35,    -1,    56,    -1,    29,    -1,
     261,   178,   262,   265,   179,    -1,   165,    -1,   262,   263,
     164,    -1,   263,   164,    -1,   264,   175,   259,    -1,   166,
      -1,   167,    -1,   265,   266,    -1,   266,    -1,   268,   178,
     267,   269,   179,    -1,   168,   175,   259,   164,    -1,   133,
     175,   186,    -1,   269,   270,    -1,   270,    -1,   169,   175,
     259,   178,   271,   179,    -1,   271,   272,   164,    -1,   272,
     164,    -1,   273,   175,   259,    -1,   170,    -1,   171,    -1,
     172,    -1,   173,    -1,   174,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   334,   334,   338,   339,   340,   354,   355,   356,   357,
     358,   359,   360,   361,   362,   363,   364,   365,   366,   374,
     384,   385,   386,   387,   388,   392,   393,   398,   403,   405,
     411,   412,   420,   421,   425,   430,   431,   432,   433,   434,
     435,   436,   437,   441,   446,   447,   448,   449,   450,   451,
     460,   470,   471,   481,   483,   485,   496,   503,   505,   510,
     512,   514,   516,   518,   520,   526,   527,   535,   537,   549,
     550,   551,   552,   553,   562,   567,   572,   580,   582,   584,
     589,   590,   591,   592,   593,   594,   598,   599,   600,   601,
     610,   612,   621,   631,   636,   644,   645,   646,   647,   648,
     649,   650,   651,   656,   657,   665,   675,   684,   699,   704,
     705,   709,   710,   714,   715,   716,   717,   718,   719,   720,
     729,   733,   737,   745,   753,   761,   776,   791,   804,   805,
     813,   814,   815,   816,   817,   818,   819,   820,   821,   822,
     823,   824,   825,   826,   830,   835,   843,   848,   849,   850,
     854,   859,   867,   872,   873,   874,   875,   876,   877,   878,
     879,   887,   897,   902,   910,   912,   914,   916,   921,   922,
     926,   927,   928,   929,   937,   939,   944,   949,   957,   959,
     971,   972,   973,   974,   975,   976,   984,   989,   994,  1002,
    1007,  1008,  1009,  1010,  1011,  1012,  1013,  1022,  1023,  1030,
    1037,  1053,  1072,  1077,  1079,  1081,  1083,  1085,  1092,  1097,
    1098,  1099,  1103,  1104,  1105,  1109,  1110,  1114,  1121,  1131,
    1140,  1145,  1147,  1152,  1153,  1161,  1163,  1171,  1176,  1184,
    1209,  1216,  1226,  1227,  1231,  1232,  1233,  1234,  1238,  1239,
    1240,  1249,  1254,  1262,  1267,  1275,  1277,  1281,  1286,  1291,
    1299,  1304,  1312,  1321,  1322,  1326,  1327,  1336,  1354,  1358,
    1363,  1371,  1376,  1377,  1381,  1386,  1394,  1399,  1404,  1409,
    1414,  1422,  1427,  1432,  1440,  1445,  1446,  1447,  1448,  1449
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
  "option_int_keyword", "unpeer_command", "unpeer_keyword",
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
  "system_option_flag_keyword", "tinker_command", "tinker_option_list",
  "tinker_option", "tinker_option_keyword", "miscellaneous_command",
  "misc_cmd_dbl_keyword", "misc_cmd_str_keyword",
  "misc_cmd_str_lcl_keyword", "drift_parm", "variable_assign",
  "t_default_or_zero", "trap_option_list", "trap_option",
  "log_config_list", "log_config_command", "interface_command",
  "interface_nic", "nic_rule_class", "nic_rule_action", "integer_list",
  "integer_list_range", "integer_list_range_elt", "integer_range",
  "string_list", "address_list", "boolean", "number", "simulate_command",
  "sim_conf_start", "sim_init_statement_list", "sim_init_statement",
  "sim_init_keyword", "sim_server_list", "sim_server", "sim_server_offset",
  "sim_server_name", "sim_act_list", "sim_act", "sim_act_stmt_list",
  "sim_act_stmt", "sim_act_keyword", 0
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
     189,   189,   190,   190,   191,   192,   192,   192,   192,   192,
     192,   192,   192,   193,   194,   194,   194,   194,   194,   194,
     195,   196,   196,   197,   197,   197,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   199,   199,   200,   200,   201,
     201,   201,   201,   201,   202,   203,   203,   204,   204,   204,
     205,   205,   205,   205,   205,   205,   206,   206,   206,   206,
     207,   207,   207,   208,   208,   209,   209,   209,   209,   209,
     209,   209,   209,   210,   210,   211,   211,   211,   211,   212,
     212,   213,   213,   214,   214,   214,   214,   214,   214,   214,
     215,   215,   215,   215,   215,   215,   215,   215,   216,   216,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   218,   218,   219,   220,   220,   220,
     221,   221,   222,   223,   223,   223,   223,   223,   223,   223,
     223,   224,   225,   225,   226,   226,   226,   226,   227,   227,
     228,   228,   228,   228,   229,   229,   230,   230,   231,   231,
     232,   232,   232,   232,   232,   232,   233,   234,   234,   235,
     236,   236,   236,   236,   236,   236,   236,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   238,
     238,   238,   239,   239,   239,   240,   240,   241,   241,   241,
     242,   243,   243,   244,   244,   245,   245,   246,   246,   247,
     248,   248,   249,   249,   250,   250,   250,   250,   251,   251,
     251,   252,   252,   253,   253,   254,   254,   255,   256,   256,
     257,   257,   258,   258,   258,   259,   259,   260,   261,   262,
     262,   263,   264,   264,   265,   265,   266,   267,   268,   269,
     269,   270,   271,   271,   272,   273,   273,   273,   273,   273
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     0,     2,     2,     2,     1,
       1,     1,     1,     1,     2,     2,     1,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     3,     5,     3,     4,     4,     3,     0,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     2,     1,     1,     1,
       2,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     2,     1,     2,     2,     2,     2,     1,     1,
       1,     1,     1,     1,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     3,     1,     2,     2,     2,     2,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     0,
       4,     1,     0,     0,     2,     2,     2,     2,     1,     1,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
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
       0,     0,     0,    23,    53,   209,     0,    65,     0,     0,
     219,     0,   202,     0,     0,     0,   232,     0,     0,   212,
       0,   215,    24,     0,     0,     0,   233,   210,     0,    22,
       0,   213,    21,   214,     0,     0,     0,   216,    20,     0,
       0,     0,   211,     0,     0,     0,     0,     0,    51,    52,
     258,     0,     2,     0,     7,     0,     8,     0,     9,    10,
      13,    11,    12,    14,    15,    16,    17,     0,     0,     0,
     197,     0,    18,     0,     5,    56,    57,    58,   180,   181,
     182,   183,   184,   185,   179,   175,   177,   178,   147,   148,
     149,   120,   145,     0,   217,   203,   174,    95,    96,    97,
      98,   102,    99,   100,   101,   103,    28,    29,    27,     0,
      25,     0,     6,    59,    60,   229,   204,   228,   251,    54,
     153,   154,   155,   156,   157,   158,   159,   160,   121,   151,
       0,    55,    64,   249,   205,    61,   128,    28,    29,   128,
     128,    25,    62,     0,   206,    90,    94,    91,   190,   191,
     192,   193,   194,   195,   196,   186,   188,     0,    85,    80,
       0,    81,    89,    87,    88,    86,    84,    82,    83,    74,
      76,     0,     0,   223,   245,     0,    63,   244,   246,   242,
     208,     1,     0,     4,    30,    50,   256,   255,   198,   199,
     200,   240,   239,   238,     0,     0,    73,    69,    70,    71,
      72,     0,    66,     0,   176,   144,   146,   218,    92,   170,
     171,   172,   173,     0,     0,   168,   169,   161,   163,     0,
       0,    26,   201,   227,   250,   150,   152,   248,   124,   128,
     128,   127,   122,     0,     0,    93,   187,   189,   254,   252,
     253,    79,    75,    77,    78,   207,     0,   243,   241,     3,
      19,   234,   235,   236,   231,   237,   230,   262,   263,     0,
       0,     0,    68,    67,   112,   111,     0,   109,   110,     0,
     104,   107,   108,   167,   166,   162,   164,   165,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   129,   125,   126,   128,   222,     0,     0,   224,
       0,    35,    36,    37,    44,    46,    45,    48,    38,    39,
      40,    41,    47,    49,    42,    31,    32,    34,    33,     0,
       0,     0,     0,   265,     0,   260,     0,   105,   119,   115,
     117,   113,   114,   116,   118,   106,   123,   221,   220,   226,
     225,     0,    43,     0,   259,   257,   264,     0,   261,   247,
     268,     0,     0,     0,     0,     0,   270,     0,     0,   266,
     269,   267,     0,     0,   275,   276,   277,   278,   279,     0,
       0,     0,   271,     0,   273,     0,   272,   274
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    51,    52,    53,    54,    55,   118,   110,   111,   250,
     315,   316,   317,   318,   319,    56,    57,    58,    59,    77,
     202,   203,    60,   169,   170,   171,   172,    61,   145,   105,
     208,   270,   271,   272,   335,    62,   228,   292,    91,    92,
      93,   128,   129,   130,    63,   217,   218,   219,   220,    64,
      85,    86,    87,    65,   155,   156,   157,    66,    67,    68,
      69,    95,   144,   338,   245,   299,   116,   117,    70,    71,
     256,   194,   180,   176,   177,   178,   134,   119,   241,   188,
      72,    73,   259,   260,   261,   322,   323,   352,   324,   355,
     356,   369,   370,   371
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -156
static const yytype_int16 yypact[] =
{
       2,  -151,   -23,  -156,  -156,  -156,    -8,  -156,    76,    11,
     -92,    76,  -156,    89,   -36,   -84,  -156,   -79,   -72,  -156,
     -61,  -156,  -156,   -36,   322,   -36,  -156,  -156,   -59,  -156,
     -57,  -156,  -156,  -156,    32,   -16,    33,  -156,  -156,   -48,
      89,   -47,  -156,    49,   428,   -41,   -52,    34,  -156,  -156,
    -156,    84,   188,   -73,  -156,   -36,  -156,   -36,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,     5,   -32,   -29,
    -156,     6,  -156,   -76,  -156,  -156,  -156,   148,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,    76,  -156,  -156,  -156,  -156,
    -156,    11,  -156,    59,    94,  -156,    76,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   141,
    -156,   -13,   334,  -156,  -156,  -156,   -61,  -156,  -156,   -36,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   322,  -156,
      81,   -36,  -156,  -156,    -1,  -156,  -156,   118,   120,  -156,
    -156,    68,  -156,   -27,  -156,    89,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,    49,  -156,     5,  -156,  -156,
     -26,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   428,
    -156,    93,     5,  -156,  -156,    97,   -52,  -156,  -156,  -156,
     100,  -156,    -5,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,     1,  -101,  -156,  -156,  -156,  -156,
    -156,   104,  -156,    21,  -156,  -156,  -156,  -156,    16,  -156,
    -156,  -156,  -156,    23,   112,  -156,  -156,   141,  -156,     5,
     -26,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   427,  -156,
    -156,   427,   427,   -41,    30,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,   -51,   143,  -156,  -156,  -156,
     266,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   -93,
      12,     3,  -156,  -156,  -156,  -156,    35,  -156,  -156,    28,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,   427,   427,  -156,   159,   -41,   129,  -156,
     130,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,   135,
      17,    29,  -126,  -156,    27,  -156,     5,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,   427,  -156,  -156,  -156,
    -156,    18,  -156,   -36,  -156,  -156,  -156,    38,  -156,  -156,
    -156,    36,    41,     5,    42,  -124,  -156,    43,     5,  -156,
    -156,  -156,    45,    67,  -156,  -156,  -156,  -156,  -156,    56,
      55,    69,  -156,    60,  -156,     5,  -156,  -156
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -156,  -156,  -156,   -40,  -156,  -156,   -14,   -34,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,    62,  -156,  -156,  -156,  -156,   -15,
    -156,  -156,  -156,  -156,  -156,  -156,  -121,  -156,  -156,   151,
    -156,  -156,   106,  -156,  -156,  -156,    31,  -156,  -156,  -156,
     235,   -58,  -156,  -156,  -156,    92,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,   133,  -156,  -156,
    -156,  -156,  -156,  -156,    74,  -156,  -156,   226,    39,  -155,
    -156,  -156,  -156,    -7,  -156,  -156,   -67,  -156,  -156,  -156,
     -99,  -156,  -111,  -156
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     109,   141,   237,     1,   174,   251,   297,   320,   136,   238,
       2,   173,   182,    74,     3,     4,     5,   244,   231,   232,
      88,   140,     6,     7,   106,   146,   107,   204,     8,     9,
     239,   328,    10,    75,   186,    11,    12,   191,   204,    13,
     320,   184,   264,   185,   137,   354,   138,    14,    76,   265,
      94,   329,   266,   345,   148,   359,   192,    15,   112,    16,
     252,   187,   253,   113,   276,   257,   258,    17,    18,   298,
     114,    19,   222,   257,   258,    20,    21,   149,   193,    22,
      23,   115,    78,   132,   181,   133,    79,   267,   135,   142,
     179,   183,    80,   150,   143,   147,   151,    24,    25,    26,
      89,   108,   195,    27,    90,   224,   108,    97,   293,   294,
     189,    98,    28,   190,   268,   206,    29,   224,    30,   139,
      31,    32,   330,   207,   175,   240,   108,    33,   331,   221,
     235,    34,    35,    36,    37,    38,    39,   226,    40,    81,
      41,   227,   229,   254,   230,   332,    42,   233,   234,   243,
      43,    44,    45,   246,    46,    47,   248,    48,    49,   249,
     262,   255,   152,   263,    99,   273,    -6,    50,   274,    82,
     269,   348,   296,   196,   336,   300,   325,   327,   326,   209,
     210,   211,   212,   337,    83,   340,   341,   333,   153,   154,
     334,   342,   343,   344,   197,   349,     2,   198,   357,   295,
       3,     4,     5,   362,   100,   347,   351,   361,     6,     7,
     354,   353,   101,    84,     8,     9,   102,   358,    10,   374,
     377,    11,    12,   363,   376,    13,   364,   365,   366,   367,
     368,   242,   103,    14,   225,   372,   104,   364,   365,   366,
     367,   368,   205,    15,   375,    16,    96,   236,   275,   223,
     247,   131,   321,    17,    18,   346,   360,    19,   373,   277,
       0,    20,    21,   339,     0,    22,    23,     0,     0,   213,
       0,     0,   199,   301,   200,     0,     0,     0,     0,   201,
       0,   302,   214,    24,    25,    26,   215,   216,     0,    27,
       0,     0,     0,     0,     0,     0,     0,     0,    28,     0,
       0,     0,    29,     0,    30,     0,    31,    32,     0,     0,
       0,     0,     0,    33,   303,     0,     0,    34,    35,    36,
      37,    38,    39,     0,    40,     0,    41,     0,     0,   350,
     304,     0,    42,     0,     0,     0,    43,    44,    45,     0,
      46,    47,     2,    48,    49,     0,     3,     4,     5,     0,
       0,   305,    -6,    50,     6,     7,   306,     0,   307,     0,
       8,     9,     0,     0,    10,     0,     0,    11,    12,     0,
     308,    13,     0,   120,   121,   122,   123,     0,     0,    14,
       0,     0,     0,     0,     0,     0,     0,   309,   310,    15,
       0,    16,     0,     0,     0,     0,     0,     0,     0,    17,
      18,     0,   124,    19,   125,     0,   126,    20,    21,   127,
       0,    22,    23,     0,     0,     0,     0,   311,     0,   312,
       0,     0,     0,   313,     0,     0,     0,   314,     0,    24,
      25,    26,     0,     0,     0,    27,     0,     0,     0,   158,
       0,     0,     0,     0,    28,   159,     0,   160,    29,     0,
      30,     0,    31,    32,     0,     0,     0,     0,     0,    33,
       0,     0,     0,    34,    35,    36,    37,    38,    39,   278,
      40,   161,    41,     0,     0,     0,     0,   279,    42,     0,
       0,     0,    43,    44,    45,     0,    46,    47,     0,    48,
      49,     0,     0,     0,   280,   281,     0,   282,     0,    50,
       0,     0,     0,   283,     0,     0,     0,     0,     0,   162,
       0,   163,     0,     0,   164,     0,   165,     0,     0,   166,
       0,     0,     0,     0,     0,     0,   284,     0,     0,   285,
     286,     0,   287,   288,   289,     0,   290,     0,     0,   167,
     168,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   291
};

static const yytype_int16 yycheck[] =
{
      14,    35,   157,     1,    56,     4,    57,   133,    24,    35,
       8,    45,    52,   164,    12,    13,    14,   172,   139,   140,
       9,    35,    20,    21,    60,    40,    62,    85,    26,    27,
      56,     3,    30,    56,    29,    33,    34,    31,    96,    37,
     133,    55,    26,    57,    60,   169,    62,    45,    56,    33,
     142,    23,    36,   179,     5,   179,    50,    55,   142,    57,
      59,    56,    61,   142,   219,   166,   167,    65,    66,   120,
     142,    69,   112,   166,   167,    73,    74,    28,    72,    77,
      78,   142,     6,   142,     0,   142,    10,    71,    56,    56,
      56,   164,    16,    44,   142,   142,    47,    95,    96,    97,
      89,   142,   178,   101,    93,   119,   142,    18,   229,   230,
     142,    22,   110,   142,    98,    56,   114,   131,   116,   135,
     118,   119,    94,    29,   176,   151,   142,   125,   100,   142,
     145,   129,   130,   131,   132,   133,   134,    56,   136,    63,
     138,   142,    24,   142,    24,   117,   144,    79,   175,    56,
     148,   149,   150,    56,   152,   153,    56,   155,   156,   164,
      56,   160,   113,   142,    75,   142,   164,   165,    56,    93,
     154,   326,   142,    25,   295,    32,   164,   142,   175,    38,
      39,    40,    41,    24,   108,    56,    56,   159,   139,   140,
     162,    56,   175,   164,    46,   177,     8,    49,   353,   233,
      12,    13,    14,   358,   115,   178,   168,   164,    20,    21,
     169,   175,   123,   137,    26,    27,   127,   175,    30,   164,
     375,    33,    34,   178,   164,    37,   170,   171,   172,   173,
     174,   169,   143,    45,   128,   179,   147,   170,   171,   172,
     173,   174,    91,    55,   175,    57,    11,   155,   217,   116,
     176,    25,   259,    65,    66,   322,   355,    69,   369,   220,
      -1,    73,    74,   297,    -1,    77,    78,    -1,    -1,   128,
      -1,    -1,   124,     7,   126,    -1,    -1,    -1,    -1,   131,
      -1,    15,   141,    95,    96,    97,   145,   146,    -1,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,
      -1,    -1,   114,    -1,   116,    -1,   118,   119,    -1,    -1,
      -1,    -1,    -1,   125,    48,    -1,    -1,   129,   130,   131,
     132,   133,   134,    -1,   136,    -1,   138,    -1,    -1,   343,
      64,    -1,   144,    -1,    -1,    -1,   148,   149,   150,    -1,
     152,   153,     8,   155,   156,    -1,    12,    13,    14,    -1,
      -1,    85,   164,   165,    20,    21,    90,    -1,    92,    -1,
      26,    27,    -1,    -1,    30,    -1,    -1,    33,    34,    -1,
     104,    37,    -1,    51,    52,    53,    54,    -1,    -1,    45,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   121,   122,    55,
      -1,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,
      66,    -1,    80,    69,    82,    -1,    84,    73,    74,    87,
      -1,    77,    78,    -1,    -1,    -1,    -1,   151,    -1,   153,
      -1,    -1,    -1,   157,    -1,    -1,    -1,   161,    -1,    95,
      96,    97,    -1,    -1,    -1,   101,    -1,    -1,    -1,    11,
      -1,    -1,    -1,    -1,   110,    17,    -1,    19,   114,    -1,
     116,    -1,   118,   119,    -1,    -1,    -1,    -1,    -1,   125,
      -1,    -1,    -1,   129,   130,   131,   132,   133,   134,    42,
     136,    43,   138,    -1,    -1,    -1,    -1,    50,   144,    -1,
      -1,    -1,   148,   149,   150,    -1,   152,   153,    -1,   155,
     156,    -1,    -1,    -1,    67,    68,    -1,    70,    -1,   165,
      -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    81,
      -1,    83,    -1,    -1,    86,    -1,    88,    -1,    -1,    91,
      -1,    -1,    -1,    -1,    -1,    -1,    99,    -1,    -1,   102,
     103,    -1,   105,   106,   107,    -1,   109,    -1,    -1,   111,
     112,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   157
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
     165,   181,   182,   183,   184,   185,   195,   196,   197,   198,
     202,   207,   215,   224,   229,   233,   237,   238,   239,   240,
     248,   249,   260,   261,   164,    56,    56,   199,     6,    10,
      16,    63,    93,   108,   137,   230,   231,   232,     9,    89,
      93,   218,   219,   220,   142,   241,   230,    18,    22,    75,
     115,   123,   127,   143,   147,   209,    60,    62,   142,   186,
     187,   188,   142,   142,   142,   142,   246,   247,   186,   257,
      51,    52,    53,    54,    80,    82,    84,    87,   221,   222,
     223,   257,   142,   142,   256,    56,    24,    60,    62,   135,
     186,   187,    56,   142,   242,   208,   209,   142,     5,    28,
      44,    47,   113,   139,   140,   234,   235,   236,    11,    17,
      19,    43,    81,    83,    86,    88,    91,   111,   112,   203,
     204,   205,   206,   187,    56,   176,   253,   254,   255,    56,
     252,     0,   183,   164,   186,   186,    29,    56,   259,   142,
     142,    31,    50,    72,   251,   178,    25,    46,    49,   124,
     126,   131,   200,   201,   231,   219,    56,    29,   210,    38,
      39,    40,    41,   128,   141,   145,   146,   225,   226,   227,
     228,   142,   183,   247,   186,   222,    56,   142,   216,    24,
      24,   216,   216,    79,   175,   209,   235,   259,    35,    56,
     151,   258,   204,    56,   259,   244,    56,   254,    56,   164,
     189,     4,    59,    61,   142,   160,   250,   166,   167,   262,
     263,   264,    56,   142,    26,    33,    36,    71,    98,   154,
     211,   212,   213,   142,    56,   226,   259,   258,    42,    50,
      67,    68,    70,    76,    99,   102,   103,   105,   106,   107,
     109,   157,   217,   216,   216,   187,   142,    57,   120,   245,
      32,     7,    15,    48,    64,    85,    90,    92,   104,   121,
     122,   151,   153,   157,   161,   190,   191,   192,   193,   194,
     133,   263,   265,   266,   268,   164,   175,   142,     3,    23,
      94,   100,   117,   159,   162,   214,   216,    24,   243,   187,
      56,    56,    56,   175,   164,   179,   266,   178,   259,   177,
     186,   168,   267,   175,   169,   269,   270,   259,   175,   179,
     270,   164,   259,   178,   170,   171,   172,   173,   174,   271,
     272,   273,   179,   272,   164,   175,   164,   259
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
#line 341 "ntp_parser.y"
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
#line 375 "ntp_parser.y"
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.peers, my_node);
		}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 394 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), (yyvsp[(1) - (2)].Integer)); }
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 399 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_UNSPEC); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 404 "ntp_parser.y"
    { (yyval.Integer) = AF_INET; }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 406 "ntp_parser.y"
    { (yyval.Integer) = AF_INET6; }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 411 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 413 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 426 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 442 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 461 "ntp_parser.y"
    {
			unpeer_node *my_node;
			
			my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
		}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 482 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 484 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.manycastserver, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 486 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.multicastclient, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 497 "ntp_parser.y"
    {
			attr_val *atrv;
			
			atrv = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			APPEND_G_FIFO(cfgt.vars, atrv);
		}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 504 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 506 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			CONCAT_G_FIFOS(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 511 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 513 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 515 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 517 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 519 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Attr_val_fifo); }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 521 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 526 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 528 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 536 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 538 "ntp_parser.y"
    {
			(yyval.Attr_val) = NULL;
			cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer);
			msyslog(LOG_WARNING,
				"'crypto revoke %d' is deprecated, "
				"please use 'revoke %d' instead.",
				cfgt.auth.revoke, cfgt.auth.revoke);
		}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 563 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.orphan_cmds, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 568 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 573 "ntp_parser.y"
    {	
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 581 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 583 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 585 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 611 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.stats_list, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 613 "ntp_parser.y"
    {
			if (input_from_file) {
				cfgt.stats_dir = (yyvsp[(2) - (2)].String);
			} else {
				YYFREE((yyvsp[(2) - (2)].String));
				yyerror("statsdir remote configuration ignored");
			}
		}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 622 "ntp_parser.y"
    {
			filegen_node *fgn;
			
			fgn = create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.filegen_opts, fgn);
		}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 632 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 637 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 656 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 658 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 666 "ntp_parser.y"
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

  case 106:

/* Line 1464 of yacc.c  */
#line 676 "ntp_parser.y"
    {
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen type remote config ignored");
			}
		}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 685 "ntp_parser.y"
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

  case 108:

/* Line 1464 of yacc.c  */
#line 700 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 730 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.discard_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 734 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.mru_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 738 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 746 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 754 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 762 "ntp_parser.y"
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

  case 126:

/* Line 1464 of yacc.c  */
#line 777 "ntp_parser.y"
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

  case 127:

/* Line 1464 of yacc.c  */
#line 792 "ntp_parser.y"
    {
			restrict_node *	rn;

			APPEND_G_FIFO((yyvsp[(3) - (3)].Int_fifo), create_int_node((yyvsp[(2) - (3)].Integer)));
			rn = create_restrict_node(
				NULL, NULL, (yyvsp[(3) - (3)].Int_fifo), ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 804 "ntp_parser.y"
    { (yyval.Int_fifo) = NULL; }
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 806 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 831 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 836 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 844 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 855 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 860 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 868 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 161:

/* Line 1464 of yacc.c  */
#line 888 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.fudge, aon);
		}
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 898 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 903 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 911 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 913 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 915 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 917 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 938 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.enable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 940 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.disable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 945 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 177:

/* Line 1464 of yacc.c  */
#line 950 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 958 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 960 "ntp_parser.y"
    { 
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				yyerror("enable/disable stats remote configuration ignored");
			}
		}
    break;

  case 186:

/* Line 1464 of yacc.c  */
#line 985 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.tinker, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 187:

/* Line 1464 of yacc.c  */
#line 990 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 188:

/* Line 1464 of yacc.c  */
#line 995 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 189:

/* Line 1464 of yacc.c  */
#line 1003 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 198:

/* Line 1464 of yacc.c  */
#line 1024 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 199:

/* Line 1464 of yacc.c  */
#line 1031 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 200:

/* Line 1464 of yacc.c  */
#line 1038 "ntp_parser.y"
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

  case 201:

/* Line 1464 of yacc.c  */
#line 1054 "ntp_parser.y"
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

  case 202:

/* Line 1464 of yacc.c  */
#line 1073 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 203:

/* Line 1464 of yacc.c  */
#line 1078 "ntp_parser.y"
    { /* see drift_parm below for actions */ }
    break;

  case 204:

/* Line 1464 of yacc.c  */
#line 1080 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.logconfig, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 1082 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.phone, (yyvsp[(2) - (2)].String_fifo)); }
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 1084 "ntp_parser.y"
    { APPEND_G_FIFO(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 1086 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.trap, aon);
		}
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 1093 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.ttl, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 217:

/* Line 1464 of yacc.c  */
#line 1115 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 218:

/* Line 1464 of yacc.c  */
#line 1122 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
			av = create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 219:

/* Line 1464 of yacc.c  */
#line 1131 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, "");
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 220:

/* Line 1464 of yacc.c  */
#line 1141 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 222:

/* Line 1464 of yacc.c  */
#line 1147 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 1152 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 224:

/* Line 1464 of yacc.c  */
#line 1154 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 1162 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 1164 "ntp_parser.y"
    {
			(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), estrdup((yyvsp[(2) - (2)].Address_node)->address));
			destroy_address_node((yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 1172 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1177 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 1185 "ntp_parser.y"
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

  case 230:

/* Line 1464 of yacc.c  */
#line 1210 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 1217 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 241:

/* Line 1464 of yacc.c  */
#line 1250 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 242:

/* Line 1464 of yacc.c  */
#line 1255 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 243:

/* Line 1464 of yacc.c  */
#line 1263 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 1268 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 245:

/* Line 1464 of yacc.c  */
#line 1276 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[(1) - (1)].Integer)); }
    break;

  case 247:

/* Line 1464 of yacc.c  */
#line 1282 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_rangeval('-', (yyvsp[(2) - (5)].Integer), (yyvsp[(4) - (5)].Integer)); }
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 1287 "ntp_parser.y"
    {
			(yyval.String_fifo) = (yyvsp[(1) - (2)].String_fifo);
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(2) - (2)].String)));
		}
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 1292 "ntp_parser.y"
    {
			(yyval.String_fifo) = NULL;
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(1) - (1)].String)));
		}
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 1300 "ntp_parser.y"
    {
			(yyval.Address_fifo) = (yyvsp[(1) - (2)].Address_fifo);
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 1305 "ntp_parser.y"
    {
			(yyval.Address_fifo) = NULL;
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(1) - (1)].Address_node));
		}
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 1313 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			} else {
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
			}
		}
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 1321 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 1322 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 1326 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 257:

/* Line 1464 of yacc.c  */
#line 1337 "ntp_parser.y"
    {
			sim_node *sn;
			
			sn =  create_sim_node((yyvsp[(3) - (5)].Attr_val_fifo), (yyvsp[(4) - (5)].Sim_server_fifo));
			APPEND_G_FIFO(cfgt.sim_details, sn);

			/* Revert from ; to \n for end-of-command */
			old_config_style = 1;
		}
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 1354 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 1359 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 1364 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 1372 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 1382 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = (yyvsp[(1) - (2)].Sim_server_fifo);
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(2) - (2)].Sim_server));
		}
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 1387 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(1) - (1)].Sim_server));
		}
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 1395 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Sim_script_fifo)); }
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 1400 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 1405 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 1410 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = (yyvsp[(1) - (2)].Sim_script_fifo);
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(2) - (2)].Sim_script));
		}
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 1415 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(1) - (1)].Sim_script));
		}
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 1423 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Attr_val_fifo)); }
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 1428 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 1433 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 1441 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1464 of yacc.c  */
#line 3514 "ntp_parser.c"
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
#line 1452 "ntp_parser.y"


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


