/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 11 "ntp_parser.y" /* yacc.c:339  */

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

  #define YYMALLOC	emalloc
  #define YYFREE	free
  #define YYERROR_VERBOSE
  #define YYMAXDEPTH	1000	/* stop the madness sooner */
  void yyerror(const char *msg);

  #ifdef SIM
  #  define ONLY_SIM(a)	(a)
  #else
  #  define ONLY_SIM(a)	NULL
  #endif

#line 100 "../../ntpd/ntp_parser.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_Abbrev = 258,
    T_Age = 259,
    T_All = 260,
    T_Allan = 261,
    T_Allpeers = 262,
    T_Auth = 263,
    T_Autokey = 264,
    T_Automax = 265,
    T_Average = 266,
    T_Bclient = 267,
    T_Beacon = 268,
    T_Broadcast = 269,
    T_Broadcastclient = 270,
    T_Broadcastdelay = 271,
    T_Burst = 272,
    T_Calibrate = 273,
    T_Ceiling = 274,
    T_Clockstats = 275,
    T_Cohort = 276,
    T_ControlKey = 277,
    T_Crypto = 278,
    T_Cryptostats = 279,
    T_Ctl = 280,
    T_Day = 281,
    T_Default = 282,
    T_Digest = 283,
    T_Disable = 284,
    T_Discard = 285,
    T_Dispersion = 286,
    T_Double = 287,
    T_Driftfile = 288,
    T_Drop = 289,
    T_Dscp = 290,
    T_Ellipsis = 291,
    T_Enable = 292,
    T_End = 293,
    T_False = 294,
    T_File = 295,
    T_Filegen = 296,
    T_Filenum = 297,
    T_Flag1 = 298,
    T_Flag2 = 299,
    T_Flag3 = 300,
    T_Flag4 = 301,
    T_Flake = 302,
    T_Floor = 303,
    T_Freq = 304,
    T_Fudge = 305,
    T_Host = 306,
    T_Huffpuff = 307,
    T_Iburst = 308,
    T_Ident = 309,
    T_Ignore = 310,
    T_Incalloc = 311,
    T_Incmem = 312,
    T_Initalloc = 313,
    T_Initmem = 314,
    T_Includefile = 315,
    T_Integer = 316,
    T_Interface = 317,
    T_Intrange = 318,
    T_Io = 319,
    T_Ipv4 = 320,
    T_Ipv4_flag = 321,
    T_Ipv6 = 322,
    T_Ipv6_flag = 323,
    T_Kernel = 324,
    T_Key = 325,
    T_Keys = 326,
    T_Keysdir = 327,
    T_Kod = 328,
    T_Mssntp = 329,
    T_Leapfile = 330,
    T_Limited = 331,
    T_Link = 332,
    T_Listen = 333,
    T_Logconfig = 334,
    T_Logfile = 335,
    T_Loopstats = 336,
    T_Lowpriotrap = 337,
    T_Manycastclient = 338,
    T_Manycastserver = 339,
    T_Mask = 340,
    T_Maxage = 341,
    T_Maxclock = 342,
    T_Maxdepth = 343,
    T_Maxdist = 344,
    T_Maxmem = 345,
    T_Maxpoll = 346,
    T_Mdnstries = 347,
    T_Mem = 348,
    T_Memlock = 349,
    T_Minclock = 350,
    T_Mindepth = 351,
    T_Mindist = 352,
    T_Minimum = 353,
    T_Minpoll = 354,
    T_Minsane = 355,
    T_Mode = 356,
    T_Mode7 = 357,
    T_Monitor = 358,
    T_Month = 359,
    T_Mru = 360,
    T_Multicastclient = 361,
    T_Nic = 362,
    T_Nolink = 363,
    T_Nomodify = 364,
    T_Nomrulist = 365,
    T_None = 366,
    T_Nonvolatile = 367,
    T_Nopeer = 368,
    T_Noquery = 369,
    T_Noselect = 370,
    T_Noserve = 371,
    T_Notrap = 372,
    T_Notrust = 373,
    T_Ntp = 374,
    T_Ntpport = 375,
    T_NtpSignDsocket = 376,
    T_Orphan = 377,
    T_Orphanwait = 378,
    T_Panic = 379,
    T_Peer = 380,
    T_Peerstats = 381,
    T_Phone = 382,
    T_Pid = 383,
    T_Pidfile = 384,
    T_Pool = 385,
    T_Port = 386,
    T_Preempt = 387,
    T_Prefer = 388,
    T_Protostats = 389,
    T_Pw = 390,
    T_Randfile = 391,
    T_Rawstats = 392,
    T_Refid = 393,
    T_Requestkey = 394,
    T_Reset = 395,
    T_Restrict = 396,
    T_Revoke = 397,
    T_Rlimit = 398,
    T_Saveconfigdir = 399,
    T_Server = 400,
    T_Setvar = 401,
    T_Source = 402,
    T_Stacksize = 403,
    T_Statistics = 404,
    T_Stats = 405,
    T_Statsdir = 406,
    T_Step = 407,
    T_Stepback = 408,
    T_Stepfwd = 409,
    T_Stepout = 410,
    T_Stratum = 411,
    T_String = 412,
    T_Sys = 413,
    T_Sysstats = 414,
    T_Tick = 415,
    T_Time1 = 416,
    T_Time2 = 417,
    T_Timer = 418,
    T_Timingstats = 419,
    T_Tinker = 420,
    T_Tos = 421,
    T_Trap = 422,
    T_True = 423,
    T_Trustedkey = 424,
    T_Ttl = 425,
    T_Type = 426,
    T_U_int = 427,
    T_Unconfig = 428,
    T_Unpeer = 429,
    T_Version = 430,
    T_WanderThreshold = 431,
    T_Week = 432,
    T_Wildcard = 433,
    T_Xleave = 434,
    T_Year = 435,
    T_Flag = 436,
    T_EOC = 437,
    T_Simulate = 438,
    T_Beep_Delay = 439,
    T_Sim_Duration = 440,
    T_Server_Offset = 441,
    T_Duration = 442,
    T_Freq_Offset = 443,
    T_Wander = 444,
    T_Jitter = 445,
    T_Prop_Delay = 446,
    T_Proc_Delay = 447
  };
