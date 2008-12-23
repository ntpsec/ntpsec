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
     T_Bias = 266,
     T_Broadcast = 267,
     T_Broadcastclient = 268,
     T_Broadcastdelay = 269,
     T_Burst = 270,
     T_Calibrate = 271,
     T_Calldelay = 272,
     T_Ceiling = 273,
     T_Clockstats = 274,
     T_Cohort = 275,
     T_ControlKey = 276,
     T_Crypto = 277,
     T_Cryptostats = 278,
     T_Day = 279,
     T_Default = 280,
     T_Disable = 281,
     T_Discard = 282,
     T_Dispersion = 283,
     T_Double = 284,
     T_Driftfile = 285,
     T_WanderThreshold = 286,
     T_Enable = 287,
     T_End = 288,
     T_False = 289,
     T_File = 290,
     T_Filegen = 291,
     T_Flag1 = 292,
     T_Flag2 = 293,
     T_Flag3 = 294,
     T_Flag4 = 295,
     T_Flake = 296,
     T_Floor = 297,
     T_Freq = 298,
     T_Fudge = 299,
     T_Host = 300,
     T_Huffpuff = 301,
     T_IPv4_address = 302,
     T_IPv6_address = 303,
     T_Iburst = 304,
     T_Ident = 305,
     T_Ignore = 306,
     T_Includefile = 307,
     T_Integer = 308,
     T_Interface = 309,
     T_Kernel = 310,
     T_Key = 311,
     T_Keys = 312,
     T_Keysdir = 313,
     T_Kod = 314,
     T_Leap = 315,
     T_Leapfile = 316,
     T_Limited = 317,
     T_Link = 318,
     T_Logconfig = 319,
     T_Logfile = 320,
     T_Loopstats = 321,
     T_Lowpriotrap = 322,
     T_Manycastclient = 323,
     T_Manycastserver = 324,
     T_Mask = 325,
     T_Maxclock = 326,
     T_Maxdist = 327,
     T_Maxhop = 328,
     T_Maxpoll = 329,
     T_Minclock = 330,
     T_Mindist = 331,
     T_Minimum = 332,
     T_Minpoll = 333,
     T_Minsane = 334,
     T_Mode = 335,
     T_Monitor = 336,
     T_Month = 337,
     T_Multicastclient = 338,
     T_Nolink = 339,
     T_Nomodify = 340,
     T_None = 341,
     T_Nopeer = 342,
     T_Noquery = 343,
     T_Noselect = 344,
     T_Noserve = 345,
     T_Notrap = 346,
     T_Notrust = 347,
     T_Novolley = 348,
     T_Ntp = 349,
     T_Ntpport = 350,
     T_NtpSignDsocket = 351,
     T_Orphan = 352,
     T_Panic = 353,
     T_Peer = 354,
     T_Peerstats = 355,
     T_Phone = 356,
     T_Pid = 357,
     T_Pidfile = 358,
     T_Pool = 359,
     T_Port = 360,
     T_Pps = 361,
     T_Preempt = 362,
     T_Prefer = 363,
     T_Protostats = 364,
     T_Pw = 365,
     T_Qos = 366,
     T_RandFile = 367,
     T_Rawstats = 368,
     T_Refid = 369,
     T_Requestkey = 370,
     T_Restrict = 371,
     T_Revoke = 372,
     T_Server = 373,
     T_Setvar = 374,
     T_Sign = 375,
     T_Statistics = 376,
     T_Stats = 377,
     T_Statsdir = 378,
     T_Step = 379,
     T_Stepout = 380,
     T_Stratum = 381,
     T_String = 382,
     T_Sysstats = 383,
     T_Tick = 384,
     T_Time1 = 385,
     T_Time2 = 386,
     T_Timingstats = 387,
     T_Tinker = 388,
     T_Tos = 389,
     T_Trap = 390,
     T_True = 391,
     T_Trustedkey = 392,
     T_Ttl = 393,
     T_Type = 394,
     T_Version = 395,
     T_Week = 396,
     T_Xleave = 397,
     T_Year = 398,
     T_Flag = 399,
     T_Void = 400,
     T_EOC = 401,
     T_Simulate = 402,
     T_Beep_Delay = 403,
     T_Sim_Duration = 404,
     T_Server_Offset = 405,
     T_Duration = 406,
     T_Freq_Offset = 407,
     T_Wander = 408,
     T_Jitter = 409,
     T_Prop_Delay = 410,
     T_Proc_Delay = 411
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
#define T_Bias 266
#define T_Broadcast 267
#define T_Broadcastclient 268
#define T_Broadcastdelay 269
#define T_Burst 270
#define T_Calibrate 271
#define T_Calldelay 272
#define T_Ceiling 273
#define T_Clockstats 274
#define T_Cohort 275
#define T_ControlKey 276
#define T_Crypto 277
#define T_Cryptostats 278
#define T_Day 279
#define T_Default 280
#define T_Disable 281
#define T_Discard 282
#define T_Dispersion 283
#define T_Double 284
#define T_Driftfile 285
#define T_WanderThreshold 286
#define T_Enable 287
#define T_End 288
#define T_False 289
#define T_File 290
#define T_Filegen 291
#define T_Flag1 292
#define T_Flag2 293
#define T_Flag3 294
#define T_Flag4 295
#define T_Flake 296
#define T_Floor 297
#define T_Freq 298
#define T_Fudge 299
#define T_Host 300
#define T_Huffpuff 301
#define T_IPv4_address 302
#define T_IPv6_address 303
#define T_Iburst 304
#define T_Ident 305
#define T_Ignore 306
#define T_Includefile 307
#define T_Integer 308
#define T_Interface 309
#define T_Kernel 310
#define T_Key 311
#define T_Keys 312
#define T_Keysdir 313
#define T_Kod 314
#define T_Leap 315
#define T_Leapfile 316
#define T_Limited 317
#define T_Link 318
#define T_Logconfig 319
#define T_Logfile 320
#define T_Loopstats 321
#define T_Lowpriotrap 322
#define T_Manycastclient 323
#define T_Manycastserver 324
#define T_Mask 325
#define T_Maxclock 326
#define T_Maxdist 327
#define T_Maxhop 328
#define T_Maxpoll 329
#define T_Minclock 330
#define T_Mindist 331
#define T_Minimum 332
#define T_Minpoll 333
#define T_Minsane 334
#define T_Mode 335
#define T_Monitor 336
#define T_Month 337
#define T_Multicastclient 338
#define T_Nolink 339
#define T_Nomodify 340
#define T_None 341
#define T_Nopeer 342
#define T_Noquery 343
#define T_Noselect 344
#define T_Noserve 345
#define T_Notrap 346
#define T_Notrust 347
#define T_Novolley 348
#define T_Ntp 349
#define T_Ntpport 350
#define T_NtpSignDsocket 351
#define T_Orphan 352
#define T_Panic 353
#define T_Peer 354
#define T_Peerstats 355
#define T_Phone 356
#define T_Pid 357
#define T_Pidfile 358
#define T_Pool 359
#define T_Port 360
#define T_Pps 361
#define T_Preempt 362
#define T_Prefer 363
#define T_Protostats 364
#define T_Pw 365
#define T_Qos 366
#define T_RandFile 367
#define T_Rawstats 368
#define T_Refid 369
#define T_Requestkey 370
#define T_Restrict 371
#define T_Revoke 372
#define T_Server 373
#define T_Setvar 374
#define T_Sign 375
#define T_Statistics 376
#define T_Stats 377
#define T_Statsdir 378
#define T_Step 379
#define T_Stepout 380
#define T_Stratum 381
#define T_String 382
#define T_Sysstats 383
#define T_Tick 384
#define T_Time1 385
#define T_Time2 386
#define T_Timingstats 387
#define T_Tinker 388
#define T_Tos 389
#define T_Trap 390
#define T_True 391
#define T_Trustedkey 392
#define T_Ttl 393
#define T_Type 394
#define T_Version 395
#define T_Week 396
#define T_Xleave 397
#define T_Year 398
#define T_Flag 399
#define T_Void 400
#define T_EOC 401
#define T_Simulate 402
#define T_Beep_Delay 403
#define T_Sim_Duration 404
#define T_Server_Offset 405
#define T_Duration 406
#define T_Freq_Offset 407
#define T_Wander 408
#define T_Jitter 409
#define T_Prop_Delay 410
#define T_Proc_Delay 411




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
#line 461 "ntp_parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 474 "ntp_parser.c"

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
#define YYFINAL  162
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   607

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  160
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  62
/* YYNRULES -- Number of rules.  */
#define YYNRULES  225
/* YYNRULES -- Number of states.  */
#define YYNSTATES  357

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   411

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   157,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   158,     2,   159,     2,     2,     2,     2,
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
     155,   156
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    15,    16,    18,    20,
      22,    24,    26,    28,    30,    32,    34,    36,    38,    42,
      45,    47,    49,    51,    53,    55,    57,    60,    63,    65,
      68,    70,    72,    75,    77,    79,    82,    85,    88,    90,
      92,    94,    96,    98,   101,   104,   107,   109,   112,   115,
     118,   121,   124,   127,   130,   133,   136,   139,   142,   144,
     145,   148,   150,   153,   156,   159,   162,   165,   168,   171,
     174,   176,   179,   182,   185,   188,   191,   194,   197,   200,
     203,   206,   209,   212,   215,   219,   222,   224,   226,   228,
     230,   232,   234,   236,   238,   240,   243,   245,   248,   251,
     253,   255,   257,   259,   261,   263,   265,   267,   269,   271,
     273,   276,   280,   284,   290,   291,   294,   296,   298,   300,
     302,   304,   306,   308,   310,   312,   314,   316,   318,   320,
     323,   325,   328,   331,   334,   338,   341,   343,   346,   349,
     352,   355,   358,   361,   364,   367,   370,   373,   376,   378,
     380,   382,   384,   386,   388,   390,   392,   395,   398,   400,
     403,   406,   409,   412,   415,   418,   421,   425,   427,   430,
     433,   436,   439,   442,   445,   448,   451,   454,   457,   460,
     464,   467,   470,   472,   475,   476,   481,   485,   488,   490,
     493,   496,   499,   501,   503,   506,   508,   511,   513,   516,
     518,   520,   522,   524,   526,   528,   534,   536,   540,   543,
     547,   551,   554,   556,   562,   567,   571,   574,   576,   583,
     587,   590,   594,   598,   602,   606
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     161,     0,    -1,   162,    -1,   162,   163,   146,    -1,   163,
     146,    -1,     1,   146,    -1,    -1,   164,    -1,   170,    -1,
     171,    -1,   178,    -1,   184,    -1,   175,    -1,   189,    -1,
     192,    -1,   195,    -1,   198,    -1,   210,    -1,   165,   166,
     168,    -1,   165,   166,    -1,   118,    -1,   104,    -1,    99,
      -1,    12,    -1,    68,    -1,   167,    -1,    47,   127,    -1,
      48,   127,    -1,   127,    -1,   168,   169,    -1,   169,    -1,
       6,    -1,    11,   209,    -1,    15,    -1,    49,    -1,    56,
      53,    -1,    78,    53,    -1,    74,    53,    -1,    89,    -1,
     107,    -1,   108,    -1,   136,    -1,   142,    -1,   138,    53,
      -1,    80,    53,    -1,   140,    53,    -1,    13,    -1,    13,
      93,    -1,    69,   207,    -1,    83,   207,    -1,     6,    53,
      -1,    21,    53,    -1,    22,   172,    -1,    57,   127,    -1,
      58,   127,    -1,   115,    53,    -1,   137,   205,    -1,    96,
     127,    -1,   173,    -1,    -1,   173,   174,    -1,   174,    -1,
      45,   127,    -1,    50,   127,    -1,   110,   127,    -1,   112,
     127,    -1,   117,    53,    -1,   120,   127,    -1,   134,   176,
      -1,   176,   177,    -1,   177,    -1,    18,    53,    -1,    42,
      53,    -1,    20,   208,    -1,    97,    53,    -1,    76,   209,
      -1,    72,   209,    -1,    75,   209,    -1,    71,   209,    -1,
      79,    53,    -1,    10,    53,    -1,    73,    53,    -1,   121,
     179,    -1,   123,   127,    -1,    36,   180,   181,    -1,   179,
     180,    -1,   180,    -1,    19,    -1,    23,    -1,    66,    -1,
     100,    -1,   113,    -1,   128,    -1,   132,    -1,   109,    -1,
     181,   182,    -1,   182,    -1,    35,   127,    -1,   139,   183,
      -1,    63,    -1,    84,    -1,    32,    -1,    26,    -1,    86,
      -1,   102,    -1,    24,    -1,   141,    -1,    82,    -1,   143,
      -1,     3,    -1,    27,   187,    -1,   116,   166,   185,    -1,
     116,    25,   185,    -1,   116,   167,    70,   167,   185,    -1,
      -1,   185,   186,    -1,    41,    -1,    51,    -1,    59,    -1,
      62,    -1,    67,    -1,    85,    -1,    87,    -1,    88,    -1,
      90,    -1,    91,    -1,    92,    -1,    95,    -1,   140,    -1,
     187,   188,    -1,   188,    -1,     8,    53,    -1,    77,    53,
      -1,    81,    53,    -1,    44,   166,   190,    -1,   190,   191,
      -1,   191,    -1,   130,   209,    -1,   131,   209,    -1,   126,
      53,    -1,   114,   127,    -1,    37,   208,    -1,    38,   208,
      -1,    39,   208,    -1,    40,   208,    -1,    32,   193,    -1,
      26,   193,    -1,   193,   194,    -1,   194,    -1,     5,    -1,
       9,    -1,    16,    -1,    55,    -1,    81,    -1,    94,    -1,
     122,    -1,   133,   196,    -1,   196,   197,    -1,   197,    -1,
       4,   209,    -1,    28,   209,    -1,    43,   209,    -1,    46,
     209,    -1,    98,   209,    -1,   124,   209,    -1,   125,   209,
      -1,    52,   127,   163,    -1,    33,    -1,    14,   209,    -1,
      17,    53,    -1,   129,   209,    -1,    30,   199,    -1,    61,
     127,    -1,   103,   127,    -1,    65,   127,    -1,     7,    53,
      -1,    64,   203,    -1,   101,   206,    -1,   119,   200,    -1,
     135,   167,   201,    -1,   138,   205,    -1,   111,   127,    -1,
     127,    -1,   127,    29,    -1,    -1,   127,   157,   127,    25,
      -1,   127,   157,   127,    -1,   201,   202,    -1,   202,    -1,
     105,    53,    -1,    54,   167,    -1,   203,   204,    -1,   204,
      -1,   127,    -1,   205,    53,    -1,    53,    -1,   206,   127,
      -1,   127,    -1,   207,   166,    -1,   166,    -1,    53,    -1,
     136,    -1,    34,    -1,    53,    -1,    29,    -1,   211,   158,
     212,   214,   159,    -1,   147,    -1,   212,   213,   146,    -1,
     213,   146,    -1,   148,   157,   209,    -1,   149,   157,   209,
      -1,   214,   215,    -1,   215,    -1,   217,   158,   216,   218,
     159,    -1,   150,   157,   209,   146,    -1,   118,   157,   166,
      -1,   218,   219,    -1,   219,    -1,   151,   157,   209,   158,
     220,   159,    -1,   220,   221,   146,    -1,   221,   146,    -1,
     152,   157,   209,    -1,   153,   157,   209,    -1,   154,   157,
     209,    -1,   155,   157,   209,    -1,   156,   157,   209,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   282,   282,   286,   287,   292,   303,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   322,   328,
     337,   338,   339,   340,   341,   345,   346,   347,   351,   355,
     356,   361,   362,   363,   364,   365,   366,   367,   368,   369,
     370,   371,   372,   373,   374,   375,   385,   387,   389,   391,
     402,   404,   406,   413,   415,   417,   419,   421,   426,   428,
     432,   433,   437,   439,   441,   443,   445,   447,   457,   462,
     463,   467,   469,   471,   473,   475,   477,   479,   481,   483,
     485,   487,   497,   499,   501,   509,   510,   514,   516,   518,
     520,   522,   524,   526,   528,   533,   534,   538,   539,   540,
     541,   542,   543,   547,   548,   549,   550,   551,   552,   553,
     562,   564,   569,   574,   582,   583,   587,   588,   589,   590,
     591,   592,   593,   594,   595,   596,   597,   598,   599,   603,
     604,   608,   609,   610,   618,   623,   624,   628,   630,   632,
     634,   636,   638,   640,   642,   651,   653,   658,   659,   663,
     664,   665,   666,   667,   668,   670,   678,   682,   683,   687,
     688,   689,   690,   691,   692,   693,   701,   717,   723,   725,
     727,   729,   731,   734,   736,   738,   741,   743,   745,   747,
     749,   751,   755,   757,   760,   765,   767,   773,   774,   778,
     779,   784,   785,   789,   806,   807,   811,   812,   816,   817,
     821,   830,   831,   835,   836,   844,   859,   863,   864,   868,
     869,   873,   874,   878,   883,   887,   891,   892,   896,   901,
     902,   906,   908,   910,   912,   914
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_Age", "T_Allan", "T_Auth",
  "T_Autokey", "T_Automax", "T_Average", "T_Bclient", "T_Beacon", "T_Bias",
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
  "T_Ntp", "T_Ntpport", "T_NtpSignDsocket", "T_Orphan", "T_Panic",
  "T_Peer", "T_Peerstats", "T_Phone", "T_Pid", "T_Pidfile", "T_Pool",
  "T_Port", "T_Pps", "T_Preempt", "T_Prefer", "T_Protostats", "T_Pw",
  "T_Qos", "T_RandFile", "T_Rawstats", "T_Refid", "T_Requestkey",
  "T_Restrict", "T_Revoke", "T_Server", "T_Setvar", "T_Sign",
  "T_Statistics", "T_Stats", "T_Statsdir", "T_Step", "T_Stepout",
  "T_Stratum", "T_String", "T_Sysstats", "T_Tick", "T_Time1", "T_Time2",
  "T_Timingstats", "T_Tinker", "T_Tos", "T_Trap", "T_True", "T_Trustedkey",
  "T_Ttl", "T_Type", "T_Version", "T_Week", "T_Xleave", "T_Year", "T_Flag",
  "T_Void", "T_EOC", "T_Simulate", "T_Beep_Delay", "T_Sim_Duration",
  "T_Server_Offset", "T_Duration", "T_Freq_Offset", "T_Wander", "T_Jitter",
  "T_Prop_Delay", "T_Proc_Delay", "'='", "'{'", "'}'", "$accept",
  "configuration", "command_list", "command", "server_command",
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
  "log_config_list", "log_config_command", "integer_list", "string_list",
  "address_list", "boolean", "number", "simulate_command",
  "sim_conf_start", "sim_init_statement_list", "sim_init_statement",
  "sim_server_list", "sim_server", "sim_server_offset", "sim_server_name",
  "sim_act_list", "sim_act", "sim_act_stmt_list", "sim_act_stmt", 0
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
     405,   406,   407,   408,   409,   410,   411,    61,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   160,   161,   162,   162,   162,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   164,   164,
     165,   165,   165,   165,   165,   166,   166,   166,   167,   168,
     168,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   170,   170,   170,   170,
     171,   171,   171,   171,   171,   171,   171,   171,   172,   172,
     173,   173,   174,   174,   174,   174,   174,   174,   175,   176,
     176,   177,   177,   177,   177,   177,   177,   177,   177,   177,
     177,   177,   178,   178,   178,   179,   179,   180,   180,   180,
     180,   180,   180,   180,   180,   181,   181,   182,   182,   182,
     182,   182,   182,   183,   183,   183,   183,   183,   183,   183,
     184,   184,   184,   184,   185,   185,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   187,
     187,   188,   188,   188,   189,   190,   190,   191,   191,   191,
     191,   191,   191,   191,   191,   192,   192,   193,   193,   194,
     194,   194,   194,   194,   194,   194,   195,   196,   196,   197,
     197,   197,   197,   197,   197,   197,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   199,   199,   199,   200,   200,   201,   201,   202,
     202,   203,   203,   204,   205,   205,   206,   206,   207,   207,
     208,   208,   208,   209,   209,   210,   211,   212,   212,   213,
     213,   214,   214,   215,   216,   217,   218,   218,   219,   220,
     220,   221,   221,   221,   221,   221
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     2,
       1,     1,     2,     1,     1,     2,     2,     2,     1,     1,
       1,     1,     1,     2,     2,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     0,
       2,     1,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     3,     5,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     2,     2,     2,     3,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     2,
       2,     2,     2,     2,     2,     2,     3,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     3,
       2,     2,     1,     2,     0,     4,     3,     2,     1,     2,
       2,     2,     1,     1,     2,     1,     2,     1,     2,     1,
       1,     1,     1,     1,     1,     5,     1,     3,     2,     3,
       3,     2,     1,     5,     4,     3,     2,     1,     6,     3,
       2,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,    23,    46,     0,     0,     0,    59,
       0,     0,   184,     0,   167,     0,     0,     0,     0,     0,
       0,     0,     0,    24,     0,     0,     0,    22,     0,     0,
      21,     0,     0,     0,    20,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   206,     0,     2,     0,     7,     0,
       8,     9,    12,    10,    11,    13,    14,    15,    16,    17,
       0,     5,    50,   175,    47,   204,   203,   168,   169,    51,
       0,     0,     0,     0,     0,     0,    52,    58,    61,   149,
     150,   151,   152,   153,   154,   155,   146,   148,     0,     0,
       0,   110,   130,   182,   171,   145,    87,    88,    89,    90,
      94,    91,    92,    93,     0,     0,     0,    28,     0,    25,
       6,    53,    54,   172,   193,   176,   192,   174,   199,    48,
      49,    57,   197,   177,   173,   181,    55,   114,   114,    25,
       0,   178,    82,    86,    83,   170,     0,     0,     0,     0,
       0,     0,     0,   156,   158,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    68,    70,     0,   195,
      56,   180,     1,     0,     4,    19,     0,    62,    63,    64,
      65,    66,    67,    60,   147,   131,   132,   133,   129,   183,
     102,   101,     0,    99,   100,     0,    84,    96,    26,    27,
       0,     0,     0,     0,     0,     0,     0,     0,   134,   136,
     166,   191,   198,   196,   112,   111,     0,     0,    85,   159,
     160,   161,   162,   163,   164,   165,   157,    80,    71,   202,
     200,   201,    73,    72,    78,    76,    81,    77,    75,    79,
      74,    69,     0,     0,   179,   188,   194,     3,    31,     0,
      33,    34,     0,     0,     0,     0,    38,    39,    40,    41,
       0,     0,    42,    18,    30,     0,     0,     0,     0,    97,
     109,   105,   107,   103,   104,   106,   108,    98,    95,   141,
     142,   143,   144,   140,   139,   137,   138,   135,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   115,   114,   186,   190,   189,   187,    32,    35,    37,
      36,    44,    43,    45,    29,     0,     0,     0,     0,     0,
     212,     0,   208,   113,   185,   209,   210,     0,   207,   205,
     211,     0,   215,     0,     0,     0,     0,     0,   217,     0,
       0,   213,   216,   214,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   218,     0,
     220,   221,   222,   223,   224,   225,   219
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    45,    46,    47,    48,    49,   118,   109,   253,   254,
      50,    51,    76,    77,    78,    52,   156,   157,    53,   132,
     104,   186,   187,   267,    54,   204,   291,    91,    92,    55,
     198,   199,    56,    86,    87,    57,   143,   144,    58,    94,
     131,   234,   235,   115,   116,   160,   123,   119,   222,    67,
      59,    60,   257,   258,   309,   310,   324,   311,   327,   328,
     341,   342
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -151
static const yytype_int16 yypact[] =
{
     112,  -123,   -28,   -15,  -151,   -59,    -7,    -8,    -6,    43,
       1,    -4,   -78,     1,  -151,    62,   -11,   -77,   -74,   -69,
     -61,   -53,   -52,  -151,   -11,   -11,   -51,  -151,   -49,   -48,
    -151,   -47,    30,   -20,  -151,   -41,    62,   -40,    -7,    11,
     368,   -36,    36,    36,  -151,    92,   258,   -50,  -151,   -11,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
     -37,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
     -30,   -10,     0,     3,    69,     5,  -151,    43,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,     1,  -151,    84,    87,
      88,    -4,  -151,   114,  -151,     1,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,   -19,    19,    20,  -151,   147,  -151,
     394,  -151,  -151,  -151,  -151,   -53,  -151,  -151,  -151,   -11,
     -11,  -151,  -151,    23,  -151,  -151,  -151,  -151,  -151,    82,
      -3,  -151,    62,  -151,  -151,  -151,    -7,    -7,    -7,    -7,
      -7,    -7,    -7,    11,  -151,   104,   108,    -5,   113,    -7,
      -7,   115,    -7,    -7,   119,   125,   368,  -151,   -46,  -151,
     126,   126,  -151,    37,  -151,   465,  -118,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,    38,  -151,  -151,     8,   -19,  -151,  -151,  -151,
      -5,    -5,    -5,    -5,    55,   135,    -7,    -7,   147,  -151,
    -151,  -151,  -151,  -151,   416,   416,   -36,    64,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,   -36,   136,   -46,  -151,  -151,  -151,  -151,    -7,
    -151,  -151,   139,   140,   143,   144,  -151,  -151,  -151,  -151,
     145,   146,  -151,   465,  -151,    45,    46,   -97,    58,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,   180,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,    -7,    -7,    49,    61,  -104,
    -151,    51,  -151,   416,  -151,  -151,  -151,   -11,  -151,  -151,
    -151,    60,  -151,    57,    66,    -7,    63,  -133,  -151,    72,
      -7,  -151,  -151,  -151,    54,   -84,    65,    67,    68,    75,
      77,   -92,    73,    -7,    -7,    -7,    -7,    -7,  -151,    80,
    -151,  -151,  -151,  -151,  -151,  -151,  -151
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -151,  -151,  -151,   -26,  -151,  -151,   -14,   -32,  -151,   -17,
    -151,  -151,  -151,  -151,   152,  -151,  -151,    81,  -151,  -151,
     -24,  -151,    35,  -151,  -151,  -125,  -151,  -151,   148,  -151,
    -151,    40,  -151,   227,   -62,  -151,  -151,    99,  -151,  -151,
    -151,  -151,     9,  -151,   129,   205,  -151,   226,  -150,   -38,
    -151,  -151,  -151,    -2,  -151,   -57,  -151,  -151,  -151,   -73,
    -151,   -88
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     135,   129,   108,   205,    88,   127,    79,   180,   232,   158,
      80,   260,   133,   181,   307,   136,   182,    81,   326,   128,
     163,   307,    65,    61,   174,    62,   331,   105,   106,   219,
     255,   256,   261,   174,    64,   165,   105,   106,    63,   137,
     269,   270,   271,   272,   183,    68,    66,    69,   220,    93,
     110,   255,   256,   111,   138,   319,    82,   139,   112,   233,
     336,   337,   338,   339,   340,   184,   113,   348,   336,   337,
     338,   339,   340,    89,   114,   117,   121,    90,   122,   124,
     125,    96,    83,   126,   200,    97,   130,   134,    70,   159,
     262,   107,   162,    71,   263,    84,   164,   167,   209,   210,
     211,   212,   213,   214,   215,   202,   202,   107,   208,   140,
     264,   224,   225,     1,   227,   228,   107,   168,     2,     3,
     185,   166,   171,    85,     4,     5,     6,   169,    98,     7,
     170,   221,   172,     8,     9,   141,   142,   175,    10,    11,
     176,   177,    12,   179,    13,    14,   188,   189,    15,   265,
     203,   266,   206,    72,   207,    73,    16,   217,   275,   276,
      74,   218,    99,    75,    17,   259,   223,   313,   226,    18,
      19,   100,   229,    20,   292,   101,    21,    22,   230,   236,
      23,    24,   273,   237,   190,   191,   192,   193,   274,   295,
     102,   293,   298,   299,   103,    25,   300,   301,   302,   303,
     294,   297,   305,   306,   312,   314,   317,   318,    26,   321,
     323,    27,   335,    28,   325,    29,    30,   326,   333,   350,
     330,   268,   343,    31,   344,   345,   356,    32,    33,   173,
      34,    35,   346,    36,   347,    37,   304,   231,   277,   178,
      95,    38,   216,   296,   201,    39,    40,    41,   161,    42,
      43,   120,   320,   349,   332,   308,     0,     0,    -6,    44,
       0,   194,     0,     0,     2,     3,     0,   315,   316,     0,
       4,     5,     6,   195,     0,     7,     0,   196,   197,     8,
       9,     0,     0,     0,    10,    11,     0,   329,    12,     0,
      13,    14,   334,     0,    15,     0,     0,     0,     0,     0,
       0,     0,    16,   322,     0,   351,   352,   353,   354,   355,
      17,     0,     0,     0,     0,    18,    19,     0,     0,    20,
       0,     0,    21,    22,     0,     0,    23,    24,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    26,     0,     0,    27,     0,    28,
       0,    29,    30,     0,     0,     0,     0,     0,     0,    31,
       0,     0,     0,    32,    33,     0,    34,    35,   145,    36,
       0,    37,     0,     0,     0,     0,   146,    38,   147,     0,
       0,    39,    40,    41,     0,    42,    43,     0,     0,     0,
       2,     3,     0,     0,    -6,    44,     4,     5,     6,     0,
     148,     7,     0,     0,     0,     8,     9,     0,     0,     0,
      10,    11,     0,     0,    12,     0,    13,    14,     0,     0,
      15,     0,     0,     0,     0,     0,     0,     0,    16,   149,
     150,   151,     0,   152,   153,     0,    17,   154,     0,     0,
       0,    18,    19,     0,     0,    20,     0,   278,    21,    22,
       0,     0,    23,    24,     0,   155,     0,   279,     0,     0,
       0,   238,     0,     0,     0,   280,   239,    25,   281,     0,
     240,     0,     0,   282,     0,     0,     0,     0,     0,     0,
      26,     0,     0,    27,     0,    28,     0,    29,    30,     0,
       0,   283,     0,   284,   285,    31,   286,   287,   288,    32,
      33,   289,    34,    35,   241,    36,     0,    37,     0,     0,
       0,   242,     0,    38,     0,     0,     0,    39,    40,    41,
       0,    42,    43,     0,     0,     0,     0,     0,     0,   243,
       0,    44,     0,   244,     0,   245,     0,     0,     0,     0,
       0,     0,     0,     0,   246,     0,   290,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   247,   248,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   249,     0,   250,     0,   251,     0,   252
};

