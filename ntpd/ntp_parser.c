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


  struct FILE_INFO *ip_file;   /* Pointer to the configuration file stream */

  #define YYMALLOC	emalloc
  #define YYFREE	free
  #define YYERROR_VERBOSE
  #define YYMAXDEPTH	1000   /* stop the madness sooner */
  void yyerror (char *msg);
  extern int input_from_file;  /* 0=input from ntpq :config */


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
     T_Auth = 261,
     T_Autokey = 262,
     T_Automax = 263,
     T_Average = 264,
     T_Bclient = 265,
     T_Beacon = 266,
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
     T_Digest = 281,
     T_Disable = 282,
     T_Discard = 283,
     T_Dispersion = 284,
     T_Double = 285,
     T_Driftfile = 286,
     T_Drop = 287,
     T_Ellipsis = 288,
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
     T_Intrange = 314,
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
     T_EOC = 420,
     T_Simulate = 421,
     T_Beep_Delay = 422,
     T_Sim_Duration = 423,
     T_Server_Offset = 424,
     T_Duration = 425,
     T_Freq_Offset = 426,
     T_Wander = 427,
     T_Jitter = 428,
     T_Prop_Delay = 429,
     T_Proc_Delay = 430
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
#define T_Digest 281
#define T_Disable 282
#define T_Discard 283
#define T_Dispersion 284
#define T_Double 285
#define T_Driftfile 286
#define T_Drop 287
#define T_Ellipsis 288
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
#define T_Intrange 314
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
#define T_EOC 420
#define T_Simulate 421
#define T_Beep_Delay 422
#define T_Sim_Duration 423
#define T_Server_Offset 424
#define T_Duration 425
#define T_Freq_Offset 426
#define T_Wander 427
#define T_Jitter 428
#define T_Prop_Delay 429
#define T_Proc_Delay 430




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 49 "ntp_parser.y"

	char *			String;
	double			Double;
	int			Integer;
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
#line 511 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 523 "ntp_parser.c"

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
#define YYFINAL  189
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   631

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  181
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  74
/* YYNRULES -- Number of rules.  */
#define YYNRULES  263
/* YYNRULES -- Number of states.  */
#define YYNSTATES  406

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   430

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     177,   178,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   176,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   179,     2,   180,     2,     2,     2,     2,
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
     175
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    15,    16,    18,    20,
      22,    24,    26,    28,    30,    32,    34,    36,    38,    40,
      44,    47,    49,    51,    53,    55,    57,    59,    62,    65,
      67,    70,    72,    74,    76,    78,    81,    84,    87,    89,
      91,    93,    95,    97,   100,   103,   106,   109,   111,   113,
     115,   118,   121,   124,   127,   130,   133,   136,   139,   142,
     145,   148,   151,   153,   154,   157,   160,   163,   166,   169,
     172,   175,   178,   181,   183,   186,   189,   192,   195,   198,
     201,   204,   207,   210,   213,   216,   219,   222,   226,   229,
     231,   233,   235,   237,   239,   241,   243,   245,   247,   250,
     252,   255,   258,   260,   262,   264,   266,   268,   270,   272,
     274,   276,   278,   280,   283,   286,   290,   296,   300,   305,
     310,   314,   315,   318,   320,   322,   324,   326,   328,   330,
     332,   334,   336,   338,   340,   342,   344,   346,   349,   351,
     354,   357,   360,   363,   365,   368,   371,   374,   377,   380,
     383,   386,   389,   393,   396,   398,   401,   404,   407,   410,
     413,   416,   419,   422,   425,   428,   431,   433,   435,   437,
     439,   441,   443,   445,   447,   450,   453,   455,   458,   461,
     464,   467,   470,   473,   476,   478,   482,   484,   487,   490,
     493,   496,   499,   502,   505,   508,   511,   514,   517,   520,
     524,   527,   530,   532,   535,   536,   541,   545,   548,   550,
     553,   556,   559,   561,   563,   567,   571,   573,   575,   577,
     579,   581,   583,   585,   587,   589,   592,   594,   597,   599,
     601,   603,   609,   612,   614,   617,   619,   621,   623,   625,
     627,   629,   635,   637,   641,   644,   648,   650,   652,   655,
     657,   663,   668,   672,   675,   677,   684,   688,   691,   695,
     697,   699,   701,   703
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     182,     0,    -1,   183,    -1,   183,   184,   165,    -1,   184,
     165,    -1,     1,   165,    -1,    -1,   185,    -1,   191,    -1,
     193,    -1,   194,    -1,   200,    -1,   206,    -1,   197,    -1,
     213,    -1,   216,    -1,   219,    -1,   222,    -1,   241,    -1,
     186,   187,   189,    -1,   186,   187,    -1,   133,    -1,   119,
      -1,   114,    -1,    12,    -1,    78,    -1,   188,    -1,    61,
     143,    -1,    63,   143,    -1,   143,    -1,   189,   190,    -1,
     190,    -1,     7,    -1,    15,    -1,    49,    -1,    65,    57,
      -1,    91,    57,    -1,    86,    57,    -1,   104,    -1,   121,
      -1,   122,    -1,   152,    -1,   162,    -1,   154,    57,    -1,
      93,    57,    -1,   158,    57,    -1,   192,   187,    -1,   156,
      -1,   157,    -1,    13,    -1,    79,   238,    -1,    97,   238,
      -1,     8,    57,    -1,    21,    57,    -1,    22,   195,    -1,
      66,   143,    -1,    67,   143,    -1,   129,    57,    -1,   131,
      57,    -1,   153,   234,    -1,   110,   143,    -1,   195,   196,
      -1,   196,    -1,    -1,    47,   143,    -1,    50,   143,    -1,
     124,   143,    -1,   126,   143,    -1,   135,   143,    -1,    26,
     143,    -1,   131,    57,    -1,   150,   198,    -1,   198,   199,
      -1,   199,    -1,    18,    57,    -1,    44,    57,    -1,    20,
     239,    -1,   111,    57,    -1,   112,    57,    -1,    89,   240,
      -1,    84,   240,    -1,    87,   240,    -1,    82,   240,    -1,
      92,    57,    -1,    11,    57,    -1,   137,   201,    -1,   139,
     143,    -1,    38,   202,   203,    -1,   201,   202,    -1,   202,
      -1,    19,    -1,    23,    -1,    76,    -1,   115,    -1,   127,
      -1,   144,    -1,   148,    -1,   123,    -1,   203,   204,    -1,
     204,    -1,    37,   143,    -1,   155,   205,    -1,    72,    -1,
      99,    -1,    34,    -1,    27,    -1,   101,    -1,   117,    -1,
      24,    -1,   160,    -1,    95,    -1,   163,    -1,     3,    -1,
      28,   209,    -1,    96,   211,    -1,   130,   187,   207,    -1,
     130,   188,    80,   188,   207,    -1,   130,    25,   207,    -1,
     130,    61,    25,   207,    -1,   130,    63,    25,   207,    -1,
     130,   136,   207,    -1,    -1,   207,   208,    -1,    43,    -1,
      51,    -1,    68,    -1,    69,    -1,    71,    -1,    77,    -1,
     100,    -1,   102,    -1,   103,    -1,   105,    -1,   106,    -1,
     107,    -1,   109,    -1,   158,    -1,   209,   210,    -1,   210,
      -1,     9,    57,    -1,    90,    57,    -1,    94,    57,    -1,
     211,   212,    -1,   212,    -1,    52,    57,    -1,    53,    57,
      -1,    54,    57,    -1,    55,    57,    -1,    81,    57,    -1,
      83,    57,    -1,    85,    57,    -1,    88,    57,    -1,    46,
     187,   214,    -1,   214,   215,    -1,   215,    -1,   146,   240,
      -1,   147,   240,    -1,   142,    57,    -1,   128,   143,    -1,
      39,   239,    -1,    40,   239,    -1,    41,   239,    -1,    42,
     239,    -1,    34,   217,    -1,    27,   217,    -1,   217,   218,
      -1,   218,    -1,     6,    -1,    10,    -1,    16,    -1,    64,
      -1,    94,    -1,   108,    -1,   138,    -1,   149,   220,    -1,
     220,   221,    -1,   221,    -1,     5,   240,    -1,    29,   240,
      -1,    45,   240,    -1,    48,   240,    -1,   113,   240,    -1,
     140,   240,    -1,   141,   240,    -1,   229,    -1,    56,   143,
     184,    -1,    35,    -1,    14,   240,    -1,    17,    57,    -1,
     145,   240,    -1,    31,   223,    -1,    70,   143,    -1,   118,
     143,    -1,    75,   143,    -1,    74,   227,    -1,   116,   237,
      -1,   132,   143,    -1,   134,   224,    -1,   151,   188,    -1,
     151,   188,   225,    -1,   154,   233,    -1,   125,   143,    -1,
     143,    -1,   143,    30,    -1,    -1,   143,   176,   143,    25,
      -1,   143,   176,   143,    -1,   225,   226,    -1,   226,    -1,
     120,    57,    -1,    58,   188,    -1,   227,   228,    -1,   228,
      -1,   143,    -1,   230,   232,   231,    -1,   230,   232,   143,
      -1,    58,    -1,    98,    -1,     4,    -1,    60,    -1,    62,
      -1,   161,    -1,    73,    -1,    51,    -1,    32,    -1,   233,
      57,    -1,    57,    -1,   234,   235,    -1,   235,    -1,    57,
      -1,   236,    -1,   177,    57,    33,    57,   178,    -1,   237,
     143,    -1,   143,    -1,   238,   187,    -1,   187,    -1,    57,
      -1,   152,    -1,    36,    -1,    57,    -1,    30,    -1,   242,
     179,   243,   246,   180,    -1,   166,    -1,   243,   244,   165,
      -1,   244,   165,    -1,   245,   176,   240,    -1,   167,    -1,
     168,    -1,   246,   247,    -1,   247,    -1,   249,   179,   248,
     250,   180,    -1,   169,   176,   240,   165,    -1,   133,   176,
     187,    -1,   250,   251,    -1,   251,    -1,   170,   176,   240,
     179,   252,   180,    -1,   252,   253,   165,    -1,   253,   165,
      -1,   254,   176,   240,    -1,   171,    -1,   172,    -1,   173,
      -1,   174,    -1,   175,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   315,   315,   319,   320,   321,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   355,
     363,   374,   375,   376,   377,   378,   382,   383,   385,   390,
     395,   397,   402,   403,   404,   405,   406,   407,   408,   409,
     410,   411,   412,   413,   414,   415,   424,   434,   435,   445,
     447,   449,   460,   467,   469,   474,   476,   478,   480,   482,
     484,   489,   494,   500,   507,   509,   511,   513,   515,   517,
     519,   536,   541,   546,   554,   556,   558,   560,   562,   564,
     566,   568,   570,   572,   574,   584,   586,   595,   605,   610,
     618,   619,   620,   621,   622,   623,   624,   625,   629,   634,
     642,   652,   661,   670,   679,   680,   684,   685,   686,   687,
     688,   689,   690,   699,   703,   707,   715,   723,   731,   746,
     761,   774,   777,   785,   786,   787,   788,   789,   790,   791,
     792,   793,   794,   795,   796,   797,   798,   802,   807,   815,
     816,   817,   821,   826,   834,   835,   836,   837,   838,   839,
     840,   841,   849,   859,   864,   872,   873,   874,   875,   876,
     877,   878,   879,   887,   889,   894,   899,   907,   908,   909,
     910,   911,   912,   913,   929,   934,   939,   947,   948,   949,
     950,   951,   952,   953,   962,   963,   978,   984,   991,   998,
    1005,  1007,  1014,  1021,  1033,  1035,  1037,  1049,  1051,  1058,
    1065,  1067,  1077,  1084,  1094,  1103,  1105,  1110,  1115,  1123,
    1125,  1133,  1138,  1146,  1171,  1178,  1188,  1189,  1193,  1194,
    1195,  1196,  1200,  1201,  1202,  1212,  1217,  1225,  1230,  1238,
    1240,  1244,  1249,  1254,  1262,  1267,  1275,  1284,  1285,  1289,
    1290,  1299,  1314,  1318,  1323,  1331,  1336,  1337,  1341,  1346,
    1354,  1359,  1364,  1369,  1374,  1382,  1387,  1392,  1400,  1405,
    1406,  1407,  1408,  1409
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_Age", "T_All", "T_Allan", "T_Auth",
  "T_Autokey", "T_Automax", "T_Average", "T_Bclient", "T_Beacon",
  "T_Broadcast", "T_Broadcastclient", "T_Broadcastdelay", "T_Burst",
  "T_Calibrate", "T_Calldelay", "T_Ceiling", "T_Clockstats", "T_Cohort",
  "T_ControlKey", "T_Crypto", "T_Cryptostats", "T_Day", "T_Default",
  "T_Digest", "T_Disable", "T_Discard", "T_Dispersion", "T_Double",
  "T_Driftfile", "T_Drop", "T_Ellipsis", "T_Enable", "T_End", "T_False",
  "T_File", "T_Filegen", "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4",
  "T_Flake", "T_Floor", "T_Freq", "T_Fudge", "T_Host", "T_Huffpuff",
  "T_Iburst", "T_Ident", "T_Ignore", "T_Incalloc", "T_Incmem",
  "T_Initalloc", "T_Initmem", "T_Includefile", "T_Integer", "T_Interface",
  "T_Intrange", "T_Ipv4", "T_Ipv4_flag", "T_Ipv6", "T_Ipv6_flag",
  "T_Kernel", "T_Key", "T_Keys", "T_Keysdir", "T_Kod", "T_Mssntp",
  "T_Leapfile", "T_Limited", "T_Link", "T_Listen", "T_Logconfig",
  "T_Logfile", "T_Loopstats", "T_Lowpriotrap", "T_Manycastclient",
  "T_Manycastserver", "T_Mask", "T_Maxage", "T_Maxclock", "T_Maxdepth",
  "T_Maxdist", "T_Maxmem", "T_Maxpoll", "T_Minclock", "T_Mindepth",
  "T_Mindist", "T_Minimum", "T_Minpoll", "T_Minsane", "T_Mode",
  "T_Monitor", "T_Month", "T_Mru", "T_Multicastclient", "T_Nic",
  "T_Nolink", "T_Nomodify", "T_None", "T_Nopeer", "T_Noquery",
  "T_Noselect", "T_Noserve", "T_Notrap", "T_Notrust", "T_Ntp", "T_Ntpport",
  "T_NtpSignDsocket", "T_Orphan", "T_Orphanwait", "T_Panic", "T_Peer",
  "T_Peerstats", "T_Phone", "T_Pid", "T_Pidfile", "T_Pool", "T_Port",
  "T_Preempt", "T_Prefer", "T_Protostats", "T_Pw", "T_Qos", "T_Randfile",
  "T_Rawstats", "T_Refid", "T_Requestkey", "T_Restrict", "T_Revoke",
  "T_Saveconfigdir", "T_Server", "T_Setvar", "T_Sign", "T_Source",
  "T_Statistics", "T_Stats", "T_Statsdir", "T_Step", "T_Stepout",
  "T_Stratum", "T_String", "T_Sysstats", "T_Tick", "T_Time1", "T_Time2",
  "T_Timingstats", "T_Tinker", "T_Tos", "T_Trap", "T_True", "T_Trustedkey",
  "T_Ttl", "T_Type", "T_Unconfig", "T_Unpeer", "T_Version",
  "T_WanderThreshold", "T_Week", "T_Wildcard", "T_Xleave", "T_Year",
  "T_Flag", "T_EOC", "T_Simulate", "T_Beep_Delay", "T_Sim_Duration",
  "T_Server_Offset", "T_Duration", "T_Freq_Offset", "T_Wander", "T_Jitter",
  "T_Prop_Delay", "T_Proc_Delay", "'='", "'('", "')'", "'{'", "'}'",
  "$accept", "configuration", "command_list", "command", "server_command",
  "client_type", "address", "ip_address", "option_list", "option",
  "unpeer_command", "unpeer_keyword", "other_mode_command",
  "authentication_command", "crypto_command_list", "crypto_command",
  "orphan_mode_command", "tos_option_list", "tos_option",
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
  "integer_list_range", "integer_list_range_elt", "integer_range",
  "string_list", "address_list", "boolean", "number", "simulate_command",
  "sim_conf_start", "sim_init_statement_list", "sim_init_statement",
  "sim_init_keyword", "sim_server_list", "sim_server", "sim_server_offset",
  "sim_server_name", "sim_act_list", "sim_act", "sim_act_stmt_list",
  "sim_act_stmt", "sim_act_keyword", 0
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
     425,   426,   427,   428,   429,   430,    61,    40,    41,   123,
     125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   181,   182,   183,   183,   183,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   185,
     185,   186,   186,   186,   186,   186,   187,   187,   187,   188,
     189,   189,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   191,   192,   192,   193,
     193,   193,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   195,   195,   195,   196,   196,   196,   196,   196,   196,
     196,   197,   198,   198,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   200,   200,   200,   201,   201,
     202,   202,   202,   202,   202,   202,   202,   202,   203,   203,
     204,   204,   204,   204,   204,   204,   205,   205,   205,   205,
     205,   205,   205,   206,   206,   206,   206,   206,   206,   206,
     206,   207,   207,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   209,   209,   210,
     210,   210,   211,   211,   212,   212,   212,   212,   212,   212,
     212,   212,   213,   214,   214,   215,   215,   215,   215,   215,
     215,   215,   215,   216,   216,   217,   217,   218,   218,   218,
     218,   218,   218,   218,   219,   220,   220,   221,   221,   221,
     221,   221,   221,   221,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   223,   223,   223,   224,   224,   225,   225,   226,
     226,   227,   227,   228,   229,   229,   230,   230,   231,   231,
     231,   231,   232,   232,   232,   233,   233,   234,   234,   235,
     235,   236,   237,   237,   238,   238,   239,   239,   239,   240,
     240,   241,   242,   243,   243,   244,   245,   245,   246,   246,
     247,   248,   249,   250,   250,   251,   252,   252,   253,   254,
     254,   254,   254,   254
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     1,     1,     1,     1,     1,     1,     2,     2,     1,
       2,     1,     1,     1,     1,     2,     2,     2,     1,     1,
       1,     1,     1,     2,     2,     2,     2,     1,     1,     1,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     1,     0,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     3,     5,     3,     4,     4,
       3,     0,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       2,     2,     2,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     3,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     1,     3,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     3,
       2,     2,     1,     2,     0,     4,     3,     2,     1,     2,
       2,     2,     1,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     2,     1,     1,
       1,     5,     2,     1,     2,     1,     1,     1,     1,     1,
       1,     5,     1,     3,     2,     3,     1,     1,     2,     1,
       5,     4,     3,     2,     1,     6,     3,     2,     3,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,    24,    49,     0,     0,     0,    63,     0,
       0,   204,     0,   186,     0,     0,     0,   216,     0,     0,
       0,     0,     0,    25,     0,     0,     0,   217,     0,    23,
       0,     0,    22,     0,     0,     0,     0,     0,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    47,    48,
     242,     0,     2,     0,     7,     0,     8,     0,     9,    10,
      13,    11,    12,    14,    15,    16,    17,   184,     0,    18,
       0,     5,    52,   240,   239,   187,   188,    53,     0,     0,
       0,     0,     0,     0,     0,    54,    62,   167,   168,   169,
     170,   171,   172,   173,   164,   166,     0,     0,     0,   113,
     138,   202,   190,   163,    90,    91,    92,    93,    97,    94,
      95,    96,     0,     0,     0,    29,     0,    26,     6,    55,
      56,   191,   213,   194,   212,   193,   235,    50,     0,     0,
       0,     0,     0,     0,     0,     0,   114,   143,    51,    60,
     233,   195,   192,   201,    57,   121,     0,     0,   121,   121,
      26,    58,   196,     0,   197,    85,    89,    86,   189,     0,
       0,     0,     0,     0,     0,     0,   174,   176,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    71,
      73,   198,   229,     0,    59,   228,   230,   226,   200,     1,
       0,     4,    20,    46,   224,   223,   222,     0,     0,    69,
      64,    65,    66,    67,    70,    68,    61,   165,   139,   140,
     141,   137,   203,   105,   104,     0,   102,   103,     0,    87,
      99,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,   152,   154,   185,   211,   234,   144,   145,   146,   147,
     148,   149,   150,   151,   142,   232,   117,   121,   121,   120,
     115,     0,     0,    88,   177,   178,   179,   180,   181,   182,
     183,   175,    84,    74,   238,   236,   237,    76,    75,    82,
      80,    81,    79,    83,    77,    78,    72,     0,     0,   199,
     208,     0,   227,   225,     3,    32,    33,    34,     0,     0,
       0,     0,    38,    39,    40,    41,     0,     0,    42,    19,
      31,   218,   219,   220,   215,   221,   214,   246,   247,     0,
       0,     0,   100,   112,   108,   110,   106,   107,   109,   111,
     101,    98,   159,   160,   161,   162,   158,   157,   155,   156,
     153,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   122,   118,   119,   121,   206,
     210,   209,   207,     0,    35,    37,    36,    44,    43,    45,
      30,     0,     0,     0,   249,     0,   244,     0,   116,   205,
       0,     0,   243,   241,   248,     0,   245,   231,   252,     0,
       0,     0,     0,     0,   254,     0,     0,   250,   253,   251,
       0,     0,   259,   260,   261,   262,   263,     0,     0,     0,
     255,     0,   257,     0,   256,   258
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    51,    52,    53,    54,    55,   126,   117,   299,   300,
      56,    57,    58,    59,    85,    86,    60,   179,   180,    61,
     155,   112,   219,   220,   320,    62,   246,   345,    99,   100,
     136,   137,    63,   231,   232,    64,    94,    95,    65,   166,
     167,    66,   102,   154,   279,   280,   123,   124,    67,    68,
     306,   197,   188,   184,   185,   186,   141,   127,   267,    75,
      69,    70,   309,   310,   311,   363,   364,   380,   365,   383,
     384,   397,   398,   399
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -176
static const yytype_int16 yypact[] =
{
     127,  -136,   -25,  -176,  -176,   -10,   -23,   -22,    60,     0,
      -1,  -103,     0,  -176,   152,   -37,   -99,  -176,   -98,   -97,
     -88,   -86,   -84,  -176,   -37,    16,   -37,  -176,   -81,  -176,
     -76,   -67,  -176,   -65,    22,    -7,    23,   -62,  -176,   -60,
     152,   -55,   -10,    37,   519,   -53,   -52,    34,  -176,  -176,
    -176,    95,   289,   -69,  -176,   -37,  -176,   -37,  -176,  -176,
    -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,   -15,  -176,
     -79,  -176,  -176,  -176,  -176,  -176,  -176,  -176,   -38,   -34,
     -32,   -31,   -29,    41,   -28,    60,  -176,  -176,  -176,  -176,
    -176,  -176,  -176,  -176,     0,  -176,    59,    96,    99,    -1,
    -176,   104,  -176,     0,  -176,  -176,  -176,  -176,  -176,  -176,
    -176,  -176,   -12,    14,    17,  -176,   246,  -176,   436,  -176,
    -176,  -176,  -176,   -86,  -176,  -176,  -176,   -37,   102,   109,
     112,   115,   117,   119,   122,   123,    16,  -176,   -37,  -176,
    -176,    39,  -176,  -176,  -176,  -176,   -16,    -6,  -176,  -176,
     110,  -176,  -176,    13,  -176,   152,  -176,  -176,  -176,   -10,
     -10,   -10,   -10,   -10,   -10,   -10,    37,  -176,   135,   139,
      -5,   143,   -10,   -10,   -10,   -10,   147,   150,   151,   519,
    -176,   -45,  -176,   154,   -52,  -176,  -176,  -176,   155,  -176,
      44,  -176,    77,  -176,  -176,  -176,  -176,     3,  -140,  -176,
    -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,
    -176,  -176,  -176,  -176,  -176,    71,  -176,  -176,    50,   -12,
    -176,  -176,  -176,    -5,    -5,    -5,    -5,    72,   159,   -10,
     -10,   246,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,
    -176,  -176,  -176,  -176,  -176,  -176,   418,  -176,  -176,   418,
     418,   -53,    74,  -176,  -176,  -176,  -176,  -176,  -176,  -176,
    -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,
    -176,  -176,  -176,  -176,  -176,  -176,  -176,   -53,   162,   -45,
    -176,   187,  -176,  -176,  -176,  -176,  -176,  -176,   164,   165,
     169,   170,  -176,  -176,  -176,  -176,   173,   175,  -176,    77,
    -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,   -95,
      68,    58,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,
    -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,
    -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,
    -176,  -176,  -176,  -176,  -176,  -176,   418,   418,  -176,   211,
    -176,  -176,  -176,   181,  -176,  -176,  -176,  -176,  -176,  -176,
    -176,    64,    88,  -119,  -176,    63,  -176,   -10,   418,  -176,
      84,   -37,  -176,  -176,  -176,    85,  -176,  -176,  -176,    79,
      93,   -10,    89,  -147,  -176,   103,   -10,  -176,  -176,  -176,
      90,    76,  -176,  -176,  -176,  -176,  -176,   157,   105,    97,
    -176,   106,  -176,   -10,  -176,  -176
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -176,  -176,  -176,   -41,  -176,  -176,   -14,   -33,  -176,   -17,
    -176,  -176,  -176,  -176,  -176,   189,  -176,  -176,   111,  -176,
    -176,    -3,  -176,    70,  -176,  -176,  -145,  -176,  -176,   192,
    -176,   158,  -176,  -176,    67,  -176,   283,   -64,  -176,  -176,
     133,  -176,  -176,  -176,  -176,    25,  -176,   182,  -176,  -176,
    -176,  -176,  -176,  -176,   124,  -176,  -176,   281,  -175,   -42,
    -176,  -176,  -176,     4,  -176,  -176,   -54,  -176,  -176,  -176,
     -71,  -176,   -83,  -176
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     158,   116,   150,   249,   250,   182,    87,   301,    96,   247,
      88,   190,   181,   277,   361,   213,    89,   194,   145,   248,
      73,   149,   214,   382,   113,   215,   114,   307,   308,    71,
     207,   264,    72,   387,    76,    77,   195,   156,   361,   207,
     101,   192,   159,   193,   118,   119,   120,    74,   322,   323,
     324,   325,   265,   313,   146,   121,   147,   122,   196,   125,
     216,   373,   139,   302,    90,   303,   160,   140,   128,   129,
     130,   131,   307,   308,   314,   278,   142,   233,   143,   144,
     151,   152,   161,   153,   285,   162,    78,   217,   157,    97,
     115,   187,   286,    98,    91,   189,   191,   132,   204,   133,
     198,   134,   346,   347,   135,   199,   115,    79,    92,   200,
      80,   201,   202,   235,   203,   205,   208,   254,   255,   256,
     257,   258,   259,   260,   235,   183,   287,   221,     1,   148,
     269,   270,   271,   272,   212,     2,   115,   222,    93,     3,
       4,     5,   288,   218,     6,   315,   304,   266,     7,     8,
     163,   316,   253,   209,     9,    10,   210,   221,    11,   236,
     222,    12,    13,   289,   305,    14,   237,   317,   290,   238,
     291,   104,   239,    15,   240,   105,   241,   164,   165,   242,
     243,   292,   245,    16,    81,    17,    82,   328,   329,   252,
     251,    83,   262,    18,    19,    84,   263,    20,   293,   294,
     268,    21,    22,   368,   273,    23,    24,   274,   275,   284,
     318,   281,   283,   319,   312,   326,   327,   349,   348,   351,
     353,   354,   355,    25,    26,    27,   356,   357,   106,   295,
     358,   296,   359,   366,   367,   297,   369,    28,   370,   298,
     371,    29,   375,    30,   350,    31,    32,   392,   393,   394,
     395,   396,    33,   372,   379,   381,    34,    35,    36,    37,
      38,    39,   377,   382,    40,   386,    41,   107,   389,   391,
     402,   404,    42,   403,   206,   108,    43,    44,    45,   109,
      46,    47,   360,    48,    49,   223,   224,   225,   226,   321,
     276,   211,    -6,    50,   244,   103,   110,     2,   330,   261,
     111,     3,     4,     5,   352,   234,     6,   138,   282,   374,
       7,     8,   388,   362,   401,     0,     9,    10,     0,     0,
      11,     0,     0,    12,    13,   376,     0,    14,   392,   393,
     394,   395,   396,     0,     0,    15,     0,   400,     0,   385,
       0,     0,     0,     0,   390,    16,     0,    17,     0,     0,
       0,     0,     0,     0,     0,    18,    19,   378,     0,    20,
       0,   405,     0,    21,    22,     0,     0,    23,    24,     0,
       0,     0,     0,     0,   227,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    25,    26,    27,   228,     0,
       0,     0,   229,   230,     0,     0,     0,     0,     0,    28,
       0,     0,     0,    29,     0,    30,     0,    31,    32,     0,
       0,     0,     0,     0,    33,     0,     0,     0,    34,    35,
      36,    37,    38,    39,     0,     0,    40,     0,    41,     0,
       0,     0,     0,     0,    42,     0,     0,     0,    43,    44,
      45,     0,    46,    47,     2,    48,    49,     0,     3,     4,
       5,     0,     0,     6,    -6,    50,     0,     7,     8,     0,
       0,   331,     0,     9,    10,     0,     0,    11,     0,   332,
      12,    13,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,    15,     0,     0,     0,   333,   334,     0,   335,
       0,     0,    16,     0,    17,   336,     0,     0,     0,     0,
       0,     0,    18,    19,     0,     0,    20,     0,     0,     0,
      21,    22,     0,     0,    23,    24,     0,     0,   337,     0,
     338,   339,     0,   340,   341,   342,     0,   343,     0,     0,
     168,     0,    25,    26,    27,     0,     0,   169,     0,   170,
       0,     0,     0,     0,     0,     0,    28,     0,     0,     0,
      29,     0,    30,     0,    31,    32,     0,     0,     0,     0,
       0,    33,     0,   171,     0,    34,    35,    36,    37,    38,
      39,     0,     0,    40,     0,    41,   344,     0,     0,     0,
       0,    42,     0,     0,     0,    43,    44,    45,     0,    46,
      47,     0,    48,    49,     0,     0,     0,     0,     0,     0,
       0,   172,    50,   173,     0,     0,   174,     0,   175,     0,
       0,   176,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     177,   178
};

