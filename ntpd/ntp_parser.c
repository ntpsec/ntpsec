/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.3"

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


  struct FILE_INFO *ip_file;	/* configuration file stream */

  #define YYMALLOC	emalloc
  #define YYFREE	free
  #define YYERROR_VERBOSE
  #define YYMAXDEPTH	1000	/* stop the madness sooner */
  void yyerror(const char *msg);
  extern int input_from_file;	/* else from ntpq :config */


/* Line 189 of yacc.c  */
#line 105 "ntp_parser.c"

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
     T_Allpeers = 261,
     T_Auth = 262,
     T_Autokey = 263,
     T_Automax = 264,
     T_Average = 265,
     T_Bclient = 266,
     T_Beacon = 267,
     T_Broadcast = 268,
     T_Broadcastclient = 269,
     T_Broadcastdelay = 270,
     T_Burst = 271,
     T_Calibrate = 272,
     T_Ceiling = 273,
     T_Clockstats = 274,
     T_Cohort = 275,
     T_ControlKey = 276,
     T_Crypto = 277,
     T_Cryptostats = 278,
     T_Ctl = 279,
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
     T_Intrange = 315,
     T_Io = 316,
     T_Ipv4 = 317,
     T_Ipv4_flag = 318,
     T_Ipv6 = 319,
     T_Ipv6_flag = 320,
     T_Kernel = 321,
     T_Key = 322,
     T_Keys = 323,
     T_Keysdir = 324,
     T_Kod = 325,
     T_Mssntp = 326,
     T_Leapfile = 327,
     T_Limited = 328,
     T_Link = 329,
     T_Listen = 330,
     T_Logconfig = 331,
     T_Logfile = 332,
     T_Loopstats = 333,
     T_Lowpriotrap = 334,
     T_Manycastclient = 335,
     T_Manycastserver = 336,
     T_Mask = 337,
     T_Maxage = 338,
     T_Maxclock = 339,
     T_Maxdepth = 340,
     T_Maxdist = 341,
     T_Maxmem = 342,
     T_Maxpoll = 343,
     T_Mem = 344,
     T_Minclock = 345,
     T_Mindepth = 346,
     T_Mindist = 347,
     T_Minimum = 348,
     T_Minpoll = 349,
     T_Minsane = 350,
     T_Mode = 351,
     T_Mode7 = 352,
     T_Monitor = 353,
     T_Month = 354,
     T_Mru = 355,
     T_Multicastclient = 356,
     T_Nic = 357,
     T_Nolink = 358,
     T_Nomodify = 359,
     T_None = 360,
     T_Nonvolatile = 361,
     T_Nopeer = 362,
     T_Noquery = 363,
     T_Noselect = 364,
     T_Noserve = 365,
     T_Notrap = 366,
     T_Notrust = 367,
     T_Ntp = 368,
     T_Ntpport = 369,
     T_NtpSignDsocket = 370,
     T_Orphan = 371,
     T_Orphanwait = 372,
     T_Panic = 373,
     T_Peer = 374,
     T_Peerstats = 375,
     T_Phone = 376,
     T_Pid = 377,
     T_Pidfile = 378,
     T_Pool = 379,
     T_Port = 380,
     T_Preempt = 381,
     T_Prefer = 382,
     T_Protostats = 383,
     T_Pw = 384,
     T_Qos = 385,
     T_Randfile = 386,
     T_Rawstats = 387,
     T_Refid = 388,
     T_Requestkey = 389,
     T_Reset = 390,
     T_Restrict = 391,
     T_Revoke = 392,
     T_Saveconfigdir = 393,
     T_Server = 394,
     T_Setvar = 395,
     T_Source = 396,
     T_Statistics = 397,
     T_Stats = 398,
     T_Statsdir = 399,
     T_Step = 400,
     T_Stepout = 401,
     T_Stratum = 402,
     T_String = 403,
     T_Sys = 404,
     T_Sysstats = 405,
     T_Tick = 406,
     T_Time1 = 407,
     T_Time2 = 408,
     T_Timer = 409,
     T_Timingstats = 410,
     T_Tinker = 411,
     T_Tos = 412,
     T_Trap = 413,
     T_True = 414,
     T_Trustedkey = 415,
     T_Ttl = 416,
     T_Type = 417,
     T_U_int = 418,
     T_Unconfig = 419,
     T_Unpeer = 420,
     T_Version = 421,
     T_WanderThreshold = 422,
     T_Week = 423,
     T_Wildcard = 424,
     T_Xleave = 425,
     T_Year = 426,
     T_Flag = 427,
     T_EOC = 428,
     T_Simulate = 429,
     T_Beep_Delay = 430,
     T_Sim_Duration = 431,
     T_Server_Offset = 432,
     T_Duration = 433,
     T_Freq_Offset = 434,
     T_Wander = 435,
     T_Jitter = 436,
     T_Prop_Delay = 437,
     T_Proc_Delay = 438
   };
#endif
/* Tokens.  */
#define T_Age 258
#define T_All 259
#define T_Allan 260
#define T_Allpeers 261
#define T_Auth 262
#define T_Autokey 263
#define T_Automax 264
#define T_Average 265
#define T_Bclient 266
#define T_Beacon 267
#define T_Broadcast 268
#define T_Broadcastclient 269
#define T_Broadcastdelay 270
#define T_Burst 271
#define T_Calibrate 272
#define T_Ceiling 273
#define T_Clockstats 274
#define T_Cohort 275
#define T_ControlKey 276
#define T_Crypto 277
#define T_Cryptostats 278
#define T_Ctl 279
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
#define T_Intrange 315
#define T_Io 316
#define T_Ipv4 317
#define T_Ipv4_flag 318
#define T_Ipv6 319
#define T_Ipv6_flag 320
#define T_Kernel 321
#define T_Key 322
#define T_Keys 323
#define T_Keysdir 324
#define T_Kod 325
#define T_Mssntp 326
#define T_Leapfile 327
#define T_Limited 328
#define T_Link 329
#define T_Listen 330
#define T_Logconfig 331
#define T_Logfile 332
#define T_Loopstats 333
#define T_Lowpriotrap 334
#define T_Manycastclient 335
#define T_Manycastserver 336
#define T_Mask 337
#define T_Maxage 338
#define T_Maxclock 339
#define T_Maxdepth 340
#define T_Maxdist 341
#define T_Maxmem 342
#define T_Maxpoll 343
#define T_Mem 344
#define T_Minclock 345
#define T_Mindepth 346
#define T_Mindist 347
#define T_Minimum 348
#define T_Minpoll 349
#define T_Minsane 350
#define T_Mode 351
#define T_Mode7 352
#define T_Monitor 353
#define T_Month 354
#define T_Mru 355
#define T_Multicastclient 356
#define T_Nic 357
#define T_Nolink 358
#define T_Nomodify 359
#define T_None 360
#define T_Nonvolatile 361
#define T_Nopeer 362
#define T_Noquery 363
#define T_Noselect 364
#define T_Noserve 365
#define T_Notrap 366
#define T_Notrust 367
#define T_Ntp 368
#define T_Ntpport 369
#define T_NtpSignDsocket 370
#define T_Orphan 371
#define T_Orphanwait 372
#define T_Panic 373
#define T_Peer 374
#define T_Peerstats 375
#define T_Phone 376
#define T_Pid 377
#define T_Pidfile 378
#define T_Pool 379
#define T_Port 380
#define T_Preempt 381
#define T_Prefer 382
#define T_Protostats 383
#define T_Pw 384
#define T_Qos 385
#define T_Randfile 386
#define T_Rawstats 387
#define T_Refid 388
#define T_Requestkey 389
#define T_Reset 390
#define T_Restrict 391
#define T_Revoke 392
#define T_Saveconfigdir 393
#define T_Server 394
#define T_Setvar 395
#define T_Source 396
#define T_Statistics 397
#define T_Stats 398
#define T_Statsdir 399
#define T_Step 400
#define T_Stepout 401
#define T_Stratum 402
#define T_String 403
#define T_Sys 404
#define T_Sysstats 405
#define T_Tick 406
#define T_Time1 407
#define T_Time2 408
#define T_Timer 409
#define T_Timingstats 410
#define T_Tinker 411
#define T_Tos 412
#define T_Trap 413
#define T_True 414
#define T_Trustedkey 415
#define T_Ttl 416
#define T_Type 417
#define T_U_int 418
#define T_Unconfig 419
#define T_Unpeer 420
#define T_Version 421
#define T_WanderThreshold 422
#define T_Week 423
#define T_Wildcard 424
#define T_Xleave 425
#define T_Year 426
#define T_Flag 427
#define T_EOC 428
#define T_Simulate 429
#define T_Beep_Delay 430
#define T_Sim_Duration 431
#define T_Server_Offset 432
#define T_Duration 433
#define T_Freq_Offset 434
#define T_Wander 435
#define T_Jitter 436
#define T_Prop_Delay 437
#define T_Proc_Delay 438




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 49 "ntp_parser.y"

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



