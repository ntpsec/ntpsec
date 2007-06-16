/* A Bison parser, made from /ntpdev/ntpd/ntp_parser.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON	1

/* Pure parsers.  */
#define YYPURE	0

/* Using locations.  */
#define YYLSP_NEEDED 0



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
     T_Broadcast = 266,
     T_Broadcastclient = 267,
     T_Broadcastdelay = 268,
     T_Burst = 269,
     T_Calibrate = 270,
     T_Calldelay = 271,
     T_Ceiling = 272,
     T_Cert = 273,
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
     T_DriftMinutes = 286,
     T_Enable = 287,
     T_End = 288,
     T_False = 289,
     T_File = 290,
     T_Filegen = 291,
     T_Flag1 = 292,
     T_Flag2 = 293,
     T_Flag3 = 294,
     T_Flag4 = 295,
     T_Floor = 296,
     T_Freq = 297,
     T_Fudge = 298,
     T_Gqpar = 299,
     T_Host = 300,
     T_Huffpuff = 301,
     T_IPv4_address = 302,
     T_IPv6_address = 303,
     T_Iburst = 304,
     T_Ident = 305,
     T_Iffpar = 306,
     T_Ignore = 307,
     T_Includefile = 308,
     T_Integer = 309,
     T_Interface = 310,
     T_Kernel = 311,
     T_Key = 312,
     T_Keys = 313,
     T_Keysdir = 314,
     T_Kod = 315,
     T_Leap = 316,
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
     T_Mvpar = 339,
     T_Nolink = 340,
     T_Nomodify = 341,
     T_None = 342,
     T_Nopeer = 343,
     T_Noquery = 344,
     T_Noselect = 345,
     T_Noserve = 346,
     T_Notrap = 347,
     T_Notrust = 348,
     T_Novolley = 349,
     T_Ntp = 350,
     T_Ntpport = 351,
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
     T_Pw = 364,
     T_RandFile = 365,
     T_Rawstats = 366,
     T_Refid = 367,
     T_Requestkey = 368,
     T_Restrict = 369,
     T_Revoke = 370,
     T_Server = 371,
     T_Setvar = 372,
     T_Sign = 373,
     T_Statistics = 374,
     T_Stats = 375,
     T_Statsdir = 376,
     T_Step = 377,
     T_Stepout = 378,
     T_Stratum = 379,
     T_String = 380,
     T_Sysstats = 381,
     T_Tick = 382,
     T_Time1 = 383,
     T_Time2 = 384,
     T_Tinker = 385,
     T_Tos = 386,
     T_Trap = 387,
     T_True = 388,
     T_Trustedkey = 389,
     T_Ttl = 390,
     T_Type = 391,
     T_Version = 392,
     T_Week = 393,
     T_Year = 394,
     T_Flag = 395,
     T_Void = 396,
     T_EOC = 397,
     T_Simulate = 398,
     T_Beep_Delay = 399,
     T_Sim_Duration = 400,
     T_Server_Offset = 401,
     T_Duration = 402,
     T_Freq_Offset = 403,
     T_Wander = 404,
     T_Jitter = 405,
     T_Prop_Delay = 406,
     T_Proc_Delay = 407
   };
#endif
#define T_Age 258
#define T_Allan 259
#define T_Auth 260
#define T_Autokey 261
#define T_Automax 262
#define T_Average 263
#define T_Bclient 264
#define T_Beacon 265
#define T_Broadcast 266
#define T_Broadcastclient 267
#define T_Broadcastdelay 268
#define T_Burst 269
#define T_Calibrate 270
#define T_Calldelay 271
#define T_Ceiling 272
#define T_Cert 273
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
#define T_DriftMinutes 286
#define T_Enable 287
#define T_End 288
#define T_False 289
#define T_File 290
#define T_Filegen 291
#define T_Flag1 292
#define T_Flag2 293
#define T_Flag3 294
#define T_Flag4 295
#define T_Floor 296
#define T_Freq 297
#define T_Fudge 298
#define T_Gqpar 299
#define T_Host 300
#define T_Huffpuff 301
#define T_IPv4_address 302
#define T_IPv6_address 303
#define T_Iburst 304
#define T_Ident 305
#define T_Iffpar 306
#define T_Ignore 307
#define T_Includefile 308
#define T_Integer 309
#define T_Interface 310
#define T_Kernel 311
#define T_Key 312
#define T_Keys 313
#define T_Keysdir 314
#define T_Kod 315
#define T_Leap 316
#define T_Limited 317
#define T_Link 318
#define T_Logconfig 319
#define T_Logfile 320
#define T_Loopstats 321
#define T_Lowpriotrap 322
#define T_Manycastclient 323
#define T_Manycastserver 324
#define T_Mask 325
#define T_Maxclock 326
#define T_Maxdist 327
#define T_Maxhop 328
#define T_Maxpoll 329
#define T_Minclock 330
#define T_Mindist 331
#define T_Minimum 332
#define T_Minpoll 333
#define T_Minsane 334
#define T_Mode 335
#define T_Monitor 336
#define T_Month 337
#define T_Multicastclient 338
#define T_Mvpar 339
#define T_Nolink 340
#define T_Nomodify 341
#define T_None 342
#define T_Nopeer 343
#define T_Noquery 344
#define T_Noselect 345
#define T_Noserve 346
#define T_Notrap 347
#define T_Notrust 348
#define T_Novolley 349
#define T_Ntp 350
#define T_Ntpport 351
#define T_Orphan 352
#define T_Panic 353
#define T_Peer 354
#define T_Peerstats 355
#define T_Phone 356
#define T_Pid 357
#define T_Pidfile 358
#define T_Pool 359
#define T_Port 360
#define T_Pps 361
#define T_Preempt 362
#define T_Prefer 363
#define T_Pw 364
#define T_RandFile 365
#define T_Rawstats 366
#define T_Refid 367
#define T_Requestkey 368
#define T_Restrict 369
#define T_Revoke 370
#define T_Server 371
#define T_Setvar 372
#define T_Sign 373
#define T_Statistics 374
#define T_Stats 375
#define T_Statsdir 376
#define T_Step 377
#define T_Stepout 378
#define T_Stratum 379
#define T_String 380
#define T_Sysstats 381
#define T_Tick 382
#define T_Time1 383
#define T_Time2 384
#define T_Tinker 385
#define T_Tos 386
#define T_Trap 387
#define T_True 388
#define T_Trustedkey 389
#define T_Ttl 390
#define T_Type 391
#define T_Version 392
#define T_Week 393
#define T_Year 394
#define T_Flag 395
#define T_Void 396
#define T_EOC 397
#define T_Simulate 398
#define T_Beep_Delay 399
#define T_Sim_Duration 400
#define T_Server_Offset 401
#define T_Duration 402
#define T_Freq_Offset 403
#define T_Wander 404
#define T_Jitter 405
#define T_Prop_Delay 406
#define T_Proc_Delay 407




/* Copy the first part of user declarations.  */
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
  void yyerror (char *msg);
  extern int input_from_file;  /* 0=input from ntpq>config command buffer */

  /* SK: The following is a terrible hack to allow the NTP code to be built
   * without OPENSSL. The following symbols need to be defined in the 
   * here because bison will balk at the presence of a C like preprocesser 
   * directive in the rules section.
   *
   * There should be a better way of doing this...
   */
#ifndef OPENSSL
  #define CRYPTO_CONF_NONE  0     
  #define CRYPTO_CONF_PRIV  1     
  #define CRYPTO_CONF_SIGN  2     
  #define CRYPTO_CONF_LEAP  3     
  #define CRYPTO_CONF_KEYS  4     
  #define CRYPTO_CONF_CERT  5     
  #define CRYPTO_CONF_RAND  6     
  #define CRYPTO_CONF_TRST  7     
  #define CRYPTO_CONF_IFFPAR 8    
  #define CRYPTO_CONF_GQPAR 9     
  #define CRYPTO_CONF_MVPAR 10    
  #define CRYPTO_CONF_PW    11    
 #define CRYPTO_CONF_IDENT 12    
#endif


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

#ifndef YYSTYPE
#line 68 "ntp_parser.y"
typedef union {
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
} yystype;
/* Line 193 of /usr/local/share/bison/yacc.c.  */
#line 449 "ntp_parser.c"
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} yyltype;
# define YYLTYPE yyltype
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* Copy the second part of user declarations.  */


