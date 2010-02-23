
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
     T_Includefile = 307,
     T_Integer = 308,
     T_Interface = 309,
     T_Ipv4 = 310,
     T_Ipv4_flag = 311,
     T_Ipv6 = 312,
     T_Ipv6_flag = 313,
     T_Kernel = 314,
     T_Key = 315,
     T_Keys = 316,
     T_Keysdir = 317,
     T_Kod = 318,
     T_Mssntp = 319,
     T_Leapfile = 320,
     T_Limited = 321,
     T_Link = 322,
     T_Listen = 323,
     T_Logconfig = 324,
     T_Logfile = 325,
     T_Loopstats = 326,
     T_Lowpriotrap = 327,
     T_Manycastclient = 328,
     T_Manycastserver = 329,
     T_Mask = 330,
     T_Maxclock = 331,
     T_Maxdist = 332,
     T_Maxpoll = 333,
     T_Minclock = 334,
     T_Mindist = 335,
     T_Minimum = 336,
     T_Minpoll = 337,
     T_Minsane = 338,
     T_Mode = 339,
     T_Monitor = 340,
     T_Month = 341,
     T_Multicastclient = 342,
     T_Nic = 343,
     T_Nolink = 344,
     T_Nomodify = 345,
     T_None = 346,
     T_Nopeer = 347,
     T_Noquery = 348,
     T_Noselect = 349,
     T_Noserve = 350,
     T_Notrap = 351,
     T_Notrust = 352,
     T_Ntp = 353,
     T_Ntpport = 354,
     T_NtpSignDsocket = 355,
     T_Orphan = 356,
     T_Panic = 357,
     T_Peer = 358,
     T_Peerstats = 359,
     T_Phone = 360,
     T_Pid = 361,
     T_Pidfile = 362,
     T_Pool = 363,
     T_Port = 364,
     T_Preempt = 365,
     T_Prefer = 366,
     T_Protostats = 367,
     T_Pw = 368,
     T_Qos = 369,
     T_Randfile = 370,
     T_Rawstats = 371,
     T_Refid = 372,
     T_Requestkey = 373,
     T_Restrict = 374,
     T_Revoke = 375,
     T_Saveconfigdir = 376,
     T_Server = 377,
     T_Setvar = 378,
     T_Sign = 379,
     T_Source = 380,
     T_Statistics = 381,
     T_Stats = 382,
     T_Statsdir = 383,
     T_Step = 384,
     T_Stepout = 385,
     T_Stratum = 386,
     T_String = 387,
     T_Sysstats = 388,
     T_Tick = 389,
     T_Time1 = 390,
     T_Time2 = 391,
     T_Timingstats = 392,
     T_Tinker = 393,
     T_Tos = 394,
     T_Trap = 395,
     T_True = 396,
     T_Trustedkey = 397,
     T_Ttl = 398,
     T_Type = 399,
     T_Unconfig = 400,
     T_Unpeer = 401,
     T_Version = 402,
     T_WanderThreshold = 403,
     T_Week = 404,
     T_Wildcard = 405,
     T_Xleave = 406,
     T_Year = 407,
     T_Flag = 408,
     T_Void = 409,
     T_EOC = 410,
     T_Simulate = 411,
     T_Beep_Delay = 412,
     T_Sim_Duration = 413,
     T_Server_Offset = 414,
     T_Duration = 415,
     T_Freq_Offset = 416,
     T_Wander = 417,
     T_Jitter = 418,
     T_Prop_Delay = 419,
     T_Proc_Delay = 420
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
#define T_Includefile 307
#define T_Integer 308
#define T_Interface 309
#define T_Ipv4 310
#define T_Ipv4_flag 311
#define T_Ipv6 312
#define T_Ipv6_flag 313
#define T_Kernel 314
#define T_Key 315
#define T_Keys 316
#define T_Keysdir 317
#define T_Kod 318
#define T_Mssntp 319
#define T_Leapfile 320
#define T_Limited 321
#define T_Link 322
#define T_Listen 323
#define T_Logconfig 324
#define T_Logfile 325
#define T_Loopstats 326
#define T_Lowpriotrap 327
#define T_Manycastclient 328
#define T_Manycastserver 329
#define T_Mask 330
#define T_Maxclock 331
#define T_Maxdist 332
#define T_Maxpoll 333
#define T_Minclock 334
#define T_Mindist 335
#define T_Minimum 336
#define T_Minpoll 337
#define T_Minsane 338
#define T_Mode 339
#define T_Monitor 340
#define T_Month 341
#define T_Multicastclient 342
#define T_Nic 343
#define T_Nolink 344
#define T_Nomodify 345
#define T_None 346
#define T_Nopeer 347
#define T_Noquery 348
#define T_Noselect 349
#define T_Noserve 350
#define T_Notrap 351
#define T_Notrust 352
#define T_Ntp 353
#define T_Ntpport 354
#define T_NtpSignDsocket 355
#define T_Orphan 356
#define T_Panic 357
#define T_Peer 358
#define T_Peerstats 359
#define T_Phone 360
#define T_Pid 361
#define T_Pidfile 362
#define T_Pool 363
#define T_Port 364
#define T_Preempt 365
#define T_Prefer 366
#define T_Protostats 367
#define T_Pw 368
#define T_Qos 369
#define T_Randfile 370
#define T_Rawstats 371
#define T_Refid 372
#define T_Requestkey 373
#define T_Restrict 374
#define T_Revoke 375
#define T_Saveconfigdir 376
#define T_Server 377
#define T_Setvar 378
#define T_Sign 379
#define T_Source 380
#define T_Statistics 381
#define T_Stats 382
#define T_Statsdir 383
#define T_Step 384
#define T_Stepout 385
#define T_Stratum 386
#define T_String 387
#define T_Sysstats 388
#define T_Tick 389
#define T_Time1 390
#define T_Time2 391
#define T_Timingstats 392
#define T_Tinker 393
#define T_Tos 394
#define T_Trap 395
#define T_True 396
#define T_Trustedkey 397
#define T_Ttl 398
#define T_Type 399
#define T_Unconfig 400
#define T_Unpeer 401
#define T_Version 402
#define T_WanderThreshold 403
#define T_Week 404
#define T_Wildcard 405
#define T_Xleave 406
#define T_Year 407
#define T_Flag 408
#define T_Void 409
#define T_EOC 410
#define T_Simulate 411
#define T_Beep_Delay 412
#define T_Sim_Duration 413
#define T_Server_Offset 414
#define T_Duration 415
#define T_Freq_Offset 416
#define T_Wander 417
#define T_Jitter 418
#define T_Prop_Delay 419
#define T_Proc_Delay 420




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
#line 490 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 502 "ntp_parser.c"

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
#define YYFINAL  174
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   679

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  169
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  68
/* YYNRULES -- Number of rules.  */
#define YYNRULES  246
/* YYNRULES -- Number of states.  */
#define YYNSTATES  386

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   420

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
       2,   166,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   167,     2,   168,     2,     2,     2,     2,
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
     165
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
     274,   276,   278,   280,   282,   285,   289,   295,   299,   304,
     309,   313,   314,   317,   319,   321,   323,   325,   327,   329,
     331,   333,   335,   337,   339,   341,   343,   345,   348,   350,
     353,   356,   359,   363,   366,   368,   371,   374,   377,   380,
     383,   386,   389,   392,   395,   398,   401,   403,   405,   407,
     409,   411,   413,   415,   417,   420,   423,   425,   428,   431,
     434,   437,   440,   443,   446,   448,   452,   454,   457,   460,
     463,   466,   469,   472,   475,   478,   481,   484,   487,   490,
     494,   497,   500,   502,   505,   506,   511,   515,   518,   520,
     523,   526,   529,   531,   533,   537,   541,   543,   545,   547,
     549,   551,   553,   555,   557,   559,   562,   564,   567,   569,
     572,   574,   576,   578,   580,   582,   584,   590,   592,   596,
     599,   603,   607,   610,   612,   618,   623,   627,   630,   632,
     639,   643,   646,   650,   654,   658,   662
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     170,     0,    -1,   171,    -1,   171,   172,   155,    -1,   172,
     155,    -1,     1,   155,    -1,    -1,   173,    -1,   179,    -1,
     181,    -1,   182,    -1,   189,    -1,   195,    -1,   186,    -1,
     200,    -1,   203,    -1,   206,    -1,   209,    -1,   225,    -1,
     174,   175,   177,    -1,   174,   175,    -1,   122,    -1,   108,
      -1,   103,    -1,    13,    -1,    73,    -1,   176,    -1,    56,
     132,    -1,    58,   132,    -1,   132,    -1,   177,   178,    -1,
     178,    -1,     7,    -1,    12,   224,    -1,    16,    -1,    49,
      -1,    60,    53,    -1,    82,    53,    -1,    78,    53,    -1,
      94,    -1,   110,    -1,   111,    -1,   141,    -1,   151,    -1,
     143,    53,    -1,    84,    53,    -1,   147,    53,    -1,   180,
     175,    -1,   145,    -1,   146,    -1,    14,    -1,    74,   222,
      -1,    87,   222,    -1,     8,    53,    -1,    22,    53,    -1,
      23,   183,    -1,    61,   132,    -1,    62,   132,    -1,   118,
      53,    -1,   120,    53,    -1,   142,   220,    -1,   100,   132,
      -1,   184,    -1,    -1,   184,   185,    -1,   185,    -1,    47,
     132,    -1,    50,   132,    -1,   113,   132,    -1,   115,   132,
      -1,   124,   132,    -1,    27,   132,    -1,   120,    53,    -1,
     139,   187,    -1,   187,   188,    -1,   188,    -1,    19,    53,
      -1,    44,    53,    -1,    21,   223,    -1,   101,    53,    -1,
      80,   224,    -1,    77,   224,    -1,    79,   224,    -1,    76,
     224,    -1,    83,    53,    -1,    11,    53,    -1,   126,   190,
      -1,   128,   132,    -1,    38,   191,   192,    -1,   190,   191,
      -1,   191,    -1,    20,    -1,    24,    -1,    71,    -1,   104,
      -1,   116,    -1,   133,    -1,   137,    -1,   112,    -1,   192,
     193,    -1,   193,    -1,    37,   132,    -1,   144,   194,    -1,
      67,    -1,    89,    -1,    34,    -1,    28,    -1,    91,    -1,
     106,    -1,    25,    -1,   149,    -1,    86,    -1,   152,    -1,
       3,    -1,    29,   198,    -1,   119,   175,   196,    -1,   119,
     176,    75,   176,   196,    -1,   119,    26,   196,    -1,   119,
      56,    26,   196,    -1,   119,    58,    26,   196,    -1,   119,
     125,   196,    -1,    -1,   196,   197,    -1,    43,    -1,    51,
      -1,    63,    -1,    64,    -1,    66,    -1,    72,    -1,    90,
      -1,    92,    -1,    93,    -1,    95,    -1,    96,    -1,    97,
      -1,    99,    -1,   147,    -1,   198,   199,    -1,   199,    -1,
       9,    53,    -1,    81,    53,    -1,    85,    53,    -1,    46,
     175,   201,    -1,   201,   202,    -1,   202,    -1,   135,   224,
      -1,   136,   224,    -1,   131,    53,    -1,   117,   132,    -1,
      39,   223,    -1,    40,   223,    -1,    41,   223,    -1,    42,
     223,    -1,    34,   204,    -1,    28,   204,    -1,   204,   205,
      -1,   205,    -1,     6,    -1,    10,    -1,    17,    -1,    59,
      -1,    85,    -1,    98,    -1,   127,    -1,   138,   207,    -1,
     207,   208,    -1,   208,    -1,     5,   224,    -1,    30,   224,
      -1,    45,   224,    -1,    48,   224,    -1,   102,   224,    -1,
     129,   224,    -1,   130,   224,    -1,   216,    -1,    52,   132,
     172,    -1,    35,    -1,    15,   224,    -1,    18,    53,    -1,
     134,   224,    -1,    32,   210,    -1,    65,   132,    -1,   107,
     132,    -1,    70,   132,    -1,    69,   214,    -1,   105,   221,
      -1,   121,   132,    -1,   123,   211,    -1,   140,   176,    -1,
     140,   176,   212,    -1,   143,   220,    -1,   114,   132,    -1,
     132,    -1,   132,    31,    -1,    -1,   132,   166,   132,    26,
      -1,   132,   166,   132,    -1,   212,   213,    -1,   213,    -1,
     109,    53,    -1,    54,   176,    -1,   214,   215,    -1,   215,
      -1,   132,    -1,   217,   219,   218,    -1,   217,   219,   132,
      -1,    54,    -1,    88,    -1,     4,    -1,    55,    -1,    57,
      -1,   150,    -1,    68,    -1,    51,    -1,    33,    -1,   220,
      53,    -1,    53,    -1,   221,   132,    -1,   132,    -1,   222,
     175,    -1,   175,    -1,    53,    -1,   141,    -1,    36,    -1,
      53,    -1,    31,    -1,   226,   167,   227,   229,   168,    -1,
     156,    -1,   227,   228,   155,    -1,   228,   155,    -1,   157,
     166,   224,    -1,   158,   166,   224,    -1,   229,   230,    -1,
     230,    -1,   232,   167,   231,   233,   168,    -1,   159,   166,
     224,   155,    -1,   122,   166,   175,    -1,   233,   234,    -1,
     234,    -1,   160,   166,   224,   167,   235,   168,    -1,   235,
     236,   155,    -1,   236,   155,    -1,   161,   166,   224,    -1,
     162,   166,   224,    -1,   163,   166,   224,    -1,   164,   166,
     224,    -1,   165,   166,   224,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   297,   297,   301,   302,   303,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   337,
     343,   352,   353,   354,   355,   356,   360,   361,   362,   366,
     370,   371,   375,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,   389,   398,   406,   407,
     417,   419,   421,   432,   434,   436,   441,   443,   445,   447,
     449,   451,   456,   458,   462,   469,   479,   481,   483,   485,
     487,   489,   491,   508,   513,   514,   518,   520,   522,   524,
     526,   528,   530,   532,   534,   536,   546,   548,   557,   565,
     566,   570,   571,   572,   573,   574,   575,   576,   577,   581,
     588,   598,   608,   617,   626,   635,   636,   640,   641,   642,
     643,   644,   645,   646,   655,   659,   664,   669,   674,   687,
     700,   712,   713,   718,   719,   720,   721,   722,   723,   724,
     725,   726,   727,   728,   729,   730,   731,   735,   737,   742,
     743,   744,   752,   757,   759,   764,   765,   766,   767,   768,
     769,   770,   771,   779,   781,   786,   793,   803,   804,   805,
     806,   807,   808,   809,   825,   829,   830,   834,   835,   836,
     837,   838,   839,   840,   849,   850,   866,   872,   874,   876,
     878,   880,   883,   885,   896,   898,   900,   910,   912,   914,
     916,   918,   923,   925,   929,   933,   935,   940,   942,   946,
     947,   951,   952,   956,   971,   976,   984,   985,   989,   990,
     991,   992,   996,   997,   998,  1008,  1009,  1013,  1014,  1018,
    1019,  1023,  1032,  1033,  1037,  1038,  1047,  1062,  1066,  1067,
    1071,  1072,  1076,  1077,  1081,  1086,  1090,  1094,  1095,  1099,
    1104,  1105,  1109,  1111,  1113,  1115,  1117
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
  "T_Ident", "T_Ignore", "T_Includefile", "T_Integer", "T_Interface",
  "T_Ipv4", "T_Ipv4_flag", "T_Ipv6", "T_Ipv6_flag", "T_Kernel", "T_Key",
  "T_Keys", "T_Keysdir", "T_Kod", "T_Mssntp", "T_Leapfile", "T_Limited",
  "T_Link", "T_Listen", "T_Logconfig", "T_Logfile", "T_Loopstats",
  "T_Lowpriotrap", "T_Manycastclient", "T_Manycastserver", "T_Mask",
  "T_Maxclock", "T_Maxdist", "T_Maxpoll", "T_Minclock", "T_Mindist",
  "T_Minimum", "T_Minpoll", "T_Minsane", "T_Mode", "T_Monitor", "T_Month",
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
  "discard_option", "fudge_command", "fudge_factor_list", "fudge_factor",
  "system_option_command", "system_option_list", "system_option",
  "tinker_command", "tinker_option_list", "tinker_option",
  "miscellaneous_command", "drift_parm", "variable_assign",
  "trap_option_list", "trap_option", "log_config_list",
  "log_config_command", "interface_command", "interface_nic",
  "nic_rule_class", "nic_rule_action", "integer_list", "string_list",
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
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,    61,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   169,   170,   171,   171,   171,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   173,
     173,   174,   174,   174,   174,   174,   175,   175,   175,   176,
     177,   177,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   179,   180,   180,
     181,   181,   181,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   183,   183,   184,   184,   185,   185,   185,   185,
     185,   185,   185,   186,   187,   187,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   189,   189,   189,   190,
     190,   191,   191,   191,   191,   191,   191,   191,   191,   192,
     192,   193,   193,   193,   193,   193,   193,   194,   194,   194,
     194,   194,   194,   194,   195,   195,   195,   195,   195,   195,
     195,   196,   196,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   198,   198,   199,
     199,   199,   200,   201,   201,   202,   202,   202,   202,   202,
     202,   202,   202,   203,   203,   204,   204,   205,   205,   205,
     205,   205,   205,   205,   206,   207,   207,   208,   208,   208,
     208,   208,   208,   208,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   210,   210,   210,   211,   211,   212,   212,   213,
     213,   214,   214,   215,   216,   216,   217,   217,   218,   218,
     218,   218,   219,   219,   219,   220,   220,   221,   221,   222,
     222,   223,   223,   223,   224,   224,   225,   226,   227,   227,
     228,   228,   229,   229,   230,   231,   232,   233,   233,   234,
     235,   235,   236,   236,   236,   236,   236
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
       1,     1,     1,     1,     2,     3,     5,     3,     4,     4,
       3,     0,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       2,     2,     3,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     1,     3,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     3,
       2,     2,     1,     2,     0,     4,     3,     2,     1,     2,
       2,     2,     1,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     2,     1,     2,
       1,     1,     1,     1,     1,     1,     5,     1,     3,     2,
       3,     3,     2,     1,     5,     4,     3,     2,     1,     6,
       3,     2,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,    24,    50,     0,     0,     0,    63,     0,
       0,   194,     0,   176,     0,     0,     0,   206,     0,     0,
       0,     0,     0,    25,     0,     0,   207,     0,    23,     0,
       0,    22,     0,     0,     0,     0,     0,    21,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,    49,   227,
       0,     2,     0,     7,     0,     8,     0,     9,    10,    13,
      11,    12,    14,    15,    16,    17,   174,     0,    18,     0,
       5,    53,   225,   224,   177,   178,    54,     0,     0,     0,
       0,     0,     0,     0,    55,    62,    65,   157,   158,   159,
     160,   161,   162,   163,   154,   156,     0,     0,     0,   114,
     138,   192,   180,   153,    91,    92,    93,    94,    98,    95,
      96,    97,     0,     0,     0,    29,     0,    26,     6,    56,
      57,   181,   203,   184,   202,   183,   220,    51,    52,    61,
     218,   185,   182,   191,    58,   121,     0,     0,   121,   121,
      26,    59,   186,     0,   187,    86,    90,    87,   179,     0,
       0,     0,     0,     0,     0,     0,   164,   166,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    73,    75,
     188,   216,    60,   190,     1,     0,     4,    20,    47,   214,
     213,   212,     0,     0,    71,    66,    67,    68,    69,    72,
      70,    64,   155,   139,   140,   141,   137,   193,   106,   105,
       0,   103,   104,     0,    88,   100,    27,    28,     0,     0,
       0,     0,     0,     0,     0,     0,   142,   144,   175,   201,
     219,   217,   117,   121,   121,   120,   115,     0,     0,    89,
     167,   168,   169,   170,   171,   172,   173,   165,    85,    76,
     223,   221,   222,    78,    77,    83,    81,    82,    80,    84,
      79,    74,     0,     0,   189,   198,   215,     3,    32,     0,
      34,    35,     0,     0,     0,     0,    39,    40,    41,    42,
       0,     0,    43,    19,    31,   208,   209,   210,   205,   211,
     204,     0,     0,     0,     0,   101,   113,   109,   111,   107,
     108,   110,   112,   102,    99,   149,   150,   151,   152,   148,
     147,   145,   146,   143,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   122,   118,
     119,   121,   196,   200,   199,   197,    33,    36,    38,    37,
      45,    44,    46,    30,     0,     0,     0,     0,     0,   233,
       0,   229,   116,   195,   230,   231,     0,   228,   226,   232,
       0,   236,     0,     0,     0,     0,     0,   238,     0,     0,
     234,   237,   235,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   239,     0,   241,
     242,   243,   244,   245,   246,   240
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    50,    51,    52,    53,    54,   126,   117,   273,   274,
      55,    56,    57,    58,    84,    85,    86,    59,   168,   169,
      60,   145,   112,   204,   205,   293,    61,   222,   318,    99,
     100,    62,   216,   217,    63,    94,    95,    64,   156,   157,
      65,   102,   144,   254,   255,   123,   124,    66,    67,   280,
     182,   172,   131,   127,   243,    74,    68,    69,   283,   284,
     338,   339,   353,   340,   356,   357,   370,   371
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -137
static const yytype_int16 yypact[] =
{
     126,  -128,   -20,  -137,  -137,     7,   -16,    -8,    32,     4,
      -1,   -85,     4,  -137,   105,   -49,   -84,  -137,   -81,   -79,
     -78,   -68,   -67,  -137,   -49,   -49,  -137,   -55,  -137,   -51,
     -47,  -137,   -37,    45,    -6,    46,   -25,  -137,     5,   105,
       6,     7,    13,   408,    14,    47,    47,  -137,  -137,  -137,
     130,   292,     2,  -137,   -49,  -137,   -49,  -137,  -137,  -137,
    -137,  -137,  -137,  -137,  -137,  -137,  -137,    -2,  -137,   -14,
    -137,  -137,  -137,  -137,  -137,  -137,  -137,    31,    35,    36,
      39,    43,   113,    49,  -137,    32,  -137,  -137,  -137,  -137,
    -137,  -137,  -137,  -137,     4,  -137,   124,   129,   131,    -1,
    -137,   148,  -137,     4,  -137,  -137,  -137,  -137,  -137,  -137,
    -137,  -137,   -11,    51,    54,  -137,    34,  -137,   428,  -137,
    -137,  -137,  -137,   -68,  -137,  -137,  -137,   -49,   -49,  -137,
    -137,    57,  -137,  -137,  -137,  -137,    -4,     3,  -137,  -137,
     115,  -137,  -137,    26,  -137,   105,  -137,  -137,  -137,     7,
       7,     7,     7,     7,     7,     7,    13,  -137,   140,   144,
     -21,   145,     7,     7,     7,     7,   149,   150,   408,  -137,
     -42,  -137,   151,   151,  -137,    50,  -137,   527,  -137,  -137,
    -137,  -137,     0,  -118,  -137,  -137,  -137,  -137,  -137,  -137,
    -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,
      69,  -137,  -137,    10,   -11,  -137,  -137,  -137,   -21,   -21,
     -21,   -21,    74,   154,     7,     7,    34,  -137,  -137,  -137,
    -137,  -137,   532,  -137,  -137,   532,   532,    14,    76,  -137,
    -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,
    -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,
    -137,  -137,    14,   157,   -42,  -137,  -137,  -137,  -137,     7,
    -137,  -137,   158,   159,   162,   163,  -137,  -137,  -137,  -137,
     167,   169,  -137,   527,  -137,  -137,  -137,  -137,  -137,  -137,
    -137,    58,    59,  -116,    68,  -137,  -137,  -137,  -137,  -137,
    -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,
    -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,
    -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,  -137,   532,
     532,  -137,   201,  -137,  -137,  -137,  -137,  -137,  -137,  -137,
    -137,  -137,  -137,  -137,     7,     7,    62,    75,  -106,  -137,
      65,  -137,   532,  -137,  -137,  -137,   -49,  -137,  -137,  -137,
      77,  -137,    71,    79,     7,    84,  -132,  -137,    80,     7,
    -137,  -137,  -137,    86,   -93,    85,    89,    90,    91,    92,
     -71,    88,     7,     7,     7,     7,     7,  -137,   104,  -137,
    -137,  -137,  -137,  -137,  -137,  -137
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -137,  -137,  -137,   -32,  -137,  -137,   -10,   -33,  -137,   -12,
    -137,  -137,  -137,  -137,  -137,  -137,   156,  -137,  -137,    94,
    -137,  -137,    -9,  -137,    63,  -137,  -137,  -136,  -137,  -137,
     164,  -137,  -137,    60,  -137,   258,   -69,  -137,  -137,   117,
    -137,  -137,  -137,  -137,    20,  -137,   152,  -137,  -137,  -137,
    -137,   231,  -137,   253,  -105,   -41,  -137,  -137,  -137,    -3,
    -137,   -59,  -137,  -137,  -137,   -73,  -137,   -86
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     148,   140,   225,   226,   275,   116,   336,   113,    96,   114,
      87,   170,   252,   286,    88,   240,   336,   198,   149,   175,
     135,    89,   223,   199,   139,   192,   200,    70,   355,   224,
     146,   179,   241,    71,   192,   287,   360,    75,    72,   281,
     282,   281,   282,   150,   177,    76,   178,   101,   118,   180,
     136,   119,   137,   120,   121,   276,   201,   277,   151,    77,
      73,   152,   348,    90,   122,   125,   181,   253,   365,   366,
     367,   368,   369,   208,   209,   210,   211,   129,   202,    78,
      97,   130,    79,   115,    98,   132,   218,   319,   320,    91,
     365,   366,   367,   368,   369,   133,   288,   377,   134,   141,
     171,   289,    92,   295,   296,   297,   298,   142,   230,   231,
     232,   233,   234,   235,   236,   153,   290,   220,   220,   138,
     242,   245,   246,   247,   248,   104,   115,     1,   206,   105,
     174,    93,   278,   203,     2,   207,   229,   143,   147,     3,
       4,     5,   154,   155,     6,    80,   115,    81,     7,     8,
     279,   212,    82,   183,     9,    10,    83,   176,    11,   291,
      12,    13,   292,   184,    14,   213,   189,   185,   186,   214,
     215,   187,    15,   301,   302,   188,   106,   193,    16,   197,
      17,   190,   194,   206,   195,   342,   207,    18,    19,   221,
     227,    20,   228,   238,   321,    21,    22,   239,   244,    23,
      24,   285,   249,   250,   256,   257,   299,   300,   322,   107,
     324,   327,   328,    25,    26,   329,   330,   108,   326,   323,
     331,   109,   332,   341,   334,   335,    27,   343,   346,    28,
     347,    29,   350,    30,    31,   362,   352,   354,   110,   355,
      32,   191,   111,   379,    33,    34,    35,    36,    37,    38,
     359,   372,    39,   364,    40,   373,   374,   375,   376,   385,
      41,   333,   251,   196,    42,    43,    44,   294,    45,    46,
     103,    47,    48,   237,   325,   219,   303,   173,   128,   349,
     337,    -6,    49,   361,   378,     0,     0,     0,     0,     0,
       0,     0,     0,   344,   345,     0,     0,     0,     0,     0,
       2,     0,     0,     0,     0,     3,     4,     5,     0,     0,
       6,     0,     0,   358,     7,     8,     0,     0,   363,     0,
       9,    10,     0,     0,    11,     0,    12,    13,     0,     0,
      14,   380,   381,   382,   383,   384,   351,     0,    15,     0,
       0,     0,     0,     0,    16,     0,    17,     0,     0,     0,
       0,     0,     0,    18,    19,     0,     0,    20,     0,     0,
       0,    21,    22,     0,     0,    23,    24,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    27,     0,     0,    28,     0,    29,     0,    30,
      31,     0,     0,     0,     0,     0,    32,     0,     0,     0,
      33,    34,    35,    36,    37,    38,     0,     0,    39,   158,
      40,     0,     0,     0,     0,     0,    41,   159,     0,   160,
      42,    43,    44,     0,    45,    46,     2,    47,    48,     0,
       0,     3,     4,     5,     0,     0,     6,    -6,    49,     0,
       7,     8,   161,     0,     0,     0,     9,    10,     0,     0,
      11,     0,    12,    13,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,    15,     0,     0,     0,     0,     0,
      16,     0,    17,     0,   162,   163,     0,   164,   165,    18,
      19,   166,     0,    20,     0,     0,     0,    21,    22,     0,
       0,    23,    24,     0,     0,     0,     0,     0,     0,   167,
       0,     0,     0,     0,     0,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
       0,    28,     0,    29,   258,    30,    31,     0,     0,   259,
       0,     0,    32,   260,     0,     0,    33,    34,    35,    36,
      37,    38,     0,     0,    39,     0,    40,     0,     0,     0,
       0,     0,    41,     0,     0,     0,    42,    43,    44,     0,
      45,    46,     0,    47,    48,   304,   261,     0,     0,     0,
       0,     0,     0,   305,    49,     0,     0,   262,     0,     0,
       0,     0,     0,     0,     0,   306,   307,     0,   308,     0,
       0,     0,     0,     0,   309,   263,     0,     0,     0,   264,
       0,   265,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   266,   310,     0,   311,   312,     0,   313,   314,   315,
       0,   316,     0,     0,     0,     0,     0,   267,   268,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   269,     0,
     270,     0,     0,     0,   271,     0,     0,     0,   272,   317
};

