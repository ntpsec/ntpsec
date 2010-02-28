
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
     T_Bias = 267,
     T_Broadcast = 268,
     T_Broadcastclient = 269,
     T_Broadcastdelay = 270,
     T_Burst = 271,
     T_Calibrate = 272,
     T_Calldelay = 273,
     T_Ceiling = 274,
     T_Clockstats = 275,
     T_Cohort = 276,
     T_ControlKey = 277,
     T_Crypto = 278,
     T_Cryptostats = 279,
     T_Day = 280,
     T_Default = 281,
     T_Digest = 282,
     T_Disable = 283,
     T_Discard = 284,
     T_Dispersion = 285,
     T_Double = 286,
     T_Driftfile = 287,
     T_Drop = 288,
     T_Enable = 289,
     T_End = 290,
     T_False = 291,
     T_File = 292,
     T_Filegen = 293,
     T_Flag1 = 294,
     T_Flag2 = 295,
     T_Flag3 = 296,
     T_Flag4 = 297,
     T_Flake = 298,
     T_Floor = 299,
     T_Freq = 300,
     T_Fudge = 301,
     T_Host = 302,
     T_Huffpuff = 303,
     T_Iburst = 304,
     T_Ident = 305,
     T_Ignore = 306,
     T_Incalloc = 307,
     T_Incmem = 308,
     T_Initalloc = 309,
     T_Initmem = 310,
     T_Includefile = 311,
     T_Integer = 312,
     T_Interface = 313,
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
     T_Nopeer = 356,
     T_Noquery = 357,
     T_Noselect = 358,
     T_Noserve = 359,
     T_Notrap = 360,
     T_Notrust = 361,
     T_Ntp = 362,
     T_Ntpport = 363,
     T_NtpSignDsocket = 364,
     T_Orphan = 365,
     T_Panic = 366,
     T_Peer = 367,
     T_Peerstats = 368,
     T_Phone = 369,
     T_Pid = 370,
     T_Pidfile = 371,
     T_Pool = 372,
     T_Port = 373,
     T_Preempt = 374,
     T_Prefer = 375,
     T_Protostats = 376,
     T_Pw = 377,
     T_Qos = 378,
     T_Randfile = 379,
     T_Rawstats = 380,
     T_Refid = 381,
     T_Requestkey = 382,
     T_Restrict = 383,
     T_Revoke = 384,
     T_Saveconfigdir = 385,
     T_Server = 386,
     T_Setvar = 387,
     T_Sign = 388,
     T_Source = 389,
     T_Statistics = 390,
     T_Stats = 391,
     T_Statsdir = 392,
     T_Step = 393,
     T_Stepout = 394,
     T_Stratum = 395,
     T_String = 396,
     T_Sysstats = 397,
     T_Tick = 398,
     T_Time1 = 399,
     T_Time2 = 400,
     T_Timingstats = 401,
     T_Tinker = 402,
     T_Tos = 403,
     T_Trap = 404,
     T_True = 405,
     T_Trustedkey = 406,
     T_Ttl = 407,
     T_Type = 408,
     T_Unconfig = 409,
     T_Unpeer = 410,
     T_Version = 411,
     T_WanderThreshold = 412,
     T_Week = 413,
     T_Wildcard = 414,
     T_Xleave = 415,
     T_Year = 416,
     T_Flag = 417,
     T_Void = 418,
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
#define T_Bias 267
#define T_Broadcast 268
#define T_Broadcastclient 269
#define T_Broadcastdelay 270
#define T_Burst 271
#define T_Calibrate 272
#define T_Calldelay 273
#define T_Ceiling 274
#define T_Clockstats 275
#define T_Cohort 276
#define T_ControlKey 277
#define T_Crypto 278
#define T_Cryptostats 279
#define T_Day 280
#define T_Default 281
#define T_Digest 282
#define T_Disable 283
#define T_Discard 284
#define T_Dispersion 285
#define T_Double 286
#define T_Driftfile 287
#define T_Drop 288
#define T_Enable 289
#define T_End 290
#define T_False 291
#define T_File 292
#define T_Filegen 293
#define T_Flag1 294
#define T_Flag2 295
#define T_Flag3 296
#define T_Flag4 297
#define T_Flake 298
#define T_Floor 299
#define T_Freq 300
#define T_Fudge 301
#define T_Host 302
#define T_Huffpuff 303
#define T_Iburst 304
#define T_Ident 305
#define T_Ignore 306
#define T_Incalloc 307
#define T_Incmem 308
#define T_Initalloc 309
#define T_Initmem 310
#define T_Includefile 311
#define T_Integer 312
#define T_Interface 313
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
#define T_Nopeer 356
#define T_Noquery 357
#define T_Noselect 358
#define T_Noserve 359
#define T_Notrap 360
#define T_Notrust 361
#define T_Ntp 362
#define T_Ntpport 363
#define T_NtpSignDsocket 364
#define T_Orphan 365
#define T_Panic 366
#define T_Peer 367
#define T_Peerstats 368
#define T_Phone 369
#define T_Pid 370
#define T_Pidfile 371
#define T_Pool 372
#define T_Port 373
#define T_Preempt 374
#define T_Prefer 375
#define T_Protostats 376
#define T_Pw 377
#define T_Qos 378
#define T_Randfile 379
#define T_Rawstats 380
#define T_Refid 381
#define T_Requestkey 382
#define T_Restrict 383
#define T_Revoke 384
#define T_Saveconfigdir 385
#define T_Server 386
#define T_Setvar 387
#define T_Sign 388
#define T_Source 389
#define T_Statistics 390
#define T_Stats 391
#define T_Statsdir 392
#define T_Step 393
#define T_Stepout 394
#define T_Stratum 395
#define T_String 396
#define T_Sysstats 397
#define T_Tick 398
#define T_Time1 399
#define T_Time2 400
#define T_Timingstats 401
#define T_Tinker 402
#define T_Tos 403
#define T_Trap 404
#define T_True 405
#define T_Trustedkey 406
#define T_Ttl 407
#define T_Type 408
#define T_Unconfig 409
#define T_Unpeer 410
#define T_Version 411
#define T_WanderThreshold 412
#define T_Week 413
#define T_Wildcard 414
#define T_Xleave 415
#define T_Year 416
#define T_Flag 417
#define T_Void 418
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
#line 50 "ntp_parser.y"

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
#line 508 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 520 "ntp_parser.c"

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
#define YYFINAL  185
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   704

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  178
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  70
/* YYNRULES -- Number of rules.  */
#define YYNRULES  257
/* YYNRULES -- Number of states.  */
#define YYNSTATES  406

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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   175,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   176,     2,   177,     2,     2,     2,     2,
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
      44,    47,    49,    51,    53,    55,    57,    59,    62,    65,
      67,    70,    72,    74,    77,    79,    81,    84,    87,    90,
      92,    94,    96,    98,   100,   103,   106,   109,   112,   114,
     116,   118,   121,   124,   127,   130,   133,   136,   139,   142,
     145,   148,   151,   153,   154,   157,   159,   162,   165,   168,
     171,   174,   177,   180,   183,   186,   188,   191,   194,   197,
     200,   203,   206,   209,   212,   215,   218,   221,   224,   228,
     231,   233,   235,   237,   239,   241,   243,   245,   247,   249,
     252,   254,   257,   260,   262,   264,   266,   268,   270,   272,
     274,   276,   278,   280,   282,   285,   288,   292,   298,   302,
     307,   312,   316,   317,   320,   322,   324,   326,   328,   330,
     332,   334,   336,   338,   340,   342,   344,   346,   348,   351,
     353,   356,   359,   362,   365,   367,   370,   373,   376,   379,
     382,   385,   388,   391,   395,   398,   400,   403,   406,   409,
     412,   415,   418,   421,   424,   427,   430,   433,   435,   437,
     439,   441,   443,   445,   447,   449,   452,   455,   457,   460,
     463,   466,   469,   472,   475,   478,   480,   484,   486,   489,
     492,   495,   498,   501,   504,   507,   510,   513,   516,   519,
     522,   526,   529,   532,   534,   537,   538,   543,   547,   550,
     552,   555,   558,   561,   563,   565,   569,   573,   575,   577,
     579,   581,   583,   585,   587,   589,   591,   594,   596,   599,
     601,   604,   606,   608,   610,   612,   614,   616,   622,   624,
     628,   631,   635,   639,   642,   644,   650,   655,   659,   662,
     664,   671,   675,   678,   682,   686,   690,   694
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     179,     0,    -1,   180,    -1,   180,   181,   164,    -1,   181,
     164,    -1,     1,   164,    -1,    -1,   182,    -1,   188,    -1,
     190,    -1,   191,    -1,   198,    -1,   204,    -1,   195,    -1,
     211,    -1,   214,    -1,   217,    -1,   220,    -1,   236,    -1,
     183,   184,   186,    -1,   183,   184,    -1,   131,    -1,   117,
      -1,   112,    -1,    13,    -1,    77,    -1,   185,    -1,    60,
     141,    -1,    62,   141,    -1,   141,    -1,   186,   187,    -1,
     187,    -1,     7,    -1,    12,   235,    -1,    16,    -1,    49,
      -1,    64,    57,    -1,    90,    57,    -1,    85,    57,    -1,
     103,    -1,   119,    -1,   120,    -1,   150,    -1,   160,    -1,
     152,    57,    -1,    92,    57,    -1,   156,    57,    -1,   189,
     184,    -1,   154,    -1,   155,    -1,    14,    -1,    78,   233,
      -1,    96,   233,    -1,     8,    57,    -1,    22,    57,    -1,
      23,   192,    -1,    65,   141,    -1,    66,   141,    -1,   127,
      57,    -1,   129,    57,    -1,   151,   231,    -1,   109,   141,
      -1,   193,    -1,    -1,   193,   194,    -1,   194,    -1,    47,
     141,    -1,    50,   141,    -1,   122,   141,    -1,   124,   141,
      -1,   133,   141,    -1,    27,   141,    -1,   129,    57,    -1,
     148,   196,    -1,   196,   197,    -1,   197,    -1,    19,    57,
      -1,    44,    57,    -1,    21,   234,    -1,   110,    57,    -1,
      88,   235,    -1,    83,   235,    -1,    86,   235,    -1,    81,
     235,    -1,    91,    57,    -1,    11,    57,    -1,   135,   199,
      -1,   137,   141,    -1,    38,   200,   201,    -1,   199,   200,
      -1,   200,    -1,    20,    -1,    24,    -1,    75,    -1,   113,
      -1,   125,    -1,   142,    -1,   146,    -1,   121,    -1,   201,
     202,    -1,   202,    -1,    37,   141,    -1,   153,   203,    -1,
      71,    -1,    98,    -1,    34,    -1,    28,    -1,   100,    -1,
     115,    -1,    25,    -1,   158,    -1,    94,    -1,   161,    -1,
       3,    -1,    29,   207,    -1,    95,   209,    -1,   128,   184,
     205,    -1,   128,   185,    79,   185,   205,    -1,   128,    26,
     205,    -1,   128,    60,    26,   205,    -1,   128,    62,    26,
     205,    -1,   128,   134,   205,    -1,    -1,   205,   206,    -1,
      43,    -1,    51,    -1,    67,    -1,    68,    -1,    70,    -1,
      76,    -1,    99,    -1,   101,    -1,   102,    -1,   104,    -1,
     105,    -1,   106,    -1,   108,    -1,   156,    -1,   207,   208,
      -1,   208,    -1,     9,    57,    -1,    89,    57,    -1,    93,
      57,    -1,   209,   210,    -1,   210,    -1,    52,    57,    -1,
      53,    57,    -1,    54,    57,    -1,    55,    57,    -1,    80,
      57,    -1,    82,    57,    -1,    84,    57,    -1,    87,    57,
      -1,    46,   184,   212,    -1,   212,   213,    -1,   213,    -1,
     144,   235,    -1,   145,   235,    -1,   140,    57,    -1,   126,
     141,    -1,    39,   234,    -1,    40,   234,    -1,    41,   234,
      -1,    42,   234,    -1,    34,   215,    -1,    28,   215,    -1,
     215,   216,    -1,   216,    -1,     6,    -1,    10,    -1,    17,
      -1,    63,    -1,    93,    -1,   107,    -1,   136,    -1,   147,
     218,    -1,   218,   219,    -1,   219,    -1,     5,   235,    -1,
      30,   235,    -1,    45,   235,    -1,    48,   235,    -1,   111,
     235,    -1,   138,   235,    -1,   139,   235,    -1,   227,    -1,
      56,   141,   181,    -1,    35,    -1,    15,   235,    -1,    18,
      57,    -1,   143,   235,    -1,    32,   221,    -1,    69,   141,
      -1,   116,   141,    -1,    74,   141,    -1,    73,   225,    -1,
     114,   232,    -1,   130,   141,    -1,   132,   222,    -1,   149,
     185,    -1,   149,   185,   223,    -1,   152,   231,    -1,   123,
     141,    -1,   141,    -1,   141,    31,    -1,    -1,   141,   175,
     141,    26,    -1,   141,   175,   141,    -1,   223,   224,    -1,
     224,    -1,   118,    57,    -1,    58,   185,    -1,   225,   226,
      -1,   226,    -1,   141,    -1,   228,   230,   229,    -1,   228,
     230,   141,    -1,    58,    -1,    97,    -1,     4,    -1,    59,
      -1,    61,    -1,   159,    -1,    72,    -1,    51,    -1,    33,
      -1,   231,    57,    -1,    57,    -1,   232,   141,    -1,   141,
      -1,   233,   184,    -1,   184,    -1,    57,    -1,   150,    -1,
      36,    -1,    57,    -1,    31,    -1,   237,   176,   238,   240,
     177,    -1,   165,    -1,   238,   239,   164,    -1,   239,   164,
      -1,   166,   175,   235,    -1,   167,   175,   235,    -1,   240,
     241,    -1,   241,    -1,   243,   176,   242,   244,   177,    -1,
     168,   175,   235,   164,    -1,   131,   175,   184,    -1,   244,
     245,    -1,   245,    -1,   169,   175,   235,   176,   246,   177,
      -1,   246,   247,   164,    -1,   247,   164,    -1,   170,   175,
     235,    -1,   171,   175,   235,    -1,   172,   175,   235,    -1,
     173,   175,   235,    -1,   174,   175,   235,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   308,   308,   312,   313,   314,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   348,
     354,   363,   364,   365,   366,   367,   371,   372,   373,   377,
     381,   382,   386,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   396,   397,   398,   399,   400,   409,   417,   418,
     428,   430,   432,   443,   445,   447,   452,   454,   456,   458,
     460,   462,   467,   469,   473,   480,   490,   492,   494,   496,
     498,   500,   502,   519,   524,   525,   529,   531,   533,   535,
     537,   539,   541,   543,   545,   547,   557,   559,   568,   576,
     577,   581,   582,   583,   584,   585,   586,   587,   588,   592,
     599,   609,   619,   628,   637,   646,   647,   651,   652,   653,
     654,   655,   656,   657,   666,   670,   674,   679,   684,   689,
     702,   715,   727,   728,   733,   734,   735,   736,   737,   738,
     739,   740,   741,   742,   743,   744,   745,   746,   750,   752,
     757,   758,   759,   763,   765,   770,   771,   772,   773,   774,
     775,   776,   777,   785,   790,   792,   797,   798,   799,   800,
     801,   802,   803,   804,   812,   814,   819,   826,   836,   837,
     838,   839,   840,   841,   842,   858,   862,   863,   867,   868,
     869,   870,   871,   872,   873,   882,   883,   899,   905,   907,
     909,   911,   913,   916,   918,   929,   931,   933,   943,   945,
     947,   949,   951,   956,   958,   962,   966,   968,   973,   975,
     979,   980,   984,   985,   989,  1004,  1009,  1017,  1018,  1022,
    1023,  1024,  1025,  1029,  1030,  1031,  1041,  1042,  1046,  1047,
    1051,  1052,  1056,  1065,  1066,  1070,  1071,  1080,  1095,  1099,
    1100,  1104,  1105,  1109,  1110,  1114,  1119,  1123,  1127,  1128,
    1132,  1137,  1138,  1142,  1144,  1146,  1148,  1150
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_Age", "T_All", "T_Allan", "T_Auth",
  "T_Autokey", "T_Automax", "T_Average", "T_Bclient", "T_Beacon", "T_Bias",
  "T_Broadcast", "T_Broadcastclient", "T_Broadcastdelay", "T_Burst",
  "T_Calibrate", "T_Calldelay", "T_Ceiling", "T_Clockstats", "T_Cohort",
  "T_ControlKey", "T_Crypto", "T_Cryptostats", "T_Day", "T_Default",
  "T_Digest", "T_Disable", "T_Discard", "T_Dispersion", "T_Double",
  "T_Driftfile", "T_Drop", "T_Enable", "T_End", "T_False", "T_File",
  "T_Filegen", "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4", "T_Flake",
  "T_Floor", "T_Freq", "T_Fudge", "T_Host", "T_Huffpuff", "T_Iburst",
  "T_Ident", "T_Ignore", "T_Incalloc", "T_Incmem", "T_Initalloc",
  "T_Initmem", "T_Includefile", "T_Integer", "T_Interface", "T_Ipv4",
  "T_Ipv4_flag", "T_Ipv6", "T_Ipv6_flag", "T_Kernel", "T_Key", "T_Keys",
  "T_Keysdir", "T_Kod", "T_Mssntp", "T_Leapfile", "T_Limited", "T_Link",
  "T_Listen", "T_Logconfig", "T_Logfile", "T_Loopstats", "T_Lowpriotrap",
  "T_Manycastclient", "T_Manycastserver", "T_Mask", "T_Maxage",
  "T_Maxclock", "T_Maxdepth", "T_Maxdist", "T_Maxmem", "T_Maxpoll",
  "T_Minclock", "T_Mindepth", "T_Mindist", "T_Minimum", "T_Minpoll",
  "T_Minsane", "T_Mode", "T_Monitor", "T_Month", "T_Mru",
  "T_Multicastclient", "T_Nic", "T_Nolink", "T_Nomodify", "T_None",
  "T_Nopeer", "T_Noquery", "T_Noselect", "T_Noserve", "T_Notrap",
  "T_Notrust", "T_Ntp", "T_Ntpport", "T_NtpSignDsocket", "T_Orphan",
  "T_Panic", "T_Peer", "T_Peerstats", "T_Phone", "T_Pid", "T_Pidfile",
  "T_Pool", "T_Port", "T_Preempt", "T_Prefer", "T_Protostats", "T_Pw",
  "T_Qos", "T_Randfile", "T_Rawstats", "T_Refid", "T_Requestkey",
  "T_Restrict", "T_Revoke", "T_Saveconfigdir", "T_Server", "T_Setvar",
  "T_Sign", "T_Source", "T_Statistics", "T_Stats", "T_Statsdir", "T_Step",
  "T_Stepout", "T_Stratum", "T_String", "T_Sysstats", "T_Tick", "T_Time1",
  "T_Time2", "T_Timingstats", "T_Tinker", "T_Tos", "T_Trap", "T_True",
  "T_Trustedkey", "T_Ttl", "T_Type", "T_Unconfig", "T_Unpeer", "T_Version",
  "T_WanderThreshold", "T_Week", "T_Wildcard", "T_Xleave", "T_Year",
  "T_Flag", "T_Void", "T_EOC", "T_Simulate", "T_Beep_Delay",
  "T_Sim_Duration", "T_Server_Offset", "T_Duration", "T_Freq_Offset",
  "T_Wander", "T_Jitter", "T_Prop_Delay", "T_Proc_Delay", "'='", "'{'",
  "'}'", "$accept", "configuration", "command_list", "command",
  "server_command", "client_type", "address", "ip_address", "option_list",
  "option", "unpeer_command", "unpeer_keyword", "other_mode_command",
  "authentication_command", "crypto_command_line", "crypto_command_list",
  "crypto_command", "orphan_mode_command", "tos_option_list", "tos_option",
  "monitoring_command", "stats_list", "stat", "filegen_option_list",
  "filegen_option", "filegen_type", "access_control_command",
  "ac_flag_list", "access_control_flag", "discard_option_list",
  "discard_option", "mru_option_list", "mru_option", "fudge_command",
  "fudge_factor_list", "fudge_factor", "system_option_command",
  "system_option_list", "system_option", "tinker_command",
  "tinker_option_list", "tinker_option", "miscellaneous_command",
  "drift_parm", "variable_assign", "trap_option_list", "trap_option",
  "log_config_list", "log_config_command", "interface_command",
  "interface_nic", "nic_rule_class", "nic_rule_action", "integer_list",
  "string_list", "address_list", "boolean", "number", "simulate_command",
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
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,    61,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   178,   179,   180,   180,   180,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   182,
     182,   183,   183,   183,   183,   183,   184,   184,   184,   185,
     186,   186,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   188,   189,   189,
     190,   190,   190,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   192,   192,   193,   193,   194,   194,   194,   194,
     194,   194,   194,   195,   196,   196,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   198,   198,   198,   199,
     199,   200,   200,   200,   200,   200,   200,   200,   200,   201,
     201,   202,   202,   202,   202,   202,   202,   203,   203,   203,
     203,   203,   203,   203,   204,   204,   204,   204,   204,   204,
     204,   204,   205,   205,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   207,   207,
     208,   208,   208,   209,   209,   210,   210,   210,   210,   210,
     210,   210,   210,   211,   212,   212,   213,   213,   213,   213,
     213,   213,   213,   213,   214,   214,   215,   215,   216,   216,
     216,   216,   216,   216,   216,   217,   218,   218,   219,   219,
     219,   219,   219,   219,   219,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   221,   221,   221,   222,   222,   223,   223,
     224,   224,   225,   225,   226,   227,   227,   228,   228,   229,
     229,   229,   229,   230,   230,   230,   231,   231,   232,   232,
     233,   233,   234,   234,   234,   235,   235,   236,   237,   238,
     238,   239,   239,   240,   240,   241,   242,   243,   244,   244,
     245,   246,   246,   247,   247,   247,   247,   247
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
       2,     2,     2,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     3,     5,     3,     4,
       4,     3,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     2,     2,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     3,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     1,     2,     2,
       2,     2,     2,     2,     2,     1,     3,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       3,     2,     2,     1,     2,     0,     4,     3,     2,     1,
       2,     2,     2,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     5,     1,     3,
       2,     3,     3,     2,     1,     5,     4,     3,     2,     1,
       6,     3,     2,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,    24,    50,     0,     0,     0,    63,     0,
       0,   205,     0,   187,     0,     0,     0,   217,     0,     0,
       0,     0,     0,    25,     0,     0,     0,   218,     0,    23,
       0,     0,    22,     0,     0,     0,     0,     0,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,    49,
     238,     0,     2,     0,     7,     0,     8,     0,     9,    10,
      13,    11,    12,    14,    15,    16,    17,   185,     0,    18,
       0,     5,    53,   236,   235,   188,   189,    54,     0,     0,
       0,     0,     0,     0,     0,    55,    62,    65,   168,   169,
     170,   171,   172,   173,   174,   165,   167,     0,     0,     0,
     114,   139,   203,   191,   164,    91,    92,    93,    94,    98,
      95,    96,    97,     0,     0,     0,    29,     0,    26,     6,
      56,    57,   192,   214,   195,   213,   194,   231,    51,     0,
       0,     0,     0,     0,     0,     0,     0,   115,   144,    52,
      61,   229,   196,   193,   202,    58,   122,     0,     0,   122,
     122,    26,    59,   197,     0,   198,    86,    90,    87,   190,
       0,     0,     0,     0,     0,     0,     0,   175,   177,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    73,
      75,   199,   227,    60,   201,     1,     0,     4,    20,    47,
     225,   224,   223,     0,     0,    71,    66,    67,    68,    69,
      72,    70,    64,   166,   140,   141,   142,   138,   204,   106,
     105,     0,   103,   104,     0,    88,   100,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,   153,   155,   186,
     212,   230,   145,   146,   147,   148,   149,   150,   151,   152,
     143,   228,   118,   122,   122,   121,   116,     0,     0,    89,
     178,   179,   180,   181,   182,   183,   184,   176,    85,    76,
     234,   232,   233,    78,    77,    83,    81,    82,    80,    84,
      79,    74,     0,     0,   200,   209,   226,     3,    32,     0,
      34,    35,     0,     0,     0,     0,    39,    40,    41,    42,
       0,     0,    43,    19,    31,   219,   220,   221,   216,   222,
     215,     0,     0,     0,     0,   101,   113,   109,   111,   107,
     108,   110,   112,   102,    99,   160,   161,   162,   163,   159,
     158,   156,   157,   154,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   123,   119,
     120,   122,   207,   211,   210,   208,    33,    36,    38,    37,
      45,    44,    46,    30,     0,     0,     0,     0,     0,   244,
       0,   240,   117,   206,   241,   242,     0,   239,   237,   243,
       0,   247,     0,     0,     0,     0,     0,   249,     0,     0,
     245,   248,   246,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   250,     0,   252,
     253,   254,   255,   256,   257,   251
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    51,    52,    53,    54,    55,   127,   118,   293,   294,
      56,    57,    58,    59,    85,    86,    87,    60,   179,   180,
      61,   156,   113,   215,   216,   313,    62,   242,   338,   100,
     101,   137,   138,    63,   227,   228,    64,    95,    96,    65,
     167,   168,    66,   103,   155,   274,   275,   124,   125,    67,
      68,   300,   193,   183,   142,   128,   263,    75,    69,    70,
     303,   304,   358,   359,   373,   360,   376,   377,   390,   391
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -148
static const yytype_int16 yypact[] =
{
     134,  -132,   -16,  -148,  -148,     7,   -15,    -8,   -19,     8,
      -2,   -91,     8,  -148,    33,   -43,   -86,  -148,   -81,   -79,
     -68,   -67,   -65,  -148,   -43,   165,   -43,  -148,   -63,  -148,
     -62,   -57,  -148,   -56,    29,    -4,    31,   -52,  -148,   -41,
      33,   -39,     7,     0,   299,   -32,    42,    42,  -148,  -148,
    -148,   107,   301,   -51,  -148,   -43,  -148,   -43,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,     3,  -148,
     -60,  -148,  -148,  -148,  -148,  -148,  -148,  -148,   -12,    -1,
       4,    10,    14,    84,    19,  -148,   -19,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,     8,  -148,    96,   108,   114,
      -2,  -148,   133,  -148,     8,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,     6,    35,    36,  -148,    41,  -148,   446,
    -148,  -148,  -148,  -148,   -67,  -148,  -148,  -148,   -43,   117,
     121,   125,   130,   131,   132,   136,   138,   165,  -148,   -43,
    -148,  -148,    50,  -148,  -148,  -148,  -148,    -5,     9,  -148,
    -148,   118,  -148,  -148,    21,  -148,    33,  -148,  -148,  -148,
       7,     7,     7,     7,     7,     7,     7,     0,  -148,   141,
     144,   -24,   145,     7,     7,     7,     7,   147,   148,   299,
    -148,   -48,  -148,   149,   149,  -148,    45,  -148,   518,  -148,
    -148,  -148,  -148,     2,  -140,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,    69,  -148,  -148,    12,     6,  -148,  -148,  -148,   -24,
     -24,   -24,   -24,    73,   158,     7,     7,    41,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,   548,  -148,  -148,   548,   548,   -32,    75,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,   -32,   169,   -48,  -148,  -148,  -148,  -148,     7,
    -148,  -148,   170,   171,   175,   176,  -148,  -148,  -148,  -148,
     177,   178,  -148,   518,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,    61,    64,  -115,    76,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,   548,
     548,  -148,   215,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,     7,     7,    67,    80,  -118,  -148,
      77,  -148,   548,  -148,  -148,  -148,   -43,  -148,  -148,  -148,
      86,  -148,    81,    89,     7,    85,  -130,  -148,    91,     7,
    -148,  -148,  -148,    83,    51,    92,    93,    95,    97,    98,
    -105,   110,     7,     7,     7,     7,     7,  -148,   111,  -148,
    -148,  -148,  -148,  -148,  -148,  -148
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -148,  -148,  -148,   -29,  -148,  -148,   -11,   -34,  -148,   -17,
    -148,  -148,  -148,  -148,  -148,  -148,   192,  -148,  -148,   100,
    -148,  -148,   -31,  -148,    65,  -148,  -148,  -147,  -148,  -148,
     184,  -148,   150,  -148,  -148,    63,  -148,   279,   -75,  -148,
    -148,   126,  -148,  -148,  -148,  -148,    18,  -148,   172,  -148,
    -148,  -148,  -148,   247,  -148,   269,  -127,   -42,  -148,  -148,
    -148,    -6,  -148,   -58,  -148,  -148,  -148,   -74,  -148,   -89
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     159,   151,   245,   246,   117,   160,   295,    97,    78,   157,
     272,   181,   260,   356,    88,   306,   356,   114,    89,   115,
     203,   243,   146,   186,   150,    90,   301,   302,    79,   203,
     161,    80,    71,   261,   209,   244,   190,   307,    73,   375,
     210,    72,    76,   211,   188,   162,   189,   380,   163,    77,
     102,   301,   302,   105,   191,   119,   147,   106,   148,   368,
     120,   296,   121,   297,    74,   385,   386,   387,   388,   389,
     273,    91,   397,   122,   123,   192,   126,   212,   140,   141,
     219,   220,   221,   222,   143,   144,   145,    98,   152,   153,
     229,    99,   315,   316,   317,   318,   339,   340,   116,   182,
     154,    92,   158,    81,   213,    82,   308,   185,   107,   116,
      83,   164,   309,   187,    84,    93,   194,   231,   250,   251,
     252,   253,   254,   255,   256,   249,   262,   310,   231,   195,
     149,   265,   266,   267,   268,     1,   217,   116,   165,   166,
     196,   200,     2,   298,    94,   197,   108,     3,     4,     5,
     218,   198,     6,   204,   109,   199,     7,     8,   110,   214,
     201,   299,     9,    10,   208,   205,    11,   223,    12,    13,
     311,   206,    14,   312,   232,   111,   217,   218,   233,   112,
      15,   224,   234,   321,   322,   225,   226,   235,   236,   237,
      16,   241,    17,   238,   362,   239,   248,   247,   258,    18,
      19,   259,   264,    20,   269,   270,   276,    21,    22,   277,
     305,    23,    24,   341,   319,   320,   342,   129,   130,   131,
     132,   385,   386,   387,   388,   389,   344,   347,   348,    25,
      26,    27,   349,   350,   351,   352,   354,   346,   343,   355,
     361,   363,   366,    28,   367,   133,    29,   134,    30,   135,
      31,    32,   136,   370,   372,   382,   374,    33,   375,   384,
     379,    34,    35,    36,    37,    38,    39,   392,   393,    40,
     394,    41,   395,   396,   399,   405,   353,    42,   202,   271,
     314,    43,    44,    45,   207,    46,    47,   240,    48,    49,
     323,   104,   345,   257,   184,   139,   230,   357,    -6,    50,
     369,   398,   381,     0,     0,     0,     0,     0,     0,     2,
     169,     0,   364,   365,     3,     4,     5,     0,   170,     6,
     171,     0,     0,     7,     8,     0,     0,     0,     0,     9,
      10,     0,   378,    11,     0,    12,    13,   383,     0,    14,
       0,     0,     0,   172,     0,     0,     0,    15,     0,     0,
     400,   401,   402,   403,   404,   371,     0,    16,     0,    17,
       0,     0,     0,     0,     0,     0,    18,    19,     0,     0,
      20,     0,     0,     0,    21,    22,     0,     0,    23,    24,
     173,     0,   174,     0,     0,   175,     0,   176,     0,     0,
     177,     0,     0,     0,     0,     0,    25,    26,    27,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   178,
      28,     0,     0,    29,     0,    30,     0,    31,    32,     0,
       0,     0,     0,     0,    33,     0,     0,     0,    34,    35,
      36,    37,    38,    39,     0,     0,    40,     0,    41,     0,
       0,     0,     0,     0,    42,     0,     0,     0,    43,    44,
      45,     0,    46,    47,     2,    48,    49,     0,     0,     3,
       4,     5,     0,     0,     6,    -6,    50,     0,     7,     8,
       0,     0,     0,     0,     9,    10,     0,     0,    11,     0,
      12,    13,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,    15,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    16,     0,    17,     0,     0,     0,     0,     0,
       0,    18,    19,     0,     0,    20,     0,     0,     0,    21,
      22,     0,     0,    23,    24,   278,     0,     0,     0,     0,
     279,     0,     0,     0,   280,     0,     0,     0,     0,     0,
       0,    25,    26,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    28,     0,     0,    29,     0,
      30,     0,    31,    32,     0,     0,     0,   281,     0,    33,
       0,     0,     0,    34,    35,    36,    37,    38,    39,     0,
       0,    40,   282,    41,     0,     0,     0,     0,     0,    42,
       0,   324,     0,    43,    44,    45,     0,    46,    47,   325,
      48,    49,     0,   283,     0,     0,     0,     0,   284,     0,
     285,    50,     0,     0,     0,   326,   327,     0,   328,     0,
       0,   286,     0,     0,   329,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   287,   288,     0,
       0,     0,     0,     0,     0,     0,     0,   330,     0,   331,
     332,     0,   333,   334,   335,     0,   336,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   289,     0,
     290,     0,     0,     0,   291,     0,     0,     0,   292,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   337
};

