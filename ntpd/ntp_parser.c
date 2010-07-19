
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
     T_Ipv4 = 315,
     T_Ipv4_flag = 316,
     T_Ipv6 = 317,
     T_Ipv6_flag = 318,
     T_Kernel = 319,
     T_Key = 320,
     T_Keys = 321,
     T_Keysdir = 322,
     T_Kod = 323,
     T_Mssntp = 324,
     T_Leapfile = 325,
     T_Limited = 326,
     T_Link = 327,
     T_Listen = 328,
     T_Logconfig = 329,
     T_Logfile = 330,
     T_Loopstats = 331,
     T_Lowpriotrap = 332,
     T_Manycastclient = 333,
     T_Manycastserver = 334,
     T_Mask = 335,
     T_Maxage = 336,
     T_Maxclock = 337,
     T_Maxdepth = 338,
     T_Maxdist = 339,
     T_Maxmem = 340,
     T_Maxpoll = 341,
     T_Minclock = 342,
     T_Mindepth = 343,
     T_Mindist = 344,
     T_Minimum = 345,
     T_Minpoll = 346,
     T_Minsane = 347,
     T_Mode = 348,
     T_Monitor = 349,
     T_Month = 350,
     T_Mru = 351,
     T_Multicastclient = 352,
     T_Nic = 353,
     T_Nolink = 354,
     T_Nomodify = 355,
     T_None = 356,
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
     T_Sign = 390,
     T_Source = 391,
     T_Statistics = 392,
     T_Stats = 393,
     T_Statsdir = 394,
     T_Step = 395,
     T_Stepout = 396,
     T_Stratum = 397,
     T_String = 398,
     T_Sysstats = 399,
     T_Tick = 400,
     T_Time1 = 401,
     T_Time2 = 402,
     T_Timingstats = 403,
     T_Tinker = 404,
     T_Tos = 405,
     T_Trap = 406,
     T_True = 407,
     T_Trustedkey = 408,
     T_Ttl = 409,
     T_Type = 410,
     T_Unconfig = 411,
     T_Unpeer = 412,
     T_Version = 413,
     T_WanderThreshold = 414,
     T_Week = 415,
     T_Wildcard = 416,
     T_Xleave = 417,
     T_Year = 418,
     T_Flag = 419,
     T_Void = 420,
     T_EOC = 421,
     T_Simulate = 422,
     T_Beep_Delay = 423,
     T_Sim_Duration = 424,
     T_Server_Offset = 425,
     T_Duration = 426,
     T_Freq_Offset = 427,
     T_Wander = 428,
     T_Jitter = 429,
     T_Prop_Delay = 430,
     T_Proc_Delay = 431
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
#define T_Ipv4 315
#define T_Ipv4_flag 316
#define T_Ipv6 317
#define T_Ipv6_flag 318
#define T_Kernel 319
#define T_Key 320
#define T_Keys 321
#define T_Keysdir 322
#define T_Kod 323
#define T_Mssntp 324
#define T_Leapfile 325
#define T_Limited 326
#define T_Link 327
#define T_Listen 328
#define T_Logconfig 329
#define T_Logfile 330
#define T_Loopstats 331
#define T_Lowpriotrap 332
#define T_Manycastclient 333
#define T_Manycastserver 334
#define T_Mask 335
#define T_Maxage 336
#define T_Maxclock 337
#define T_Maxdepth 338
#define T_Maxdist 339
#define T_Maxmem 340
#define T_Maxpoll 341
#define T_Minclock 342
#define T_Mindepth 343
#define T_Mindist 344
#define T_Minimum 345
#define T_Minpoll 346
#define T_Minsane 347
#define T_Mode 348
#define T_Monitor 349
#define T_Month 350
#define T_Mru 351
#define T_Multicastclient 352
#define T_Nic 353
#define T_Nolink 354
#define T_Nomodify 355
#define T_None 356
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
#define T_Sign 390
#define T_Source 391
#define T_Statistics 392
#define T_Stats 393
#define T_Statsdir 394
#define T_Step 395
#define T_Stepout 396
#define T_Stratum 397
#define T_String 398
#define T_Sysstats 399
#define T_Tick 400
#define T_Time1 401
#define T_Time2 402
#define T_Timingstats 403
#define T_Tinker 404
#define T_Tos 405
#define T_Trap 406
#define T_True 407
#define T_Trustedkey 408
#define T_Ttl 409
#define T_Type 410
#define T_Unconfig 411
#define T_Unpeer 412
#define T_Version 413
#define T_WanderThreshold 414
#define T_Week 415
#define T_Wildcard 416
#define T_Xleave 417
#define T_Year 418
#define T_Flag 419
#define T_Void 420
#define T_EOC 421
#define T_Simulate 422
#define T_Beep_Delay 423
#define T_Sim_Duration 424
#define T_Server_Offset 425
#define T_Duration 426
#define T_Freq_Offset 427
#define T_Wander 428
#define T_Jitter 429
#define T_Prop_Delay 430
#define T_Proc_Delay 431




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
#line 512 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 524 "ntp_parser.c"

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
#define YYFINAL  190
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   718

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  182
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  73
/* YYNRULES -- Number of rules.  */
#define YYNRULES  263
/* YYNRULES -- Number of states.  */
#define YYNSTATES  417

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   431

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     178,   179,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   177,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   180,     2,   181,     2,     2,     2,     2,
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
     175,   176
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
     200,   203,   206,   209,   212,   215,   218,   221,   224,   227,
     231,   234,   236,   238,   240,   242,   244,   246,   248,   250,
     252,   255,   257,   260,   263,   265,   267,   269,   271,   273,
     275,   277,   279,   281,   283,   285,   288,   291,   295,   301,
     305,   310,   315,   319,   320,   323,   325,   327,   329,   331,
     333,   335,   337,   339,   341,   343,   345,   347,   349,   351,
     354,   356,   359,   362,   365,   368,   370,   373,   376,   379,
     382,   385,   388,   391,   394,   398,   401,   403,   406,   409,
     412,   415,   418,   421,   424,   427,   430,   433,   436,   438,
     440,   442,   444,   446,   448,   450,   452,   455,   458,   460,
     463,   466,   469,   472,   475,   478,   481,   483,   487,   489,
     492,   495,   498,   501,   504,   507,   510,   513,   516,   519,
     522,   525,   529,   532,   535,   537,   540,   541,   546,   550,
     553,   555,   558,   561,   564,   566,   568,   572,   576,   578,
     580,   582,   584,   586,   588,   590,   592,   594,   597,   599,
     602,   604,   606,   608,   614,   617,   619,   622,   624,   626,
     628,   630,   632,   634,   640,   642,   646,   649,   653,   657,
     660,   662,   668,   673,   677,   680,   682,   689,   693,   696,
     700,   704,   708,   712
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     183,     0,    -1,   184,    -1,   184,   185,   166,    -1,   185,
     166,    -1,     1,   166,    -1,    -1,   186,    -1,   192,    -1,
     194,    -1,   195,    -1,   202,    -1,   208,    -1,   199,    -1,
     215,    -1,   218,    -1,   221,    -1,   224,    -1,   243,    -1,
     187,   188,   190,    -1,   187,   188,    -1,   133,    -1,   119,
      -1,   114,    -1,    13,    -1,    78,    -1,   189,    -1,    61,
     143,    -1,    63,   143,    -1,   143,    -1,   190,   191,    -1,
     191,    -1,     7,    -1,    12,   242,    -1,    16,    -1,    50,
      -1,    65,    58,    -1,    91,    58,    -1,    86,    58,    -1,
     104,    -1,   121,    -1,   122,    -1,   152,    -1,   162,    -1,
     154,    58,    -1,    93,    58,    -1,   158,    58,    -1,   193,
     188,    -1,   156,    -1,   157,    -1,    14,    -1,    79,   240,
      -1,    97,   240,    -1,     8,    58,    -1,    22,    58,    -1,
      23,   196,    -1,    66,   143,    -1,    67,   143,    -1,   129,
      58,    -1,   131,    58,    -1,   153,   236,    -1,   110,   143,
      -1,   197,    -1,    -1,   197,   198,    -1,   198,    -1,    48,
     143,    -1,    51,   143,    -1,   124,   143,    -1,   126,   143,
      -1,   135,   143,    -1,    27,   143,    -1,   131,    58,    -1,
     150,   200,    -1,   200,   201,    -1,   201,    -1,    19,    58,
      -1,    45,    58,    -1,    21,   241,    -1,   111,    58,    -1,
     112,    58,    -1,    89,   242,    -1,    84,   242,    -1,    87,
     242,    -1,    82,   242,    -1,    92,    58,    -1,    11,    58,
      -1,   137,   203,    -1,   139,   143,    -1,    39,   204,   205,
      -1,   203,   204,    -1,   204,    -1,    20,    -1,    24,    -1,
      76,    -1,   115,    -1,   127,    -1,   144,    -1,   148,    -1,
     123,    -1,   205,   206,    -1,   206,    -1,    38,   143,    -1,
     155,   207,    -1,    72,    -1,    99,    -1,    35,    -1,    28,
      -1,   101,    -1,   117,    -1,    25,    -1,   160,    -1,    95,
      -1,   163,    -1,     3,    -1,    29,   211,    -1,    96,   213,
      -1,   130,   188,   209,    -1,   130,   189,    80,   189,   209,
      -1,   130,    26,   209,    -1,   130,    61,    26,   209,    -1,
     130,    63,    26,   209,    -1,   130,   136,   209,    -1,    -1,
     209,   210,    -1,    44,    -1,    52,    -1,    68,    -1,    69,
      -1,    71,    -1,    77,    -1,   100,    -1,   102,    -1,   103,
      -1,   105,    -1,   106,    -1,   107,    -1,   109,    -1,   158,
      -1,   211,   212,    -1,   212,    -1,     9,    58,    -1,    90,
      58,    -1,    94,    58,    -1,   213,   214,    -1,   214,    -1,
      53,    58,    -1,    54,    58,    -1,    55,    58,    -1,    56,
      58,    -1,    81,    58,    -1,    83,    58,    -1,    85,    58,
      -1,    88,    58,    -1,    47,   188,   216,    -1,   216,   217,
      -1,   217,    -1,   146,   242,    -1,   147,   242,    -1,   142,
      58,    -1,   128,   143,    -1,    40,   241,    -1,    41,   241,
      -1,    42,   241,    -1,    43,   241,    -1,    35,   219,    -1,
      28,   219,    -1,   219,   220,    -1,   220,    -1,     6,    -1,
      10,    -1,    17,    -1,    64,    -1,    94,    -1,   108,    -1,
     138,    -1,   149,   222,    -1,   222,   223,    -1,   223,    -1,
       5,   242,    -1,    30,   242,    -1,    46,   242,    -1,    49,
     242,    -1,   113,   242,    -1,   140,   242,    -1,   141,   242,
      -1,   231,    -1,    57,   143,   185,    -1,    36,    -1,    15,
     242,    -1,    18,    58,    -1,   145,   242,    -1,    32,   225,
      -1,    70,   143,    -1,   118,   143,    -1,    75,   143,    -1,
      74,   229,    -1,   116,   239,    -1,   132,   143,    -1,   134,
     226,    -1,   151,   189,    -1,   151,   189,   227,    -1,   154,
     235,    -1,   125,   143,    -1,   143,    -1,   143,    31,    -1,
      -1,   143,   177,   143,    26,    -1,   143,   177,   143,    -1,
     227,   228,    -1,   228,    -1,   120,    58,    -1,    59,   189,
      -1,   229,   230,    -1,   230,    -1,   143,    -1,   232,   234,
     233,    -1,   232,   234,   143,    -1,    59,    -1,    98,    -1,
       4,    -1,    60,    -1,    62,    -1,   161,    -1,    73,    -1,
      52,    -1,    33,    -1,   235,    58,    -1,    58,    -1,   236,
     237,    -1,   237,    -1,    58,    -1,   238,    -1,   178,    58,
      34,    58,   179,    -1,   239,   143,    -1,   143,    -1,   240,
     188,    -1,   188,    -1,    58,    -1,   152,    -1,    37,    -1,
      58,    -1,    31,    -1,   244,   180,   245,   247,   181,    -1,
     167,    -1,   245,   246,   166,    -1,   246,   166,    -1,   168,
     177,   242,    -1,   169,   177,   242,    -1,   247,   248,    -1,
     248,    -1,   250,   180,   249,   251,   181,    -1,   170,   177,
     242,   166,    -1,   133,   177,   188,    -1,   251,   252,    -1,
     252,    -1,   171,   177,   242,   180,   253,   181,    -1,   253,
     254,   166,    -1,   254,   166,    -1,   172,   177,   242,    -1,
     173,   177,   242,    -1,   174,   177,   242,    -1,   175,   177,
     242,    -1,   176,   177,   242,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   313,   313,   317,   318,   319,   333,   334,   335,   336,
     337,   338,   339,   340,   341,   342,   343,   344,   345,   353,
     359,   368,   369,   370,   371,   372,   376,   377,   378,   382,
     386,   387,   391,   392,   393,   394,   395,   396,   397,   398,
     399,   400,   401,   402,   403,   404,   405,   414,   422,   423,
     433,   435,   437,   448,   450,   452,   457,   459,   461,   463,
     465,   467,   472,   474,   478,   485,   495,   497,   499,   501,
     503,   505,   507,   524,   529,   530,   534,   536,   538,   540,
     542,   544,   546,   548,   550,   552,   554,   564,   566,   575,
     583,   584,   588,   589,   590,   591,   592,   593,   594,   595,
     599,   606,   616,   626,   635,   644,   653,   654,   658,   659,
     660,   661,   662,   663,   664,   673,   677,   681,   686,   691,
     696,   709,   722,   734,   735,   740,   741,   742,   743,   744,
     745,   746,   747,   748,   749,   750,   751,   752,   753,   757,
     759,   764,   765,   766,   770,   772,   777,   778,   779,   780,
     781,   782,   783,   784,   792,   797,   799,   804,   805,   806,
     807,   808,   809,   810,   811,   819,   821,   826,   833,   843,
     844,   845,   846,   847,   848,   849,   865,   869,   870,   874,
     875,   876,   877,   878,   879,   880,   889,   890,   906,   912,
     914,   916,   918,   920,   923,   925,   936,   938,   940,   950,
     952,   954,   956,   958,   963,   965,   969,   973,   975,   980,
     982,   986,   987,   991,   992,   996,  1011,  1016,  1024,  1025,
    1029,  1030,  1031,  1032,  1036,  1037,  1038,  1048,  1049,  1053,
    1055,  1060,  1062,  1066,  1071,  1072,  1076,  1077,  1081,  1090,
    1091,  1095,  1096,  1105,  1120,  1124,  1125,  1129,  1130,  1134,
    1135,  1139,  1144,  1148,  1152,  1153,  1157,  1162,  1163,  1167,
    1169,  1171,  1173,  1175
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
  "T_Driftfile", "T_Drop", "T_Ellipsis", "T_Enable", "T_End", "T_False",
  "T_File", "T_Filegen", "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4",
  "T_Flake", "T_Floor", "T_Freq", "T_Fudge", "T_Host", "T_Huffpuff",
  "T_Iburst", "T_Ident", "T_Ignore", "T_Incalloc", "T_Incmem",
  "T_Initalloc", "T_Initmem", "T_Includefile", "T_Integer", "T_Interface",
  "T_Ipv4", "T_Ipv4_flag", "T_Ipv6", "T_Ipv6_flag", "T_Kernel", "T_Key",
  "T_Keys", "T_Keysdir", "T_Kod", "T_Mssntp", "T_Leapfile", "T_Limited",
  "T_Link", "T_Listen", "T_Logconfig", "T_Logfile", "T_Loopstats",
  "T_Lowpriotrap", "T_Manycastclient", "T_Manycastserver", "T_Mask",
  "T_Maxage", "T_Maxclock", "T_Maxdepth", "T_Maxdist", "T_Maxmem",
  "T_Maxpoll", "T_Minclock", "T_Mindepth", "T_Mindist", "T_Minimum",
  "T_Minpoll", "T_Minsane", "T_Mode", "T_Monitor", "T_Month", "T_Mru",
  "T_Multicastclient", "T_Nic", "T_Nolink", "T_Nomodify", "T_None",
  "T_Nopeer", "T_Noquery", "T_Noselect", "T_Noserve", "T_Notrap",
  "T_Notrust", "T_Ntp", "T_Ntpport", "T_NtpSignDsocket", "T_Orphan",
  "T_Orphanwait", "T_Panic", "T_Peer", "T_Peerstats", "T_Phone", "T_Pid",
  "T_Pidfile", "T_Pool", "T_Port", "T_Preempt", "T_Prefer", "T_Protostats",
  "T_Pw", "T_Qos", "T_Randfile", "T_Rawstats", "T_Refid", "T_Requestkey",
  "T_Restrict", "T_Revoke", "T_Saveconfigdir", "T_Server", "T_Setvar",
  "T_Sign", "T_Source", "T_Statistics", "T_Stats", "T_Statsdir", "T_Step",
  "T_Stepout", "T_Stratum", "T_String", "T_Sysstats", "T_Tick", "T_Time1",
  "T_Time2", "T_Timingstats", "T_Tinker", "T_Tos", "T_Trap", "T_True",
  "T_Trustedkey", "T_Ttl", "T_Type", "T_Unconfig", "T_Unpeer", "T_Version",
  "T_WanderThreshold", "T_Week", "T_Wildcard", "T_Xleave", "T_Year",
  "T_Flag", "T_Void", "T_EOC", "T_Simulate", "T_Beep_Delay",
  "T_Sim_Duration", "T_Server_Offset", "T_Duration", "T_Freq_Offset",
  "T_Wander", "T_Jitter", "T_Prop_Delay", "T_Proc_Delay", "'='", "'('",
  "')'", "'{'", "'}'", "$accept", "configuration", "command_list",
  "command", "server_command", "client_type", "address", "ip_address",
  "option_list", "option", "unpeer_command", "unpeer_keyword",
  "other_mode_command", "authentication_command", "crypto_command_line",
  "crypto_command_list", "crypto_command", "orphan_mode_command",
  "tos_option_list", "tos_option", "monitoring_command", "stats_list",
  "stat", "filegen_option_list", "filegen_option", "filegen_type",
  "access_control_command", "ac_flag_list", "access_control_flag",
  "discard_option_list", "discard_option", "mru_option_list", "mru_option",
  "fudge_command", "fudge_factor_list", "fudge_factor",
  "system_option_command", "system_option_list", "system_option",
  "tinker_command", "tinker_option_list", "tinker_option",
  "miscellaneous_command", "drift_parm", "variable_assign",
  "trap_option_list", "trap_option", "log_config_list",
  "log_config_command", "interface_command", "interface_nic",
  "nic_rule_class", "nic_rule_action", "integer_list",
  "integer_list_range", "integer_list_range_elt", "integer_range",
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
     425,   426,   427,   428,   429,   430,   431,    61,    40,    41,
     123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   182,   183,   184,   184,   184,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   186,
     186,   187,   187,   187,   187,   187,   188,   188,   188,   189,
     190,   190,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   192,   193,   193,
     194,   194,   194,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   196,   196,   197,   197,   198,   198,   198,   198,
     198,   198,   198,   199,   200,   200,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   202,   202,   202,
     203,   203,   204,   204,   204,   204,   204,   204,   204,   204,
     205,   205,   206,   206,   206,   206,   206,   206,   207,   207,
     207,   207,   207,   207,   207,   208,   208,   208,   208,   208,
     208,   208,   208,   209,   209,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   211,
     211,   212,   212,   212,   213,   213,   214,   214,   214,   214,
     214,   214,   214,   214,   215,   216,   216,   217,   217,   217,
     217,   217,   217,   217,   217,   218,   218,   219,   219,   220,
     220,   220,   220,   220,   220,   220,   221,   222,   222,   223,
     223,   223,   223,   223,   223,   223,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   225,   225,   225,   226,   226,   227,
     227,   228,   228,   229,   229,   230,   231,   231,   232,   232,
     233,   233,   233,   233,   234,   234,   234,   235,   235,   236,
     236,   237,   237,   238,   239,   239,   240,   240,   241,   241,
     241,   242,   242,   243,   244,   245,   245,   246,   246,   247,
     247,   248,   249,   250,   251,   251,   252,   253,   253,   254,
     254,   254,   254,   254
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     3,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     3,     5,     3,
       4,     4,     3,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     2,     2,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     1,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     2,     2,     1,     2,     0,     4,     3,     2,
       1,     2,     2,     2,     1,     1,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       1,     1,     1,     5,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     5,     1,     3,     2,     3,     3,     2,
       1,     5,     4,     3,     2,     1,     6,     3,     2,     3,
       3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,    24,    50,     0,     0,     0,    63,     0,
       0,   206,     0,   188,     0,     0,     0,   218,     0,     0,
       0,     0,     0,    25,     0,     0,     0,   219,     0,    23,
       0,     0,    22,     0,     0,     0,     0,     0,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,    49,
     244,     0,     2,     0,     7,     0,     8,     0,     9,    10,
      13,    11,    12,    14,    15,    16,    17,   186,     0,    18,
       0,     5,    53,   242,   241,   189,   190,    54,     0,     0,
       0,     0,     0,     0,     0,    55,    62,    65,   169,   170,
     171,   172,   173,   174,   175,   166,   168,     0,     0,     0,
     115,   140,   204,   192,   165,    92,    93,    94,    95,    99,
      96,    97,    98,     0,     0,     0,    29,     0,    26,     6,
      56,    57,   193,   215,   196,   214,   195,   237,    51,     0,
       0,     0,     0,     0,     0,     0,     0,   116,   145,    52,
      61,   235,   197,   194,   203,    58,   123,     0,     0,   123,
     123,    26,    59,   198,     0,   199,    87,    91,    88,   191,
       0,     0,     0,     0,     0,     0,     0,   176,   178,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      73,    75,   200,   231,     0,    60,   230,   232,   228,   202,
       1,     0,     4,    20,    47,   226,   225,   224,     0,     0,
      71,    66,    67,    68,    69,    72,    70,    64,   167,   141,
     142,   143,   139,   205,   107,   106,     0,   104,   105,     0,
      89,   101,    27,    28,     0,     0,     0,     0,     0,     0,
       0,     0,   154,   156,   187,   213,   236,   146,   147,   148,
     149,   150,   151,   152,   153,   144,   234,   119,   123,   123,
     122,   117,     0,     0,    90,   179,   180,   181,   182,   183,
     184,   185,   177,    86,    76,   240,   238,   239,    78,    77,
      84,    82,    83,    81,    85,    79,    80,    74,     0,     0,
     201,   210,     0,   229,   227,     3,    32,     0,    34,    35,
       0,     0,     0,     0,    39,    40,    41,    42,     0,     0,
      43,    19,    31,   220,   221,   222,   217,   223,   216,     0,
       0,     0,     0,   102,   114,   110,   112,   108,   109,   111,
     113,   103,   100,   161,   162,   163,   164,   160,   159,   157,
     158,   155,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   124,   120,   121,   123,
     208,   212,   211,   209,     0,    33,    36,    38,    37,    45,
      44,    46,    30,     0,     0,     0,     0,     0,   250,     0,
     246,   118,   207,     0,   247,   248,     0,   245,   243,   249,
       0,   233,   253,     0,     0,     0,     0,     0,   255,     0,
       0,   251,   254,   252,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   256,     0,
     258,   259,   260,   261,   262,   263,   257
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    51,    52,    53,    54,    55,   127,   118,   301,   302,
      56,    57,    58,    59,    85,    86,    87,    60,   180,   181,
      61,   156,   113,   220,   221,   321,    62,   247,   346,   100,
     101,   137,   138,    63,   232,   233,    64,    95,    96,    65,
     167,   168,    66,   103,   155,   280,   281,   124,   125,    67,
      68,   308,   198,   189,   185,   186,   187,   142,   128,   268,
      75,    69,    70,   311,   312,   367,   368,   384,   369,   387,
     388,   401,   402
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -148
static const yytype_int16 yypact[] =
{
     137,  -132,   -20,  -148,  -148,    -7,   -17,   -16,    44,    -1,
       9,   -99,    -1,  -148,   115,   -46,   -98,  -148,   -97,   -95,
     -94,   -90,   -88,  -148,   -46,   144,   -46,  -148,   -85,  -148,
     -84,   -78,  -148,   -77,    -2,    13,    12,   -71,  -148,   -70,
     115,   -66,    -7,     1,   538,   -65,   -51,    32,  -148,  -148,
    -148,    91,   310,   -68,  -148,   -46,  -148,   -46,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,   -12,  -148,
     -80,  -148,  -148,  -148,  -148,  -148,  -148,  -148,   -47,   -39,
     -38,   -28,   -27,    55,   -26,  -148,    44,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,    -1,  -148,    71,    72,    86,
       9,  -148,   117,  -148,    -1,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,   -15,    10,    14,  -148,    39,  -148,   457,
    -148,  -148,  -148,  -148,   -90,  -148,  -148,  -148,   -46,    96,
     104,   105,   106,   119,   120,   122,   124,   144,  -148,   -46,
    -148,  -148,    40,  -148,  -148,  -148,  -148,     3,     4,  -148,
    -148,   107,  -148,  -148,    15,  -148,   115,  -148,  -148,  -148,
      -7,    -7,    -7,    -7,    -7,    -7,    -7,     1,  -148,   132,
     135,     6,   143,    -7,    -7,    -7,    -7,   147,   148,   150,
     538,  -148,   -37,  -148,   151,   -51,  -148,  -148,  -148,   152,
    -148,    29,  -148,   530,  -148,  -148,  -148,  -148,     0,  -142,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,    70,  -148,  -148,    11,
     -15,  -148,  -148,  -148,     6,     6,     6,     6,    74,   156,
      -7,    -7,    39,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,   560,  -148,  -148,
     560,   560,   -65,    81,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,   -65,   168,
     -37,  -148,   194,  -148,  -148,  -148,  -148,    -7,  -148,  -148,
     173,   178,   179,   181,  -148,  -148,  -148,  -148,   182,   183,
    -148,   530,  -148,  -148,  -148,  -148,  -148,  -148,  -148,    66,
      69,  -100,    82,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,   560,   560,  -148,
     223,  -148,  -148,  -148,   192,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,    -7,    -7,    75,    88,  -114,  -148,    77,
    -148,   560,  -148,    79,  -148,  -148,   -46,  -148,  -148,  -148,
      90,  -148,  -148,    84,    93,    -7,    95,  -146,  -148,    99,
      -7,  -148,  -148,  -148,    97,    47,    98,   101,   102,   103,
     108,   -87,   118,    -7,    -7,    -7,    -7,    -7,  -148,   123,
    -148,  -148,  -148,  -148,  -148,  -148,  -148
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -148,  -148,  -148,   -44,  -148,  -148,    -3,   -34,  -148,   -18,
    -148,  -148,  -148,  -148,  -148,  -148,   187,  -148,  -148,   112,
    -148,  -148,   -30,  -148,    61,  -148,  -148,  -147,  -148,  -148,
     195,  -148,   159,  -148,  -148,    65,  -148,   286,   -67,  -148,
    -148,   133,  -148,  -148,  -148,  -148,    19,  -148,   177,  -148,
    -148,  -148,  -148,  -148,  -148,   121,  -148,  -148,   276,  -116,
     -42,  -148,  -148,  -148,    -6,  -148,   -60,  -148,  -148,  -148,
     -79,  -148,   -92
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     159,   151,   250,   251,   303,    88,   160,   183,   191,    89,
     157,   182,   117,   214,   314,   114,    90,   115,    97,   365,
     215,   195,   278,   216,    73,   386,   309,   310,   208,   248,
     249,   161,   150,   365,    71,   391,   315,   208,    72,   146,
     196,    76,    77,   265,   102,   119,   120,   162,   121,   122,
     163,    74,   193,   123,   194,   126,   145,   217,   140,   141,
     304,   197,   305,    91,   266,   143,   144,   378,   309,   310,
     152,    78,   153,   154,   147,   234,   148,   158,   116,   224,
     225,   226,   227,   279,   218,   396,   397,   398,   399,   400,
     188,   190,    79,    92,   408,    80,   200,   116,   192,    98,
     199,   347,   348,    99,   201,   202,   316,    93,   323,   324,
     325,   326,   317,   205,   164,   203,   204,   206,   255,   256,
     257,   258,   259,   260,   261,   236,   254,   184,   318,   209,
     210,   270,   271,   272,   273,   105,   236,    94,     1,   106,
     219,   165,   166,   306,   211,     2,   222,   223,   213,   149,
       3,     4,     5,   222,   237,     6,   116,   223,   267,     7,
       8,   307,   238,   239,   240,     9,    10,   228,    81,    11,
      82,   319,    12,    13,   320,    83,    14,   241,   242,    84,
     243,   229,   244,   246,    15,   230,   231,   252,   329,   330,
     263,   107,   253,   264,    16,   285,    17,   129,   130,   131,
     132,   269,   371,    18,    19,   274,   275,    20,   276,   282,
     284,    21,    22,   313,   328,    23,    24,   327,   349,   396,
     397,   398,   399,   400,   350,   133,   352,   134,   354,   135,
     108,   356,   136,    25,    26,    27,   357,   358,   109,   359,
     360,   361,   110,   363,   351,   355,   364,    28,   370,   372,
     373,    29,   376,    30,   377,    31,    32,   380,   381,   111,
     383,   385,    33,   112,   386,   393,    34,    35,    36,    37,
      38,    39,   390,   207,    40,   403,    41,   395,   404,   405,
     406,   322,    42,   362,   410,   407,    43,    44,    45,   416,
      46,    47,   277,    48,    49,   212,   245,   331,   104,   353,
     262,   235,   139,    -6,    50,   366,   283,   379,   392,   409,
       0,     0,     0,     0,     0,     0,     0,     0,     2,     0,
       0,   374,   375,     3,     4,     5,     0,     0,     6,     0,
       0,     0,     7,     8,     0,     0,     0,     0,     9,    10,
       0,     0,    11,   389,     0,    12,    13,     0,   394,    14,
       0,     0,     0,     0,     0,     0,     0,    15,     0,     0,
       0,   411,   412,   413,   414,   415,     0,    16,     0,    17,
       0,     0,     0,   382,     0,     0,    18,    19,     0,     0,
      20,     0,     0,     0,    21,    22,     0,     0,    23,    24,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    25,    26,    27,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,     0,     0,     0,    29,     0,    30,     0,    31,    32,
       0,     0,     0,     0,     0,    33,     0,     0,     0,    34,
      35,    36,    37,    38,    39,     0,     0,    40,     0,    41,
       0,     0,     0,     0,     0,    42,     0,     0,     0,    43,
      44,    45,     0,    46,    47,     2,    48,    49,     0,     0,
       3,     4,     5,     0,     0,     6,    -6,    50,     0,     7,
       8,     0,     0,     0,     0,     9,    10,     0,     0,    11,
       0,     0,    12,    13,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,    15,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    16,     0,    17,     0,     0,     0,
       0,     0,     0,    18,    19,     0,     0,    20,     0,     0,
       0,    21,    22,     0,     0,    23,    24,   286,     0,     0,
       0,     0,   287,     0,     0,     0,   288,     0,     0,   169,
       0,     0,     0,    25,    26,    27,     0,   170,     0,   171,
       0,     0,     0,     0,     0,     0,     0,    28,     0,     0,
       0,    29,     0,    30,     0,    31,    32,     0,     0,     0,
     289,     0,    33,   172,     0,     0,    34,    35,    36,    37,
      38,    39,     0,     0,    40,   290,    41,     0,     0,     0,
       0,     0,    42,     0,   332,     0,    43,    44,    45,     0,
      46,    47,   333,    48,    49,     0,   291,     0,     0,     0,
     173,   292,   174,   293,    50,   175,     0,   176,   334,   335,
     177,   336,     0,     0,   294,     0,     0,   337,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   178,
     179,   295,   296,     0,     0,     0,     0,     0,     0,     0,
     338,     0,   339,   340,     0,   341,   342,   343,     0,   344,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   297,     0,   298,     0,     0,     0,   299,     0,
       0,     0,   300,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   345
};

