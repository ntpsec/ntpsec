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
  void yyerror(const char *msg);
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
     T_Ceiling = 272,
     T_Clockstats = 273,
     T_Cohort = 274,
     T_ControlKey = 275,
     T_Crypto = 276,
     T_Cryptostats = 277,
     T_Day = 278,
     T_Default = 279,
     T_Digest = 280,
     T_Disable = 281,
     T_Discard = 282,
     T_Dispersion = 283,
     T_Double = 284,
     T_Driftfile = 285,
     T_Drop = 286,
     T_Ellipsis = 287,
     T_Enable = 288,
     T_End = 289,
     T_False = 290,
     T_File = 291,
     T_Filegen = 292,
     T_Flag1 = 293,
     T_Flag2 = 294,
     T_Flag3 = 295,
     T_Flag4 = 296,
     T_Flake = 297,
     T_Floor = 298,
     T_Freq = 299,
     T_Fudge = 300,
     T_Host = 301,
     T_Huffpuff = 302,
     T_Iburst = 303,
     T_Ident = 304,
     T_Ignore = 305,
     T_Incalloc = 306,
     T_Incmem = 307,
     T_Initalloc = 308,
     T_Initmem = 309,
     T_Includefile = 310,
     T_Integer = 311,
     T_Interface = 312,
     T_Intrange = 313,
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
     T_Orphanwait = 366,
     T_Panic = 367,
     T_Peer = 368,
     T_Peerstats = 369,
     T_Phone = 370,
     T_Pid = 371,
     T_Pidfile = 372,
     T_Pool = 373,
     T_Port = 374,
     T_Preempt = 375,
     T_Prefer = 376,
     T_Protostats = 377,
     T_Pw = 378,
     T_Qos = 379,
     T_Randfile = 380,
     T_Rawstats = 381,
     T_Refid = 382,
     T_Requestkey = 383,
     T_Restrict = 384,
     T_Revoke = 385,
     T_Saveconfigdir = 386,
     T_Server = 387,
     T_Setvar = 388,
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
     T_EOC = 418,
     T_Simulate = 419,
     T_Beep_Delay = 420,
     T_Sim_Duration = 421,
     T_Server_Offset = 422,
     T_Duration = 423,
     T_Freq_Offset = 424,
     T_Wander = 425,
     T_Jitter = 426,
     T_Prop_Delay = 427,
     T_Proc_Delay = 428
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
#define T_Ceiling 272
#define T_Clockstats 273
#define T_Cohort 274
#define T_ControlKey 275
#define T_Crypto 276
#define T_Cryptostats 277
#define T_Day 278
#define T_Default 279
#define T_Digest 280
#define T_Disable 281
#define T_Discard 282
#define T_Dispersion 283
#define T_Double 284
#define T_Driftfile 285
#define T_Drop 286
#define T_Ellipsis 287
#define T_Enable 288
#define T_End 289
#define T_False 290
#define T_File 291
#define T_Filegen 292
#define T_Flag1 293
#define T_Flag2 294
#define T_Flag3 295
#define T_Flag4 296
#define T_Flake 297
#define T_Floor 298
#define T_Freq 299
#define T_Fudge 300
#define T_Host 301
#define T_Huffpuff 302
#define T_Iburst 303
#define T_Ident 304
#define T_Ignore 305
#define T_Incalloc 306
#define T_Incmem 307
#define T_Initalloc 308
#define T_Initmem 309
#define T_Includefile 310
#define T_Integer 311
#define T_Interface 312
#define T_Intrange 313
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
#define T_Orphanwait 366
#define T_Panic 367
#define T_Peer 368
#define T_Peerstats 369
#define T_Phone 370
#define T_Pid 371
#define T_Pidfile 372
#define T_Pool 373
#define T_Port 374
#define T_Preempt 375
#define T_Prefer 376
#define T_Protostats 377
#define T_Pw 378
#define T_Qos 379
#define T_Randfile 380
#define T_Rawstats 381
#define T_Refid 382
#define T_Requestkey 383
#define T_Restrict 384
#define T_Revoke 385
#define T_Saveconfigdir 386
#define T_Server 387
#define T_Setvar 388
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
#define T_EOC 418
#define T_Simulate 419
#define T_Beep_Delay 420
#define T_Sim_Duration 421
#define T_Server_Offset 422
#define T_Duration 423
#define T_Freq_Offset 424
#define T_Wander 425
#define T_Jitter 426
#define T_Prop_Delay 427
#define T_Proc_Delay 428




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
#line 507 "ntp_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 519 "ntp_parser.c"

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
#define YYFINAL  188
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   604

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  179
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  94
/* YYNRULES -- Number of rules.  */
#define YYNRULES  280
/* YYNRULES -- Number of states.  */
#define YYNSTATES  379

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   428

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     175,   176,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   174,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   177,     2,   178,     2,     2,     2,     2,
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
     165,   166,   167,   168,   169,   170,   171,   172,   173
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    15,    16,    18,    20,
      22,    24,    26,    28,    30,    32,    34,    36,    38,    40,
      44,    47,    49,    51,    53,    55,    57,    59,    62,    64,
      66,    68,    71,    73,    75,    77,    79,    81,    83,    85,
      87,    89,    91,    93,    95,    98,   100,   102,   104,   106,
     108,   110,   113,   115,   117,   119,   122,   125,   128,   131,
     134,   137,   140,   143,   146,   149,   152,   155,   157,   158,
     161,   164,   166,   168,   170,   172,   174,   177,   180,   182,
     185,   188,   191,   193,   195,   197,   199,   201,   203,   205,
     207,   209,   211,   214,   217,   221,   224,   226,   228,   230,
     232,   234,   236,   238,   240,   242,   243,   246,   249,   252,
     254,   256,   258,   260,   262,   264,   266,   268,   270,   272,
     274,   276,   278,   281,   284,   288,   294,   298,   303,   308,
     312,   313,   316,   318,   320,   322,   324,   326,   328,   330,
     332,   334,   336,   338,   340,   342,   344,   347,   349,   352,
     354,   356,   358,   361,   363,   366,   368,   370,   372,   374,
     376,   378,   380,   382,   386,   389,   391,   394,   397,   400,
     403,   405,   407,   409,   411,   413,   415,   418,   421,   424,
     426,   428,   430,   432,   434,   436,   438,   440,   442,   445,
     448,   450,   453,   455,   457,   459,   461,   463,   465,   467,
     469,   472,   475,   478,   482,   484,   487,   490,   493,   496,
     500,   503,   505,   507,   509,   511,   513,   515,   517,   519,
     522,   523,   528,   530,   531,   532,   535,   538,   541,   544,
     546,   548,   552,   556,   558,   560,   562,   564,   566,   568,
     570,   572,   574,   577,   579,   582,   584,   586,   588,   594,
     597,   599,   602,   604,   606,   608,   610,   612,   614,   620,
     622,   626,   629,   633,   635,   637,   640,   642,   648,   653,
     657,   660,   662,   669,   673,   676,   680,   682,   684,   686,
     688
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     180,     0,    -1,   181,    -1,   181,   182,   163,    -1,   182,
     163,    -1,     1,   163,    -1,    -1,   183,    -1,   194,    -1,
     196,    -1,   197,    -1,   206,    -1,   214,    -1,   201,    -1,
     223,    -1,   228,    -1,   232,    -1,   236,    -1,   259,    -1,
     184,   185,   188,    -1,   184,   185,    -1,   132,    -1,   118,
      -1,   113,    -1,    12,    -1,    77,    -1,   186,    -1,   187,
     141,    -1,   141,    -1,    60,    -1,    62,    -1,   188,   189,
      -1,   189,    -1,   190,    -1,   192,    -1,   191,    -1,     7,
      -1,    15,    -1,    48,    -1,   103,    -1,   120,    -1,   121,
      -1,   150,    -1,   160,    -1,   193,    56,    -1,    64,    -1,
      90,    -1,    85,    -1,   152,    -1,    92,    -1,   156,    -1,
     195,   185,    -1,   154,    -1,   155,    -1,    13,    -1,    78,
     256,    -1,    96,   256,    -1,     8,    56,    -1,    20,    56,
      -1,    21,   198,    -1,    65,   141,    -1,    66,   141,    -1,
     128,    56,    -1,   130,    56,    -1,   151,   252,    -1,   109,
     141,    -1,   198,   199,    -1,   199,    -1,    -1,   200,   141,
      -1,   130,    56,    -1,    46,    -1,    49,    -1,   123,    -1,
     125,    -1,    25,    -1,   148,   202,    -1,   202,   203,    -1,
     203,    -1,   204,    56,    -1,   205,   258,    -1,    19,   257,
      -1,    17,    -1,    43,    -1,   110,    -1,   111,    -1,    91,
      -1,    11,    -1,    88,    -1,    83,    -1,    86,    -1,    81,
      -1,   135,   207,    -1,   137,   141,    -1,    37,   208,   209,
      -1,   207,   208,    -1,   208,    -1,    18,    -1,    22,    -1,
      75,    -1,   114,    -1,   126,    -1,   142,    -1,   146,    -1,
     122,    -1,    -1,   209,   210,    -1,    36,   141,    -1,   153,
     213,    -1,   211,    -1,   212,    -1,    71,    -1,    98,    -1,
      33,    -1,    26,    -1,   100,    -1,   116,    -1,    23,    -1,
     158,    -1,    94,    -1,   161,    -1,     3,    -1,    27,   217,
      -1,    95,   220,    -1,   129,   185,   215,    -1,   129,   186,
      79,   186,   215,    -1,   129,    24,   215,    -1,   129,    60,
      24,   215,    -1,   129,    62,    24,   215,    -1,   129,   134,
     215,    -1,    -1,   215,   216,    -1,    42,    -1,    50,    -1,
      67,    -1,    68,    -1,    70,    -1,    76,    -1,    99,    -1,
     101,    -1,   102,    -1,   104,    -1,   105,    -1,   106,    -1,
     108,    -1,   156,    -1,   217,   218,    -1,   218,    -1,   219,
      56,    -1,     9,    -1,    89,    -1,    93,    -1,   220,   221,
      -1,   221,    -1,   222,    56,    -1,    51,    -1,    52,    -1,
      53,    -1,    54,    -1,    80,    -1,    82,    -1,    84,    -1,
      87,    -1,    45,   185,   224,    -1,   224,   225,    -1,   225,
      -1,   226,   258,    -1,   227,   257,    -1,   140,    56,    -1,
     127,   141,    -1,   144,    -1,   145,    -1,    38,    -1,    39,
      -1,    40,    -1,    41,    -1,    33,   229,    -1,    26,   229,
      -1,   229,   230,    -1,   230,    -1,   231,    -1,   136,    -1,
       6,    -1,    10,    -1,    16,    -1,    63,    -1,    93,    -1,
     107,    -1,   147,   233,    -1,   233,   234,    -1,   234,    -1,
     235,   258,    -1,     5,    -1,    28,    -1,    44,    -1,    47,
      -1,   112,    -1,   138,    -1,   139,    -1,   247,    -1,   237,
     258,    -1,   238,   141,    -1,   239,   141,    -1,    55,   141,
     182,    -1,    34,    -1,    30,   240,    -1,    73,   245,    -1,
     115,   255,    -1,   133,   241,    -1,   149,   186,   243,    -1,
     152,   251,    -1,    14,    -1,   143,    -1,    69,    -1,   117,
      -1,   124,    -1,    74,    -1,   131,    -1,   141,    -1,   141,
      29,    -1,    -1,   141,   174,   141,   242,    -1,    24,    -1,
      -1,    -1,   243,   244,    -1,   119,    56,    -1,    57,   186,
      -1,   245,   246,    -1,   246,    -1,   141,    -1,   248,   250,
     249,    -1,   248,   250,   141,    -1,    57,    -1,    97,    -1,
       4,    -1,    59,    -1,    61,    -1,   159,    -1,    72,    -1,
      50,    -1,    31,    -1,   251,    56,    -1,    56,    -1,   252,
     253,    -1,   253,    -1,    56,    -1,   254,    -1,   175,    56,
      32,    56,   176,    -1,   255,   141,    -1,   141,    -1,   256,
     185,    -1,   185,    -1,    56,    -1,   150,    -1,    35,    -1,
      56,    -1,    29,    -1,   260,   177,   261,   264,   178,    -1,
     164,    -1,   261,   262,   163,    -1,   262,   163,    -1,   263,
     174,   258,    -1,   165,    -1,   166,    -1,   264,   265,    -1,
     265,    -1,   267,   177,   266,   268,   178,    -1,   167,   174,
     258,   163,    -1,   132,   174,   185,    -1,   268,   269,    -1,
     269,    -1,   168,   174,   258,   177,   270,   178,    -1,   270,
     271,   163,    -1,   271,   163,    -1,   272,   174,   258,    -1,
     169,    -1,   170,    -1,   171,    -1,   172,    -1,   173,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   333,   333,   337,   338,   339,   353,   354,   355,   356,
     357,   358,   359,   360,   361,   362,   363,   364,   365,   373,
     380,   390,   391,   392,   393,   394,   398,   399,   404,   409,
     411,   416,   418,   423,   424,   428,   433,   434,   435,   436,
     437,   438,   439,   440,   444,   449,   450,   451,   452,   453,
     454,   462,   472,   473,   483,   485,   487,   498,   505,   507,
     512,   514,   516,   518,   520,   522,   527,   532,   538,   545,
     547,   559,   560,   561,   562,   563,   572,   577,   582,   590,
     592,   594,   600,   601,   602,   603,   604,   605,   609,   610,
     611,   612,   621,   623,   632,   642,   647,   655,   656,   657,
     658,   659,   660,   661,   662,   667,   670,   678,   688,   697,
     712,   717,   718,   722,   723,   728,   729,   730,   731,   732,
     733,   734,   743,   747,   751,   759,   767,   775,   790,   805,
     818,   821,   829,   830,   831,   832,   833,   834,   835,   836,
     837,   838,   839,   840,   841,   842,   846,   851,   859,   864,
     865,   866,   870,   875,   883,   888,   889,   890,   891,   892,
     893,   894,   895,   903,   913,   918,   926,   928,   930,   932,
     937,   938,   942,   943,   944,   945,   953,   955,   960,   965,
     973,   975,   987,   988,   989,   990,   991,   992,  1000,  1005,
    1010,  1018,  1023,  1024,  1025,  1026,  1027,  1028,  1029,  1038,
    1039,  1046,  1053,  1069,  1088,  1093,  1095,  1097,  1099,  1101,
    1108,  1113,  1114,  1118,  1119,  1120,  1124,  1125,  1129,  1136,
    1146,  1155,  1160,  1162,  1167,  1170,  1178,  1180,  1188,  1193,
    1201,  1226,  1233,  1243,  1244,  1248,  1249,  1250,  1251,  1255,
    1256,  1257,  1267,  1272,  1280,  1285,  1293,  1295,  1299,  1304,
    1309,  1317,  1322,  1330,  1339,  1340,  1344,  1345,  1354,  1372,
    1376,  1381,  1389,  1394,  1395,  1399,  1404,  1412,  1417,  1422,
    1427,  1432,  1440,  1445,  1450,  1458,  1463,  1464,  1465,  1466,
    1467
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
  "T_Calibrate", "T_Ceiling", "T_Clockstats", "T_Cohort", "T_ControlKey",
  "T_Crypto", "T_Cryptostats", "T_Day", "T_Default", "T_Digest",
  "T_Disable", "T_Discard", "T_Dispersion", "T_Double", "T_Driftfile",
  "T_Drop", "T_Ellipsis", "T_Enable", "T_End", "T_False", "T_File",
  "T_Filegen", "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4", "T_Flake",
  "T_Floor", "T_Freq", "T_Fudge", "T_Host", "T_Huffpuff", "T_Iburst",
  "T_Ident", "T_Ignore", "T_Incalloc", "T_Incmem", "T_Initalloc",
  "T_Initmem", "T_Includefile", "T_Integer", "T_Interface", "T_Intrange",
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
  "T_Source", "T_Statistics", "T_Stats", "T_Statsdir", "T_Step",
  "T_Stepout", "T_Stratum", "T_String", "T_Sysstats", "T_Tick", "T_Time1",
  "T_Time2", "T_Timingstats", "T_Tinker", "T_Tos", "T_Trap", "T_True",
  "T_Trustedkey", "T_Ttl", "T_Type", "T_Unconfig", "T_Unpeer", "T_Version",
  "T_WanderThreshold", "T_Week", "T_Wildcard", "T_Xleave", "T_Year",
  "T_Flag", "T_EOC", "T_Simulate", "T_Beep_Delay", "T_Sim_Duration",
  "T_Server_Offset", "T_Duration", "T_Freq_Offset", "T_Wander", "T_Jitter",
  "T_Prop_Delay", "T_Proc_Delay", "'='", "'('", "')'", "'{'", "'}'",
  "$accept", "configuration", "command_list", "command", "server_command",
  "client_type", "address", "ip_address", "address_fam", "option_list",
  "option", "option_flag", "option_flag_keyword", "option_int",
  "option_int_keyword", "unpeer_command", "unpeer_keyword",
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
  "system_option_flag_keyword", "tinker_command", "tinker_option_list",
  "tinker_option", "tinker_option_keyword", "miscellaneous_command",
  "misc_cmd_dbl_keyword", "misc_cmd_str_keyword",
  "misc_cmd_str_lcl_keyword", "drift_parm", "variable_assign",
  "t_default_or_zero", "trap_option_list", "trap_option",
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
     425,   426,   427,   428,    61,    40,    41,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   179,   180,   181,   181,   181,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   183,
     183,   184,   184,   184,   184,   184,   185,   185,   186,   187,
     187,   188,   188,   189,   189,   190,   191,   191,   191,   191,
     191,   191,   191,   191,   192,   193,   193,   193,   193,   193,
     193,   194,   195,   195,   196,   196,   196,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   198,   198,   198,   199,
     199,   200,   200,   200,   200,   200,   201,   202,   202,   203,
     203,   203,   204,   204,   204,   204,   204,   204,   205,   205,
     205,   205,   206,   206,   206,   207,   207,   208,   208,   208,
     208,   208,   208,   208,   208,   209,   209,   210,   210,   210,
     210,   211,   211,   212,   212,   213,   213,   213,   213,   213,
     213,   213,   214,   214,   214,   214,   214,   214,   214,   214,
     215,   215,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   217,   217,   218,   219,
     219,   219,   220,   220,   221,   222,   222,   222,   222,   222,
     222,   222,   222,   223,   224,   224,   225,   225,   225,   225,
     226,   226,   227,   227,   227,   227,   228,   228,   229,   229,
     230,   230,   231,   231,   231,   231,   231,   231,   232,   233,
     233,   234,   235,   235,   235,   235,   235,   235,   235,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   237,   237,   238,   238,   238,   239,   239,   240,   240,
     240,   241,   242,   242,   243,   243,   244,   244,   245,   245,
     246,   247,   247,   248,   248,   249,   249,   249,   249,   250,
     250,   250,   251,   251,   252,   252,   253,   253,   254,   255,
     255,   256,   256,   257,   257,   257,   258,   258,   259,   260,
     261,   261,   262,   263,   263,   264,   264,   265,   266,   267,
     268,   268,   269,   270,   270,   271,   272,   272,   272,   272,
     272
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     0,     2,
       2,     1,     1,     1,     1,     1,     2,     2,     1,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     2,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     3,     5,     3,     4,     4,     3,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     1,
       1,     1,     2,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     2,     1,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     3,     1,     2,     2,     2,     2,     3,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       0,     4,     1,     0,     0,     2,     2,     2,     2,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     2,     1,     1,     1,     5,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     5,     1,
       3,     2,     3,     1,     1,     2,     1,     5,     4,     3,
       2,     1,     6,     3,     2,     3,     1,     1,     1,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,    24,    54,   211,     0,    68,     0,     0,
     220,     0,   204,     0,     0,     0,   233,     0,     0,   213,
       0,   216,    25,     0,     0,     0,   234,     0,    23,     0,
     214,    22,   215,     0,     0,     0,   217,    21,     0,     0,
       0,   212,     0,     0,     0,     0,     0,    52,    53,   259,
       0,     2,     0,     7,     0,     8,     0,     9,    10,    13,
      11,    12,    14,    15,    16,    17,     0,     0,     0,   199,
       0,    18,     0,     5,    57,    58,    75,    71,    72,    73,
      74,     0,    59,    67,     0,   182,   183,   184,   185,   186,
     187,   181,   177,   179,   180,   149,   150,   151,   122,   147,
       0,   218,   205,   176,    97,    98,    99,   100,   104,   101,
     102,   103,   105,    29,    30,    28,     0,    26,     0,     6,
      60,    61,   230,   206,   229,   252,    55,   155,   156,   157,
     158,   159,   160,   161,   162,   123,   153,     0,    56,    65,
     250,   207,    62,   130,    29,    30,   130,   130,    26,    63,
       0,   208,    92,    96,    93,   192,   193,   194,   195,   196,
     197,   198,   188,   190,     0,    87,    82,     0,    83,    91,
      89,    90,    88,    86,    84,    85,    76,    78,     0,     0,
     224,   246,     0,    64,   245,   247,   243,   210,     1,     0,
       4,    20,    51,   257,   256,   200,   201,   202,   241,   240,
     239,     0,     0,    70,    66,    69,   178,   146,   148,   219,
      94,   172,   173,   174,   175,     0,     0,   170,   171,   163,
     165,     0,     0,    27,   203,   228,   251,   152,   154,   249,
     126,   130,   130,   129,   124,     0,     0,    95,   189,   191,
     255,   253,   254,    81,    77,    79,    80,   209,     0,   244,
     242,     3,    36,    37,    38,    45,    47,    46,    49,    39,
      40,    41,    42,    48,    50,    43,    19,    32,    33,    35,
      34,     0,   235,   236,   237,   232,   238,   231,   263,   264,
       0,     0,     0,   114,   113,     0,   111,   112,     0,   106,
     109,   110,   169,   168,   164,   166,   167,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   131,   127,   128,   130,   223,     0,     0,   225,     0,
      31,    44,     0,     0,     0,   266,     0,   261,     0,   107,
     121,   117,   119,   115,   116,   118,   120,   108,   125,   222,
     221,   227,   226,     0,     0,   260,   258,   265,     0,   262,
     248,   269,     0,     0,     0,     0,     0,   271,     0,     0,
     267,   270,   268,     0,     0,   276,   277,   278,   279,   280,
       0,     0,     0,   272,     0,   274,     0,   273,   275
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    50,    51,    52,    53,    54,   125,   117,   118,   266,
     267,   268,   269,   270,   271,    55,    56,    57,    58,    82,
      83,    84,    59,   176,   177,   178,   179,    60,   152,   112,
     210,   289,   290,   291,   337,    61,   230,   311,    98,    99,
     100,   135,   136,   137,    62,   219,   220,   221,   222,    63,
      92,    93,    94,    64,   162,   163,   164,    65,    66,    67,
      68,   102,   151,   340,   247,   318,   123,   124,    69,    70,
     277,   201,   187,   183,   184,   185,   141,   126,   243,   195,
      71,    72,   280,   281,   282,   324,   325,   353,   326,   356,
     357,   370,   371,   372
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -163
static const yytype_int16 yypact[] =
{
     108,  -138,   -21,  -163,  -163,  -163,   -19,    90,    18,     9,
    -102,    18,  -163,    26,   -46,   -98,  -163,   -96,   -95,  -163,
     -94,  -163,  -163,   -46,   399,   -46,  -163,   -88,  -163,   -84,
    -163,  -163,  -163,     4,    17,     6,  -163,  -163,   -78,    26,
     -77,  -163,     5,   493,   -75,   -49,    12,  -163,  -163,  -163,
      70,   266,   -91,  -163,   -46,  -163,   -46,  -163,  -163,  -163,
    -163,  -163,  -163,  -163,  -163,  -163,    -6,   -68,   -67,  -163,
     -18,  -163,   -99,  -163,  -163,  -163,  -163,  -163,  -163,  -163,
    -163,    19,    90,  -163,   -61,  -163,  -163,  -163,  -163,  -163,
    -163,  -163,    18,  -163,  -163,  -163,  -163,  -163,     9,  -163,
      31,    61,  -163,    18,  -163,  -163,  -163,  -163,  -163,  -163,
    -163,  -163,  -163,  -163,  -163,  -163,   168,  -163,   -47,   411,
    -163,  -163,  -163,   -94,  -163,  -163,   -46,  -163,  -163,  -163,
    -163,  -163,  -163,  -163,  -163,   399,  -163,    36,   -46,  -163,
    -163,   -42,  -163,  -163,    69,    76,  -163,  -163,    27,  -163,
     -70,  -163,    26,  -163,  -163,  -163,  -163,  -163,  -163,  -163,
    -163,  -163,     5,  -163,    -6,  -163,  -163,   -20,  -163,  -163,
    -163,  -163,  -163,  -163,  -163,  -163,   493,  -163,    51,    -6,
    -163,  -163,    52,   -49,  -163,  -163,  -163,    54,  -163,   -36,
    -163,    98,  -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,
    -163,     8,  -136,  -163,  -163,  -163,  -163,  -163,  -163,  -163,
      25,  -163,  -163,  -163,  -163,   -27,    75,  -163,  -163,   168,
    -163,    -6,   -20,  -163,  -163,  -163,  -163,  -163,  -163,  -163,
     248,  -163,  -163,   248,   248,   -75,    -9,  -163,  -163,  -163,
    -163,  -163,  -163,  -163,  -163,  -163,  -163,   -48,   101,  -163,
    -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,
    -163,  -163,  -163,  -163,  -163,  -163,    98,  -163,  -163,  -163,
    -163,    81,  -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,
    -110,   -16,   -24,  -163,  -163,    14,  -163,  -163,     3,  -163,
    -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,
    -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,
    -163,  -163,   248,   248,  -163,   132,   -75,   103,  -163,   104,
    -163,  -163,   -17,     7,  -113,  -163,    -8,  -163,    -6,  -163,
    -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,   248,  -163,
    -163,  -163,  -163,    -1,   -46,  -163,  -163,  -163,    13,  -163,
    -163,  -163,     2,    11,    -6,    10,  -147,  -163,    24,    -6,
    -163,  -163,  -163,    16,    58,  -163,  -163,  -163,  -163,  -163,
     -87,    28,    15,  -163,    32,  -163,    -6,  -163,  -163
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -163,  -163,  -163,   -43,  -163,  -163,   -14,   -33,  -163,  -163,
     -72,  -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,  -163,
     114,  -163,  -163,  -163,    22,  -163,  -163,  -163,  -163,   -34,
    -163,  -163,  -163,  -163,  -163,  -163,  -143,  -163,  -163,   102,
    -163,  -163,    64,  -163,  -163,  -163,    -7,  -163,  -163,  -163,
     199,   -65,  -163,  -163,  -163,    49,  -163,  -163,  -163,  -163,
    -163,  -163,  -163,  -163,  -163,  -163,  -163,    93,  -163,  -163,
    -163,  -163,  -163,  -163,    39,  -163,  -163,   208,    20,  -162,
    -163,  -163,  -163,   -56,  -163,  -163,   -90,  -163,  -163,  -163,
    -121,  -163,  -126,  -163
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
     116,   148,   239,   233,   234,   153,   330,   181,   189,   316,
     155,   180,   272,   198,   113,   240,   114,   246,    95,   322,
     147,   355,   322,   193,    85,    73,   331,   206,    86,   278,
     279,   360,   199,   156,    87,    74,   241,    75,   206,   101,
     191,   143,   192,   119,   104,   120,   121,   122,   105,   157,
     194,   283,   158,   139,   200,   278,   279,   140,   284,   295,
     142,   285,   149,   150,   154,   346,   115,   273,   186,   274,
     188,   317,   190,   196,   197,   203,   224,   144,   202,   145,
     205,    88,   365,   366,   367,   368,   369,   208,   312,   313,
     209,   373,   228,   231,   223,   115,   286,   332,    96,   229,
     232,   106,    97,   333,   236,   252,   235,   245,   248,     1,
     250,    89,   226,   253,   292,    76,     2,   159,   237,   334,
       3,     4,     5,   287,   226,    90,   182,   251,     6,     7,
     242,   293,   315,   319,     8,     9,    77,   321,    10,    78,
     107,    11,    12,   160,   161,    13,   254,   327,   108,   275,
     328,   146,   109,    14,    91,   329,   339,   344,   115,   342,
     343,   335,   255,    15,   336,    16,   349,   276,   110,   348,
     345,   338,   111,    17,    18,   350,   354,    19,   288,   355,
     352,    20,    21,   256,   359,    22,    23,   362,   257,   376,
     258,   375,   358,   364,   320,   377,   204,   363,   244,   227,
     207,   259,   314,    24,    25,    26,   211,   212,   213,   214,
     103,   238,   294,    79,   378,    80,   225,    27,   260,   261,
      81,    28,   249,    29,   323,    30,    31,   365,   366,   367,
     368,   369,    32,   138,   347,   361,    33,    34,    35,    36,
      37,    38,   296,    39,   374,    40,     0,     0,   262,     0,
     263,    41,     0,     0,   264,    42,    43,    44,   265,    45,
      46,     0,    47,    48,     0,     0,     0,     0,     0,     0,
       0,    -6,    49,     0,     2,     0,     0,     0,     3,     4,
       5,     0,     0,   341,     0,     0,     6,     7,     0,     0,
     297,     0,     8,     9,     0,   215,    10,     0,   298,    11,
      12,     0,     0,    13,     0,     0,     0,     0,   216,     0,
       0,    14,   217,   218,     0,   299,   300,     0,   301,     0,
       0,    15,     0,    16,   302,     0,     0,     0,     0,     0,
     351,    17,    18,     0,     0,    19,     0,     0,     0,    20,
      21,     0,     0,    22,    23,     0,     0,   303,     0,   304,
     305,     0,   306,   307,   308,     0,   309,     0,     0,     0,
       0,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    27,     0,     0,     0,    28,
       0,    29,     0,    30,    31,     0,     0,     0,     0,     0,
      32,     0,     0,     0,    33,    34,    35,    36,    37,    38,
       0,    39,     0,    40,   310,     0,     0,     0,     0,    41,
       0,     0,     0,    42,    43,    44,     0,    45,    46,     2,
      47,    48,     0,     3,     4,     5,     0,     0,     0,    -6,
      49,     6,     7,     0,     0,     0,     0,     8,     9,     0,
       0,    10,     0,     0,    11,    12,     0,     0,    13,     0,
     127,   128,   129,   130,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,     0,    16,     0,
       0,     0,     0,     0,     0,     0,    17,    18,     0,   131,
      19,   132,     0,   133,    20,    21,   134,     0,    22,    23,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   165,     0,    24,    25,    26,     0,
     166,     0,   167,     0,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,    28,     0,    29,     0,    30,    31,
       0,     0,     0,     0,     0,    32,   168,     0,     0,    33,
      34,    35,    36,    37,    38,     0,    39,     0,    40,     0,
       0,     0,     0,     0,    41,     0,     0,     0,    42,    43,
      44,     0,    45,    46,     0,    47,    48,     0,     0,     0,
       0,     0,     0,     0,   169,    49,   170,     0,     0,   171,
       0,   172,     0,     0,   173,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   174,   175
};