#endif
/* Tokens.  */
#define T_Abbrev 258
#define T_Age 259
#define T_All 260
#define T_Allan 261
#define T_Allpeers 262
#define T_Auth 263
#define T_Autokey 264
#define T_Automax 265
#define T_Average 266
#define T_Bclient 267
#define T_Beacon 268
#define T_Broadcast 269
#define T_Broadcastclient 270
#define T_Broadcastdelay 271
#define T_Burst 272
#define T_Calibrate 273
#define T_Ceiling 274
#define T_Clockstats 275
#define T_Cohort 276
#define T_ControlKey 277
#define T_Crypto 278
#define T_Cryptostats 279
#define T_Ctl 280
#define T_Day 281
#define T_Default 282
#define T_Digest 283
#define T_Disable 284
#define T_Discard 285
#define T_Dispersion 286
#define T_Double 287
#define T_Driftfile 288
#define T_Drop 289
#define T_Dscp 290
#define T_Ellipsis 291
#define T_Enable 292
#define T_End 293
#define T_False 294
#define T_File 295
#define T_Filegen 296
#define T_Filenum 297
#define T_Flag1 298
#define T_Flag2 299
#define T_Flag3 300
#define T_Flag4 301
#define T_Flake 302
#define T_Floor 303
#define T_Freq 304
#define T_Fudge 305
#define T_Host 306
#define T_Huffpuff 307
#define T_Iburst 308
#define T_Ident 309
#define T_Ignore 310
#define T_Incalloc 311
#define T_Incmem 312
#define T_Initalloc 313
#define T_Initmem 314
#define T_Includefile 315
#define T_Integer 316
#define T_Interface 317
#define T_Intrange 318
#define T_Io 319
#define T_Ipv4 320
#define T_Ipv4_flag 321
#define T_Ipv6 322
#define T_Ipv6_flag 323
#define T_Kernel 324
#define T_Key 325
#define T_Keys 326
#define T_Keysdir 327
#define T_Kod 328
#define T_Mssntp 329
#define T_Leapfile 330
#define T_Limited 331
#define T_Link 332
#define T_Listen 333
#define T_Logconfig 334
#define T_Logfile 335
#define T_Loopstats 336
#define T_Lowpriotrap 337
#define T_Manycastclient 338
#define T_Manycastserver 339
#define T_Mask 340
#define T_Maxage 341
#define T_Maxclock 342
#define T_Maxdepth 343
#define T_Maxdist 344
#define T_Maxmem 345
#define T_Maxpoll 346
#define T_Mdnstries 347
#define T_Mem 348
#define T_Memlock 349
#define T_Minclock 350
#define T_Mindepth 351
#define T_Mindist 352
#define T_Minimum 353
#define T_Minpoll 354
#define T_Minsane 355
#define T_Mode 356
#define T_Mode7 357
#define T_Monitor 358
#define T_Month 359
#define T_Mru 360
#define T_Multicastclient 361
#define T_Nic 362
#define T_Nolink 363
#define T_Nomodify 364
#define T_Nomrulist 365
#define T_None 366
#define T_Nonvolatile 367
#define T_Nopeer 368
#define T_Noquery 369
#define T_Noselect 370
#define T_Noserve 371
#define T_Notrap 372
#define T_Notrust 373
#define T_Ntp 374
#define T_Ntpport 375
#define T_NtpSignDsocket 376
#define T_Orphan 377
#define T_Orphanwait 378
#define T_Panic 379
#define T_Peer 380
#define T_Peerstats 381
#define T_Phone 382
#define T_Pid 383
#define T_Pidfile 384
#define T_Pool 385
#define T_Port 386
#define T_Preempt 387
#define T_Prefer 388
#define T_Protostats 389
#define T_Pw 390
#define T_Randfile 391
#define T_Rawstats 392
#define T_Refid 393
#define T_Requestkey 394
#define T_Reset 395
#define T_Restrict 396
#define T_Revoke 397
#define T_Rlimit 398
#define T_Saveconfigdir 399
#define T_Server 400
#define T_Setvar 401
#define T_Source 402
#define T_Stacksize 403
#define T_Statistics 404
#define T_Stats 405
#define T_Statsdir 406
#define T_Step 407
#define T_Stepback 408
#define T_Stepfwd 409
#define T_Stepout 410
#define T_Stratum 411
#define T_String 412
#define T_Sys 413
#define T_Sysstats 414
#define T_Tick 415
#define T_Time1 416
#define T_Time2 417
#define T_Timer 418
#define T_Timingstats 419
#define T_Tinker 420
#define T_Tos 421
#define T_Trap 422
#define T_True 423
#define T_Trustedkey 424
#define T_Ttl 425
#define T_Type 426
#define T_U_int 427
#define T_Unconfig 428
#define T_Unpeer 429
#define T_Version 430
#define T_WanderThreshold 431
#define T_Week 432
#define T_Wildcard 433
#define T_Xleave 434
#define T_Year 435
#define T_Flag 436
#define T_EOC 437
#define T_Simulate 438
#define T_Beep_Delay 439
#define T_Sim_Duration 440
#define T_Server_Offset 441
#define T_Duration 442
#define T_Freq_Offset 443
#define T_Wander 444
#define T_Jitter 445
#define T_Prop_Delay 446
#define T_Proc_Delay 447

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 51 "ntp_parser.y" /* yacc.c:355  */

	char *			String;
	double			Double;
	int			Integer;
	unsigned		U_int;
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

#line 543 "../../ntpd/ntp_parser.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 558 "../../ntpd/ntp_parser.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  209
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   627

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  198
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  105
/* YYNRULES -- Number of rules.  */
#define YYNRULES  312
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  418

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   447

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     194,   195,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   193,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   196,     2,   197,     2,     2,     2,     2,
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
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   365,   365,   369,   370,   371,   386,   387,   388,   389,
     390,   391,   392,   393,   394,   395,   396,   397,   398,   399,
     407,   417,   418,   419,   420,   421,   425,   426,   431,   436,
     438,   444,   445,   453,   454,   455,   459,   464,   465,   466,
     467,   468,   469,   470,   471,   475,   477,   482,   483,   484,
     485,   486,   487,   491,   496,   505,   515,   516,   526,   528,
     530,   532,   543,   550,   552,   557,   559,   561,   563,   565,
     574,   580,   581,   589,   591,   603,   604,   605,   606,   607,
     616,   621,   626,   634,   636,   638,   643,   644,   645,   646,
     647,   648,   652,   653,   654,   655,   664,   666,   675,   685,
     690,   698,   699,   700,   701,   702,   703,   704,   705,   710,
     711,   719,   729,   738,   753,   758,   759,   763,   764,   768,
     769,   770,   771,   772,   773,   774,   783,   787,   791,   799,
     807,   815,   830,   845,   858,   859,   867,   868,   869,   870,
     871,   872,   873,   874,   875,   876,   877,   878,   879,   880,
     881,   885,   890,   898,   903,   904,   905,   909,   914,   922,
     927,   928,   929,   930,   931,   932,   933,   934,   942,   952,
     957,   965,   967,   969,   971,   973,   978,   979,   983,   984,
     985,   986,   994,   999,  1004,  1012,  1017,  1018,  1019,  1028,
    1030,  1035,  1040,  1048,  1050,  1067,  1068,  1069,  1070,  1071,
    1072,  1076,  1077,  1085,  1090,  1095,  1103,  1108,  1109,  1110,
    1111,  1112,  1113,  1114,  1115,  1116,  1117,  1126,  1127,  1128,
    1135,  1142,  1149,  1165,  1184,  1186,  1188,  1190,  1192,  1194,
    1201,  1206,  1207,  1208,  1212,  1216,  1217,  1218,  1222,  1223,
    1227,  1234,  1244,  1253,  1258,  1260,  1265,  1266,  1274,  1276,
    1284,  1289,  1297,  1322,  1329,  1339,  1340,  1344,  1345,  1346,
    1347,  1351,  1352,  1353,  1357,  1362,  1367,  1375,  1376,  1377,
    1378,  1379,  1380,  1381,  1391,  1396,  1404,  1409,  1417,  1419,
    1423,  1428,  1433,  1441,  1446,  1454,  1463,  1464,  1468,  1469,
    1478,  1496,  1500,  1505,  1513,  1518,  1519,  1523,  1528,  1536,
    1541,  1546,  1551,  1556,  1564,  1569,  1574,  1582,  1587,  1588,
    1589,  1590,  1591
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_Abbrev", "T_Age", "T_All", "T_Allan",
  "T_Allpeers", "T_Auth", "T_Autokey", "T_Automax", "T_Average",
  "T_Bclient", "T_Beacon", "T_Broadcast", "T_Broadcastclient",
  "T_Broadcastdelay", "T_Burst", "T_Calibrate", "T_Ceiling",
  "T_Clockstats", "T_Cohort", "T_ControlKey", "T_Crypto", "T_Cryptostats",
  "T_Ctl", "T_Day", "T_Default", "T_Digest", "T_Disable", "T_Discard",
  "T_Dispersion", "T_Double", "T_Driftfile", "T_Drop", "T_Dscp",
  "T_Ellipsis", "T_Enable", "T_End", "T_False", "T_File", "T_Filegen",
  "T_Filenum", "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4", "T_Flake",
  "T_Floor", "T_Freq", "T_Fudge", "T_Host", "T_Huffpuff", "T_Iburst",
  "T_Ident", "T_Ignore", "T_Incalloc", "T_Incmem", "T_Initalloc",
  "T_Initmem", "T_Includefile", "T_Integer", "T_Interface", "T_Intrange",
  "T_Io", "T_Ipv4", "T_Ipv4_flag", "T_Ipv6", "T_Ipv6_flag", "T_Kernel",
  "T_Key", "T_Keys", "T_Keysdir", "T_Kod", "T_Mssntp", "T_Leapfile",
  "T_Limited", "T_Link", "T_Listen", "T_Logconfig", "T_Logfile",
  "T_Loopstats", "T_Lowpriotrap", "T_Manycastclient", "T_Manycastserver",
  "T_Mask", "T_Maxage", "T_Maxclock", "T_Maxdepth", "T_Maxdist",
  "T_Maxmem", "T_Maxpoll", "T_Mdnstries", "T_Mem", "T_Memlock",
  "T_Minclock", "T_Mindepth", "T_Mindist", "T_Minimum", "T_Minpoll",
  "T_Minsane", "T_Mode", "T_Mode7", "T_Monitor", "T_Month", "T_Mru",
  "T_Multicastclient", "T_Nic", "T_Nolink", "T_Nomodify", "T_Nomrulist",
  "T_None", "T_Nonvolatile", "T_Nopeer", "T_Noquery", "T_Noselect",
  "T_Noserve", "T_Notrap", "T_Notrust", "T_Ntp", "T_Ntpport",
  "T_NtpSignDsocket", "T_Orphan", "T_Orphanwait", "T_Panic", "T_Peer",
  "T_Peerstats", "T_Phone", "T_Pid", "T_Pidfile", "T_Pool", "T_Port",
  "T_Preempt", "T_Prefer", "T_Protostats", "T_Pw", "T_Randfile",
  "T_Rawstats", "T_Refid", "T_Requestkey", "T_Reset", "T_Restrict",
  "T_Revoke", "T_Rlimit", "T_Saveconfigdir", "T_Server", "T_Setvar",
  "T_Source", "T_Stacksize", "T_Statistics", "T_Stats", "T_Statsdir",
  "T_Step", "T_Stepback", "T_Stepfwd", "T_Stepout", "T_Stratum",
  "T_String", "T_Sys", "T_Sysstats", "T_Tick", "T_Time1", "T_Time2",
  "T_Timer", "T_Timingstats", "T_Tinker", "T_Tos", "T_Trap", "T_True",
  "T_Trustedkey", "T_Ttl", "T_Type", "T_U_int", "T_Unconfig", "T_Unpeer",
  "T_Version", "T_WanderThreshold", "T_Week", "T_Wildcard", "T_Xleave",
  "T_Year", "T_Flag", "T_EOC", "T_Simulate", "T_Beep_Delay",
  "T_Sim_Duration", "T_Server_Offset", "T_Duration", "T_Freq_Offset",
  "T_Wander", "T_Jitter", "T_Prop_Delay", "T_Proc_Delay", "'='", "'('",
  "')'", "'{'", "'}'", "$accept", "configuration", "command_list",
  "command", "server_command", "client_type", "address", "ip_address",
  "address_fam", "option_list", "option", "option_flag",
  "option_flag_keyword", "option_int", "option_int_keyword", "option_str",
  "option_str_keyword", "unpeer_command", "unpeer_keyword",
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
  "rlimit_command", "rlimit_option_list", "rlimit_option",
  "rlimit_option_keyword", "system_option_command", "system_option_list",
  "system_option", "system_option_flag_keyword",
  "system_option_local_flag_keyword", "tinker_command",
  "tinker_option_list", "tinker_option", "tinker_option_keyword",
  "miscellaneous_command", "misc_cmd_dbl_keyword", "misc_cmd_int_keyword",
  "misc_cmd_str_keyword", "misc_cmd_str_lcl_keyword", "drift_parm",
  "variable_assign", "t_default_or_zero", "trap_option_list",
  "trap_option", "log_config_list", "log_config_command",
  "interface_command", "interface_nic", "nic_rule_class",
  "nic_rule_action", "reset_command", "counter_set_list",
  "counter_set_keyword", "integer_list", "integer_list_range",
  "integer_list_range_elt", "integer_range", "string_list", "address_list",
  "boolean", "number", "simulate_command", "sim_conf_start",
  "sim_init_statement_list", "sim_init_statement", "sim_init_keyword",
  "sim_server_list", "sim_server", "sim_server_offset", "sim_server_name",
  "sim_act_list", "sim_act", "sim_act_stmt_list", "sim_act_stmt",
  "sim_act_keyword", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
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
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,    61,    40,    41,   123,   125
};
# endif

