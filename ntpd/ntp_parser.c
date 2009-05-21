
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
  void yyerror (char *msg);
  extern int input_from_file;  /* 0=input from ntpq>config command buffer */
  extern int cryptosw;


/* Line 189 of yacc.c  */
#line 107 "ntp_parser.c"

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
     T_IPv4_flag = 307,
     T_IPv6_flag = 308,
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
     T_NtpSignDsocket = 350,
     T_Orphan = 351,
     T_Panic = 352,
     T_Peer = 353,
     T_Peerstats = 354,
     T_Phone = 355,
     T_Pid = 356,
     T_Pidfile = 357,
     T_Pool = 358,
     T_Port = 359,
     T_Pps = 360,
     T_Preempt = 361,
     T_Prefer = 362,
     T_Protostats = 363,
     T_Pw = 364,
     T_Qos = 365,
     T_RandFile = 366,
     T_Rawstats = 367,
     T_Refid = 368,
     T_Requestkey = 369,
     T_Restrict = 370,
     T_Revoke = 371,
     T_Server = 372,
     T_Setvar = 373,
     T_Sign = 374,
     T_Statistics = 375,
     T_Stats = 376,
     T_Statsdir = 377,
     T_Step = 378,
     T_Stepout = 379,
     T_Stratum = 380,
     T_String = 381,
     T_Sysstats = 382,
     T_Tick = 383,
     T_Time1 = 384,
     T_Time2 = 385,
     T_Timingstats = 386,
     T_Tinker = 387,
     T_Tos = 388,
     T_Trap = 389,
     T_True = 390,
     T_Trustedkey = 391,
     T_Ttl = 392,
     T_Type = 393,
     T_Unconfig = 394,
     T_Unpeer = 395,
     T_Version = 396,
     T_WanderThreshold = 397,
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
#define T_IPv4_flag 307
#define T_IPv6_flag 308
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
#define T_NtpSignDsocket 350
#define T_Orphan 351
#define T_Panic 352
#define T_Peer 353
#define T_Peerstats 354
#define T_Phone 355
#define T_Pid 356
#define T_Pidfile 357
#define T_Pool 358
#define T_Port 359
#define T_Pps 360
#define T_Preempt 361
#define T_Prefer 362
#define T_Protostats 363
#define T_Pw 364
#define T_Qos 365
#define T_RandFile 366
#define T_Rawstats 367
#define T_Refid 368
#define T_Requestkey 369
#define T_Restrict 370
#define T_Revoke 371
#define T_Server 372
#define T_Setvar 373
#define T_Sign 374
#define T_Statistics 375
#define T_Stats 376
#define T_Statsdir 377
#define T_Step 378
#define T_Stepout 379
#define T_Stratum 380
#define T_String 381
#define T_Sysstats 382
#define T_Tick 383
#define T_Time1 384
#define T_Time2 385
#define T_Timingstats 386
#define T_Tinker 387
#define T_Tos 388
#define T_Trap 389
#define T_True 390
#define T_Trustedkey 391
#define T_Ttl 392
#define T_Type 393
#define T_Unconfig 394
#define T_Unpeer 395
#define T_Version 396
#define T_WanderThreshold 397
#define T_Week 398
#define T_Xleave 399
#define T_Year 400
#define T_Flag 401
#define T_Void 402
#define T_EOC 403
#define T_Simulate 404
#define T_Beep_Delay 405
#define T_Sim_Duration 406
#define T_Server_Offset 407
#define T_Duration 408
#define T_Freq_Offset 409
#define T_Wander 410
#define T_Jitter 411
#define T_Prop_Delay 412
#define T_Proc_Delay 413




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 44 "ntp_parser.y"

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
#line 476 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 488 "ntp_parser.c"

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
#define YYFINAL  168
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   638

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  162
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  231
/* YYNRULES -- Number of states.  */
#define YYNSTATES  368

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
     274,   276,   278,   280,   282,   285,   289,   293,   298,   303,
     309,   310,   313,   315,   317,   319,   321,   323,   325,   327,
     329,   331,   333,   335,   337,   339,   342,   344,   347,   350,
     353,   357,   360,   362,   365,   368,   371,   374,   377,   380,
     383,   386,   389,   392,   395,   397,   399,   401,   403,   405,
     407,   409,   411,   414,   417,   419,   422,   425,   428,   431,
     434,   437,   440,   444,   446,   449,   452,   455,   458,   461,
     464,   467,   470,   473,   476,   479,   483,   486,   489,   491,
     494,   495,   500,   504,   507,   509,   512,   515,   518,   520,
     522,   525,   527,   530,   532,   535,   537,   539,   541,   543,
     545,   547,   553,   555,   559,   562,   566,   570,   573,   575,
     581,   586,   590,   593,   595,   602,   606,   609,   613,   617,
     621,   625
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     163,     0,    -1,   164,    -1,   164,   165,   148,    -1,   165,
     148,    -1,     1,   148,    -1,    -1,   166,    -1,   172,    -1,
     174,    -1,   175,    -1,   182,    -1,   188,    -1,   179,    -1,
     193,    -1,   196,    -1,   199,    -1,   202,    -1,   214,    -1,
     167,   168,   170,    -1,   167,   168,    -1,   117,    -1,   103,
      -1,    98,    -1,    12,    -1,    67,    -1,   169,    -1,    52,
     126,    -1,    53,   126,    -1,   126,    -1,   170,   171,    -1,
     171,    -1,     6,    -1,    11,   213,    -1,    15,    -1,    46,
      -1,    55,    50,    -1,    77,    50,    -1,    73,    50,    -1,
      88,    -1,   106,    -1,   107,    -1,   135,    -1,   144,    -1,
     137,    50,    -1,    79,    50,    -1,   141,    50,    -1,   173,
     168,    -1,   139,    -1,   140,    -1,    13,    -1,    13,    92,
      -1,    68,   211,    -1,    82,   211,    -1,     6,    50,    -1,
      21,    50,    -1,    22,   176,    -1,    56,   126,    -1,    57,
     126,    -1,   114,    50,    -1,   136,   209,    -1,    95,   126,
      -1,   177,    -1,    -1,   177,   178,    -1,   178,    -1,    44,
     126,    -1,    47,   126,    -1,   109,   126,    -1,   111,   126,
      -1,   116,    50,    -1,   119,   126,    -1,   133,   180,    -1,
     180,   181,    -1,   181,    -1,    18,    50,    -1,    41,    50,
      -1,    20,   212,    -1,    96,    50,    -1,    75,   213,    -1,
      71,   213,    -1,    74,   213,    -1,    70,   213,    -1,    78,
      50,    -1,    10,    50,    -1,    72,    50,    -1,   120,   183,
      -1,   122,   126,    -1,    35,   184,   185,    -1,   183,   184,
      -1,   184,    -1,    19,    -1,    23,    -1,    65,    -1,    99,
      -1,   112,    -1,   127,    -1,   131,    -1,   108,    -1,   185,
     186,    -1,   186,    -1,    34,   126,    -1,   138,   187,    -1,
      62,    -1,    83,    -1,    31,    -1,    26,    -1,    85,    -1,
     101,    -1,    24,    -1,   143,    -1,    81,    -1,   145,    -1,
       3,    -1,    27,   191,    -1,   115,   168,   189,    -1,   115,
      25,   189,    -1,   115,    52,    25,   189,    -1,   115,    53,
      25,   189,    -1,   115,   169,    69,   169,   189,    -1,    -1,
     189,   190,    -1,    40,    -1,    48,    -1,    58,    -1,    61,
      -1,    66,    -1,    84,    -1,    86,    -1,    87,    -1,    89,
      -1,    90,    -1,    91,    -1,    94,    -1,   141,    -1,   191,
     192,    -1,   192,    -1,     8,    50,    -1,    76,    50,    -1,
      80,    50,    -1,    43,   168,   194,    -1,   194,   195,    -1,
     195,    -1,   129,   213,    -1,   130,   213,    -1,   125,    50,
      -1,   113,   126,    -1,    36,   212,    -1,    37,   212,    -1,
      38,   212,    -1,    39,   212,    -1,    31,   197,    -1,    26,
     197,    -1,   197,   198,    -1,   198,    -1,     5,    -1,     9,
      -1,    16,    -1,    54,    -1,    80,    -1,    93,    -1,   121,
      -1,   132,   200,    -1,   200,   201,    -1,   201,    -1,     4,
     213,    -1,    28,   213,    -1,    42,   213,    -1,    45,   213,
      -1,    97,   213,    -1,   123,   213,    -1,   124,   213,    -1,
      49,   126,   165,    -1,    32,    -1,    14,   213,    -1,    17,
      50,    -1,   128,   213,    -1,    30,   203,    -1,    60,   126,
      -1,   102,   126,    -1,    64,   126,    -1,     7,    50,    -1,
      63,   207,    -1,   100,   210,    -1,   118,   204,    -1,   134,
     169,   205,    -1,   137,   209,    -1,   110,   126,    -1,   126,
      -1,   126,    29,    -1,    -1,   126,   159,   126,    25,    -1,
     126,   159,   126,    -1,   205,   206,    -1,   206,    -1,   104,
      50,    -1,    51,   169,    -1,   207,   208,    -1,   208,    -1,
     126,    -1,   209,    50,    -1,    50,    -1,   210,   126,    -1,
     126,    -1,   211,   168,    -1,   168,    -1,    50,    -1,   135,
      -1,    33,    -1,    50,    -1,    29,    -1,   215,   160,   216,
     218,   161,    -1,   149,    -1,   216,   217,   148,    -1,   217,
     148,    -1,   150,   159,   213,    -1,   151,   159,   213,    -1,
     218,   219,    -1,   219,    -1,   221,   160,   220,   222,   161,
      -1,   152,   159,   213,   148,    -1,   117,   159,   168,    -1,
     222,   223,    -1,   223,    -1,   153,   159,   213,   160,   224,
     161,    -1,   224,   225,   148,    -1,   225,   148,    -1,   154,
     159,   213,    -1,   155,   159,   213,    -1,   156,   159,   213,
      -1,   157,   159,   213,    -1,   158,   159,   213,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   280,   280,   284,   285,   286,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   327,
     333,   342,   343,   344,   345,   346,   350,   351,   352,   356,
     360,   361,   365,   366,   367,   368,   369,   370,   371,   372,
     373,   374,   375,   376,   377,   378,   379,   388,   396,   397,
     407,   409,   411,   413,   424,   426,   428,   436,   438,   440,
     442,   444,   449,   451,   455,   456,   460,   462,   464,   466,
     468,   470,   480,   485,   486,   490,   492,   494,   496,   498,
     500,   502,   504,   506,   508,   510,   520,   522,   524,   532,
     533,   537,   539,   541,   543,   545,   547,   549,   551,   556,
     557,   561,   563,   565,   566,   567,   568,   572,   573,   574,
     575,   576,   577,   578,   587,   591,   596,   601,   614,   627,
     635,   636,   640,   641,   642,   643,   644,   645,   646,   647,
     648,   649,   650,   651,   652,   656,   657,   661,   662,   663,
     671,   676,   677,   681,   683,   685,   687,   689,   691,   693,
     695,   704,   706,   711,   712,   716,   717,   718,   719,   720,
     721,   723,   731,   735,   736,   740,   741,   742,   743,   744,
     745,   746,   754,   770,   776,   778,   780,   782,   784,   787,
     789,   791,   794,   796,   798,   800,   802,   804,   808,   810,
     814,   818,   820,   826,   827,   831,   832,   837,   838,   842,
     861,   862,   866,   867,   871,   872,   876,   885,   886,   890,
     891,   899,   914,   918,   919,   923,   924,   928,   929,   933,
     938,   942,   946,   947,   951,   956,   957,   961,   963,   965,
     967,   969
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
  "T_Includefile", "T_Integer", "T_Interface", "T_IPv4_flag",
  "T_IPv6_flag", "T_Kernel", "T_Key", "T_Keys", "T_Keysdir", "T_Kod",
  "T_Leap", "T_Leapfile", "T_Limited", "T_Link", "T_Logconfig",
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
  "T_Ttl", "T_Type", "T_Unconfig", "T_Unpeer", "T_Version",
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
     187,   187,   187,   187,   188,   188,   188,   188,   188,   188,
     189,   189,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   191,   191,   192,   192,   192,
     193,   194,   194,   195,   195,   195,   195,   195,   195,   195,
     195,   196,   196,   197,   197,   198,   198,   198,   198,   198,
     198,   198,   199,   200,   200,   201,   201,   201,   201,   201,
     201,   201,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   203,   203,
     203,   204,   204,   205,   205,   206,   206,   207,   207,   208,
     209,   209,   210,   210,   211,   211,   212,   212,   212,   213,
     213,   214,   215,   216,   216,   217,   217,   218,   218,   219,
     220,   221,   222,   222,   223,   224,   224,   225,   225,   225,
     225,   225
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
       1,     1,     1,     1,     2,     3,     3,     4,     4,     5,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     2,     2,     2,
       3,     2,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     3,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     3,     2,     2,     1,     2,
       0,     4,     3,     2,     1,     2,     2,     2,     1,     1,
       2,     1,     2,     1,     2,     1,     1,     1,     1,     1,
       1,     5,     1,     3,     2,     3,     3,     2,     1,     5,
       4,     3,     2,     1,     6,     3,     2,     3,     3,     3,
       3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,    24,    50,     0,     0,     0,    63,
       0,     0,   190,     0,   173,     0,     0,     0,     0,     0,
       0,     0,     0,    25,     0,     0,     0,    23,     0,     0,
      22,     0,     0,     0,    21,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,    49,   212,     0,     2,     0,
       7,     0,     8,     0,     9,    10,    13,    11,    12,    14,
      15,    16,    17,    18,     0,     5,    54,   181,    51,   210,
     209,   174,   175,    55,     0,     0,     0,     0,     0,     0,
      56,    62,    65,   155,   156,   157,   158,   159,   160,   161,
     152,   154,     0,     0,     0,   114,   136,   188,   177,   151,
      91,    92,    93,    94,    98,    95,    96,    97,     0,     0,
       0,    29,     0,    26,     6,    57,    58,   178,   199,   182,
     198,   180,   205,    52,    53,    61,   203,   183,   179,   187,
      59,   120,     0,     0,   120,    26,     0,   184,    86,    90,
      87,   176,     0,     0,     0,     0,     0,     0,     0,   162,
     164,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    72,    74,     0,   201,    60,   186,     1,     0,
       4,    20,    47,     0,    66,    67,    68,    69,    70,    71,
      64,   153,   137,   138,   139,   135,   189,   106,   105,     0,
     103,   104,     0,    88,   100,    27,    28,     0,     0,     0,
       0,     0,     0,     0,     0,   140,   142,   172,   197,   204,
     202,   116,   120,   120,   115,     0,     0,    89,   165,   166,
     167,   168,   169,   170,   171,   163,    84,    75,   208,   206,
     207,    77,    76,    82,    80,    85,    81,    79,    83,    78,
      73,     0,     0,   185,   194,   200,     3,    32,     0,    34,
      35,     0,     0,     0,     0,    39,    40,    41,    42,     0,
       0,    43,    19,    31,     0,     0,     0,     0,   101,   113,
     109,   111,   107,   108,   110,   112,   102,    99,   147,   148,
     149,   150,   146,   145,   143,   144,   141,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     121,   117,   118,   120,   192,   196,   195,   193,    33,    36,
      38,    37,    45,    44,    46,    30,     0,     0,     0,     0,
       0,   218,     0,   214,   119,   191,   215,   216,     0,   213,
     211,   217,     0,   221,     0,     0,     0,     0,     0,   223,
       0,     0,   219,   222,   220,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   224,
       0,   226,   227,   228,   229,   230,   231,   225
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    47,    48,    49,    50,    51,   122,   113,   262,   263,
      52,    53,    54,    55,    80,    81,    82,    56,   162,   163,
      57,   138,   108,   193,   194,   276,    58,   211,   300,    95,
      96,    59,   205,   206,    60,    90,    91,    61,   149,   150,
      62,    98,   137,   243,   244,   119,   120,   166,   127,   123,
     231,    71,    63,    64,   266,   267,   320,   321,   335,   322,
     338,   339,   352,   353
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -133
static const yytype_int16 yypact[] =
{
     118,  -118,    -8,    -7,  -133,   -46,    -6,    -3,     1,   -26,
       8,    19,   -77,     8,  -133,   157,   -48,   -73,   -72,   -69,
     -68,   -67,   -65,  -133,   -48,   -48,   -62,  -133,   -61,   -56,
    -133,   -55,    29,   -15,  -133,   -44,   157,   -40,    -6,     3,
     542,   -39,    34,    34,  -133,  -133,  -133,    89,   279,   -57,
    -133,   -48,  -133,   -48,  -133,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,   -66,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,   -29,   -28,   -24,   -13,    53,     7,
    -133,   -26,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,
       8,  -133,    66,    84,    86,    19,  -133,   108,  -133,     8,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,   -23,    12,
      20,  -133,    30,  -133,   417,  -133,  -133,  -133,  -133,   -67,
    -133,  -133,  -133,   -48,   -48,  -133,  -133,    21,  -133,  -133,
    -133,  -133,    15,    16,  -133,    82,    -2,  -133,   157,  -133,
    -133,  -133,    -6,    -6,    -6,    -6,    -6,    -6,    -6,     3,
    -133,   102,   112,   -21,   113,    -6,    -6,   114,    -6,    -6,
     119,   120,   542,  -133,   -31,  -133,   122,   122,  -133,    10,
    -133,   492,  -133,  -117,  -133,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,    42,
    -133,  -133,    11,   -23,  -133,  -133,  -133,   -21,   -21,   -21,
     -21,    47,   127,    -6,    -6,    30,  -133,  -133,  -133,  -133,
    -133,   301,  -133,  -133,   301,   -39,    58,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,
    -133,   -39,   129,   -31,  -133,  -133,  -133,  -133,    -6,  -133,
    -133,   142,   143,   151,   152,  -133,  -133,  -133,  -133,   153,
     154,  -133,   492,  -133,    46,    48,   -95,    60,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,
    -133,   301,   301,  -133,   181,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,    -6,    -6,    52,    64,
     -98,  -133,    54,  -133,   301,  -133,  -133,  -133,   -48,  -133,
    -133,  -133,    63,  -133,    65,    70,    -6,    67,  -125,  -133,
      69,    -6,  -133,  -133,  -133,    59,    41,    68,    71,    72,
      75,    78,    33,    77,    -6,    -6,    -6,    -6,    -6,  -133,
      81,  -133,  -133,  -133,  -133,  -133,  -133,  -133
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -133,  -133,  -133,   -42,  -133,  -133,    -1,   -32,  -133,   -20,
    -133,  -133,  -133,  -133,  -133,  -133,   158,  -133,  -133,    79,
    -133,  -133,   -10,  -133,    50,  -133,  -133,  -132,  -133,  -133,
     149,  -133,  -133,    40,  -133,   234,   -74,  -133,  -133,    99,
    -133,  -133,  -133,  -133,     6,  -133,   134,   216,  -133,   235,
    -123,   -38,  -133,  -133,  -133,    -5,  -133,   -58,  -133,  -133,
    -133,   -75,  -133,   -88
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     141,   135,   214,   187,   109,   110,   169,   142,   188,   164,
     131,   189,   228,    83,   269,   112,   181,    84,    74,   318,
     241,    75,   318,    69,    85,   181,   139,    92,   337,   229,
      65,   143,   134,   264,   265,   270,   342,   132,   133,   190,
     212,   213,    66,    67,    70,   144,    68,    72,   145,    97,
     171,    73,   172,   114,   115,   264,   265,   116,   117,   118,
     191,   121,    86,   330,   125,   126,   197,   198,   199,   200,
     128,   129,   207,   242,   278,   279,   280,   281,   111,   130,
     301,   302,   136,    76,   165,    77,   140,   111,    87,   168,
      78,   170,   271,    79,   173,    93,   272,   174,   175,    94,
     146,    88,   176,   178,   218,   219,   220,   221,   222,   223,
     224,   111,   273,   177,   230,   192,   182,   233,   234,     1,
     236,   237,   209,   209,     2,     3,   147,   148,   217,    89,
       4,     5,     6,   179,   183,     7,   184,   186,   195,     8,
       9,   195,   196,   201,    10,    11,   196,   210,    12,    13,
      14,   215,   226,    15,   274,   202,   275,   216,   246,   203,
     204,    16,   227,   232,   235,   284,   285,    17,   268,   238,
     239,   324,   245,   282,    18,    19,   100,   283,    20,   306,
     101,    21,    22,   303,   304,    23,    24,   347,   348,   349,
     350,   351,   309,   310,   359,   347,   348,   349,   350,   351,
      25,   311,   312,   313,   314,   316,   325,   317,   323,   305,
     308,   328,   329,    26,   332,   334,    27,   344,    28,   346,
      29,    30,   102,   337,   336,   361,   341,   354,    31,   367,
     355,   356,    32,    33,   357,    34,    35,   358,    36,   180,
      37,   240,   315,   277,   185,   286,    38,    99,   225,   307,
      39,    40,    41,   208,    42,    43,   103,    44,    45,   167,
     124,   319,   331,   343,   360,   104,    -6,    46,     0,   105,
       0,     0,     0,     0,     0,     0,     0,     0,   326,   327,
       0,     0,     0,     0,   106,     2,     3,     0,   107,     0,
       0,     4,     5,     6,     0,     0,     7,     0,   340,     0,
       8,     9,     0,   345,     0,    10,    11,     0,     0,    12,
      13,    14,     0,     0,    15,     0,   362,   363,   364,   365,
     366,     0,    16,     0,     0,     0,     0,   333,    17,     0,
       0,     0,     0,     0,     0,    18,    19,     0,     0,    20,
       0,   287,    21,    22,     0,     0,    23,    24,     0,   288,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   289,
       0,    25,   290,     0,     0,     0,     0,   291,     0,     0,
       0,     0,     0,     0,    26,     0,     0,    27,     0,    28,
       0,    29,    30,     0,     0,   292,     0,   293,   294,    31,
     295,   296,   297,    32,    33,   298,    34,    35,     0,    36,
       0,    37,     0,     0,     0,     0,     0,    38,     0,     0,
       0,    39,    40,    41,     0,    42,    43,     0,    44,    45,
       0,     0,     0,     2,     3,     0,     0,    -6,    46,     4,
       5,     6,     0,     0,     7,     0,     0,     0,     8,     9,
       0,     0,   299,    10,    11,     0,     0,    12,    13,    14,
       0,     0,    15,     0,     0,     0,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    17,     0,     0,     0,
       0,     0,     0,    18,    19,     0,     0,    20,     0,     0,
      21,    22,     0,     0,    23,    24,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   247,    25,
       0,     0,     0,   248,     0,     0,     0,   249,     0,     0,
       0,     0,    26,     0,     0,    27,     0,    28,     0,    29,
      30,     0,     0,     0,     0,     0,     0,    31,     0,     0,
       0,    32,    33,     0,    34,    35,     0,    36,   250,    37,
       0,     0,     0,     0,     0,    38,     0,   251,     0,    39,
      40,    41,   151,    42,    43,     0,    44,    45,     0,     0,
     152,     0,   153,     0,     0,   252,    46,     0,     0,   253,
       0,   254,     0,     0,     0,     0,     0,     0,     0,     0,
     255,     0,     0,   154,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   256,   257,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   155,   156,   157,     0,   158,   159,     0,     0,
     160,     0,     0,     0,     0,     0,     0,   258,     0,   259,
       0,     0,     0,   260,     0,     0,   261,     0,   161
};