/* Line 214 of yacc.c  */
#line 528 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 540 "ntp_parser.c"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
#define YYFINAL  195
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   560

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  189
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  100
/* YYNRULES -- Number of rules.  */
#define YYNRULES  297
/* YYNRULES -- Number of states.  */
#define YYNSTATES  398

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   438

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     185,   186,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   184,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   187,     2,   188,     2,     2,     2,     2,
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
     175,   176,   177,   178,   179,   180,   181,   182,   183
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    15,    16,    18,    20,
      22,    24,    26,    28,    30,    32,    34,    36,    38,    40,
      44,    46,    48,    50,    52,    54,    56,    59,    61,    63,
      65,    66,    69,    71,    73,    75,    77,    79,    81,    83,
      85,    87,    89,    91,    93,    96,    99,   101,   103,   105,
     107,   109,   111,   114,   116,   119,   121,   123,   125,   128,
     131,   134,   137,   140,   143,   146,   149,   152,   155,   158,
     159,   162,   165,   168,   170,   172,   174,   176,   178,   181,
     184,   186,   189,   192,   195,   197,   199,   201,   203,   205,
     207,   209,   211,   213,   215,   218,   221,   225,   228,   230,
     232,   234,   236,   238,   240,   242,   244,   246,   247,   250,
     253,   256,   258,   260,   262,   264,   266,   268,   270,   272,
     274,   276,   278,   280,   282,   285,   288,   292,   298,   302,
     307,   312,   316,   317,   320,   322,   324,   326,   328,   330,
     332,   334,   336,   338,   340,   342,   344,   346,   348,   351,
     353,   356,   358,   360,   362,   365,   367,   370,   372,   374,
     376,   378,   380,   382,   384,   386,   390,   393,   395,   398,
     401,   404,   407,   409,   411,   413,   415,   417,   419,   422,
     425,   428,   430,   432,   434,   436,   438,   440,   442,   444,
     446,   448,   450,   453,   456,   458,   461,   463,   465,   467,
     469,   471,   473,   475,   477,   479,   482,   485,   488,   492,
     494,   497,   500,   503,   506,   510,   513,   515,   517,   519,
     521,   523,   525,   527,   529,   531,   533,   536,   537,   542,
     544,   545,   546,   549,   552,   555,   558,   560,   562,   566,
     570,   572,   574,   576,   578,   580,   582,   584,   586,   588,
     591,   594,   596,   598,   600,   602,   604,   606,   608,   610,
     613,   615,   618,   620,   622,   624,   630,   633,   635,   638,
     640,   642,   644,   646,   648,   650,   656,   658,   662,   665,
     669,   671,   673,   676,   678,   684,   689,   693,   696,   698,
     705,   709,   712,   716,   718,   720,   722,   724
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     190,     0,    -1,   191,    -1,   191,   192,   173,    -1,   192,
     173,    -1,     1,   173,    -1,    -1,   193,    -1,   206,    -1,
     208,    -1,   209,    -1,   218,    -1,   226,    -1,   213,    -1,
     235,    -1,   240,    -1,   245,    -1,   249,    -1,   275,    -1,
     194,   195,   198,    -1,   139,    -1,   124,    -1,   119,    -1,
      13,    -1,    80,    -1,   196,    -1,   197,   148,    -1,   148,
      -1,    63,    -1,    65,    -1,    -1,   198,   199,    -1,   200,
      -1,   202,    -1,   204,    -1,   201,    -1,     8,    -1,    16,
      -1,    50,    -1,   109,    -1,   126,    -1,   127,    -1,   159,
      -1,   170,    -1,   203,    58,    -1,   203,   163,    -1,    67,
      -1,    94,    -1,    88,    -1,   161,    -1,    96,    -1,   166,
      -1,   205,   148,    -1,    51,    -1,   207,   195,    -1,   164,
      -1,   165,    -1,    14,    -1,    81,   272,    -1,   101,   272,
      -1,     9,    58,    -1,    21,    58,    -1,    22,   210,    -1,
      68,   148,    -1,    69,   148,    -1,   134,    58,    -1,   137,
      58,    -1,   160,   268,    -1,   115,   148,    -1,    -1,   210,
     211,    -1,   212,   148,    -1,   137,    58,    -1,    48,    -1,
      51,    -1,   129,    -1,   131,    -1,    27,    -1,   157,   214,
      -1,   214,   215,    -1,   215,    -1,   216,    58,    -1,   217,
     274,    -1,    20,   273,    -1,    18,    -1,    45,    -1,   116,
      -1,   117,    -1,    95,    -1,    12,    -1,    92,    -1,    86,
      -1,    90,    -1,    84,    -1,   142,   219,    -1,   144,   148,
      -1,    39,   220,   221,    -1,   219,   220,    -1,   220,    -1,
      19,    -1,    23,    -1,    78,    -1,   120,    -1,   132,    -1,
     150,    -1,   155,    -1,   128,    -1,    -1,   221,   222,    -1,
      38,   148,    -1,   162,   225,    -1,   223,    -1,   224,    -1,
      74,    -1,   103,    -1,    35,    -1,    28,    -1,   105,    -1,
     122,    -1,    25,    -1,   168,    -1,    99,    -1,   171,    -1,
       3,    -1,    29,   229,    -1,   100,   232,    -1,   136,   195,
     227,    -1,   136,   196,    82,   196,   227,    -1,   136,    26,
     227,    -1,   136,    63,    26,   227,    -1,   136,    65,    26,
     227,    -1,   136,   141,   227,    -1,    -1,   227,   228,    -1,
      44,    -1,    52,    -1,    70,    -1,    71,    -1,    73,    -1,
      79,    -1,   104,    -1,   107,    -1,   108,    -1,   110,    -1,
     111,    -1,   112,    -1,   114,    -1,   166,    -1,   229,   230,
      -1,   230,    -1,   231,    58,    -1,    10,    -1,    93,    -1,
      98,    -1,   232,   233,    -1,   233,    -1,   234,    58,    -1,
      53,    -1,    54,    -1,    55,    -1,    56,    -1,    83,    -1,
      85,    -1,    87,    -1,    91,    -1,    47,   195,   236,    -1,
     236,   237,    -1,   237,    -1,   238,   274,    -1,   239,   273,
      -1,   147,    58,    -1,   133,   148,    -1,   152,    -1,   153,
      -1,    40,    -1,    41,    -1,    42,    -1,    43,    -1,    35,
     241,    -1,    28,   241,    -1,   241,   242,    -1,   242,    -1,
     243,    -1,   244,    -1,     7,    -1,    11,    -1,    17,    -1,
      66,    -1,    98,    -1,   113,    -1,    97,    -1,   143,    -1,
     156,   246,    -1,   246,   247,    -1,   247,    -1,   248,   274,
      -1,     5,    -1,    30,    -1,    46,    -1,    49,    -1,   118,
      -1,   145,    -1,   146,    -1,   260,    -1,   264,    -1,   250,
     274,    -1,   251,   148,    -1,   252,   148,    -1,    57,   148,
     192,    -1,    36,    -1,    32,   253,    -1,    76,   258,    -1,
     121,   271,    -1,   140,   254,    -1,   158,   196,   256,    -1,
     161,   267,    -1,    15,    -1,   106,    -1,   151,    -1,    51,
      -1,    72,    -1,   123,    -1,   130,    -1,    77,    -1,   138,
      -1,   148,    -1,   148,    31,    -1,    -1,   148,   184,   148,
     255,    -1,    26,    -1,    -1,    -1,   256,   257,    -1,   125,
      58,    -1,    59,   196,    -1,   258,   259,    -1,   259,    -1,
     148,    -1,   261,   263,   262,    -1,   261,   263,   148,    -1,
      59,    -1,   102,    -1,     4,    -1,    62,    -1,    64,    -1,
     169,    -1,    75,    -1,    52,    -1,    33,    -1,   135,   265,
      -1,   265,   266,    -1,   266,    -1,     6,    -1,     7,    -1,
      24,    -1,    61,    -1,    89,    -1,   149,    -1,   154,    -1,
     267,    58,    -1,    58,    -1,   268,   269,    -1,   269,    -1,
      58,    -1,   270,    -1,   185,    58,    34,    58,   186,    -1,
     271,   148,    -1,   148,    -1,   272,   195,    -1,   195,    -1,
      58,    -1,   159,    -1,    37,    -1,    58,    -1,    31,    -1,
     276,   187,   277,   280,   188,    -1,   174,    -1,   277,   278,
     173,    -1,   278,   173,    -1,   279,   184,   274,    -1,   175,
      -1,   176,    -1,   280,   281,    -1,   281,    -1,   283,   187,
     282,   284,   188,    -1,   177,   184,   274,   173,    -1,   139,
     184,   195,    -1,   284,   285,    -1,   285,    -1,   178,   184,
     274,   187,   286,   188,    -1,   286,   287,   173,    -1,   287,
     173,    -1,   288,   184,   274,    -1,   179,    -1,   180,    -1,
     181,    -1,   182,    -1,   183,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   350,   350,   354,   355,   356,   370,   371,   372,   373,
     374,   375,   376,   377,   378,   379,   380,   381,   382,   390,
     400,   401,   402,   403,   404,   408,   409,   414,   419,   421,
     427,   428,   436,   437,   438,   442,   447,   448,   449,   450,
     451,   452,   453,   454,   458,   460,   465,   466,   467,   468,
     469,   470,   474,   479,   488,   498,   499,   509,   511,   513,
     524,   531,   533,   538,   540,   542,   544,   546,   548,   554,
     555,   563,   565,   577,   578,   579,   580,   581,   590,   595,
     600,   608,   610,   612,   617,   618,   619,   620,   621,   622,
     626,   627,   628,   629,   638,   640,   649,   659,   664,   672,
     673,   674,   675,   676,   677,   678,   679,   684,   685,   693,
     703,   712,   727,   732,   733,   737,   738,   742,   743,   744,
     745,   746,   747,   748,   757,   761,   765,   773,   781,   789,
     804,   819,   832,   833,   841,   842,   843,   844,   845,   846,
     847,   848,   849,   850,   851,   852,   853,   854,   858,   863,
     871,   876,   877,   878,   882,   887,   895,   900,   901,   902,
     903,   904,   905,   906,   907,   915,   925,   930,   938,   940,
     942,   944,   949,   950,   954,   955,   956,   957,   965,   967,
     972,   977,   985,   987,  1004,  1005,  1006,  1007,  1008,  1009,
    1013,  1014,  1022,  1027,  1032,  1040,  1045,  1046,  1047,  1048,
    1049,  1050,  1051,  1060,  1061,  1062,  1069,  1076,  1092,  1111,
    1116,  1118,  1120,  1122,  1124,  1131,  1136,  1137,  1138,  1142,
    1143,  1144,  1145,  1149,  1150,  1154,  1161,  1171,  1180,  1185,
    1187,  1192,  1193,  1201,  1203,  1211,  1216,  1224,  1249,  1256,
    1266,  1267,  1271,  1272,  1273,  1274,  1278,  1279,  1280,  1284,
    1289,  1294,  1302,  1303,  1304,  1305,  1306,  1307,  1308,  1318,
    1323,  1331,  1336,  1344,  1346,  1350,  1355,  1360,  1368,  1373,
    1381,  1390,  1391,  1395,  1396,  1405,  1423,  1427,  1432,  1440,
    1445,  1446,  1450,  1455,  1463,  1468,  1473,  1478,  1483,  1491,
    1496,  1501,  1509,  1514,  1515,  1516,  1517,  1518
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_Age", "T_All", "T_Allan",
  "T_Allpeers", "T_Auth", "T_Autokey", "T_Automax", "T_Average",
  "T_Bclient", "T_Beacon", "T_Broadcast", "T_Broadcastclient",
  "T_Broadcastdelay", "T_Burst", "T_Calibrate", "T_Ceiling",
  "T_Clockstats", "T_Cohort", "T_ControlKey", "T_Crypto", "T_Cryptostats",
  "T_Ctl", "T_Day", "T_Default", "T_Digest", "T_Disable", "T_Discard",
  "T_Dispersion", "T_Double", "T_Driftfile", "T_Drop", "T_Ellipsis",
  "T_Enable", "T_End", "T_False", "T_File", "T_Filegen", "T_Flag1",
  "T_Flag2", "T_Flag3", "T_Flag4", "T_Flake", "T_Floor", "T_Freq",
  "T_Fudge", "T_Host", "T_Huffpuff", "T_Iburst", "T_Ident", "T_Ignore",
  "T_Incalloc", "T_Incmem", "T_Initalloc", "T_Initmem", "T_Includefile",
  "T_Integer", "T_Interface", "T_Intrange", "T_Io", "T_Ipv4",
  "T_Ipv4_flag", "T_Ipv6", "T_Ipv6_flag", "T_Kernel", "T_Key", "T_Keys",
  "T_Keysdir", "T_Kod", "T_Mssntp", "T_Leapfile", "T_Limited", "T_Link",
  "T_Listen", "T_Logconfig", "T_Logfile", "T_Loopstats", "T_Lowpriotrap",
  "T_Manycastclient", "T_Manycastserver", "T_Mask", "T_Maxage",
  "T_Maxclock", "T_Maxdepth", "T_Maxdist", "T_Maxmem", "T_Maxpoll",
  "T_Mem", "T_Minclock", "T_Mindepth", "T_Mindist", "T_Minimum",
  "T_Minpoll", "T_Minsane", "T_Mode", "T_Mode7", "T_Monitor", "T_Month",
  "T_Mru", "T_Multicastclient", "T_Nic", "T_Nolink", "T_Nomodify",
  "T_None", "T_Nonvolatile", "T_Nopeer", "T_Noquery", "T_Noselect",
  "T_Noserve", "T_Notrap", "T_Notrust", "T_Ntp", "T_Ntpport",
  "T_NtpSignDsocket", "T_Orphan", "T_Orphanwait", "T_Panic", "T_Peer",
  "T_Peerstats", "T_Phone", "T_Pid", "T_Pidfile", "T_Pool", "T_Port",
  "T_Preempt", "T_Prefer", "T_Protostats", "T_Pw", "T_Qos", "T_Randfile",
  "T_Rawstats", "T_Refid", "T_Requestkey", "T_Reset", "T_Restrict",
  "T_Revoke", "T_Saveconfigdir", "T_Server", "T_Setvar", "T_Source",
  "T_Statistics", "T_Stats", "T_Statsdir", "T_Step", "T_Stepout",
  "T_Stratum", "T_String", "T_Sys", "T_Sysstats", "T_Tick", "T_Time1",
  "T_Time2", "T_Timer", "T_Timingstats", "T_Tinker", "T_Tos", "T_Trap",
  "T_True", "T_Trustedkey", "T_Ttl", "T_Type", "T_U_int", "T_Unconfig",
  "T_Unpeer", "T_Version", "T_WanderThreshold", "T_Week", "T_Wildcard",
  "T_Xleave", "T_Year", "T_Flag", "T_EOC", "T_Simulate", "T_Beep_Delay",
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
  "system_option_command", "system_option_list", "system_option",
  "system_option_flag_keyword", "system_option_local_flag_keyword",
  "tinker_command", "tinker_option_list", "tinker_option",
  "tinker_option_keyword", "miscellaneous_command", "misc_cmd_dbl_keyword",
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
  "sim_act_keyword", 0
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
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,    61,    40,    41,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   189,   190,   191,   191,   191,   192,   192,   192,   192,
     192,   192,   192,   192,   192,   192,   192,   192,   192,   193,
     194,   194,   194,   194,   194,   195,   195,   196,   197,   197,
     198,   198,   199,   199,   199,   200,   201,   201,   201,   201,
     201,   201,   201,   201,   202,   202,   203,   203,   203,   203,
     203,   203,   204,   205,   206,   207,   207,   208,   208,   208,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   210,
     210,   211,   211,   212,   212,   212,   212,   212,   213,   214,
     214,   215,   215,   215,   216,   216,   216,   216,   216,   216,
     217,   217,   217,   217,   218,   218,   218,   219,   219,   220,
     220,   220,   220,   220,   220,   220,   220,   221,   221,   222,
     222,   222,   222,   223,   223,   224,   224,   225,   225,   225,
     225,   225,   225,   225,   226,   226,   226,   226,   226,   226,
     226,   226,   227,   227,   228,   228,   228,   228,   228,   228,
     228,   228,   228,   228,   228,   228,   228,   228,   229,   229,
     230,   231,   231,   231,   232,   232,   233,   234,   234,   234,
     234,   234,   234,   234,   234,   235,   236,   236,   237,   237,
     237,   237,   238,   238,   239,   239,   239,   239,   240,   240,
     241,   241,   242,   242,   243,   243,   243,   243,   243,   243,
     244,   244,   245,   246,   246,   247,   248,   248,   248,   248,
     248,   248,   248,   249,   249,   249,   249,   249,   249,   249,
     249,   249,   249,   249,   249,   249,   250,   250,   250,   251,
     251,   251,   251,   252,   252,   253,   253,   253,   254,   255,
     255,   256,   256,   257,   257,   258,   258,   259,   260,   260,
     261,   261,   262,   262,   262,   262,   263,   263,   263,   264,
     265,   265,   266,   266,   266,   266,   266,   266,   266,   267,
     267,   268,   268,   269,   269,   270,   271,   271,   272,   272,
     273,   273,   273,   274,   274,   275,   276,   277,   277,   278,
     279,   279,   280,   280,   281,   282,   283,   284,   284,   285,
     286,   286,   287,   288,   288,   288,   288,   288
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     2,     1,     2,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     0,
       2,     2,     2,     1,     1,     1,     1,     1,     2,     2,
       1,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     3,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     3,     5,     3,     4,
       4,     3,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     1,     1,     1,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     1,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     2,     3,     1,
       2,     2,     2,     2,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     0,     4,     1,
       0,     0,     2,     2,     2,     2,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     2,     1,     1,     1,     5,     2,     1,     2,     1,
       1,     1,     1,     1,     1,     5,     1,     3,     2,     3,
       1,     1,     2,     1,     5,     4,     3,     2,     1,     6,
       3,     2,     3,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,    23,    57,   216,     0,    69,     0,     0,
     227,     0,   209,     0,     0,   219,     0,   240,     0,     0,
     220,     0,   223,    24,     0,     0,     0,   241,   217,     0,
      22,     0,   221,    21,   222,     0,     0,     0,     0,   224,
      20,     0,     0,     0,   218,     0,     0,     0,     0,     0,
      55,    56,   276,     0,     2,     0,     7,     0,     8,     0,
       9,    10,    13,    11,    12,    14,    15,    16,    17,     0,
       0,     0,   203,     0,   204,    18,     0,     5,    60,    61,
      62,   184,   185,   186,   187,   190,   188,   189,   191,   179,
     181,   182,   183,   151,   152,   153,   124,   149,     0,   225,
     210,   178,    99,   100,   101,   102,   106,   103,   104,   105,
     107,    28,    29,    27,     0,    25,     0,     6,    63,    64,
     237,   211,   236,   269,    58,   157,   158,   159,   160,   161,
     162,   163,   164,   125,   155,     0,    59,    68,   267,   212,
      65,   252,   253,   254,   255,   256,   257,   258,   249,   251,
     132,    28,    29,   132,   132,    25,    66,     0,   213,    94,
      98,    95,   196,   197,   198,   199,   200,   201,   202,   192,
     194,     0,    89,    84,     0,    85,    93,    91,    92,    90,
      88,    86,    87,    78,    80,     0,     0,   231,   263,     0,
      67,   262,   264,   260,   215,     1,     0,     4,    30,    54,
     274,   273,   205,   206,   207,   248,   247,   246,     0,     0,
      77,    73,    74,    75,    76,     0,    70,     0,   180,   148,
     150,   226,    96,   174,   175,   176,   177,     0,     0,   172,
     173,   165,   167,     0,     0,    26,   208,   235,   268,   154,
     156,   266,   250,   128,   132,   132,   131,   126,     0,     0,
      97,   193,   195,   272,   270,   271,    83,    79,    81,    82,
     214,     0,   261,   259,     3,    19,   242,   243,   244,   239,
     245,   238,   280,   281,     0,     0,     0,    72,    71,   116,
     115,     0,   113,   114,     0,   108,   111,   112,   171,   170,
     166,   168,   169,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   133,   129,   130,
     132,   230,     0,     0,   232,     0,    36,    37,    38,    53,
      46,    48,    47,    50,    39,    40,    41,    42,    49,    51,
      43,    31,    32,    35,    33,     0,    34,     0,     0,     0,
       0,   283,     0,   278,     0,   109,   123,   119,   121,   117,
     118,   120,   122,   110,   127,   229,   228,   234,   233,     0,
      44,    45,    52,     0,   277,   275,   282,     0,   279,   265,
     286,     0,     0,     0,     0,     0,   288,     0,     0,   284,
     287,   285,     0,     0,   293,   294,   295,   296,   297,     0,
       0,     0,   289,     0,   291,     0,   290,   292
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    53,    54,    55,    56,    57,   123,   115,   116,   265,
     331,   332,   333,   334,   335,   336,   337,    58,    59,    60,
      61,    80,   216,   217,    62,   183,   184,   185,   186,    63,
     159,   110,   222,   285,   286,   287,   353,    64,   243,   307,
      96,    97,    98,   133,   134,   135,    65,   231,   232,   233,
     234,    66,    89,    90,    91,    92,    67,   169,   170,   171,
      68,    69,    70,    71,   100,   158,   356,   260,   314,   121,
     122,    72,    73,   271,   208,    74,   148,   149,   194,   190,
     191,   192,   139,   124,   256,   202,    75,    76,   274,   275,
     276,   340,   341,   372,   342,   375,   376,   389,   390,   391
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -170
static const yytype_int16 yypact[] =
{
      12,  -144,   -20,  -170,  -170,  -170,   -16,  -170,   123,     8,
     -98,   123,  -170,   -11,    -7,  -170,   -96,  -170,   -95,   -94,
    -170,   -91,  -170,  -170,    -7,   467,    -7,  -170,  -170,   -72,
    -170,   -71,  -170,  -170,  -170,    -3,    54,    40,    27,  -170,
    -170,   -62,   -11,   -61,  -170,    19,   419,   -58,   -53,    33,
    -170,  -170,  -170,    96,   178,   -75,  -170,    -7,  -170,    -7,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,    39,
     -49,   -48,  -170,   -13,  -170,  -170,   -83,  -170,  -170,  -170,
     241,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,   123,
    -170,  -170,  -170,  -170,  -170,  -170,     8,  -170,    49,    80,
    -170,   123,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,   264,  -170,   -32,   332,  -170,  -170,
    -170,   -91,  -170,  -170,    -7,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,   467,  -170,    61,    -7,  -170,  -170,   -25,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,    54,  -170,
    -170,   100,   102,  -170,  -170,    47,  -170,   -39,  -170,   -11,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,    19,
    -170,    39,  -170,  -170,   -21,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,   419,  -170,    95,    39,  -170,  -170,    97,
     -53,  -170,  -170,  -170,    99,  -170,   -12,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,    10,  -140,
    -170,  -170,  -170,  -170,  -170,   101,  -170,    14,  -170,  -170,
    -170,  -170,   195,  -170,  -170,  -170,  -170,    18,   109,  -170,
    -170,   264,  -170,    39,   -21,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,   311,  -170,  -170,   311,   311,   -58,    30,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
     -50,   146,  -170,  -170,  -170,   390,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,   -93,     9,    -1,  -170,  -170,  -170,
    -170,    36,  -170,  -170,     3,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,   311,   311,
    -170,   164,   -58,   143,  -170,   144,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,  -170,   -43,  -170,    57,    31,    38,
    -109,  -170,    29,  -170,    39,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,   311,  -170,  -170,  -170,  -170,    32,
    -170,  -170,  -170,    -7,  -170,  -170,  -170,    42,  -170,  -170,
    -170,    43,    44,    39,    48,  -156,  -170,    51,    39,  -170,
    -170,  -170,    41,    15,  -170,  -170,  -170,  -170,  -170,    60,
      58,    50,  -170,    65,  -170,    39,  -170,  -170
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -170,  -170,  -170,   -44,  -170,  -170,   -14,   -36,  -170,  -170,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,  -170,  -170,    62,  -170,  -170,  -170,
    -170,   -35,  -170,  -170,  -170,  -170,  -170,  -170,  -150,  -170,
    -170,   148,  -170,  -170,   116,  -170,  -170,  -170,    20,  -170,
    -170,  -170,   242,   -70,  -170,  -170,  -170,  -170,    83,  -170,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
     135,  -170,  -170,  -170,  -170,  -170,  -170,   112,  -170,  -170,
      67,  -170,  -170,   235,    28,  -169,  -170,  -170,  -170,   -10,
    -170,  -170,   -77,  -170,  -170,  -170,  -110,  -170,  -122,  -170
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     114,   155,   252,   246,   247,   188,   346,   160,   102,   312,
     196,   187,   103,     1,   266,   360,   253,   259,    93,   218,
     205,     2,   374,   154,   162,     3,     4,     5,   347,    77,
     338,   218,   379,     6,     7,   272,   273,   254,    78,   206,
       8,     9,    79,   198,    10,   199,   338,    11,    12,   163,
      99,    13,   117,   118,   119,   140,   111,   120,   112,    14,
     141,   142,   207,    15,   291,   164,   150,   104,   165,    16,
     200,    17,   267,   236,   268,   313,   137,   138,   143,   365,
      18,    19,   272,   273,    20,   156,   157,   161,    21,    22,
     113,   193,    23,    24,   308,   309,   195,   201,   197,   203,
     204,    94,   348,   151,   209,   152,    95,   220,   349,   105,
     238,   221,    25,    26,    27,   144,   235,   106,    28,   240,
     361,   107,   238,   241,   250,   350,   244,    29,   245,   248,
      81,    30,   189,    31,    82,    32,    33,   166,   255,   108,
      83,   113,    34,   145,   109,   249,    35,    36,    37,    38,
      39,    40,    41,   258,    42,   261,    43,   263,   269,   277,
     354,   264,   278,    44,   167,   168,   288,   289,    45,    46,
      47,   351,    48,    49,   352,   368,    50,    51,   311,   270,
     315,   153,   343,   344,   345,    -6,    52,     2,   113,    84,
     355,     3,     4,     5,   384,   385,   386,   387,   388,     6,
       7,   358,   359,   146,   377,   362,     8,     9,   147,   382,
      10,   364,   310,    11,    12,   363,   367,    13,   369,   371,
      85,    86,   374,   279,   381,    14,   397,   373,   383,    15,
     280,   394,   378,   281,   395,    16,    87,    17,   396,   384,
     385,   386,   387,   388,   219,   257,    18,    19,   392,   239,
      20,   290,   251,   101,    21,    22,   237,   262,    23,    24,
     242,   136,   292,   366,   339,   380,    88,   393,   210,   282,
       0,     0,     0,     0,     0,     0,   357,     0,    25,    26,
      27,     0,     0,     0,    28,     0,     0,     0,     0,   211,
       0,     0,   212,    29,     0,     0,     0,    30,   283,    31,
       0,    32,    33,     0,   223,   224,   225,   226,    34,     0,
       0,     0,    35,    36,    37,    38,    39,    40,    41,     0,
      42,     0,    43,     0,     0,     0,     0,     0,     0,    44,
       0,     0,     0,     0,    45,    46,    47,     0,    48,    49,
       0,     2,    50,    51,     0,     3,     4,     5,     0,   370,
       0,    -6,    52,     6,     7,   293,     0,   284,     0,     0,
       8,     9,     0,   294,    10,     0,     0,    11,    12,     0,
     213,    13,   214,     0,     0,     0,     0,     0,   215,    14,
       0,   295,   296,    15,   297,     0,     0,     0,     0,    16,
     298,    17,     0,     0,     0,     0,     0,   227,   316,     0,
      18,    19,     0,     0,    20,     0,   317,     0,    21,    22,
       0,   228,    23,    24,     0,   299,   229,   230,   300,   301,
       0,   302,   303,   304,     0,   305,     0,     0,     0,     0,
       0,   172,    25,    26,    27,     0,     0,   173,    28,   174,
     318,   319,     0,     0,     0,     0,     0,    29,     0,     0,
       0,    30,     0,    31,     0,    32,    33,   320,     0,     0,
       0,     0,    34,     0,   175,     0,    35,    36,    37,    38,
      39,    40,    41,     0,    42,     0,    43,   306,   321,     0,
       0,     0,     0,    44,   322,     0,   323,     0,    45,    46,
      47,     0,    48,    49,     0,     0,    50,    51,     0,   324,
       0,     0,     0,   176,     0,   177,    52,     0,     0,   178,
       0,   179,     0,     0,   180,     0,   325,   326,     0,     0,
     125,   126,   127,   128,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   181,   182,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   327,
     129,   328,   130,     0,   131,     0,   329,     0,   132,     0,
     330
};

