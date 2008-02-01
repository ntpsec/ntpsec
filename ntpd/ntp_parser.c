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
     T_Floor = 295,
     T_Freq = 296,
     T_Fudge = 297,
     T_Host = 298,
     T_Huffpuff = 299,
     T_IPv4_address = 300,
     T_IPv6_address = 301,
     T_Iburst = 302,
     T_Ident = 303,
     T_Ignore = 304,
     T_Includefile = 305,
     T_Integer = 306,
     T_Interface = 307,
     T_Kernel = 308,
     T_Key = 309,
     T_Keys = 310,
     T_Keysdir = 311,
     T_Kod = 312,
     T_Leap = 313,
     T_Leapfile = 314,
     T_Limited = 315,
     T_Link = 316,
     T_Logconfig = 317,
     T_Logfile = 318,
     T_Loopstats = 319,
     T_Lowpriotrap = 320,
     T_Manycastclient = 321,
     T_Manycastserver = 322,
     T_Mask = 323,
     T_Maxclock = 324,
     T_Maxdist = 325,
     T_Maxhop = 326,
     T_Maxpoll = 327,
     T_Minclock = 328,
     T_Mindist = 329,
     T_Minimum = 330,
     T_Minpoll = 331,
     T_Minsane = 332,
     T_Mode = 333,
     T_Monitor = 334,
     T_Month = 335,
     T_Multicastclient = 336,
     T_Nolink = 337,
     T_Nomodify = 338,
     T_None = 339,
     T_Nopeer = 340,
     T_Noquery = 341,
     T_Noselect = 342,
     T_Noserve = 343,
     T_Notrap = 344,
     T_Notrust = 345,
     T_Novolley = 346,
     T_Ntp = 347,
     T_Ntpport = 348,
     T_Orphan = 349,
     T_Panic = 350,
     T_Peer = 351,
     T_Peerstats = 352,
     T_Phone = 353,
     T_Pid = 354,
     T_Pidfile = 355,
     T_Pool = 356,
     T_Port = 357,
     T_Pps = 358,
     T_Preempt = 359,
     T_Prefer = 360,
     T_Pw = 361,
     T_Qos = 362,
     T_RandFile = 363,
     T_Rawstats = 364,
     T_Refid = 365,
     T_Requestkey = 366,
     T_Restrict = 367,
     T_Revoke = 368,
     T_Server = 369,
     T_Setvar = 370,
     T_Sign = 371,
     T_Statistics = 372,
     T_Stats = 373,
     T_Statsdir = 374,
     T_Step = 375,
     T_Stepout = 376,
     T_Stratum = 377,
     T_String = 378,
     T_Sysstats = 379,
     T_Tick = 380,
     T_Time1 = 381,
     T_Time2 = 382,
     T_Tinker = 383,
     T_Tos = 384,
     T_Trap = 385,
     T_True = 386,
     T_Trustedkey = 387,
     T_Ttl = 388,
     T_Type = 389,
     T_Version = 390,
     T_Week = 391,
     T_Year = 392,
     T_Flag = 393,
     T_Void = 394,
     T_EOC = 395,
     T_Simulate = 396,
     T_Beep_Delay = 397,
     T_Sim_Duration = 398,
     T_Server_Offset = 399,
     T_Duration = 400,
     T_Freq_Offset = 401,
     T_Wander = 402,
     T_Jitter = 403,
     T_Prop_Delay = 404,
     T_Proc_Delay = 405
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
#define T_Floor 295
#define T_Freq 296
#define T_Fudge 297
#define T_Host 298
#define T_Huffpuff 299
#define T_IPv4_address 300
#define T_IPv6_address 301
#define T_Iburst 302
#define T_Ident 303
#define T_Ignore 304
#define T_Includefile 305
#define T_Integer 306
#define T_Interface 307
#define T_Kernel 308
#define T_Key 309
#define T_Keys 310
#define T_Keysdir 311
#define T_Kod 312
#define T_Leap 313
#define T_Leapfile 314
#define T_Limited 315
#define T_Link 316
#define T_Logconfig 317
#define T_Logfile 318
#define T_Loopstats 319
#define T_Lowpriotrap 320
#define T_Manycastclient 321
#define T_Manycastserver 322
#define T_Mask 323
#define T_Maxclock 324
#define T_Maxdist 325
#define T_Maxhop 326
#define T_Maxpoll 327
#define T_Minclock 328
#define T_Mindist 329
#define T_Minimum 330
#define T_Minpoll 331
#define T_Minsane 332
#define T_Mode 333
#define T_Monitor 334
#define T_Month 335
#define T_Multicastclient 336
#define T_Nolink 337
#define T_Nomodify 338
#define T_None 339
#define T_Nopeer 340
#define T_Noquery 341
#define T_Noselect 342
#define T_Noserve 343
#define T_Notrap 344
#define T_Notrust 345
#define T_Novolley 346
#define T_Ntp 347
#define T_Ntpport 348
#define T_Orphan 349
#define T_Panic 350
#define T_Peer 351
#define T_Peerstats 352
#define T_Phone 353
#define T_Pid 354
#define T_Pidfile 355
#define T_Pool 356
#define T_Port 357
#define T_Pps 358
#define T_Preempt 359
#define T_Prefer 360
#define T_Pw 361
#define T_Qos 362
#define T_RandFile 363
#define T_Rawstats 364
#define T_Refid 365
#define T_Requestkey 366
#define T_Restrict 367
#define T_Revoke 368
#define T_Server 369
#define T_Setvar 370
#define T_Sign 371
#define T_Statistics 372
#define T_Stats 373
#define T_Statsdir 374
#define T_Step 375
#define T_Stepout 376
#define T_Stratum 377
#define T_String 378
#define T_Sysstats 379
#define T_Tick 380
#define T_Time1 381
#define T_Time2 382
#define T_Tinker 383
#define T_Tos 384
#define T_Trap 385
#define T_True 386
#define T_Trustedkey 387
#define T_Ttl 388
#define T_Type 389
#define T_Version 390
#define T_Week 391
#define T_Year 392
#define T_Flag 393
#define T_Void 394
#define T_EOC 395
#define T_Simulate 396
#define T_Beep_Delay 397
#define T_Sim_Duration 398
#define T_Server_Offset 399
#define T_Duration 400
#define T_Freq_Offset 401
#define T_Wander 402
#define T_Jitter 403
#define T_Prop_Delay 404
#define T_Proc_Delay 405




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
#line 449 "../../ntpd/ntp_parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 462 "../../ntpd/ntp_parser.c"

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
#define YYFINAL  163
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   464

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  156
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  226
/* YYNRULES -- Number of states.  */
#define YYNSTATES  356

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   405

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   152,     2,   153,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   151,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   154,     2,   155,     2,     2,     2,     2,
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
     145,   146,   147,   148,   149,   150
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
     227,   229,   232,   234,   237,   240,   242,   244,   246,   248,
     250,   252,   254,   256,   258,   260,   262,   265,   269,   273,
     279,   280,   283,   285,   287,   289,   291,   293,   295,   297,
     299,   301,   303,   305,   307,   310,   312,   315,   318,   321,
     325,   328,   330,   333,   336,   339,   342,   345,   348,   351,
     354,   357,   360,   363,   365,   367,   369,   371,   373,   375,
     377,   379,   382,   385,   387,   390,   393,   396,   399,   402,
     405,   408,   412,   414,   417,   420,   423,   426,   429,   432,
     435,   438,   441,   444,   447,   451,   454,   457,   459,   462,
     463,   468,   472,   475,   477,   480,   483,   486,   488,   491,
     493,   495,   497,   499,   502,   504,   507,   509,   512,   514,
     516,   518,   520,   522,   524,   530,   532,   536,   539,   543,
     547,   550,   552,   558,   563,   567,   569,   571,   574,   576,
     583,   587,   590,   594,   598,   602,   606
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     157,     0,    -1,   158,    -1,   158,   159,   140,    -1,   159,
     140,    -1,     1,   140,    -1,    -1,   160,    -1,   166,    -1,
     167,    -1,   174,    -1,   180,    -1,   171,    -1,   185,    -1,
     188,    -1,   191,    -1,   194,    -1,   207,    -1,   161,   162,
     164,    -1,   161,   162,    -1,   114,    -1,   101,    -1,    96,
      -1,    11,    -1,    66,    -1,   163,    -1,   123,    -1,    51,
     123,    -1,    45,    -1,    46,    -1,   164,   165,    -1,   165,
      -1,     6,    -1,    14,    -1,    47,    -1,    54,    51,    -1,
      76,    51,    -1,    72,    51,    -1,    87,    -1,   104,    -1,
     105,    -1,   131,    -1,   133,    51,    -1,    78,    51,    -1,
     135,    51,    -1,    12,    -1,    12,    91,    -1,    67,   204,
      -1,    81,   204,    -1,     6,    51,    -1,    20,    51,    -1,
      21,   168,    -1,    55,   123,    -1,    56,   123,    -1,   111,
      51,    -1,   132,   202,    -1,   169,    -1,    -1,   169,   170,
      -1,   170,    -1,    43,   123,    -1,    48,   123,    -1,   106,
     123,    -1,   108,   123,    -1,   113,    51,    -1,   116,   123,
      -1,   129,   172,    -1,   172,   173,    -1,   173,    -1,    17,
      51,    -1,    40,    51,    -1,    19,   205,    -1,    94,    51,
      -1,    74,   206,    -1,    70,   206,    -1,    73,   206,    -1,
      69,   206,    -1,    77,    51,    -1,    10,    51,    -1,    71,
      51,    -1,   117,   175,    -1,   119,   123,    -1,    35,   176,
     177,    -1,   175,   176,    -1,   176,    -1,    18,    -1,    22,
      -1,    64,    -1,    97,    -1,   109,    -1,   124,    -1,   177,
     178,    -1,   178,    -1,    34,   123,    -1,   134,   179,    -1,
      61,    -1,    82,    -1,    31,    -1,    25,    -1,    84,    -1,
      99,    -1,    23,    -1,   136,    -1,    80,    -1,   137,    -1,
       3,    -1,    26,   183,    -1,   112,   162,   181,    -1,   112,
      24,   181,    -1,   112,   163,    68,   163,   181,    -1,    -1,
     181,   182,    -1,    49,    -1,    57,    -1,    60,    -1,    65,
      -1,    83,    -1,    85,    -1,    86,    -1,    88,    -1,    89,
      -1,    90,    -1,    93,    -1,   135,    -1,   183,   184,    -1,
     184,    -1,     8,    51,    -1,    75,    51,    -1,    79,    51,
      -1,    42,   162,   186,    -1,   186,   187,    -1,   187,    -1,
     126,   206,    -1,   127,   206,    -1,   122,    51,    -1,   110,
     123,    -1,    36,   205,    -1,    37,   205,    -1,    38,   205,
      -1,    39,   205,    -1,    31,   189,    -1,    25,   189,    -1,
     189,   190,    -1,   190,    -1,     5,    -1,     9,    -1,    15,
      -1,    53,    -1,    79,    -1,    92,    -1,   118,    -1,   128,
     192,    -1,   192,   193,    -1,   193,    -1,     4,   206,    -1,
      27,   206,    -1,    41,   206,    -1,    44,   206,    -1,    95,
     206,    -1,   120,   206,    -1,   121,   206,    -1,    50,   123,
     159,    -1,    32,    -1,    13,   206,    -1,    16,    51,    -1,
     125,   206,    -1,    29,   195,    -1,    59,   123,    -1,   100,
     123,    -1,    63,   123,    -1,     7,    51,    -1,    62,   199,
      -1,    98,   203,    -1,   115,   196,    -1,   130,   163,   197,
      -1,   133,   202,    -1,   107,   123,    -1,   123,    -1,   123,
      28,    -1,    -1,   123,   151,   123,    24,    -1,   123,   151,
     123,    -1,   197,   198,    -1,   198,    -1,   102,    51,    -1,
      52,   163,    -1,   199,   200,    -1,   200,    -1,   201,   123,
      -1,   123,    -1,   152,    -1,   153,    -1,   151,    -1,   202,
      51,    -1,    51,    -1,   203,   123,    -1,   123,    -1,   204,
     162,    -1,   162,    -1,    51,    -1,   131,    -1,    33,    -1,
      51,    -1,    28,    -1,   208,   154,   209,   211,   155,    -1,
     141,    -1,   209,   210,   140,    -1,   210,   140,    -1,   142,
     151,   206,    -1,   143,   151,   206,    -1,   211,   212,    -1,
     212,    -1,   214,   154,   213,   216,   155,    -1,   144,   151,
     206,   140,    -1,   114,   151,   215,    -1,   163,    -1,   123,
      -1,   216,   217,    -1,   217,    -1,   145,   151,   206,   154,
     218,   155,    -1,   218,   219,   140,    -1,   219,   140,    -1,
     146,   151,   206,    -1,   147,   151,   206,    -1,   148,   151,
     206,    -1,   149,   151,   206,    -1,   150,   151,   206,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   279,   279,   283,   284,   289,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,   319,   325,
     334,   335,   336,   337,   338,   342,   343,   344,   360,   361,
     365,   366,   371,   372,   373,   374,   375,   376,   377,   378,
     379,   380,   381,   382,   383,   393,   395,   397,   399,   410,
     412,   414,   421,   423,   425,   427,   432,   434,   438,   439,
     443,   445,   447,   449,   451,   453,   463,   468,   469,   473,
     475,   477,   479,   481,   483,   485,   487,   489,   491,   493,
     503,   505,   507,   515,   516,   520,   522,   524,   526,   528,
     530,   535,   536,   540,   541,   542,   543,   544,   545,   549,
     550,   551,   552,   553,   554,   555,   564,   566,   571,   576,
     584,   585,   589,   590,   591,   592,   593,   594,   595,   596,
     597,   598,   599,   600,   604,   605,   609,   610,   611,   619,
     624,   625,   629,   631,   633,   635,   637,   639,   641,   643,
     652,   654,   659,   660,   664,   665,   666,   667,   668,   669,
     671,   679,   683,   684,   688,   689,   690,   691,   692,   693,
     694,   702,   718,   724,   726,   728,   730,   732,   735,   737,
     739,   742,   744,   746,   748,   750,   752,   756,   758,   761,
     766,   768,   774,   775,   779,   780,   785,   786,   790,   791,
     808,   809,   810,   819,   820,   824,   825,   829,   830,   834,
     843,   844,   848,   849,   857,   872,   876,   877,   881,   882,
     886,   887,   891,   896,   900,   904,   905,   909,   910,   914,
     919,   920,   924,   926,   928,   930,   932
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
  "T_Filegen", "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4", "T_Floor",
  "T_Freq", "T_Fudge", "T_Host", "T_Huffpuff", "T_IPv4_address",
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
  "T_Preempt", "T_Prefer", "T_Pw", "T_Qos", "T_RandFile", "T_Rawstats",
  "T_Refid", "T_Requestkey", "T_Restrict", "T_Revoke", "T_Server",
  "T_Setvar", "T_Sign", "T_Statistics", "T_Stats", "T_Statsdir", "T_Step",
  "T_Stepout", "T_Stratum", "T_String", "T_Sysstats", "T_Tick", "T_Time1",
  "T_Time2", "T_Tinker", "T_Tos", "T_Trap", "T_True", "T_Trustedkey",
  "T_Ttl", "T_Type", "T_Version", "T_Week", "T_Year", "T_Flag", "T_Void",
  "T_EOC", "T_Simulate", "T_Beep_Delay", "T_Sim_Duration",
  "T_Server_Offset", "T_Duration", "T_Freq_Offset", "T_Wander", "T_Jitter",
  "T_Prop_Delay", "T_Proc_Delay", "'='", "'+'", "'-'", "'{'", "'}'",
  "$accept", "configuration", "command_list", "command", "server_command",
  "client_type", "address", "ip_address", "option_list", "option",
  "other_mode_command", "authentication_command", "crypto_command_line",
  "crypto_command_list", "crypto_command", "orphan_mode_command",
  "tos_option_list", "tos_option", "monitoring_command", "stats_list",
  "stat", "filegen_option_list", "filegen_option", "filegen_type",
  "access_control_command", "ac_flag_list", "access_control_flag",
  "discard_option_list", "discard_option", "fudge_command",
  "fudge_factor_list", "fudge_factor", "system_option_command",
  "system_option_list", "system_option", "tinker_command",
  "tinker_option_list", "tinker_option", "miscellaneous_command",
  "drift_parm", "variable_assign", "trap_option_list", "trap_option",
  "log_config_list", "log_config_command", "log_config_prefix",
  "integer_list", "string_list", "address_list", "boolean", "number",
  "simulate_command", "sim_conf_start", "sim_init_statement_list",
  "sim_init_statement", "sim_server_list", "sim_server",
  "sim_server_offset", "sim_server_name", "sim_address", "sim_act_list",
  "sim_act", "sim_act_stmt_list", "sim_act_stmt", 0
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
     405,    61,    43,    45,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   156,   157,   158,   158,   158,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   160,   160,
     161,   161,   161,   161,   161,   162,   162,   162,   163,   163,
     164,   164,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   166,   166,   166,   166,   167,
     167,   167,   167,   167,   167,   167,   168,   168,   169,   169,
     170,   170,   170,   170,   170,   170,   171,   172,   172,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     174,   174,   174,   175,   175,   176,   176,   176,   176,   176,
     176,   177,   177,   178,   178,   178,   178,   178,   178,   179,
     179,   179,   179,   179,   179,   179,   180,   180,   180,   180,
     181,   181,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   183,   183,   184,   184,   184,   185,
     186,   186,   187,   187,   187,   187,   187,   187,   187,   187,
     188,   188,   189,   189,   190,   190,   190,   190,   190,   190,
     190,   191,   192,   192,   193,   193,   193,   193,   193,   193,
     193,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   195,   195,   195,
     196,   196,   197,   197,   198,   198,   199,   199,   200,   200,
     201,   201,   201,   202,   202,   203,   203,   204,   204,   205,
     205,   205,   206,   206,   207,   208,   209,   209,   210,   210,
     211,   211,   212,   213,   214,   215,   215,   216,   216,   217,
     218,   218,   219,   219,   219,   219,   219
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
       1,     2,     1,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     3,     3,     5,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     2,     2,     2,     3,
       2,     1,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     1,     2,     2,     2,     2,     2,     2,
       2,     3,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     2,     2,     1,     2,     0,
       4,     3,     2,     1,     2,     2,     2,     1,     2,     1,
       1,     1,     1,     2,     1,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     5,     1,     3,     2,     3,     3,
       2,     1,     5,     4,     3,     1,     1,     2,     1,     6,
       3,     2,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,    23,    45,     0,     0,     0,    57,
       0,     0,   179,     0,   162,     0,     0,     0,     0,     0,
       0,     0,     0,    24,     0,     0,    22,     0,     0,    21,
       0,     0,     0,    20,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   205,     0,     2,     0,     7,     0,     8,
       9,    12,    10,    11,    13,    14,    15,    16,    17,     0,
       5,    49,   170,    46,   203,   202,   163,   164,    50,     0,
       0,     0,     0,     0,     0,    51,    56,    59,   144,   145,
     146,   147,   148,   149,   150,   141,   143,     0,     0,     0,
     106,   125,   177,   166,   140,    85,    86,    87,    88,    89,
      90,     0,    28,    29,     0,    26,     0,    25,     6,    52,
      53,   167,   189,   192,   190,   191,   171,   187,     0,   169,
     198,    47,    48,   196,   172,   168,   176,    54,   110,   110,
      25,     0,   173,    80,    84,    81,   165,     0,     0,     0,
       0,     0,     0,     0,   151,   153,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    66,    68,     0,
     194,    55,   175,     1,     0,     4,    19,     0,    60,    61,
      62,    63,    64,    65,    58,   142,   126,   127,   128,   124,
     178,    98,    97,     0,    95,    96,     0,    82,    92,    27,
       0,     0,     0,     0,     0,     0,     0,     0,   129,   131,
     161,   186,   188,   197,   195,   108,   107,     0,     0,    83,
     154,   155,   156,   157,   158,   159,   160,   152,    78,    69,
     201,   199,   200,    71,    70,    76,    74,    79,    75,    73,
      77,    72,    67,     0,     0,   174,   183,   193,     3,    32,
      33,    34,     0,     0,     0,     0,    38,    39,    40,    41,
       0,     0,    18,    31,     0,     0,     0,     0,    93,   105,
     101,   103,    99,   100,   102,   104,    94,    91,   136,   137,
     138,   139,   135,   134,   132,   133,   130,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   111,
     110,   181,   185,   184,   182,    35,    37,    36,    43,    42,
      44,    30,     0,     0,     0,     0,     0,   211,     0,   207,
     109,   180,   208,   209,     0,   206,   204,   210,     0,   216,
     215,   214,     0,     0,     0,     0,     0,   218,     0,     0,
     212,   217,   213,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   219,     0,   221,
     222,   223,   224,   225,   226,   220
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    44,    45,    46,    47,    48,   120,   107,   252,   253,
      49,    50,    75,    76,    77,    51,   157,   158,    52,   133,
     101,   187,   188,   266,    53,   205,   289,    90,    91,    54,
     198,   199,    55,    85,    86,    56,   144,   145,    57,    93,
     132,   235,   236,   116,   117,   118,   161,   124,   121,   223,
      66,    58,    59,   256,   257,   306,   307,   323,   308,   321,
     326,   327,   340,   341
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -138
static const yytype_int16 yypact[] =
{
      11,  -137,   -44,   -26,  -138,   -80,     5,   -13,   -10,   132,
       1,     7,   -68,     1,  -138,   -14,   -32,   -52,   -42,   -33,
     -29,   -64,   -24,  -138,   -32,   -32,  -138,   -15,    -8,  -138,
      -2,    82,     6,  -138,    12,   -14,    15,     5,    93,   370,
      86,    94,    94,  -138,    79,   161,    13,  -138,   -32,  -138,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,    -7,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,    43,
      46,    47,    55,   111,    61,  -138,   132,  -138,  -138,  -138,
    -138,  -138,  -138,  -138,  -138,     1,  -138,   134,   138,   140,
       7,  -138,   155,  -138,     1,  -138,  -138,  -138,  -138,  -138,
    -138,    14,  -138,  -138,    71,  -138,   336,  -138,   304,  -138,
    -138,  -138,  -138,  -138,  -138,  -138,   -64,  -138,    72,  -138,
    -138,   -32,   -32,  -138,    74,  -138,  -138,  -138,  -138,  -138,
     133,    49,  -138,   -14,  -138,  -138,  -138,     5,     5,     5,
       5,     5,     5,     5,    93,  -138,   153,   154,    -4,   167,
       5,     5,   170,     5,     5,   171,   174,   370,  -138,   -18,
    -138,   175,   175,  -138,    95,  -138,   165,    22,  -138,  -138,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,
    -138,  -138,  -138,   110,  -138,  -138,    62,    14,  -138,  -138,
      -4,    -4,    -4,    -4,   113,   183,     5,     5,   336,  -138,
    -138,  -138,  -138,  -138,  -138,   293,   293,    86,   116,  -138,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,
    -138,  -138,  -138,    86,   193,   -18,  -138,  -138,  -138,  -138,
    -138,  -138,   195,   196,   198,   199,  -138,  -138,  -138,  -138,
     200,   202,   165,  -138,   103,   104,   -79,   118,  -138,  -138,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,
    -138,   232,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,
    -138,  -138,     5,     5,   109,   123,   -86,  -138,   117,  -138,
     293,  -138,  -138,  -138,   -25,  -138,  -138,  -138,   120,  -138,
    -138,  -138,   126,   122,     5,   128,   -83,  -138,   141,     5,
    -138,  -138,  -138,   130,     8,   131,   137,   144,   146,   148,
      60,   145,     5,     5,     5,     5,     5,  -138,   163,  -138,
    -138,  -138,  -138,  -138,  -138,  -138
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -138,  -138,  -138,   -40,  -138,  -138,    28,   -31,  -138,    52,
    -138,  -138,  -138,  -138,   236,  -138,  -138,   156,  -138,  -138,
      -9,  -138,    87,  -138,  -138,  -127,  -138,  -138,   224,  -138,
    -138,   121,  -138,   305,   -36,  -138,  -138,   177,  -138,  -138,
    -138,  -138,    88,  -138,   206,  -138,   284,  -138,   302,    39,
     -37,  -138,  -138,  -138,    75,  -138,    26,  -138,  -138,  -138,
    -138,     2,  -138,    -6
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     136,   130,   206,    60,    95,   164,    78,    61,    96,   159,
      79,    63,     1,   102,   103,    87,    80,     2,     3,   104,
     102,   103,     4,     5,     6,    62,   134,     7,   304,   220,
     128,     8,     9,    64,   233,   304,    10,    11,    67,   181,
      12,    68,    13,    14,   106,   182,    15,   221,   183,   175,
      97,   102,   103,    16,    81,    92,    65,   104,   175,   112,
     129,    17,   325,   254,   255,   259,    18,    19,   200,   316,
      20,   108,   330,    21,    22,   184,   166,    23,    24,   163,
      82,   109,    88,    98,   234,   260,    89,   113,   114,   115,
     110,   105,    25,    83,   111,    99,   185,   137,   319,   119,
     210,   211,   212,   213,   214,   215,   216,    26,   123,    27,
     100,    28,    29,   225,   226,   125,   228,   229,    30,    84,
     138,   126,    31,    32,   209,    33,    34,   222,    35,   105,
      36,   102,   103,   127,   139,   131,    37,   140,   135,    38,
      39,    40,   261,    41,    42,   160,   262,   167,   186,   203,
     203,    -6,    43,   165,   335,   336,   337,   338,   339,   274,
     275,   263,   172,   310,   254,   255,   168,     2,     3,   169,
     170,   239,     4,     5,     6,    69,   290,     7,   171,   240,
      70,     8,     9,   180,   173,   176,    10,    11,   141,   177,
      12,   178,    13,    14,   189,   202,    15,   204,   264,   265,
     208,   207,   292,    16,   218,   219,   335,   336,   337,   338,
     339,    17,   241,   142,   143,   347,    18,    19,   224,   242,
      20,   227,   230,    21,    22,   231,   237,    23,    24,   268,
     269,   270,   271,   258,   273,   238,   272,   243,    71,   291,
      72,   244,    25,   245,   293,    73,   295,   296,    74,   297,
     298,   299,   246,   300,   302,   303,   311,    26,   309,    27,
     314,    28,    29,   315,   322,   312,   313,   325,    30,   247,
     248,   318,    31,    32,   267,    33,    34,   324,    35,   329,
      36,   332,   342,   320,   334,   349,    37,   328,   343,    38,
      39,    40,   333,    41,    42,   344,   249,   345,   250,   346,
     251,    -6,    43,   355,   301,   350,   351,   352,   353,   354,
       2,     3,   174,   232,   179,     4,     5,     6,    94,   276,
       7,   217,   201,   294,     8,     9,   162,   122,   331,    10,
      11,   305,   317,    12,   348,    13,    14,     0,     0,    15,
       0,     0,   277,     0,     0,     0,    16,     0,     0,     0,
     278,     0,     0,   279,    17,     0,     0,     0,   280,    18,
      19,     0,     0,    20,     0,     0,    21,    22,     0,     0,
      23,    24,   190,   191,   192,   193,   281,     0,   282,   283,
     146,   284,   285,   286,     0,    25,   287,   147,     0,   148,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      26,     0,    27,     0,    28,    29,     0,     0,     0,     0,
     149,    30,     0,     0,     0,    31,    32,     0,    33,    34,
       0,    35,     0,    36,     0,     0,     0,     0,   288,    37,
       0,     0,    38,    39,    40,     0,    41,    42,     0,   150,
     151,   152,     0,   153,   154,    43,   194,   155,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   195,     0,
       0,     0,   196,   197,   156
};