#define YYPACT_NINF -184

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-184)))

#define YYTABLE_NINF -7

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      10,  -176,   -33,  -184,  -184,  -184,   -31,  -184,    45,    -7,
    -123,  -184,    45,  -184,   211,   -52,  -184,  -107,  -184,  -103,
    -101,  -184,   -98,  -184,  -184,   -52,   -20,   130,   -52,  -184,
    -184,   -96,  -184,   -83,  -184,  -184,    -9,    70,    44,    22,
     -21,  -184,  -184,   -71,   211,   -65,  -184,   291,   489,   -60,
     -53,    39,  -184,  -184,  -184,   101,   184,   -79,  -184,   -52,
    -184,   -52,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,   -19,    43,   -50,   -49,  -184,    21,  -184,  -184,
     -87,  -184,  -184,  -184,   196,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,    45,  -184,  -184,  -184,  -184,  -184,  -184,
      -7,  -184,    50,    87,  -184,    45,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,   122,  -184,
     -29,   349,  -184,  -184,  -184,   -98,  -184,  -184,   -52,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,   130,  -184,
      62,   -52,  -184,  -184,   -27,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,    70,  -184,  -184,    97,   105,  -184,  -184,
      48,  -184,  -184,  -184,  -184,   -21,  -184,    83,   -51,  -184,
     211,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,   291,  -184,   -19,  -184,  -184,   -30,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,   489,  -184,    84,
     -19,  -184,  -184,    85,   -53,  -184,  -184,  -184,   110,  -184,
     -24,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,     0,  -105,  -184,  -184,  -184,  -184,  -184,
     111,  -184,     5,  -184,  -184,  -184,  -184,    -2,    16,  -184,
    -184,  -184,  -184,    17,   121,  -184,  -184,   122,  -184,   -19,
     -30,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,   452,
    -184,  -184,   452,   452,   -60,  -184,  -184,    28,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,   -47,   160,
    -184,  -184,  -184,   372,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -116,    15,     9,  -184,  -184,  -184,  -184,    46,
    -184,  -184,    32,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,   452,   452,
    -184,   177,   -60,   144,  -184,   147,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,   -54,  -184,    54,    30,    47,
    -135,  -184,    34,  -184,   -19,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,   452,  -184,  -184,  -184,  -184,    41,
    -184,  -184,  -184,   -52,  -184,  -184,  -184,    59,  -184,  -184,
    -184,    56,    64,   -19,    60,   -99,  -184,    55,   -19,  -184,
    -184,  -184,    58,    81,  -184,  -184,  -184,  -184,  -184,    51,
      75,    65,  -184,    79,  -184,   -19,  -184,  -184
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,    24,    58,   231,     0,    71,     0,     0,
     242,   234,     0,   224,     0,     0,   235,     0,   255,     0,
       0,   236,     0,   238,    25,     0,     0,     0,     0,   256,
     232,     0,    23,     0,   237,    22,     0,     0,     0,     0,
       0,   239,    21,     0,     0,     0,   233,     0,     0,     0,
       0,     0,    56,    57,   291,     0,     2,     0,     7,     0,
       8,     0,     9,    10,    13,    11,    12,    14,    15,    16,
      17,    18,     0,     0,     0,     0,   217,     0,   218,    19,
       0,     5,    62,    63,    64,   195,   196,   197,   198,   201,
     199,   200,   202,   190,   192,   193,   194,   154,   155,   156,
     126,   152,     0,   240,   225,   189,   101,   102,   103,   104,
     108,   105,   106,   107,   109,    29,    30,    28,     0,    26,
       0,     6,    65,    66,   252,   226,   251,   284,    59,    61,
     160,   161,   162,   163,   164,   165,   166,   167,   127,   158,
       0,    60,    70,   282,   227,    67,   267,   268,   269,   270,
     271,   272,   273,   264,   266,   134,    29,    30,   134,   134,
      26,    68,   188,   186,   187,   182,   184,     0,     0,   228,
      96,   100,    97,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   203,   205,     0,    91,    86,     0,    87,
      95,    93,    94,    92,    90,    88,    89,    80,    82,     0,
       0,   246,   278,     0,    69,   277,   279,   275,   230,     1,
       0,     4,    31,    55,   289,   288,   219,   220,   221,   222,
     263,   262,   261,     0,     0,    79,    75,    76,    77,    78,
       0,    72,     0,   191,   151,   153,   241,    98,     0,   178,
     179,   180,   181,     0,     0,   176,   177,   168,   170,     0,
       0,    27,   223,   250,   283,   157,   159,   281,   265,   130,
     134,   134,   133,   128,     0,   183,   185,     0,    99,   204,
     206,   287,   285,   286,    85,    81,    83,    84,   229,     0,
     276,   274,     3,    20,   257,   258,   259,   254,   260,   253,
     295,   296,     0,     0,     0,    74,    73,   118,   117,     0,
     115,   116,     0,   110,   113,   114,   174,   175,   173,   169,
     171,   172,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   135,   131,   132,
     134,   245,     0,     0,   247,     0,    37,    38,    39,    54,
      47,    49,    48,    51,    40,    41,    42,    43,    50,    52,
      44,    32,    33,    36,    34,     0,    35,     0,     0,     0,
       0,   298,     0,   293,     0,   111,   125,   121,   123,   119,
     120,   122,   124,   112,   129,   244,   243,   249,   248,     0,
      45,    46,    53,     0,   292,   290,   297,     0,   294,   280,
     301,     0,     0,     0,     0,     0,   303,     0,     0,   299,
     302,   300,     0,     0,   308,   309,   310,   311,   312,     0,
       0,     0,   304,     0,   306,     0,   305,   307
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -184,  -184,  -184,   -34,  -184,  -184,   -15,   -37,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,    68,  -184,  -184,  -184,
    -184,   -41,  -184,  -184,  -184,  -184,  -184,  -184,  -140,  -184,
    -184,   152,  -184,  -184,   124,  -184,  -184,  -184,    19,  -184,
    -184,  -184,  -184,   109,  -184,  -184,   263,   -56,  -184,  -184,
    -184,  -184,    94,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,   154,  -184,  -184,  -184,  -184,
    -184,  -184,   127,  -184,  -184,    77,  -184,  -184,   254,    35,
    -183,  -184,  -184,  -184,    -6,  -184,  -184,   -73,  -184,  -184,
    -184,  -102,  -184,  -121,  -184
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    55,    56,    57,    58,    59,   127,   119,   120,   283,
     351,   352,   353,   354,   355,   356,   357,    60,    61,    62,
      63,    84,   231,   232,    64,   197,   198,   199,   200,    65,
     170,   114,   237,   303,   304,   305,   373,    66,   259,   327,
     100,   101,   102,   138,   139,   140,    67,   247,   248,   249,
     250,    68,   165,   166,   167,    69,    93,    94,    95,    96,
      70,   183,   184,   185,    71,    72,    73,    74,    75,   104,
     169,   376,   278,   334,   125,   126,    76,    77,   289,   223,
      78,   153,   154,   208,   204,   205,   206,   144,   128,   274,
     216,    79,    80,   292,   293,   294,   360,   361,   392,   362,
     395,   396,   409,   410,   411
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     118,   160,   270,   171,    97,   284,    81,   380,   202,   271,
     358,     1,   201,   214,   115,   332,   116,   277,   262,   263,
       2,   162,   210,   159,     3,     4,     5,   297,    82,   358,
      83,   272,     6,     7,   103,   298,   366,   233,   299,     8,
       9,   129,   215,    10,   212,    11,   213,    12,    13,   233,
     121,    14,   145,    85,   122,   220,   123,    86,   367,   124,
      15,   142,   385,    87,    16,   285,   310,   286,   290,   291,
      17,   155,    18,   163,   143,   300,   221,   146,   147,   290,
     291,    19,    20,   161,   333,    21,   168,   252,   394,    22,
      23,    98,   172,    24,    25,   148,    99,   117,   399,   222,
     207,   209,    26,   211,   217,   117,   301,   218,   219,   224,
     156,   235,   157,   254,    88,    27,    28,    29,   381,   236,
     328,   329,    30,   256,   260,   238,   254,   164,   251,   268,
     257,    31,   261,   264,   149,    32,   368,    33,   273,    34,
      35,   203,   267,   369,   266,   276,   279,    89,    90,    36,
      37,    38,    39,    40,    41,    42,    43,   287,   282,    44,
     370,    45,   296,   150,    91,   239,   240,   241,   242,   302,
      46,   281,   295,   306,   307,    47,    48,    49,   288,    50,
      51,   388,   308,    52,    53,   331,   130,   131,   132,   133,
     374,   158,    -6,    54,     2,    92,   335,   363,     3,     4,
       5,   117,   364,   365,   375,   378,     6,     7,   379,   371,
     397,   382,   372,     8,     9,   402,   134,    10,   135,    11,
     136,    12,    13,   383,   225,    14,   137,   330,   151,   384,
     387,   106,   417,   152,    15,   107,   389,   401,    16,   404,
     405,   406,   407,   408,    17,   391,    18,   226,   412,   393,
     227,   394,   234,   398,   403,    19,    20,   414,   415,    21,
     243,   416,   255,    22,    23,   275,   309,    24,    25,   404,
     405,   406,   407,   408,   265,   105,    26,   269,   244,   253,
     258,   280,   141,   245,   246,   311,   359,   386,   413,    27,
      28,    29,   108,   400,     0,   377,    30,   173,     0,     0,
       0,     0,     0,     0,     0,    31,     0,     0,     0,    32,
       0,    33,     0,    34,    35,     0,     0,     0,     0,     0,
       0,     0,   174,    36,    37,    38,    39,    40,    41,    42,
      43,   228,   229,    44,     0,    45,     0,   109,   230,     0,
     175,     0,     0,   176,    46,   110,     0,     0,   111,    47,
      48,    49,     0,    50,    51,     0,     0,    52,    53,     2,
       0,     0,     0,     3,     4,     5,    -6,    54,   390,     0,
     112,     6,     7,     0,     0,   113,     0,     0,     8,     9,
       0,   336,    10,     0,    11,     0,    12,    13,     0,   337,
      14,     0,     0,     0,     0,     0,     0,     0,     0,    15,
       0,     0,     0,    16,     0,     0,     0,     0,     0,    17,
       0,    18,     0,     0,     0,   177,     0,     0,     0,     0,
      19,    20,     0,     0,    21,   338,   339,     0,    22,    23,
       0,     0,    24,    25,     0,     0,     0,     0,     0,     0,
       0,    26,   340,   178,   179,   180,   181,     0,     0,     0,
       0,   182,     0,     0,    27,    28,    29,     0,     0,     0,
       0,    30,     0,   341,     0,     0,     0,     0,     0,     0,
      31,   342,     0,   343,    32,     0,    33,     0,    34,    35,
       0,     0,     0,     0,     0,     0,     0,   344,    36,    37,
      38,    39,    40,    41,    42,    43,     0,     0,    44,   312,
      45,     0,   186,     0,   345,   346,     0,   313,   187,    46,
     188,     0,     0,     0,    47,    48,    49,     0,    50,    51,
       0,     0,    52,    53,     0,   314,   315,     0,   316,     0,
       0,     0,    54,     0,   317,     0,     0,   189,     0,     0,
     347,     0,   348,     0,     0,     0,     0,   349,     0,     0,
       0,   350,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   318,   319,     0,     0,   320,   321,     0,   322,   323,
     324,     0,   325,     0,     0,     0,   190,     0,   191,     0,
       0,     0,     0,     0,   192,     0,   193,     0,     0,   194,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   195,   196,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   326
};

