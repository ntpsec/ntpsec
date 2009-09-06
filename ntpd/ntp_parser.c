
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.1"

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


  struct FILE_INFO *ip_file;   /* Pointer to the configuration file stream */

  #define YYMALLOC	emalloc
  #define YYFREE	free
  #define YYERROR_VERBOSE
  #define YYMAXDEPTH	1000   /* stop the madness sooner */
  void yyerror (char *msg);
  extern int input_from_file;  /* 0=input from ntpq :config */
  extern int cryptosw;


/* Line 189 of yacc.c  */
#line 108 "ntp_parser.c"

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
     T_Iburst = 301,
     T_Ident = 302,
     T_Ignore = 303,
     T_Includefile = 304,
     T_Integer = 305,
     T_Interface = 306,
     T_Ipv4_flag = 307,
     T_Ipv6_flag = 308,
     T_Kernel = 309,
     T_Key = 310,
     T_Keys = 311,
     T_Keysdir = 312,
     T_Kod = 313,
     T_Mssntp = 314,
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
     T_Ntp = 347,
     T_Ntpport = 348,
     T_NtpSignDsocket = 349,
     T_Orphan = 350,
     T_Panic = 351,
     T_Peer = 352,
     T_Peerstats = 353,
     T_Phone = 354,
     T_Pid = 355,
     T_Pidfile = 356,
     T_Pool = 357,
     T_Port = 358,
     T_Preempt = 359,
     T_Prefer = 360,
     T_Protostats = 361,
     T_Pw = 362,
     T_Qos = 363,
     T_RandFile = 364,
     T_Rawstats = 365,
     T_Refid = 366,
     T_Requestkey = 367,
     T_Restrict = 368,
     T_Revoke = 369,
     T_Server = 370,
     T_Setvar = 371,
     T_Sign = 372,
     T_Statistics = 373,
     T_Stats = 374,
     T_Statsdir = 375,
     T_Step = 376,
     T_Stepout = 377,
     T_Stratum = 378,
     T_String = 379,
     T_Sysstats = 380,
     T_Tick = 381,
     T_Time1 = 382,
     T_Time2 = 383,
     T_Timingstats = 384,
     T_Tinker = 385,
     T_Tos = 386,
     T_Trap = 387,
     T_True = 388,
     T_Trustedkey = 389,
     T_Ttl = 390,
     T_Type = 391,
     T_Unconfig = 392,
     T_Unpeer = 393,
     T_Version = 394,
     T_WanderThreshold = 395,
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
#define T_Iburst 301
#define T_Ident 302
#define T_Ignore 303
#define T_Includefile 304
#define T_Integer 305
#define T_Interface 306
#define T_Ipv4_flag 307
#define T_Ipv6_flag 308
#define T_Kernel 309
#define T_Key 310
#define T_Keys 311
#define T_Keysdir 312
#define T_Kod 313
#define T_Mssntp 314
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
#define T_Ntp 347
#define T_Ntpport 348
#define T_NtpSignDsocket 349
#define T_Orphan 350
#define T_Panic 351
#define T_Peer 352
#define T_Peerstats 353
#define T_Phone 354
#define T_Pid 355
#define T_Pidfile 356
#define T_Pool 357
#define T_Port 358
#define T_Preempt 359
#define T_Prefer 360
#define T_Protostats 361
#define T_Pw 362
#define T_Qos 363
#define T_RandFile 364
#define T_Rawstats 365
#define T_Refid 366
#define T_Requestkey 367
#define T_Restrict 368
#define T_Revoke 369
#define T_Server 370
#define T_Setvar 371
#define T_Sign 372
#define T_Statistics 373
#define T_Stats 374
#define T_Statsdir 375
#define T_Step 376
#define T_Stepout 377
#define T_Stratum 378
#define T_String 379
#define T_Sysstats 380
#define T_Tick 381
#define T_Time1 382
#define T_Time2 383
#define T_Timingstats 384
#define T_Tinker 385
#define T_Tos 386
#define T_Trap 387
#define T_True 388
#define T_Trustedkey 389
#define T_Ttl 390
#define T_Type 391
#define T_Unconfig 392
#define T_Unpeer 393
#define T_Version 394
#define T_WanderThreshold 395
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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 51 "ntp_parser.y"

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