static const yytype_int16 yycheck[] =
{
      42,    35,   149,   150,    15,     5,     4,     9,    27,    40,
      58,    45,    36,   131,     6,     3,   131,    60,    10,    62,
      95,    26,    26,    52,    35,    17,   166,   167,    47,   104,
      30,    50,   164,    57,    28,    26,    33,    25,    31,   169,
      34,    57,    57,    37,    55,    45,    57,   177,    48,    57,
     141,   166,   167,    20,    51,   141,    60,    24,    62,   177,
     141,    59,   141,    61,    57,   170,   171,   172,   173,   174,
     118,    63,   177,   141,   141,    72,   141,    71,   141,   141,
      39,    40,    41,    42,   141,   141,    57,    89,    57,   141,
     119,    93,   219,   220,   221,   222,   243,   244,   141,    57,
     141,    93,   141,   122,    98,   124,    94,     0,    75,   141,
     129,   111,   100,   164,   133,   107,   176,   128,   160,   161,
     162,   163,   164,   165,   166,   156,   150,   115,   139,   141,
     134,   173,   174,   175,   176,     1,   141,   141,   138,   139,
     141,    57,     8,   141,   136,   141,   113,    13,    14,    15,
     141,   141,    18,    57,   121,   141,    22,    23,   125,   153,
     141,   159,    28,    29,    31,    57,    32,   126,    34,    35,
     158,    57,    38,   161,    57,   142,   141,   141,    57,   146,
      46,   140,    57,   225,   226,   144,   145,    57,    57,    57,
      56,   141,    58,    57,   341,    57,   175,    79,    57,    65,
      66,    57,    57,    69,    57,    57,    57,    73,    74,   164,
     141,    77,    78,   247,   141,    57,   141,    52,    53,    54,
      55,   170,   171,   172,   173,   174,    57,    57,    57,    95,
      96,    97,    57,    57,    57,    57,   175,   279,   272,   175,
     164,    26,   175,   109,   164,    80,   112,    82,   114,    84,
     116,   117,    87,   176,   168,   164,   175,   123,   169,   176,
     175,   127,   128,   129,   130,   131,   132,   175,   175,   135,
     175,   137,   175,   175,   164,   164,   293,   143,    86,   179,
     215,   147,   148,   149,   100,   151,   152,   137,   154,   155,
     227,    12,   274,   167,    47,    26,   124,   303,   164,   165,
     358,   390,   376,    -1,    -1,    -1,    -1,    -1,    -1,     8,
      11,    -1,   354,   355,    13,    14,    15,    -1,    19,    18,
      21,    -1,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,
      29,    -1,   374,    32,    -1,    34,    35,   379,    -1,    38,
      -1,    -1,    -1,    44,    -1,    -1,    -1,    46,    -1,    -1,
     392,   393,   394,   395,   396,   366,    -1,    56,    -1,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    -1,    -1,
      69,    -1,    -1,    -1,    73,    74,    -1,    -1,    77,    78,
      81,    -1,    83,    -1,    -1,    86,    -1,    88,    -1,    -1,
      91,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,
     109,    -1,    -1,   112,    -1,   114,    -1,   116,   117,    -1,
      -1,    -1,    -1,    -1,   123,    -1,    -1,    -1,   127,   128,
     129,   130,   131,   132,    -1,    -1,   135,    -1,   137,    -1,
      -1,    -1,    -1,    -1,   143,    -1,    -1,    -1,   147,   148,
     149,    -1,   151,   152,     8,   154,   155,    -1,    -1,    13,
      14,    15,    -1,    -1,    18,   164,   165,    -1,    22,    23,
      -1,    -1,    -1,    -1,    28,    29,    -1,    -1,    32,    -1,
      34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    56,    -1,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    65,    66,    -1,    -1,    69,    -1,    -1,    -1,    73,
      74,    -1,    -1,    77,    78,     7,    -1,    -1,    -1,    -1,
      12,    -1,    -1,    -1,    16,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    96,    97,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   109,    -1,    -1,   112,    -1,
     114,    -1,   116,   117,    -1,    -1,    -1,    49,    -1,   123,
      -1,    -1,    -1,   127,   128,   129,   130,   131,   132,    -1,
      -1,   135,    64,   137,    -1,    -1,    -1,    -1,    -1,   143,
      -1,    43,    -1,   147,   148,   149,    -1,   151,   152,    51,
     154,   155,    -1,    85,    -1,    -1,    -1,    -1,    90,    -1,
      92,   165,    -1,    -1,    -1,    67,    68,    -1,    70,    -1,
      -1,   103,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,   120,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,    -1,   101,
     102,    -1,   104,   105,   106,    -1,   108,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   150,    -1,
     152,    -1,    -1,    -1,   156,    -1,    -1,    -1,   160,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   156
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     8,    13,    14,    15,    18,    22,    23,    28,
      29,    32,    34,    35,    38,    46,    56,    58,    65,    66,
      69,    73,    74,    77,    78,    95,    96,    97,   109,   112,
     114,   116,   117,   123,   127,   128,   129,   130,   131,   132,
     135,   137,   143,   147,   148,   149,   151,   152,   154,   155,
     165,   179,   180,   181,   182,   183,   188,   189,   190,   191,
     195,   198,   204,   211,   214,   217,   220,   227,   228,   236,
     237,   164,    57,    31,    57,   235,    57,    57,    27,    47,
      50,   122,   124,   129,   133,   192,   193,   194,     6,    10,
      17,    63,    93,   107,   136,   215,   216,     9,    89,    93,
     207,   208,   141,   221,   215,    20,    24,    75,   113,   121,
     125,   142,   146,   200,    60,    62,   141,   184,   185,   141,
     141,   141,   141,   141,   225,   226,   141,   184,   233,    52,
      53,    54,    55,    80,    82,    84,    87,   209,   210,   233,
     141,   141,   232,   141,   141,    57,    26,    60,    62,   134,
     184,   185,    57,   141,   141,   222,   199,   200,   141,   235,
       5,    30,    45,    48,   111,   138,   139,   218,   219,    11,
      19,    21,    44,    81,    83,    86,    88,    91,   110,   196,
     197,   185,    57,   231,   231,     0,   181,   164,   184,   184,
      33,    51,    72,   230,   176,   141,   141,   141,   141,   141,
      57,   141,   194,   216,    57,    57,    57,   208,    31,    28,
      34,    37,    71,    98,   153,   201,   202,   141,   141,    39,
      40,    41,    42,   126,   140,   144,   145,   212,   213,   181,
     226,   184,    57,    57,    57,    57,    57,    57,    57,    57,
     210,   141,   205,    26,    26,   205,   205,    79,   175,   200,
     235,   235,   235,   235,   235,   235,   235,   219,    57,    57,
      36,    57,   150,   234,    57,   235,   235,   235,   235,    57,
      57,   197,    58,   118,   223,   224,    57,   164,     7,    12,
      16,    49,    64,    85,    90,    92,   103,   119,   120,   150,
     152,   156,   160,   186,   187,     4,    59,    61,   141,   159,
     229,   166,   167,   238,   239,   141,     3,    25,    94,   100,
     115,   158,   161,   203,   202,   234,   234,   234,   234,   141,
      57,   235,   235,   213,    43,    51,    67,    68,    70,    76,
      99,   101,   102,   104,   105,   106,   108,   156,   206,   205,
     205,   185,   141,   185,    57,   224,   235,    57,    57,    57,
      57,    57,    57,   187,   175,   175,   131,   239,   240,   241,
     243,   164,   205,    26,   235,   235,   175,   164,   177,   241,
     176,   184,   168,   242,   175,   169,   244,   245,   235,   175,
     177,   245,   164,   235,   176,   170,   171,   172,   173,   174,
     246,   247,   175,   175,   175,   175,   175,   177,   247,   164,
     235,   235,   235,   235,   235,   164
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
#line 315 "ntp_parser.y"
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

/* Line 1455 of yacc.c  */
#line 349 "ntp_parser.y"
    {
			struct peer_node *my_node =  create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue));
			if (my_node)
				enqueue(cfgt.peers, my_node);
		}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 355 "ntp_parser.y"
    {
			struct peer_node *my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
			if (my_node)
				enqueue(cfgt.peers, my_node);
		}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 372 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 373 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET6); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 377 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), 0); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 381 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 382 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 386 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 387 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 388 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 389 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 390 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 391 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 392 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 393 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 394 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 395 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 396 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 397 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 398 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 399 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 400 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 410 "ntp_parser.y"
    {
			struct unpeer_node *my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				enqueue(cfgt.unpeers, my_node);
		}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 429 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 431 "ntp_parser.y"
    { append_queue(cfgt.manycastserver, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 433 "ntp_parser.y"
    { append_queue(cfgt.multicastclient, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 444 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer))); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 446 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 448 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			append_queue(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 453 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 455 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 457 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 459 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 461 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Queue); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 463 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 469 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 474 "ntp_parser.y"
    { 
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 481 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 491 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 493 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 495 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 497 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 499 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 501 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 503 "ntp_parser.y"
    {
			(yyval.Attr_val) = NULL;
			cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer);
			msyslog(LOG_WARNING,
				"'crypto revoke %d' is deprecated, "
				"please use 'revoke %d' instead.",
				cfgt.auth.revoke, cfgt.auth.revoke);
		}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 520 "ntp_parser.y"
    { append_queue(cfgt.orphan_cmds,(yyvsp[(2) - (2)].Queue)); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 524 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 525 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 530 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 532 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 534 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 536 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 538 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 540 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 542 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 544 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 546 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 548 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 558 "ntp_parser.y"
    { append_queue(cfgt.stats_list, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 560 "ntp_parser.y"
    {
			if (input_from_file)
				cfgt.stats_dir = (yyvsp[(2) - (2)].String);
			else {
				free((yyvsp[(2) - (2)].String));
				yyerror("statsdir remote configuration ignored");
			}
		}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 569 "ntp_parser.y"
    {
			enqueue(cfgt.filegen_opts,
				create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Queue)));
		}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 576 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 577 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 593 "ntp_parser.y"
    {
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 600 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 610 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			else {
				(yyval.Attr_val) = NULL;
				free((yyvsp[(2) - (2)].String));
				yyerror("filegen file remote configuration ignored");
			}
		}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 620 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen type remote configuration ignored");
			}
		}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 629 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen link remote configuration ignored");
			}
		}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 638 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen nolink remote configuration ignored");
			}
		}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 646 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 647 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 667 "ntp_parser.y"
    {
			append_queue(cfgt.discard_opts, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 671 "ntp_parser.y"
    {
			append_queue(cfgt.mru_opts, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 675 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 680 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Queue), ip_file->line_no));
		}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 685 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 690 "ntp_parser.y"
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

  case 120:

/* Line 1455 of yacc.c  */
#line 703 "ntp_parser.y"
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

  case 121:

/* Line 1455 of yacc.c  */
#line 716 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(
					NULL, NULL,
					enqueue((yyvsp[(3) - (3)].Queue), create_ival((yyvsp[(2) - (3)].Integer))),
					ip_file->line_no));
		}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 727 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 729 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 751 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 753 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 757 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 758 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 759 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 764 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 766 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 770 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 771 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 772 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 773 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 774 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 775 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 776 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 777 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 786 "ntp_parser.y"
    { enqueue(cfgt.fudge, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 791 "ntp_parser.y"
    { enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 793 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 797 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 798 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 799 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 800 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 801 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 802 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 803 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 804 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 813 "ntp_parser.y"
    { append_queue(cfgt.enable_opts, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 815 "ntp_parser.y"
    { append_queue(cfgt.disable_opts, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 820 "ntp_parser.y"
    {
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 827 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 836 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 837 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 838 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 839 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 840 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 841 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 843 "ntp_parser.y"
    { 
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("enable/disable stats remote configuration ignored");
			}
		}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 858 "ntp_parser.y"
    { append_queue(cfgt.tinker, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 862 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 863 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 867 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 868 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 869 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 870 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 871 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 872 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 873 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 884 "ntp_parser.y"
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

  case 187:

/* Line 1455 of yacc.c  */
#line 900 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 906 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double))); }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 908 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer))); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 910 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double))); }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 912 "ntp_parser.y"
    { /* Null action, possibly all null parms */ }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 914 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 917 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 919 "ntp_parser.y"
    {
			if (input_from_file)
				enqueue(cfgt.vars,
					create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)));
			else {
				free((yyvsp[(2) - (2)].String));
				yyerror("logfile remote configuration ignored");
			}
		}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 930 "ntp_parser.y"
    { append_queue(cfgt.logconfig, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 932 "ntp_parser.y"
    { append_queue(cfgt.phone, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 934 "ntp_parser.y"
    {
			if (input_from_file)
				enqueue(cfgt.vars,
					create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)));
			else {
				free((yyvsp[(2) - (2)].String));
				yyerror("saveconfigdir remote configuration ignored");
			}
		}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 944 "ntp_parser.y"
    { enqueue(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 946 "ntp_parser.y"
    { enqueue(cfgt.trap, create_addr_opts_node((yyvsp[(2) - (2)].Address_node), NULL)); }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 948 "ntp_parser.y"
    { enqueue(cfgt.trap, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 950 "ntp_parser.y"
    { append_queue(cfgt.ttl, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 952 "ntp_parser.y"
    { enqueue(cfgt.qos, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 957 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String))); }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 959 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double)));
			  enqueue(cfgt.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String))); }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 962 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 967 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 969 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (3)].String), (yyvsp[(3) - (3)].String), 0); }
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 974 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 975 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 979 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 980 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_pval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 984 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 985 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 990 "ntp_parser.y"
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

  case 215:

/* Line 1455 of yacc.c  */
#line 1005 "ntp_parser.y"
    {
			enqueue(cfgt.nic_rules,
				create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer)));
		}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 1010 "ntp_parser.y"
    {
			enqueue(cfgt.nic_rules,
				create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer)));
		}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 1041 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 1042 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 1046 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_pval((yyvsp[(2) - (2)].String))); }
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 1047 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_pval((yyvsp[(1) - (1)].String))); }
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 1051 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 1052 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Address_node)); }
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 1057 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			}
			else
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
		}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 1065 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 1066 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 1070 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 1081 "ntp_parser.y"
    {
			cfgt.sim_details = create_sim_node((yyvsp[(3) - (5)].Queue), (yyvsp[(4) - (5)].Queue));

			/* Reset the old_config_style variable */
			old_config_style = 1;
		}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 1095 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 1099 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 1100 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 1104 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 1105 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 1109 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_server)); }
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 1110 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_server)); }
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 1115 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Queue)); }
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 1119 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 1123 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 1127 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_script)); }
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 1128 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_script)); }
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 1133 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Queue)); }
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 1137 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 1138 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 1143 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 1145 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 1147 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 1149 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 1151 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1455 of yacc.c  */
#line 3770 "ntp_parser.c"
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
#line 1155 "ntp_parser.y"


void yyerror (char *msg)
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