static const yytype_int16 yycheck[] =
{
      42,    15,    35,   148,   149,    57,     6,     4,     9,    25,
      10,    52,    45,    58,   133,    27,    16,    32,    25,    25,
      30,    35,    34,   170,    61,    37,    63,   167,   168,   165,
      94,    36,    57,   180,    57,    57,    51,    40,   133,   103,
     143,    55,     5,    57,   143,   143,   143,    57,   223,   224,
     225,   226,    57,     3,    61,   143,    63,   143,    73,   143,
      72,   180,   143,    60,    64,    62,    29,   143,    52,    53,
      54,    55,   167,   168,    24,   120,   143,   118,   143,    57,
      57,   143,    45,   143,     7,    48,    26,    99,   143,    90,
     143,    57,    15,    94,    94,     0,   165,    81,    57,    83,
     179,    85,   247,   248,    88,   143,   143,    47,   108,   143,
      50,   143,   143,   127,   143,   143,    57,   159,   160,   161,
     162,   163,   164,   165,   138,   177,    49,   143,     1,   136,
     172,   173,   174,   175,    30,     8,   143,   143,   138,    12,
      13,    14,    65,   155,    17,    95,   143,   152,    21,    22,
     113,   101,   155,    57,    27,    28,    57,   143,    31,    57,
     143,    34,    35,    86,   161,    38,    57,   117,    91,    57,
      93,    19,    57,    46,    57,    23,    57,   140,   141,    57,
      57,   104,   143,    56,   124,    58,   126,   229,   230,   176,
      80,   131,    57,    66,    67,   135,    57,    70,   121,   122,
      57,    74,    75,   348,    57,    78,    79,    57,    57,   165,
     160,    57,    57,   163,   143,   143,    57,   143,   251,    57,
      33,    57,    57,    96,    97,    98,    57,    57,    76,   152,
      57,   154,    57,   165,   176,   158,    25,   110,    57,   162,
     176,   114,   179,   116,   277,   118,   119,   171,   172,   173,
     174,   175,   125,   165,   169,   176,   129,   130,   131,   132,
     133,   134,   178,   170,   137,   176,   139,   115,   165,   179,
     165,   165,   145,   176,    85,   123,   149,   150,   151,   127,
     153,   154,   299,   156,   157,    39,    40,    41,    42,   219,
     179,    99,   165,   166,   136,    12,   144,     8,   231,   166,
     148,    12,    13,    14,   279,   123,    17,    26,   184,   363,
      21,    22,   383,   309,   397,    -1,    27,    28,    -1,    -1,
      31,    -1,    -1,    34,    35,   367,    -1,    38,   171,   172,
     173,   174,   175,    -1,    -1,    46,    -1,   180,    -1,   381,
      -1,    -1,    -1,    -1,   386,    56,    -1,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,   371,    -1,    70,
      -1,   403,    -1,    74,    75,    -1,    -1,    78,    79,    -1,
      -1,    -1,    -1,    -1,   128,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    96,    97,    98,   142,    -1,
      -1,    -1,   146,   147,    -1,    -1,    -1,    -1,    -1,   110,
      -1,    -1,    -1,   114,    -1,   116,    -1,   118,   119,    -1,
      -1,    -1,    -1,    -1,   125,    -1,    -1,    -1,   129,   130,
     131,   132,   133,   134,    -1,    -1,   137,    -1,   139,    -1,
      -1,    -1,    -1,    -1,   145,    -1,    -1,    -1,   149,   150,
     151,    -1,   153,   154,     8,   156,   157,    -1,    12,    13,
      14,    -1,    -1,    17,   165,   166,    -1,    21,    22,    -1,
      -1,    43,    -1,    27,    28,    -1,    -1,    31,    -1,    51,
      34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    -1,    68,    69,    -1,    71,
      -1,    -1,    56,    -1,    58,    77,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    67,    -1,    -1,    70,    -1,    -1,    -1,
      74,    75,    -1,    -1,    78,    79,    -1,    -1,   100,    -1,
     102,   103,    -1,   105,   106,   107,    -1,   109,    -1,    -1,
      11,    -1,    96,    97,    98,    -1,    -1,    18,    -1,    20,
      -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,    -1,    -1,
     114,    -1,   116,    -1,   118,   119,    -1,    -1,    -1,    -1,
      -1,   125,    -1,    44,    -1,   129,   130,   131,   132,   133,
     134,    -1,    -1,   137,    -1,   139,   158,    -1,    -1,    -1,
      -1,   145,    -1,    -1,    -1,   149,   150,   151,    -1,   153,
     154,    -1,   156,   157,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,   166,    84,    -1,    -1,    87,    -1,    89,    -1,
      -1,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     111,   112
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     8,    12,    13,    14,    17,    21,    22,    27,
      28,    31,    34,    35,    38,    46,    56,    58,    66,    67,
      70,    74,    75,    78,    79,    96,    97,    98,   110,   114,
     116,   118,   119,   125,   129,   130,   131,   132,   133,   134,
     137,   139,   145,   149,   150,   151,   153,   154,   156,   157,
     166,   182,   183,   184,   185,   186,   191,   192,   193,   194,
     197,   200,   206,   213,   216,   219,   222,   229,   230,   241,
     242,   165,    57,    30,    57,   240,    57,    57,    26,    47,
      50,   124,   126,   131,   135,   195,   196,     6,    10,    16,
      64,    94,   108,   138,   217,   218,     9,    90,    94,   209,
     210,   143,   223,   217,    19,    23,    76,   115,   123,   127,
     144,   148,   202,    61,    63,   143,   187,   188,   143,   143,
     143,   143,   143,   227,   228,   143,   187,   238,    52,    53,
      54,    55,    81,    83,    85,    88,   211,   212,   238,   143,
     143,   237,   143,   143,    57,    25,    61,    63,   136,   187,
     188,    57,   143,   143,   224,   201,   202,   143,   240,     5,
      29,    45,    48,   113,   140,   141,   220,   221,    11,    18,
      20,    44,    82,    84,    87,    89,    92,   111,   112,   198,
     199,   188,    57,   177,   234,   235,   236,    57,   233,     0,
     184,   165,   187,   187,    32,    51,    73,   232,   179,   143,
     143,   143,   143,   143,    57,   143,   196,   218,    57,    57,
      57,   210,    30,    27,    34,    37,    72,    99,   155,   203,
     204,   143,   143,    39,    40,    41,    42,   128,   142,   146,
     147,   214,   215,   184,   228,   187,    57,    57,    57,    57,
      57,    57,    57,    57,   212,   143,   207,    25,    25,   207,
     207,    80,   176,   202,   240,   240,   240,   240,   240,   240,
     240,   221,    57,    57,    36,    57,   152,   239,    57,   240,
     240,   240,   240,    57,    57,    57,   199,    58,   120,   225,
     226,    57,   235,    57,   165,     7,    15,    49,    65,    86,
      91,    93,   104,   121,   122,   152,   154,   158,   162,   189,
     190,     4,    60,    62,   143,   161,   231,   167,   168,   243,
     244,   245,   143,     3,    24,    95,   101,   117,   160,   163,
     205,   204,   239,   239,   239,   239,   143,    57,   240,   240,
     215,    43,    51,    68,    69,    71,    77,   100,   102,   103,
     105,   106,   107,   109,   158,   208,   207,   207,   188,   143,
     188,    57,   226,    33,    57,    57,    57,    57,    57,    57,
     190,   133,   244,   246,   247,   249,   165,   176,   207,    25,
      57,   176,   165,   180,   247,   179,   240,   178,   187,   169,
     248,   176,   170,   250,   251,   240,   176,   180,   251,   165,
     240,   179,   171,   172,   173,   174,   175,   252,   253,   254,
     180,   253,   165,   176,   165,   240
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
#line 322 "ntp_parser.y"
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
#line 356 "ntp_parser.y"
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			if (my_node)
				APPEND_G_FIFO(cfgt.peers, my_node);
		}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 364 "ntp_parser.y"
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
			if (my_node)
				APPEND_G_FIFO(cfgt.peers, my_node);
		}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 384 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 386 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET6); }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 391 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_UNSPEC); }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 396 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = append_gen_fifo((yyvsp[(1) - (2)].Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 398 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = append_gen_fifo(NULL, (yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 402 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 403 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 404 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 405 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 406 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 407 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 408 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 409 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 410 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 411 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 412 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 413 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 414 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 415 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 425 "ntp_parser.y"
    {
			unpeer_node *my_node;
			
			my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
		}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 446 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 448 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.manycastserver, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 450 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.multicastclient, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 461 "ntp_parser.y"
    {
			attr_val *atrv;
			
			atrv = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			APPEND_G_FIFO(cfgt.vars, atrv);
		}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 468 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 470 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			CONCAT_G_FIFOS(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 475 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 477 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 479 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 481 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 483 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Attr_val_fifo); }
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 485 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 490 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 495 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 500 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), NULL);
		}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 508 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 510 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 512 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 514 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 516 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 518 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 520 "ntp_parser.y"
    {
			(yyval.Attr_val) = NULL;
			cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer);
			msyslog(LOG_WARNING,
				"'crypto revoke %d' is deprecated, "
				"please use 'revoke %d' instead.",
				cfgt.auth.revoke, cfgt.auth.revoke);
		}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 537 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.orphan_cmds, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 542 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 547 "ntp_parser.y"
    {	
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 555 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 557 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 559 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 561 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 563 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 565 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 567 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 569 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 571 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 573 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 575 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 585 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.stats_list, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 587 "ntp_parser.y"
    {
			if (input_from_file) {
				cfgt.stats_dir = (yyvsp[(2) - (2)].String);
			} else {
				YYFREE((yyvsp[(2) - (2)].String));
				yyerror("statsdir remote configuration ignored");
			}
		}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 596 "ntp_parser.y"
    {
			filegen_node *fgn;
			
			fgn = create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.filegen_opts, fgn);
		}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 606 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 611 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 630 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 635 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 643 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			else {
				(yyval.Attr_val) = NULL;
				YYFREE((yyvsp[(2) - (2)].String));
				yyerror("filegen file remote configuration ignored");
			}
		}
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 653 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen type remote configuration ignored");
			}
		}
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 662 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen link remote configuration ignored");
			}
		}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 671 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen nolink remote configuration ignored");
			}
		}
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 679 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 680 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 113:

/* Line 1464 of yacc.c  */
#line 700 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.discard_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 704 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.mru_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 708 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 716 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 724 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 732 "ntp_parser.y"
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

  case 119:

/* Line 1464 of yacc.c  */
#line 747 "ntp_parser.y"
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

  case 120:

/* Line 1464 of yacc.c  */
#line 762 "ntp_parser.y"
    {
			restrict_node *	rn;

			append_gen_fifo((yyvsp[(3) - (3)].Int_fifo), create_int_node((yyvsp[(2) - (3)].Integer)));
			rn = create_restrict_node(
				NULL, NULL, (yyvsp[(3) - (3)].Int_fifo), ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 774 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
		}
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 778 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 803 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 808 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 815 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 816 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 817 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 822 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 827 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 834 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 835 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 836 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 837 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 838 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 839 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 840 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 841 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 850 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.fudge, aon);
		}
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 860 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 865 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 872 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 873 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 157:

/* Line 1464 of yacc.c  */
#line 874 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 158:

/* Line 1464 of yacc.c  */
#line 875 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 159:

/* Line 1464 of yacc.c  */
#line 876 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 160:

/* Line 1464 of yacc.c  */
#line 877 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 161:

/* Line 1464 of yacc.c  */
#line 878 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 879 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 888 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.enable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 890 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.disable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 895 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 900 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 907 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 908 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 909 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 910 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 911 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 912 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 914 "ntp_parser.y"
    { 
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				yyerror("enable/disable stats remote configuration ignored");
			}
		}
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 930 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.tinker, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 935 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 940 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 177:

/* Line 1464 of yacc.c  */
#line 947 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 948 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 949 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 950 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 951 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 952 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 953 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 185:

/* Line 1464 of yacc.c  */
#line 964 "ntp_parser.y"
    {
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

  case 186:

/* Line 1464 of yacc.c  */
#line 979 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 187:

/* Line 1464 of yacc.c  */
#line 985 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 188:

/* Line 1464 of yacc.c  */
#line 992 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 189:

/* Line 1464 of yacc.c  */
#line 999 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 190:

/* Line 1464 of yacc.c  */
#line 1006 "ntp_parser.y"
    { /* see drift_parm below for actions */ }
    break;

  case 191:

/* Line 1464 of yacc.c  */
#line 1008 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 192:

/* Line 1464 of yacc.c  */
#line 1015 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 193:

/* Line 1464 of yacc.c  */
#line 1022 "ntp_parser.y"
    {
			attr_val *av;

			if (input_from_file) {
				av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
				APPEND_G_FIFO(cfgt.vars, av);
			} else {
				YYFREE((yyvsp[(2) - (2)].String));
				yyerror("logfile remote configuration ignored");
			}
		}
    break;

  case 194:

/* Line 1464 of yacc.c  */
#line 1034 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.logconfig, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 195:

/* Line 1464 of yacc.c  */
#line 1036 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.phone, (yyvsp[(2) - (2)].String_fifo)); }
    break;

  case 196:

/* Line 1464 of yacc.c  */
#line 1038 "ntp_parser.y"
    {
			attr_val *av;

			if (input_from_file) {
				av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
				APPEND_G_FIFO(cfgt.vars, av);
			} else {
				YYFREE((yyvsp[(2) - (2)].String));
				yyerror("saveconfigdir remote configuration ignored");
			}
		}
    break;

  case 197:

/* Line 1464 of yacc.c  */
#line 1050 "ntp_parser.y"
    { APPEND_G_FIFO(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 198:

/* Line 1464 of yacc.c  */
#line 1052 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (2)].Address_node), NULL);
			APPEND_G_FIFO(cfgt.trap, aon);
		}
    break;

  case 199:

/* Line 1464 of yacc.c  */
#line 1059 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.trap, aon);
		}
    break;

  case 200:

/* Line 1464 of yacc.c  */
#line 1066 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.ttl, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 201:

/* Line 1464 of yacc.c  */
#line 1068 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			APPEND_G_FIFO(cfgt.qos, av);
		}
    break;

  case 202:

/* Line 1464 of yacc.c  */
#line 1078 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 203:

/* Line 1464 of yacc.c  */
#line 1085 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
			av = create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 204:

/* Line 1464 of yacc.c  */
#line 1094 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, "");
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 1104 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 1106 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (3)].String), (yyvsp[(3) - (3)].String), 0); }
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 1111 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 1116 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 1124 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 1126 "ntp_parser.y"
    {
			(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), estrdup((yyvsp[(2) - (2)].Address_node)->address));
			destroy_address_node((yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 211:

/* Line 1464 of yacc.c  */
#line 1134 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 212:

/* Line 1464 of yacc.c  */
#line 1139 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 213:

/* Line 1464 of yacc.c  */
#line 1147 "ntp_parser.y"
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

  case 214:

/* Line 1464 of yacc.c  */
#line 1172 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 215:

/* Line 1464 of yacc.c  */
#line 1179 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 1213 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 1218 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 1226 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1231 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 1239 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[(1) - (1)].Integer)); }
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 1245 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_rangeval('-', (yyvsp[(2) - (5)].Integer), (yyvsp[(4) - (5)].Integer)); }
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 1250 "ntp_parser.y"
    {
			(yyval.String_fifo) = (yyvsp[(1) - (2)].String_fifo);
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(2) - (2)].String)));
		}
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 1255 "ntp_parser.y"
    {
			(yyval.String_fifo) = NULL;
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(1) - (1)].String)));
		}
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 1263 "ntp_parser.y"
    {
			(yyval.Address_fifo) = (yyvsp[(1) - (2)].Address_fifo);
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 1268 "ntp_parser.y"
    {
			(yyval.Address_fifo) = NULL;
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(1) - (1)].Address_node));
		}
    break;

  case 236:

/* Line 1464 of yacc.c  */
#line 1276 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			} else {
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
			}
		}
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 1284 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 238:

/* Line 1464 of yacc.c  */
#line 1285 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 239:

/* Line 1464 of yacc.c  */
#line 1289 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 241:

/* Line 1464 of yacc.c  */
#line 1300 "ntp_parser.y"
    {
			cfgt.sim_details = create_sim_node((yyvsp[(3) - (5)].Attr_val_fifo), (yyvsp[(4) - (5)].Sim_server_fifo));

			/* Reset the old_config_style variable */
			old_config_style = 1;
		}
    break;

  case 242:

/* Line 1464 of yacc.c  */
#line 1314 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 243:

/* Line 1464 of yacc.c  */
#line 1319 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 1324 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 245:

/* Line 1464 of yacc.c  */
#line 1332 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 1342 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = (yyvsp[(1) - (2)].Sim_server_fifo);
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(2) - (2)].Sim_server));
		}
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 1347 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(1) - (1)].Sim_server));
		}
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 1355 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Sim_script_fifo)); }
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 1360 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 1365 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 1370 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = (yyvsp[(1) - (2)].Sim_script_fifo);
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(2) - (2)].Sim_script));
		}
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 1375 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(1) - (1)].Sim_script));
		}
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 1383 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Attr_val_fifo)); }
    break;

  case 256:

/* Line 1464 of yacc.c  */
#line 1388 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 257:

/* Line 1464 of yacc.c  */
#line 1393 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 1401 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1464 of yacc.c  */
#line 3973 "ntp_parser.c"
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
#line 1412 "ntp_parser.y"


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