/* Line 214 of yacc.c  */
#line 473 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 485 "ntp_parser.c"

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
#define YYFINAL  167
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   639

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  160
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  232
/* YYNRULES -- Number of states.  */
#define YYNSTATES  368

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
      22,    24,    26,    28,    30,    32,    34,    36,    38,    40,
      44,    47,    49,    51,    53,    55,    57,    59,    62,    65,
      67,    70,    72,    74,    77,    79,    81,    84,    87,    90,
      92,    94,    96,    98,   100,   103,   106,   109,   112,   114,
     116,   118,   121,   124,   127,   130,   133,   136,   139,   142,
     145,   148,   150,   151,   154,   156,   159,   162,   165,   168,
     171,   174,   177,   180,   182,   185,   188,   191,   194,   197,
     200,   203,   206,   209,   212,   215,   218,   221,   225,   228,
     230,   232,   234,   236,   238,   240,   242,   244,   246,   249,
     251,   254,   257,   259,   261,   263,   265,   267,   269,   271,
     273,   275,   277,   279,   282,   286,   290,   295,   300,   306,
     307,   310,   312,   314,   316,   318,   320,   322,   324,   326,
     328,   330,   332,   334,   336,   338,   341,   343,   346,   349,
     352,   356,   359,   361,   364,   367,   370,   373,   376,   379,
     382,   385,   388,   391,   394,   396,   398,   400,   402,   404,
     406,   408,   410,   413,   416,   418,   421,   424,   427,   430,
     433,   436,   439,   443,   445,   448,   451,   454,   457,   460,
     463,   466,   469,   472,   475,   478,   481,   485,   488,   491,
     493,   496,   497,   502,   506,   509,   511,   514,   517,   520,
     522,   524,   527,   529,   532,   534,   537,   539,   541,   543,
     545,   547,   549,   555,   557,   561,   564,   568,   572,   575,
     577,   583,   588,   592,   595,   597,   604,   608,   611,   615,
     619,   623,   627
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     161,     0,    -1,   162,    -1,   162,   163,   146,    -1,   163,
     146,    -1,     1,   146,    -1,    -1,   164,    -1,   170,    -1,
     172,    -1,   173,    -1,   180,    -1,   186,    -1,   177,    -1,
     191,    -1,   194,    -1,   197,    -1,   200,    -1,   212,    -1,
     165,   166,   168,    -1,   165,   166,    -1,   115,    -1,   102,
      -1,    97,    -1,    12,    -1,    67,    -1,   167,    -1,    52,
     124,    -1,    53,   124,    -1,   124,    -1,   168,   169,    -1,
     169,    -1,     6,    -1,    11,   211,    -1,    15,    -1,    46,
      -1,    55,    50,    -1,    77,    50,    -1,    73,    50,    -1,
      88,    -1,   104,    -1,   105,    -1,   133,    -1,   142,    -1,
     135,    50,    -1,    79,    50,    -1,   139,    50,    -1,   171,
     166,    -1,   137,    -1,   138,    -1,    13,    -1,    68,   209,
      -1,    82,   209,    -1,     6,    50,    -1,    21,    50,    -1,
      22,   174,    -1,    56,   124,    -1,    57,   124,    -1,   112,
      50,    -1,   134,   207,    -1,    94,   124,    -1,   175,    -1,
      -1,   175,   176,    -1,   176,    -1,    44,   124,    -1,    47,
     124,    -1,   107,   124,    -1,   109,   124,    -1,   114,    50,
      -1,   117,   124,    -1,   131,   178,    -1,   178,   179,    -1,
     179,    -1,    18,    50,    -1,    41,    50,    -1,    20,   210,
      -1,    95,    50,    -1,    75,   211,    -1,    71,   211,    -1,
      74,   211,    -1,    70,   211,    -1,    78,    50,    -1,    10,
      50,    -1,    72,    50,    -1,   118,   181,    -1,   120,   124,
      -1,    35,   182,   183,    -1,   181,   182,    -1,   182,    -1,
      19,    -1,    23,    -1,    65,    -1,    98,    -1,   110,    -1,
     125,    -1,   129,    -1,   106,    -1,   183,   184,    -1,   184,
      -1,    34,   124,    -1,   136,   185,    -1,    62,    -1,    83,
      -1,    31,    -1,    26,    -1,    85,    -1,   100,    -1,    24,
      -1,   141,    -1,    81,    -1,   143,    -1,     3,    -1,    27,
     189,    -1,   113,   166,   187,    -1,   113,    25,   187,    -1,
     113,    52,    25,   187,    -1,   113,    53,    25,   187,    -1,
     113,   167,    69,   167,   187,    -1,    -1,   187,   188,    -1,
      40,    -1,    48,    -1,    58,    -1,    59,    -1,    61,    -1,
      66,    -1,    84,    -1,    86,    -1,    87,    -1,    89,    -1,
      90,    -1,    91,    -1,    93,    -1,   139,    -1,   189,   190,
      -1,   190,    -1,     8,    50,    -1,    76,    50,    -1,    80,
      50,    -1,    43,   166,   192,    -1,   192,   193,    -1,   193,
      -1,   127,   211,    -1,   128,   211,    -1,   123,    50,    -1,
     111,   124,    -1,    36,   210,    -1,    37,   210,    -1,    38,
     210,    -1,    39,   210,    -1,    31,   195,    -1,    26,   195,
      -1,   195,   196,    -1,   196,    -1,     5,    -1,     9,    -1,
      16,    -1,    54,    -1,    80,    -1,    92,    -1,   119,    -1,
     130,   198,    -1,   198,   199,    -1,   199,    -1,     4,   211,
      -1,    28,   211,    -1,    42,   211,    -1,    45,   211,    -1,
      96,   211,    -1,   121,   211,    -1,   122,   211,    -1,    49,
     124,   163,    -1,    32,    -1,    14,   211,    -1,    17,    50,
      -1,   126,   211,    -1,    30,   201,    -1,    60,   124,    -1,
     101,   124,    -1,    64,   124,    -1,     7,    50,    -1,    63,
     205,    -1,    99,   208,    -1,   116,   202,    -1,   132,   167,
      -1,   132,   167,   203,    -1,   135,   207,    -1,   108,   124,
      -1,   124,    -1,   124,    29,    -1,    -1,   124,   157,   124,
      25,    -1,   124,   157,   124,    -1,   203,   204,    -1,   204,
      -1,   103,    50,    -1,    51,   167,    -1,   205,   206,    -1,
     206,    -1,   124,    -1,   207,    50,    -1,    50,    -1,   208,
     124,    -1,   124,    -1,   209,   166,    -1,   166,    -1,    50,
      -1,   133,    -1,    33,    -1,    50,    -1,    29,    -1,   213,
     158,   214,   216,   159,    -1,   147,    -1,   214,   215,   146,
      -1,   215,   146,    -1,   148,   157,   211,    -1,   149,   157,
     211,    -1,   216,   217,    -1,   217,    -1,   219,   158,   218,
     220,   159,    -1,   150,   157,   211,   146,    -1,   115,   157,
     166,    -1,   220,   221,    -1,   221,    -1,   151,   157,   211,
     158,   222,   159,    -1,   222,   223,   146,    -1,   223,   146,
      -1,   152,   157,   211,    -1,   153,   157,   211,    -1,   154,
     157,   211,    -1,   155,   157,   211,    -1,   156,   157,   211,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   285,   285,   289,   290,   291,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   335,
     341,   350,   351,   352,   353,   354,   358,   359,   360,   364,
     368,   369,   373,   374,   375,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   396,   404,   405,
     415,   417,   419,   430,   432,   434,   442,   444,   446,   448,
     450,   455,   457,   461,   462,   466,   468,   470,   472,   474,
     476,   486,   491,   492,   496,   498,   500,   502,   504,   506,
     508,   510,   512,   514,   516,   526,   528,   530,   538,   539,
     543,   545,   547,   549,   551,   553,   555,   557,   562,   563,
     567,   569,   571,   572,   573,   574,   578,   579,   580,   581,
     582,   583,   584,   593,   597,   602,   607,   620,   633,   641,
     642,   646,   647,   648,   649,   650,   651,   652,   653,   654,
     655,   656,   657,   658,   659,   663,   664,   668,   669,   670,
     678,   683,   684,   688,   690,   692,   694,   696,   698,   700,
     702,   711,   713,   718,   719,   723,   724,   725,   726,   727,
     728,   730,   738,   742,   743,   747,   748,   749,   750,   751,
     752,   753,   761,   777,   783,   785,   787,   789,   791,   794,
     796,   798,   801,   803,   805,   807,   809,   811,   813,   817,
     819,   823,   827,   829,   835,   837,   841,   842,   847,   848,
     852,   872,   873,   877,   878,   882,   883,   887,   896,   897,
     901,   902,   911,   926,   930,   931,   935,   936,   940,   941,
     945,   950,   954,   958,   959,   963,   968,   969,   973,   975,
     977,   979,   981
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
  "T_Enable", "T_End", "T_False", "T_File", "T_Filegen", "T_Flag1",
  "T_Flag2", "T_Flag3", "T_Flag4", "T_Flake", "T_Floor", "T_Freq",
  "T_Fudge", "T_Host", "T_Huffpuff", "T_Iburst", "T_Ident", "T_Ignore",
  "T_Includefile", "T_Integer", "T_Interface", "T_Ipv4_flag",
  "T_Ipv6_flag", "T_Kernel", "T_Key", "T_Keys", "T_Keysdir", "T_Kod",
  "T_Mssntp", "T_Leapfile", "T_Limited", "T_Link", "T_Logconfig",
  "T_Logfile", "T_Loopstats", "T_Lowpriotrap", "T_Manycastclient",
  "T_Manycastserver", "T_Mask", "T_Maxclock", "T_Maxdist", "T_Maxhop",
  "T_Maxpoll", "T_Minclock", "T_Mindist", "T_Minimum", "T_Minpoll",
  "T_Minsane", "T_Mode", "T_Monitor", "T_Month", "T_Multicastclient",
  "T_Nolink", "T_Nomodify", "T_None", "T_Nopeer", "T_Noquery",
  "T_Noselect", "T_Noserve", "T_Notrap", "T_Notrust", "T_Ntp", "T_Ntpport",
  "T_NtpSignDsocket", "T_Orphan", "T_Panic", "T_Peer", "T_Peerstats",
  "T_Phone", "T_Pid", "T_Pidfile", "T_Pool", "T_Port", "T_Preempt",
  "T_Prefer", "T_Protostats", "T_Pw", "T_Qos", "T_RandFile", "T_Rawstats",
  "T_Refid", "T_Requestkey", "T_Restrict", "T_Revoke", "T_Server",
  "T_Setvar", "T_Sign", "T_Statistics", "T_Stats", "T_Statsdir", "T_Step",
  "T_Stepout", "T_Stratum", "T_String", "T_Sysstats", "T_Tick", "T_Time1",
  "T_Time2", "T_Timingstats", "T_Tinker", "T_Tos", "T_Trap", "T_True",
  "T_Trustedkey", "T_Ttl", "T_Type", "T_Unconfig", "T_Unpeer", "T_Version",
  "T_WanderThreshold", "T_Week", "T_Xleave", "T_Year", "T_Flag", "T_Void",
  "T_EOC", "T_Simulate", "T_Beep_Delay", "T_Sim_Duration",
  "T_Server_Offset", "T_Duration", "T_Freq_Offset", "T_Wander", "T_Jitter",
  "T_Prop_Delay", "T_Proc_Delay", "'='", "'{'", "'}'", "$accept",
  "configuration", "command_list", "command", "server_command",
  "client_type", "address", "ip_address", "option_list", "option",
  "unpeer_command", "unpeer_keyword", "other_mode_command",
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
  "log_config_command", "integer_list", "string_list", "address_list",
  "boolean", "number", "simulate_command", "sim_conf_start",
  "sim_init_statement_list", "sim_init_statement", "sim_server_list",
  "sim_server", "sim_server_offset", "sim_server_name", "sim_act_list",
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
     405,   406,   407,   408,   409,   410,   411,    61,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   160,   161,   162,   162,   162,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   164,
     164,   165,   165,   165,   165,   165,   166,   166,   166,   167,
     168,   168,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   170,   171,   171,
     172,   172,   172,   173,   173,   173,   173,   173,   173,   173,
     173,   174,   174,   175,   175,   176,   176,   176,   176,   176,
     176,   177,   178,   178,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   180,   180,   180,   181,   181,
     182,   182,   182,   182,   182,   182,   182,   182,   183,   183,
     184,   184,   184,   184,   184,   184,   185,   185,   185,   185,
     185,   185,   185,   186,   186,   186,   186,   186,   186,   187,
     187,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   189,   189,   190,   190,   190,
     191,   192,   192,   193,   193,   193,   193,   193,   193,   193,
     193,   194,   194,   195,   195,   196,   196,   196,   196,   196,
     196,   196,   197,   198,   198,   199,   199,   199,   199,   199,
     199,   199,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   201,
     201,   201,   202,   202,   203,   203,   204,   204,   205,   205,
     206,   207,   207,   208,   208,   209,   209,   210,   210,   210,
     211,   211,   212,   213,   214,   214,   215,   215,   216,   216,
     217,   218,   219,   220,   220,   221,   222,   222,   223,   223,
     223,   223,   223
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     1,     1,     1,     1,     1,     1,     2,     2,     1,
       2,     1,     1,     2,     1,     1,     2,     2,     2,     1,
       1,     1,     1,     1,     2,     2,     2,     2,     1,     1,
       1,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     0,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     4,     4,     5,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     2,     2,     2,
       3,     2,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     3,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     3,     2,     2,     1,
       2,     0,     4,     3,     2,     1,     2,     2,     2,     1,
       1,     2,     1,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     5,     1,     3,     2,     3,     3,     2,     1,
       5,     4,     3,     2,     1,     6,     3,     2,     3,     3,
       3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,    24,    50,     0,     0,     0,    62,
       0,     0,   191,     0,   173,     0,     0,     0,     0,     0,
       0,     0,     0,    25,     0,     0,     0,    23,     0,     0,
      22,     0,     0,     0,    21,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,    49,   213,     0,     2,     0,
       7,     0,     8,     0,     9,    10,    13,    11,    12,    14,
      15,    16,    17,    18,     0,     5,    53,   181,   211,   210,
     174,   175,    54,     0,     0,     0,     0,     0,     0,    55,
      61,    64,   155,   156,   157,   158,   159,   160,   161,   152,
     154,     0,     0,     0,   113,   136,   189,   177,   151,    90,
      91,    92,    93,    97,    94,    95,    96,     0,     0,     0,
      29,     0,    26,     6,    56,    57,   178,   200,   182,   199,
     180,   206,    51,    52,    60,   204,   183,   179,   188,    58,
     119,     0,     0,   119,    26,     0,   184,    85,    89,    86,
     176,     0,     0,     0,     0,     0,     0,     0,   162,   164,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    71,    73,   185,   202,    59,   187,     1,     0,     4,
      20,    47,     0,    65,    66,    67,    68,    69,    70,    63,
     153,   137,   138,   139,   135,   190,   105,   104,     0,   102,
     103,     0,    87,    99,    27,    28,     0,     0,     0,     0,
       0,     0,     0,     0,   140,   142,   172,   198,   205,   203,
     115,   119,   119,   114,     0,     0,    88,   165,   166,   167,
     168,   169,   170,   171,   163,    83,    74,   209,   207,   208,
      76,    75,    81,    79,    84,    80,    78,    82,    77,    72,
       0,     0,   186,   195,   201,     3,    32,     0,    34,    35,
       0,     0,     0,     0,    39,    40,    41,    42,     0,     0,
      43,    19,    31,     0,     0,     0,     0,   100,   112,   108,
     110,   106,   107,   109,   111,   101,    98,   147,   148,   149,
     150,   146,   145,   143,   144,   141,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     120,   116,   117,   119,   193,   197,   196,   194,    33,    36,
      38,    37,    45,    44,    46,    30,     0,     0,     0,     0,
       0,   219,     0,   215,   118,   192,   216,   217,     0,   214,
     212,   218,     0,   222,     0,     0,     0,     0,     0,   224,
       0,     0,   220,   223,   221,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   225,
       0,   227,   228,   229,   230,   231,   232,   226
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    47,    48,    49,    50,    51,   121,   112,   261,   262,
      52,    53,    54,    55,    79,    80,    81,    56,   161,   162,
      57,   137,   107,   192,   193,   275,    58,   210,   300,    94,
      95,    59,   204,   205,    60,    89,    90,    61,   148,   149,
      62,    97,   136,   242,   243,   118,   119,   165,   126,   122,
     230,    70,    63,    64,   265,   266,   320,   321,   335,   322,
     338,   339,   352,   353
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -132
static const yytype_int16 yypact[] =
{
     117,  -122,   -25,   -19,  -132,  -132,   -13,   -14,   -12,   -30,
     185,    14,   -84,   185,  -132,   215,   -48,   -83,   -80,   -79,
     -78,   -76,   -75,  -132,   -48,   -48,   -69,  -132,   -67,   -65,
    -132,   -62,    -8,     1,  -132,   -61,   215,   -59,   -13,    19,
     420,   -56,     6,     6,  -132,  -132,  -132,    69,   279,   -71,
    -132,   -48,  -132,   -48,  -132,  -132,  -132,  -132,  -132,  -132,
    -132,  -132,  -132,  -132,   -88,  -132,  -132,  -132,  -132,  -132,
    -132,  -132,  -132,   -46,   -42,   -41,   -35,    36,   -10,  -132,
     -30,  -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,   185,
    -132,    42,    46,    83,    14,  -132,   103,  -132,   185,  -132,
    -132,  -132,  -132,  -132,  -132,  -132,  -132,   -23,    11,    12,
    -132,    35,  -132,   415,  -132,  -132,  -132,  -132,   -76,  -132,
    -132,  -132,   -48,   -48,  -132,  -132,    18,  -132,  -132,  -132,
    -132,     2,     3,  -132,    76,    -7,  -132,   215,  -132,  -132,
    -132,   -13,   -13,   -13,   -13,   -13,   -13,   -13,    19,  -132,
     104,   106,   -21,   107,   -13,   -13,   118,   -13,   -13,   119,
     121,   420,  -132,   -45,  -132,   125,   125,  -132,    21,  -132,
      82,  -132,  -130,  -132,  -132,  -132,  -132,  -132,  -132,  -132,
    -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,    52,  -132,
    -132,    10,   -23,  -132,  -132,  -132,   -21,   -21,   -21,   -21,
      54,   129,   -13,   -13,    35,  -132,  -132,  -132,  -132,  -132,
     500,  -132,  -132,   500,   -56,    59,  -132,  -132,  -132,  -132,
    -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,
    -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,
     -56,   138,   -45,  -132,  -132,  -132,  -132,   -13,  -132,  -132,
     139,   141,   142,   143,  -132,  -132,  -132,  -132,   145,   146,
    -132,    82,  -132,    40,    41,   -82,    56,  -132,  -132,  -132,
    -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,
    -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,
    -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,  -132,
    -132,   500,   500,  -132,   175,  -132,  -132,  -132,  -132,  -132,
    -132,  -132,  -132,  -132,  -132,  -132,   -13,   -13,    55,    67,
    -108,  -132,    62,  -132,   500,  -132,  -132,  -132,   -48,  -132,
    -132,  -132,    72,  -132,    66,    75,   -13,    70,  -116,  -132,
      85,   -13,  -132,  -132,  -132,    78,   -54,    71,    84,    87,
      88,    89,    51,    94,   -13,   -13,   -13,   -13,   -13,  -132,
      96,  -132,  -132,  -132,  -132,  -132,  -132,  -132
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -132,  -132,  -132,   -28,  -132,  -132,    -1,   -32,  -132,   -11,
    -132,  -132,  -132,  -132,  -132,  -132,   173,  -132,  -132,    99,
    -132,  -132,   -26,  -132,    74,  -132,  -132,  -131,  -132,  -132,
     167,  -132,  -132,    58,  -132,   254,   -68,  -132,  -132,   120,
    -132,  -132,  -132,  -132,    27,  -132,   152,   228,  -132,   247,
      60,   -38,  -132,  -132,  -132,     8,  -132,   -44,  -132,  -132,
    -132,   -64,  -132,   -77
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     140,   134,   213,   186,   108,   109,   240,   318,   187,   163,
     138,   188,   227,   268,    73,   111,    68,    74,   263,   264,
     168,   180,    91,   141,    65,    66,   130,   211,   212,   228,
     180,    67,   133,   318,   269,   337,    71,    69,    72,   189,
      96,   113,   129,   342,   114,   115,   116,   142,   117,   120,
     170,   330,   171,   131,   132,   124,   164,   125,   241,   127,
     190,   143,   128,   135,   144,   139,   263,   264,   110,   167,
     172,   196,   197,   198,   199,   169,   110,    75,   173,    76,
     301,   302,   174,   175,    77,   206,   177,    78,   246,   176,
      92,   270,   181,   247,    93,   271,   182,   248,   347,   348,
     349,   350,   351,   217,   218,   219,   220,   221,   222,   223,
     272,   216,   229,   191,   178,   145,   232,   233,     1,   235,
     236,   208,   208,     2,     3,   110,   194,   195,   249,     4,
       5,     6,   185,   183,     7,   194,   195,   250,     8,     9,
     146,   147,   209,    10,    11,   214,   200,    12,    13,    14,
     215,   273,    15,   274,   225,   251,   226,   231,   201,   252,
      16,   253,   202,   203,   283,   284,    17,   245,   234,   237,
     254,   238,   324,    18,    19,   244,   267,    20,   281,   282,
      21,    22,   303,   304,    23,    24,   255,   256,   306,   309,
      82,   310,   311,   312,    83,   313,   314,   316,   317,    25,
     325,    84,   323,   347,   348,   349,   350,   351,   305,   308,
     359,    26,   328,   329,    27,   257,    28,   258,    29,    30,
     332,   259,   334,   336,   260,    31,   337,   341,   354,    32,
      33,   344,    34,    35,    99,    36,   346,    37,   100,    85,
     361,   355,   367,    38,   356,   357,   358,    39,    40,    41,
     315,    42,    43,   179,    44,    45,   277,   278,   279,   280,
     239,   184,   285,    -6,    46,    86,   276,    98,   224,   307,
     207,   166,   123,   319,   343,   360,   331,    87,   326,   327,
     101,     0,     0,     0,     0,     2,     3,     0,     0,     0,
       0,     4,     5,     6,     0,     0,     7,     0,   340,     0,
       8,     9,     0,   345,    88,    10,    11,     0,     0,    12,
      13,    14,     0,   102,    15,     0,   362,   363,   364,   365,
     366,   103,    16,     0,     0,   104,     0,   333,    17,     0,
       0,     0,     0,     0,     0,    18,    19,     0,     0,    20,
     105,     0,    21,    22,   106,     0,    23,    24,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    26,     0,     0,    27,     0,    28,     0,
      29,    30,     0,     0,     0,     0,     0,    31,     0,     0,
       0,    32,    33,     0,    34,    35,     0,    36,     0,    37,
       0,     0,     0,     0,     0,    38,     0,     0,     0,    39,
      40,    41,     0,    42,    43,     0,    44,    45,     0,     0,
       0,     2,     3,     0,     0,    -6,    46,     4,     5,     6,
     150,     0,     7,     0,     0,     0,     8,     9,   151,     0,
     152,    10,    11,     0,     0,    12,    13,    14,     0,     0,
      15,     0,     0,     0,     0,     0,     0,     0,    16,     0,
       0,   153,     0,     0,    17,     0,     0,     0,     0,     0,
       0,    18,    19,     0,     0,    20,     0,     0,    21,    22,
       0,     0,    23,    24,     0,     0,     0,     0,     0,     0,
     154,   155,   156,     0,   157,   158,     0,    25,   159,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    26,
       0,     0,    27,     0,    28,   160,    29,    30,     0,     0,
       0,     0,     0,    31,     0,     0,     0,    32,    33,     0,
      34,    35,     0,    36,     0,    37,     0,     0,     0,     0,
     286,    38,     0,     0,     0,    39,    40,    41,   287,    42,
      43,     0,    44,    45,     0,     0,     0,     0,   288,   289,
       0,   290,    46,     0,     0,     0,   291,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   292,     0,   293,   294,     0,   295,
     296,   297,     0,   298,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   299
};