static const yytype_int16 yycheck[] =
{
      15,    38,   185,    44,    11,     5,   182,    61,    61,    39,
     145,     1,    49,    32,    66,    62,    68,   200,   158,   159,
      10,    42,    56,    38,    14,    15,    16,    29,    61,   145,
      61,    61,    22,    23,   157,    37,     4,    93,    40,    29,
      30,    61,    61,    33,    59,    35,    61,    37,    38,   105,
     157,    41,    61,     8,   157,    34,   157,    12,    26,   157,
      50,   157,   197,    18,    54,    65,   249,    67,   184,   185,
      60,    27,    62,    94,   157,    77,    55,     7,     8,   184,
     185,    71,    72,    61,   131,    75,   157,   121,   187,    79,
      80,    98,   157,    83,    84,    25,   103,   157,   197,    78,
      61,     0,    92,   182,    61,   157,   108,   157,   157,   196,
      66,    61,    68,   128,    69,   105,   106,   107,   172,    32,
     260,   261,   112,    61,    27,     3,   141,   148,   157,   170,
     157,   121,    27,    85,    64,   125,   104,   127,   168,   129,
     130,   194,   193,   111,    61,    61,    61,   102,   103,   139,
     140,   141,   142,   143,   144,   145,   146,   157,   182,   149,
     128,   151,   157,    93,   119,    43,    44,    45,    46,   171,
     160,    61,    61,   157,   157,   165,   166,   167,   178,   169,
     170,   364,    61,   173,   174,   157,    56,    57,    58,    59,
     330,   147,   182,   183,    10,   150,    36,   182,    14,    15,
      16,   157,   193,   157,    27,    61,    22,    23,    61,   177,
     393,   157,   180,    29,    30,   398,    86,    33,    88,    35,
      90,    37,    38,   193,    28,    41,    96,   264,   158,   182,
     196,    20,   415,   163,    50,    24,   195,   182,    54,   188,
     189,   190,   191,   192,    60,   186,    62,    51,   197,   193,
      54,   187,   100,   193,   196,    71,    72,   182,   193,    75,
     138,   182,   138,    79,    80,   197,   247,    83,    84,   188,
     189,   190,   191,   192,   165,    12,    92,   183,   156,   125,
     153,   204,    28,   161,   162,   250,   292,   360,   409,   105,
     106,   107,    81,   395,    -1,   332,   112,     6,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,   125,
      -1,   127,    -1,   129,   130,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,   139,   140,   141,   142,   143,   144,   145,
     146,   135,   136,   149,    -1,   151,    -1,   126,   142,    -1,
      49,    -1,    -1,    52,   160,   134,    -1,    -1,   137,   165,
     166,   167,    -1,   169,   170,    -1,    -1,   173,   174,    10,
      -1,    -1,    -1,    14,    15,    16,   182,   183,   383,    -1,
     159,    22,    23,    -1,    -1,   164,    -1,    -1,    29,    30,
      -1,     9,    33,    -1,    35,    -1,    37,    38,    -1,    17,
      41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,
      -1,    -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,    60,
      -1,    62,    -1,    -1,    -1,   124,    -1,    -1,    -1,    -1,
      71,    72,    -1,    -1,    75,    53,    54,    -1,    79,    80,
      -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    92,    70,   152,   153,   154,   155,    -1,    -1,    -1,
      -1,   160,    -1,    -1,   105,   106,   107,    -1,    -1,    -1,
      -1,   112,    -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,
     121,    99,    -1,   101,   125,    -1,   127,    -1,   129,   130,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   115,   139,   140,
     141,   142,   143,   144,   145,   146,    -1,    -1,   149,    47,
     151,    -1,    13,    -1,   132,   133,    -1,    55,    19,   160,
      21,    -1,    -1,    -1,   165,   166,   167,    -1,   169,   170,
      -1,    -1,   173,   174,    -1,    73,    74,    -1,    76,    -1,
      -1,    -1,   183,    -1,    82,    -1,    -1,    48,    -1,    -1,
     168,    -1,   170,    -1,    -1,    -1,    -1,   175,    -1,    -1,
      -1,   179,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   109,   110,    -1,    -1,   113,   114,    -1,   116,   117,
     118,    -1,   120,    -1,    -1,    -1,    87,    -1,    89,    -1,
      -1,    -1,    -1,    -1,    95,    -1,    97,    -1,    -1,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   122,   123,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   175
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     1,    10,    14,    15,    16,    22,    23,    29,    30,
      33,    35,    37,    38,    41,    50,    54,    60,    62,    71,
      72,    75,    79,    80,    83,    84,    92,   105,   106,   107,
     112,   121,   125,   127,   129,   130,   139,   140,   141,   142,
     143,   144,   145,   146,   149,   151,   160,   165,   166,   167,
     169,   170,   173,   174,   183,   199,   200,   201,   202,   203,
     215,   216,   217,   218,   222,   227,   235,   244,   249,   253,
     258,   262,   263,   264,   265,   266,   274,   275,   278,   289,
     290,   182,    61,    61,   219,     8,    12,    18,    69,   102,
     103,   119,   150,   254,   255,   256,   257,    11,    98,   103,
     238,   239,   240,   157,   267,   254,    20,    24,    81,   126,
     134,   137,   159,   164,   229,    66,    68,   157,   204,   205,
     206,   157,   157,   157,   157,   272,   273,   204,   286,    61,
      56,    57,    58,    59,    86,    88,    90,    96,   241,   242,
     243,   286,   157,   157,   285,    61,     7,     8,    25,    64,
      93,   158,   163,   279,   280,    27,    66,    68,   147,   204,
     205,    61,    42,    94,   148,   250,   251,   252,   157,   268,
     228,   229,   157,     6,    31,    49,    52,   124,   152,   153,
     154,   155,   160,   259,   260,   261,    13,    19,    21,    48,
      87,    89,    95,    97,   100,   122,   123,   223,   224,   225,
     226,   205,    61,   194,   282,   283,   284,    61,   281,     0,
     201,   182,   204,   204,    32,    61,   288,    61,   157,   157,
      34,    55,    78,   277,   196,    28,    51,    54,   135,   136,
     142,   220,   221,   255,   239,    61,    32,   230,     3,    43,
      44,    45,    46,   138,   156,   161,   162,   245,   246,   247,
     248,   157,   201,   273,   204,   242,    61,   157,   280,   236,
      27,    27,   236,   236,    85,   251,    61,   193,   229,   260,
     288,    39,    61,   168,   287,   224,    61,   288,   270,    61,
     283,    61,   182,   207,     5,    65,    67,   157,   178,   276,
     184,   185,   291,   292,   293,    61,   157,    29,    37,    40,
      77,   108,   171,   231,   232,   233,   157,   157,    61,   246,
     288,   287,    47,    55,    73,    74,    76,    82,   109,   110,
     113,   114,   116,   117,   118,   120,   175,   237,   236,   236,
     205,   157,    62,   131,   271,    36,     9,    17,    53,    54,
      70,    91,    99,   101,   115,   132,   133,   168,   170,   175,
     179,   208,   209,   210,   211,   212,   213,   214,   145,   292,
     294,   295,   297,   182,   193,   157,     4,    26,   104,   111,
     128,   177,   180,   234,   236,    27,   269,   205,    61,    61,
      61,   172,   157,   193,   182,   197,   295,   196,   288,   195,
     204,   186,   296,   193,   187,   298,   299,   288,   193,   197,
     299,   182,   288,   196,   188,   189,   190,   191,   192,   300,
     301,   302,   197,   301,   182,   193,   182,   288
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   198,   199,   200,   200,   200,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     202,   203,   203,   203,   203,   203,   204,   204,   205,   206,
     206,   207,   207,   208,   208,   208,   209,   210,   210,   210,
     210,   210,   210,   210,   210,   211,   211,   212,   212,   212,
     212,   212,   212,   213,   214,   215,   216,   216,   217,   217,
     217,   217,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   219,   219,   220,   220,   221,   221,   221,   221,   221,
     222,   223,   223,   224,   224,   224,   225,   225,   225,   225,
     225,   225,   226,   226,   226,   226,   227,   227,   227,   228,
     228,   229,   229,   229,   229,   229,   229,   229,   229,   230,
     230,   231,   231,   231,   231,   232,   232,   233,   233,   234,
     234,   234,   234,   234,   234,   234,   235,   235,   235,   235,
     235,   235,   235,   235,   236,   236,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   238,   238,   239,   240,   240,   240,   241,   241,   242,
     243,   243,   243,   243,   243,   243,   243,   243,   244,   245,
     245,   246,   246,   246,   246,   246,   247,   247,   248,   248,
     248,   248,   249,   250,   250,   251,   252,   252,   252,   253,
     253,   254,   254,   255,   255,   256,   256,   256,   256,   256,
     256,   257,   257,   258,   259,   259,   260,   261,   261,   261,
     261,   261,   261,   261,   261,   261,   261,   262,   262,   262,
     262,   262,   262,   262,   262,   262,   262,   262,   262,   262,
     262,   263,   263,   263,   264,   265,   265,   265,   266,   266,
     267,   267,   267,   268,   269,   269,   270,   270,   271,   271,
     272,   272,   273,   274,   274,   275,   275,   276,   276,   276,
     276,   277,   277,   277,   278,   279,   279,   280,   280,   280,
     280,   280,   280,   280,   281,   281,   282,   282,   283,   283,
     284,   285,   285,   286,   286,   287,   287,   287,   288,   288,
     289,   290,   291,   291,   292,   293,   293,   294,   294,   295,
     296,   297,   298,   298,   299,   300,   300,   301,   302,   302,
     302,   302,   302
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     0,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     1,     1,     1,
       1,     1,     1,     2,     1,     2,     1,     1,     1,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     0,     2,     2,     2,     1,     1,     1,     1,     1,
       2,     2,     1,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     3,     5,
       3,     4,     4,     3,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     2,     1,     1,     1,     2,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       1,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     2,     2,     1,     2,     1,     1,     1,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     2,     2,     3,     1,     2,     2,     2,     2,     3,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     0,     4,     1,     0,     0,     2,     2,     2,
       2,     1,     1,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     2,     1,     1,     1,
       5,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       5,     1,     3,     2,     3,     1,     1,     2,     1,     5,
       4,     3,     2,     1,     6,     3,     2,     3,     1,     1,
       1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

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
#line 372 "ntp_parser.y" /* yacc.c:1646  */
    {
			/* I will need to incorporate much more fine grained
			 * error messages. The following should suffice for
			 * the time being.
			 */
			struct FILE_INFO * ip_ctx = lex_current();
			msyslog(LOG_ERR, 
				"syntax error in %s line %d, column %d",
				ip_ctx->fname,
				ip_ctx->errpos.nline,
				ip_ctx->errpos.ncol);
		}
#line 2100 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 20:
#line 408 "ntp_parser.y" /* yacc.c:1646  */
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[-2].Integer), (yyvsp[-1].Address_node), (yyvsp[0].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.peers, my_node);
		}