static const yytype_int16 yycheck[] =
{
      42,    35,   149,   150,     4,     6,     5,    58,    52,    10,
      40,    45,    15,    28,     3,    61,    17,    63,     9,   133,
      35,    33,    59,    38,    31,   171,   168,   169,    95,    26,
      26,    30,    35,   133,   166,   181,    25,   104,    58,    26,
      52,    58,    58,    37,   143,   143,   143,    46,   143,   143,
      49,    58,    55,   143,    57,   143,    58,    72,   143,   143,
      60,    73,    62,    64,    58,   143,   143,   181,   168,   169,
      58,    27,   143,   143,    61,   119,    63,   143,   143,    40,
      41,    42,    43,   120,    99,   172,   173,   174,   175,   176,
      58,     0,    48,    94,   181,    51,   143,   143,   166,    90,
     180,   248,   249,    94,   143,   143,    95,   108,   224,   225,
     226,   227,   101,    58,   113,   143,   143,   143,   160,   161,
     162,   163,   164,   165,   166,   128,   156,   178,   117,    58,
      58,   173,   174,   175,   176,    20,   139,   138,     1,    24,
     155,   140,   141,   143,    58,     8,   143,   143,    31,   136,
      13,    14,    15,   143,    58,    18,   143,   143,   152,    22,
      23,   161,    58,    58,    58,    28,    29,   128,   124,    32,
     126,   160,    35,    36,   163,   131,    39,    58,    58,   135,
      58,   142,    58,   143,    47,   146,   147,    80,   230,   231,
      58,    76,   177,    58,    57,   166,    59,    53,    54,    55,
      56,    58,   349,    66,    67,    58,    58,    70,    58,    58,
      58,    74,    75,   143,    58,    78,    79,   143,   252,   172,
     173,   174,   175,   176,   143,    81,    58,    83,    34,    85,
     115,    58,    88,    96,    97,    98,    58,    58,   123,    58,
      58,    58,   127,   177,   278,   287,   177,   110,   166,    26,
      58,   114,   177,   116,   166,   118,   119,   180,   179,   144,
     170,   177,   125,   148,   171,   166,   129,   130,   131,   132,
     133,   134,   177,    86,   137,   177,   139,   180,   177,   177,
     177,   220,   145,   301,   166,   177,   149,   150,   151,   166,
     153,   154,   180,   156,   157,   100,   137,   232,    12,   280,
     167,   124,    26,   166,   167,   311,   185,   367,   387,   401,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     8,    -1,
      -1,   363,   364,    13,    14,    15,    -1,    -1,    18,    -1,
      -1,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,
      -1,    -1,    32,   385,    -1,    35,    36,    -1,   390,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,   403,   404,   405,   406,   407,    -1,    57,    -1,    59,
      -1,    -1,    -1,   376,    -1,    -1,    66,    67,    -1,    -1,
      70,    -1,    -1,    -1,    74,    75,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    96,    97,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     110,    -1,    -1,    -1,   114,    -1,   116,    -1,   118,   119,
      -1,    -1,    -1,    -1,    -1,   125,    -1,    -1,    -1,   129,
     130,   131,   132,   133,   134,    -1,    -1,   137,    -1,   139,
      -1,    -1,    -1,    -1,    -1,   145,    -1,    -1,    -1,   149,
     150,   151,    -1,   153,   154,     8,   156,   157,    -1,    -1,
      13,    14,    15,    -1,    -1,    18,   166,   167,    -1,    22,
      23,    -1,    -1,    -1,    -1,    28,    29,    -1,    -1,    32,
      -1,    -1,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    57,    -1,    59,    -1,    -1,    -1,
      -1,    -1,    -1,    66,    67,    -1,    -1,    70,    -1,    -1,
      -1,    74,    75,    -1,    -1,    78,    79,     7,    -1,    -1,
      -1,    -1,    12,    -1,    -1,    -1,    16,    -1,    -1,    11,
      -1,    -1,    -1,    96,    97,    98,    -1,    19,    -1,    21,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,    -1,
      -1,   114,    -1,   116,    -1,   118,   119,    -1,    -1,    -1,
      50,    -1,   125,    45,    -1,    -1,   129,   130,   131,   132,
     133,   134,    -1,    -1,   137,    65,   139,    -1,    -1,    -1,
      -1,    -1,   145,    -1,    44,    -1,   149,   150,   151,    -1,
     153,   154,    52,   156,   157,    -1,    86,    -1,    -1,    -1,
      82,    91,    84,    93,   167,    87,    -1,    89,    68,    69,
      92,    71,    -1,    -1,   104,    -1,    -1,    77,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,
     112,   121,   122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     100,    -1,   102,   103,    -1,   105,   106,   107,    -1,   109,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   152,    -1,   154,    -1,    -1,    -1,   158,    -1,
      -1,    -1,   162,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   158
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     8,    13,    14,    15,    18,    22,    23,    28,
      29,    32,    35,    36,    39,    47,    57,    59,    66,    67,
      70,    74,    75,    78,    79,    96,    97,    98,   110,   114,
     116,   118,   119,   125,   129,   130,   131,   132,   133,   134,
     137,   139,   145,   149,   150,   151,   153,   154,   156,   157,
     167,   183,   184,   185,   186,   187,   192,   193,   194,   195,
     199,   202,   208,   215,   218,   221,   224,   231,   232,   243,
     244,   166,    58,    31,    58,   242,    58,    58,    27,    48,
      51,   124,   126,   131,   135,   196,   197,   198,     6,    10,
      17,    64,    94,   108,   138,   219,   220,     9,    90,    94,
     211,   212,   143,   225,   219,    20,    24,    76,   115,   123,
     127,   144,   148,   204,    61,    63,   143,   188,   189,   143,
     143,   143,   143,   143,   229,   230,   143,   188,   240,    53,
      54,    55,    56,    81,    83,    85,    88,   213,   214,   240,
     143,   143,   239,   143,   143,    58,    26,    61,    63,   136,
     188,   189,    58,   143,   143,   226,   203,   204,   143,   242,
       5,    30,    46,    49,   113,   140,   141,   222,   223,    11,
      19,    21,    45,    82,    84,    87,    89,    92,   111,   112,
     200,   201,   189,    58,   178,   236,   237,   238,    58,   235,
       0,   185,   166,   188,   188,    33,    52,    73,   234,   180,
     143,   143,   143,   143,   143,    58,   143,   198,   220,    58,
      58,    58,   212,    31,    28,    35,    38,    72,    99,   155,
     205,   206,   143,   143,    40,    41,    42,    43,   128,   142,
     146,   147,   216,   217,   185,   230,   188,    58,    58,    58,
      58,    58,    58,    58,    58,   214,   143,   209,    26,    26,
     209,   209,    80,   177,   204,   242,   242,   242,   242,   242,
     242,   242,   223,    58,    58,    37,    58,   152,   241,    58,
     242,   242,   242,   242,    58,    58,    58,   201,    59,   120,
     227,   228,    58,   237,    58,   166,     7,    12,    16,    50,
      65,    86,    91,    93,   104,   121,   122,   152,   154,   158,
     162,   190,   191,     4,    60,    62,   143,   161,   233,   168,
     169,   245,   246,   143,     3,    25,    95,   101,   117,   160,
     163,   207,   206,   241,   241,   241,   241,   143,    58,   242,
     242,   217,    44,    52,    68,    69,    71,    77,   100,   102,
     103,   105,   106,   107,   109,   158,   210,   209,   209,   189,
     143,   189,    58,   228,    34,   242,    58,    58,    58,    58,
      58,    58,   191,   177,   177,   133,   246,   247,   248,   250,
     166,   209,    26,    58,   242,   242,   177,   166,   181,   248,
     180,   179,   188,   170,   249,   177,   171,   251,   252,   242,
     177,   181,   252,   166,   242,   180,   172,   173,   174,   175,
     176,   253,   254,   177,   177,   177,   177,   177,   181,   254,
     166,   242,   242,   242,   242,   242,   166
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
#line 320 "ntp_parser.y"
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
#line 354 "ntp_parser.y"
    {
			struct peer_node *my_node =  create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue));
			if (my_node)
				enqueue(cfgt.peers, my_node);
		}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 360 "ntp_parser.y"
    {
			struct peer_node *my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
			if (my_node)
				enqueue(cfgt.peers, my_node);
		}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 377 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 378 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET6); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 382 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), 0); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 386 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 387 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 391 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 392 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 393 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 394 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 395 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 396 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 397 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 398 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 399 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 400 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 401 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 402 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 403 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 404 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 405 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 415 "ntp_parser.y"
    {
			struct unpeer_node *my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				enqueue(cfgt.unpeers, my_node);
		}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 434 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 436 "ntp_parser.y"
    { append_queue(cfgt.manycastserver, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 438 "ntp_parser.y"
    { append_queue(cfgt.multicastclient, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 449 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer))); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 451 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 453 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			append_queue(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 458 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 460 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 462 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 464 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 466 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Queue); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 468 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 474 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 479 "ntp_parser.y"
    { 
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 486 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 496 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 498 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 500 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 502 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 504 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 506 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 508 "ntp_parser.y"
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
#line 525 "ntp_parser.y"
    { append_queue(cfgt.orphan_cmds,(yyvsp[(2) - (2)].Queue)); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 529 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 530 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 535 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 537 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 539 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 541 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 543 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 545 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 547 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 549 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 551 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 553 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 555 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 565 "ntp_parser.y"
    { append_queue(cfgt.stats_list, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 567 "ntp_parser.y"
    {
			if (input_from_file)
				cfgt.stats_dir = (yyvsp[(2) - (2)].String);
			else {
				free((yyvsp[(2) - (2)].String));
				yyerror("statsdir remote configuration ignored");
			}
		}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 576 "ntp_parser.y"
    {
			enqueue(cfgt.filegen_opts,
				create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Queue)));
		}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 583 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 584 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 600 "ntp_parser.y"
    {
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 607 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 617 "ntp_parser.y"
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

  case 103:

/* Line 1455 of yacc.c  */
#line 627 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen type remote configuration ignored");
			}
		}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 636 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen link remote configuration ignored");
			}
		}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 645 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen nolink remote configuration ignored");
			}
		}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 653 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 654 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 674 "ntp_parser.y"
    {
			append_queue(cfgt.discard_opts, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 678 "ntp_parser.y"
    {
			append_queue(cfgt.mru_opts, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 682 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 687 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Queue), ip_file->line_no));
		}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 692 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 697 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(
					create_address_node(
						estrdup("0.0.0.0"), 
						AF_INET),
					create_address_node(
						estrdup("0.0.0.0"), 
						AF_INET),
					(yyvsp[(4) - (4)].Queue), 
					ip_file->line_no));
		}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 710 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(
					create_address_node(
						estrdup("::"), 
						AF_INET6),
					create_address_node(
						estrdup("::"), 
						AF_INET6),
					(yyvsp[(4) - (4)].Queue), 
					ip_file->line_no));
		}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 723 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(
					NULL, NULL,
					enqueue((yyvsp[(3) - (3)].Queue), create_ival((yyvsp[(2) - (3)].Integer))),
					ip_file->line_no));
		}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 734 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 736 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 758 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 760 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 764 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 765 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 766 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 771 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 773 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 777 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 778 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 779 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 780 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 781 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 782 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 783 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 784 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 793 "ntp_parser.y"
    { enqueue(cfgt.fudge, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 798 "ntp_parser.y"
    { enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 800 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 804 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 805 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 806 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 807 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 808 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 809 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 810 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 811 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 820 "ntp_parser.y"
    { append_queue(cfgt.enable_opts, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 822 "ntp_parser.y"
    { append_queue(cfgt.disable_opts, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 827 "ntp_parser.y"
    {
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 834 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 843 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 844 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 845 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 846 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 847 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 848 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 850 "ntp_parser.y"
    { 
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("enable/disable stats remote configuration ignored");
			}
		}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 865 "ntp_parser.y"
    { append_queue(cfgt.tinker, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 869 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 870 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 874 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 875 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 876 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 877 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 878 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 879 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 880 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 891 "ntp_parser.y"
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

  case 188:

/* Line 1455 of yacc.c  */
#line 907 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 913 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double))); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 915 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer))); }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 917 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double))); }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 919 "ntp_parser.y"
    { /* Null action, possibly all null parms */ }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 921 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 924 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 926 "ntp_parser.y"
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

  case 196:

/* Line 1455 of yacc.c  */
#line 937 "ntp_parser.y"
    { append_queue(cfgt.logconfig, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 939 "ntp_parser.y"
    { append_queue(cfgt.phone, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 941 "ntp_parser.y"
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

  case 199:

/* Line 1455 of yacc.c  */
#line 951 "ntp_parser.y"
    { enqueue(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 953 "ntp_parser.y"
    { enqueue(cfgt.trap, create_addr_opts_node((yyvsp[(2) - (2)].Address_node), NULL)); }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 955 "ntp_parser.y"
    { enqueue(cfgt.trap, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 957 "ntp_parser.y"
    { append_queue(cfgt.ttl, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 959 "ntp_parser.y"
    { enqueue(cfgt.qos, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 964 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String))); }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 966 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double)));
			  enqueue(cfgt.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String))); }
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 969 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 974 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 976 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (3)].String), (yyvsp[(3) - (3)].String), 0); }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 981 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 982 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 986 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 987 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_pval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 991 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 992 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 997 "ntp_parser.y"
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

  case 216:

/* Line 1455 of yacc.c  */
#line 1012 "ntp_parser.y"
    {
			enqueue(cfgt.nic_rules,
				create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer)));
		}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 1017 "ntp_parser.y"
    {
			enqueue(cfgt.nic_rules,
				create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer)));
		}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 1048 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 1049 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 1054 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 1056 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 1061 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[(1) - (1)].Integer)); }
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 1067 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_shorts('-', (yyvsp[(2) - (5)].Integer), (yyvsp[(4) - (5)].Integer)); }
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 1071 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_pval((yyvsp[(2) - (2)].String))); }
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 1072 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_pval((yyvsp[(1) - (1)].String))); }
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 1076 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 1077 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Address_node)); }
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 1082 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			}
			else
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
		}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 1090 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 1091 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 1095 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 1106 "ntp_parser.y"
    {
			cfgt.sim_details = create_sim_node((yyvsp[(3) - (5)].Queue), (yyvsp[(4) - (5)].Queue));

			/* Reset the old_config_style variable */
			old_config_style = 1;
		}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 1120 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 1124 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 1125 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 1129 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 1130 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 1134 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_server)); }
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 1135 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_server)); }
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 1140 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Queue)); }
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 1144 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 1148 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 1152 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_script)); }
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 1153 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_script)); }
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 1158 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Queue)); }
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 1162 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 1163 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 1168 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 1170 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 1172 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 1174 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 1176 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1455 of yacc.c  */
#line 3826 "ntp_parser.c"
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
#line 1180 "ntp_parser.y"


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