static const yytype_int16 yycheck[] =
{
      38,    33,   134,    26,    52,    53,    48,     4,    31,    41,
      25,    34,    33,     5,     3,    16,    90,     9,    44,   117,
      51,    47,   117,    29,    16,    99,    36,     8,   153,    50,
     148,    28,    33,   150,   151,    24,   161,    52,    53,    62,
      25,    25,    50,    50,    50,    42,    92,    50,    45,   126,
      51,    50,    53,   126,   126,   150,   151,   126,   126,   126,
      83,   126,    54,   161,   126,   126,    36,    37,    38,    39,
     126,   126,   114,   104,   197,   198,   199,   200,   126,    50,
     212,   213,   126,   109,    50,   111,   126,   126,    80,     0,
     116,   148,    81,   119,   160,    76,    85,   126,   126,    80,
      97,    93,   126,    50,   142,   143,   144,   145,   146,   147,
     148,   126,   101,   126,   135,   138,    50,   155,   156,     1,
     158,   159,   123,   124,     6,     7,   123,   124,   138,   121,
      12,    13,    14,   126,    50,    17,    50,    29,   126,    21,
      22,   126,   126,   113,    26,    27,   126,   126,    30,    31,
      32,    69,    50,    35,   143,   125,   145,   159,   148,   129,
     130,    43,    50,    50,    50,   203,   204,    49,   126,    50,
      50,   303,    50,   126,    56,    57,    19,    50,    60,    50,
      23,    63,    64,   215,   126,    67,    68,   154,   155,   156,
     157,   158,    50,    50,   161,   154,   155,   156,   157,   158,
      82,    50,    50,    50,    50,   159,    25,   159,   148,   241,
     248,   159,   148,    95,   160,   152,    98,   148,   100,   160,
     102,   103,    65,   153,   159,   148,   159,   159,   110,   148,
     159,   159,   114,   115,   159,   117,   118,   159,   120,    81,
     122,   162,   262,   193,    95,   205,   128,    13,   149,   243,
     132,   133,   134,   119,   136,   137,    99,   139,   140,    43,
      25,   266,   320,   338,   352,   108,   148,   149,    -1,   112,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   316,   317,
      -1,    -1,    -1,    -1,   127,     6,     7,    -1,   131,    -1,
      -1,    12,    13,    14,    -1,    -1,    17,    -1,   336,    -1,
      21,    22,    -1,   341,    -1,    26,    27,    -1,    -1,    30,
      31,    32,    -1,    -1,    35,    -1,   354,   355,   356,   357,
     358,    -1,    43,    -1,    -1,    -1,    -1,   328,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    56,    57,    -1,    -1,    60,
      -1,    40,    63,    64,    -1,    -1,    67,    68,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,
      -1,    82,    61,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    -1,    -1,    98,    -1,   100,
      -1,   102,   103,    -1,    -1,    84,    -1,    86,    87,   110,
      89,    90,    91,   114,   115,    94,   117,   118,    -1,   120,
      -1,   122,    -1,    -1,    -1,    -1,    -1,   128,    -1,    -1,
      -1,   132,   133,   134,    -1,   136,   137,    -1,   139,   140,
      -1,    -1,    -1,     6,     7,    -1,    -1,   148,   149,    12,
      13,    14,    -1,    -1,    17,    -1,    -1,    -1,    21,    22,
      -1,    -1,   141,    26,    27,    -1,    -1,    30,    31,    32,
      -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    56,    57,    -1,    -1,    60,    -1,    -1,
      63,    64,    -1,    -1,    67,    68,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     6,    82,
      -1,    -1,    -1,    11,    -1,    -1,    -1,    15,    -1,    -1,
      -1,    -1,    95,    -1,    -1,    98,    -1,   100,    -1,   102,
     103,    -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,    -1,
      -1,   114,   115,    -1,   117,   118,    -1,   120,    46,   122,
      -1,    -1,    -1,    -1,    -1,   128,    -1,    55,    -1,   132,
     133,   134,    10,   136,   137,    -1,   139,   140,    -1,    -1,
      18,    -1,    20,    -1,    -1,    73,   149,    -1,    -1,    77,
      -1,    79,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      88,    -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,   107,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      78,    -1,    -1,    -1,    -1,    -1,    -1,   135,    -1,   137,
      -1,    -1,    -1,   141,    -1,    -1,   144,    -1,    96
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     6,     7,    12,    13,    14,    17,    21,    22,
      26,    27,    30,    31,    32,    35,    43,    49,    56,    57,
      60,    63,    64,    67,    68,    82,    95,    98,   100,   102,
     103,   110,   114,   115,   117,   118,   120,   122,   128,   132,
     133,   134,   136,   137,   139,   140,   149,   163,   164,   165,
     166,   167,   172,   173,   174,   175,   179,   182,   188,   193,
     196,   199,   202,   214,   215,   148,    50,    50,    92,    29,
      50,   213,    50,    50,    44,    47,   109,   111,   116,   119,
     176,   177,   178,     5,     9,    16,    54,    80,    93,   121,
     197,   198,     8,    76,    80,   191,   192,   126,   203,   197,
      19,    23,    65,    99,   108,   112,   127,   131,   184,    52,
      53,   126,   168,   169,   126,   126,   126,   126,   126,   207,
     208,   126,   168,   211,   211,   126,   126,   210,   126,   126,
      50,    25,    52,    53,   168,   169,   126,   204,   183,   184,
     126,   213,     4,    28,    42,    45,    97,   123,   124,   200,
     201,    10,    18,    20,    41,    70,    71,    72,    74,    75,
      78,    96,   180,   181,   169,    50,   209,   209,     0,   165,
     148,   168,   168,   160,   126,   126,   126,   126,    50,   126,
     178,   198,    50,    50,    50,   192,    29,    26,    31,    34,
      62,    83,   138,   185,   186,   126,   126,    36,    37,    38,
      39,   113,   125,   129,   130,   194,   195,   165,   208,   168,
     126,   189,    25,    25,   189,    69,   159,   184,   213,   213,
     213,   213,   213,   213,   213,   201,    50,    50,    33,    50,
     135,   212,    50,   213,   213,    50,   213,   213,    50,    50,
     181,    51,   104,   205,   206,    50,   148,     6,    11,    15,
      46,    55,    73,    77,    79,    88,   106,   107,   135,   137,
     141,   144,   170,   171,   150,   151,   216,   217,   126,     3,
      24,    81,    85,   101,   143,   145,   187,   186,   212,   212,
     212,   212,   126,    50,   213,   213,   195,    40,    48,    58,
      61,    66,    84,    86,    87,    89,    90,    91,    94,   141,
     190,   189,   189,   169,   126,   169,    50,   206,   213,    50,
      50,    50,    50,    50,    50,   171,   159,   159,   117,   217,
     218,   219,   221,   148,   189,    25,   213,   213,   159,   148,
     161,   219,   160,   168,   152,   220,   159,   153,   222,   223,
     213,   159,   161,   223,   148,   213,   160,   154,   155,   156,
     157,   158,   224,   225,   159,   159,   159,   159,   159,   161,
     225,   148,   213,   213,   213,   213,   213,   148
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
#line 287 "ntp_parser.y"
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
					ip_file->line_no - 1);
			} else if (input_from_file != 0)
				msyslog(LOG_ERR,
					"parse: bad boolean input flag");
	}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 328 "ntp_parser.y"
    {
			struct peer_node *my_node =  create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue));
			if (my_node)
				enqueue(my_config.peers, my_node);
		}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 334 "ntp_parser.y"
    {
			struct peer_node *my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
			if (my_node)
				enqueue(my_config.peers, my_node);
		}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 342 "ntp_parser.y"
    { (yyval.Integer) = T_Server; }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 343 "ntp_parser.y"
    { (yyval.Integer) = T_Pool; }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 344 "ntp_parser.y"
    { (yyval.Integer) = T_Peer; }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 345 "ntp_parser.y"
    { (yyval.Integer) = T_Broadcast; }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 346 "ntp_parser.y"
    { (yyval.Integer) = T_Manycastclient; }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 350 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(1) - (1)].Address_node); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 351 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 352 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET6); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 356 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), 0); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 360 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 361 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 365 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_SKEY); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 366 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Bias, (yyvsp[(2) - (2)].Double)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 367 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_BURST); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 368 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_IBURST); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 369 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Key, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 370 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Minpoll, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 371 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Maxpoll, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 372 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_NOSELECT); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 373 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREEMPT); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 374 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_PREFER); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 375 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_TRUE); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 376 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, FLAG_XLEAVE); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 377 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Ttl, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 378 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Mode, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 379 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Version, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 389 "ntp_parser.y"
    {
			struct unpeer_node *my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				enqueue(my_config.unpeers, my_node);
		}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 396 "ntp_parser.y"
    { (yyval.Integer) = T_Unconfig; }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 397 "ntp_parser.y"
    { (yyval.Integer) = T_Unpeer; }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 408 "ntp_parser.y"
    { my_config.broadcastclient = SIMPLE; }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 410 "ntp_parser.y"
    { my_config.broadcastclient = NOVOLLEY; }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 412 "ntp_parser.y"
    { append_queue(my_config.manycastserver, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 414 "ntp_parser.y"
    { append_queue(my_config.multicastclient, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 425 "ntp_parser.y"
    { my_config.auth.autokey = (yyvsp[(2) - (2)].Integer); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 427 "ntp_parser.y"
    { my_config.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 429 "ntp_parser.y"
    { 
			if (my_config.auth.crypto_cmd_list != NULL)
				append_queue(my_config.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Queue));
			else
				my_config.auth.crypto_cmd_list = (yyvsp[(2) - (2)].Queue);
			cryptosw++;
		}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 437 "ntp_parser.y"
    { my_config.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 439 "ntp_parser.y"
    { my_config.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 441 "ntp_parser.y"
    { my_config.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 443 "ntp_parser.y"
    { my_config.auth.trusted_key_list = (yyvsp[(2) - (2)].Queue); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 445 "ntp_parser.y"
    { my_config.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 451 "ntp_parser.y"
    { (yyval.Queue) = NULL; }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 455 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 456 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 461 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PRIV, (yyvsp[(2) - (2)].String)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 463 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_IDENT, (yyvsp[(2) - (2)].String)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 465 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_PW, (yyvsp[(2) - (2)].String)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 467 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_RAND, (yyvsp[(2) - (2)].String)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 469 "ntp_parser.y"
    { my_config.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 471 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CRYPTO_CONF_SIGN, (yyvsp[(2) - (2)].String)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 481 "ntp_parser.y"
    { append_queue(my_config.orphan_cmds,(yyvsp[(2) - (2)].Queue)); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 485 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 486 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 491 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_CEILING, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 493 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_FLOOR, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 495 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_COHORT, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 497 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_ORPHAN, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 499 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINDISP, (yyvsp[(2) - (2)].Double)); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 501 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXDIST, (yyvsp[(2) - (2)].Double)); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 503 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINCLOCK, (yyvsp[(2) - (2)].Double)); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 505 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXCLOCK, (yyvsp[(2) - (2)].Double)); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 507 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MINSANE, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 509 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_BEACON, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 511 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(PROTO_MAXHOP, (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 521 "ntp_parser.y"
    { append_queue(my_config.stats_list, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 523 "ntp_parser.y"
    { my_config.stats_dir = (yyvsp[(2) - (2)].String); }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 525 "ntp_parser.y"
    {
			enqueue(my_config.filegen_opts,
				create_filegen_node((yyvsp[(2) - (3)].VoidPtr), (yyvsp[(3) - (3)].Queue)));
		}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 532 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 533 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].VoidPtr)); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 538 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("clockstats"); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 540 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("cryptostats"); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 542 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("loopstats"); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 544 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("peerstats"); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 546 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("rawstats"); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 548 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("sysstats"); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 550 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("timingstats"); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 552 "ntp_parser.y"
    { (yyval.VoidPtr) = create_pval("protostats"); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 556 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 557 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 562 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(T_File, (yyvsp[(2) - (2)].String)); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 564 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Type, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 565 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Link); }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 566 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Nolink); }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 567 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Enable); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 568 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, T_Disable); }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 572 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_NONE; }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 573 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_PID; }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 574 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_DAY; }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 575 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_WEEK; }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 576 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_MONTH; }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 577 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_YEAR; }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 578 "ntp_parser.y"
    { (yyval.Integer) = FILEGEN_AGE; }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 588 "ntp_parser.y"
    {   
			append_queue(my_config.discard_opts, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 592 "ntp_parser.y"
    {
			enqueue(my_config.restrict_opts,
				create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 597 "ntp_parser.y"
    {
			enqueue(my_config.restrict_opts,
				create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 602 "ntp_parser.y"
    {
			enqueue(my_config.restrict_opts,
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

  case 118:

/* Line 1455 of yacc.c  */
#line 615 "ntp_parser.y"
    {
			enqueue(my_config.restrict_opts,
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

  case 119:

/* Line 1455 of yacc.c  */
#line 628 "ntp_parser.y"
    {
			enqueue(my_config.restrict_opts,
				create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Queue), ip_file->line_no));
		}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 635 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 636 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].VoidPtr)); }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 640 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_TIMEOUT); }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 641 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_IGNORE); }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 642 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_KOD); }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 643 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LIMITED); }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 644 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_LPTRAP); }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 645 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOMODIFY); }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 646 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOPEER); }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 647 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOQUERY); }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 648 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTSERVE); }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 649 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_NOTRAP); }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 650 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_DONTTRUST); }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 651 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RESM_NTPONLY); }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 652 "ntp_parser.y"
    { (yyval.VoidPtr) = create_ival(RES_VERSION); }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 656 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 657 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 661 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Average, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 662 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Minimum, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 663 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Monitor, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 672 "ntp_parser.y"
    { enqueue(my_config.fudge, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 676 "ntp_parser.y"
    { enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 677 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 682 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME1, (yyvsp[(2) - (2)].Double)); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 684 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(CLK_HAVETIME2, (yyvsp[(2) - (2)].Double)); }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 686 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEVAL1,  (yyvsp[(2) - (2)].Integer)); }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 688 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval(CLK_HAVEVAL2,  (yyvsp[(2) - (2)].String)); }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 690 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG1, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 692 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG2, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 694 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG3, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 696 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(CLK_HAVEFLAG4, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 705 "ntp_parser.y"
    { append_queue(my_config.enable_opts,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 707 "ntp_parser.y"
    { append_queue(my_config.disable_opts,(yyvsp[(2) - (2)].Queue));  }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 711 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 712 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 716 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 717 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_BROADCLIENT); }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 718 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_CAL); }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 719 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_KERNEL); }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 720 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_MONITOR); }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 721 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_NTP); }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 723 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, PROTO_FILEGEN); }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 731 "ntp_parser.y"
    { append_queue(my_config.tinker, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 735 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 736 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 740 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_ALLAN, (yyvsp[(2) - (2)].Double)); }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 741 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PHI, (yyvsp[(2) - (2)].Double)); }
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 742 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_FREQ, (yyvsp[(2) - (2)].Double)); }
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 743 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_HUFFPUFF, (yyvsp[(2) - (2)].Double)); }
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 744 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_PANIC, (yyvsp[(2) - (2)].Double)); }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 745 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MAX, (yyvsp[(2) - (2)].Double)); }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 746 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(LOOP_MINSTEP, (yyvsp[(2) - (2)].Double)); }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 755 "ntp_parser.y"
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
#line 771 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 777 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, (yyvsp[(2) - (2)].Double))); }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 779 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Calldelay, (yyvsp[(2) - (2)].Integer))); }
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 781 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Tick, (yyvsp[(2) - (2)].Double))); }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 783 "ntp_parser.y"
    { /* Null action, possibly all null parms */ }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 785 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Leapfile, (yyvsp[(2) - (2)].String))); }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 788 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Pidfile, (yyvsp[(2) - (2)].String))); }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 790 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Logfile, (yyvsp[(2) - (2)].String))); }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 792 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Automax, (yyvsp[(2) - (2)].Integer))); }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 795 "ntp_parser.y"
    { append_queue(my_config.logconfig, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 797 "ntp_parser.y"
    { append_queue(my_config.phone, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 799 "ntp_parser.y"
    { enqueue(my_config.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 801 "ntp_parser.y"
    { enqueue(my_config.trap, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 803 "ntp_parser.y"
    { append_queue(my_config.ttl, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 805 "ntp_parser.y"
    { enqueue(my_config.qos, create_attr_sval(T_Qos, (yyvsp[(2) - (2)].String))); }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 809 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String))); }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 811 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double)));
			  enqueue(my_config.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String))); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 814 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 819 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), DEF); }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 821 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (3)].String), (yyvsp[(3) - (3)].String), 0); }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 826 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 827 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 831 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Port, (yyvsp[(2) - (2)].Integer)); }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 832 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_pval(T_Interface, (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 837 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 838 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 843 "ntp_parser.y"
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

  case 200:

/* Line 1455 of yacc.c  */
#line 861 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 862 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 866 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_pval((yyvsp[(2) - (2)].String))); }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 867 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_pval((yyvsp[(1) - (1)].String))); }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 871 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 872 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Address_node)); }
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 877 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			}
			else
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
		}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 885 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 886 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 890 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 891 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(1) - (1)].Double); }
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 900 "ntp_parser.y"
    {
			my_config.sim_details = create_sim_node((yyvsp[(3) - (5)].Queue), (yyvsp[(4) - (5)].Queue));

			/* Reset the old_config_style variable */
			old_config_style = 1;
		}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 914 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 918 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 919 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 923 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Beep_Delay, (yyvsp[(3) - (3)].Double)); }
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 924 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Sim_Duration, (yyvsp[(3) - (3)].Double)); }
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 928 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_server)); }
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 929 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_server)); }
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 934 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Queue)); }
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 938 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 942 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 946 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_script)); }
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 947 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_script)); }
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 952 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Queue)); }
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 956 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 957 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 962 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Freq_Offset, (yyvsp[(3) - (3)].Double)); }
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 964 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Wander, (yyvsp[(3) - (3)].Double)); }
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 966 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Jitter, (yyvsp[(3) - (3)].Double)); }
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 968 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Prop_Delay, (yyvsp[(3) - (3)].Double)); }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 970 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval(T_Proc_Delay, (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1455 of yacc.c  */
#line 3746 "ntp_parser.c"
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
#line 974 "ntp_parser.y"


/* KEYWORDS
 * --------
 */

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