static const yytype_int16 yycheck[] =
{
      38,    33,    16,   128,     8,    25,     5,    26,    54,    41,
       9,     3,    36,    32,   118,     4,    35,    16,   151,    33,
      46,   118,    29,   146,    86,    53,   159,    47,    48,    34,
     148,   149,    24,    95,    93,    49,    47,    48,    53,    28,
     190,   191,   192,   193,    63,    53,    53,    53,    53,   127,
     127,   148,   149,   127,    43,   159,    55,    46,   127,   105,
     152,   153,   154,   155,   156,    84,   127,   159,   152,   153,
     154,   155,   156,    77,   127,   127,   127,    81,   127,   127,
     127,    19,    81,    53,   110,    23,   127,   127,    45,    53,
      82,   127,     0,    50,    86,    94,   146,   127,   136,   137,
     138,   139,   140,   141,   142,   119,   120,   127,   132,    98,
     102,   149,   150,     1,   152,   153,   127,   127,     6,     7,
     139,   158,    53,   122,    12,    13,    14,   127,    66,    17,
     127,   136,   127,    21,    22,   124,   125,    53,    26,    27,
      53,    53,    30,    29,    32,    33,   127,   127,    36,   141,
     127,   143,    70,   110,   157,   112,    44,    53,   196,   197,
     117,    53,   100,   120,    52,   127,    53,   292,    53,    57,
      58,   109,    53,    61,   206,   113,    64,    65,    53,    53,
      68,    69,   127,   146,    37,    38,    39,    40,    53,    53,
     128,   127,    53,    53,   132,    83,    53,    53,    53,    53,
     232,   239,   157,   157,   146,    25,   157,   146,    96,   158,
     150,    99,   158,   101,   157,   103,   104,   151,   146,   146,
     157,   186,   157,   111,   157,   157,   146,   115,   116,    77,
     118,   119,   157,   121,   157,   123,   253,   156,   198,    91,
      13,   129,   143,   234,   115,   133,   134,   135,    43,   137,
     138,    25,   309,   341,   327,   257,    -1,    -1,   146,   147,
      -1,   114,    -1,    -1,     6,     7,    -1,   305,   306,    -1,
      12,    13,    14,   126,    -1,    17,    -1,   130,   131,    21,
      22,    -1,    -1,    -1,    26,    27,    -1,   325,    30,    -1,
      32,    33,   330,    -1,    36,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,   317,    -1,   343,   344,   345,   346,   347,
      52,    -1,    -1,    -1,    -1,    57,    58,    -1,    -1,    61,
      -1,    -1,    64,    65,    -1,    -1,    68,    69,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    96,    -1,    -1,    99,    -1,   101,
      -1,   103,   104,    -1,    -1,    -1,    -1,    -1,    -1,   111,
      -1,    -1,    -1,   115,   116,    -1,   118,   119,    10,   121,
      -1,   123,    -1,    -1,    -1,    -1,    18,   129,    20,    -1,
      -1,   133,   134,   135,    -1,   137,   138,    -1,    -1,    -1,
       6,     7,    -1,    -1,   146,   147,    12,    13,    14,    -1,
      42,    17,    -1,    -1,    -1,    21,    22,    -1,    -1,    -1,
      26,    27,    -1,    -1,    30,    -1,    32,    33,    -1,    -1,
      36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    71,
      72,    73,    -1,    75,    76,    -1,    52,    79,    -1,    -1,
      -1,    57,    58,    -1,    -1,    61,    -1,    41,    64,    65,
      -1,    -1,    68,    69,    -1,    97,    -1,    51,    -1,    -1,
      -1,     6,    -1,    -1,    -1,    59,    11,    83,    62,    -1,
      15,    -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      96,    -1,    -1,    99,    -1,   101,    -1,   103,   104,    -1,
      -1,    85,    -1,    87,    88,   111,    90,    91,    92,   115,
     116,    95,   118,   119,    49,   121,    -1,   123,    -1,    -1,
      -1,    56,    -1,   129,    -1,    -1,    -1,   133,   134,   135,
      -1,   137,   138,    -1,    -1,    -1,    -1,    -1,    -1,    74,
      -1,   147,    -1,    78,    -1,    80,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,   140,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,   108,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   136,    -1,   138,    -1,   140,    -1,   142
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     6,     7,    12,    13,    14,    17,    21,    22,
      26,    27,    30,    32,    33,    36,    44,    52,    57,    58,
      61,    64,    65,    68,    69,    83,    96,    99,   101,   103,
     104,   111,   115,   116,   118,   119,   121,   123,   129,   133,
     134,   135,   137,   138,   147,   161,   162,   163,   164,   165,
     170,   171,   175,   178,   184,   189,   192,   195,   198,   210,
     211,   146,    53,    53,    93,    29,    53,   209,    53,    53,
      45,    50,   110,   112,   117,   120,   172,   173,   174,     5,
       9,    16,    55,    81,    94,   122,   193,   194,     8,    77,
      81,   187,   188,   127,   199,   193,    19,    23,    66,   100,
     109,   113,   128,   132,   180,    47,    48,   127,   166,   167,
     127,   127,   127,   127,   127,   203,   204,   127,   166,   207,
     207,   127,   127,   206,   127,   127,    53,    25,   166,   167,
     127,   200,   179,   180,   127,   209,     4,    28,    43,    46,
      98,   124,   125,   196,   197,    10,    18,    20,    42,    71,
      72,    73,    75,    76,    79,    97,   176,   177,   167,    53,
     205,   205,     0,   163,   146,   166,   158,   127,   127,   127,
     127,    53,   127,   174,   194,    53,    53,    53,   188,    29,
      26,    32,    35,    63,    84,   139,   181,   182,   127,   127,
      37,    38,    39,    40,   114,   126,   130,   131,   190,   191,
     163,   204,   166,   127,   185,   185,    70,   157,   180,   209,
     209,   209,   209,   209,   209,   209,   197,    53,    53,    34,
      53,   136,   208,    53,   209,   209,    53,   209,   209,    53,
      53,   177,    54,   105,   201,   202,    53,   146,     6,    11,
      15,    49,    56,    74,    78,    80,    89,   107,   108,   136,
     138,   140,   142,   168,   169,   148,   149,   212,   213,   127,
       3,    24,    82,    86,   102,   141,   143,   183,   182,   208,
     208,   208,   208,   127,    53,   209,   209,   191,    41,    51,
      59,    62,    67,    85,    87,    88,    90,    91,    92,    95,
     140,   186,   167,   127,   167,    53,   202,   209,    53,    53,
      53,    53,    53,    53,   169,   157,   157,   118,   213,   214,
     215,   217,   146,   185,    25,   209,   209,   157,   146,   159,
     215,   158,   166,   150,   216,   157,   151,   218,   219,   209,
     157,   159,   219,   146,   209,   158,   152,   153,   154,   155,
     156,   220,   221,   157,   157,   157,   157,   157,   159,   221,
     146,   209,   209,   209,   209,   209,   146
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
#line 293 "ntp_parser.y"
    {
					if (input_from_file == 1) {
			msyslog(LOG_ERR, "parse error %s line %d ignored\n",
                            ip_file->fname, ip_file->line_no);
			} else if (input_from_file != 0)
				msyslog(LOG_ERR,
				    "parse: bad boolean input flag\n");
                }
    break;

  case 18:
#line 323 "ntp_parser.y"
    {
                        struct peer_node *my_node =  create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue));
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;

  case 19:
#line 329 "ntp_parser.y"
    {
                        struct peer_node *my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;

  case 20:
#line 337 "ntp_parser.y"
    { (yyval.Integer) = T_Server; }
    break;

  case 21:
#line 338 "ntp_parser.y"
    { (yyval.Integer) = T_Pool; }
    break;

  case 22:
#line 339 "ntp_parser.y"
    { (yyval.Integer) = T_Peer; }
    break;

  case 23:
#line 340 "ntp_parser.y"
    { (yyval.Integer) = T_Broadcast; }
    break;

  case 24:
#line 341 "ntp_parser.y"
    { (yyval.Integer) = T_Manycastclient; }
    break;

  case 25:
#line 345 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(1) - (1)].Address_node); }
    break;

  case 26:
#line 346 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET); }
    break;

  case 27:
#line 347 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET6); }
    break;

  case 28:
#line 351 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), 0); }
    break;

  case 29:
#line 355 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 30:
#line 356 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 31:
#line 361 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_SKEY); }
    break;

  case 32:
#line 362 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Bias, (yyvsp[(2) - (2)].Double)); }
    break;

  case 33:
#line 363 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_BURST); }
    break;

  case 34:
#line 364 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_IBURST); }
    break;

  case 35:
#line 365 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Key, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 36:
#line 366 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Minpoll, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 37:
#line 367 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Maxpoll, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 38:
#line 368 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_NOSELECT);}
    break;

  case 39:
#line 369 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREEMPT); }
    break;

  case 40:
#line 370 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREFER); }
    break;

  case 41:
#line 371 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_TRUE); }
    break;

  case 42:
#line 372 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_XLEAVE); }
    break;

  case 43:
#line 373 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Ttl, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 44:
#line 374 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Mode, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:
#line 375 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Version, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 46:
#line 386 "ntp_parser.y"
    { my_config.broadcastclient = SIMPLE; }
    break;

  case 47:
#line 388 "ntp_parser.y"
    { my_config.broadcastclient = NOVOLLEY;  }
    break;

  case 48:
#line 390 "ntp_parser.y"
    { append_queue(my_config.manycastserver, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 49:
#line 392 "ntp_parser.y"
    { append_queue(my_config.multicastclient, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 50:
#line 403 "ntp_parser.y"
    { my_config.auth.autokey = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 51:
#line 405 "ntp_parser.y"
    { my_config.auth.control_key = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 52:
#line 407 "ntp_parser.y"
    { if (my_config.auth.crypto_cmd_list != NULL)
					append_queue(my_config.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Queue));
				else
					my_config.auth.crypto_cmd_list = (yyvsp[(2) - (2)].Queue);
			cryptosw++;
			}
    break;

  case 53:
#line 414 "ntp_parser.y"
    { my_config.auth.keys = (yyvsp[(2) - (2)].String);  }
    break;

  case 54:
#line 416 "ntp_parser.y"
    { my_config.auth.keysdir = (yyvsp[(2) - (2)].String);  }
    break;

  case 55:
#line 418 "ntp_parser.y"
    { my_config.auth.requested_key = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 56:
#line 420 "ntp_parser.y"
    { my_config.auth.trusted_key_list = (yyvsp[(2) - (2)].Queue);  }
    break;

  case 57:
#line 422 "ntp_parser.y"
    { my_config.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String);  }
    break;

  case 59:
#line 428 "ntp_parser.y"
    { (yyval.Queue) = NULL; }
    break;

  case 60:
#line 432 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 61:
#line 433 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 62:
#line 438 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PRIV, (yyvsp[(2) - (2)].String)); }
    break;

  case 63:
#line 440 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_IDENT, (yyvsp[(2) - (2)].String)); }
    break;

  case 64:
#line 442 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PW, (yyvsp[(2) - (2)].String)); }
    break;

  case 65:
#line 444 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_RAND, (yyvsp[(2) - (2)].String)); }
    break;

  case 66:
#line 446 "ntp_parser.y"
    { my_config.auth.revoke = (yyvsp[(2) - (2)].Integer);  }
    break;

  case 67:
#line 448 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_SIGN, (yyvsp[(2) - (2)].String)); }
    break;

  case 68:
#line 458 "ntp_parser.y"
    { append_queue(my_config.orphan_cmds,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 69:
#line 462 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 70:
#line 463 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 71:
#line 468 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_CEILING, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 72:
#line 470 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_FLOOR, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 73:
#line 472 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_COHORT, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 74:
#line 474 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_ORPHAN, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 75:
#line 476 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINDISP, (yyvsp[(2) - (2)].Double)); }
    break;

  case 76:
#line 478 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXDIST, (yyvsp[(2) - (2)].Double)); }
    break;

  case 77:
#line 480 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINCLOCK, (yyvsp[(2) - (2)].Double)); }
    break;

  case 78:
#line 482 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXCLOCK, (yyvsp[(2) - (2)].Double)); }
    break;

  case 79:
#line 484 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINSANE, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 80:
#line 486 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_BEACON, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 81:
#line 488 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXHOP, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 82:
#line 498 "ntp_parser.y"
    { append_queue(my_config.stats_list, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 83:
#line 500 "ntp_parser.y"
    { my_config.stats_dir = (yyvsp[(2) - (2)].String);  }
    break;

  case 84:
#line 502 "ntp_parser.y"
    {
                        enqueue(my_config.filegen_opts,
                                create_filegen_node((yyvsp[(2) - (3)].VoidPtr), (yyvsp[(3) - (3)].Queue)));
                    }
    break;

  case 85:
#line 509 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); }
    break;

  case 86:
#line 510 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].VoidPtr)); }
    break;

  case 87:
#line 515 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("clockstats"); }
    break;

  case 88:
#line 517 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("cryptostats"); }
    break;

  case 89:
#line 519 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("loopstats"); }
    break;

  case 90:
#line 521 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("peerstats"); }
    break;

  case 91:
#line 523 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("rawstats"); }
    break;

  case 92:
#line 525 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("sysstats"); }
    break;

  case 93:
#line 527 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("timingstats"); }
    break;

  case 94:
#line 529 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("protostats"); }
    break;

  case 95:
#line 533 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 96:
#line 534 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 97:
#line 538 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(T_File, (yyvsp[(2) - (2)].String)); }
    break;

  case 98:
#line 539 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Type, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 99:
#line 540 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Link); }
    break;

  case 100:
#line 541 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Nolink); }
    break;

  case 101:
#line 542 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Enable); }
    break;

  case 102:
#line 543 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Disable); }
    break;

  case 103:
#line 547 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_NONE; }
    break;

  case 104:
#line 548 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_PID; }
    break;

  case 105:
#line 549 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_DAY; }
    break;

  case 106:
#line 550 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_WEEK; }
    break;

  case 107:
#line 551 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_MONTH; }
    break;

  case 108:
#line 552 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_YEAR; }
    break;

  case 109:
#line 553 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_AGE; }
    break;

  case 110:
#line 563 "ntp_parser.y"
    {   append_queue(my_config.discard_opts, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 111:
#line 565 "ntp_parser.y"
    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
                    }
    break;

  case 112:
#line 570 "ntp_parser.y"
    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
                    }
    break;

  case 113:
#line 575 "ntp_parser.y"
    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Queue), ip_file->line_no));
                    }
    break;

  case 114:
#line 582 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 115:
#line 583 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); }
    break;

  case 116:
#line 587 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_TIMEOUT); }
    break;

  case 117:
#line 588 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_IGNORE); }
    break;

  case 118:
#line 589 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_KOD); }
    break;

  case 119:
#line 590 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LIMITED); }
    break;

  case 120:
#line 591 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LPTRAP); }
    break;

  case 121:
#line 592 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOMODIFY); }
    break;

  case 122:
#line 593 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOPEER); }
    break;

  case 123:
#line 594 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOQUERY); }
    break;

  case 124:
#line 595 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTSERVE); }
    break;

  case 125:
#line 596 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOTRAP); }
    break;

  case 126:
#line 597 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTTRUST); }
    break;

  case 127:
#line 598 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RESM_NTPONLY); }
    break;

  case 128:
#line 599 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_VERSION); }
    break;

  case 129:
#line 603 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 130:
#line 604 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 131:
#line 608 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Average, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 132:
#line 609 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Minimum, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 133:
#line 610 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Monitor, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 134:
#line 619 "ntp_parser.y"
    { enqueue(my_config.fudge, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue)));  }
    break;

  case 135:
#line 623 "ntp_parser.y"
    { enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 136:
#line 624 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 137:
#line 629 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME1, (yyvsp[(2) - (2)].Double)); }
    break;

  case 138:
#line 631 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME2, (yyvsp[(2) - (2)].Double)); }
    break;

  case 139:
#line 633 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEVAL1,  (yyvsp[(2) - (2)].Integer)); }
    break;

  case 140:
#line 635 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CLK_HAVEVAL2,  (yyvsp[(2) - (2)].String)); }
    break;

  case 141:
#line 637 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG1, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 142:
#line 639 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG2, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 143:
#line 641 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG3, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 144:
#line 643 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG4, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 145:
#line 652 "ntp_parser.y"
    { append_queue(my_config.enable_opts,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 146:
#line 654 "ntp_parser.y"
    { append_queue(my_config.disable_opts,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 147:
#line 658 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 148:
#line 659 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 149:
#line 663 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); }
    break;

  case 150:
#line 664 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_BROADCLIENT); }
    break;

  case 151:
#line 665 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_CAL); }
    break;

  case 152:
#line 666 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_KERNEL); }
    break;

  case 153:
#line 667 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_MONITOR); }
    break;

  case 154:
#line 668 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_NTP); }
    break;

  case 155:
#line 670 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_FILEGEN); }
    break;

  case 156:
#line 678 "ntp_parser.y"
    { append_queue(my_config.tinker, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 157:
#line 682 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 158:
#line 683 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 159:
#line 687 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_ALLAN, (yyvsp[(2) - (2)].Double)); }
    break;

  case 160:
#line 688 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PHI, (yyvsp[(2) - (2)].Double)); }
    break;

  case 161:
#line 689 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_FREQ, (yyvsp[(2) - (2)].Double)); }
    break;

  case 162:
#line 690 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_HUFFPUFF, (yyvsp[(2) - (2)].Double)); }
    break;

  case 163:
#line 691 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PANIC, (yyvsp[(2) - (2)].Double)); }
    break;

  case 164:
#line 692 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MAX, (yyvsp[(2) - (2)].Double)); }
    break;

  case 165:
#line 693 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MINSTEP, (yyvsp[(2) - (2)].Double)); }
    break;

  case 166:
#line 702 "ntp_parser.y"
    {
                    if (curr_include_level >= MAXINCLUDELEVEL) {
                        fprintf(stderr, "getconfig: Maximum include file level exceeded.\n");
                        msyslog(LOG_ERR, "getconfig: Maximum include file level exceeded.");
                    }
                    else {
                        fp[curr_include_level + 1] = F_OPEN(FindConfig((yyvsp[(2) - (3)].String)), "r");
                        if (fp[curr_include_level + 1] == NULL) {
                            fprintf(stderr, "getconfig: Couldn't open <%s>\n", FindConfig((yyvsp[(2) - (3)].String)));
                            msyslog(LOG_ERR, "getconfig: Couldn't open <%s>", FindConfig((yyvsp[(2) - (3)].String)));
                        }
                        else
                            ip_file = fp[++curr_include_level];
                    }
                }
    break;

  case 167:
#line 718 "ntp_parser.y"
    {
                    while (curr_include_level != -1)
                        FCLOSE(fp[curr_include_level--]);
                }
    break;

  case 168:
#line 724 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, (yyvsp[(2) - (2)].Double)));  }
    break;

  case 169:
#line 726 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Calldelay, (yyvsp[(2) - (2)].Integer)));  }
    break;

  case 170:
#line 728 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Tick, (yyvsp[(2) - (2)].Double)));  }
    break;

  case 171:
#line 730 "ntp_parser.y"
    { /* Null action, possibly all null parms */ }
    break;

  case 172:
#line 732 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Leapfile, (yyvsp[(2) - (2)].String))); }
    break;

  case 173:
#line 735 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Pidfile, (yyvsp[(2) - (2)].String)));  }
    break;

  case 174:
#line 737 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Logfile, (yyvsp[(2) - (2)].String)));  }
    break;

  case 175:
#line 739 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Automax, (yyvsp[(2) - (2)].Integer)));  }
    break;

  case 176:
#line 742 "ntp_parser.y"
    { append_queue(my_config.logconfig, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 177:
#line 744 "ntp_parser.y"
    { append_queue(my_config.phone, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 178:
#line 746 "ntp_parser.y"
    { enqueue(my_config.setvar, (yyvsp[(2) - (2)].Set_var));  }
    break;

  case 179:
#line 748 "ntp_parser.y"
    { enqueue(my_config.trap, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue)));  }
    break;

  case 180:
#line 750 "ntp_parser.y"
    { append_queue(my_config.ttl, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 181:
#line 752 "ntp_parser.y"
    { enqueue(my_config.qos, create_attr_sval(T_Qos, (yyvsp[(2) - (2)].String))); }
    break;

  case 182:
#line 756 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String))); }
    break;

  case 183:
#line 758 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double)));
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String))); }
    break;

  case 184:
#line 760 "ntp_parser.y"
    { /* Null driftfile,  indicated by null string "\0" */
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;

  case 185:
#line 766 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), DEF); }
    break;

  case 186:
#line 768 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (3)].String), (yyvsp[(3) - (3)].String), 0); }
    break;

  case 187:
#line 773 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 188:
#line 774 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 189:
#line 778 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Port, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 190:
#line 779 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_pval(T_Interface, (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 191:
#line 784 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 192:
#line 785 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 193:
#line 790 "ntp_parser.y"
    {
                    char prefix = (yyvsp[(1) - (1)].String)[0];
                    char *type = &((yyvsp[(1) - (1)].String)[1]);
                    if (prefix != '+' && prefix != '-' && prefix != '=') {
                        yyerror("Logconfig prefix is not '+', '-' or '='\n");
                    }
                    else
                        (yyval.Attr_val) = create_attr_sval(prefix, type);
                }
    break;

  case 194:
#line 806 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 195:
#line 807 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 196:
#line 811 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_pval((yyvsp[(2) - (2)].String))); }
    break;

  case 197:
#line 812 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_pval((yyvsp[(1) - (1)].String))); }
    break;

  case 198:
#line 816 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 199:
#line 817 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Address_node)); }
    break;

  case 200:
#line 822 "ntp_parser.y"
    {
                    if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
                        yyerror("Integer value is not boolean (0 or 1). Assuming 1");
                        (yyval.Integer) = 1;
                    }
                    else
                        (yyval.Integer) = (yyvsp[(1) - (1)].Integer);
                }
    break;

  case 201:
#line 830 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 202:
#line 831 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 203:
#line 835 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 204:
#line 836 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(1) - (1)].Double); }
    break;

  case 205:
#line 845 "ntp_parser.y"
    {
                 my_config.sim_details = create_sim_node((yyvsp[(3) - (5)].Queue), (yyvsp[(4) - (5)].Queue));

                 /* Reset the old_config_style variable */
                 old_config_style = 1;
             }
    break;

  case 206:
#line 859 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 207:
#line 863 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 208:
#line 864 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 209:
#line 868 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Beep_Delay, (yyvsp[(3) - (3)].Double)); }
    break;

  case 210:
#line 869 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Sim_Duration, (yyvsp[(3) - (3)].Double)); }
    break;

  case 211:
#line 873 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_server)); }
    break;

  case 212:
#line 874 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_server)); }
    break;

  case 213:
#line 879 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Queue)); }
    break;

  case 214:
#line 883 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 215:
#line 887 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 216:
#line 891 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_script)); }
    break;

  case 217:
#line 892 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_script)); }
    break;

  case 218:
#line 897 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Queue)); }
    break;

  case 219:
#line 901 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 220:
#line 902 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 221:
#line 907 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Freq_Offset, (yyvsp[(3) - (3)].Double)); }
    break;

  case 222:
#line 909 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Wander, (yyvsp[(3) - (3)].Double)); }
    break;

  case 223:
#line 911 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Jitter, (yyvsp[(3) - (3)].Double)); }
    break;

  case 224:
#line 913 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Prop_Delay, (yyvsp[(3) - (3)].Double)); }
    break;

  case 225:
#line 915 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Proc_Delay, (yyvsp[(3) - (3)].Double)); }
    break;


/* Line 1267 of yacc.c.  */
#line 3225 "ntp_parser.c"
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


#line 919 "ntp_parser.y"


/* KEYWORDS
 * --------
 */

void yyerror (char *msg)
{
    int retval;
    if (input_from_file)
        msyslog(LOG_ERR, "%s\n", msg);
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