/* Line 213 of /usr/local/share/bison/yacc.c.  */
#line 470 "ntp_parser.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];	\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  164
#define YYLAST   464

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  158
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  63
/* YYNRULES -- Number of rules. */
#define YYNRULES  227
/* YYNRULES -- Number of states. */
#define YYNSTATES  361

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   407

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   154,     2,   155,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   153,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   156,     2,   157,     2,     2,     2,     2,
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
     145,   146,   147,   148,   149,   150,   151,   152
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    15,    16,    18,    20,
      22,    24,    26,    28,    30,    32,    34,    36,    38,    42,
      45,    47,    49,    51,    53,    55,    57,    59,    62,    64,
      66,    69,    71,    73,    75,    77,    80,    83,    86,    88,
      90,    92,    94,    97,   100,   103,   105,   108,   111,   114,
     117,   120,   123,   126,   129,   132,   135,   138,   141,   143,
     146,   149,   152,   155,   158,   161,   164,   167,   170,   173,
     176,   179,   181,   184,   187,   190,   193,   196,   199,   202,
     205,   208,   211,   214,   217,   220,   224,   227,   229,   231,
     233,   235,   237,   239,   241,   244,   246,   249,   252,   254,
     256,   258,   260,   262,   264,   266,   268,   270,   272,   274,
     277,   281,   285,   291,   292,   295,   297,   299,   301,   303,
     305,   307,   309,   311,   313,   315,   317,   319,   322,   324,
     327,   330,   333,   337,   340,   342,   345,   348,   351,   354,
     357,   360,   363,   366,   369,   372,   375,   377,   379,   381,
     383,   385,   387,   389,   391,   394,   397,   399,   402,   405,
     408,   411,   414,   417,   420,   424,   426,   429,   432,   435,
     438,   441,   444,   447,   450,   453,   456,   460,   463,   465,
     468,   469,   474,   478,   481,   483,   486,   489,   492,   494,
     497,   499,   501,   503,   505,   508,   510,   513,   515,   518,
     520,   522,   524,   526,   528,   530,   536,   538,   542,   545,
     549,   553,   556,   558,   564,   569,   573,   575,   577,   580,
     582,   589,   593,   596,   600,   604,   608,   612
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     159,     0,    -1,   160,    -1,   160,   161,   142,    -1,   161,
     142,    -1,     1,   142,    -1,    -1,   162,    -1,   168,    -1,
     169,    -1,   175,    -1,   181,    -1,   172,    -1,   186,    -1,
     189,    -1,   192,    -1,   195,    -1,   208,    -1,   163,   164,
     166,    -1,   163,   164,    -1,   116,    -1,   104,    -1,    99,
      -1,    11,    -1,    68,    -1,   165,    -1,   125,    -1,    54,
     125,    -1,    47,    -1,    48,    -1,   166,   167,    -1,   167,
      -1,     6,    -1,    14,    -1,    49,    -1,    57,    54,    -1,
      78,    54,    -1,    74,    54,    -1,    90,    -1,   107,    -1,
     108,    -1,   133,    -1,   135,    54,    -1,    80,    54,    -1,
     137,    54,    -1,    12,    -1,    12,    94,    -1,    69,   205,
      -1,    83,   205,    -1,     6,    54,    -1,    21,    54,    -1,
      22,   170,    -1,    58,   125,    -1,    59,   125,    -1,   113,
      54,    -1,   115,    54,    -1,   134,   203,    -1,   170,   171,
      -1,   171,    -1,    18,   125,    -1,    61,   125,    -1,   110,
     125,    -1,    45,   125,    -1,   118,   125,    -1,    50,   125,
      -1,    51,   125,    -1,    44,   125,    -1,    84,   125,    -1,
     109,   125,    -1,   131,   173,    -1,   173,   174,    -1,   174,
      -1,    17,    54,    -1,    41,    54,    -1,    20,   206,    -1,
      97,    54,    -1,    76,   207,    -1,    72,   207,    -1,    75,
     207,    -1,    71,   207,    -1,    79,    54,    -1,    10,    54,
      -1,    73,    54,    -1,   119,   176,    -1,   121,   125,    -1,
      36,   177,   178,    -1,   176,   177,    -1,   177,    -1,    19,
      -1,    23,    -1,    66,    -1,   100,    -1,   111,    -1,   126,
      -1,   178,   179,    -1,   179,    -1,    35,   125,    -1,   136,
     180,    -1,    63,    -1,    85,    -1,    32,    -1,    26,    -1,
      87,    -1,   102,    -1,    24,    -1,   138,    -1,    82,    -1,
     139,    -1,     3,    -1,    27,   184,    -1,   114,   164,   182,
      -1,   114,    25,   182,    -1,   114,   165,    70,   165,   182,
      -1,    -1,   182,   183,    -1,    52,    -1,    60,    -1,    62,
      -1,    67,    -1,    86,    -1,    88,    -1,    89,    -1,    91,
      -1,    92,    -1,    93,    -1,    96,    -1,   137,    -1,   184,
     185,    -1,   185,    -1,     8,    54,    -1,    77,    54,    -1,
      81,    54,    -1,    43,   164,   187,    -1,   187,   188,    -1,
     188,    -1,   128,   207,    -1,   129,   207,    -1,   124,    54,
      -1,   112,   125,    -1,    37,   206,    -1,    38,   206,    -1,
      39,   206,    -1,    40,   206,    -1,    32,   190,    -1,    26,
     190,    -1,   190,   191,    -1,   191,    -1,     5,    -1,     9,
      -1,    15,    -1,    56,    -1,    81,    -1,    95,    -1,   120,
      -1,   130,   193,    -1,   193,   194,    -1,   194,    -1,     4,
     207,    -1,    28,   207,    -1,    42,   207,    -1,    46,   207,
      -1,    98,   207,    -1,   122,   207,    -1,   123,   207,    -1,
      53,   125,   161,    -1,    33,    -1,    13,   207,    -1,    16,
      54,    -1,   127,   207,    -1,    30,   196,    -1,   103,   125,
      -1,    65,   125,    -1,     7,    54,    -1,    64,   200,    -1,
     101,   204,    -1,   117,   197,    -1,   132,   165,   198,    -1,
     135,   203,    -1,   125,    -1,   125,    54,    -1,    -1,   125,
     153,   125,    25,    -1,   125,   153,   125,    -1,   198,   199,
      -1,   199,    -1,   105,    54,    -1,    55,   165,    -1,   200,
     201,    -1,   201,    -1,   202,   125,    -1,   125,    -1,   154,
      -1,   155,    -1,   153,    -1,   203,    54,    -1,    54,    -1,
     204,   125,    -1,   125,    -1,   205,   164,    -1,   164,    -1,
      54,    -1,   133,    -1,    34,    -1,    54,    -1,    29,    -1,
     209,   156,   210,   212,   157,    -1,   143,    -1,   210,   211,
     142,    -1,   211,   142,    -1,   144,   153,   207,    -1,   145,
     153,   207,    -1,   212,   213,    -1,   213,    -1,   215,   156,
     214,   217,   157,    -1,   146,   153,   207,   142,    -1,   116,
     153,   216,    -1,   165,    -1,   125,    -1,   217,   218,    -1,
     218,    -1,   147,   153,   207,   156,   219,   157,    -1,   219,
     220,   142,    -1,   220,   142,    -1,   148,   153,   207,    -1,
     149,   153,   207,    -1,   150,   153,   207,    -1,   151,   153,
     207,    -1,   152,   153,   207,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   299,   299,   303,   304,   309,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   338,   344,
     353,   354,   355,   356,   357,   361,   362,   363,   379,   380,
     384,   385,   390,   391,   392,   393,   394,   395,   396,   397,
     398,   399,   400,   401,   402,   412,   414,   416,   418,   429,
     431,   433,   439,   441,   443,   445,   447,   452,   453,   457,
     459,   461,   463,   465,   467,   469,   471,   473,   475,   485,
     490,   491,   495,   497,   499,   501,   503,   505,   507,   509,
     511,   513,   515,   525,   527,   529,   537,   538,   542,   544,
     546,   548,   550,   552,   557,   558,   562,   563,   564,   565,
     566,   567,   571,   572,   573,   574,   575,   576,   577,   586,
     588,   593,   598,   606,   607,   611,   612,   613,   614,   615,
     616,   617,   618,   619,   620,   621,   622,   626,   627,   631,
     632,   633,   641,   646,   647,   651,   653,   655,   657,   659,
     661,   663,   665,   674,   676,   681,   682,   686,   687,   688,
     689,   690,   691,   693,   701,   705,   706,   710,   711,   712,
     713,   714,   715,   716,   724,   740,   746,   748,   750,   752,
     755,   757,   759,   762,   764,   766,   768,   770,   774,   776,
     779,   784,   786,   792,   793,   797,   798,   803,   804,   808,
     809,   826,   827,   828,   837,   838,   842,   843,   847,   848,
     852,   861,   862,   866,   867,   875,   890,   894,   895,   899,
     900,   904,   905,   909,   914,   918,   922,   923,   927,   928,
     932,   937,   938,   942,   944,   946,   948,   950
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_Age", "T_Allan", "T_Auth", "T_Autokey", 
  "T_Automax", "T_Average", "T_Bclient", "T_Beacon", "T_Broadcast", 
  "T_Broadcastclient", "T_Broadcastdelay", "T_Burst", "T_Calibrate", 
  "T_Calldelay", "T_Ceiling", "T_Cert", "T_Clockstats", "T_Cohort", 
  "T_ControlKey", "T_Crypto", "T_Cryptostats", "T_Day", "T_Default", 
  "T_Disable", "T_Discard", "T_Dispersion", "T_Double", "T_Driftfile", 
  "T_DriftMinutes", "T_Enable", "T_End", "T_False", "T_File", "T_Filegen", 
  "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4", "T_Floor", "T_Freq", 
  "T_Fudge", "T_Gqpar", "T_Host", "T_Huffpuff", "T_IPv4_address", 
  "T_IPv6_address", "T_Iburst", "T_Ident", "T_Iffpar", "T_Ignore", 
  "T_Includefile", "T_Integer", "T_Interface", "T_Kernel", "T_Key", 
  "T_Keys", "T_Keysdir", "T_Kod", "T_Leap", "T_Limited", "T_Link", 
  "T_Logconfig", "T_Logfile", "T_Loopstats", "T_Lowpriotrap", 
  "T_Manycastclient", "T_Manycastserver", "T_Mask", "T_Maxclock", 
  "T_Maxdist", "T_Maxhop", "T_Maxpoll", "T_Minclock", "T_Mindist", 
  "T_Minimum", "T_Minpoll", "T_Minsane", "T_Mode", "T_Monitor", "T_Month", 
  "T_Multicastclient", "T_Mvpar", "T_Nolink", "T_Nomodify", "T_None", 
  "T_Nopeer", "T_Noquery", "T_Noselect", "T_Noserve", "T_Notrap", 
  "T_Notrust", "T_Novolley", "T_Ntp", "T_Ntpport", "T_Orphan", "T_Panic", 
  "T_Peer", "T_Peerstats", "T_Phone", "T_Pid", "T_Pidfile", "T_Pool", 
  "T_Port", "T_Pps", "T_Preempt", "T_Prefer", "T_Pw", "T_RandFile", 
  "T_Rawstats", "T_Refid", "T_Requestkey", "T_Restrict", "T_Revoke", 
  "T_Server", "T_Setvar", "T_Sign", "T_Statistics", "T_Stats", 
  "T_Statsdir", "T_Step", "T_Stepout", "T_Stratum", "T_String", 
  "T_Sysstats", "T_Tick", "T_Time1", "T_Time2", "T_Tinker", "T_Tos", 
  "T_Trap", "T_True", "T_Trustedkey", "T_Ttl", "T_Type", "T_Version", 
  "T_Week", "T_Year", "T_Flag", "T_Void", "T_EOC", "T_Simulate", 
  "T_Beep_Delay", "T_Sim_Duration", "T_Server_Offset", "T_Duration", 
  "T_Freq_Offset", "T_Wander", "T_Jitter", "T_Prop_Delay", "T_Proc_Delay", 
  "'='", "'+'", "'-'", "'{'", "'}'", "$accept", "configuration", 
  "command_list", "command", "server_command", "client_type", "address", 
  "ip_address", "option_list", "option", "other_mode_command", 
  "authentication_command", "crypto_command_list", "crypto_command", 
  "orphan_mode_command", "tos_option_list", "tos_option", 
  "monitoring_command", "stats_list", "stat", "filegen_option_list", 
  "filegen_option", "filegen_type", "access_control_command", 
  "ac_flag_list", "access_control_flag", "discard_option_list", 
  "discard_option", "fudge_command", "fudge_factor_list", "fudge_factor", 
  "system_option_command", "system_option_list", "system_option", 
  "tinker_command", "tinker_option_list", "tinker_option", 
  "miscellaneous_command", "drift_parm", "variable_assign", 
  "trap_option_list", "trap_option", "log_config_list", 
  "log_config_command", "log_config_prefix", "integer_list", 
  "string_list", "address_list", "boolean", "number", "simulate_command", 
  "sim_conf_start", "sim_init_statement_list", "sim_init_statement", 
  "sim_server_list", "sim_server", "sim_server_offset", "sim_server_name", 
  "sim_address", "sim_act_list", "sim_act", "sim_act_stmt_list", 
  "sim_act_stmt", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
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
     405,   406,   407,    61,    43,    45,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   158,   159,   160,   160,   160,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   162,   162,
     163,   163,   163,   163,   163,   164,   164,   164,   165,   165,
     166,   166,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   168,   168,   168,   168,   169,
     169,   169,   169,   169,   169,   169,   169,   170,   170,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   172,
     173,   173,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   175,   175,   175,   176,   176,   177,   177,
     177,   177,   177,   177,   178,   178,   179,   179,   179,   179,
     179,   179,   180,   180,   180,   180,   180,   180,   180,   181,
     181,   181,   181,   182,   182,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   184,   184,   185,
     185,   185,   186,   187,   187,   188,   188,   188,   188,   188,
     188,   188,   188,   189,   189,   190,   190,   191,   191,   191,
     191,   191,   191,   191,   192,   193,   193,   194,   194,   194,
     194,   194,   194,   194,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   196,   196,
     196,   197,   197,   198,   198,   199,   199,   200,   200,   201,
     201,   202,   202,   202,   203,   203,   204,   204,   205,   205,
     206,   206,   206,   207,   207,   208,   209,   210,   210,   211,
     211,   212,   212,   213,   214,   215,   216,   216,   217,   217,
     218,   219,   219,   220,   220,   220,   220,   220
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     3,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       2,     1,     1,     1,     1,     2,     2,     2,     1,     1,
       1,     1,     2,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     3,     2,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       3,     3,     5,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       2,     2,     3,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     3,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     3,     2,     1,     2,
       0,     4,     3,     2,     1,     2,     2,     2,     1,     2,
       1,     1,     1,     1,     2,     1,     2,     1,     2,     1,
       1,     1,     1,     1,     1,     5,     1,     3,     2,     3,
       3,     2,     1,     5,     4,     3,     1,     1,     2,     1,
       6,     3,     2,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     0,    23,    45,     0,     0,     0,     0,
       0,     0,   180,     0,   165,     0,     0,     0,     0,     0,
       0,     0,    24,     0,     0,    22,     0,     0,    21,     0,
       0,     0,    20,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   206,     0,     2,     0,     7,     0,     8,     9,
      12,    10,    11,    13,    14,    15,    16,    17,     0,     5,
      49,   172,    46,   204,   203,   166,   167,    50,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    51,    58,
     147,   148,   149,   150,   151,   152,   153,   144,   146,     0,
       0,     0,   109,   128,   178,   169,   143,    88,    89,    90,
      91,    92,    93,     0,    28,    29,     0,    26,     0,    25,
       6,    52,    53,   190,   193,   191,   192,   173,   188,     0,
     171,   199,    47,    48,   197,   174,   170,    54,   113,   113,
      25,    55,     0,   175,    83,    87,    84,   168,     0,     0,
       0,     0,     0,     0,     0,   154,   156,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    69,    71,
       0,   195,    56,   177,     1,     0,     4,    19,     0,    59,
      66,    62,    64,    65,    60,    67,    68,    61,    63,    57,
     145,   129,   130,   131,   127,   179,   101,   100,     0,    98,
      99,     0,    85,    95,    27,     0,     0,     0,     0,     0,
       0,     0,     0,   132,   134,   164,   187,   189,   198,   196,
     111,   110,     0,     0,    86,   157,   158,   159,   160,   161,
     162,   163,   155,    81,    72,   202,   200,   201,    74,    73,
      79,    77,    82,    78,    76,    80,    75,    70,     0,     0,
     176,   184,   194,     3,    32,    33,    34,     0,     0,     0,
       0,    38,    39,    40,    41,     0,     0,    18,    31,     0,
       0,     0,     0,    96,   108,   104,   106,   102,   103,   105,
     107,    97,    94,   139,   140,   141,   142,   138,   137,   135,
     136,   133,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   114,   113,   182,   186,   185,   183,
      35,    37,    36,    43,    42,    44,    30,     0,     0,     0,
       0,     0,   212,     0,   208,   112,   181,   209,   210,     0,
     207,   205,   211,     0,   217,   216,   215,     0,     0,     0,
       0,     0,   219,     0,     0,   213,   218,   214,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   220,     0,   222,   223,   224,   225,   226,   227,
     221
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    43,    44,    45,    46,    47,   121,   109,   257,   258,
      48,    49,    78,    79,    50,   158,   159,    51,   134,   103,
     192,   193,   271,    52,   210,   294,    92,    93,    53,   203,
     204,    54,    87,    88,    55,   145,   146,    56,    95,   133,
     240,   241,   117,   118,   119,   162,   125,   122,   228,    65,
      57,    58,   261,   262,   311,   312,   328,   313,   326,   331,
     332,   345,   346
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -135
static const short yypact[] =
{
      10,  -134,   -41,   -34,  -135,   -66,   -15,   -19,    -7,   129,
       0,    -4,   -75,     0,  -135,    32,    42,   -73,   -71,   -65,
      50,   -49,  -135,    42,    42,  -135,   -43,   -33,  -135,    26,
      37,    47,  -135,     5,    32,    13,   -15,    -1,   179,    39,
      56,    56,  -135,   135,   165,     4,  -135,    42,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,    -6,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,    24,    29,
      34,    35,    36,    44,    45,    63,    68,    69,   129,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,     0,  -135,   103,
     146,   148,    -4,  -135,   152,  -135,     0,  -135,  -135,  -135,
    -135,  -135,  -135,    -2,  -135,  -135,    82,  -135,    27,  -135,
     310,  -135,  -135,  -135,  -135,  -135,  -135,    50,  -135,    87,
    -135,  -135,    42,    42,  -135,    94,  -135,  -135,  -135,  -135,
     151,  -135,    72,  -135,    32,  -135,  -135,  -135,   -15,   -15,
     -15,   -15,   -15,   -15,   -15,    -1,  -135,   168,   172,   -16,
     173,   -15,   -15,   174,   -15,   -15,   177,   178,   179,  -135,
     -11,  -135,   181,   181,  -135,    95,  -135,   327,   -45,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,   120,  -135,
    -135,    46,    -2,  -135,  -135,   -16,   -16,   -16,   -16,   121,
     182,   -15,   -15,    27,  -135,  -135,  -135,  -135,  -135,  -135,
     299,   299,    39,   128,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,    39,   202,
     -11,  -135,  -135,  -135,  -135,  -135,  -135,   203,   211,   213,
     216,  -135,  -135,  -135,  -135,   219,   220,   327,  -135,   122,
     124,    66,   141,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,   260,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,   -15,   -15,   134,
     147,  -109,  -135,   132,  -135,   299,  -135,  -135,  -135,    11,
    -135,  -135,  -135,   145,  -135,  -135,  -135,   149,   154,   -15,
     150,  -128,  -135,   162,   -15,  -135,  -135,  -135,   138,   111,
     153,   156,   157,   166,   167,    92,   163,   -15,   -15,   -15,
     -15,   -15,  -135,   176,  -135,  -135,  -135,  -135,  -135,  -135,
    -135
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -135,  -135,  -135,   -38,  -135,  -135,    41,   -29,  -135,    67,
    -135,  -135,  -135,   247,  -135,  -135,   169,  -135,  -135,   -22,
    -135,   136,  -135,  -135,  -127,  -135,  -135,   237,  -135,  -135,
     127,  -135,   321,   -62,  -135,  -135,   190,  -135,  -135,  -135,
    -135,    98,  -135,   222,  -135,   303,  -135,   323,    19,   -36,
    -135,  -135,  -135,    84,  -135,    38,  -135,  -135,  -135,  -135,
      17,  -135,     7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -7
static const short yytable[] =
{
     137,   130,   211,   138,    89,    80,   165,   309,    59,    81,
     160,     1,   135,    60,    63,    82,     2,     3,   225,   330,
      61,     4,     5,     6,   186,   180,     7,   139,    62,   335,
     187,     8,     9,   188,   180,    66,    10,    11,   226,    64,
      12,   140,    13,    14,   238,   141,    15,    67,   321,   264,
      94,    97,   110,    16,   111,    98,    83,   108,   104,   105,
     112,   189,   128,    17,   195,   196,   197,   198,    18,    19,
     265,   129,   205,    90,    20,    21,   120,    91,    22,    23,
     127,    84,   124,   190,   104,   105,   104,   105,   167,   104,
     105,   106,   126,    24,   239,    85,   106,   142,    99,   259,
     260,   131,   215,   216,   217,   218,   219,   220,   221,    25,
     161,    26,   214,    27,    28,   230,   231,   227,   233,   234,
      86,   143,   144,    29,    30,    31,    32,    33,   266,    34,
     132,    35,   100,   267,   191,   164,   324,    36,   136,   199,
      37,    38,    39,   101,    40,    41,   166,    68,   268,   169,
     168,   200,    -6,    42,   170,   201,   202,   181,   102,   171,
     172,   173,   107,   208,   208,   279,   280,   107,   315,   174,
     175,     2,     3,    69,    70,   113,     4,     5,     6,    71,
      72,     7,   309,   295,   269,   270,     8,     9,   176,   147,
      73,    10,    11,   177,   178,    12,   148,    13,    14,   149,
     182,    15,   183,   114,   115,   116,   185,   194,    16,   297,
     259,   260,   207,    74,   273,   274,   275,   276,    17,   209,
     150,   212,   223,    18,    19,   213,   224,   229,   232,    20,
      21,   235,   236,    22,    23,   242,   278,   243,    75,    76,
     340,   341,   342,   343,   344,   263,   277,    77,    24,   352,
     151,   152,   153,   296,   154,   155,   298,   300,   156,   340,
     341,   342,   343,   344,    25,   301,    26,   302,    27,    28,
     303,   317,   318,   304,   305,   307,   157,   308,    29,    30,
      31,    32,    33,   314,    34,   316,    35,   319,   323,   320,
     325,   327,    36,   333,   339,    37,    38,    39,   338,    40,
      41,   330,   329,   334,   337,   354,   347,    -6,    42,   348,
     349,   355,   356,   357,   358,   359,     2,     3,   360,   350,
     351,     4,     5,     6,   306,   179,     7,   237,   272,   184,
     281,     8,     9,   244,    96,   222,    10,    11,   299,   206,
      12,   245,    13,    14,   163,   310,    15,   123,   336,   322,
       0,   282,   353,    16,     0,     0,     0,     0,     0,   283,
       0,   284,     0,    17,     0,     0,   285,     0,    18,    19,
       0,     0,     0,     0,    20,    21,   246,     0,    22,    23,
       0,     0,     0,     0,   247,   286,     0,   287,   288,     0,
     289,   290,   291,    24,     0,   292,     0,     0,     0,     0,
       0,   248,     0,     0,     0,   249,     0,   250,     0,    25,
       0,    26,     0,    27,    28,     0,     0,   251,     0,     0,
       0,     0,     0,    29,    30,    31,    32,    33,     0,    34,
       0,    35,     0,     0,   252,   253,   293,    36,     0,     0,
      37,    38,    39,     0,    40,    41,     0,     0,     0,     0,
       0,     0,     0,    42,     0,     0,     0,     0,     0,     0,
     254,     0,   255,     0,   256
};

static const short yycheck[] =
{
      36,    30,   129,     4,     8,     5,    44,   116,   142,     9,
      39,     1,    34,    54,    29,    15,     6,     7,    34,   147,
      54,    11,    12,    13,    26,    87,    16,    28,    94,   157,
      32,    21,    22,    35,    96,    54,    26,    27,    54,    54,
      30,    42,    32,    33,    55,    46,    36,    54,   157,     3,
     125,    19,   125,    43,   125,    23,    56,    16,    47,    48,
     125,    63,    25,    53,    37,    38,    39,    40,    58,    59,
      24,    30,   110,    77,    64,    65,   125,    81,    68,    69,
      54,    81,   125,    85,    47,    48,    47,    48,    47,    47,
      48,    54,   125,    83,   105,    95,    54,    98,    66,   144,
     145,    54,   138,   139,   140,   141,   142,   143,   144,    99,
      54,   101,   134,   103,   104,   151,   152,   133,   154,   155,
     120,   122,   123,   113,   114,   115,   116,   117,    82,   119,
     125,   121,   100,    87,   136,     0,   125,   127,   125,   112,
     130,   131,   132,   111,   134,   135,   142,    18,   102,   125,
     156,   124,   142,   143,   125,   128,   129,    54,   126,   125,
     125,   125,   125,   122,   123,   201,   202,   125,   295,   125,
     125,     6,     7,    44,    45,   125,    11,    12,    13,    50,
      51,    16,   116,   212,   138,   139,    21,    22,   125,    10,
      61,    26,    27,   125,   125,    30,    17,    32,    33,    20,
      54,    36,    54,   153,   154,   155,    54,   125,    43,   238,
     144,   145,   125,    84,   195,   196,   197,   198,    53,   125,
      41,    70,    54,    58,    59,   153,    54,    54,    54,    64,
      65,    54,    54,    68,    69,    54,    54,   142,   109,   110,
     148,   149,   150,   151,   152,   125,   125,   118,    83,   157,
      71,    72,    73,   125,    75,    76,    54,    54,    79,   148,
     149,   150,   151,   152,    99,    54,   101,    54,   103,   104,
      54,   307,   308,    54,    54,   153,    97,   153,   113,   114,
     115,   116,   117,   142,   119,    25,   121,   153,   156,   142,
     319,   146,   127,   329,   156,   130,   131,   132,   334,   134,
     135,   147,   153,   153,   142,   142,   153,   142,   143,   153,
     153,   347,   348,   349,   350,   351,     6,     7,   142,   153,
     153,    11,    12,    13,   257,    78,    16,   158,   192,    92,
     203,    21,    22,     6,    13,   145,    26,    27,   240,   117,
      30,    14,    32,    33,    41,   261,    36,    24,   331,   311,
      -1,    52,   345,    43,    -1,    -1,    -1,    -1,    -1,    60,
      -1,    62,    -1,    53,    -1,    -1,    67,    -1,    58,    59,
      -1,    -1,    -1,    -1,    64,    65,    49,    -1,    68,    69,
      -1,    -1,    -1,    -1,    57,    86,    -1,    88,    89,    -1,
      91,    92,    93,    83,    -1,    96,    -1,    -1,    -1,    -1,
      -1,    74,    -1,    -1,    -1,    78,    -1,    80,    -1,    99,
      -1,   101,    -1,   103,   104,    -1,    -1,    90,    -1,    -1,
      -1,    -1,    -1,   113,   114,   115,   116,   117,    -1,   119,
      -1,   121,    -1,    -1,   107,   108,   137,   127,    -1,    -1,
     130,   131,   132,    -1,   134,   135,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   143,    -1,    -1,    -1,    -1,    -1,    -1,
     133,    -1,   135,    -1,   137
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     6,     7,    11,    12,    13,    16,    21,    22,
      26,    27,    30,    32,    33,    36,    43,    53,    58,    59,
      64,    65,    68,    69,    83,    99,   101,   103,   104,   113,
     114,   115,   116,   117,   119,   121,   127,   130,   131,   132,
     134,   135,   143,   159,   160,   161,   162,   163,   168,   169,
     172,   175,   181,   186,   189,   192,   195,   208,   209,   142,
      54,    54,    94,    29,    54,   207,    54,    54,    18,    44,
      45,    50,    51,    61,    84,   109,   110,   118,   170,   171,
       5,     9,    15,    56,    81,    95,   120,   190,   191,     8,
      77,    81,   184,   185,   125,   196,   190,    19,    23,    66,
     100,   111,   126,   177,    47,    48,    54,   125,   164,   165,
     125,   125,   125,   125,   153,   154,   155,   200,   201,   202,
     125,   164,   205,   205,   125,   204,   125,    54,    25,   164,
     165,    54,   125,   197,   176,   177,   125,   207,     4,    28,
      42,    46,    98,   122,   123,   193,   194,    10,    17,    20,
      41,    71,    72,    73,    75,    76,    79,    97,   173,   174,
     165,    54,   203,   203,     0,   161,   142,   164,   156,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   171,
     191,    54,    54,    54,   185,    54,    26,    32,    35,    63,
      85,   136,   178,   179,   125,    37,    38,    39,    40,   112,
     124,   128,   129,   187,   188,   161,   201,   125,   164,   125,
     182,   182,    70,   153,   177,   207,   207,   207,   207,   207,
     207,   207,   194,    54,    54,    34,    54,   133,   206,    54,
     207,   207,    54,   207,   207,    54,    54,   174,    55,   105,
     198,   199,    54,   142,     6,    14,    49,    57,    74,    78,
      80,    90,   107,   108,   133,   135,   137,   166,   167,   144,
     145,   210,   211,   125,     3,    24,    82,    87,   102,   138,
     139,   180,   179,   206,   206,   206,   206,   125,    54,   207,
     207,   188,    52,    60,    62,    67,    86,    88,    89,    91,
      92,    93,    96,   137,   183,   165,   125,   165,    54,   199,
      54,    54,    54,    54,    54,    54,   167,   153,   153,   116,
     211,   212,   213,   215,   142,   182,    25,   207,   207,   153,
     142,   157,   213,   156,   125,   165,   216,   146,   214,   153,
     147,   217,   218,   207,   153,   157,   218,   142,   207,   156,
     148,   149,   150,   151,   152,   219,   220,   153,   153,   153,
     153,   153,   157,   220,   142,   207,   207,   207,   207,   207,
     142
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

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
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)           \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#define YYLEX	yylex ()

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
} while (0)
# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*-----------------------------.
| Print this symbol on YYOUT.  |
`-----------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yysymprint (FILE* yyout, int yytype, YYSTYPE yyvalue)
#else
yysymprint (yyout, yytype, yyvalue)
    FILE* yyout;
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyout, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyout, yytoknum[yytype], yyvalue);
# endif
    }
  else
    YYFPRINTF (yyout, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyout, ")");
}
#endif /* YYDEBUG. */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yydestruct (int yytype, YYSTYPE yyvalue)
#else
yydestruct (yytype, yyvalue)
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  switch (yytype)
    {
      default:
        break;
    }
}



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of parse errors so far.  */
int yynerrs;


int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with.  */

  if (yychar <= 0)		/* This means end of input.  */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more.  */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

      /* We have to keep this `#if YYDEBUG', since we use variables
	 which are defined only if `YYDEBUG' is set.  */
      YYDPRINTF ((stderr, "Next token is "));
      YYDSYMPRINT ((stderr, yychar1, yylval));
      YYDPRINTF ((stderr, "\n"));
    }

  /* If the proper action on seeing token YYCHAR1 is to reduce or to
     detect an error, take that action.  */
  yyn += yychar1;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yychar1)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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



#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn - 1, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] >= 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif
  switch (yyn)
    {
        case 5:
#line 310 "ntp_parser.y"
    { 
					if (input_from_file == 1) {
                    	msyslog(LOG_INFO, "parse error %s line %d ignored\n",
                            ip_file->fname, ip_file->line_no);
                	} else if (input_from_file != 0)
						msyslog(LOG_INFO, "parse: bad boolean input flag\n"); 
                }
    break;

  case 18:
#line 339 "ntp_parser.y"
    { 
                        struct peer_node *my_node =  create_peer_node(yyvsp[-2].Integer, yyvsp[-1].Address_node, yyvsp[0].Queue); 
                        if (my_node)
                            enqueue(my_config.peers, my_node);                        
                    }
    break;

  case 19:
#line 345 "ntp_parser.y"
    { 
                        struct peer_node *my_node = create_peer_node(yyvsp[-1].Integer, yyvsp[0].Address_node, NULL); 
                        if (my_node)
                            enqueue(my_config.peers, my_node);                        
                    }
    break;

  case 20:
#line 353 "ntp_parser.y"
    { yyval.Integer = T_Server; }
    break;

  case 21:
#line 354 "ntp_parser.y"
    { yyval.Integer = T_Pool; }
    break;

  case 22:
#line 355 "ntp_parser.y"
    { yyval.Integer = T_Peer; }
    break;

  case 23:
#line 356 "ntp_parser.y"
    { yyval.Integer = T_Broadcast; }
    break;

  case 24:
#line 357 "ntp_parser.y"
    { yyval.Integer = T_Manycastclient; }
    break;

  case 25:
#line 361 "ntp_parser.y"
    { yyval.Address_node = yyvsp[0].Address_node; }
    break;

  case 26:
#line 362 "ntp_parser.y"
    { yyval.Address_node = create_address_node(yyvsp[0].String, default_ai_family); }
    break;

  case 27:
#line 364 "ntp_parser.y"
    {
                        if (yyvsp[-1].Integer == -4)
                            yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET);
                        else if (yyvsp[-1].Integer == -6)
                            yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET6);
                        else {
                            yyerror("Invalid address type specified. Assuming default.\n");
                            yyval.Address_node = create_address_node(yyvsp[0].String, default_ai_family);
                        }
                    }
    break;

  case 28:
#line 379 "ntp_parser.y"
    { yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET); }
    break;

  case 29:
#line 380 "ntp_parser.y"
    { yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET6); }
    break;

  case 30:
#line 384 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;

  case 31:
#line 385 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;

  case 32:
#line 390 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, FLAG_SKEY | FLAG_AUTHENABLE); }
    break;

  case 33:
#line 391 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, FLAG_BURST); }
    break;

  case 34:
#line 392 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, FLAG_IBURST); }
    break;

  case 35:
#line 393 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Key, yyvsp[0].Integer); }
    break;

  case 36:
#line 394 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Minpoll, yyvsp[0].Integer); }
    break;

  case 37:
#line 395 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Maxpoll, yyvsp[0].Integer); }
    break;

  case 38:
#line 396 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, FLAG_NOSELECT);}
    break;

  case 39:
#line 397 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREEMPT); }
    break;

  case 40:
#line 398 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREFER); }
    break;

  case 41:
#line 399 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, FLAG_TRUE); }
    break;

  case 42:
#line 400 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Ttl, yyvsp[0].Integer); }
    break;

  case 43:
#line 401 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Mode, yyvsp[0].Integer); }
    break;

  case 44:
#line 402 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Version, yyvsp[0].Integer); }
    break;

  case 45:
#line 413 "ntp_parser.y"
    { my_config.broadcastclient = SIMPLE; }
    break;

  case 46:
#line 415 "ntp_parser.y"
    { my_config.broadcastclient = NOVOLLEY;  }
    break;

  case 47:
#line 417 "ntp_parser.y"
    { append_queue(my_config.manycastserver, yyvsp[0].Queue);  }
    break;

  case 48:
#line 419 "ntp_parser.y"
    { append_queue(my_config.multicastclient, yyvsp[0].Queue);  }
    break;

  case 49:
#line 430 "ntp_parser.y"
    { my_config.auth.autokey = yyvsp[0].Integer;  }
    break;

  case 50:
#line 432 "ntp_parser.y"
    { my_config.auth.control_key = yyvsp[0].Integer;  }
    break;

  case 51:
#line 434 "ntp_parser.y"
    { if (my_config.auth.crypto_cmd_list != NULL) 
					append_queue(my_config.auth.crypto_cmd_list, yyvsp[0].Queue); 
		 		else 
					my_config.auth.crypto_cmd_list = yyvsp[0].Queue;
			}
    break;

  case 52:
#line 440 "ntp_parser.y"
    { my_config.auth.keys = yyvsp[0].String;  }
    break;

  case 53:
#line 442 "ntp_parser.y"
    { my_config.auth.keysdir = yyvsp[0].String;  }
    break;

  case 54:
#line 444 "ntp_parser.y"
    { my_config.auth.requested_key = yyvsp[0].Integer;  }
    break;

  case 55:
#line 446 "ntp_parser.y"
    { my_config.auth.revoke = yyvsp[0].Integer;  }
    break;

  case 56:
#line 448 "ntp_parser.y"
    { my_config.auth.trusted_key_list = yyvsp[0].Queue;  }
    break;

  case 57:
#line 452 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;

  case 58:
#line 453 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;

  case 59:
#line 458 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(CRYPTO_CONF_CERT, yyvsp[0].String); }
    break;

  case 60:
#line 460 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(CRYPTO_CONF_LEAP, yyvsp[0].String); }
    break;

  case 61:
#line 462 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(CRYPTO_CONF_RAND, yyvsp[0].String); }
    break;

  case 62:
#line 464 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PRIV, yyvsp[0].String); }
    break;

  case 63:
#line 466 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(CRYPTO_CONF_SIGN, yyvsp[0].String); }
    break;

  case 64:
#line 468 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(CRYPTO_CONF_IDENT, yyvsp[0].String); }
    break;

  case 65:
#line 470 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(CRYPTO_CONF_IFFPAR, yyvsp[0].String); }
    break;

  case 66:
#line 472 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(CRYPTO_CONF_GQPAR, yyvsp[0].String); }
    break;

  case 67:
#line 474 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(CRYPTO_CONF_MVPAR, yyvsp[0].String); }
    break;

  case 68:
#line 476 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PW, yyvsp[0].String); }
    break;

  case 69:
#line 486 "ntp_parser.y"
    { append_queue(my_config.orphan_cmds,yyvsp[0].Queue);  }
    break;

  case 70:
#line 490 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;

  case 71:
#line 491 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;

  case 72:
#line 496 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(PROTO_CEILING, (double)yyvsp[0].Integer); }
    break;

  case 73:
#line 498 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(PROTO_FLOOR, (double)yyvsp[0].Integer); }
    break;

  case 74:
#line 500 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(PROTO_COHORT, (double)yyvsp[0].Integer); }
    break;

  case 75:
#line 502 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(PROTO_ORPHAN, (double)yyvsp[0].Integer); }
    break;

  case 76:
#line 504 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(PROTO_MINDISP, yyvsp[0].Double); }
    break;

  case 77:
#line 506 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(PROTO_MAXDIST, yyvsp[0].Double); }
    break;

  case 78:
#line 508 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(PROTO_MINCLOCK, yyvsp[0].Double); }
    break;

  case 79:
#line 510 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(PROTO_MAXCLOCK, yyvsp[0].Double); }
    break;

  case 80:
#line 512 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(PROTO_MINSANE, (double)yyvsp[0].Integer); }
    break;

  case 81:
#line 514 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(PROTO_BEACON, (double)yyvsp[0].Integer); }
    break;

  case 82:
#line 516 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(PROTO_MAXHOP, (double)yyvsp[0].Integer); }
    break;

  case 83:
#line 526 "ntp_parser.y"
    { append_queue(my_config.stats_list, yyvsp[0].Queue);  }
    break;

  case 84:
#line 528 "ntp_parser.y"
    { my_config.stats_dir = yyvsp[0].String;  }
    break;

  case 85:
#line 530 "ntp_parser.y"
    {
                        enqueue(my_config.filegen_opts, 
                                create_filegen_node(yyvsp[-1].VoidPtr, yyvsp[0].Queue));                         
                    }
    break;

  case 86:
#line 537 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); }
    break;

  case 87:
#line 538 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].VoidPtr); }
    break;

  case 88:
#line 543 "ntp_parser.y"
    { yyval.VoidPtr = create_pval("clockstats"); }
    break;

  case 89:
#line 545 "ntp_parser.y"
    { yyval.VoidPtr = create_pval("cryptostats"); }
    break;

  case 90:
#line 547 "ntp_parser.y"
    { yyval.VoidPtr = create_pval("loopstats"); }
    break;

  case 91:
#line 549 "ntp_parser.y"
    { yyval.VoidPtr = create_pval("peerstats"); }
    break;

  case 92:
#line 551 "ntp_parser.y"
    { yyval.VoidPtr = create_pval("rawstats"); }
    break;

  case 93:
#line 553 "ntp_parser.y"
    { yyval.VoidPtr = create_pval("sysstats"); }
    break;

  case 94:
#line 557 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;

  case 95:
#line 558 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;

  case 96:
#line 562 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(T_File, yyvsp[0].String); }
    break;

  case 97:
#line 563 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Type, yyvsp[0].Integer); }
    break;

  case 98:
#line 564 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, T_Link); }
    break;

  case 99:
#line 565 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, T_Nolink); }
    break;

  case 100:
#line 566 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, T_Enable); }
    break;

  case 101:
#line 567 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, T_Disable); }
    break;

  case 102:
#line 571 "ntp_parser.y"
    { yyval.Integer = FILEGEN_NONE; }
    break;

  case 103:
#line 572 "ntp_parser.y"
    { yyval.Integer = FILEGEN_PID; }
    break;

  case 104:
#line 573 "ntp_parser.y"
    { yyval.Integer = FILEGEN_DAY; }
    break;

  case 105:
#line 574 "ntp_parser.y"
    { yyval.Integer = FILEGEN_WEEK; }
    break;

  case 106:
#line 575 "ntp_parser.y"
    { yyval.Integer = FILEGEN_MONTH; }
    break;

  case 107:
#line 576 "ntp_parser.y"
    { yyval.Integer = FILEGEN_YEAR; }
    break;

  case 108:
#line 577 "ntp_parser.y"
    { yyval.Integer = FILEGEN_AGE; }
    break;

  case 109:
#line 587 "ntp_parser.y"
    {   append_queue(my_config.discard_opts, yyvsp[0].Queue); }
    break;

  case 110:
#line 589 "ntp_parser.y"
    { 
                        enqueue(my_config.restrict_opts, 
                                create_restrict_node(yyvsp[-1].Address_node, NULL, yyvsp[0].Queue, ip_file->line_no));                          
                    }
    break;

  case 111:
#line 594 "ntp_parser.y"
    { 
                        enqueue(my_config.restrict_opts, 
                                create_restrict_node(NULL, NULL, yyvsp[0].Queue, ip_file->line_no)); 
                    }
    break;

  case 112:
#line 599 "ntp_parser.y"
    { 
                        enqueue(my_config.restrict_opts, 
                                create_restrict_node(yyvsp[-3].Address_node, yyvsp[-1].Address_node, yyvsp[0].Queue, ip_file->line_no)); 
                    }
    break;

  case 113:
#line 606 "ntp_parser.y"
    { yyval.Queue = create_queue(); }
    break;

  case 114:
#line 607 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); }
    break;

  case 115:
#line 611 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RES_IGNORE); }
    break;

  case 116:
#line 612 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RES_DEMOBILIZE); }
    break;

  case 117:
#line 613 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RES_LIMITED); }
    break;

  case 118:
#line 614 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RES_LPTRAP); }
    break;

  case 119:
#line 615 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RES_NOMODIFY); }
    break;

  case 120:
#line 616 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RES_NOPEER); }
    break;

  case 121:
#line 617 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RES_NOQUERY); }
    break;

  case 122:
#line 618 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RES_DONTSERVE); }
    break;

  case 123:
#line 619 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RES_NOTRAP); }
    break;

  case 124:
#line 620 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RES_DONTTRUST); }
    break;

  case 125:
#line 621 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RESM_NTPONLY); }
    break;

  case 126:
#line 622 "ntp_parser.y"
    { yyval.VoidPtr = create_ival(RES_VERSION); }
    break;

  case 127:
#line 626 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;

  case 128:
#line 627 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;

  case 129:
#line 631 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Average, yyvsp[0].Integer); }
    break;

  case 130:
#line 632 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Minimum, yyvsp[0].Integer); }
    break;

  case 131:
#line 633 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Monitor, yyvsp[0].Integer); }
    break;

  case 132:
#line 642 "ntp_parser.y"
    { enqueue(my_config.fudge, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  }
    break;

  case 133:
#line 646 "ntp_parser.y"
    { enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;

  case 134:
#line 647 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;

  case 135:
#line 652 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(CLK_HAVETIME1, yyvsp[0].Double); }
    break;

  case 136:
#line 654 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(CLK_HAVETIME2, yyvsp[0].Double); }
    break;

  case 137:
#line 656 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(CLK_HAVEVAL1,  yyvsp[0].Integer); }
    break;

  case 138:
#line 658 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(CLK_HAVEVAL2,  yyvsp[0].String); }
    break;

  case 139:
#line 660 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG1, yyvsp[0].Integer); }
    break;

  case 140:
#line 662 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG2, yyvsp[0].Integer); }
    break;

  case 141:
#line 664 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG3, yyvsp[0].Integer); }
    break;

  case 142:
#line 666 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG4, yyvsp[0].Integer); }
    break;

  case 143:
#line 675 "ntp_parser.y"
    { append_queue(my_config.enable_opts,yyvsp[0].Queue);  }
    break;

  case 144:
#line 677 "ntp_parser.y"
    { append_queue(my_config.disable_opts,yyvsp[0].Queue);  }
    break;

  case 145:
#line 681 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;

  case 146:
#line 682 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;

  case 147:
#line 686 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); }
    break;

  case 148:
#line 687 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, PROTO_BROADCLIENT); }
    break;

  case 149:
#line 688 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, PROTO_CAL); }
    break;

  case 150:
#line 689 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, PROTO_KERNEL); }
    break;

  case 151:
#line 690 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, PROTO_MONITOR); }
    break;

  case 152:
#line 691 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, PROTO_NTP); }
    break;

  case 153:
#line 693 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Flag, PROTO_FILEGEN); }
    break;

  case 154:
#line 701 "ntp_parser.y"
    { append_queue(my_config.tinker, yyvsp[0].Queue);  }
    break;

  case 155:
#line 705 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;

  case 156:
#line 706 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;

  case 157:
#line 710 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(LOOP_ALLAN, yyvsp[0].Double); }
    break;

  case 158:
#line 711 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(LOOP_PHI, yyvsp[0].Double); }
    break;

  case 159:
#line 712 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(LOOP_FREQ, yyvsp[0].Double); }
    break;

  case 160:
#line 713 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(LOOP_HUFFPUFF, yyvsp[0].Double); }
    break;

  case 161:
#line 714 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(LOOP_PANIC, yyvsp[0].Double); }
    break;

  case 162:
#line 715 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(LOOP_MAX, yyvsp[0].Double); }
    break;

  case 163:
#line 716 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(LOOP_MINSTEP, yyvsp[0].Double); }
    break;

  case 164:
#line 725 "ntp_parser.y"
    {
                    if (curr_include_level >= MAXINCLUDELEVEL) {
                        fprintf(stderr, "getconfig: Maximum include file level exceeded.\n");
                        msyslog(LOG_INFO, "getconfig: Maximum include file level exceeded.");
                    }
                    else {
                        fp[curr_include_level + 1] = F_OPEN(FindConfig(yyvsp[-1].String), "r");
                        if (fp[curr_include_level + 1] == NULL) {
                            fprintf(stderr, "getconfig: Couldn't open <%s>\n", FindConfig(yyvsp[-1].String));
                            msyslog(LOG_INFO, "getconfig: Couldn't open <%s>", FindConfig(yyvsp[-1].String));
                        }
                        else
                            ip_file = fp[++curr_include_level];
                    }
                }
    break;

  case 165:
#line 741 "ntp_parser.y"
    {
                    while (curr_include_level != -1) 
                        FCLOSE(fp[curr_include_level--]);
                }
    break;

  case 166:
#line 747 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, yyvsp[0].Double));  }
    break;

  case 167:
#line 749 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Calldelay, yyvsp[0].Integer));  }
    break;

  case 168:
#line 751 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_dval(T_Tick, yyvsp[0].Double));  }
    break;

  case 169:
#line 753 "ntp_parser.y"
    { /* Null action, possibly all null parms */ }
    break;

  case 170:
#line 756 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Pidfile, yyvsp[0].String));  }
    break;

  case 171:
#line 758 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Logfile, yyvsp[0].String));  }
    break;

  case 172:
#line 760 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_Automax, yyvsp[0].Integer));  }
    break;

  case 173:
#line 763 "ntp_parser.y"
    { append_queue(my_config.logconfig, yyvsp[0].Queue);  }
    break;

  case 174:
#line 765 "ntp_parser.y"
    { append_queue(my_config.phone, yyvsp[0].Queue);  }
    break;

  case 175:
#line 767 "ntp_parser.y"
    { enqueue(my_config.setvar, yyvsp[0].Set_var);  }
    break;

  case 176:
#line 769 "ntp_parser.y"
    { enqueue(my_config.trap, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  }
    break;

  case 177:
#line 771 "ntp_parser.y"
    { append_queue(my_config.ttl, yyvsp[0].Queue); }
    break;

  case 178:
#line 775 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_sval(T_Driftfile, yyvsp[0].String)); }
    break;

  case 179:
#line 777 "ntp_parser.y"
    { enqueue(my_config.vars, create_attr_ival(T_DriftMinutes, yyvsp[0].Integer)); 
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, yyvsp[-1].String)); }
    break;

  case 180:
#line 779 "ntp_parser.y"
    { /* Null driftfile,  indicated by null string "\0" */ 
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;

  case 181:
#line 785 "ntp_parser.y"
    { yyval.Set_var = create_setvar_node(yyvsp[-3].String, yyvsp[-1].String, DEF); }
    break;

  case 182:
#line 787 "ntp_parser.y"
    { yyval.Set_var = create_setvar_node(yyvsp[-2].String, yyvsp[0].String, 0); }
    break;

  case 183:
#line 792 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;

  case 184:
#line 793 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;

  case 185:
#line 797 "ntp_parser.y"
    { yyval.Attr_val = create_attr_ival(T_Port, yyvsp[0].Integer); }
    break;

  case 186:
#line 798 "ntp_parser.y"
    { yyval.Attr_val = create_attr_pval(T_Interface, yyvsp[0].Address_node); }
    break;

  case 187:
#line 803 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;

  case 188:
#line 804 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;

  case 189:
#line 808 "ntp_parser.y"
    { yyval.Attr_val = create_attr_sval(yyvsp[-1].Integer, yyvsp[0].String); }
    break;

  case 190:
#line 810 "ntp_parser.y"
    { 
                    /* YUCK!! This is needed because '+' and '-' are not special characters 
                     * while '=' is. 
                     * We really need a better way of defining strings
                     */
                    char prefix = yyvsp[0].String[0];
                    char *type = &(yyvsp[0].String[1]);
                    if (prefix != '+' && prefix != '-') {
                        yyerror("Logconfig prefix is not '+', '-' or '='\n");
                    }
                    else
                        yyval.Attr_val = create_attr_sval(prefix, type);
                }
    break;

  case 191:
#line 826 "ntp_parser.y"
    { yyval.Integer = '+'; }
    break;

  case 192:
#line 827 "ntp_parser.y"
    { yyval.Integer = '-'; }
    break;

  case 193:
#line 828 "ntp_parser.y"
    { yyval.Integer = '='; }
    break;

  case 194:
#line 837 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, create_ival(yyvsp[0].Integer)); }
    break;

  case 195:
#line 838 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(create_ival(yyvsp[0].Integer)); }
    break;

  case 196:
#line 842 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, create_pval(yyvsp[0].String)); }
    break;

  case 197:
#line 843 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(create_pval(yyvsp[0].String)); }
    break;

  case 198:
#line 847 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Address_node); }
    break;

  case 199:
#line 848 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Address_node); }
    break;

  case 200:
#line 853 "ntp_parser.y"
    { 
                    if (yyvsp[0].Integer != 0 && yyvsp[0].Integer != 1) {
                        yyerror("Integer value is not boolean (0 or 1). Assuming 1");
                        yyval.Integer = 1;
                    }
                    else
                        yyval.Integer = yyvsp[0].Integer; 
                }
    break;

  case 201:
#line 861 "ntp_parser.y"
    { yyval.Integer = 1; }
    break;

  case 202:
#line 862 "ntp_parser.y"
    { yyval.Integer = 0; }
    break;

  case 203:
#line 866 "ntp_parser.y"
    { yyval.Double = (double)yyvsp[0].Integer; }
    break;

  case 204:
#line 867 "ntp_parser.y"
    { yyval.Double = yyvsp[0].Double; }
    break;

  case 205:
#line 876 "ntp_parser.y"
    {
                 my_config.sim_details = create_sim_node(yyvsp[-2].Queue, yyvsp[-1].Queue);

                 /* Reset the old_config_style variable */
                 old_config_style = 1;
             }
    break;

  case 206:
#line 890 "ntp_parser.y"
    { old_config_style = 0; }
    break;

  case 207:
#line 894 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); }
    break;

  case 208:
#line 895 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); }
    break;

  case 209:
#line 899 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(T_Beep_Delay, yyvsp[0].Double); }
    break;

  case 210:
#line 900 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(T_Sim_Duration, yyvsp[0].Double); }
    break;

  case 211:
#line 904 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_server); }
    break;

  case 212:
#line 905 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_server); }
    break;

  case 213:
#line 910 "ntp_parser.y"
    { yyval.Sim_server = create_sim_server(yyvsp[-4].Address_node, yyvsp[-2].Double, yyvsp[-1].Queue); }
    break;

  case 214:
#line 914 "ntp_parser.y"
    { yyval.Double = yyvsp[-1].Double; }
    break;

  case 215:
#line 918 "ntp_parser.y"
    { yyval.Address_node = yyvsp[0].Address_node; }
    break;

  case 216:
#line 922 "ntp_parser.y"
    { yyval.Address_node = yyvsp[0].Address_node; }
    break;

  case 217:
#line 923 "ntp_parser.y"
    { yyval.Address_node = create_address_node(yyvsp[0].String, T_String); }
    break;

  case 218:
#line 927 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_script); }
    break;

  case 219:
#line 928 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_script); }
    break;

  case 220:
#line 933 "ntp_parser.y"
    { yyval.Sim_script = create_sim_script_info(yyvsp[-3].Double, yyvsp[-1].Queue); }
    break;

  case 221:
#line 937 "ntp_parser.y"
    { yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); }
    break;

  case 222:
#line 938 "ntp_parser.y"
    { yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); }
    break;

  case 223:
#line 943 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(T_Freq_Offset, yyvsp[0].Double); }
    break;

  case 224:
#line 945 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(T_Wander, yyvsp[0].Double); }
    break;

  case 225:
#line 947 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(T_Jitter, yyvsp[0].Double); }
    break;

  case 226:
#line 949 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(T_Prop_Delay, yyvsp[0].Double); }
    break;

  case 227:
#line 951 "ntp_parser.y"
    { yyval.Attr_val = create_attr_dval(T_Proc_Delay, yyvsp[0].Double); }
    break;


    }

/* Line 1016 of /usr/local/share/bison/yacc.c.  */
#line 2844 "ntp_parser.c"

  yyvsp -= yylen;
  yyssp -= yylen;


#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyssp > yyss)
	    {
	      YYDPRINTF ((stderr, "Error: popping "));
	      YYDSYMPRINT ((stderr,
			    yystos[*yyssp],
			    *yyvsp));
	      YYDPRINTF ((stderr, "\n"));
	      yydestruct (yystos[*yyssp], *yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yydestruct (yychar1, yylval);
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

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

      YYDPRINTF ((stderr, "Error: popping "));
      YYDSYMPRINT ((stderr,
		    yystos[*yyssp], *yyvsp));
      YYDPRINTF ((stderr, "\n"));

      yydestruct (yystos[yystate], *yyvsp);
      yyvsp--;
      yystate = *--yyssp;


#if YYDEBUG
      if (yydebug)
	{
	  short *yyssp1 = yyss - 1;
	  YYFPRINTF (stderr, "Error: state stack now");
	  while (yyssp1 != yyssp)
	    YYFPRINTF (stderr, " %d", *++yyssp1);
	  YYFPRINTF (stderr, "\n");
	}
#endif
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 955 "ntp_parser.y"


/* KEYWORDS
 * --------
 */

void yyerror (char *msg)
{
    int retval;
    if (input_from_file)
        fprintf(stderr, "%s\n", msg);
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