static const yytype_int16 yycheck[] =
{
      41,    34,   138,   139,     4,    15,   122,    56,     9,    58,
       6,    44,    54,     3,    10,    36,   122,    28,     5,    51,
      26,    17,    26,    34,    34,    94,    37,   155,   160,    26,
      39,    33,    53,    53,   103,    25,   168,    53,    31,   157,
     158,   157,   158,    30,    54,    53,    56,   132,   132,    51,
      56,   132,    58,   132,   132,    55,    67,    57,    45,    27,
      53,    48,   168,    59,   132,   132,    68,   109,   161,   162,
     163,   164,   165,    39,    40,    41,    42,   132,    89,    47,
      81,   132,    50,   132,    85,   132,   118,   223,   224,    85,
     161,   162,   163,   164,   165,   132,    86,   168,    53,    53,
      53,    91,    98,   208,   209,   210,   211,   132,   149,   150,
     151,   152,   153,   154,   155,   102,   106,   127,   128,   125,
     141,   162,   163,   164,   165,    20,   132,     1,   132,    24,
       0,   127,   132,   144,     8,   132,   145,   132,   132,    13,
      14,    15,   129,   130,    18,   113,   132,   115,    22,    23,
     150,   117,   120,   167,    28,    29,   124,   155,    32,   149,
      34,    35,   152,   132,    38,   131,    53,   132,   132,   135,
     136,   132,    46,   214,   215,   132,    71,    53,    52,    31,
      54,   132,    53,   132,    53,   321,   132,    61,    62,   132,
      75,    65,   166,    53,   227,    69,    70,    53,    53,    73,
      74,   132,    53,    53,    53,   155,   132,    53,   132,   104,
      53,    53,    53,    87,    88,    53,    53,   112,   259,   252,
      53,   116,    53,   155,   166,   166,   100,    26,   166,   103,
     155,   105,   167,   107,   108,   155,   159,   166,   133,   160,
     114,    85,   137,   155,   118,   119,   120,   121,   122,   123,
     166,   166,   126,   167,   128,   166,   166,   166,   166,   155,
     134,   273,   168,    99,   138,   139,   140,   204,   142,   143,
      12,   145,   146,   156,   254,   123,   216,    46,    25,   338,
     283,   155,   156,   356,   370,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   334,   335,    -1,    -1,    -1,    -1,    -1,
       8,    -1,    -1,    -1,    -1,    13,    14,    15,    -1,    -1,
      18,    -1,    -1,   354,    22,    23,    -1,    -1,   359,    -1,
      28,    29,    -1,    -1,    32,    -1,    34,    35,    -1,    -1,
      38,   372,   373,   374,   375,   376,   346,    -1,    46,    -1,
      -1,    -1,    -1,    -1,    52,    -1,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    61,    62,    -1,    -1,    65,    -1,    -1,
      -1,    69,    70,    -1,    -1,    73,    74,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,
      88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   100,    -1,    -1,   103,    -1,   105,    -1,   107,
     108,    -1,    -1,    -1,    -1,    -1,   114,    -1,    -1,    -1,
     118,   119,   120,   121,   122,   123,    -1,    -1,   126,    11,
     128,    -1,    -1,    -1,    -1,    -1,   134,    19,    -1,    21,
     138,   139,   140,    -1,   142,   143,     8,   145,   146,    -1,
      -1,    13,    14,    15,    -1,    -1,    18,   155,   156,    -1,
      22,    23,    44,    -1,    -1,    -1,    28,    29,    -1,    -1,
      32,    -1,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    54,    -1,    76,    77,    -1,    79,    80,    61,
      62,    83,    -1,    65,    -1,    -1,    -1,    69,    70,    -1,
      -1,    73,    74,    -1,    -1,    -1,    -1,    -1,    -1,   101,
      -1,    -1,    -1,    -1,    -1,    87,    88,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   100,    -1,
      -1,   103,    -1,   105,     7,   107,   108,    -1,    -1,    12,
      -1,    -1,   114,    16,    -1,    -1,   118,   119,   120,   121,
     122,   123,    -1,    -1,   126,    -1,   128,    -1,    -1,    -1,
      -1,    -1,   134,    -1,    -1,    -1,   138,   139,   140,    -1,
     142,   143,    -1,   145,   146,    43,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    51,   156,    -1,    -1,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    64,    -1,    66,    -1,
      -1,    -1,    -1,    -1,    72,    78,    -1,    -1,    -1,    82,
      -1,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    94,    90,    -1,    92,    93,    -1,    95,    96,    97,
      -1,    99,    -1,    -1,    -1,    -1,    -1,   110,   111,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   141,    -1,
     143,    -1,    -1,    -1,   147,    -1,    -1,    -1,   151,   147
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     8,    13,    14,    15,    18,    22,    23,    28,
      29,    32,    34,    35,    38,    46,    52,    54,    61,    62,
      65,    69,    70,    73,    74,    87,    88,   100,   103,   105,
     107,   108,   114,   118,   119,   120,   121,   122,   123,   126,
     128,   134,   138,   139,   140,   142,   143,   145,   146,   156,
     170,   171,   172,   173,   174,   179,   180,   181,   182,   186,
     189,   195,   200,   203,   206,   209,   216,   217,   225,   226,
     155,    53,    31,    53,   224,    53,    53,    27,    47,    50,
     113,   115,   120,   124,   183,   184,   185,     6,    10,    17,
      59,    85,    98,   127,   204,   205,     9,    81,    85,   198,
     199,   132,   210,   204,    20,    24,    71,   104,   112,   116,
     133,   137,   191,    56,    58,   132,   175,   176,   132,   132,
     132,   132,   132,   214,   215,   132,   175,   222,   222,   132,
     132,   221,   132,   132,    53,    26,    56,    58,   125,   175,
     176,    53,   132,   132,   211,   190,   191,   132,   224,     5,
      30,    45,    48,   102,   129,   130,   207,   208,    11,    19,
      21,    44,    76,    77,    79,    80,    83,   101,   187,   188,
     176,    53,   220,   220,     0,   172,   155,   175,   175,    33,
      51,    68,   219,   167,   132,   132,   132,   132,   132,    53,
     132,   185,   205,    53,    53,    53,   199,    31,    28,    34,
      37,    67,    89,   144,   192,   193,   132,   132,    39,    40,
      41,    42,   117,   131,   135,   136,   201,   202,   172,   215,
     175,   132,   196,    26,    26,   196,   196,    75,   166,   191,
     224,   224,   224,   224,   224,   224,   224,   208,    53,    53,
      36,    53,   141,   223,    53,   224,   224,   224,   224,    53,
      53,   188,    54,   109,   212,   213,    53,   155,     7,    12,
      16,    49,    60,    78,    82,    84,    94,   110,   111,   141,
     143,   147,   151,   177,   178,     4,    55,    57,   132,   150,
     218,   157,   158,   227,   228,   132,     3,    25,    86,    91,
     106,   149,   152,   194,   193,   223,   223,   223,   223,   132,
      53,   224,   224,   202,    43,    51,    63,    64,    66,    72,
      90,    92,    93,    95,    96,    97,    99,   147,   197,   196,
     196,   176,   132,   176,    53,   213,   224,    53,    53,    53,
      53,    53,    53,   178,   166,   166,   122,   228,   229,   230,
     232,   155,   196,    26,   224,   224,   166,   155,   168,   230,
     167,   175,   159,   231,   166,   160,   233,   234,   224,   166,
     168,   234,   155,   224,   167,   161,   162,   163,   164,   165,
     235,   236,   166,   166,   166,   166,   166,   168,   236,   155,
     224,   224,   224,   224,   224,   155
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
#line 304 "ntp_parser.y"
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
#line 338 "ntp_parser.y"
    {
			struct peer_node *my_node =  create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue));
			if (my_node)
				enqueue(cfgt.peers, my_node);
		}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 344 "ntp_parser.y"
    {
			struct peer_node *my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
			if (my_node)
				enqueue(cfgt.peers, my_node);
		}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 361 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 362 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET6); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 366 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), 0); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 370 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 371 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 375 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 376 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 377 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 378 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 379 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 380 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 381 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 382 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 383 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 384 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 385 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 386 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 387 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 388 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 389 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 399 "ntp_parser.y"
    {
			struct unpeer_node *my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				enqueue(cfgt.unpeers, my_node);
		}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 418 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 420 "ntp_parser.y"
    { append_queue(cfgt.manycastserver, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 422 "ntp_parser.y"
    { append_queue(cfgt.multicastclient, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 433 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer))); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 435 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 437 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			append_queue(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 442 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 444 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 446 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 448 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 450 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Queue); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 452 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 458 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 463 "ntp_parser.y"
    { 
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 470 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 480 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 482 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 484 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 486 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 488 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 490 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 492 "ntp_parser.y"
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
#line 509 "ntp_parser.y"
    { append_queue(cfgt.orphan_cmds,(yyvsp[(2) - (2)].Queue)); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 513 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 514 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 519 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 521 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 523 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 525 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 527 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 529 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 531 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 533 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 535 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 537 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 547 "ntp_parser.y"
    { append_queue(cfgt.stats_list, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 549 "ntp_parser.y"
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
#line 558 "ntp_parser.y"
    {
			enqueue(cfgt.filegen_opts,
				create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Queue)));
		}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 565 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 566 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 582 "ntp_parser.y"
    {
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 589 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 599 "ntp_parser.y"
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
#line 609 "ntp_parser.y"
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
#line 618 "ntp_parser.y"
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
#line 627 "ntp_parser.y"
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
#line 635 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 636 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 656 "ntp_parser.y"
    {
			append_queue(cfgt.discard_opts, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 660 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 665 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Queue), ip_file->line_no));
		}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 670 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 675 "ntp_parser.y"
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

  case 119:

/* Line 1455 of yacc.c  */
#line 688 "ntp_parser.y"
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

  case 120:

/* Line 1455 of yacc.c  */
#line 701 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(
					NULL, NULL,
					enqueue((yyvsp[(3) - (3)].Queue), create_ival((yyvsp[(2) - (3)].Integer))),
					ip_file->line_no));
		}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 712 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 714 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 736 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 738 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 742 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 743 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 744 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 753 "ntp_parser.y"
    { enqueue(cfgt.fudge, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 758 "ntp_parser.y"
    { enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 760 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 764 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 765 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 766 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 767 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 768 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 769 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 770 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 771 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 780 "ntp_parser.y"
    { append_queue(cfgt.enable_opts, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 782 "ntp_parser.y"
    { append_queue(cfgt.disable_opts, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 787 "ntp_parser.y"
    {
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 794 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 803 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 804 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 805 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 806 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 807 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 808 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 810 "ntp_parser.y"
    { 
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("enable/disable stats remote configuration ignored");
			}
		}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 825 "ntp_parser.y"
    { append_queue(cfgt.tinker, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 829 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 830 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 834 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 835 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 836 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 837 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 838 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 839 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 840 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 851 "ntp_parser.y"
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

  case 176:

/* Line 1455 of yacc.c  */
#line 867 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 873 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double))); }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 875 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer))); }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 877 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double))); }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 879 "ntp_parser.y"
    { /* Null action, possibly all null parms */ }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 881 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 884 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 886 "ntp_parser.y"
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

  case 184:

/* Line 1455 of yacc.c  */
#line 897 "ntp_parser.y"
    { append_queue(cfgt.logconfig, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 899 "ntp_parser.y"
    { append_queue(cfgt.phone, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 901 "ntp_parser.y"
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

  case 187:

/* Line 1455 of yacc.c  */
#line 911 "ntp_parser.y"
    { enqueue(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 913 "ntp_parser.y"
    { enqueue(cfgt.trap, create_addr_opts_node((yyvsp[(2) - (2)].Address_node), NULL)); }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 915 "ntp_parser.y"
    { enqueue(cfgt.trap, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 917 "ntp_parser.y"
    { append_queue(cfgt.ttl, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 919 "ntp_parser.y"
    { enqueue(cfgt.qos, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 924 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String))); }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 926 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double)));
			  enqueue(cfgt.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String))); }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 929 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 934 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 936 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (3)].String), (yyvsp[(3) - (3)].String), 0); }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 941 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 942 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 946 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 947 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_pval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 951 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 952 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 957 "ntp_parser.y"
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

  case 204:

/* Line 1455 of yacc.c  */
#line 972 "ntp_parser.y"
    {
			enqueue(cfgt.nic_rules,
				create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer)));
		}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 977 "ntp_parser.y"
    {
			enqueue(cfgt.nic_rules,
				create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer)));
		}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 1008 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 1009 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 1013 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_pval((yyvsp[(2) - (2)].String))); }
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 1014 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_pval((yyvsp[(1) - (1)].String))); }
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 1018 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 1019 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Address_node)); }
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 1024 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			}
			else
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
		}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 1032 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 1033 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 1037 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 1048 "ntp_parser.y"
    {
			cfgt.sim_details = create_sim_node((yyvsp[(3) - (5)].Queue), (yyvsp[(4) - (5)].Queue));

			/* Reset the old_config_style variable */
			old_config_style = 1;
		}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 1062 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 1066 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 1067 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 1071 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 1072 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 1076 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_server)); }
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 1077 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_server)); }
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 1082 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Queue)); }
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 1086 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 1090 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 1094 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_script)); }
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 1095 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_script)); }
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 1100 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Queue)); }
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 1104 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 1105 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 1110 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 1112 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 1114 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 1116 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 1118 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1455 of yacc.c  */
#line 3651 "ntp_parser.c"
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
#line 1122 "ntp_parser.y"


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