#line 2111 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 27:
#line 427 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Address_node) = create_address_node((yyvsp[0].String), (yyvsp[-1].Integer)); }
#line 2117 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 28:
#line 432 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Address_node) = create_address_node((yyvsp[0].String), AF_UNSPEC); }
#line 2123 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 29:
#line 437 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Integer) = AF_INET; }
#line 2129 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 30:
#line 439 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Integer) = AF_INET6; }
#line 2135 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 31:
#line 444 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val_fifo) = NULL; }
#line 2141 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 32:
#line 446 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2150 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 36:
#line 460 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[0].Integer)); }
#line 2156 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 45:
#line 476 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_ival((yyvsp[-1].Integer), (yyvsp[0].Integer)); }
#line 2162 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 46:
#line 478 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_uval((yyvsp[-1].Integer), (yyvsp[0].Integer)); }
#line 2168 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 53:
#line 492 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_sval((yyvsp[-1].Integer), (yyvsp[0].String)); }
#line 2174 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 55:
#line 506 "ntp_parser.y" /* yacc.c:1646  */
    {
			unpeer_node *my_node;
			
			my_node = create_unpeer_node((yyvsp[0].Address_node));
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
		}
#line 2186 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 58:
#line 527 "ntp_parser.y" /* yacc.c:1646  */
    { cfgt.broadcastclient = 1; }
