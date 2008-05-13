/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_Age = 258,
     T_Allan = 259,
     T_Auth = 260,
     T_Autokey = 261,
     T_Automax = 262,
     T_Average = 263,
     T_Bclient = 264,
     T_Beacon = 265,
     T_Broadcast = 266,
     T_Broadcastclient = 267,
     T_Broadcastdelay = 268,
     T_Burst = 269,
     T_Calibrate = 270,
     T_Calldelay = 271,
     T_Ceiling = 272,
     T_Clockstats = 273,
     T_Cohort = 274,
     T_ControlKey = 275,
     T_Crypto = 276,
     T_Cryptostats = 277,
     T_Day = 278,
     T_Default = 279,
     T_Disable = 280,
     T_Discard = 281,
     T_Dispersion = 282,
     T_Double = 283,
     T_Driftfile = 284,
     T_WanderThreshold = 285,
     T_Enable = 286,
     T_End = 287,
     T_False = 288,
     T_File = 289,
     T_Filegen = 290,
     T_Flag1 = 291,
     T_Flag2 = 292,
     T_Flag3 = 293,
     T_Flag4 = 294,
     T_Flake = 295,
     T_Floor = 296,
     T_Freq = 297,
     T_Fudge = 298,
     T_Host = 299,
     T_Huffpuff = 300,
     T_IPv4_address = 301,
     T_IPv6_address = 302,
     T_Iburst = 303,
     T_Ident = 304,
     T_Ignore = 305,
     T_Includefile = 306,
     T_Integer = 307,
     T_Interface = 308,
     T_Kernel = 309,
     T_Key = 310,
     T_Keys = 311,
     T_Keysdir = 312,
     T_Kod = 313,
     T_Leap = 314,
     T_Leapfile = 315,
     T_Limited = 316,
     T_Link = 317,
     T_Logconfig = 318,
     T_Logfile = 319,
     T_Loopstats = 320,
     T_Lowpriotrap = 321,
     T_Manycastclient = 322,
     T_Manycastserver = 323,
     T_Mask = 324,
     T_Maxclock = 325,
     T_Maxdist = 326,
     T_Maxhop = 327,
     T_Maxpoll = 328,
     T_Minclock = 329,
     T_Mindist = 330,
     T_Minimum = 331,
     T_Minpoll = 332,
     T_Minsane = 333,
     T_Mode = 334,
     T_Monitor = 335,
     T_Month = 336,
     T_Multicastclient = 337,
     T_Nolink = 338,
     T_Nomodify = 339,
     T_None = 340,
     T_Nopeer = 341,
     T_Noquery = 342,
     T_Noselect = 343,
     T_Noserve = 344,
     T_Notrap = 345,
     T_Notrust = 346,
     T_Novolley = 347,
     T_Ntp = 348,
     T_Ntpport = 349,
     T_Orphan = 350,
     T_Panic = 351,
     T_Peer = 352,
     T_Peerstats = 353,
     T_Phone = 354,
     T_Pid = 355,
     T_Pidfile = 356,
     T_Pool = 357,
     T_Port = 358,
     T_Pps = 359,
     T_Preempt = 360,
     T_Prefer = 361,
     T_Protostats = 362,
     T_Pw = 363,
     T_Qos = 364,
     T_RandFile = 365,
     T_Rawstats = 366,
     T_Refid = 367,
     T_Requestkey = 368,
     T_Restrict = 369,
     T_Revoke = 370,
     T_Server = 371,
     T_Setvar = 372,
     T_Sign = 373,
     T_Statistics = 374,
     T_Stats = 375,
     T_Statsdir = 376,
     T_Step = 377,
     T_Stepout = 378,
     T_Stratum = 379,
     T_String = 380,
     T_Sysstats = 381,
     T_Tick = 382,
     T_Time1 = 383,
     T_Time2 = 384,
     T_Tinker = 385,
     T_Tos = 386,
     T_Trap = 387,
     T_True = 388,
     T_Trustedkey = 389,
     T_Ttl = 390,
     T_Type = 391,
     T_Version = 392,
     T_Week = 393,
     T_Year = 394,
     T_Flag = 395,
     T_Void = 396,
     T_EOC = 397,
     T_Simulate = 398,
     T_Beep_Delay = 399,
     T_Sim_Duration = 400,
     T_Server_Offset = 401,
     T_Duration = 402,
     T_Freq_Offset = 403,
     T_Wander = 404,
     T_Jitter = 405,
     T_Prop_Delay = 406,
     T_Proc_Delay = 407
   };
#endif
/* Tokens.  */
#define T_Age 258
#define T_Allan 259
#define T_Auth 260
#define T_Autokey 261
#define T_Automax 262
#define T_Average 263
#define T_Bclient 264
#define T_Beacon 265
#define T_Broadcast 266
#define T_Broadcastclient 267
#define T_Broadcastdelay 268
#define T_Burst 269
#define T_Calibrate 270
#define T_Calldelay 271
#define T_Ceiling 272
#define T_Clockstats 273
#define T_Cohort 274
#define T_ControlKey 275
#define T_Crypto 276
#define T_Cryptostats 277
#define T_Day 278
#define T_Default 279
#define T_Disable 280
#define T_Discard 281
#define T_Dispersion 282
#define T_Double 283
#define T_Driftfile 284
#define T_WanderThreshold 285
#define T_Enable 286
#define T_End 287
#define T_False 288
#define T_File 289
#define T_Filegen 290
#define T_Flag1 291
#define T_Flag2 292
#define T_Flag3 293
#define T_Flag4 294
#define T_Flake 295
#define T_Floor 296
#define T_Freq 297
#define T_Fudge 298
#define T_Host 299
#define T_Huffpuff 300
#define T_IPv4_address 301
#define T_IPv6_address 302
#define T_Iburst 303
#define T_Ident 304
#define T_Ignore 305
#define T_Includefile 306
#define T_Integer 307
#define T_Interface 308
#define T_Kernel 309
#define T_Key 310
#define T_Keys 311
#define T_Keysdir 312
#define T_Kod 313
#define T_Leap 314
#define T_Leapfile 315
#define T_Limited 316
#define T_Link 317
#define T_Logconfig 318
#define T_Logfile 319
#define T_Loopstats 320
#define T_Lowpriotrap 321
#define T_Manycastclient 322
#define T_Manycastserver 323
#define T_Mask 324
#define T_Maxclock 325
#define T_Maxdist 326
#define T_Maxhop 327
#define T_Maxpoll 328
#define T_Minclock 329
#define T_Mindist 330
#define T_Minimum 331
#define T_Minpoll 332
#define T_Minsane 333
#define T_Mode 334
#define T_Monitor 335
#define T_Month 336
#define T_Multicastclient 337
#define T_Nolink 338
#define T_Nomodify 339
#define T_None 340
#define T_Nopeer 341
#define T_Noquery 342
#define T_Noselect 343
#define T_Noserve 344
#define T_Notrap 345
#define T_Notrust 346
#define T_Novolley 347
#define T_Ntp 348
#define T_Ntpport 349
#define T_Orphan 350
#define T_Panic 351
#define T_Peer 352
#define T_Peerstats 353
#define T_Phone 354
#define T_Pid 355
#define T_Pidfile 356
#define T_Pool 357
#define T_Port 358
#define T_Pps 359
#define T_Preempt 360
#define T_Prefer 361
#define T_Protostats 362
#define T_Pw 363
#define T_Qos 364
#define T_RandFile 365
#define T_Rawstats 366
#define T_Refid 367
#define T_Requestkey 368
#define T_Restrict 369
#define T_Revoke 370
#define T_Server 371
#define T_Setvar 372
#define T_Sign 373
#define T_Statistics 374
#define T_Stats 375
#define T_Statsdir 376
#define T_Step 377
#define T_Stepout 378
#define T_Stratum 379
#define T_String 380
#define T_Sysstats 381
#define T_Tick 382
#define T_Time1 383
#define T_Time2 384
#define T_Tinker 385
#define T_Tos 386
#define T_Trap 387
#define T_True 388
#define T_Trustedkey 389
#define T_Ttl 390
#define T_Type 391
#define T_Version 392
#define T_Week 393
#define T_Year 394
#define T_Flag 395
#define T_Void 396
#define T_EOC 397
#define T_Simulate 398
#define T_Beep_Delay 399
#define T_Sim_Duration 400
#define T_Server_Offset 401
#define T_Duration 402
#define T_Freq_Offset 403
#define T_Wander 404
#define T_Jitter 405
#define T_Prop_Delay 406
#define T_Proc_Delay 407