static const yytype_int16 yycheck[] =
{
      38,    33,   133,    26,    52,    53,    51,   115,    31,    41,
      36,    34,    33,     3,    44,    16,    29,    47,   148,   149,
      48,    89,     8,     4,   146,    50,    25,    25,    25,    50,
      98,    50,    33,   115,    24,   151,    50,    50,    50,    62,
     124,   124,    50,   159,   124,   124,   124,    28,   124,   124,
      51,   159,    53,    52,    53,   124,    50,   124,   103,   124,
      83,    42,   124,   124,    45,   124,   148,   149,   124,     0,
     158,    36,    37,    38,    39,   146,   124,   107,   124,   109,
     211,   212,   124,   124,   114,   113,    50,   117,     6,   124,
      76,    81,    50,    11,    80,    85,    50,    15,   152,   153,
     154,   155,   156,   141,   142,   143,   144,   145,   146,   147,
     100,   137,   133,   136,   124,    96,   154,   155,     1,   157,
     158,   122,   123,     6,     7,   124,   124,   124,    46,    12,
      13,    14,    29,    50,    17,   124,   124,    55,    21,    22,
     121,   122,   124,    26,    27,    69,   111,    30,    31,    32,
     157,   141,    35,   143,    50,    73,    50,    50,   123,    77,
      43,    79,   127,   128,   202,   203,    49,   146,    50,    50,
      88,    50,   303,    56,    57,    50,   124,    60,   124,    50,
      63,    64,   214,   124,    67,    68,   104,   105,    50,    50,
       5,    50,    50,    50,     9,    50,    50,   157,   157,    82,
      25,    16,   146,   152,   153,   154,   155,   156,   240,   247,
     159,    94,   157,   146,    97,   133,    99,   135,   101,   102,
     158,   139,   150,   157,   142,   108,   151,   157,   157,   112,
     113,   146,   115,   116,    19,   118,   158,   120,    23,    54,
     146,   157,   146,   126,   157,   157,   157,   130,   131,   132,
     261,   134,   135,    80,   137,   138,   196,   197,   198,   199,
     161,    94,   204,   146,   147,    80,   192,    13,   148,   242,
     118,    43,    25,   265,   338,   352,   320,    92,   316,   317,
      65,    -1,    -1,    -1,    -1,     6,     7,    -1,    -1,    -1,
      -1,    12,    13,    14,    -1,    -1,    17,    -1,   336,    -1,
      21,    22,    -1,   341,   119,    26,    27,    -1,    -1,    30,
      31,    32,    -1,    98,    35,    -1,   354,   355,   356,   357,
     358,   106,    43,    -1,    -1,   110,    -1,   328,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    56,    57,    -1,    -1,    60,
     125,    -1,    63,    64,   129,    -1,    67,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    97,    -1,    99,    -1,
     101,   102,    -1,    -1,    -1,    -1,    -1,   108,    -1,    -1,
      -1,   112,   113,    -1,   115,   116,    -1,   118,    -1,   120,
      -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,   130,
     131,   132,    -1,   134,   135,    -1,   137,   138,    -1,    -1,
      -1,     6,     7,    -1,    -1,   146,   147,    12,    13,    14,
      10,    -1,    17,    -1,    -1,    -1,    21,    22,    18,    -1,
      20,    26,    27,    -1,    -1,    30,    31,    32,    -1,    -1,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,
      -1,    41,    -1,    -1,    49,    -1,    -1,    -1,    -1,    -1,
      -1,    56,    57,    -1,    -1,    60,    -1,    -1,    63,    64,
      -1,    -1,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    71,    72,    -1,    74,    75,    -1,    82,    78,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,
      -1,    -1,    97,    -1,    99,    95,   101,   102,    -1,    -1,
      -1,    -1,    -1,   108,    -1,    -1,    -1,   112,   113,    -1,
     115,   116,    -1,   118,    -1,   120,    -1,    -1,    -1,    -1,
      40,   126,    -1,    -1,    -1,   130,   131,   132,    48,   134,
     135,    -1,   137,   138,    -1,    -1,    -1,    -1,    58,    59,
      -1,    61,   147,    -1,    -1,    -1,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    86,    87,    -1,    89,
      90,    91,    -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   139
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     6,     7,    12,    13,    14,    17,    21,    22,
      26,    27,    30,    31,    32,    35,    43,    49,    56,    57,
      60,    63,    64,    67,    68,    82,    94,    97,    99,   101,
     102,   108,   112,   113,   115,   116,   118,   120,   126,   130,
     131,   132,   134,   135,   137,   138,   147,   161,   162,   163,
     164,   165,   170,   171,   172,   173,   177,   180,   186,   191,
     194,   197,   200,   212,   213,   146,    50,    50,    29,    50,
     211,    50,    50,    44,    47,   107,   109,   114,   117,   174,
     175,   176,     5,     9,    16,    54,    80,    92,   119,   195,
     196,     8,    76,    80,   189,   190,   124,   201,   195,    19,
      23,    65,    98,   106,   110,   125,   129,   182,    52,    53,
     124,   166,   167,   124,   124,   124,   124,   124,   205,   206,
     124,   166,   209,   209,   124,   124,   208,   124,   124,    50,
      25,    52,    53,   166,   167,   124,   202,   181,   182,   124,
     211,     4,    28,    42,    45,    96,   121,   122,   198,   199,
      10,    18,    20,    41,    70,    71,    72,    74,    75,    78,
      95,   178,   179,   167,    50,   207,   207,     0,   163,   146,
     166,   166,   158,   124,   124,   124,   124,    50,   124,   176,
     196,    50,    50,    50,   190,    29,    26,    31,    34,    62,
      83,   136,   183,   184,   124,   124,    36,    37,    38,    39,
     111,   123,   127,   128,   192,   193,   163,   206,   166,   124,
     187,    25,    25,   187,    69,   157,   182,   211,   211,   211,
     211,   211,   211,   211,   199,    50,    50,    33,    50,   133,
     210,    50,   211,   211,    50,   211,   211,    50,    50,   179,
      51,   103,   203,   204,    50,   146,     6,    11,    15,    46,
      55,    73,    77,    79,    88,   104,   105,   133,   135,   139,
     142,   168,   169,   148,   149,   214,   215,   124,     3,    24,
      81,    85,   100,   141,   143,   185,   184,   210,   210,   210,
     210,   124,    50,   211,   211,   193,    40,    48,    58,    59,
      61,    66,    84,    86,    87,    89,    90,    91,    93,   139,
     188,   187,   187,   167,   124,   167,    50,   204,   211,    50,
      50,    50,    50,    50,    50,   169,   157,   157,   115,   215,
     216,   217,   219,   146,   187,    25,   211,   211,   157,   146,
     159,   217,   158,   166,   150,   218,   157,   151,   220,   221,
     211,   157,   159,   221,   146,   211,   158,   152,   153,   154,
     155,   156,   222,   223,   157,   157,   157,   157,   157,   159,
     223,   146,   211,   211,   211,   211,   211,   146
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

/* Line 1455 of yacc.c  */
#line 292 "ntp_parser.y"
    {
			/* I will need to incorporate much more fine grained
			 * error messages. The following should suffice for
			 * the time being.  ip_file->col_no is always 1 here,
			 * and ip_file->line_no is one higher than the
			 * problem line.  In other words, the scanner has
			 * moved on to the start of the next line.
			 */
			if (input_from_file == 1) {
				msyslog(LOG_ERR, 
					"syntax error in %s line %d, "
					"ignored",
					ip_file->fname,
					ip_file->line_no -
						(ip_file->col_no == 1)
						    ? 1
						    : 2);
			} else if (input_from_file != 0)
				msyslog(LOG_ERR,
					"parse: bad boolean input flag");
	}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 336 "ntp_parser.y"
    {
			struct peer_node *my_node =  create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue));
			if (my_node)
				enqueue(cfgt.peers, my_node);
		}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 342 "ntp_parser.y"
    {
			struct peer_node *my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
			if (my_node)
				enqueue(cfgt.peers, my_node);
		}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 359 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 360 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET6); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 364 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), 0); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 368 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 369 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 373 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_SKEY); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 374 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 375 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_BURST); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 376 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_IBURST); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 377 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 378 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 379 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 380 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_NOSELECT); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 381 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREEMPT); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 382 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREFER); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 383 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_TRUE); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 384 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_XLEAVE); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 385 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 386 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 387 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 397 "ntp_parser.y"
    {
			struct unpeer_node *my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				enqueue(cfgt.unpeers, my_node);
		}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 416 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 418 "ntp_parser.y"
    { append_queue(cfgt.manycastserver, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 420 "ntp_parser.y"
    { append_queue(cfgt.multicastclient, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 431 "ntp_parser.y"
    { cfgt.auth.autokey = (yyvsp[(2) - (2)].Integer); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 433 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 435 "ntp_parser.y"
    { 
			if (cfgt.auth.crypto_cmd_list != NULL)
				append_queue(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Queue));
			else
				cfgt.auth.crypto_cmd_list = (yyvsp[(2) - (2)].Queue);
			cryptosw++;
		}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 443 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 445 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 447 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 449 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Queue); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 451 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 457 "ntp_parser.y"
    { (yyval.Queue) = NULL; }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 461 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 462 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 467 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PRIV, (yyvsp[(2) - (2)].String)); }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 469 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_IDENT, (yyvsp[(2) - (2)].String)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 471 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PW, (yyvsp[(2) - (2)].String)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 473 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_RAND, (yyvsp[(2) - (2)].String)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 475 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 477 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_SIGN, (yyvsp[(2) - (2)].String)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 487 "ntp_parser.y"
    { append_queue(cfgt.orphan_cmds,(yyvsp[(2) - (2)].Queue)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 491 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 492 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 497 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_CEILING, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 499 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_FLOOR, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 501 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_COHORT, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 503 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_ORPHAN, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 505 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINDISP, (yyvsp[(2) - (2)].Double)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 507 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXDIST, (yyvsp[(2) - (2)].Double)); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 509 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINCLOCK, (yyvsp[(2) - (2)].Double)); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 511 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXCLOCK, (yyvsp[(2) - (2)].Double)); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 513 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINSANE, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 515 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_BEACON, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 517 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXHOP, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 527 "ntp_parser.y"
    { append_queue(cfgt.stats_list, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 529 "ntp_parser.y"
    { cfgt.stats_dir = (yyvsp[(2) - (2)].String); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 531 "ntp_parser.y"
    {
			enqueue(cfgt.filegen_opts,
				create_filegen_node((yyvsp[(2) - (3)].VoidPtr), (yyvsp[(3) - (3)].Queue)));
		}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 538 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 539 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].VoidPtr)); }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 544 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("clockstats"); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 546 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("cryptostats"); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 548 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("loopstats"); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 550 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("peerstats"); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 552 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("rawstats"); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 554 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("sysstats"); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 556 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("timingstats"); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 558 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("protostats"); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 562 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 563 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 568 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(T_File, (yyvsp[(2) - (2)].String)); }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 570 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Type, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 571 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 572 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 573 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 574 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 578 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_NONE; }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 579 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_PID; }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 580 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_DAY; }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 581 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_WEEK; }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 582 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_MONTH; }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 583 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_YEAR; }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 584 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_AGE; }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 594 "ntp_parser.y"
    {   
			append_queue(cfgt.discard_opts, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 598 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 603 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 608 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(
					create_address_node(
						estrdup("0.0.0.0"), 
						AF_INET),
					create_address_node(
						estrdup("255.255.255.255"), 
						AF_INET),
					(yyvsp[(4) - (4)].Queue), 
					ip_file->line_no));
		}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 621 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(
					create_address_node(
						estrdup("::"), 
						AF_INET6),
					create_address_node(
						estrdup("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"),
						AF_INET6),
					(yyvsp[(4) - (4)].Queue), 
					ip_file->line_no));
		}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 634 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Queue), ip_file->line_no));
		}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 641 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 642 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 646 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_TIMEOUT); }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 647 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_IGNORE); }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 648 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_KOD); }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 649 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_MSSNTP); }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 650 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LIMITED); }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 651 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LPTRAP); }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 652 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOMODIFY); }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 653 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOPEER); }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 654 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOQUERY); }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 655 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTSERVE); }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 656 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOTRAP); }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 657 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTTRUST); }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 658 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RESM_NTPONLY); }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 659 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_VERSION); }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 663 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 664 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 668 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 669 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 670 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 679 "ntp_parser.y"
    { enqueue(cfgt.fudge, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 683 "ntp_parser.y"
    { enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 684 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 689 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME1, (yyvsp[(2) - (2)].Double)); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 691 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME2, (yyvsp[(2) - (2)].Double)); }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 693 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEVAL1,  (yyvsp[(2) - (2)].Integer)); }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 695 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CLK_HAVEVAL2,  (yyvsp[(2) - (2)].String)); }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 697 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG1, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 699 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG2, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 701 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG3, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 703 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG4, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 712 "ntp_parser.y"
    { append_queue(cfgt.enable_opts,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 714 "ntp_parser.y"
    { append_queue(cfgt.disable_opts,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 718 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 719 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 723 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 724 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_BROADCLIENT); }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 725 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_CAL); }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 726 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_KERNEL); }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 727 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_MONITOR); }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 728 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_NTP); }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 730 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_FILEGEN); }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 738 "ntp_parser.y"
    { append_queue(cfgt.tinker, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 742 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 743 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 747 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_ALLAN, (yyvsp[(2) - (2)].Double)); }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 748 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PHI, (yyvsp[(2) - (2)].Double)); }
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 749 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_FREQ, (yyvsp[(2) - (2)].Double)); }
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 750 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_HUFFPUFF, (yyvsp[(2) - (2)].Double)); }
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 751 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PANIC, (yyvsp[(2) - (2)].Double)); }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 752 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MAX, (yyvsp[(2) - (2)].Double)); }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 753 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MINSTEP, (yyvsp[(2) - (2)].Double)); }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 762 "ntp_parser.y"
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

  case 173:

/* Line 1455 of yacc.c  */
#line 778 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 784 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double))); }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 786 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer))); }
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 788 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double))); }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 790 "ntp_parser.y"
    { /* Null action, possibly all null parms */ }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 792 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 795 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 797 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 799 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer))); }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 802 "ntp_parser.y"
    { append_queue(cfgt.logconfig, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 804 "ntp_parser.y"
    { append_queue(cfgt.phone, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 806 "ntp_parser.y"
    { enqueue(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 808 "ntp_parser.y"
    { enqueue(cfgt.trap, create_addr_opts_node((yyvsp[(2) - (2)].Address_node), NULL)); }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 810 "ntp_parser.y"
    { enqueue(cfgt.trap, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 812 "ntp_parser.y"
    { append_queue(cfgt.ttl, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 814 "ntp_parser.y"
    { enqueue(cfgt.qos, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 818 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String))); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 820 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double)));
			  enqueue(cfgt.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String))); }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 823 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 828 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), DEF); }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 830 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (3)].String), (yyvsp[(3) - (3)].String), 0); }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 836 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 837 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 841 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 842 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_pval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 847 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 848 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 853 "ntp_parser.y"
    {
			char prefix = (yyvsp[(1) - (1)].String)[0];
			char *type = (yyvsp[(1) - (1)].String) + 1;
			
			if (prefix != '+' && prefix != '-' && prefix != '=') {
				yyerror("Logconfig prefix is not '+', '-' or '='\n");
			}
			else
				(yyval.Attr_val) = create_attr_sval(prefix, estrdup(type));
			YYFREE((yyvsp[(1) - (1)].String));
		}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 872 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 873 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 877 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_pval((yyvsp[(2) - (2)].String))); }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 878 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_pval((yyvsp[(1) - (1)].String))); }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 882 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 883 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Address_node)); }
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 888 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			}
			else
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
		}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 896 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 897 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 901 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 912 "ntp_parser.y"
    {
			cfgt.sim_details = create_sim_node((yyvsp[(3) - (5)].Queue), (yyvsp[(4) - (5)].Queue));

			/* Reset the old_config_style variable */
			old_config_style = 1;
		}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 926 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 930 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 931 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 935 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 936 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 940 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_server)); }
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 941 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_server)); }
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 946 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Queue)); }
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 950 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 954 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 958 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_script)); }
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 959 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_script)); }
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 964 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Queue)); }
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 968 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 969 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 974 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 976 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 978 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 980 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 982 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1455 of yacc.c  */
#line 3689 "ntp_parser.c"
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



/* Line 1675 of yacc.c  */
#line 986 "ntp_parser.y"


void yyerror (char *msg)
{
	int retval;
	
	if (input_from_file)
		msyslog(LOG_ERR, 
			"line %d column %d %s", 
			ip_file->line_no,
			ip_file->prev_token_col_no,
			msg);
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