#line 2192 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 59:
#line 529 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.manycastserver, (yyvsp[0].Address_fifo)); }
#line 2198 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 60:
#line 531 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.multicastclient, (yyvsp[0].Address_fifo)); }
#line 2204 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 61:
#line 533 "ntp_parser.y" /* yacc.c:1646  */
    { cfgt.mdnstries = (yyvsp[0].Integer); }
#line 2210 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 62:
#line 544 "ntp_parser.y" /* yacc.c:1646  */
    {
			attr_val *atrv;
			
			atrv = create_attr_ival((yyvsp[-1].Integer), (yyvsp[0].Integer));
			APPEND_G_FIFO(cfgt.vars, atrv);
		}
#line 2221 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 63:
#line 551 "ntp_parser.y" /* yacc.c:1646  */
    { cfgt.auth.control_key = (yyvsp[0].Integer); }
#line 2227 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 64:
#line 553 "ntp_parser.y" /* yacc.c:1646  */
    { 
			cfgt.auth.cryptosw++;
			CONCAT_G_FIFOS(cfgt.auth.crypto_cmd_list, (yyvsp[0].Attr_val_fifo));
		}
#line 2236 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 65:
#line 558 "ntp_parser.y" /* yacc.c:1646  */
    { cfgt.auth.keys = (yyvsp[0].String); }
#line 2242 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 66:
#line 560 "ntp_parser.y" /* yacc.c:1646  */
    { cfgt.auth.keysdir = (yyvsp[0].String); }
#line 2248 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 67:
#line 562 "ntp_parser.y" /* yacc.c:1646  */
    { cfgt.auth.request_key = (yyvsp[0].Integer); }
#line 2254 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 68:
#line 564 "ntp_parser.y" /* yacc.c:1646  */
    { cfgt.auth.revoke = (yyvsp[0].Integer); }
#line 2260 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 69:
#line 566 "ntp_parser.y" /* yacc.c:1646  */
    {
			cfgt.auth.trusted_key_list = (yyvsp[0].Attr_val_fifo);

			// if (!cfgt.auth.trusted_key_list)
			// 	cfgt.auth.trusted_key_list = $2;
			// else
			// 	LINK_SLIST(cfgt.auth.trusted_key_list, $2, link);
		}
#line 2273 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 70:
#line 575 "ntp_parser.y" /* yacc.c:1646  */
    { cfgt.auth.ntp_signd_socket = (yyvsp[0].String); }
#line 2279 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 71:
#line 580 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val_fifo) = NULL; }
#line 2285 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 72:
#line 582 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2294 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 73:
#line 590 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_sval((yyvsp[-1].Integer), (yyvsp[0].String)); }
#line 2300 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 74:
#line 592 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val) = NULL;
			cfgt.auth.revoke = (yyvsp[0].Integer);
			msyslog(LOG_WARNING,
				"'crypto revoke %d' is deprecated, "
				"please use 'revoke %d' instead.",
				cfgt.auth.revoke, cfgt.auth.revoke);
		}
#line 2313 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 80:
#line 617 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.orphan_cmds, (yyvsp[0].Attr_val_fifo)); }
#line 2319 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 81:
#line 622 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2328 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 82:
#line 627 "ntp_parser.y" /* yacc.c:1646  */
    {	
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2337 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 83:
#line 635 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_dval((yyvsp[-1].Integer), (double)(yyvsp[0].Integer)); }
#line 2343 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 84:
#line 637 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_dval((yyvsp[-1].Integer), (yyvsp[0].Double)); }
#line 2349 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 85:
#line 639 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_dval((yyvsp[-1].Integer), (double)(yyvsp[0].Integer)); }
#line 2355 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 96:
#line 665 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.stats_list, (yyvsp[0].Int_fifo)); }
#line 2361 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 97:
#line 667 "ntp_parser.y" /* yacc.c:1646  */
    {
			if (lex_from_file()) {
				cfgt.stats_dir = (yyvsp[0].String);
			} else {
				YYFREE((yyvsp[0].String));
				yyerror("statsdir remote configuration ignored");
			}
		}
#line 2374 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 98:
#line 676 "ntp_parser.y" /* yacc.c:1646  */
    {
			filegen_node *fgn;
			
			fgn = create_filegen_node((yyvsp[-1].Integer), (yyvsp[0].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.filegen_opts, fgn);
		}
#line 2385 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 99:
#line 686 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Int_fifo) = (yyvsp[-1].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[0].Integer)));
		}
#line 2394 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 100:
#line 691 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[0].Integer)));
		}
#line 2403 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 109:
#line 710 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val_fifo) = NULL; }
#line 2409 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 110:
#line 712 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2418 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 111:
#line 720 "ntp_parser.y" /* yacc.c:1646  */
    {
			if (lex_from_file()) {
				(yyval.Attr_val) = create_attr_sval((yyvsp[-1].Integer), (yyvsp[0].String));
			} else {
				(yyval.Attr_val) = NULL;
				YYFREE((yyvsp[0].String));
				yyerror("filegen file remote config ignored");
			}
		}