static const yytype_int16 yycheck[] =
{
      14,    34,   164,   146,   147,    39,     3,    56,    51,    57,
       5,    44,     4,    31,    60,    35,    62,   179,     9,   132,
      34,   168,   132,    29,     6,   163,    23,    92,    10,   165,
     166,   178,    50,    28,    16,    56,    56,    56,   103,   141,
      54,    24,    56,   141,    18,   141,   141,   141,    22,    44,
      56,    26,    47,   141,    72,   165,   166,   141,    33,   221,
      56,    36,    56,   141,   141,   178,   141,    59,    56,    61,
       0,   119,   163,   141,   141,    56,   119,    60,   177,    62,
     141,    63,   169,   170,   171,   172,   173,    56,   231,   232,
      29,   178,    56,    24,   141,   141,    71,    94,    89,   141,
      24,    75,    93,   100,   174,     7,    79,    56,    56,     1,
      56,    93,   126,    15,   141,    25,     8,   112,   152,   116,
      12,    13,    14,    98,   138,   107,   175,   163,    20,    21,
     150,    56,   141,    32,    26,    27,    46,    56,    30,    49,
     114,    33,    34,   138,   139,    37,    48,   163,   122,   141,
     174,   134,   126,    45,   136,   141,    24,   174,   141,    56,
      56,   158,    64,    55,   161,    57,   328,   159,   142,   177,
     163,   314,   146,    65,    66,   176,   174,    69,   153,   168,
     167,    73,    74,    85,   174,    77,    78,   163,    90,   174,
      92,   163,   354,   177,   266,   163,    82,   359,   176,   135,
      98,   103,   235,    95,    96,    97,    38,    39,    40,    41,
      11,   162,   219,   123,   376,   125,   123,   109,   120,   121,
     130,   113,   183,   115,   280,   117,   118,   169,   170,   171,
     172,   173,   124,    25,   324,   356,   128,   129,   130,   131,
     132,   133,   222,   135,   370,   137,    -1,    -1,   150,    -1,
     152,   143,    -1,    -1,   156,   147,   148,   149,   160,   151,
     152,    -1,   154,   155,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   163,   164,    -1,     8,    -1,    -1,    -1,    12,    13,
      14,    -1,    -1,   316,    -1,    -1,    20,    21,    -1,    -1,
      42,    -1,    26,    27,    -1,   127,    30,    -1,    50,    33,
      34,    -1,    -1,    37,    -1,    -1,    -1,    -1,   140,    -1,
      -1,    45,   144,   145,    -1,    67,    68,    -1,    70,    -1,
      -1,    55,    -1,    57,    76,    -1,    -1,    -1,    -1,    -1,
     344,    65,    66,    -1,    -1,    69,    -1,    -1,    -1,    73,
      74,    -1,    -1,    77,    78,    -1,    -1,    99,    -1,   101,
     102,    -1,   104,   105,   106,    -1,   108,    -1,    -1,    -1,
      -1,    95,    96,    97,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   109,    -1,    -1,    -1,   113,
      -1,   115,    -1,   117,   118,    -1,    -1,    -1,    -1,    -1,
     124,    -1,    -1,    -1,   128,   129,   130,   131,   132,   133,
      -1,   135,    -1,   137,   156,    -1,    -1,    -1,    -1,   143,
      -1,    -1,    -1,   147,   148,   149,    -1,   151,   152,     8,
     154,   155,    -1,    12,    13,    14,    -1,    -1,    -1,   163,
     164,    20,    21,    -1,    -1,    -1,    -1,    26,    27,    -1,
      -1,    30,    -1,    -1,    33,    34,    -1,    -1,    37,    -1,
      51,    52,    53,    54,    -1,    -1,    45,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    57,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    -1,    80,
      69,    82,    -1,    84,    73,    74,    87,    -1,    77,    78,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    11,    -1,    95,    96,    97,    -1,
      17,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     109,    -1,    -1,    -1,   113,    -1,   115,    -1,   117,   118,
      -1,    -1,    -1,    -1,    -1,   124,    43,    -1,    -1,   128,
     129,   130,   131,   132,   133,    -1,   135,    -1,   137,    -1,
      -1,    -1,    -1,    -1,   143,    -1,    -1,    -1,   147,   148,
     149,    -1,   151,   152,    -1,   154,   155,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    81,   164,    83,    -1,    -1,    86,
      -1,    88,    -1,    -1,    91,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   110,   111
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     1,     8,    12,    13,    14,    20,    21,    26,    27,
      30,    33,    34,    37,    45,    55,    57,    65,    66,    69,
      73,    74,    77,    78,    95,    96,    97,   109,   113,   115,
     117,   118,   124,   128,   129,   130,   131,   132,   133,   135,
     137,   143,   147,   148,   149,   151,   152,   154,   155,   164,
     180,   181,   182,   183,   184,   194,   195,   196,   197,   201,
     206,   214,   223,   228,   232,   236,   237,   238,   239,   247,
     248,   259,   260,   163,    56,    56,    25,    46,    49,   123,
     125,   130,   198,   199,   200,     6,    10,    16,    63,    93,
     107,   136,   229,   230,   231,     9,    89,    93,   217,   218,
     219,   141,   240,   229,    18,    22,    75,   114,   122,   126,
     142,   146,   208,    60,    62,   141,   185,   186,   187,   141,
     141,   141,   141,   245,   246,   185,   256,    51,    52,    53,
      54,    80,    82,    84,    87,   220,   221,   222,   256,   141,
     141,   255,    56,    24,    60,    62,   134,   185,   186,    56,
     141,   241,   207,   208,   141,     5,    28,    44,    47,   112,
     138,   139,   233,   234,   235,    11,    17,    19,    43,    81,
      83,    86,    88,    91,   110,   111,   202,   203,   204,   205,
     186,    56,   175,   252,   253,   254,    56,   251,     0,   182,
     163,   185,   185,    29,    56,   258,   141,   141,    31,    50,
      72,   250,   177,    56,   199,   141,   230,   218,    56,    29,
     209,    38,    39,    40,    41,   127,   140,   144,   145,   224,
     225,   226,   227,   141,   182,   246,   185,   221,    56,   141,
     215,    24,    24,   215,   215,    79,   174,   208,   234,   258,
      35,    56,   150,   257,   203,    56,   258,   243,    56,   253,
      56,   163,     7,    15,    48,    64,    85,    90,    92,   103,
     120,   121,   150,   152,   156,   160,   188,   189,   190,   191,
     192,   193,     4,    59,    61,   141,   159,   249,   165,   166,
     261,   262,   263,    26,    33,    36,    71,    98,   153,   210,
     211,   212,   141,    56,   225,   258,   257,    42,    50,    67,
      68,    70,    76,    99,   101,   102,   104,   105,   106,   108,
     156,   216,   215,   215,   186,   141,    57,   119,   244,    32,
     189,    56,   132,   262,   264,   265,   267,   163,   174,   141,
       3,    23,    94,   100,   116,   158,   161,   213,   215,    24,
     242,   186,    56,    56,   174,   163,   178,   265,   177,   258,
     176,   185,   167,   266,   174,   168,   268,   269,   258,   174,
     178,   269,   163,   258,   177,   169,   170,   171,   172,   173,
     270,   271,   272,   178,   271,   163,   174,   163,   258
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
#line 340 "ntp_parser.y"
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
#line 374 "ntp_parser.y"
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.peers, my_node);
		}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 381 "ntp_parser.y"
    {
			peer_node *my_node;

			my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
			APPEND_G_FIFO(cfgt.peers, my_node);
		}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 400 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), (yyvsp[(1) - (2)].Integer)); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 405 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), AF_UNSPEC); }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 410 "ntp_parser.y"
    { (yyval.Integer) = AF_INET; }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 412 "ntp_parser.y"
    { (yyval.Integer) = AF_INET6; }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 417 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = append_gen_fifo((yyvsp[(1) - (2)].Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 419 "ntp_parser.y"
    { (yyval.Attr_val_fifo) = append_gen_fifo(NULL, (yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 429 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 445 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 463 "ntp_parser.y"
    {
			unpeer_node *my_node;
			
			my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
		}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 484 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 486 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.manycastserver, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 488 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.multicastclient, (yyvsp[(2) - (2)].Address_fifo)); }
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 499 "ntp_parser.y"
    {
			attr_val *atrv;
			
			atrv = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			APPEND_G_FIFO(cfgt.vars, atrv);
		}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 506 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 508 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			CONCAT_G_FIFOS(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 513 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 515 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 517 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 519 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 521 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Attr_val_fifo); }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 523 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 528 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 533 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 538 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), NULL);
		}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 546 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 548 "ntp_parser.y"
    {
			(yyval.Attr_val) = NULL;
			cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer);
			msyslog(LOG_WARNING,
				"'crypto revoke %d' is deprecated, "
				"please use 'revoke %d' instead.",
				cfgt.auth.revoke, cfgt.auth.revoke);
		}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 573 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.orphan_cmds, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 578 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 583 "ntp_parser.y"
    {	
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 591 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 593 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 595 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 622 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.stats_list, (yyvsp[(2) - (2)].Int_fifo)); }
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 624 "ntp_parser.y"
    {
			if (input_from_file) {
				cfgt.stats_dir = (yyvsp[(2) - (2)].String);
			} else {
				YYFREE((yyvsp[(2) - (2)].String));
				yyerror("statsdir remote configuration ignored");
			}
		}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 633 "ntp_parser.y"
    {
			filegen_node *fgn;
			
			fgn = create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.filegen_opts, fgn);
		}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 643 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 648 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 667 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
		}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 671 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 679 "ntp_parser.y"
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

  case 108:

/* Line 1464 of yacc.c  */
#line 689 "ntp_parser.y"
    {
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen type remote config ignored");
			}
		}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 698 "ntp_parser.y"
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

  case 110:

/* Line 1464 of yacc.c  */
#line 713 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 744 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.discard_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 748 "ntp_parser.y"
    {
			CONCAT_G_FIFOS(cfgt.mru_opts, (yyvsp[(2) - (2)].Attr_val_fifo));
		}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 752 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 760 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 768 "ntp_parser.y"
    {
			restrict_node *rn;

			rn = create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Int_fifo),
						  ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 776 "ntp_parser.y"
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

  case 128:

/* Line 1464 of yacc.c  */
#line 791 "ntp_parser.y"
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

  case 129:

/* Line 1464 of yacc.c  */
#line 806 "ntp_parser.y"
    {
			restrict_node *	rn;

			append_gen_fifo((yyvsp[(3) - (3)].Int_fifo), create_int_node((yyvsp[(2) - (3)].Integer)));
			rn = create_restrict_node(
				NULL, NULL, (yyvsp[(3) - (3)].Int_fifo), ip_file->line_no);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 818 "ntp_parser.y"
    {
			(yyval.Int_fifo) = NULL;
		}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 822 "ntp_parser.y"
    {
			(yyval.Int_fifo) = (yyvsp[(1) - (2)].Int_fifo);
			APPEND_G_FIFO((yyval.Int_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 847 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 852 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 860 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 871 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 876 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 884 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 904 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.fudge, aon);
		}
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 914 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 919 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 927 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 929 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 931 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 933 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 954 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.enable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 177:

/* Line 1464 of yacc.c  */
#line 956 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.disable_opts, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 961 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 966 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 974 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 976 "ntp_parser.y"
    { 
			if (input_from_file) {
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			} else {
				(yyval.Attr_val) = NULL;
				yyerror("enable/disable stats remote configuration ignored");
			}
		}
    break;

  case 188:

/* Line 1464 of yacc.c  */
#line 1001 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.tinker, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 189:

/* Line 1464 of yacc.c  */
#line 1006 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 190:

/* Line 1464 of yacc.c  */
#line 1011 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 191:

/* Line 1464 of yacc.c  */
#line 1019 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 200:

/* Line 1464 of yacc.c  */
#line 1040 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 201:

/* Line 1464 of yacc.c  */
#line 1047 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 202:

/* Line 1464 of yacc.c  */
#line 1054 "ntp_parser.y"
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

  case 203:

/* Line 1464 of yacc.c  */
#line 1070 "ntp_parser.y"
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

  case 204:

/* Line 1464 of yacc.c  */
#line 1089 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 1094 "ntp_parser.y"
    { /* see drift_parm below for actions */ }
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 1096 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.logconfig, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 1098 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.phone, (yyvsp[(2) - (2)].String_fifo)); }
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 1100 "ntp_parser.y"
    { APPEND_G_FIFO(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 1102 "ntp_parser.y"
    {
			addr_opts_node *aon;
			
			aon = create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Attr_val_fifo));
			APPEND_G_FIFO(cfgt.trap, aon);
		}
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 1109 "ntp_parser.y"
    { CONCAT_G_FIFOS(cfgt.ttl, (yyvsp[(2) - (2)].Attr_val_fifo)); }
    break;

  case 218:

/* Line 1464 of yacc.c  */
#line 1130 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 219:

/* Line 1464 of yacc.c  */
#line 1137 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String));
			APPEND_G_FIFO(cfgt.vars, av);
			av = create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double));
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 220:

/* Line 1464 of yacc.c  */
#line 1146 "ntp_parser.y"
    {
			attr_val *av;
			
			av = create_attr_sval(T_Driftfile, "");
			APPEND_G_FIFO(cfgt.vars, av);
		}
    break;

  case 221:

/* Line 1464 of yacc.c  */
#line 1156 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 1162 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 224:

/* Line 1464 of yacc.c  */
#line 1167 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
		}
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 1171 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 1179 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 1181 "ntp_parser.y"
    {
			(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), estrdup((yyvsp[(2) - (2)].Address_node)->address));
			destroy_address_node((yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1189 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 1194 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 1202 "ntp_parser.y"
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

  case 231:

/* Line 1464 of yacc.c  */
#line 1227 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 1234 "ntp_parser.y"
    {
			nic_rule_node *nrn;
			
			nrn = create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer));
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
    break;

  case 242:

/* Line 1464 of yacc.c  */
#line 1268 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(2) - (2)].Integer)));
		}
    break;

  case 243:

/* Line 1464 of yacc.c  */
#line 1273 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), create_int_node((yyvsp[(1) - (1)].Integer)));
		}
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 1281 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (2)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (2)].Attr_val));
		}
    break;

  case 245:

/* Line 1464 of yacc.c  */
#line 1286 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (1)].Attr_val));
		}
    break;

  case 246:

/* Line 1464 of yacc.c  */
#line 1294 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[(1) - (1)].Integer)); }
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 1300 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_rangeval('-', (yyvsp[(2) - (5)].Integer), (yyvsp[(4) - (5)].Integer)); }
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 1305 "ntp_parser.y"
    {
			(yyval.String_fifo) = (yyvsp[(1) - (2)].String_fifo);
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(2) - (2)].String)));
		}
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 1310 "ntp_parser.y"
    {
			(yyval.String_fifo) = NULL;
			APPEND_G_FIFO((yyval.String_fifo), create_string_node((yyvsp[(1) - (1)].String)));
		}
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 1318 "ntp_parser.y"
    {
			(yyval.Address_fifo) = (yyvsp[(1) - (2)].Address_fifo);
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(2) - (2)].Address_node));
		}
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 1323 "ntp_parser.y"
    {
			(yyval.Address_fifo) = NULL;
			APPEND_G_FIFO((yyval.Address_fifo), (yyvsp[(1) - (1)].Address_node));
		}
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 1331 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			} else {
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
			}
		}
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 1339 "ntp_parser.y"
    { (yyval.Integer) = 1; }
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 1340 "ntp_parser.y"
    { (yyval.Integer) = 0; }
    break;

  case 256:

/* Line 1464 of yacc.c  */
#line 1344 "ntp_parser.y"
    { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 1355 "ntp_parser.y"
    {
			sim_node *sn;
			
			sn =  create_sim_node((yyvsp[(3) - (5)].Attr_val_fifo), (yyvsp[(4) - (5)].Sim_server_fifo));
			APPEND_G_FIFO(cfgt.sim_details, sn);

			/* Revert from ; to \n for end-of-command */
			old_config_style = 1;
		}
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 1372 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 1377 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 1382 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 1390 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 1400 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = (yyvsp[(1) - (2)].Sim_server_fifo);
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(2) - (2)].Sim_server));
		}
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 1405 "ntp_parser.y"
    {
			(yyval.Sim_server_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_server_fifo), (yyvsp[(1) - (1)].Sim_server));
		}
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 1413 "ntp_parser.y"
    { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Sim_script_fifo)); }
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 1418 "ntp_parser.y"
    { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 1423 "ntp_parser.y"
    { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 1428 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = (yyvsp[(1) - (2)].Sim_script_fifo);
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(2) - (2)].Sim_script));
		}
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 1433 "ntp_parser.y"
    {
			(yyval.Sim_script_fifo) = NULL;
			APPEND_G_FIFO((yyval.Sim_script_fifo), (yyvsp[(1) - (1)].Sim_script));
		}
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 1441 "ntp_parser.y"
    { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Attr_val_fifo)); }
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 1446 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = (yyvsp[(1) - (3)].Attr_val_fifo);
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(2) - (3)].Attr_val));
		}
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 1451 "ntp_parser.y"
    {
			(yyval.Attr_val_fifo) = NULL;
			APPEND_G_FIFO((yyval.Attr_val_fifo), (yyvsp[(1) - (2)].Attr_val));
		}
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 1459 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
    break;



/* Line 1464 of yacc.c  */
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



/* Line 1684 of yacc.c  */
#line 1470 "ntp_parser.y"


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


