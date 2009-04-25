
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

  /*  #include "ntp_parser.h"     SK: Arent't we generating this using bison?
				   This was not an issue when we were
				   directly including the source file. However,
				   we will need a seperate description of the
				   tokens in the scanner.
				*/


  struct FILE_INFO *ip_file;   /* Pointer to the configuration file stream */

  #define YYMALLOC	emalloc
  #define YYFREE	free
  #define YYERROR_VERBOSE
  void yyerror (char *msg);
  extern int input_from_file;  /* 0=input from ntpq>config command buffer */
  extern int cryptosw;


/* Line 189 of yacc.c  */
#line 114 "ntp_parser.c"

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
     T_Unconfig = 395,
     T_Unpeer = 396,
     T_Version = 397,
     T_Week = 398,
     T_Xleave = 399,
     T_Year = 400,
     T_Flag = 401,
     T_Void = 402,
     T_EOC = 403,
     T_Simulate = 404,
     T_Beep_Delay = 405,
     T_Sim_Duration = 406,
     T_Server_Offset = 407,
     T_Duration = 408,
     T_Freq_Offset = 409,
     T_Wander = 410,
     T_Jitter = 411,
     T_Prop_Delay = 412,
     T_Proc_Delay = 413
   };
#endif



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
#line 325 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 337 "ntp_parser.c"

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
#define YYFINAL  166
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   631

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  162
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  229
/* YYNRULES -- Number of states.  */
#define YYNSTATES  362

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   413

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
       2,   159,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   160,     2,   161,     2,     2,     2,     2,
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
     155,   156,   157,   158
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
     145,   148,   151,   153,   154,   157,   159,   162,   165,   168,
     171,   174,   177,   180,   183,   185,   188,   191,   194,   197,
     200,   203,   206,   209,   212,   215,   218,   221,   224,   228,
     231,   233,   235,   237,   239,   241,   243,   245,   247,   249,
     252,   254,   257,   260,   262,   264,   266,   268,   270,   272,
     274,   276,   278,   280,   282,   285,   289,   293,   299,   300,
     303,   305,   307,   309,   311,   313,   315,   317,   319,   321,
     323,   325,   327,   329,   332,   334,   337,   340,   343,   347,
     350,   352,   355,   358,   361,   364,   367,   370,   373,   376,
     379,   382,   385,   387,   389,   391,   393,   395,   397,   399,
     401,   404,   407,   409,   412,   415,   418,   421,   424,   427,
     430,   434,   436,   439,   442,   445,   448,   451,   454,   457,
     460,   463,   466,   469,   473,   476,   479,   481,   484,   485,
     490,   494,   497,   499,   502,   505,   508,   510,   512,   515,
     517,   520,   522,   525,   527,   529,   531,   533,   535,   537,
     543,   545,   549,   552,   556,   560,   563,   565,   571,   576,
     580,   583,   585,   592,   596,   599,   603,   607,   611,   615
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     163,     0,    -1,   164,    -1,   164,   165,   148,    -1,   165,
     148,    -1,     1,   148,    -1,    -1,   166,    -1,   172,    -1,
     174,    -1,   175,    -1,   182,    -1,   188,    -1,   179,    -1,
     193,    -1,   196,    -1,   199,    -1,   202,    -1,   214,    -1,
     167,   168,   170,    -1,   167,   168,    -1,   118,    -1,   104,
      -1,    99,    -1,    12,    -1,    68,    -1,   169,    -1,    47,
     127,    -1,    48,   127,    -1,   127,    -1,   170,   171,    -1,
     171,    -1,     6,    -1,    11,   213,    -1,    15,    -1,    49,
      -1,    56,    53,    -1,    78,    53,    -1,    74,    53,    -1,
      89,    -1,   107,    -1,   108,    -1,   136,    -1,   144,    -1,
     138,    53,    -1,    80,    53,    -1,   142,    53,    -1,   173,
     168,    -1,   140,    -1,   141,    -1,    13,    -1,    13,    93,
      -1,    69,   211,    -1,    83,   211,    -1,     6,    53,    -1,
      21,    53,    -1,    22,   176,    -1,    57,   127,    -1,    58,
     127,    -1,   115,    53,    -1,   137,   209,    -1,    96,   127,
      -1,   177,    -1,    -1,   177,   178,    -1,   178,    -1,    45,
     127,    -1,    50,   127,    -1,   110,   127,    -1,   112,   127,
      -1,   117,    53,    -1,   120,   127,    -1,   134,   180,    -1,
     180,   181,    -1,   181,    -1,    18,    53,    -1,    42,    53,
      -1,    20,   212,    -1,    97,    53,    -1,    76,   213,    -1,
      72,   213,    -1,    75,   213,    -1,    71,   213,    -1,    79,
      53,    -1,    10,    53,    -1,    73,    53,    -1,   121,   183,
      -1,   123,   127,    -1,    36,   184,   185,    -1,   183,   184,
      -1,   184,    -1,    19,    -1,    23,    -1,    66,    -1,   100,
      -1,   113,    -1,   128,    -1,   132,    -1,   109,    -1,   185,
     186,    -1,   186,    -1,    35,   127,    -1,   139,   187,    -1,
      63,    -1,    84,    -1,    32,    -1,    26,    -1,    86,    -1,
     102,    -1,    24,    -1,   143,    -1,    82,    -1,   145,    -1,
       3,    -1,    27,   191,    -1,   116,   168,   189,    -1,   116,
      25,   189,    -1,   116,   169,    70,   169,   189,    -1,    -1,
     189,   190,    -1,    41,    -1,    51,    -1,    59,    -1,    62,
      -1,    67,    -1,    85,    -1,    87,    -1,    88,    -1,    90,
      -1,    91,    -1,    92,    -1,    95,    -1,   142,    -1,   191,
     192,    -1,   192,    -1,     8,    53,    -1,    77,    53,    -1,
      81,    53,    -1,    44,   168,   194,    -1,   194,   195,    -1,
     195,    -1,   130,   213,    -1,   131,   213,    -1,   126,    53,
      -1,   114,   127,    -1,    37,   212,    -1,    38,   212,    -1,
      39,   212,    -1,    40,   212,    -1,    32,   197,    -1,    26,
     197,    -1,   197,   198,    -1,   198,    -1,     5,    -1,     9,
      -1,    16,    -1,    55,    -1,    81,    -1,    94,    -1,   122,
      -1,   133,   200,    -1,   200,   201,    -1,   201,    -1,     4,
     213,    -1,    28,   213,    -1,    43,   213,    -1,    46,   213,
      -1,    98,   213,    -1,   124,   213,    -1,   125,   213,    -1,
      52,   127,   165,    -1,    33,    -1,    14,   213,    -1,    17,
      53,    -1,   129,   213,    -1,    30,   203,    -1,    61,   127,
      -1,   103,   127,    -1,    65,   127,    -1,     7,    53,    -1,
      64,   207,    -1,   101,   210,    -1,   119,   204,    -1,   135,
     169,   205,    -1,   138,   209,    -1,   111,   127,    -1,   127,
      -1,   127,    29,    -1,    -1,   127,   159,   127,    25,    -1,
     127,   159,   127,    -1,   205,   206,    -1,   206,    -1,   105,
      53,    -1,    54,   169,    -1,   207,   208,    -1,   208,    -1,
     127,    -1,   209,    53,    -1,    53,    -1,   210,   127,    -1,
     127,    -1,   211,   168,    -1,   168,    -1,    53,    -1,   136,
      -1,    34,    -1,    53,    -1,    29,    -1,   215,   160,   216,
     218,   161,    -1,   149,    -1,   216,   217,   148,    -1,   217,
     148,    -1,   150,   159,   213,    -1,   151,   159,   213,    -1,
     218,   219,    -1,   219,    -1,   221,   160,   220,   222,   161,
      -1,   152,   159,   213,   148,    -1,   118,   159,   168,    -1,
     222,   223,    -1,   223,    -1,   153,   159,   213,   160,   224,
     161,    -1,   224,   225,   148,    -1,   225,   148,    -1,   154,
     159,   213,    -1,   155,   159,   213,    -1,   156,   159,   213,
      -1,   157,   159,   213,    -1,   158,   159,   213,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   287,   287,   291,   292,   293,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   328,
     334,   343,   344,   345,   346,   347,   351,   352,   353,   357,
     361,   362,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   390,   398,   399,
     409,   411,   413,   415,   426,   428,   430,   438,   440,   442,
     444,   446,   451,   453,   457,   458,   462,   464,   466,   468,
     470,   472,   482,   487,   488,   492,   494,   496,   498,   500,
     502,   504,   506,   508,   510,   512,   522,   524,   526,   534,
     535,   539,   541,   543,   545,   547,   549,   551,   553,   558,
     559,   563,   565,   567,   568,   569,   570,   574,   575,   576,
     577,   578,   579,   580,   589,   593,   598,   603,   611,   612,
     616,   617,   618,   619,   620,   621,   622,   623,   624,   625,
     626,   627,   628,   632,   633,   637,   638,   639,   647,   652,
     653,   657,   659,   661,   663,   665,   667,   669,   671,   680,
     682,   687,   688,   692,   693,   694,   695,   696,   697,   699,
     707,   711,   712,   716,   717,   718,   719,   720,   721,   722,
     730,   746,   752,   754,   756,   758,   760,   763,   765,   767,
     770,   772,   774,   776,   778,   780,   784,   786,   790,   794,
     796,   802,   803,   807,   808,   813,   814,   818,   837,   838,
     842,   843,   847,   848,   852,   861,   862,   866,   867,   875,
     890,   894,   895,   899,   900,   904,   905,   909,   914,   918,
     922,   923,   927,   932,   933,   937,   939,   941,   943,   945
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
  "T_Ttl", "T_Type", "T_Unconfig", "T_Unpeer", "T_Version", "T_Week",
  "T_Xleave", "T_Year", "T_Flag", "T_Void", "T_EOC", "T_Simulate",
  "T_Beep_Delay", "T_Sim_Duration", "T_Server_Offset", "T_Duration",
  "T_Freq_Offset", "T_Wander", "T_Jitter", "T_Prop_Delay", "T_Proc_Delay",
  "'='", "'{'", "'}'", "$accept", "configuration", "command_list",
  "command", "server_command", "client_type", "address", "ip_address",
  "option_list", "option", "unpeer_command", "unpeer_keyword",
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
     405,   406,   407,   408,   409,   410,   411,   412,   413,    61,
     123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   162,   163,   164,   164,   164,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   166,
     166,   167,   167,   167,   167,   167,   168,   168,   168,   169,
     170,   170,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   172,   173,   173,
     174,   174,   174,   174,   175,   175,   175,   175,   175,   175,
     175,   175,   176,   176,   177,   177,   178,   178,   178,   178,
     178,   178,   179,   180,   180,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   182,   182,   182,   183,
     183,   184,   184,   184,   184,   184,   184,   184,   184,   185,
     185,   186,   186,   186,   186,   186,   186,   187,   187,   187,
     187,   187,   187,   187,   188,   188,   188,   188,   189,   189,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   191,   191,   192,   192,   192,   193,   194,
     194,   195,   195,   195,   195,   195,   195,   195,   195,   196,
     196,   197,   197,   198,   198,   198,   198,   198,   198,   198,
     199,   200,   200,   201,   201,   201,   201,   201,   201,   201,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   203,   203,   203,   204,
     204,   205,   205,   206,   206,   207,   207,   208,   209,   209,
     210,   210,   211,   211,   212,   212,   212,   213,   213,   214,
     215,   216,   216,   217,   217,   218,   218,   219,   220,   221,
     222,   222,   223,   224,   224,   225,   225,   225,   225,   225
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
       2,     2,     1,     0,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     3,     3,     5,     0,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     2,     2,     2,     3,     2,
       1,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     1,     2,     2,     2,     2,     2,     2,     2,
       3,     1,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     3,     2,     2,     1,     2,     0,     4,
       3,     2,     1,     2,     2,     2,     1,     1,     2,     1,
       2,     1,     2,     1,     1,     1,     1,     1,     1,     5,
       1,     3,     2,     3,     3,     2,     1,     5,     4,     3,
       2,     1,     6,     3,     2,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,    24,    50,     0,     0,     0,    63,
       0,     0,   188,     0,   171,     0,     0,     0,     0,     0,
       0,     0,     0,    25,     0,     0,     0,    23,     0,     0,
      22,     0,     0,     0,    21,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,    49,   210,     0,     2,     0,
       7,     0,     8,     0,     9,    10,    13,    11,    12,    14,
      15,    16,    17,    18,     0,     5,    54,   179,    51,   208,
     207,   172,   173,    55,     0,     0,     0,     0,     0,     0,
      56,    62,    65,   153,   154,   155,   156,   157,   158,   159,
     150,   152,     0,     0,     0,   114,   134,   186,   175,   149,
      91,    92,    93,    94,    98,    95,    96,    97,     0,     0,
       0,    29,     0,    26,     6,    57,    58,   176,   197,   180,
     196,   178,   203,    52,    53,    61,   201,   181,   177,   185,
      59,   118,   118,    26,     0,   182,    86,    90,    87,   174,
       0,     0,     0,     0,     0,     0,     0,   160,   162,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      72,    74,     0,   199,    60,   184,     1,     0,     4,    20,
      47,     0,    66,    67,    68,    69,    70,    71,    64,   151,
     135,   136,   137,   133,   187,   106,   105,     0,   103,   104,
       0,    88,   100,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,   138,   140,   170,   195,   202,   200,   116,
     115,     0,     0,    89,   163,   164,   165,   166,   167,   168,
     169,   161,    84,    75,   206,   204,   205,    77,    76,    82,
      80,    85,    81,    79,    83,    78,    73,     0,     0,   183,
     192,   198,     3,    32,     0,    34,    35,     0,     0,     0,
       0,    39,    40,    41,    42,     0,     0,    43,    19,    31,
       0,     0,     0,     0,   101,   113,   109,   111,   107,   108,
     110,   112,   102,    99,   145,   146,   147,   148,   144,   143,
     141,   142,   139,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   119,   118,   190,   194,
     193,   191,    33,    36,    38,    37,    45,    44,    46,    30,
       0,     0,     0,     0,     0,   216,     0,   212,   117,   189,
     213,   214,     0,   211,   209,   215,     0,   219,     0,     0,
       0,     0,     0,   221,     0,     0,   217,   220,   218,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   222,     0,   224,   225,   226,   227,   228,
     229,   223
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    47,    48,    49,    50,    51,   122,   113,   258,   259,
      52,    53,    54,    55,    80,    81,    82,    56,   160,   161,
      57,   136,   108,   191,   192,   272,    58,   209,   296,    95,
      96,    59,   203,   204,    60,    90,    91,    61,   147,   148,
      62,    98,   135,   239,   240,   119,   120,   164,   127,   123,
     227,    71,    63,    64,   262,   263,   314,   315,   329,   316,
     332,   333,   346,   347
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -135
static const yytype_int16 yypact[] =
{
     126,  -134,   -26,   -22,  -135,   -73,    -1,   -19,    -7,   -33,
       2,    16,   -79,     2,  -135,    22,     3,   -69,   -62,   -61,
     -59,   -58,   -56,  -135,     3,     3,   -55,  -135,   -54,   -52,
    -135,   -51,    25,   -15,  -135,   -47,    22,   -45,    -1,     1,
     278,   -42,    28,    28,  -135,  -135,  -135,    54,   273,   -53,
    -135,     3,  -135,     3,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,   -74,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,   -27,   -16,    -6,     7,    48,     9,
    -135,   -33,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
       2,  -135,    64,    67,    75,    16,  -135,   108,  -135,     2,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,   -10,    14,
      15,  -135,   213,  -135,   411,  -135,  -135,  -135,  -135,   -58,
    -135,  -135,  -135,     3,     3,  -135,  -135,    18,  -135,  -135,
    -135,  -135,  -135,    74,    -8,  -135,    22,  -135,  -135,  -135,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,  -135,    93,
      96,   -13,   107,    -1,    -1,   112,    -1,    -1,   113,   114,
     278,  -135,   -46,  -135,   116,   116,  -135,    13,  -135,   487,
    -135,  -108,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,    49,  -135,  -135,
      12,   -10,  -135,  -135,  -135,   -13,   -13,   -13,   -13,    50,
     127,    -1,    -1,   213,  -135,  -135,  -135,  -135,  -135,   433,
     433,   -42,    55,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,   -42,   128,   -46,
    -135,  -135,  -135,  -135,    -1,  -135,  -135,   132,   133,   135,
     136,  -135,  -135,  -135,  -135,   139,   140,  -135,   487,  -135,
      37,    38,   -95,    51,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,   173,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
      -1,    -1,    41,    53,  -112,  -135,    42,  -135,   433,  -135,
    -135,  -135,     3,  -135,  -135,  -135,    52,  -135,    44,    57,
      -1,    56,  -123,  -135,    59,    -1,  -135,  -135,  -135,    58,
      17,    60,    61,    62,    65,    69,   -94,    63,    -1,    -1,
      -1,    -1,    -1,  -135,    66,  -135,  -135,  -135,  -135,  -135,
    -135,  -135
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -135,  -135,  -135,   -44,  -135,  -135,   -14,   -32,  -135,   -50,
    -135,  -135,  -135,  -135,  -135,  -135,   131,  -135,  -135,    71,
    -135,  -135,   -23,  -135,    26,  -135,  -135,  -129,  -135,  -135,
     118,  -135,  -135,    20,  -135,   203,   -64,  -135,  -135,    79,
    -135,  -135,  -135,  -135,    -5,  -135,   117,   189,  -135,   208,
    -106,   -38,  -135,  -135,  -135,   -24,  -135,   -75,  -135,  -135,
    -135,   -97,  -135,  -103
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     139,   133,   112,   210,   167,   140,   312,    83,   237,   162,
     131,    84,    74,   137,    65,   265,   185,    75,    85,   132,
      68,   224,   186,   312,    92,   187,   179,    66,    69,   141,
     331,    67,   109,   110,    72,   179,   266,   169,   336,   170,
     225,   100,   260,   261,   142,   101,    73,   143,    97,   324,
     109,   110,    70,   188,   166,   260,   261,    86,   114,   238,
     341,   342,   343,   344,   345,   115,   116,   353,   117,   118,
     205,   121,   125,   126,   189,   128,   129,    76,   130,    77,
     134,   163,   138,    87,    78,   111,   171,    79,   102,   274,
     275,   276,   277,    93,   267,   168,    88,    94,   268,   144,
     172,   176,   214,   215,   216,   217,   218,   219,   220,   207,
     207,   173,   111,   213,   269,   229,   230,   180,   232,   233,
     181,   174,   103,   226,    89,   145,   146,     1,   182,   190,
     111,   104,     2,     3,   175,   105,   177,   184,     4,     5,
       6,   193,   194,     7,   211,   208,   222,     8,     9,   223,
     106,   212,    10,    11,   107,   270,    12,   271,    13,    14,
     228,   242,    15,   280,   281,   231,   234,   235,   318,   241,
      16,   341,   342,   343,   344,   345,   264,   278,    17,   297,
     279,   300,   298,    18,    19,   303,   304,    20,   305,   306,
      21,    22,   307,   308,    23,    24,   310,   311,   319,   317,
     322,   323,   326,   330,   328,   299,   302,   338,   309,    25,
     331,   355,   178,   183,   361,   335,    99,   273,   340,   348,
     349,   350,    26,   282,   351,    27,   221,    28,   352,    29,
      30,   236,   165,   124,   301,   337,   206,    31,   313,   325,
       0,    32,    33,   354,    34,    35,     0,    36,     0,    37,
     195,   196,   197,   198,     0,    38,     0,     0,     0,    39,
      40,    41,     0,    42,    43,     0,    44,    45,     0,     0,
       0,     0,   320,   321,    -6,    46,     0,     0,     0,     2,
       3,     0,     0,     0,     0,     4,     5,     6,   149,     0,
       7,     0,   334,     0,     8,     9,   150,   339,   151,    10,
      11,     0,     0,    12,     0,    13,    14,     0,   327,    15,
     356,   357,   358,   359,   360,     0,     0,    16,     0,     0,
     152,     0,     0,     0,     0,    17,     0,   199,     0,     0,
      18,    19,     0,     0,    20,     0,     0,    21,    22,   200,
       0,    23,    24,   201,   202,     0,     0,     0,     0,   153,
     154,   155,     0,   156,   157,     0,    25,   158,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    26,
       0,     0,    27,     0,    28,   159,    29,    30,     0,     0,
       0,     0,     0,     0,    31,     0,     0,     0,    32,    33,
       0,    34,    35,     0,    36,     0,    37,     0,     0,     0,
       0,     0,    38,     0,     0,     0,    39,    40,    41,     0,
      42,    43,     0,    44,    45,     0,     0,     2,     3,     0,
       0,    -6,    46,     4,     5,     6,     0,     0,     7,     0,
       0,     0,     8,     9,     0,     0,     0,    10,    11,     0,
       0,    12,     0,    13,    14,     0,     0,    15,     0,     0,
       0,     0,     0,     0,     0,    16,     0,     0,     0,     0,
       0,     0,     0,    17,     0,     0,     0,     0,    18,    19,
       0,     0,    20,     0,   283,    21,    22,     0,     0,    23,
      24,     0,     0,     0,   284,     0,     0,     0,     0,     0,
       0,     0,   285,   243,    25,   286,     0,     0,   244,     0,
     287,     0,   245,     0,     0,     0,     0,    26,     0,     0,
      27,     0,    28,     0,    29,    30,     0,     0,   288,     0,
     289,   290,    31,   291,   292,   293,    32,    33,   294,    34,
      35,     0,    36,     0,    37,     0,   246,     0,     0,     0,
      38,     0,     0,   247,    39,    40,    41,     0,    42,    43,
       0,    44,    45,     0,     0,     0,     0,     0,     0,     0,
      46,   248,     0,     0,     0,   249,     0,   250,     0,     0,
       0,     0,     0,     0,     0,   295,   251,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   252,   253,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   254,     0,   255,     0,     0,     0,   256,
       0,   257
};