#line 2432 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 112:
#line 730 "ntp_parser.y" /* yacc.c:1646  */
    {
			if (lex_from_file()) {
				(yyval.Attr_val) = create_attr_ival((yyvsp[-1].Integer), (yyvsp[0].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen type remote config ignored");
			}
		}
#line 2445 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 113:
#line 739 "ntp_parser.y" /* yacc.c:1646  */
    {
			const char *err;
			
			if (lex_from_file()) {
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[0].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				if (T_Link == (yyvsp[0].Integer))
					err = "filegen link remote config ignored";
				else
					err = "filegen nolink remote config ignored";
				yyerror(err);
			}
		}
#line 2464 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 114:
#line 754 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[0].Integer)); }
#line 2470 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 126:
#line 784 "ntp_parser.y" /* yacc.c:1646  */
    {
			CONCAT_G_FIFOS(cfgt.discard_opts, (yyvsp[0].Attr_val_fifo));
		}
#line 2478 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 127:
#line 788 "ntp_parser.y" /* yacc.c:1646  */
    {
			CONCAT_G_FIFOS(cfgt.mru_opts, (yyvsp[0].Attr_val_fifo));
		}
#line 2486 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 128:
#line 792 "ntp_parser.y" /* yacc.c:1646  */
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[-1].Address_node), NULL, (yyvsp[0].Int_fifo),
						  lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
#line 2498 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 129:
#line 800 "ntp_parser.y" /* yacc.c:1646  */
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[-3].Address_node), (yyvsp[-1].Address_node), (yyvsp[0].Int_fifo),
						  lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
#line 2510 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 130:
#line 808 "ntp_parser.y" /* yacc.c:1646  */
    {
			restrict_node *rn;

			rn = create_restrict_node(NULL, NULL, (yyvsp[0].Int_fifo),
						  lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
#line 2522 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 131:
#line 816 "ntp_parser.y" /* yacc.c:1646  */
    {
			restrict_node *rn;

			rn = create_restrict_node(
				create_address_node(
					estrdup("0.0.0.0"), 
					AF_INET),
				create_address_node(
					estrdup("0.0.0.0"), 
					AF_INET),
				(yyvsp[0].Int_fifo), 
				lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
#line 2541 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 132:
#line 831 "ntp_parser.y" /* yacc.c:1646  */
    {
			restrict_node *rn;
			
			rn = create_restrict_node(
				create_address_node(
					estrdup("::"), 
					AF_INET6),
				create_address_node(
					estrdup("::"), 
					AF_INET6),
				(yyvsp[0].Int_fifo), 
				lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
#line 2560 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 133:
#line 846 "ntp_parser.y" /* yacc.c:1646  */
    {
			restrict_node *	rn;

			APPEND_G_FIFO((yyvsp[0].Int_fifo), create_int_node((yyvsp[-1].Integer)));
			rn = create_restrict_node(
				NULL, NULL, (yyvsp[0].Int_fifo), lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
#line 2573 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 134:
#line 858 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Int_fifo) = NULL; }
#line 2579 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 135:
#line 860 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Int_fifo) = (yyvsp[-1].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[0].Integer)));
		}
#line 2588 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 151:
#line 886 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2597 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 152:
#line 891 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2606 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 153:
#line 899 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_ival((yyvsp[-1].Integer), (yyvsp[0].Integer)); }
#line 2612 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 157:
#line 910 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2621 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 158:
#line 915 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2630 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 159:
#line 923 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_ival((yyvsp[-1].Integer), (yyvsp[0].Integer)); }
#line 2636 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 168:
#line 943 "ntp_parser.y" /* yacc.c:1646  */
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[-1].Address_node), (yyvsp[0].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.fudge, aon);
		}
#line 2647 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 169:
#line 953 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2656 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 170:
#line 958 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2665 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 171:
#line 966 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_dval((yyvsp[-1].Integer), (yyvsp[0].Double)); }
#line 2671 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 172:
#line 968 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_ival((yyvsp[-1].Integer), (yyvsp[0].Integer)); }
#line 2677 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 173:
#line 970 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_ival((yyvsp[-1].Integer), (yyvsp[0].Integer)); }
#line 2683 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 174:
#line 972 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_sval((yyvsp[-1].Integer), (yyvsp[0].String)); }
#line 2689 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 175:
#line 974 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_sval((yyvsp[-1].Integer), (yyvsp[0].String)); }
#line 2695 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 182:
#line 995 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.rlimit, (yyvsp[0].Attr_val_fifo)); }
#line 2701 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 183:
#line 1000 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2710 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 184:
#line 1005 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2719 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 185:
#line 1013 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_ival((yyvsp[-1].Integer), (yyvsp[0].Integer)); }
#line 2725 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 189:
#line 1029 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.enable_opts, (yyvsp[0].Attr_val_fifo)); }
#line 2731 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 190:
#line 1031 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.disable_opts, (yyvsp[0].Attr_val_fifo)); }
#line 2737 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 191:
#line 1036 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2746 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 192:
#line 1041 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2755 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 193:
#line 1049 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[0].Integer)); }
#line 2761 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 194:
#line 1051 "ntp_parser.y" /* yacc.c:1646  */
    { 
			if (lex_from_file()) {
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[0].Integer));
			} else {
				char err_str[128];
				
				(yyval.Attr_val) = NULL;
				snprintf(err_str, sizeof(err_str),
					 "enable/disable %s remote configuration ignored",
					 keyword((yyvsp[0].Integer)));
				yyerror(err_str);
			}
		}
#line 2779 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 203:
#line 1086 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.tinker, (yyvsp[0].Attr_val_fifo)); }
#line 2785 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 204:
#line 1091 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2794 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 205:
#line 1096 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2803 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 206:
#line 1104 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_dval((yyvsp[-1].Integer), (yyvsp[0].Double)); }
#line 2809 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 219:
#line 1129 "ntp_parser.y" /* yacc.c:1646  */
    {
			attr_val *av;
			
			av = create_attr_dval((yyvsp[-1].Integer), (yyvsp[0].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
#line 2820 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 220:
#line 1136 "ntp_parser.y" /* yacc.c:1646  */
    {
			attr_val *av;
			
			av = create_attr_ival((yyvsp[-1].Integer), (yyvsp[0].Integer));
			APPEND_G_FIFO(cfgt.vars, av);
		}
#line 2831 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 221:
#line 1143 "ntp_parser.y" /* yacc.c:1646  */
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[-1].Integer), (yyvsp[0].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
#line 2842 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 222:
#line 1150 "ntp_parser.y" /* yacc.c:1646  */
    {
			char error_text[64];
			attr_val *av;

			if (lex_from_file()) {
				av = create_attr_sval((yyvsp[-1].Integer), (yyvsp[0].String));
				APPEND_G_FIFO(cfgt.vars, av);
			} else {
				YYFREE((yyvsp[0].String));
				snprintf(error_text, sizeof(error_text),
					 "%s remote config ignored",
					 keyword((yyvsp[-1].Integer)));
				yyerror(error_text);
			}
		}
#line 2862 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 223:
#line 1166 "ntp_parser.y" /* yacc.c:1646  */
    {
			if (!lex_from_file()) {
				YYFREE((yyvsp[-1].String)); /* avoid leak */
				yyerror("remote includefile ignored");
				break;
			}
			if (lex_level() > MAXINCLUDELEVEL) {
				fprintf(stderr, "getconfig: Maximum include file level exceeded.\n");
				msyslog(LOG_ERR, "getconfig: Maximum include file level exceeded.");
			} else {
				const char * path = FindConfig((yyvsp[-1].String)); /* might return $2! */
				if (!lex_push_file(path, "r")) {
					fprintf(stderr, "getconfig: Couldn't open <%s>\n", path);
					msyslog(LOG_ERR, "getconfig: Couldn't open <%s>", path);
				}
			}
			YYFREE((yyvsp[-1].String)); /* avoid leak */
		}
#line 2885 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 224:
#line 1185 "ntp_parser.y" /* yacc.c:1646  */
    { lex_flush_stack(); }
#line 2891 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 225:
#line 1187 "ntp_parser.y" /* yacc.c:1646  */
    { /* see drift_parm below for actions */ }
#line 2897 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 226:
#line 1189 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.logconfig, (yyvsp[0].Attr_val_fifo)); }
#line 2903 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 227:
#line 1191 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.phone, (yyvsp[0].String_fifo)); }
#line 2909 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 228:
#line 1193 "ntp_parser.y" /* yacc.c:1646  */
    { APPEND_G_FIFO(cfgt.setvar, (yyvsp[0].Set_var)); }
#line 2915 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 229:
#line 1195 "ntp_parser.y" /* yacc.c:1646  */
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[-1].Address_node), (yyvsp[0].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.trap, aon);
		}
#line 2926 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 230:
#line 1202 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.ttl, (yyvsp[0].Attr_val_fifo)); }
#line 2932 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 240:
#line 1228 "ntp_parser.y" /* yacc.c:1646  */
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[0].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
#line 2943 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1235 "ntp_parser.y" /* yacc.c:1646  */
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[-1].String));
			APPEND_G_FIFO(cfgt.vars, av);
			av = create_attr_dval(T_WanderThreshold, (yyvsp[0].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
#line 2956 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 242:
#line 1244 "ntp_parser.y" /* yacc.c:1646  */
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, "");
			APPEND_G_FIFO(cfgt.vars, av);
		}
#line 2967 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 243:
#line 1254 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Set_var) = create_setvar_node((yyvsp[-3].String), (yyvsp[-1].String), (yyvsp[0].Integer)); }
#line 2973 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 245:
#line 1260 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Integer) = 0; }
#line 2979 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 246:
#line 1265 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val_fifo) = NULL; }
#line 2985 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 247:
#line 1267 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 2994 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 248:
#line 1275 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_ival((yyvsp[-1].Integer), (yyvsp[0].Integer)); }
#line 3000 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 249:
#line 1277 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val) = create_attr_sval((yyvsp[-1].Integer), estrdup((yyvsp[0].Address_node)->address));
			destroy_address_node((yyvsp[0].Address_node));
		}
#line 3009 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 250:
#line 1285 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 3018 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 251:
#line 1290 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 3027 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 252:
#line 1298 "ntp_parser.y" /* yacc.c:1646  */
    {
			char	prefix;
			char *	type;
			
			switch ((yyvsp[0].String)[0]) {
			
			case '+':
			case '-':
			case '=':
				prefix = (yyvsp[0].String)[0];
				type = (yyvsp[0].String) + 1;
				break;
				
			default:
				prefix = '=';
				type = (yyvsp[0].String);
			}	
			
			(yyval.Attr_val) = create_attr_sval(prefix, estrdup(type));
			YYFREE((yyvsp[0].String));
		}
#line 3053 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 253:
#line 1323 "ntp_parser.y" /* yacc.c:1646  */
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node((yyvsp[0].Integer), NULL, (yyvsp[-1].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
#line 3064 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 254:
#line 1330 "ntp_parser.y" /* yacc.c:1646  */
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node(0, (yyvsp[0].String), (yyvsp[-1].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
#line 3075 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 264:
#line 1358 "ntp_parser.y" /* yacc.c:1646  */
    { CONCAT_G_FIFOS(cfgt.reset_counters, (yyvsp[0].Int_fifo)); }
#line 3081 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 265:
#line 1363 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Int_fifo) = (yyvsp[-1].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[0].Integer)));
		}
#line 3090 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 266:
#line 1368 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[0].Integer)));
		}
#line 3099 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 274:
#line 1392 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[0].Integer)));
		}
#line 3108 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 275:
#line 1397 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[0].Integer)));
		}
#line 3117 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 276:
#line 1405 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-1].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 3126 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 277:
#line 1410 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[0].Attr_val));
		}