static const yytype_int16 yycheck[] =
{
      37,    32,   129,   140,    18,    45,     5,    51,    22,    40,
       9,    91,     1,    45,    46,     8,    15,     6,     7,    51,
      45,    46,    11,    12,    13,    51,    35,    16,   114,    33,
      24,    20,    21,    28,    52,   114,    25,    26,    51,    25,
      29,    51,    31,    32,    16,    31,    35,    51,    34,    85,
      64,    45,    46,    42,    53,   123,    51,    51,    94,   123,
      32,    50,   145,   142,   143,     3,    55,    56,   108,   155,
      59,   123,   155,    62,    63,    61,    48,    66,    67,     0,
      79,   123,    75,    97,   102,    23,    79,   151,   152,   153,
     123,   123,    81,    92,   123,   109,    82,     4,   123,   123,
     137,   138,   139,   140,   141,   142,   143,    96,   123,    98,
     124,   100,   101,   150,   151,   123,   153,   154,   107,   118,
      27,   123,   111,   112,   133,   114,   115,   131,   117,   123,
     119,    45,    46,    51,    41,   123,   125,    44,   123,   128,
     129,   130,    80,   132,   133,    51,    84,   154,   134,   121,
     122,   140,   141,   140,   146,   147,   148,   149,   150,   196,
     197,    99,    51,   290,   142,   143,   123,     6,     7,   123,
     123,     6,    11,    12,    13,    43,   207,    16,   123,    14,
      48,    20,    21,    28,   123,    51,    25,    26,    95,    51,
      29,    51,    31,    32,   123,   123,    35,   123,   136,   137,
     151,    68,   233,    42,    51,    51,   146,   147,   148,   149,
     150,    50,    47,   120,   121,   155,    55,    56,    51,    54,
      59,    51,    51,    62,    63,    51,    51,    66,    67,   190,
     191,   192,   193,   123,    51,   140,   123,    72,   106,   123,
     108,    76,    81,    78,    51,   113,    51,    51,   116,    51,
      51,    51,    87,    51,   151,   151,    24,    96,   140,    98,
     151,   100,   101,   140,   144,   302,   303,   145,   107,   104,
     105,   154,   111,   112,   187,   114,   115,   151,   117,   151,
     119,   140,   151,   314,   154,   140,   125,   324,   151,   128,
     129,   130,   329,   132,   133,   151,   131,   151,   133,   151,
     135,   140,   141,   140,   252,   342,   343,   344,   345,   346,
       6,     7,    76,   157,    90,    11,    12,    13,    13,   198,
      16,   144,   116,   235,    20,    21,    42,    25,   326,    25,
      26,   256,   306,    29,   340,    31,    32,    -1,    -1,    35,
      -1,    -1,    49,    -1,    -1,    -1,    42,    -1,    -1,    -1,
      57,    -1,    -1,    60,    50,    -1,    -1,    -1,    65,    55,
      56,    -1,    -1,    59,    -1,    -1,    62,    63,    -1,    -1,
      66,    67,    36,    37,    38,    39,    83,    -1,    85,    86,
      10,    88,    89,    90,    -1,    81,    93,    17,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      96,    -1,    98,    -1,   100,   101,    -1,    -1,    -1,    -1,
      40,   107,    -1,    -1,    -1,   111,   112,    -1,   114,   115,
      -1,   117,    -1,   119,    -1,    -1,    -1,    -1,   135,   125,
      -1,    -1,   128,   129,   130,    -1,   132,   133,    -1,    69,
      70,    71,    -1,    73,    74,   141,   110,    77,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,    -1,
      -1,    -1,   126,   127,    94
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     6,     7,    11,    12,    13,    16,    20,    21,
      25,    26,    29,    31,    32,    35,    42,    50,    55,    56,
      59,    62,    63,    66,    67,    81,    96,    98,   100,   101,
     107,   111,   112,   114,   115,   117,   119,   125,   128,   129,
     130,   132,   133,   141,   157,   158,   159,   160,   161,   166,
     167,   171,   174,   180,   185,   188,   191,   194,   207,   208,
     140,    51,    51,    91,    28,    51,   206,    51,    51,    43,
      48,   106,   108,   113,   116,   168,   169,   170,     5,     9,
      15,    53,    79,    92,   118,   189,   190,     8,    75,    79,
     183,   184,   123,   195,   189,    18,    22,    64,    97,   109,
     124,   176,    45,    46,    51,   123,   162,   163,   123,   123,
     123,   123,   123,   151,   152,   153,   199,   200,   201,   123,
     162,   204,   204,   123,   203,   123,   123,    51,    24,   162,
     163,   123,   196,   175,   176,   123,   206,     4,    27,    41,
      44,    95,   120,   121,   192,   193,    10,    17,    19,    40,
      69,    70,    71,    73,    74,    77,    94,   172,   173,   163,
      51,   202,   202,     0,   159,   140,   162,   154,   123,   123,
     123,   123,    51,   123,   170,   190,    51,    51,    51,   184,
      28,    25,    31,    34,    61,    82,   134,   177,   178,   123,
      36,    37,    38,    39,   110,   122,   126,   127,   186,   187,
     159,   200,   123,   162,   123,   181,   181,    68,   151,   176,
     206,   206,   206,   206,   206,   206,   206,   193,    51,    51,
      33,    51,   131,   205,    51,   206,   206,    51,   206,   206,
      51,    51,   173,    52,   102,   197,   198,    51,   140,     6,
      14,    47,    54,    72,    76,    78,    87,   104,   105,   131,
     133,   135,   164,   165,   142,   143,   209,   210,   123,     3,
      23,    80,    84,    99,   136,   137,   179,   178,   205,   205,
     205,   205,   123,    51,   206,   206,   187,    49,    57,    60,
      65,    83,    85,    86,    88,    89,    90,    93,   135,   182,
     163,   123,   163,    51,   198,    51,    51,    51,    51,    51,
      51,   165,   151,   151,   114,   210,   211,   212,   214,   140,
     181,    24,   206,   206,   151,   140,   155,   212,   154,   123,
     163,   215,   144,   213,   151,   145,   216,   217,   206,   151,
     155,   217,   140,   206,   154,   146,   147,   148,   149,   150,
     218,   219,   151,   151,   151,   151,   151,   155,   219,   140,
     206,   206,   206,   206,   206,   140
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
#line 290 "ntp_parser.y"
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
#line 320 "ntp_parser.y"
    {
                        struct peer_node *my_node =  create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue));
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;

  case 19:
#line 326 "ntp_parser.y"
    {
                        struct peer_node *my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;

  case 20:
#line 334 "ntp_parser.y"
    { (yyval.Integer) = T_Server; }
    break;

  case 21:
#line 335 "ntp_parser.y"
    { (yyval.Integer) = T_Pool; }
    break;

  case 22:
#line 336 "ntp_parser.y"
    { (yyval.Integer) = T_Peer; }
    break;

  case 23:
#line 337 "ntp_parser.y"
    { (yyval.Integer) = T_Broadcast; }
    break;

  case 24:
#line 338 "ntp_parser.y"
    { (yyval.Integer) = T_Manycastclient; }
    break;

  case 25:
#line 342 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(1) - (1)].Address_node); }
    break;

  case 26:
#line 343 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), default_ai_family); }
    break;

  case 27:
#line 345 "ntp_parser.y"
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
#line 360 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_INET); }
    break;

  case 29:
#line 361 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_INET6); }
    break;

  case 30:
#line 365 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 31:
#line 366 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 32:
#line 371 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_SKEY | FLAG_AUTHENABLE); }
    break;

  case 33:
#line 372 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_BURST); }
    break;

  case 34:
#line 373 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_IBURST); }
    break;

  case 35:
#line 374 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Key, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 36:
#line 375 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Minpoll, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 37:
#line 376 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Maxpoll, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 38:
#line 377 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_NOSELECT);}
    break;

  case 39:
#line 378 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREEMPT); }
    break;

  case 40:
#line 379 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREFER); }
    break;

  case 41:
#line 380 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_TRUE); }
    break;

  case 42:
#line 381 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Ttl, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 43:
#line 382 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Mode, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 44:
#line 383 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Version, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:
#line 394 "ntp_parser.y"
    { my_config.broadcastclient = SIMPLE; }
    break;

  case 46:
#line 396 "ntp_parser.y"
    { my_config.broadcastclient = NOVOLLEY;  }
    break;

  case 47:
#line 398 "ntp_parser.y"
    { append_queue(my_config.manycastserver, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 48:
#line 400 "ntp_parser.y"
    { append_queue(my_config.multicastclient, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 49:
#line 411 "ntp_parser.y"
    { my_config.auth.autokey = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 50:
#line 413 "ntp_parser.y"
    { my_config.auth.control_key = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 51:
#line 415 "ntp_parser.y"
    { if (my_config.auth.crypto_cmd_list != NULL)
					append_queue(my_config.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Queue));
				else
					my_config.auth.crypto_cmd_list = (yyvsp[(2) - (2)].Queue);
			cryptosw++;
			}
    break;

  case 52:
#line 422 "ntp_parser.y"
    { my_config.auth.keys = (yyvsp[(2) - (2)].String);  }
    break;

  case 53:
#line 424 "ntp_parser.y"
    { my_config.auth.keysdir = (yyvsp[(2) - (2)].String);  }
    break;

  case 54:
#line 426 "ntp_parser.y"
    { my_config.auth.requested_key = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 55:
#line 428 "ntp_parser.y"
    { my_config.auth.trusted_key_list = (yyvsp[(2) - (2)].Queue);  }
    break;

  case 57:
#line 434 "ntp_parser.y"
    { (yyval.Queue) = NULL; }
    break;

  case 58:
#line 438 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 59:
#line 439 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 60:
#line 444 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PRIV, (yyvsp[(2) - (2)].String)); }
    break;

  case 61:
#line 446 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_IDENT, (yyvsp[(2) - (2)].String)); }
    break;

  case 62:
#line 448 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PW, (yyvsp[(2) - (2)].String)); }
    break;

  case 63:
#line 450 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_RAND, (yyvsp[(2) - (2)].String)); }
    break;

  case 64:
#line 452 "ntp_parser.y"
    { my_config.auth.revoke = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 65:
#line 454 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_SIGN, (yyvsp[(2) - (2)].String)); }
    break;

  case 66:
#line 464 "ntp_parser.y"
    { append_queue(my_config.orphan_cmds,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 67:
#line 468 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 68:
#line 469 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 69:
#line 474 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_CEILING, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 70:
#line 476 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_FLOOR, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 71:
#line 478 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_COHORT, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 72:
#line 480 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_ORPHAN, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 73:
#line 482 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINDISP, (yyvsp[(2) - (2)].Double)); }
    break;

  case 74:
#line 484 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXDIST, (yyvsp[(2) - (2)].Double)); }
    break;

  case 75:
#line 486 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINCLOCK, (yyvsp[(2) - (2)].Double)); }
    break;

  case 76:
#line 488 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXCLOCK, (yyvsp[(2) - (2)].Double)); }
    break;

  case 77:
#line 490 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINSANE, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 78:
#line 492 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_BEACON, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 79:
#line 494 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXHOP, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 80:
#line 504 "ntp_parser.y"
    { append_queue(my_config.stats_list, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 81:
#line 506 "ntp_parser.y"
    { my_config.stats_dir = (yyvsp[(2) - (2)].String);  }
    break;

  case 82:
#line 508 "ntp_parser.y"
    {
                        enqueue(my_config.filegen_opts,
                                create_filegen_node((yyvsp[(2) - (3)].VoidPtr), (yyvsp[(3) - (3)].Queue)));
                    }
    break;

  case 83:
#line 515 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); }
    break;

  case 84:
#line 516 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].VoidPtr)); }
    break;

  case 85:
#line 521 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("clockstats"); }
    break;

  case 86:
#line 523 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("cryptostats"); }
    break;

  case 87:
#line 525 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("loopstats"); }
    break;

  case 88:
#line 527 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("peerstats"); }
    break;

  case 89:
#line 529 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("rawstats"); }
    break;

  case 90:
#line 531 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("sysstats"); }
    break;

  case 91:
#line 535 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 92:
#line 536 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 93:
#line 540 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(T_File, (yyvsp[(2) - (2)].String)); }
    break;

  case 94:
#line 541 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Type, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 95:
#line 542 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Link); }
    break;

  case 96:
#line 543 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Nolink); }
    break;

  case 97:
#line 544 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Enable); }
    break;

  case 98:
#line 545 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Disable); }
    break;

  case 99:
#line 549 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_NONE; }
    break;

  case 100:
#line 550 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_PID; }
    break;

  case 101:
#line 551 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_DAY; }
    break;

  case 102:
#line 552 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_WEEK; }
    break;

  case 103:
#line 553 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_MONTH; }
    break;

  case 104:
#line 554 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_YEAR; }
    break;

  case 105:
#line 555 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_AGE; }
    break;

  case 106:
#line 565 "ntp_parser.y"
    {   append_queue(my_config.discard_opts, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 107:
#line 567 "ntp_parser.y"
    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
                    }
    break;

  case 108:
#line 572 "ntp_parser.y"
    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
                    }
    break;

  case 109:
#line 577 "ntp_parser.y"
    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Queue), ip_file->line_no));
                    }
    break;

  case 110:
#line 584 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 111:
#line 585 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); }
    break;

  case 112:
#line 589 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_IGNORE); }
    break;

  case 113:
#line 590 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_KOD); }
    break;

  case 114:
#line 591 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LIMITED); }
    break;

  case 115:
#line 592 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LPTRAP); }
    break;

  case 116:
#line 593 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOMODIFY); }
    break;

  case 117:
#line 594 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOPEER); }
    break;

  case 118:
#line 595 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOQUERY); }
    break;

  case 119:
#line 596 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTSERVE); }
    break;

  case 120:
#line 597 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOTRAP); }
    break;

  case 121:
#line 598 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTTRUST); }
    break;

  case 122:
#line 599 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RESM_NTPONLY); }
    break;

  case 123:
#line 600 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_VERSION); }
    break;

  case 124:
#line 604 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 125:
#line 605 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 126:
#line 609 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Average, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 127:
#line 610 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Minimum, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 128:
#line 611 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Monitor, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 129:
#line 620 "ntp_parser.y"
    { enqueue(my_config.fudge, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue)));  }
    break;

  case 130:
#line 624 "ntp_parser.y"
    { enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 131:
#line 625 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 132:
#line 630 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME1, (yyvsp[(2) - (2)].Double)); }
    break;

  case 133:
#line 632 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME2, (yyvsp[(2) - (2)].Double)); }
    break;

  case 134:
#line 634 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEVAL1,  (yyvsp[(2) - (2)].Integer)); }
    break;

  case 135:
#line 636 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CLK_HAVEVAL2,  (yyvsp[(2) - (2)].String)); }
    break;

  case 136:
#line 638 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG1, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 137:
#line 640 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG2, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 138:
#line 642 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG3, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 139:
#line 644 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG4, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 140:
#line 653 "ntp_parser.y"
    { append_queue(my_config.enable_opts,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 141:
#line 655 "ntp_parser.y"
    { append_queue(my_config.disable_opts,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 142:
#line 659 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 143:
#line 660 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 144:
#line 664 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); }
    break;

  case 145:
#line 665 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_BROADCLIENT); }
    break;

  case 146:
#line 666 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_CAL); }
    break;

  case 147:
#line 667 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_KERNEL); }
    break;

  case 148:
#line 668 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_MONITOR); }
    break;

  case 149:
#line 669 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_NTP); }
    break;

  case 150:
#line 671 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_FILEGEN); }
    break;

  case 151:
#line 679 "ntp_parser.y"
    { append_queue(my_config.tinker, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 152:
#line 683 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 153:
#line 684 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 154:
#line 688 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_ALLAN, (yyvsp[(2) - (2)].Double)); }
    break;

  case 155:
#line 689 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PHI, (yyvsp[(2) - (2)].Double)); }
    break;

  case 156:
#line 690 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_FREQ, (yyvsp[(2) - (2)].Double)); }
    break;

  case 157:
#line 691 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_HUFFPUFF, (yyvsp[(2) - (2)].Double)); }
    break;

  case 158:
#line 692 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PANIC, (yyvsp[(2) - (2)].Double)); }
    break;

  case 159:
#line 693 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MAX, (yyvsp[(2) - (2)].Double)); }
    break;

  case 160:
#line 694 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MINSTEP, (yyvsp[(2) - (2)].Double)); }
    break;

  case 161:
#line 703 "ntp_parser.y"
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

  case 162:
#line 719 "ntp_parser.y"
    {
                    while (curr_include_level != -1)
                        FCLOSE(fp[curr_include_level--]);
                }
    break;

  case 163:
#line 725 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, (yyvsp[(2) - (2)].Double)));  }
    break;

  case 164:
#line 727 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Calldelay, (yyvsp[(2) - (2)].Integer)));  }
    break;

  case 165:
#line 729 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Tick, (yyvsp[(2) - (2)].Double)));  }
    break;

  case 166:
#line 731 "ntp_parser.y"
    { /* Null action, possibly all null parms */ }
    break;

  case 167:
#line 733 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Leapfile, (yyvsp[(2) - (2)].String))); }
    break;

  case 168:
#line 736 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Pidfile, (yyvsp[(2) - (2)].String)));  }
    break;

  case 169:
#line 738 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Logfile, (yyvsp[(2) - (2)].String)));  }
    break;

  case 170:
#line 740 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Automax, (yyvsp[(2) - (2)].Integer)));  }
    break;

  case 171:
#line 743 "ntp_parser.y"
    { append_queue(my_config.logconfig, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 172:
#line 745 "ntp_parser.y"
    { append_queue(my_config.phone, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 173:
#line 747 "ntp_parser.y"
    { enqueue(my_config.setvar, (yyvsp[(2) - (2)].Set_var));  }
    break;

  case 174:
#line 749 "ntp_parser.y"
    { enqueue(my_config.trap, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue)));  }
    break;

  case 175:
#line 751 "ntp_parser.y"
    { append_queue(my_config.ttl, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 176:
#line 753 "ntp_parser.y"
    { enqueue(my_config.qos, create_attr_sval(T_Qos, (yyvsp[(2) - (2)].String))); }
    break;

  case 177:
#line 757 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String))); }
    break;

  case 178:
#line 759 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double)));
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String))); }
    break;

  case 179:
#line 761 "ntp_parser.y"
    { /* Null driftfile,  indicated by null string "\0" */
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;

  case 180:
#line 767 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), DEF); }
    break;

  case 181:
#line 769 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (3)].String), (yyvsp[(3) - (3)].String), 0); }
    break;

  case 182:
#line 774 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 183:
#line 775 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 184:
#line 779 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Port, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 185:
#line 780 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_pval(T_Interface, (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 186:
#line 785 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 187:
#line 786 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 188:
#line 790 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 189:
#line 792 "ntp_parser.y"
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

  case 190:
#line 808 "ntp_parser.y"
    { (yyval.Integer) = '+'; }
    break;

  case 191:
#line 809 "ntp_parser.y"
    { (yyval.Integer) = '-'; }
    break;

  case 192:
#line 810 "ntp_parser.y"
    { (yyval.Integer) = '='; }
    break;

  case 193:
#line 819 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 194:
#line 820 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 195:
#line 824 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_pval((yyvsp[(2) - (2)].String))); }
    break;

  case 196:
#line 825 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_pval((yyvsp[(1) - (1)].String))); }
    break;

  case 197:
#line 829 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 198:
#line 830 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Address_node)); }
    break;

  case 199:
#line 835 "ntp_parser.y"
    {
                    if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
                        yyerror("Integer value is not boolean (0 or 1). Assuming 1");
                        (yyval.Integer) = 1;
                    }
                    else
                        (yyval.Integer) = (yyvsp[(1) - (1)].Integer);
                }
    break;

  case 200:
#line 843 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 201:
#line 844 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 202:
#line 848 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 203:
#line 849 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(1) - (1)].Double); }
    break;

  case 204:
#line 858 "ntp_parser.y"
    {
                 my_config.sim_details = create_sim_node((yyvsp[(3) - (5)].Queue), (yyvsp[(4) - (5)].Queue));

                 /* Reset the old_config_style variable */
                 old_config_style = 1;
             }
    break;

  case 205:
#line 872 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 206:
#line 876 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 207:
#line 877 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 208:
#line 881 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Beep_Delay, (yyvsp[(3) - (3)].Double)); }
    break;

  case 209:
#line 882 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Sim_Duration, (yyvsp[(3) - (3)].Double)); }
    break;

  case 210:
#line 886 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_server)); }
    break;

  case 211:
#line 887 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_server)); }
    break;

  case 212:
#line 892 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Queue)); }
    break;

  case 213:
#line 896 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 214:
#line 900 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 215:
#line 904 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(1) - (1)].Address_node); }
    break;

  case 216:
#line 905 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), T_String); }
    break;

  case 217:
#line 909 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_script)); }
    break;

  case 218:
#line 910 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_script)); }
    break;

  case 219:
#line 915 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Queue)); }
    break;

  case 220:
#line 919 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 221:
#line 920 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 222:
#line 925 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Freq_Offset, (yyvsp[(3) - (3)].Double)); }
    break;

  case 223:
#line 927 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Wander, (yyvsp[(3) - (3)].Double)); }
    break;

  case 224:
#line 929 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Jitter, (yyvsp[(3) - (3)].Double)); }
    break;

  case 225:
#line 931 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Prop_Delay, (yyvsp[(3) - (3)].Double)); }
    break;

  case 226:
#line 933 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Proc_Delay, (yyvsp[(3) - (3)].Double)); }
    break;


/* Line 1267 of yacc.c.  */
#line 3202 "../../ntpd/ntp_parser.c"
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


#line 937 "ntp_parser.y"


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