static const yytype_int16 yycheck[] =
{
      38,    33,    16,   132,    48,     4,   118,     5,    54,    41,
      25,     9,    45,    36,   148,     3,    26,    50,    16,    33,
      93,    34,    32,   118,     8,    35,    90,    53,    29,    28,
     153,    53,    47,    48,    53,    99,    24,    51,   161,    53,
      53,    19,   150,   151,    43,    23,    53,    46,   127,   161,
      47,    48,    53,    63,     0,   150,   151,    55,   127,   105,
     154,   155,   156,   157,   158,   127,   127,   161,   127,   127,
     114,   127,   127,   127,    84,   127,   127,   110,    53,   112,
     127,    53,   127,    81,   117,   127,   160,   120,    66,   195,
     196,   197,   198,    77,    82,   148,    94,    81,    86,    98,
     127,    53,   140,   141,   142,   143,   144,   145,   146,   123,
     124,   127,   127,   136,   102,   153,   154,    53,   156,   157,
      53,   127,   100,   136,   122,   124,   125,     1,    53,   139,
     127,   109,     6,     7,   127,   113,   127,    29,    12,    13,
      14,   127,   127,    17,    70,   127,    53,    21,    22,    53,
     128,   159,    26,    27,   132,   143,    30,   145,    32,    33,
      53,   148,    36,   201,   202,    53,    53,    53,   297,    53,
      44,   154,   155,   156,   157,   158,   127,   127,    52,   211,
      53,    53,   127,    57,    58,    53,    53,    61,    53,    53,
      64,    65,    53,    53,    68,    69,   159,   159,    25,   148,
     159,   148,   160,   159,   152,   237,   244,   148,   258,    83,
     153,   148,    81,    95,   148,   159,    13,   191,   160,   159,
     159,   159,    96,   203,   159,    99,   147,   101,   159,   103,
     104,   160,    43,    25,   239,   332,   119,   111,   262,   314,
      -1,   115,   116,   346,   118,   119,    -1,   121,    -1,   123,
      37,    38,    39,    40,    -1,   129,    -1,    -1,    -1,   133,
     134,   135,    -1,   137,   138,    -1,   140,   141,    -1,    -1,
      -1,    -1,   310,   311,   148,   149,    -1,    -1,    -1,     6,
       7,    -1,    -1,    -1,    -1,    12,    13,    14,    10,    -1,
      17,    -1,   330,    -1,    21,    22,    18,   335,    20,    26,
      27,    -1,    -1,    30,    -1,    32,    33,    -1,   322,    36,
     348,   349,   350,   351,   352,    -1,    -1,    44,    -1,    -1,
      42,    -1,    -1,    -1,    -1,    52,    -1,   114,    -1,    -1,
      57,    58,    -1,    -1,    61,    -1,    -1,    64,    65,   126,
      -1,    68,    69,   130,   131,    -1,    -1,    -1,    -1,    71,
      72,    73,    -1,    75,    76,    -1,    83,    79,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    96,
      -1,    -1,    99,    -1,   101,    97,   103,   104,    -1,    -1,
      -1,    -1,    -1,    -1,   111,    -1,    -1,    -1,   115,   116,
      -1,   118,   119,    -1,   121,    -1,   123,    -1,    -1,    -1,
      -1,    -1,   129,    -1,    -1,    -1,   133,   134,   135,    -1,
     137,   138,    -1,   140,   141,    -1,    -1,     6,     7,    -1,
      -1,   148,   149,    12,    13,    14,    -1,    -1,    17,    -1,
      -1,    -1,    21,    22,    -1,    -1,    -1,    26,    27,    -1,
      -1,    30,    -1,    32,    33,    -1,    -1,    36,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    -1,    -1,    -1,    -1,    57,    58,
      -1,    -1,    61,    -1,    41,    64,    65,    -1,    -1,    68,
      69,    -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    59,     6,    83,    62,    -1,    -1,    11,    -1,
      67,    -1,    15,    -1,    -1,    -1,    -1,    96,    -1,    -1,
      99,    -1,   101,    -1,   103,   104,    -1,    -1,    85,    -1,
      87,    88,   111,    90,    91,    92,   115,   116,    95,   118,
     119,    -1,   121,    -1,   123,    -1,    49,    -1,    -1,    -1,
     129,    -1,    -1,    56,   133,   134,   135,    -1,   137,   138,
      -1,   140,   141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     149,    74,    -1,    -1,    -1,    78,    -1,    80,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   142,    89,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   136,    -1,   138,    -1,    -1,    -1,   142,
      -1,   144
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     6,     7,    12,    13,    14,    17,    21,    22,
      26,    27,    30,    32,    33,    36,    44,    52,    57,    58,
      61,    64,    65,    68,    69,    83,    96,    99,   101,   103,
     104,   111,   115,   116,   118,   119,   121,   123,   129,   133,
     134,   135,   137,   138,   140,   141,   149,   163,   164,   165,
     166,   167,   172,   173,   174,   175,   179,   182,   188,   193,
     196,   199,   202,   214,   215,   148,    53,    53,    93,    29,
      53,   213,    53,    53,    45,    50,   110,   112,   117,   120,
     176,   177,   178,     5,     9,    16,    55,    81,    94,   122,
     197,   198,     8,    77,    81,   191,   192,   127,   203,   197,
      19,    23,    66,   100,   109,   113,   128,   132,   184,    47,
      48,   127,   168,   169,   127,   127,   127,   127,   127,   207,
     208,   127,   168,   211,   211,   127,   127,   210,   127,   127,
      53,    25,   168,   169,   127,   204,   183,   184,   127,   213,
       4,    28,    43,    46,    98,   124,   125,   200,   201,    10,
      18,    20,    42,    71,    72,    73,    75,    76,    79,    97,
     180,   181,   169,    53,   209,   209,     0,   165,   148,   168,
     168,   160,   127,   127,   127,   127,    53,   127,   178,   198,
      53,    53,    53,   192,    29,    26,    32,    35,    63,    84,
     139,   185,   186,   127,   127,    37,    38,    39,    40,   114,
     126,   130,   131,   194,   195,   165,   208,   168,   127,   189,
     189,    70,   159,   184,   213,   213,   213,   213,   213,   213,
     213,   201,    53,    53,    34,    53,   136,   212,    53,   213,
     213,    53,   213,   213,    53,    53,   181,    54,   105,   205,
     206,    53,   148,     6,    11,    15,    49,    56,    74,    78,
      80,    89,   107,   108,   136,   138,   142,   144,   170,   171,
     150,   151,   216,   217,   127,     3,    24,    82,    86,   102,
     143,   145,   187,   186,   212,   212,   212,   212,   127,    53,
     213,   213,   195,    41,    51,    59,    62,    67,    85,    87,
      88,    90,    91,    92,    95,   142,   190,   169,   127,   169,
      53,   206,   213,    53,    53,    53,    53,    53,    53,   171,
     159,   159,   118,   217,   218,   219,   221,   148,   189,    25,
     213,   213,   159,   148,   161,   219,   160,   168,   152,   220,
     159,   153,   222,   223,   213,   159,   161,   223,   148,   213,
     160,   154,   155,   156,   157,   158,   224,   225,   159,   159,
     159,   159,   159,   161,   225,   148,   213,   213,   213,   213,
     213,   148
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
#line 294 "ntp_parser.y"
    {
			/* I will need to incorporate much more fine grained
			 * error messages. The following should suffice for
			 * the time being.
			 */
			if (input_from_file == 1) {
				msyslog(LOG_ERR, "parse error %s line %d ignored\n",
					ip_file->fname, ip_file->line_no);
			} else if (input_from_file != 0)
				msyslog(LOG_ERR,
					"parse: bad boolean input flag\n");
	;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 329 "ntp_parser.y"
    {
			struct peer_node *my_node =  create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue));
			if (my_node)
				enqueue(my_config.peers, my_node);
		;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 335 "ntp_parser.y"
    {
			struct peer_node *my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
			if (my_node)
				enqueue(my_config.peers, my_node);
		;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 343 "ntp_parser.y"
    { (yyval.Integer) = T_Server; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 344 "ntp_parser.y"
    { (yyval.Integer) = T_Pool; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 345 "ntp_parser.y"
    { (yyval.Integer) = T_Peer; ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 346 "ntp_parser.y"
    { (yyval.Integer) = T_Broadcast; ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 347 "ntp_parser.y"
    { (yyval.Integer) = T_Manycastclient; ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 351 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(1) - (1)].Address_node); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 352 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 353 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET6); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 357 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), 0); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 361 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 362 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 367 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_SKEY); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 368 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Bias, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 369 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_BURST); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 370 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_IBURST); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 371 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Key, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 372 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Minpoll, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 373 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Maxpoll, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 374 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_NOSELECT); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 375 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREEMPT); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 376 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREFER); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 377 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_TRUE); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 378 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_XLEAVE); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 379 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Ttl, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 380 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Mode, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 381 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Version, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 391 "ntp_parser.y"
    {
			struct unpeer_node *my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				enqueue(my_config.unpeers, my_node);
		;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 398 "ntp_parser.y"
    { (yyval.Integer) = T_Unconfig ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 399 "ntp_parser.y"
    { (yyval.Integer) = T_Unpeer ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 410 "ntp_parser.y"
    { my_config.broadcastclient = SIMPLE; ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 412 "ntp_parser.y"
    { my_config.broadcastclient = NOVOLLEY; ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 414 "ntp_parser.y"
    { append_queue(my_config.manycastserver, (yyvsp[(2) - (2)].Queue)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 416 "ntp_parser.y"
    { append_queue(my_config.multicastclient, (yyvsp[(2) - (2)].Queue)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 427 "ntp_parser.y"
    { my_config.auth.autokey = (yyvsp[(2) - (2)].Integer); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 429 "ntp_parser.y"
    { my_config.auth.control_key = (yyvsp[(2) - (2)].Integer); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 431 "ntp_parser.y"
    { 
			if (my_config.auth.crypto_cmd_list != NULL)
				append_queue(my_config.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Queue));
			else
				my_config.auth.crypto_cmd_list = (yyvsp[(2) - (2)].Queue);
			cryptosw++;
		;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 439 "ntp_parser.y"
    { my_config.auth.keys = (yyvsp[(2) - (2)].String); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 441 "ntp_parser.y"
    { my_config.auth.keysdir = (yyvsp[(2) - (2)].String); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 443 "ntp_parser.y"
    { my_config.auth.request_key = (yyvsp[(2) - (2)].Integer); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 445 "ntp_parser.y"
    { my_config.auth.trusted_key_list = (yyvsp[(2) - (2)].Queue); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 447 "ntp_parser.y"
    { my_config.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 453 "ntp_parser.y"
    { (yyval.Queue) = NULL; ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 457 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 458 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 463 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PRIV, (yyvsp[(2) - (2)].String)); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 465 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_IDENT, (yyvsp[(2) - (2)].String)); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 467 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PW, (yyvsp[(2) - (2)].String)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 469 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_RAND, (yyvsp[(2) - (2)].String)); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 471 "ntp_parser.y"
    { my_config.auth.revoke = (yyvsp[(2) - (2)].Integer); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 473 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_SIGN, (yyvsp[(2) - (2)].String)); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 483 "ntp_parser.y"
    { append_queue(my_config.orphan_cmds,(yyvsp[(2) - (2)].Queue)); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 487 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 488 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 493 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_CEILING, (double)(yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 495 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_FLOOR, (double)(yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 497 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_COHORT, (double)(yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 499 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_ORPHAN, (double)(yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 501 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINDISP, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 503 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXDIST, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 505 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINCLOCK, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 507 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXCLOCK, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 509 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINSANE, (double)(yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 511 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_BEACON, (double)(yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 513 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXHOP, (double)(yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 523 "ntp_parser.y"
    { append_queue(my_config.stats_list, (yyvsp[(2) - (2)].Queue)); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 525 "ntp_parser.y"
    { my_config.stats_dir = (yyvsp[(2) - (2)].String); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 527 "ntp_parser.y"
    {
			enqueue(my_config.filegen_opts,
				create_filegen_node((yyvsp[(2) - (3)].VoidPtr), (yyvsp[(3) - (3)].Queue)));
		;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 534 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 535 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].VoidPtr)); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 540 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("clockstats"); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 542 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("cryptostats"); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 544 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("loopstats"); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 546 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("peerstats"); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 548 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("rawstats"); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 550 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("sysstats"); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 552 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("timingstats"); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 554 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("protostats"); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 558 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 559 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 564 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(T_File, (yyvsp[(2) - (2)].String)); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 566 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Type, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 567 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Link); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 568 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Nolink); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 569 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Enable); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 570 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Disable); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 574 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_NONE; ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 575 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_PID; ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 576 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_DAY; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 577 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_WEEK; ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 578 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_MONTH; ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 579 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_YEAR; ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 580 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_AGE; ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 590 "ntp_parser.y"
    {   
			append_queue(my_config.discard_opts, (yyvsp[(2) - (2)].Queue));
		;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 594 "ntp_parser.y"
    {
			enqueue(my_config.restrict_opts,
				create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 599 "ntp_parser.y"
    {
			enqueue(my_config.restrict_opts,
				create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 604 "ntp_parser.y"
    {
			enqueue(my_config.restrict_opts,
				create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Queue), ip_file->line_no));
		;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 611 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 612 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 616 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_TIMEOUT); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 617 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_IGNORE); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 618 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_KOD); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 619 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LIMITED); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 620 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LPTRAP); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 621 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOMODIFY); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 622 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOPEER); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 623 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOQUERY); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 624 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTSERVE); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 625 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOTRAP); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 626 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTTRUST); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 627 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RESM_NTPONLY); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 628 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_VERSION); ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 632 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 633 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 637 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Average, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 638 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Minimum, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 639 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Monitor, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 648 "ntp_parser.y"
    { enqueue(my_config.fudge, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 652 "ntp_parser.y"
    { enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 653 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 658 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME1, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 660 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME2, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 662 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEVAL1,  (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 664 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CLK_HAVEVAL2,  (yyvsp[(2) - (2)].String)); ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 666 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG1, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 668 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG2, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 670 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG3, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 672 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG4, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 681 "ntp_parser.y"
    { append_queue(my_config.enable_opts,(yyvsp[(2) - (2)].Queue));  ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 683 "ntp_parser.y"
    { append_queue(my_config.disable_opts,(yyvsp[(2) - (2)].Queue));  ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 687 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 688 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 692 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 693 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_BROADCLIENT); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 694 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_CAL); ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 695 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_KERNEL); ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 696 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_MONITOR); ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 697 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_NTP); ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 699 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_FILEGEN); ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 707 "ntp_parser.y"
    { append_queue(my_config.tinker, (yyvsp[(2) - (2)].Queue)); ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 711 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 712 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 716 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_ALLAN, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 717 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PHI, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 718 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_FREQ, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 719 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_HUFFPUFF, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 720 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PANIC, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 721 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MAX, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 722 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MINSTEP, (yyvsp[(2) - (2)].Double)); ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 731 "ntp_parser.y"
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
		;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 747 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 753 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, (yyvsp[(2) - (2)].Double))); ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 755 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Calldelay, (yyvsp[(2) - (2)].Integer))); ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 757 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Tick, (yyvsp[(2) - (2)].Double))); ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 759 "ntp_parser.y"
    { /* Null action, possibly all null parms */ ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 761 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Leapfile, (yyvsp[(2) - (2)].String))); ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 764 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Pidfile, (yyvsp[(2) - (2)].String))); ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 766 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Logfile, (yyvsp[(2) - (2)].String))); ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 768 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Automax, (yyvsp[(2) - (2)].Integer))); ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 771 "ntp_parser.y"
    { append_queue(my_config.logconfig, (yyvsp[(2) - (2)].Queue)); ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 773 "ntp_parser.y"
    { append_queue(my_config.phone, (yyvsp[(2) - (2)].Queue)); ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 775 "ntp_parser.y"
    { enqueue(my_config.setvar, (yyvsp[(2) - (2)].Set_var)); ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 777 "ntp_parser.y"
    { enqueue(my_config.trap, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 779 "ntp_parser.y"
    { append_queue(my_config.ttl, (yyvsp[(2) - (2)].Queue)); ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 781 "ntp_parser.y"
    { enqueue(my_config.qos, create_attr_sval(T_Qos, (yyvsp[(2) - (2)].String))); ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 785 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String))); ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 787 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double)));
			  enqueue(my_config.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String))); ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 790 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Driftfile, "\0")); ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 795 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), DEF); ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 797 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (3)].String), (yyvsp[(3) - (3)].String), 0); ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 802 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 803 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 807 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Port, (yyvsp[(2) - (2)].Integer)); ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 808 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_pval(T_Interface, (yyvsp[(2) - (2)].Address_node)); ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 813 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 814 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 819 "ntp_parser.y"
    {
			char prefix = (yyvsp[(1) - (1)].String)[0];
			char *type = (yyvsp[(1) - (1)].String) + 1;
			
			if (prefix != '+' && prefix != '-' && prefix != '=') {
				yyerror("Logconfig prefix is not '+', '-' or '='\n");
			}
			else
				(yyval.Attr_val) = create_attr_sval(prefix, strdup(type));
			YYFREE((yyvsp[(1) - (1)].String));
		;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 837 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 838 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 842 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_pval((yyvsp[(2) - (2)].String))); ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 843 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_pval((yyvsp[(1) - (1)].String))); ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 847 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Address_node)); ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 848 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Address_node)); ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 853 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			}
			else
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
		;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 861 "ntp_parser.y"
    { (yyval.Integer) = 1; ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 862 "ntp_parser.y"
    { (yyval.Integer) = 0; ;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 866 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); ;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 867 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(1) - (1)].Double); ;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 876 "ntp_parser.y"
    {
			my_config.sim_details = create_sim_node((yyvsp[(3) - (5)].Queue), (yyvsp[(4) - (5)].Queue));

			/* Reset the old_config_style variable */
			old_config_style = 1;
		;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 890 "ntp_parser.y"
    { old_config_style = 0; ;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 894 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); ;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 895 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); ;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 899 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Beep_Delay, (yyvsp[(3) - (3)].Double)); ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 900 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Sim_Duration, (yyvsp[(3) - (3)].Double)); ;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 904 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_server)); ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 905 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_server)); ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 910 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Queue)); ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 914 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 918 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); ;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 922 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_script)); ;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 923 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_script)); ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 928 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Queue)); ;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 932 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 933 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 938 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Freq_Offset, (yyvsp[(3) - (3)].Double)); ;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 940 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Wander, (yyvsp[(3) - (3)].Double)); ;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 942 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Jitter, (yyvsp[(3) - (3)].Double)); ;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 944 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Prop_Delay, (yyvsp[(3) - (3)].Double)); ;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 946 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Proc_Delay, (yyvsp[(3) - (3)].Double)); ;}
    break;



/* Line 1455 of yacc.c  */
#line 3547 "ntp_parser.c"
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
#line 950 "ntp_parser.y"


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