/* Copy the first part of user declarations.  */
#line 11 "ntp_parser.y"

  #ifdef HAVE_CONFIG_H
  # include <config.h>
  #endif

  #include "ntpd.h"
  #include "ntp_machine.h"
  #include "ntp.h"
  #include "ntp_stdlib.h"
  #include "ntp_filegen.h"
  #include "ntp_data_structures.h"
  #include "ntp_scanner.h"
  #include "ntp_config.h"
  #include "ntp_crypto.h"

  #include "ntpsim.h"		/* HMS: Do we really want this all the time? */
                                /* SK: It might be a good idea to always
				   include the simulator code. That way
				   someone can use the same configuration file
				   for both the simulator and the daemon
				*/

  /*  #include "ntp_parser.h"     SK: Arent't we generating this using bison?
				   This was not an issue when we were
				   directly including the source file. However,
				   we will need a seperate description of the
				   tokens in the scanner.
				*/


  struct FILE_INFO *ip_file;   /* Pointer to the configuration file stream */

  #define YYERROR_VERBOSE
  void yyerror (char *msg);
  extern int input_from_file;  /* 0=input from ntpq>config command buffer */
  extern int cryptosw;


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
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 49 "ntp_parser.y"
{
    char   *String;
    double  Double;
    int     Integer;
    void   *VoidPtr;
    queue  *Queue;
    struct attr_val *Attr_val;
    struct address_node *Address_node;
    struct setvar_node *Set_var;

    /* Simulation types */
    server_info *Sim_server;
    script_info *Sim_script;
}
/* Line 193 of yacc.c.  */
#line 453 "../../ntpd/ntp_parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 466 "../../ntpd/ntp_parser.c"

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
# if YYENABLE_NLS
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
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
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
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  164
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   513

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  158
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  228
/* YYNRULES -- Number of states.  */
#define YYNSTATES  358

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   407

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   154,     2,   155,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   153,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   156,     2,   157,     2,     2,     2,     2,
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
     145,   146,   147,   148,   149,   150,   151,   152
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    15,    16,    18,    20,
      22,    24,    26,    28,    30,    32,    34,    36,    38,    42,
      45,    47,    49,    51,    53,    55,    57,    59,    62,    64,
      66,    69,    71,    73,    75,    77,    80,    83,    86,    88,
      90,    92,    94,    97,   100,   103,   105,   108,   111,   114,
     117,   120,   123,   126,   129,   132,   135,   137,   138,   141,
     143,   146,   149,   152,   155,   158,   161,   164,   167,   169,
     172,   175,   178,   181,   184,   187,   190,   193,   196,   199,
     202,   205,   208,   212,   215,   217,   219,   221,   223,   225,
     227,   229,   231,   234,   236,   239,   242,   244,   246,   248,
     250,   252,   254,   256,   258,   260,   262,   264,   267,   271,
     275,   281,   282,   285,   287,   289,   291,   293,   295,   297,
     299,   301,   303,   305,   307,   309,   311,   314,   316,   319,
     322,   325,   329,   332,   334,   337,   340,   343,   346,   349,
     352,   355,   358,   361,   364,   367,   369,   371,   373,   375,
     377,   379,   381,   383,   386,   389,   391,   394,   397,   400,
     403,   406,   409,   412,   416,   418,   421,   424,   427,   430,
     433,   436,   439,   442,   445,   448,   451,   455,   458,   461,
     463,   466,   467,   472,   476,   479,   481,   484,   487,   490,
     492,   495,   497,   499,   501,   503,   506,   508,   511,   513,
     516,   518,   520,   522,   524,   526,   528,   534,   536,   540,
     543,   547,   551,   554,   556,   562,   567,   571,   573,   575,
     578,   580,   587,   591,   594,   598,   602,   606,   610
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     159,     0,    -1,   160,    -1,   160,   161,   142,    -1,   161,
     142,    -1,     1,   142,    -1,    -1,   162,    -1,   168,    -1,
     169,    -1,   176,    -1,   182,    -1,   173,    -1,   187,    -1,
     190,    -1,   193,    -1,   196,    -1,   209,    -1,   163,   164,
     166,    -1,   163,   164,    -1,   116,    -1,   102,    -1,    97,
      -1,    11,    -1,    67,    -1,   165,    -1,   125,    -1,    52,
     125,    -1,    46,    -1,    47,    -1,   166,   167,    -1,   167,
      -1,     6,    -1,    14,    -1,    48,    -1,    55,    52,    -1,
      77,    52,    -1,    73,    52,    -1,    88,    -1,   105,    -1,
     106,    -1,   133,    -1,   135,    52,    -1,    79,    52,    -1,
     137,    52,    -1,    12,    -1,    12,    92,    -1,    68,   206,
      -1,    82,   206,    -1,     6,    52,    -1,    20,    52,    -1,
      21,   170,    -1,    56,   125,    -1,    57,   125,    -1,   113,
      52,    -1,   134,   204,    -1,   171,    -1,    -1,   171,   172,
      -1,   172,    -1,    44,   125,    -1,    49,   125,    -1,   108,
     125,    -1,   110,   125,    -1,   115,    52,    -1,   118,   125,
      -1,   131,   174,    -1,   174,   175,    -1,   175,    -1,    17,
      52,    -1,    41,    52,    -1,    19,   207,    -1,    95,    52,
      -1,    75,   208,    -1,    71,   208,    -1,    74,   208,    -1,
      70,   208,    -1,    78,    52,    -1,    10,    52,    -1,    72,
      52,    -1,   119,   177,    -1,   121,   125,    -1,    35,   178,
     179,    -1,   177,   178,    -1,   178,    -1,    18,    -1,    22,
      -1,    65,    -1,    98,    -1,   111,    -1,   126,    -1,   107,
      -1,   179,   180,    -1,   180,    -1,    34,   125,    -1,   136,
     181,    -1,    62,    -1,    83,    -1,    31,    -1,    25,    -1,
      85,    -1,   100,    -1,    23,    -1,   138,    -1,    81,    -1,
     139,    -1,     3,    -1,    26,   185,    -1,   114,   164,   183,
      -1,   114,    24,   183,    -1,   114,   165,    69,   165,   183,
      -1,    -1,   183,   184,    -1,    40,    -1,    50,    -1,    58,
      -1,    61,    -1,    66,    -1,    84,    -1,    86,    -1,    87,
      -1,    89,    -1,    90,    -1,    91,    -1,    94,    -1,   137,
      -1,   185,   186,    -1,   186,    -1,     8,    52,    -1,    76,
      52,    -1,    80,    52,    -1,    43,   164,   188,    -1,   188,
     189,    -1,   189,    -1,   128,   208,    -1,   129,   208,    -1,
     124,    52,    -1,   112,   125,    -1,    36,   207,    -1,    37,
     207,    -1,    38,   207,    -1,    39,   207,    -1,    31,   191,
      -1,    25,   191,    -1,   191,   192,    -1,   192,    -1,     5,
      -1,     9,    -1,    15,    -1,    54,    -1,    80,    -1,    93,
      -1,   120,    -1,   130,   194,    -1,   194,   195,    -1,   195,
      -1,     4,   208,    -1,    27,   208,    -1,    42,   208,    -1,
      45,   208,    -1,    96,   208,    -1,   122,   208,    -1,   123,
     208,    -1,    51,   125,   161,    -1,    32,    -1,    13,   208,
      -1,    16,    52,    -1,   127,   208,    -1,    29,   197,    -1,
      60,   125,    -1,   101,   125,    -1,    64,   125,    -1,     7,
      52,    -1,    63,   201,    -1,    99,   205,    -1,   117,   198,
      -1,   132,   165,   199,    -1,   135,   204,    -1,   109,   125,
      -1,   125,    -1,   125,    28,    -1,    -1,   125,   153,   125,
      24,    -1,   125,   153,   125,    -1,   199,   200,    -1,   200,
      -1,   103,    52,    -1,    53,   165,    -1,   201,   202,    -1,
     202,    -1,   203,   125,    -1,   125,    -1,   154,    -1,   155,
      -1,   153,    -1,   204,    52,    -1,    52,    -1,   205,   125,
      -1,   125,    -1,   206,   164,    -1,   164,    -1,    52,    -1,
     133,    -1,    33,    -1,    52,    -1,    28,    -1,   210,   156,
     211,   213,   157,    -1,   143,    -1,   211,   212,   142,    -1,
     212,   142,    -1,   144,   153,   208,    -1,   145,   153,   208,
      -1,   213,   214,    -1,   214,    -1,   216,   156,   215,   218,
     157,    -1,   146,   153,   208,   142,    -1,   116,   153,   217,
      -1,   165,    -1,   125,    -1,   218,   219,    -1,   219,    -1,
     147,   153,   208,   156,   220,   157,    -1,   220,   221,   142,
      -1,   221,   142,    -1,   148,   153,   208,    -1,   149,   153,
     208,    -1,   150,   153,   208,    -1,   151,   153,   208,    -1,
     152,   153,   208,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   281,   281,   285,   286,   291,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   321,   327,
     336,   337,   338,   339,   340,   344,   345,   346,   362,   363,
     367,   368,   373,   374,   375,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   395,   397,   399,   401,   412,
     414,   416,   423,   425,   427,   429,   434,   436,   440,   441,
     445,   447,   449,   451,   453,   455,   465,   470,   471,   475,
     477,   479,   481,   483,   485,   487,   489,   491,   493,   495,
     505,   507,   509,   517,   518,   522,   524,   526,   528,   530,
     532,   534,   539,   540,   544,   545,   546,   547,   548,   549,
     553,   554,   555,   556,   557,   558,   559,   568,   570,   575,
     580,   588,   589,   593,   594,   595,   596,   597,   598,   599,
     600,   601,   602,   603,   604,   605,   609,   610,   614,   615,
     616,   624,   629,   630,   634,   636,   638,   640,   642,   644,
     646,   648,   657,   659,   664,   665,   669,   670,   671,   672,
     673,   674,   676,   684,   688,   689,   693,   694,   695,   696,
     697,   698,   699,   707,   723,   729,   731,   733,   735,   737,
     740,   742,   744,   747,   749,   751,   753,   755,   757,   761,
     763,   766,   771,   773,   779,   780,   784,   785,   790,   791,
     795,   796,   813,   814,   815,   824,   825,   829,   830,   834,
     835,   839,   848,   849,   853,   854,   862,   877,   881,   882,
     886,   887,   891,   892,   896,   901,   905,   909,   910,   914,
     915,   919,   924,   925,   929,   931,   933,   935,   937
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_Age", "T_Allan", "T_Auth",
  "T_Autokey", "T_Automax", "T_Average", "T_Bclient", "T_Beacon",
  "T_Broadcast", "T_Broadcastclient", "T_Broadcastdelay", "T_Burst",
  "T_Calibrate", "T_Calldelay", "T_Ceiling", "T_Clockstats", "T_Cohort",
  "T_ControlKey", "T_Crypto", "T_Cryptostats", "T_Day", "T_Default",
  "T_Disable", "T_Discard", "T_Dispersion", "T_Double", "T_Driftfile",
  "T_WanderThreshold", "T_Enable", "T_End", "T_False", "T_File",
  "T_Filegen", "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4", "T_Flake",
  "T_Floor", "T_Freq", "T_Fudge", "T_Host", "T_Huffpuff", "T_IPv4_address",
  "T_IPv6_address", "T_Iburst", "T_Ident", "T_Ignore", "T_Includefile",
  "T_Integer", "T_Interface", "T_Kernel", "T_Key", "T_Keys", "T_Keysdir",
  "T_Kod", "T_Leap", "T_Leapfile", "T_Limited", "T_Link", "T_Logconfig",
  "T_Logfile", "T_Loopstats", "T_Lowpriotrap", "T_Manycastclient",
  "T_Manycastserver", "T_Mask", "T_Maxclock", "T_Maxdist", "T_Maxhop",
  "T_Maxpoll", "T_Minclock", "T_Mindist", "T_Minimum", "T_Minpoll",
  "T_Minsane", "T_Mode", "T_Monitor", "T_Month", "T_Multicastclient",
  "T_Nolink", "T_Nomodify", "T_None", "T_Nopeer", "T_Noquery",
  "T_Noselect", "T_Noserve", "T_Notrap", "T_Notrust", "T_Novolley",
  "T_Ntp", "T_Ntpport", "T_Orphan", "T_Panic", "T_Peer", "T_Peerstats",
  "T_Phone", "T_Pid", "T_Pidfile", "T_Pool", "T_Port", "T_Pps",
  "T_Preempt", "T_Prefer", "T_Protostats", "T_Pw", "T_Qos", "T_RandFile",
  "T_Rawstats", "T_Refid", "T_Requestkey", "T_Restrict", "T_Revoke",
  "T_Server", "T_Setvar", "T_Sign", "T_Statistics", "T_Stats",
  "T_Statsdir", "T_Step", "T_Stepout", "T_Stratum", "T_String",
  "T_Sysstats", "T_Tick", "T_Time1", "T_Time2", "T_Tinker", "T_Tos",
  "T_Trap", "T_True", "T_Trustedkey", "T_Ttl", "T_Type", "T_Version",
  "T_Week", "T_Year", "T_Flag", "T_Void", "T_EOC", "T_Simulate",
  "T_Beep_Delay", "T_Sim_Duration", "T_Server_Offset", "T_Duration",
  "T_Freq_Offset", "T_Wander", "T_Jitter", "T_Prop_Delay", "T_Proc_Delay",
  "'='", "'+'", "'-'", "'{'", "'}'", "$accept", "configuration",
  "command_list", "command", "server_command", "client_type", "address",
  "ip_address", "option_list", "option", "other_mode_command",
  "authentication_command", "crypto_command_line", "crypto_command_list",
  "crypto_command", "orphan_mode_command", "tos_option_list", "tos_option",
  "monitoring_command", "stats_list", "stat", "filegen_option_list",
  "filegen_option", "filegen_type", "access_control_command",
  "ac_flag_list", "access_control_flag", "discard_option_list",
  "discard_option", "fudge_command", "fudge_factor_list", "fudge_factor",
  "system_option_command", "system_option_list", "system_option",
  "tinker_command", "tinker_option_list", "tinker_option",
  "miscellaneous_command", "drift_parm", "variable_assign",
  "trap_option_list", "trap_option", "log_config_list",
  "log_config_command", "log_config_prefix", "integer_list", "string_list",
  "address_list", "boolean", "number", "simulate_command",
  "sim_conf_start", "sim_init_statement_list", "sim_init_statement",
  "sim_server_list", "sim_server", "sim_server_offset", "sim_server_name",
  "sim_address", "sim_act_list", "sim_act", "sim_act_stmt_list",
  "sim_act_stmt", 0
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
     405,   406,   407,    61,    43,    45,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   158,   159,   160,   160,   160,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   162,   162,
     163,   163,   163,   163,   163,   164,   164,   164,   165,   165,
     166,   166,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   168,   168,   168,   168,   169,
     169,   169,   169,   169,   169,   169,   170,   170,   171,   171,
     172,   172,   172,   172,   172,   172,   173,   174,   174,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     176,   176,   176,   177,   177,   178,   178,   178,   178,   178,
     178,   178,   179,   179,   180,   180,   180,   180,   180,   180,
     181,   181,   181,   181,   181,   181,   181,   182,   182,   182,
     182,   183,   183,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   185,   185,   186,   186,
     186,   187,   188,   188,   189,   189,   189,   189,   189,   189,
     189,   189,   190,   190,   191,   191,   192,   192,   192,   192,
     192,   192,   192,   193,   194,   194,   195,   195,   195,   195,
     195,   195,   195,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   197,
     197,   197,   198,   198,   199,   199,   200,   200,   201,   201,
     202,   202,   203,   203,   203,   204,   204,   205,   205,   206,
     206,   207,   207,   207,   208,   208,   209,   210,   211,   211,
     212,   212,   213,   213,   214,   215,   216,   217,   217,   218,
     218,   219,   220,   220,   221,   221,   221,   221,   221
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       2,     1,     1,     1,     1,     2,     2,     2,     1,     1,
       1,     1,     2,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     0,     2,     1,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     3,     3,
       5,     0,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     2,
       2,     3,     2,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     3,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     3,     2,     2,     1,
       2,     0,     4,     3,     2,     1,     2,     2,     2,     1,
       2,     1,     1,     1,     1,     2,     1,     2,     1,     2,
       1,     1,     1,     1,     1,     1,     5,     1,     3,     2,
       3,     3,     2,     1,     5,     4,     3,     1,     1,     2,
       1,     6,     3,     2,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,    23,    45,     0,     0,     0,    57,
       0,     0,   181,     0,   164,     0,     0,     0,     0,     0,
       0,     0,     0,    24,     0,     0,    22,     0,     0,    21,
       0,     0,     0,    20,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   207,     0,     2,     0,     7,     0,     8,
       9,    12,    10,    11,    13,    14,    15,    16,    17,     0,
       5,    49,   172,    46,   205,   204,   165,   166,    50,     0,
       0,     0,     0,     0,     0,    51,    56,    59,   146,   147,
     148,   149,   150,   151,   152,   143,   145,     0,     0,     0,
     107,   127,   179,   168,   142,    85,    86,    87,    88,    91,
      89,    90,     0,    28,    29,     0,    26,     0,    25,     6,
      52,    53,   169,   191,   194,   192,   193,   173,   189,     0,
     171,   200,    47,    48,   198,   174,   170,   178,    54,   111,
     111,    25,     0,   175,    80,    84,    81,   167,     0,     0,
       0,     0,     0,     0,     0,   153,   155,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    66,    68,
       0,   196,    55,   177,     1,     0,     4,    19,     0,    60,
      61,    62,    63,    64,    65,    58,   144,   128,   129,   130,
     126,   180,    99,    98,     0,    96,    97,     0,    82,    93,
      27,     0,     0,     0,     0,     0,     0,     0,     0,   131,
     133,   163,   188,   190,   199,   197,   109,   108,     0,     0,
      83,   156,   157,   158,   159,   160,   161,   162,   154,    78,
      69,   203,   201,   202,    71,    70,    76,    74,    79,    75,
      73,    77,    72,    67,     0,     0,   176,   185,   195,     3,
      32,    33,    34,     0,     0,     0,     0,    38,    39,    40,
      41,     0,     0,    18,    31,     0,     0,     0,     0,    94,
     106,   102,   104,   100,   101,   103,   105,    95,    92,   138,
     139,   140,   141,   137,   136,   134,   135,   132,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   112,   111,   183,   187,   186,   184,    35,    37,    36,
      43,    42,    44,    30,     0,     0,     0,     0,     0,   213,
       0,   209,   110,   182,   210,   211,     0,   208,   206,   212,
       0,   218,   217,   216,     0,     0,     0,     0,     0,   220,
       0,     0,   214,   219,   215,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   221,
       0,   223,   224,   225,   226,   227,   228,   222
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    44,    45,    46,    47,    48,   121,   108,   253,   254,
      49,    50,    75,    76,    77,    51,   158,   159,    52,   134,
     102,   188,   189,   267,    53,   206,   291,    90,    91,    54,
     199,   200,    55,    85,    86,    56,   145,   146,    57,    93,
     133,   236,   237,   117,   118,   119,   162,   125,   122,   224,
      66,    58,    59,   257,   258,   308,   309,   325,   310,   323,
     328,   329,   342,   343
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -129
static const yytype_int16 yypact[] =
{
      11,  -126,   -18,    -7,  -129,   -45,   -13,     7,    13,   124,
      43,     0,   -53,    43,  -129,    64,   -27,   -48,   -40,   -29,
      -9,  -104,    -6,  -129,   -27,   -27,  -129,    12,    14,  -129,
      19,    42,     9,  -129,    22,    64,    23,   -13,    -1,   418,
     -33,    48,    48,  -129,   152,   215,    16,  -129,   -27,  -129,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,     3,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,    31,
      44,    47,    49,   115,    51,  -129,   124,  -129,  -129,  -129,
    -129,  -129,  -129,  -129,  -129,    43,  -129,   118,   126,   127,
       0,  -129,   154,  -129,    43,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,     4,  -129,  -129,    55,  -129,   162,  -129,   348,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -104,  -129,    60,
    -129,  -129,   -27,   -27,  -129,    61,  -129,  -129,  -129,  -129,
    -129,   122,    34,  -129,    64,  -129,  -129,  -129,   -13,   -13,
     -13,   -13,   -13,   -13,   -13,    -1,  -129,   140,   157,   -22,
     158,   -13,   -13,   160,   -13,   -13,   168,   171,   418,  -129,
     -46,  -129,   172,   172,  -129,    72,  -129,    78,   -81,  -129,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,  -129,   100,  -129,  -129,    50,     4,  -129,
    -129,   -22,   -22,   -22,   -22,   104,   178,   -13,   -13,   162,
    -129,  -129,  -129,  -129,  -129,  -129,   335,   335,   -33,   108,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,  -129,   -33,   185,   -46,  -129,  -129,  -129,
    -129,  -129,  -129,   186,   191,   193,   196,  -129,  -129,  -129,
    -129,   197,   199,    78,  -129,    99,   101,   -56,   111,  -129,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,   233,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,  -129,   -13,   -13,   106,   119,   -88,  -129,
     107,  -129,   335,  -129,  -129,  -129,   -42,  -129,  -129,  -129,
     114,  -129,  -129,  -129,   109,   117,   -13,   112,   -66,  -129,
     128,   -13,  -129,  -129,  -129,   113,    56,   120,   123,   131,
     134,   135,    45,   138,   -13,   -13,   -13,   -13,   -13,  -129,
     139,  -129,  -129,  -129,  -129,  -129,  -129,  -129
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -129,  -129,  -129,   -39,  -129,  -129,   133,   -31,  -129,    24,
    -129,  -129,  -129,  -129,   216,  -129,  -129,   137,  -129,  -129,
     -25,  -129,   105,  -129,  -129,  -128,  -129,  -129,   206,  -129,
    -129,   102,  -129,   285,     5,  -129,  -129,   155,  -129,  -129,
    -129,  -129,    63,  -129,   187,  -129,   260,  -129,   278,    25,
     -37,  -129,  -129,  -129,    58,  -129,    -3,  -129,  -129,  -129,
    -129,   -15,  -129,   -36
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     137,   131,   207,   138,   103,   104,   165,   234,    87,   160,
     135,   221,     1,   103,   104,    64,    60,     2,     3,   103,
     104,   113,     4,     5,     6,   105,   139,     7,   306,   182,
     222,     8,     9,   129,    61,   183,    10,    11,   184,    65,
      12,   140,    13,    14,   141,    62,    15,    63,    78,   114,
     115,   116,    79,   260,    16,   103,   104,   235,    80,    67,
     306,   105,    17,   255,   256,    68,   185,    18,    19,   318,
     201,    20,    92,   261,    21,    22,    88,   109,    23,    24,
      89,   327,    95,   321,   240,   110,    96,   186,   255,   256,
     176,   332,   241,    25,   128,   142,   111,    81,   106,   176,
     161,   211,   212,   213,   214,   215,   216,   217,    26,   210,
      27,   223,    28,    29,   226,   227,   112,   229,   230,   120,
      30,   143,   144,    82,    31,    32,   242,    33,    34,    97,
      35,   262,    36,   243,   106,   263,    83,   124,    37,   126,
     187,    38,    39,    40,   127,    41,    42,   132,   136,   107,
     264,   244,   164,    -6,    43,   245,   169,   246,   166,   168,
     275,   276,    98,    84,   312,   130,   247,   173,    69,   170,
     177,    99,   171,    70,   172,   100,   174,   292,   178,   179,
     190,   167,   181,   248,   249,   203,   205,   209,   265,   266,
     101,   208,   219,   337,   338,   339,   340,   341,   191,   192,
     193,   194,   349,   294,   337,   338,   339,   340,   341,   220,
     225,   250,   228,   251,   239,   252,   269,   270,   271,   272,
     231,     2,     3,   232,   238,   259,     4,     5,     6,   273,
     274,     7,    71,   293,    72,     8,     9,   295,   297,    73,
      10,    11,    74,   298,    12,   299,    13,    14,   300,   301,
      15,   302,   304,   311,   305,   204,   204,   313,    16,   316,
     324,   317,   326,   320,   327,   331,    17,   314,   315,   336,
     334,    18,    19,   344,   195,    20,   345,   303,    21,    22,
     351,   357,    23,    24,   346,   322,   196,   347,   348,   330,
     197,   198,   175,   268,   335,   233,   180,    25,    94,   296,
     218,   277,   163,   123,   202,   319,   350,   352,   353,   354,
     355,   356,    26,   333,    27,   307,    28,    29,     0,     0,
       0,     0,     0,     0,    30,     0,     0,     0,    31,    32,
       0,    33,    34,     0,    35,     0,    36,     0,     0,     0,
       0,     0,    37,     0,     0,    38,    39,    40,     0,    41,
      42,     0,     0,     0,     2,     3,     0,    -6,    43,     4,
       5,     6,     0,     0,     7,     0,     0,     0,     8,     9,
       0,     0,     0,    10,    11,   278,     0,    12,     0,    13,
      14,     0,     0,    15,     0,   279,     0,     0,     0,     0,
       0,    16,     0,   280,     0,     0,   281,     0,     0,    17,
       0,   282,     0,     0,    18,    19,     0,     0,    20,     0,
       0,    21,    22,     0,     0,    23,    24,     0,     0,   283,
       0,   284,   285,     0,   286,   287,   288,     0,   147,   289,
      25,     0,     0,     0,     0,   148,     0,   149,     0,     0,
       0,     0,     0,     0,     0,    26,     0,    27,     0,    28,
      29,     0,     0,     0,     0,     0,     0,    30,     0,   150,
       0,    31,    32,     0,    33,    34,     0,    35,     0,    36,
       0,     0,   290,     0,     0,    37,     0,     0,    38,    39,
      40,     0,    41,    42,     0,     0,     0,     0,   151,   152,
     153,    43,   154,   155,     0,     0,   156,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   157
};