static const yytype_int16 yycheck[] =
{
      14,    37,   171,   153,   154,    58,     3,    42,    19,    59,
      54,    47,    23,     1,     4,    58,    37,   186,    10,    89,
      33,     9,   178,    37,     5,    13,    14,    15,    25,   173,
     139,   101,   188,    21,    22,   175,   176,    58,    58,    52,
      28,    29,    58,    57,    32,    59,   139,    35,    36,    30,
     148,    39,   148,   148,   148,    58,    63,   148,    65,    47,
       6,     7,    75,    51,   233,    46,    26,    78,    49,    57,
      31,    59,    62,   117,    64,   125,   148,   148,    24,   188,
      68,    69,   175,   176,    72,    58,   148,   148,    76,    77,
     148,    58,    80,    81,   244,   245,     0,    58,   173,   148,
     148,    93,    99,    63,   187,    65,    98,    58,   105,   120,
     124,    31,   100,   101,   102,    61,   148,   128,   106,    58,
     163,   132,   136,   148,   159,   122,    26,   115,    26,    82,
       7,   119,   185,   121,    11,   123,   124,   118,   159,   150,
      17,   148,   130,    89,   155,   184,   134,   135,   136,   137,
     138,   139,   140,    58,   142,    58,   144,    58,   148,    58,
     310,   173,   148,   151,   145,   146,   148,    58,   156,   157,
     158,   168,   160,   161,   171,   344,   164,   165,   148,   169,
      34,   141,   173,   184,   148,   173,   174,     9,   148,    66,
      26,    13,    14,    15,   179,   180,   181,   182,   183,    21,
      22,    58,    58,   149,   373,   148,    28,    29,   154,   378,
      32,   173,   248,    35,    36,   184,   187,    39,   186,   177,
      97,    98,   178,    28,   173,    47,   395,   184,   187,    51,
      35,   173,   184,    38,   184,    57,   113,    59,   173,   179,
     180,   181,   182,   183,    96,   183,    68,    69,   188,   133,
      72,   231,   169,    11,    76,    77,   121,   190,    80,    81,
     148,    26,   234,   340,   274,   375,   143,   389,    27,    74,
      -1,    -1,    -1,    -1,    -1,    -1,   312,    -1,   100,   101,
     102,    -1,    -1,    -1,   106,    -1,    -1,    -1,    -1,    48,
      -1,    -1,    51,   115,    -1,    -1,    -1,   119,   103,   121,
      -1,   123,   124,    -1,    40,    41,    42,    43,   130,    -1,
      -1,    -1,   134,   135,   136,   137,   138,   139,   140,    -1,
     142,    -1,   144,    -1,    -1,    -1,    -1,    -1,    -1,   151,
      -1,    -1,    -1,    -1,   156,   157,   158,    -1,   160,   161,
      -1,     9,   164,   165,    -1,    13,    14,    15,    -1,   363,
      -1,   173,   174,    21,    22,    44,    -1,   162,    -1,    -1,
      28,    29,    -1,    52,    32,    -1,    -1,    35,    36,    -1,
     129,    39,   131,    -1,    -1,    -1,    -1,    -1,   137,    47,
      -1,    70,    71,    51,    73,    -1,    -1,    -1,    -1,    57,
      79,    59,    -1,    -1,    -1,    -1,    -1,   133,     8,    -1,
      68,    69,    -1,    -1,    72,    -1,    16,    -1,    76,    77,
      -1,   147,    80,    81,    -1,   104,   152,   153,   107,   108,
      -1,   110,   111,   112,    -1,   114,    -1,    -1,    -1,    -1,
      -1,    12,   100,   101,   102,    -1,    -1,    18,   106,    20,
      50,    51,    -1,    -1,    -1,    -1,    -1,   115,    -1,    -1,
      -1,   119,    -1,   121,    -1,   123,   124,    67,    -1,    -1,
      -1,    -1,   130,    -1,    45,    -1,   134,   135,   136,   137,
     138,   139,   140,    -1,   142,    -1,   144,   166,    88,    -1,
      -1,    -1,    -1,   151,    94,    -1,    96,    -1,   156,   157,
     158,    -1,   160,   161,    -1,    -1,   164,   165,    -1,   109,
      -1,    -1,    -1,    84,    -1,    86,   174,    -1,    -1,    90,
      -1,    92,    -1,    -1,    95,    -1,   126,   127,    -1,    -1,
      53,    54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   116,   117,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   159,
      83,   161,    85,    -1,    87,    -1,   166,    -1,    91,    -1,
     170
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     1,     9,    13,    14,    15,    21,    22,    28,    29,
      32,    35,    36,    39,    47,    51,    57,    59,    68,    69,
      72,    76,    77,    80,    81,   100,   101,   102,   106,   115,
     119,   121,   123,   124,   130,   134,   135,   136,   137,   138,
     139,   140,   142,   144,   151,   156,   157,   158,   160,   161,
     164,   165,   174,   190,   191,   192,   193,   194,   206,   207,
     208,   209,   213,   218,   226,   235,   240,   245,   249,   250,
     251,   252,   260,   261,   264,   275,   276,   173,    58,    58,
     210,     7,    11,    17,    66,    97,    98,   113,   143,   241,
     242,   243,   244,    10,    93,    98,   229,   230,   231,   148,
     253,   241,    19,    23,    78,   120,   128,   132,   150,   155,
     220,    63,    65,   148,   195,   196,   197,   148,   148,   148,
     148,   258,   259,   195,   272,    53,    54,    55,    56,    83,
      85,    87,    91,   232,   233,   234,   272,   148,   148,   271,
      58,     6,     7,    24,    61,    89,   149,   154,   265,   266,
      26,    63,    65,   141,   195,   196,    58,   148,   254,   219,
     220,   148,     5,    30,    46,    49,   118,   145,   146,   246,
     247,   248,    12,    18,    20,    45,    84,    86,    90,    92,
      95,   116,   117,   214,   215,   216,   217,   196,    58,   185,
     268,   269,   270,    58,   267,     0,   192,   173,   195,   195,
      31,    58,   274,   148,   148,    33,    52,    75,   263,   187,
      27,    48,    51,   129,   131,   137,   211,   212,   242,   230,
      58,    31,   221,    40,    41,    42,    43,   133,   147,   152,
     153,   236,   237,   238,   239,   148,   192,   259,   195,   233,
      58,   148,   266,   227,    26,    26,   227,   227,    82,   184,
     220,   247,   274,    37,    58,   159,   273,   215,    58,   274,
     256,    58,   269,    58,   173,   198,     4,    62,    64,   148,
     169,   262,   175,   176,   277,   278,   279,    58,   148,    28,
      35,    38,    74,   103,   162,   222,   223,   224,   148,    58,
     237,   274,   273,    44,    52,    70,    71,    73,    79,   104,
     107,   108,   110,   111,   112,   114,   166,   228,   227,   227,
     196,   148,    59,   125,   257,    34,     8,    16,    50,    51,
      67,    88,    94,    96,   109,   126,   127,   159,   161,   166,
     170,   199,   200,   201,   202,   203,   204,   205,   139,   278,
     280,   281,   283,   173,   184,   148,     3,    25,    99,   105,
     122,   168,   171,   225,   227,    26,   255,   196,    58,    58,
      58,   163,   148,   184,   173,   188,   281,   187,   274,   186,
     195,   177,   282,   184,   178,   284,   285,   274,   184,   188,
     285,   173,   274,   187,   179,   180,   181,   182,   183,   286,
     287,   288,   188,   287,   173,   184,   173,   274
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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

/* Line 1464 of yacc.c  */
#line 357 "ntp_parser.y"
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

/* Line 1464 of yacc.c  */
#line 391 "ntp_parser.y"
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.peers, my_node);
		}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 410 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), (yyvsp[(1) - (2)].Integer)); }
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 415 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_UNSPEC); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 420 "ntp_parser.y"
    { (yyval.Integer) = AF_INET; }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 422 "ntp_parser.y"
    { (yyval.Integer) = AF_INET6; }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 427 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 429 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 443 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 459 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 461 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_uval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 475 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 489 "ntp_parser.y"
    {
			unpeer_node *my_node;
			
			my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
		}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 510 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 512 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.manycastserver, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 514 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.multicastclient, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 525 "ntp_parser.y"
    {
			attr_val *atrv;
			
			atrv = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			APPEND_G_FIFO(cfgt.vars, atrv);
		}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 532 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 534 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			CONCAT_G_FIFOS(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 539 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 541 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 543 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 545 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 547 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Attr_val_fifo); }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 549 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 554 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 556 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 564 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 566 "ntp_parser.y"
    {
			(yyval.Attr_val) = NULL;
			cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer);
			msyslog(LOG_WARNING,
				"'crypto revoke %d' is deprecated, "
				"please use 'revoke %d' instead.",
				cfgt.auth.revoke, cfgt.auth.revoke);
		}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 591 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.orphan_cmds, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 596 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 601 "ntp_parser.y"
    {	
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 609 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 611 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 613 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 639 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.stats_list, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 641 "ntp_parser.y"
    {
			if (input_from_file) {
				cfgt.stats_dir = (yyvsp[(2) - (2)].String);
			} else {
				YYFREE((yyvsp[(2) - (2)].String));
				yyerror("statsdir remote configuration ignored");
			}
		}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 650 "ntp_parser.y"
    {
			filegen_node *fgn;
			
			fgn = create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.filegen_opts, fgn);
		}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 660 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 665 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 684 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 686 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 694 "ntp_parser.y"
    {
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			} else {
				(yyval.Attr_val) = NULL;
				YYFREE((yyvsp[(2) - (2)].String));
				yyerror("filegen file remote config ignored");
			}
		}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 704 "ntp_parser.y"
    {
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen type remote config ignored");
			}
		}
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 713 "ntp_parser.y"
    {
			const char *err;
			
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				if (T_Link == (yyvsp[(1) - (1)].Integer))
					err = "filegen link remote config ignored";
				else
					err = "filegen nolink remote config ignored";
				yyerror(err);
			}
		}
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 728 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 758 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.discard_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 762 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.mru_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 766 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 774 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 782 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 790 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node(
				create_address_node(
					estrdup("0.0.0.0"), 
					AF_INET),
				create_address_node(
					estrdup("0.0.0.0"), 
					AF_INET),
				(yyvsp[(4) - (4)].Int_fifo), 
				ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 805 "ntp_parser.y"
    {
			restrict_node *rn;
			
			rn = create_restrict_node(
				create_address_node(
					estrdup("::"), 
					AF_INET6),
				create_address_node(
					estrdup("::"), 
					AF_INET6),
				(yyvsp[(4) - (4)].Int_fifo), 
				ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 820 "ntp_parser.y"
    {
			restrict_node *	rn;

			APPEND_G_FIFO((yyvsp[(3) - (3)].Int_fifo), create_int_node((yyvsp[(2) - (3)].Integer)));
			rn = create_restrict_node(
				NULL, NULL, (yyvsp[(3) - (3)].Int_fifo), ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 832 "ntp_parser.y"
    { (yyval.Int_fifo) = NULL; }
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 834 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 859 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 864 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 872 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 883 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 888 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 896 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 916 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.fudge, aon);
		}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 926 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 931 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 939 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 941 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 943 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 945 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 966 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.enable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 968 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.disable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 973 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 978 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 986 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 988 "ntp_parser.y"
    { 
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			} else {
				char err_str[128];
				
				(yyval.Attr_val) = NULL;
				snprintf(err_str, sizeof(err_str),
					 "enable/disable %s remote configuration ignored",
					 keyword((yyvsp[(1) - (1)].Integer)));
				yyerror(err_str);
			}
		}
    break;

  case 192:

/* Line 1464 of yacc.c  */
#line 1023 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.tinker, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 193:

/* Line 1464 of yacc.c  */
#line 1028 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 194:

/* Line 1464 of yacc.c  */
#line 1033 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 195:

/* Line 1464 of yacc.c  */
#line 1041 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 1063 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 1070 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 1077 "ntp_parser.y"
    {
			char error_text[64];
			attr_val *av;

			if (input_from_file) {
				av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
				APPEND_G_FIFO(cfgt.vars, av);
			} else {
				YYFREE((yyvsp[(2) - (2)].String));
				snprintf(error_text, sizeof(error_text),
					 "%s remote config ignored",
					 keyword((yyvsp[(1) - (2)].Integer)));
				yyerror(error_text);
			}
		}
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 1093 "ntp_parser.y"
    {
			if (!input_from_file) {
				yyerror("remote includefile ignored");
				break;
			}
			if (curr_include_level >= MAXINCLUDELEVEL) {
				fprintf(stderr, "getconfig: Maximum include file level exceeded.\n");
				msyslog(LOG_ERR, "getconfig: Maximum include file level exceeded.\n");
			} else {
				fp[curr_include_level + 1] = F_OPEN(FindConfig((yyvsp[(2) - (3)].String)), "r");
				if (fp[curr_include_level + 1] == NULL) {
					fprintf(stderr, "getconfig: Couldn't open <%s>\n", FindConfig((yyvsp[(2) - (3)].String)));
					msyslog(LOG_ERR, "getconfig: Couldn't open <%s>\n", FindConfig((yyvsp[(2) - (3)].String)));
				} else {
					ip_file = fp[++curr_include_level];
				}
			}
		}
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 1112 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 1117 "ntp_parser.y"
    { /* see drift_parm below for actions */ }
    break;

  case 211:

/* Line 1464 of yacc.c  */
#line 1119 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.logconfig, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 212:

/* Line 1464 of yacc.c  */
#line 1121 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.phone, (yyvsp[(2) - (2)].String_fifo)); }
    break;

  case 213:

/* Line 1464 of yacc.c  */
#line 1123 "ntp_parser.y"
    { APPEND_G_FIFO(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 214:

/* Line 1464 of yacc.c  */
#line 1125 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.trap, aon);
		}
    break;

  case 215:

/* Line 1464 of yacc.c  */
#line 1132 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.ttl, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 1155 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 1162 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
			av = create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 1171 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, "");
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1181 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 1187 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 1192 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = NULL; }
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 1194 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 1202 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 1204 "ntp_parser.y"
    {
			(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), estrdup((yyvsp[(2) - (2)].Address_node)->address));
			destroy_address_node((yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 1212 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 236:

/* Line 1464 of yacc.c  */
#line 1217 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 1225 "ntp_parser.y"
    {
			char	prefix;
			char *	type;
			
			switch ((yyvsp[(1) - (1)].String)[0]) {
			
			case '+':
			case '-':
			case '=':
				prefix = (yyvsp[(1) - (1)].String)[0];
				type = (yyvsp[(1) - (1)].String) + 1;
				break;
				
			default:
				prefix = '=';
				type = (yyvsp[(1) - (1)].String);
			}	
			
			(yyval.Attr_val) = create_attr_sval(prefix, estrdup(type));
			YYFREE((yyvsp[(1) - (1)].String));
		}
    break;

  case 238:

/* Line 1464 of yacc.c  */
#line 1250 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 239:

/* Line 1464 of yacc.c  */
#line 1257 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 1285 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.reset_counters, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 1290 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 1295 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 1319 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 1324 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 1332 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 1337 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 1345 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[(1) - (1)].Integer)); }
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 1351 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_rangeval('-', (yyvsp[(2) - (5)].Integer), (yyvsp[(4) - (5)].Integer)); }
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 1356 "ntp_parser.y"
    {
			(yyval.String_fifo) = (yyvsp[(1) - (2)].String_fifo);
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(2) - (2)].String)));
		}
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 1361 "ntp_parser.y"
    {
			(yyval.String_fifo) = NULL;
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(1) - (1)].String)));
		}
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 1369 "ntp_parser.y"
    {
			(yyval.Address_fifo) = (yyvsp[(1) - (2)].Address_fifo);
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 1374 "ntp_parser.y"
    {
			(yyval.Address_fifo) = NULL;
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(1) - (1)].Address_node));
		}
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 1382 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			} else {
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
			}
		}
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 1390 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 1391 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 1395 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 1406 "ntp_parser.y"
    {
			sim_node *sn;
			
			sn =  create_sim_node((yyvsp[(3) - (5)].Attr_val_fifo), (yyvsp[(4) - (5)].Sim_server_fifo));
			APPEND_G_FIFO(cfgt.sim_details, sn);

			/* Revert from ; to \n for end-of-command */
			old_config_style = 1;
		}
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 1423 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 277:

/* Line 1464 of yacc.c  */
#line 1428 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 278:

/* Line 1464 of yacc.c  */
#line 1433 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 279:

/* Line 1464 of yacc.c  */
#line 1441 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 282:

/* Line 1464 of yacc.c  */
#line 1451 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = (yyvsp[(1) - (2)].Sim_server_fifo);
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(2) - (2)].Sim_server));
		}
    break;

  case 283:

/* Line 1464 of yacc.c  */
#line 1456 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(1) - (1)].Sim_server));
		}
    break;

  case 284:

/* Line 1464 of yacc.c  */
#line 1464 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Sim_script_fifo)); }
    break;

  case 285:

/* Line 1464 of yacc.c  */
#line 1469 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 286:

/* Line 1464 of yacc.c  */
#line 1474 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 287:

/* Line 1464 of yacc.c  */
#line 1479 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = (yyvsp[(1) - (2)].Sim_script_fifo);
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(2) - (2)].Sim_script));
		}
    break;

  case 288:

/* Line 1464 of yacc.c  */
#line 1484 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(1) - (1)].Sim_script));
		}
    break;

  case 289:

/* Line 1464 of yacc.c  */
#line 1492 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Attr_val_fifo)); }
    break;

  case 290:

/* Line 1464 of yacc.c  */
#line 1497 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 291:

/* Line 1464 of yacc.c  */
#line 1502 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 292:

/* Line 1464 of yacc.c  */
#line 1510 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1464 of yacc.c  */
#line 3599 "ntp_parser.c"
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



/* Line 1684 of yacc.c  */
#line 1521 "ntp_parser.y"


void 
yyerror(
	const char *msg
	)
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