#line 3135 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 278:
#line 1418 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[0].Integer)); }
#line 3141 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 280:
#line 1424 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_rangeval('-', (yyvsp[-3].Integer), (yyvsp[-1].Integer)); }
#line 3147 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 281:
#line 1429 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.String_fifo) = (yyvsp[-1].String_fifo);
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[0].String)));
		}
#line 3156 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 282:
#line 1434 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.String_fifo) = NULL;
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[0].String)));
		}
#line 3165 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 283:
#line 1442 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Address_fifo) = (yyvsp[-1].Address_fifo);
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[0].Address_node));
		}
#line 3174 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 284:
#line 1447 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Address_fifo) = NULL;
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[0].Address_node));
		}
#line 3183 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 285:
#line 1455 "ntp_parser.y" /* yacc.c:1646  */
    {
			if ((yyvsp[0].Integer) != 0 && (yyvsp[0].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			} else {
				(yyval.Integer) = (yyvsp[0].Integer);
			}
		}
#line 3196 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 286:
#line 1463 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Integer) = 1; }
#line 3202 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 287:
#line 1464 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Integer) = 0; }
#line 3208 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 288:
#line 1468 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Double) = (double)(yyvsp[0].Integer); }
#line 3214 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 290:
#line 1479 "ntp_parser.y" /* yacc.c:1646  */
    {
			sim_node *sn;
			
			sn =  create_sim_node((yyvsp[-2].Attr_val_fifo), (yyvsp[-1].Sim_server_fifo));
			APPEND_G_FIFO(cfgt.sim_details, sn);

			/* Revert from ; to \n for end-of-command */
			old_config_style = 1;
		}
#line 3228 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 291:
#line 1496 "ntp_parser.y" /* yacc.c:1646  */
    { old_config_style = 0; }
#line 3234 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 292:
#line 1501 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-2].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[-1].Attr_val));
		}
#line 3243 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 293:
#line 1506 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[-1].Attr_val));
		}
#line 3252 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 294:
#line 1514 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_dval((yyvsp[-2].Integer), (yyvsp[0].Double)); }
#line 3258 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 297:
#line 1524 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Sim_server_fifo) = (yyvsp[-1].Sim_server_fifo);
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[0].Sim_server));
		}
#line 3267 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 298:
#line 1529 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Sim_server_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[0].Sim_server));
		}
#line 3276 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 299:
#line 1537 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Sim_server) = ONLY_SIM(create_sim_server((yyvsp[-4].Address_node), (yyvsp[-2].Double), (yyvsp[-1].Sim_script_fifo))); }
#line 3282 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 300:
#line 1542 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Double) = (yyvsp[-1].Double); }
#line 3288 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 301:
#line 1547 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Address_node) = (yyvsp[0].Address_node); }
#line 3294 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 302:
#line 1552 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Sim_script_fifo) = (yyvsp[-1].Sim_script_fifo);
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[0].Sim_script));
		}
#line 3303 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 303:
#line 1557 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Sim_script_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[0].Sim_script));
		}
#line 3312 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 304:
#line 1565 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Sim_script) = ONLY_SIM(create_sim_script_info((yyvsp[-3].Double), (yyvsp[-1].Attr_val_fifo))); }
#line 3318 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 305:
#line 1570 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = (yyvsp[-2].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[-1].Attr_val));
		}
#line 3327 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 306:
#line 1575 "ntp_parser.y" /* yacc.c:1646  */
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[-1].Attr_val));
		}
#line 3336 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;

  case 307:
#line 1583 "ntp_parser.y" /* yacc.c:1646  */
    { (yyval.Attr_val) = create_attr_dval((yyvsp[-2].Integer), (yyvsp[0].Double)); }
#line 3342 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
    break;


#line 3346 "../../ntpd/ntp_parser.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 1594 "ntp_parser.y" /* yacc.c:1906  */


void 
yyerror(
	const char *msg
	)
{
	int retval;
	struct FILE_INFO * ip_ctx;

	ip_ctx = lex_current();
	ip_ctx->errpos = ip_ctx->tokpos;
	
	msyslog(LOG_ERR, "line %d column %d %s", 
		ip_ctx->errpos.nline, ip_ctx->errpos.ncol, msg);
	if (!lex_from_file()) {
		/* Save the error message in the correct buffer */
		retval = snprintf(remote_config.err_msg + remote_config.err_pos,
				  MAXLINE - remote_config.err_pos,
				  "column %d %s",
				  ip_ctx->errpos.ncol, msg);

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