static const yytype_int16 yycheck[] =
{
      37,    32,   130,     4,    46,    47,    45,    53,     8,    40,
      35,    33,     1,    46,    47,    28,   142,     6,     7,    46,
      47,   125,    11,    12,    13,    52,    27,    16,   116,    25,
      52,    20,    21,    24,    52,    31,    25,    26,    34,    52,
      29,    42,    31,    32,    45,    52,    35,    92,     5,   153,
     154,   155,     9,     3,    43,    46,    47,   103,    15,    52,
     116,    52,    51,   144,   145,    52,    62,    56,    57,   157,
     109,    60,   125,    23,    63,    64,    76,   125,    67,    68,
      80,   147,    18,   125,     6,   125,    22,    83,   144,   145,
      85,   157,    14,    82,    52,    96,   125,    54,   125,    94,
      52,   138,   139,   140,   141,   142,   143,   144,    97,   134,
      99,   133,   101,   102,   151,   152,   125,   154,   155,   125,
     109,   122,   123,    80,   113,   114,    48,   116,   117,    65,
     119,    81,   121,    55,   125,    85,    93,   125,   127,   125,
     136,   130,   131,   132,   125,   134,   135,   125,   125,    16,
     100,    73,     0,   142,   143,    77,   125,    79,   142,   156,
     197,   198,    98,   120,   292,    32,    88,    52,    44,   125,
      52,   107,   125,    49,   125,   111,   125,   208,    52,    52,
     125,    48,    28,   105,   106,   125,   125,   153,   138,   139,
     126,    69,    52,   148,   149,   150,   151,   152,    36,    37,
      38,    39,   157,   234,   148,   149,   150,   151,   152,    52,
      52,   133,    52,   135,   142,   137,   191,   192,   193,   194,
      52,     6,     7,    52,    52,   125,    11,    12,    13,   125,
      52,    16,   108,   125,   110,    20,    21,    52,    52,   115,
      25,    26,   118,    52,    29,    52,    31,    32,    52,    52,
      35,    52,   153,   142,   153,   122,   123,    24,    43,   153,
     146,   142,   153,   156,   147,   153,    51,   304,   305,   156,
     142,    56,    57,   153,   112,    60,   153,   253,    63,    64,
     142,   142,    67,    68,   153,   316,   124,   153,   153,   326,
     128,   129,    76,   188,   331,   158,    90,    82,    13,   236,
     145,   199,    42,    25,   117,   308,   342,   344,   345,   346,
     347,   348,    97,   328,    99,   257,   101,   102,    -1,    -1,
      -1,    -1,    -1,    -1,   109,    -1,    -1,    -1,   113,   114,
      -1,   116,   117,    -1,   119,    -1,   121,    -1,    -1,    -1,
      -1,    -1,   127,    -1,    -1,   130,   131,   132,    -1,   134,
     135,    -1,    -1,    -1,     6,     7,    -1,   142,   143,    11,
      12,    13,    -1,    -1,    16,    -1,    -1,    -1,    20,    21,
      -1,    -1,    -1,    25,    26,    40,    -1,    29,    -1,    31,
      32,    -1,    -1,    35,    -1,    50,    -1,    -1,    -1,    -1,
      -1,    43,    -1,    58,    -1,    -1,    61,    -1,    -1,    51,
      -1,    66,    -1,    -1,    56,    57,    -1,    -1,    60,    -1,
      -1,    63,    64,    -1,    -1,    67,    68,    -1,    -1,    84,
      -1,    86,    87,    -1,    89,    90,    91,    -1,    10,    94,
      82,    -1,    -1,    -1,    -1,    17,    -1,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    97,    -1,    99,    -1,   101,
     102,    -1,    -1,    -1,    -1,    -1,    -1,   109,    -1,    41,
      -1,   113,   114,    -1,   116,   117,    -1,   119,    -1,   121,
      -1,    -1,   137,    -1,    -1,   127,    -1,    -1,   130,   131,
     132,    -1,   134,   135,    -1,    -1,    -1,    -1,    70,    71,
      72,   143,    74,    75,    -1,    -1,    78,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     6,     7,    11,    12,    13,    16,    20,    21,
      25,    26,    29,    31,    32,    35,    43,    51,    56,    57,
      60,    63,    64,    67,    68,    82,    97,    99,   101,   102,
     109,   113,   114,   116,   117,   119,   121,   127,   130,   131,
     132,   134,   135,   143,   159,   160,   161,   162,   163,   168,
     169,   173,   176,   182,   187,   190,   193,   196,   209,   210,
     142,    52,    52,    92,    28,    52,   208,    52,    52,    44,
      49,   108,   110,   115,   118,   170,   171,   172,     5,     9,
      15,    54,    80,    93,   120,   191,   192,     8,    76,    80,
     185,   186,   125,   197,   191,    18,    22,    65,    98,   107,
     111,   126,   178,    46,    47,    52,   125,   164,   165,   125,
     125,   125,   125,   125,   153,   154,   155,   201,   202,   203,
     125,   164,   206,   206,   125,   205,   125,   125,    52,    24,
     164,   165,   125,   198,   177,   178,   125,   208,     4,    27,
      42,    45,    96,   122,   123,   194,   195,    10,    17,    19,
      41,    70,    71,    72,    74,    75,    78,    95,   174,   175,
     165,    52,   204,   204,     0,   161,   142,   164,   156,   125,
     125,   125,   125,    52,   125,   172,   192,    52,    52,    52,
     186,    28,    25,    31,    34,    62,    83,   136,   179,   180,
     125,    36,    37,    38,    39,   112,   124,   128,   129,   188,
     189,   161,   202,   125,   164,   125,   183,   183,    69,   153,
     178,   208,   208,   208,   208,   208,   208,   208,   195,    52,
      52,    33,    52,   133,   207,    52,   208,   208,    52,   208,
     208,    52,    52,   175,    53,   103,   199,   200,    52,   142,
       6,    14,    48,    55,    73,    77,    79,    88,   105,   106,
     133,   135,   137,   166,   167,   144,   145,   211,   212,   125,
       3,    23,    81,    85,   100,   138,   139,   181,   180,   207,
     207,   207,   207,   125,    52,   208,   208,   189,    40,    50,
      58,    61,    66,    84,    86,    87,    89,    90,    91,    94,
     137,   184,   165,   125,   165,    52,   200,    52,    52,    52,
      52,    52,    52,   167,   153,   153,   116,   212,   213,   214,
     216,   142,   183,    24,   208,   208,   153,   142,   157,   214,
     156,   125,   165,   217,   146,   215,   153,   147,   218,   219,
     208,   153,   157,   219,   142,   208,   156,   148,   149,   150,
     151,   152,   220,   221,   153,   153,   153,   153,   153,   157,
     221,   142,   208,   208,   208,   208,   208,   142
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
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

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
# if YYLTYPE_IS_TRIVIAL
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
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
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
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
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



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

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
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
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
#line 292 "ntp_parser.y"
    {
					if (input_from_file == 1) {
			msyslog(LOG_INFO, "parse error %s line %d ignored\n",
                            ip_file->fname, ip_file->line_no);
			} else if (input_from_file != 0)
				msyslog(LOG_INFO,
				    "parse: bad boolean input flag\n");
                }
    break;

  case 18:
#line 322 "ntp_parser.y"
    {
                        struct peer_node *my_node =  create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue));
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;

  case 19:
#line 328 "ntp_parser.y"
    {
                        struct peer_node *my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;

  case 20:
#line 336 "ntp_parser.y"
    { (yyval.Integer) = T_Server; }
    break;

  case 21:
#line 337 "ntp_parser.y"
    { (yyval.Integer) = T_Pool; }
    break;

  case 22:
#line 338 "ntp_parser.y"
    { (yyval.Integer) = T_Peer; }
    break;

  case 23:
#line 339 "ntp_parser.y"
    { (yyval.Integer) = T_Broadcast; }
    break;

  case 24:
#line 340 "ntp_parser.y"
    { (yyval.Integer) = T_Manycastclient; }
    break;

  case 25:
#line 344 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(1) - (1)].Address_node); }
    break;

  case 26:
#line 345 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), default_ai_family); }
    break;

  case 27:
#line 347 "ntp_parser.y"
    {
                        if ((yyvsp[(1) - (2)].Integer) == -4)
                            (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET);
                        else if ((yyvsp[(1) - (2)].Integer) == -6)
                            (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET6);
                        else {
                            yyerror("Invalid address type specified. Assuming default.\n");
                            (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), default_ai_family);
                        }
                    }
    break;

  case 28:
#line 362 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_INET); }
    break;

  case 29:
#line 363 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_INET6); }
    break;

  case 30:
#line 367 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 31:
#line 368 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 32:
#line 373 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_SKEY); }
    break;

  case 33:
#line 374 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_BURST); }
    break;

  case 34:
#line 375 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_IBURST); }
    break;

  case 35:
#line 376 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Key, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 36:
#line 377 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Minpoll, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 37:
#line 378 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Maxpoll, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 38:
#line 379 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_NOSELECT);}
    break;

  case 39:
#line 380 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREEMPT); }
    break;

  case 40:
#line 381 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREFER); }
    break;

  case 41:
#line 382 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_TRUE); }
    break;

  case 42:
#line 383 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Ttl, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 43:
#line 384 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Mode, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 44:
#line 385 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Version, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:
#line 396 "ntp_parser.y"
    { my_config.broadcastclient = SIMPLE; }
    break;

  case 46:
#line 398 "ntp_parser.y"
    { my_config.broadcastclient = NOVOLLEY;  }
    break;

  case 47:
#line 400 "ntp_parser.y"
    { append_queue(my_config.manycastserver, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 48:
#line 402 "ntp_parser.y"
    { append_queue(my_config.multicastclient, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 49:
#line 413 "ntp_parser.y"
    { my_config.auth.autokey = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 50:
#line 415 "ntp_parser.y"
    { my_config.auth.control_key = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 51:
#line 417 "ntp_parser.y"
    { if (my_config.auth.crypto_cmd_list != NULL)
					append_queue(my_config.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Queue));
				else
					my_config.auth.crypto_cmd_list = (yyvsp[(2) - (2)].Queue);
			cryptosw++;
			}
    break;

  case 52:
#line 424 "ntp_parser.y"
    { my_config.auth.keys = (yyvsp[(2) - (2)].String);  }
    break;

  case 53:
#line 426 "ntp_parser.y"
    { my_config.auth.keysdir = (yyvsp[(2) - (2)].String);  }
    break;

  case 54:
#line 428 "ntp_parser.y"
    { my_config.auth.requested_key = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 55:
#line 430 "ntp_parser.y"
    { my_config.auth.trusted_key_list = (yyvsp[(2) - (2)].Queue);  }
    break;

  case 57:
#line 436 "ntp_parser.y"
    { (yyval.Queue) = NULL; }
    break;

  case 58:
#line 440 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 59:
#line 441 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 60:
#line 446 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PRIV, (yyvsp[(2) - (2)].String)); }
    break;

  case 61:
#line 448 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_IDENT, (yyvsp[(2) - (2)].String)); }
    break;

  case 62:
#line 450 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PW, (yyvsp[(2) - (2)].String)); }
    break;

  case 63:
#line 452 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_RAND, (yyvsp[(2) - (2)].String)); }
    break;

  case 64:
#line 454 "ntp_parser.y"
    { my_config.auth.revoke = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 65:
#line 456 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_SIGN, (yyvsp[(2) - (2)].String)); }
    break;

  case 66:
#line 466 "ntp_parser.y"
    { append_queue(my_config.orphan_cmds,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 67:
#line 470 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 68:
#line 471 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 69:
#line 476 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_CEILING, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 70:
#line 478 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_FLOOR, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 71:
#line 480 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_COHORT, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 72:
#line 482 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_ORPHAN, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 73:
#line 484 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINDISP, (yyvsp[(2) - (2)].Double)); }
    break;

  case 74:
#line 486 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXDIST, (yyvsp[(2) - (2)].Double)); }
    break;

  case 75:
#line 488 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINCLOCK, (yyvsp[(2) - (2)].Double)); }
    break;

  case 76:
#line 490 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXCLOCK, (yyvsp[(2) - (2)].Double)); }
    break;

  case 77:
#line 492 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINSANE, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 78:
#line 494 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_BEACON, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 79:
#line 496 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXHOP, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 80:
#line 506 "ntp_parser.y"
    { append_queue(my_config.stats_list, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 81:
#line 508 "ntp_parser.y"
    { my_config.stats_dir = (yyvsp[(2) - (2)].String);  }
    break;

  case 82:
#line 510 "ntp_parser.y"
    {
                        enqueue(my_config.filegen_opts,
                                create_filegen_node((yyvsp[(2) - (3)].VoidPtr), (yyvsp[(3) - (3)].Queue)));
                    }
    break;

  case 83:
#line 517 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); }
    break;

  case 84:
#line 518 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].VoidPtr)); }
    break;

  case 85:
#line 523 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("clockstats"); }
    break;

  case 86:
#line 525 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("cryptostats"); }
    break;

  case 87:
#line 527 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("loopstats"); }
    break;

  case 88:
#line 529 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("peerstats"); }
    break;

  case 89:
#line 531 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("rawstats"); }
    break;

  case 90:
#line 533 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("sysstats"); }
    break;

  case 91:
#line 535 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("protostats"); }
    break;

  case 92:
#line 539 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 93:
#line 540 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 94:
#line 544 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(T_File, (yyvsp[(2) - (2)].String)); }
    break;

  case 95:
#line 545 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Type, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 96:
#line 546 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Link); }
    break;

  case 97:
#line 547 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Nolink); }
    break;

  case 98:
#line 548 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Enable); }
    break;

  case 99:
#line 549 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Disable); }
    break;

  case 100:
#line 553 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_NONE; }
    break;

  case 101:
#line 554 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_PID; }
    break;

  case 102:
#line 555 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_DAY; }
    break;

  case 103:
#line 556 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_WEEK; }
    break;

  case 104:
#line 557 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_MONTH; }
    break;

  case 105:
#line 558 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_YEAR; }
    break;

  case 106:
#line 559 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_AGE; }
    break;

  case 107:
#line 569 "ntp_parser.y"
    {   append_queue(my_config.discard_opts, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 108:
#line 571 "ntp_parser.y"
    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
                    }
    break;

  case 109:
#line 576 "ntp_parser.y"
    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
                    }
    break;

  case 110:
#line 581 "ntp_parser.y"
    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Queue), ip_file->line_no));
                    }
    break;

  case 111:
#line 588 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 112:
#line 589 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); }
    break;

  case 113:
#line 593 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_TIMEOUT); }
    break;

  case 114:
#line 594 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_IGNORE); }
    break;

  case 115:
#line 595 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_KOD); }
    break;

  case 116:
#line 596 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LIMITED); }
    break;

  case 117:
#line 597 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LPTRAP); }
    break;

  case 118:
#line 598 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOMODIFY); }
    break;

  case 119:
#line 599 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOPEER); }
    break;

  case 120:
#line 600 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOQUERY); }
    break;

  case 121:
#line 601 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTSERVE); }
    break;

  case 122:
#line 602 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOTRAP); }
    break;

  case 123:
#line 603 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTTRUST); }
    break;

  case 124:
#line 604 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RESM_NTPONLY); }
    break;

  case 125:
#line 605 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_VERSION); }
    break;

  case 126:
#line 609 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 127:
#line 610 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 128:
#line 614 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Average, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 129:
#line 615 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Minimum, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 130:
#line 616 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Monitor, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 131:
#line 625 "ntp_parser.y"
    { enqueue(my_config.fudge, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue)));  }
    break;

  case 132:
#line 629 "ntp_parser.y"
    { enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 133:
#line 630 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 134:
#line 635 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME1, (yyvsp[(2) - (2)].Double)); }
    break;

  case 135:
#line 637 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME2, (yyvsp[(2) - (2)].Double)); }
    break;

  case 136:
#line 639 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEVAL1,  (yyvsp[(2) - (2)].Integer)); }
    break;

  case 137:
#line 641 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CLK_HAVEVAL2,  (yyvsp[(2) - (2)].String)); }
    break;

  case 138:
#line 643 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG1, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 139:
#line 645 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG2, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 140:
#line 647 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG3, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 141:
#line 649 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG4, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 142:
#line 658 "ntp_parser.y"
    { append_queue(my_config.enable_opts,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 143:
#line 660 "ntp_parser.y"
    { append_queue(my_config.disable_opts,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 144:
#line 664 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 145:
#line 665 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 146:
#line 669 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); }
    break;

  case 147:
#line 670 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_BROADCLIENT); }
    break;

  case 148:
#line 671 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_CAL); }
    break;

  case 149:
#line 672 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_KERNEL); }
    break;

  case 150:
#line 673 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_MONITOR); }
    break;

  case 151:
#line 674 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_NTP); }
    break;

  case 152:
#line 676 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_FILEGEN); }
    break;

  case 153:
#line 684 "ntp_parser.y"
    { append_queue(my_config.tinker, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 154:
#line 688 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 155:
#line 689 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 156:
#line 693 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_ALLAN, (yyvsp[(2) - (2)].Double)); }
    break;

  case 157:
#line 694 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PHI, (yyvsp[(2) - (2)].Double)); }
    break;

  case 158:
#line 695 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_FREQ, (yyvsp[(2) - (2)].Double)); }
    break;

  case 159:
#line 696 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_HUFFPUFF, (yyvsp[(2) - (2)].Double)); }
    break;

  case 160:
#line 697 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PANIC, (yyvsp[(2) - (2)].Double)); }
    break;

  case 161:
#line 698 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MAX, (yyvsp[(2) - (2)].Double)); }
    break;

  case 162:
#line 699 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MINSTEP, (yyvsp[(2) - (2)].Double)); }
    break;

  case 163:
#line 708 "ntp_parser.y"
    {
                    if (curr_include_level >= MAXINCLUDELEVEL) {
                        fprintf(stderr, "getconfig: Maximum include file level exceeded.\n");
                        msyslog(LOG_INFO, "getconfig: Maximum include file level exceeded.");
                    }
                    else {
                        fp[curr_include_level + 1] = F_OPEN(FindConfig((yyvsp[(2) - (3)].String)), "r");
                        if (fp[curr_include_level + 1] == NULL) {
                            fprintf(stderr, "getconfig: Couldn't open <%s>\n", FindConfig((yyvsp[(2) - (3)].String)));
                            msyslog(LOG_INFO, "getconfig: Couldn't open <%s>", FindConfig((yyvsp[(2) - (3)].String)));
                        }
                        else
                            ip_file = fp[++curr_include_level];
                    }
                }
    break;

  case 164:
#line 724 "ntp_parser.y"
    {
                    while (curr_include_level != -1)
                        FCLOSE(fp[curr_include_level--]);
                }
    break;

  case 165:
#line 730 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, (yyvsp[(2) - (2)].Double)));  }
    break;

  case 166:
#line 732 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Calldelay, (yyvsp[(2) - (2)].Integer)));  }
    break;

  case 167:
#line 734 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Tick, (yyvsp[(2) - (2)].Double)));  }
    break;

  case 168:
#line 736 "ntp_parser.y"
    { /* Null action, possibly all null parms */ }
    break;

  case 169:
#line 738 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Leapfile, (yyvsp[(2) - (2)].String))); }
    break;

  case 170:
#line 741 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Pidfile, (yyvsp[(2) - (2)].String)));  }
    break;

  case 171:
#line 743 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Logfile, (yyvsp[(2) - (2)].String)));  }
    break;

  case 172:
#line 745 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Automax, (yyvsp[(2) - (2)].Integer)));  }
    break;

  case 173:
#line 748 "ntp_parser.y"
    { append_queue(my_config.logconfig, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 174:
#line 750 "ntp_parser.y"
    { append_queue(my_config.phone, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 175:
#line 752 "ntp_parser.y"
    { enqueue(my_config.setvar, (yyvsp[(2) - (2)].Set_var));  }
    break;

  case 176:
#line 754 "ntp_parser.y"
    { enqueue(my_config.trap, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue)));  }
    break;

  case 177:
#line 756 "ntp_parser.y"
    { append_queue(my_config.ttl, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 178:
#line 758 "ntp_parser.y"
    { enqueue(my_config.qos, create_attr_sval(T_Qos, (yyvsp[(2) - (2)].String))); }
    break;

  case 179:
#line 762 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String))); }
    break;

  case 180:
#line 764 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double)));
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String))); }
    break;

  case 181:
#line 766 "ntp_parser.y"
    { /* Null driftfile,  indicated by null string "\0" */
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;

  case 182:
#line 772 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), DEF); }
    break;

  case 183:
#line 774 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (3)].String), (yyvsp[(3) - (3)].String), 0); }
    break;

  case 184:
#line 779 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 185:
#line 780 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 186:
#line 784 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Port, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 187:
#line 785 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_pval(T_Interface, (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 188:
#line 790 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 189:
#line 791 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 190:
#line 795 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 191:
#line 797 "ntp_parser.y"
    {
                    /* YUCK!! This is needed because '+' and '-' are not special characters
                     * while '=' is.
                     * We really need a better way of defining strings
                     */
                    char prefix = (yyvsp[(1) - (1)].String)[0];
                    char *type = &((yyvsp[(1) - (1)].String)[1]);
                    if (prefix != '+' && prefix != '-') {
                        yyerror("Logconfig prefix is not '+', '-' or '='\n");
                    }
                    else
                        (yyval.Attr_val) = create_attr_sval(prefix, type);
                }
    break;

  case 192:
#line 813 "ntp_parser.y"
    { (yyval.Integer) = '+'; }
    break;

  case 193:
#line 814 "ntp_parser.y"
    { (yyval.Integer) = '-'; }
    break;

  case 194:
#line 815 "ntp_parser.y"
    { (yyval.Integer) = '='; }
    break;

  case 195:
#line 824 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 196:
#line 825 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 197:
#line 829 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_pval((yyvsp[(2) - (2)].String))); }
    break;

  case 198:
#line 830 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_pval((yyvsp[(1) - (1)].String))); }
    break;

  case 199:
#line 834 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 200:
#line 835 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Address_node)); }
    break;

  case 201:
#line 840 "ntp_parser.y"
    {
                    if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
                        yyerror("Integer value is not boolean (0 or 1). Assuming 1");
                        (yyval.Integer) = 1;
                    }
                    else
                        (yyval.Integer) = (yyvsp[(1) - (1)].Integer);
                }
    break;

  case 202:
#line 848 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 203:
#line 849 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 204:
#line 853 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 205:
#line 854 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(1) - (1)].Double); }
    break;

  case 206:
#line 863 "ntp_parser.y"
    {
                 my_config.sim_details = create_sim_node((yyvsp[(3) - (5)].Queue), (yyvsp[(4) - (5)].Queue));

                 /* Reset the old_config_style variable */
                 old_config_style = 1;
             }
    break;

  case 207:
#line 877 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 208:
#line 881 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 209:
#line 882 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 210:
#line 886 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Beep_Delay, (yyvsp[(3) - (3)].Double)); }
    break;

  case 211:
#line 887 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Sim_Duration, (yyvsp[(3) - (3)].Double)); }
    break;

  case 212:
#line 891 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_server)); }
    break;

  case 213:
#line 892 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_server)); }
    break;

  case 214:
#line 897 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Queue)); }
    break;

  case 215:
#line 901 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 216:
#line 905 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 217:
#line 909 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(1) - (1)].Address_node); }
    break;

  case 218:
#line 910 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), T_String); }
    break;

  case 219:
#line 914 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_script)); }
    break;

  case 220:
#line 915 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_script)); }
    break;

  case 221:
#line 920 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Queue)); }
    break;

  case 222:
#line 924 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 223:
#line 925 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 224:
#line 930 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Freq_Offset, (yyvsp[(3) - (3)].Double)); }
    break;

  case 225:
#line 932 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Wander, (yyvsp[(3) - (3)].Double)); }
    break;

  case 226:
#line 934 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Jitter, (yyvsp[(3) - (3)].Double)); }
    break;

  case 227:
#line 936 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Prop_Delay, (yyvsp[(3) - (3)].Double)); }
    break;

  case 228:
#line 938 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Proc_Delay, (yyvsp[(3) - (3)].Double)); }
    break;


/* Line 1267 of yacc.c.  */
#line 3228 "../../ntpd/ntp_parser.c"
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
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
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

  if (yyn == YYFINAL)
    YYACCEPT;

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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
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


#line 942 "ntp_parser.y"


/* KEYWORDS
 * --------
 */

void yyerror (char *msg)
{
    int retval;
    if (input_from_file)
        fprintf(stderr, "%s\n", msg);
    else {
        /* Save the error message in the correct buffer */
        retval = snprintf(remote_config.err_msg + remote_config.err_pos,
                          MAXLINE - remote_config.err_pos,
                          "%s\n", msg);

        /* Increment the value of err_pos */
        if (retval > 0)
            remote_config.err_pos += retval;

        /* Increment the number of errors */
        ++remote_config.no_errors;
    }
}


/* Initial Testing function -- ignore
int main(int argc, char *argv[])
{
    ip_file = FOPEN(argv[1], "r");
    if (!ip_file) {
        fprintf(stderr, "ERROR!! Could not open file: %s\n", argv[1]);
    }
    key_scanner = create_keyword_scanner(keyword_list);
    print_keyword_scanner(key_scanner, 0);
    yyparse();
    return 0;
}
*/


