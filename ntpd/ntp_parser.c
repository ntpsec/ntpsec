/* A Bison parser, made from /backroom/ntp-dev/A.whimsy-noopenssl/ntpd/../../ntpd/ntp_parser.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	T_Age	257
# define	T_Allan	258
# define	T_Auth	259
# define	T_Autokey	260
# define	T_Automax	261
# define	T_Average	262
# define	T_Bclient	263
# define	T_Beacon	264
# define	T_Broadcast	265
# define	T_Broadcastclient	266
# define	T_Broadcastdelay	267
# define	T_Burst	268
# define	T_Calibrate	269
# define	T_Calldelay	270
# define	T_Ceiling	271
# define	T_Cert	272
# define	T_Clockstats	273
# define	T_Cohort	274
# define	T_ControlKey	275
# define	T_Crypto	276
# define	T_Cryptostats	277
# define	T_Day	278
# define	T_Default	279
# define	T_Disable	280
# define	T_Discard	281
# define	T_Dispersion	282
# define	T_Double	283
# define	T_Driftfile	284
# define	T_WanderThreshold	285
# define	T_Enable	286
# define	T_End	287
# define	T_False	288
# define	T_File	289
# define	T_Filegen	290
# define	T_Flag1	291
# define	T_Flag2	292
# define	T_Flag3	293
# define	T_Flag4	294
# define	T_Floor	295
# define	T_Freq	296
# define	T_Fudge	297
# define	T_Gqpar	298
# define	T_Host	299
# define	T_Huffpuff	300
# define	T_IPv4_address	301
# define	T_IPv6_address	302
# define	T_Iburst	303
# define	T_Ident	304
# define	T_Iffpar	305
# define	T_Ignore	306
# define	T_Includefile	307
# define	T_Integer	308
# define	T_Interface	309
# define	T_Kernel	310
# define	T_Key	311
# define	T_Keys	312
# define	T_Keysdir	313
# define	T_Kod	314
# define	T_Leap	315
# define	T_Leapfile	316
# define	T_Limited	317
# define	T_Link	318
# define	T_Logconfig	319
# define	T_Logfile	320
# define	T_Loopstats	321
# define	T_Lowpriotrap	322
# define	T_Manycastclient	323
# define	T_Manycastserver	324
# define	T_Mask	325
# define	T_Maxclock	326
# define	T_Maxdist	327
# define	T_Maxhop	328
# define	T_Maxpoll	329
# define	T_Minclock	330
# define	T_Mindist	331
# define	T_Minimum	332
# define	T_Minpoll	333
# define	T_Minsane	334
# define	T_Mode	335
# define	T_Monitor	336
# define	T_Month	337
# define	T_Multicastclient	338
# define	T_Mvpar	339
# define	T_Nolink	340
# define	T_Nomodify	341
# define	T_None	342
# define	T_Nopeer	343
# define	T_Noquery	344
# define	T_Noselect	345
# define	T_Noserve	346
# define	T_Notrap	347
# define	T_Notrust	348
# define	T_Novolley	349
# define	T_Ntp	350
# define	T_Ntpport	351
# define	T_Orphan	352
# define	T_Panic	353
# define	T_Peer	354
# define	T_Peerstats	355
# define	T_Phone	356
# define	T_Pid	357
# define	T_Pidfile	358
# define	T_Pool	359
# define	T_Port	360
# define	T_Pps	361
# define	T_Preempt	362
# define	T_Prefer	363
# define	T_Pw	364
# define	T_RandFile	365
# define	T_Rawstats	366
# define	T_Refid	367
# define	T_Requestkey	368
# define	T_Restrict	369
# define	T_Revoke	370
# define	T_Server	371
# define	T_Setvar	372
# define	T_Sign	373
# define	T_Statistics	374
# define	T_Stats	375
# define	T_Statsdir	376
# define	T_Step	377
# define	T_Stepout	378
# define	T_Stratum	379
# define	T_String	380
# define	T_Sysstats	381
# define	T_Tick	382
# define	T_Time1	383
# define	T_Time2	384
# define	T_Tinker	385
# define	T_Tos	386
# define	T_Trap	387
# define	T_True	388
# define	T_Trustedkey	389
# define	T_Ttl	390
# define	T_Type	391
# define	T_Version	392
# define	T_Week	393
# define	T_Year	394
# define	T_Flag	395
# define	T_Void	396
# define	T_EOC	397
# define	T_Simulate	398
# define	T_Beep_Delay	399
# define	T_Sim_Duration	400
# define	T_Server_Offset	401
# define	T_Duration	402
# define	T_Freq_Offset	403
# define	T_Wander	404
# define	T_Jitter	405
# define	T_Prop_Delay	406
# define	T_Proc_Delay	407

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

  #define YYERROR_VERBOSE
  void yyerror (char *msg);
  extern int input_from_file;  /* 0=input from ntpq>config command buffer */

#line 48 "ntp_parser.y"
#ifndef YYSTYPE
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
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 1
#endif



#define	YYFINAL		363
#define	YYFLAG		-32768
#define	YYNTBASE	159

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 407 ? yytranslate[x] : 221)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const short yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   155,     2,   156,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   154,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   157,     2,   158,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     6,     9,    12,    13,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    39,    42,
      44,    46,    48,    50,    52,    54,    56,    59,    61,    63,
      66,    68,    70,    72,    74,    77,    80,    83,    85,    87,
      89,    91,    94,    97,   100,   102,   105,   108,   111,   114,
     117,   120,   123,   126,   129,   132,   135,   137,   140,   143,
     146,   149,   152,   155,   158,   161,   164,   167,   170,   173,
     176,   178,   181,   184,   187,   190,   193,   196,   199,   202,
     205,   208,   211,   214,   217,   221,   224,   226,   228,   230,
     232,   234,   236,   238,   241,   243,   246,   249,   251,   253,
     255,   257,   259,   261,   263,   265,   267,   269,   271,   274,
     278,   282,   288,   289,   292,   294,   296,   298,   300,   302,
     304,   306,   308,   310,   312,   314,   316,   319,   321,   324,
     327,   330,   334,   337,   339,   342,   345,   348,   351,   354,
     357,   360,   363,   366,   369,   372,   374,   376,   378,   380,
     382,   384,   386,   388,   391,   394,   396,   399,   402,   405,
     408,   411,   414,   417,   421,   423,   426,   429,   432,   435,
     438,   441,   444,   447,   450,   453,   456,   460,   463,   465,
     468,   469,   474,   478,   481,   483,   486,   489,   492,   494,
     497,   499,   501,   503,   505,   508,   510,   513,   515,   518,
     520,   522,   524,   526,   528,   530,   536,   538,   542,   545,
     549,   553,   556,   558,   564,   569,   573,   575,   577,   580,
     582,   589,   593,   596,   600,   604,   608,   612
};
static const short yyrhs[] =
{
     160,     0,   160,   161,   143,     0,   161,   143,     0,     1,
     143,     0,     0,   162,     0,   168,     0,   169,     0,   175,
       0,   181,     0,   172,     0,   186,     0,   189,     0,   192,
       0,   195,     0,   208,     0,   163,   164,   166,     0,   163,
     164,     0,   117,     0,   105,     0,   100,     0,    11,     0,
      69,     0,   165,     0,   126,     0,    54,   126,     0,    47,
       0,    48,     0,   166,   167,     0,   167,     0,     6,     0,
      14,     0,    49,     0,    57,    54,     0,    79,    54,     0,
      75,    54,     0,    91,     0,   108,     0,   109,     0,   134,
       0,   136,    54,     0,    81,    54,     0,   138,    54,     0,
      12,     0,    12,    95,     0,    70,   205,     0,    84,   205,
       0,     6,    54,     0,    21,    54,     0,    22,   170,     0,
      58,   126,     0,    59,   126,     0,   114,    54,     0,   135,
     203,     0,   170,   171,     0,   171,     0,    18,   126,     0,
      44,   126,     0,    45,   126,     0,    50,   126,     0,    51,
     126,     0,    61,   126,     0,    85,   126,     0,   110,   126,
       0,   111,   126,     0,   116,    54,     0,   119,   126,     0,
     132,   173,     0,   173,   174,     0,   174,     0,    17,    54,
       0,    41,    54,     0,    20,   206,     0,    98,    54,     0,
      77,   207,     0,    73,   207,     0,    76,   207,     0,    72,
     207,     0,    80,    54,     0,    10,    54,     0,    74,    54,
       0,   120,   176,     0,   122,   126,     0,    36,   177,   178,
       0,   176,   177,     0,   177,     0,    19,     0,    23,     0,
      67,     0,   101,     0,   112,     0,   127,     0,   178,   179,
       0,   179,     0,    35,   126,     0,   137,   180,     0,    64,
       0,    86,     0,    32,     0,    26,     0,    88,     0,   103,
       0,    24,     0,   139,     0,    83,     0,   140,     0,     3,
       0,    27,   184,     0,   115,   164,   182,     0,   115,    25,
     182,     0,   115,   165,    71,   165,   182,     0,     0,   182,
     183,     0,    52,     0,    60,     0,    63,     0,    68,     0,
      87,     0,    89,     0,    90,     0,    92,     0,    93,     0,
      94,     0,    97,     0,   138,     0,   184,   185,     0,   185,
       0,     8,    54,     0,    78,    54,     0,    82,    54,     0,
      43,   164,   187,     0,   187,   188,     0,   188,     0,   129,
     207,     0,   130,   207,     0,   125,    54,     0,   113,   126,
       0,    37,   206,     0,    38,   206,     0,    39,   206,     0,
      40,   206,     0,    32,   190,     0,    26,   190,     0,   190,
     191,     0,   191,     0,     5,     0,     9,     0,    15,     0,
      56,     0,    82,     0,    96,     0,   121,     0,   131,   193,
       0,   193,   194,     0,   194,     0,     4,   207,     0,    28,
     207,     0,    42,   207,     0,    46,   207,     0,    99,   207,
       0,   123,   207,     0,   124,   207,     0,    53,   126,   161,
       0,    33,     0,    13,   207,     0,    16,    54,     0,   128,
     207,     0,    30,   196,     0,    62,   126,     0,   104,   126,
       0,    66,   126,     0,     7,    54,     0,    65,   200,     0,
     102,   204,     0,   118,   197,     0,   133,   165,   198,     0,
     136,   203,     0,   126,     0,   126,    29,     0,     0,   126,
     154,   126,    25,     0,   126,   154,   126,     0,   198,   199,
       0,   199,     0,   106,    54,     0,    55,   165,     0,   200,
     201,     0,   201,     0,   202,   126,     0,   126,     0,   155,
       0,   156,     0,   154,     0,   203,    54,     0,    54,     0,
     204,   126,     0,   126,     0,   205,   164,     0,   164,     0,
      54,     0,   134,     0,    34,     0,    54,     0,    29,     0,
     209,   157,   210,   212,   158,     0,   144,     0,   210,   211,
     143,     0,   211,   143,     0,   145,   154,   207,     0,   146,
     154,   207,     0,   212,   213,     0,   213,     0,   215,   157,
     214,   217,   158,     0,   147,   154,   207,   143,     0,   117,
     154,   216,     0,   165,     0,   126,     0,   217,   218,     0,
     218,     0,   148,   154,   207,   157,   219,   158,     0,   219,
     220,   143,     0,   220,   143,     0,   149,   154,   207,     0,
     150,   154,   207,     0,   151,   154,   207,     0,   152,   154,
     207,     0,   153,   154,   207,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   280,   284,   285,   290,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   320,   326,   335,
     336,   337,   338,   339,   343,   344,   345,   361,   362,   366,
     367,   372,   373,   374,   375,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   394,   396,   398,   400,   411,   413,
     415,   421,   423,   425,   427,   432,   433,   437,   439,   441,
     443,   445,   447,   449,   451,   453,   455,   457,   467,   472,
     473,   477,   479,   481,   483,   485,   487,   489,   491,   493,
     495,   497,   507,   509,   511,   519,   520,   524,   526,   528,
     530,   532,   534,   539,   540,   544,   545,   546,   547,   548,
     549,   553,   554,   555,   556,   557,   558,   559,   568,   570,
     575,   580,   588,   589,   593,   594,   595,   596,   597,   598,
     599,   600,   601,   602,   603,   604,   608,   609,   613,   614,
     615,   623,   628,   629,   633,   635,   637,   639,   641,   643,
     645,   647,   656,   658,   663,   664,   668,   669,   670,   671,
     672,   673,   675,   683,   687,   688,   692,   693,   694,   695,
     696,   697,   698,   706,   722,   728,   730,   732,   734,   736,
     739,   741,   743,   746,   748,   750,   752,   754,   758,   760,
     763,   768,   770,   776,   777,   781,   782,   787,   788,   792,
     793,   810,   811,   812,   821,   822,   826,   827,   831,   832,
     836,   845,   846,   850,   851,   859,   874,   878,   879,   883,
     884,   888,   889,   893,   898,   902,   906,   907,   911,   912,
     916,   921,   922,   926,   928,   930,   932,   934
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "T_Age", "T_Allan", "T_Auth", "T_Autokey", 
  "T_Automax", "T_Average", "T_Bclient", "T_Beacon", "T_Broadcast", 
  "T_Broadcastclient", "T_Broadcastdelay", "T_Burst", "T_Calibrate", 
  "T_Calldelay", "T_Ceiling", "T_Cert", "T_Clockstats", "T_Cohort", 
  "T_ControlKey", "T_Crypto", "T_Cryptostats", "T_Day", "T_Default", 
  "T_Disable", "T_Discard", "T_Dispersion", "T_Double", "T_Driftfile", 
  "T_WanderThreshold", "T_Enable", "T_End", "T_False", "T_File", 
  "T_Filegen", "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4", "T_Floor", 
  "T_Freq", "T_Fudge", "T_Gqpar", "T_Host", "T_Huffpuff", 
  "T_IPv4_address", "T_IPv6_address", "T_Iburst", "T_Ident", "T_Iffpar", 
  "T_Ignore", "T_Includefile", "T_Integer", "T_Interface", "T_Kernel", 
  "T_Key", "T_Keys", "T_Keysdir", "T_Kod", "T_Leap", "T_Leapfile", 
  "T_Limited", "T_Link", "T_Logconfig", "T_Logfile", "T_Loopstats", 
  "T_Lowpriotrap", "T_Manycastclient", "T_Manycastserver", "T_Mask", 
  "T_Maxclock", "T_Maxdist", "T_Maxhop", "T_Maxpoll", "T_Minclock", 
  "T_Mindist", "T_Minimum", "T_Minpoll", "T_Minsane", "T_Mode", 
  "T_Monitor", "T_Month", "T_Multicastclient", "T_Mvpar", "T_Nolink", 
  "T_Nomodify", "T_None", "T_Nopeer", "T_Noquery", "T_Noselect", 
  "T_Noserve", "T_Notrap", "T_Notrust", "T_Novolley", "T_Ntp", 
  "T_Ntpport", "T_Orphan", "T_Panic", "T_Peer", "T_Peerstats", "T_Phone", 
  "T_Pid", "T_Pidfile", "T_Pool", "T_Port", "T_Pps", "T_Preempt", 
  "T_Prefer", "T_Pw", "T_RandFile", "T_Rawstats", "T_Refid", 
  "T_Requestkey", "T_Restrict", "T_Revoke", "T_Server", "T_Setvar", 
  "T_Sign", "T_Statistics", "T_Stats", "T_Statsdir", "T_Step", 
  "T_Stepout", "T_Stratum", "T_String", "T_Sysstats", "T_Tick", "T_Time1", 
  "T_Time2", "T_Tinker", "T_Tos", "T_Trap", "T_True", "T_Trustedkey", 
  "T_Ttl", "T_Type", "T_Version", "T_Week", "T_Year", "T_Flag", "T_Void", 
  "T_EOC", "T_Simulate", "T_Beep_Delay", "T_Sim_Duration", 
  "T_Server_Offset", "T_Duration", "T_Freq_Offset", "T_Wander", 
  "T_Jitter", "T_Prop_Delay", "T_Proc_Delay", "'='", "'+'", "'-'", "'{'", 
  "'}'", "configuration", "command_list", "command", "server_command", 
  "client_type", "address", "ip_address", "option_list", "option", 
  "other_mode_command", "authentication_command", "crypto_command_list", 
  "crypto_command", "orphan_mode_command", "tos_option_list", 
  "tos_option", "monitoring_command", "stats_list", "stat", 
  "filegen_option_list", "filegen_option", "filegen_type", 
  "access_control_command", "ac_flag_list", "access_control_flag", 
  "discard_option_list", "discard_option", "fudge_command", 
  "fudge_factor_list", "fudge_factor", "system_option_command", 
  "system_option_list", "system_option", "tinker_command", 
  "tinker_option_list", "tinker_option", "miscellaneous_command", 
  "drift_parm", "variable_assign", "trap_option_list", "trap_option", 
  "log_config_list", "log_config_command", "log_config_prefix", 
  "integer_list", "string_list", "address_list", "boolean", "number", 
  "simulate_command", "sim_conf_start", "sim_init_statement_list", 
  "sim_init_statement", "sim_server_list", "sim_server", 
  "sim_server_offset", "sim_server_name", "sim_address", "sim_act_list", 
  "sim_act", "sim_act_stmt_list", "sim_act_stmt", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,   159,   160,   160,   160,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   162,   162,   163,
     163,   163,   163,   163,   164,   164,   164,   165,   165,   166,
     166,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   168,   168,   168,   168,   169,   169,
     169,   169,   169,   169,   169,   170,   170,   171,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   172,   173,
     173,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   175,   175,   175,   176,   176,   177,   177,   177,
     177,   177,   177,   178,   178,   179,   179,   179,   179,   179,
     179,   180,   180,   180,   180,   180,   180,   180,   181,   181,
     181,   181,   182,   182,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   184,   184,   185,   185,
     185,   186,   187,   187,   188,   188,   188,   188,   188,   188,
     188,   188,   189,   189,   190,   190,   191,   191,   191,   191,
     191,   191,   191,   192,   193,   193,   194,   194,   194,   194,
     194,   194,   194,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   196,   196,
     196,   197,   197,   198,   198,   199,   199,   200,   200,   201,
     201,   202,   202,   202,   203,   203,   204,   204,   205,   205,
     206,   206,   206,   207,   207,   208,   209,   210,   210,   211,
     211,   212,   212,   213,   214,   215,   216,   216,   217,   217,
     218,   219,   219,   220,   220,   220,   220,   220
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     3,     2,     2,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     2,
       1,     1,     1,     1,     2,     2,     2,     1,     1,     1,
       1,     2,     2,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       3,     5,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     2,
       2,     3,     2,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     3,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     3,     2,     1,     2,
       0,     4,     3,     2,     1,     2,     2,     2,     1,     2,
       1,     1,     1,     1,     2,     1,     2,     1,     2,     1,
       1,     1,     1,     1,     1,     5,     1,     3,     2,     3,
       3,     2,     1,     5,     4,     3,     1,     1,     2,     1,
       6,     3,     2,     3,     3,     3,     3,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     0,     0,     0,    22,    44,     0,     0,     0,     0,
       0,     0,   180,     0,   164,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,    21,     0,     0,    20,
       0,     0,    19,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   206,     1,     0,     6,     0,     7,     8,    11,
       9,    10,    12,    13,    14,    15,    16,     0,     4,    48,
     172,    45,   204,   203,   165,   166,    49,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    50,    56,
     146,   147,   148,   149,   150,   151,   152,   143,   145,     0,
       0,     0,   108,   127,   178,   168,   142,    87,    88,    89,
      90,    91,    92,     0,    27,    28,     0,    25,     0,    24,
       5,    51,    52,   169,   190,   193,   191,   192,   173,   188,
       0,   171,   199,    46,    47,   197,   174,   170,    53,   112,
     112,    24,     0,   175,    82,    86,    83,   167,     0,     0,
       0,     0,     0,     0,     0,   153,   155,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    68,    70,
       0,   195,    54,   177,     0,     3,    18,     0,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    55,
     144,   128,   129,   130,   126,   179,   100,    99,     0,    97,
      98,     0,    84,    94,    26,     0,     0,     0,     0,     0,
       0,     0,     0,   131,   133,   163,   187,   189,   198,   196,
     110,   109,     0,     0,    85,   156,   157,   158,   159,   160,
     161,   162,   154,    80,    71,   202,   200,   201,    73,    72,
      78,    76,    81,    77,    75,    79,    74,    69,     0,     0,
     176,   184,   194,     2,    31,    32,    33,     0,     0,     0,
       0,    37,    38,    39,    40,     0,     0,    17,    30,     0,
       0,     0,     0,    95,   107,   103,   105,   101,   102,   104,
     106,    96,    93,   138,   139,   140,   141,   137,   136,   134,
     135,   132,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   113,   112,   182,   186,   185,   183,
      34,    36,    35,    42,    41,    43,    29,     0,     0,     0,
       0,     0,   212,     0,   208,   111,   181,   209,   210,     0,
     207,   205,   211,     0,   217,   216,   215,     0,     0,     0,
       0,     0,   219,     0,     0,   213,   218,   214,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   220,     0,   222,   223,   224,   225,   226,   227,
     221,     0,     0,     0
};

static const short yydefgoto[] =
{
     361,    43,    44,    45,    46,   122,   109,   257,   258,    47,
      48,    78,    79,    49,   158,   159,    50,   134,   103,   192,
     193,   271,    51,   210,   294,    92,    93,    52,   203,   204,
      53,    87,    88,    54,   145,   146,    55,    95,   133,   240,
     241,   118,   119,   120,   162,   126,   123,   228,    64,    56,
      57,   261,   262,   311,   312,   328,   313,   326,   331,   332,
     345,   346
};

static const short yypact[] =
{
       9,  -125,   -28,   -21,-32768,   -57,   -10,    -4,     2,   184,
      -1,    -5,   -62,    -1,-32768,    24,   -41,   -50,   -46,   -40,
     -37,    31,   -34,-32768,   -41,   -41,-32768,   -32,   -16,-32768,
      34,    12,-32768,     4,    24,     7,   -10,    23,   324,    10,
      36,    36,-32768,   148,    -8,-32768,   -41,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,   -61,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,    13,    30,    32,
      45,    46,    47,    50,    53,    57,    95,    62,   184,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,    -1,-32768,    96,
     108,   123,    -5,-32768,   163,-32768,    -1,-32768,-32768,-32768,
  -32768,-32768,-32768,    -3,-32768,-32768,    67,-32768,   220,-32768,
     310,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    31,-32768,
      68,-32768,-32768,   -41,   -41,-32768,    69,-32768,-32768,-32768,
  -32768,   127,    49,-32768,    24,-32768,-32768,-32768,   -10,   -10,
     -10,   -10,   -10,   -10,   -10,    23,-32768,   150,   151,    -6,
     155,   -10,   -10,   157,   -10,   -10,   158,   161,   324,-32768,
      15,-32768,   162,   162,    76,-32768,   342,  -134,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    94,-32768,
  -32768,    60,    -3,-32768,-32768,    -6,    -6,    -6,    -6,   100,
     173,   -10,   -10,   220,-32768,-32768,-32768,-32768,-32768,-32768,
     366,   366,    10,   105,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    10,   179,
      15,-32768,-32768,-32768,-32768,-32768,-32768,   187,   188,   189,
     190,-32768,-32768,-32768,-32768,   192,   193,   342,-32768,    97,
     101,    51,   106,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,   229,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,   -10,   -10,   102,
     118,   -71,-32768,   107,-32768,   366,-32768,-32768,-32768,     6,
  -32768,-32768,-32768,   120,-32768,-32768,-32768,   119,   126,   -10,
     121,  -124,-32768,   134,   -10,-32768,-32768,-32768,   125,    87,
     124,   131,   132,   133,   136,    72,   145,   -10,   -10,   -10,
     -10,   -10,-32768,   153,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,   297,   299,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,   -38,-32768,-32768,    66,   -30,-32768,    44,-32768,
  -32768,-32768,   224,-32768,-32768,   146,-32768,-32768,   -17,-32768,
     113,-32768,-32768,  -128,-32768,-32768,   214,-32768,-32768,   104,
  -32768,   295,   -47,-32768,-32768,   164,-32768,-32768,-32768,-32768,
      70,-32768,   200,-32768,   278,-32768,   300,   -97,   -36,-32768,
  -32768,-32768,    59,-32768,    16,-32768,-32768,-32768,-32768,    -7,
  -32768,   -15
};


#define	YYLAST		504


static const short yytable[] =
{
     137,   131,   211,    89,    80,   164,   104,   105,    81,   160,
       1,   259,   260,   106,    82,     2,     3,   135,    58,    62,
       4,     5,     6,   186,   330,     7,    59,   138,   225,   187,
       8,     9,   188,    60,   335,    10,    11,   129,    61,    12,
     180,    13,    14,    97,    63,    15,   309,    98,   226,   180,
      65,   139,    16,   104,   105,    83,    66,   104,   105,   104,
     105,   189,    17,   264,    94,   140,   106,    18,    19,   141,
     238,    20,   205,    90,    21,    22,   110,    91,    23,    24,
     111,    84,   108,   190,   265,   107,   112,   321,   128,   113,
     161,    99,   121,    25,   125,    85,   167,   130,   273,   274,
     275,   276,   215,   216,   217,   218,   219,   220,   221,    26,
     127,    27,   166,    28,    29,   230,   231,   214,   233,   234,
      86,   239,   142,    30,    31,   100,    32,    33,   227,    34,
     132,    35,   324,   136,   191,   165,   101,    36,   107,   168,
      37,    38,    39,   266,    40,    41,   143,   144,   267,   177,
     181,   102,    -5,    42,     2,     3,   169,   114,   170,     4,
       5,     6,   182,   268,     7,   279,   280,   315,   309,     8,
       9,   171,   172,   173,    10,    11,   174,   183,    12,   175,
      13,    14,   295,   176,    15,   115,   116,   117,   178,   208,
     208,    16,   185,   194,   207,   209,   259,   260,   212,   269,
     270,    17,    67,   213,   223,   224,    18,    19,   297,   229,
      20,   232,   235,    21,    22,   236,   242,    23,    24,   243,
     263,   340,   341,   342,   343,   344,   277,   278,    68,    69,
     352,   296,    25,   298,    70,    71,   340,   341,   342,   343,
     344,   300,   301,   302,   303,    72,   304,   305,    26,   314,
      27,   307,    28,    29,   316,   308,   319,   195,   196,   197,
     198,   320,    30,    31,   323,    32,    33,   327,    34,    73,
      35,   317,   318,   329,   330,   334,    36,   337,   347,    37,
      38,    39,   339,    40,    41,   348,   349,   350,   354,   325,
     351,    -5,    42,   333,    74,    75,   360,   362,   338,   363,
      76,   306,   179,    77,   237,   272,   184,   281,    96,   222,
     299,   355,   356,   357,   358,   359,     2,     3,   206,   163,
     310,     4,     5,     6,   336,   124,     7,   322,     0,     0,
     353,     8,     9,   199,   147,     0,    10,    11,     0,     0,
      12,   148,    13,    14,   149,   200,    15,     0,   244,   201,
     202,     0,     0,    16,     0,     0,   245,     0,     0,     0,
       0,     0,     0,    17,     0,   150,     0,     0,    18,    19,
       0,     0,    20,     0,     0,    21,    22,     0,     0,    23,
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   246,     0,     0,    25,     0,   151,   152,   153,   247,
     154,   155,     0,     0,   156,     0,     0,     0,     0,     0,
      26,     0,    27,     0,    28,    29,     0,   248,   282,     0,
       0,   249,   157,   250,    30,    31,   283,    32,    33,   284,
      34,     0,    35,   251,   285,     0,     0,     0,    36,     0,
       0,    37,    38,    39,     0,    40,    41,     0,     0,     0,
     252,   253,     0,   286,    42,   287,   288,     0,   289,   290,
     291,     0,     0,   292,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   254,     0,   255,     0,
     256,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   293
};

static const short yycheck[] =
{
      36,    31,   130,     8,     5,    43,    47,    48,     9,    39,
       1,   145,   146,    54,    15,     6,     7,    34,   143,    29,
      11,    12,    13,    26,   148,    16,    54,     4,    34,    32,
      21,    22,    35,    54,   158,    26,    27,    25,    95,    30,
      87,    32,    33,    19,    54,    36,   117,    23,    54,    96,
      54,    28,    43,    47,    48,    56,    54,    47,    48,    47,
      48,    64,    53,     3,   126,    42,    54,    58,    59,    46,
      55,    62,   110,    78,    65,    66,   126,    82,    69,    70,
     126,    82,    16,    86,    24,   126,   126,   158,    54,   126,
      54,    67,   126,    84,   126,    96,   157,    31,   195,   196,
     197,   198,   138,   139,   140,   141,   142,   143,   144,   100,
     126,   102,    46,   104,   105,   151,   152,   134,   154,   155,
     121,   106,    99,   114,   115,   101,   117,   118,   134,   120,
     126,   122,   126,   126,   137,   143,   112,   128,   126,   126,
     131,   132,   133,    83,   135,   136,   123,   124,    88,    54,
      54,   127,   143,   144,     6,     7,   126,   126,   126,    11,
      12,    13,    54,   103,    16,   201,   202,   295,   117,    21,
      22,   126,   126,   126,    26,    27,   126,    54,    30,   126,
      32,    33,   212,   126,    36,   154,   155,   156,   126,   123,
     124,    43,    29,   126,   126,   126,   145,   146,    71,   139,
     140,    53,    18,   154,    54,    54,    58,    59,   238,    54,
      62,    54,    54,    65,    66,    54,    54,    69,    70,   143,
     126,   149,   150,   151,   152,   153,   126,    54,    44,    45,
     158,   126,    84,    54,    50,    51,   149,   150,   151,   152,
     153,    54,    54,    54,    54,    61,    54,    54,   100,   143,
     102,   154,   104,   105,    25,   154,   154,    37,    38,    39,
      40,   143,   114,   115,   157,   117,   118,   147,   120,    85,
     122,   307,   308,   154,   148,   154,   128,   143,   154,   131,
     132,   133,   157,   135,   136,   154,   154,   154,   143,   319,
     154,   143,   144,   329,   110,   111,   143,     0,   334,     0,
     116,   257,    78,   119,   158,   192,    92,   203,    13,   145,
     240,   347,   348,   349,   350,   351,     6,     7,   118,    41,
     261,    11,    12,    13,   331,    25,    16,   311,    -1,    -1,
     345,    21,    22,   113,    10,    -1,    26,    27,    -1,    -1,
      30,    17,    32,    33,    20,   125,    36,    -1,     6,   129,
     130,    -1,    -1,    43,    -1,    -1,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    -1,    41,    -1,    -1,    58,    59,
      -1,    -1,    62,    -1,    -1,    65,    66,    -1,    -1,    69,
      70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    -1,    -1,    84,    -1,    72,    73,    74,    57,
      76,    77,    -1,    -1,    80,    -1,    -1,    -1,    -1,    -1,
     100,    -1,   102,    -1,   104,   105,    -1,    75,    52,    -1,
      -1,    79,    98,    81,   114,   115,    60,   117,   118,    63,
     120,    -1,   122,    91,    68,    -1,    -1,    -1,   128,    -1,
      -1,   131,   132,   133,    -1,   135,   136,    -1,    -1,    -1,
     108,   109,    -1,    87,   144,    89,    90,    -1,    92,    93,
      94,    -1,    -1,    97,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   134,    -1,   136,    -1,
     138,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   138
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/gnu/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

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
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

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
	    (To)[yyi] = (From)[yyi];		\
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
#define YYABORT 	goto yyabortlab
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
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


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
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
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

#ifdef YYERROR_VERBOSE

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
#endif

#line 315 "/usr/local/gnu/share/bison/bison.simple"


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

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

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

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
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
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
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
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
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
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

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
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 4:
#line 291 "ntp_parser.y"
{
					if (input_from_file == 1) {
			msyslog(LOG_INFO, "parse error %s line %d ignored\n",
                            ip_file->fname, ip_file->line_no);
			} else if (input_from_file != 0)
				msyslog(LOG_INFO,
				    "parse: bad boolean input flag\n");
                }
    break;
case 17:
#line 321 "ntp_parser.y"
{
                        struct peer_node *my_node =  create_peer_node(yyvsp[-2].Integer, yyvsp[-1].Address_node, yyvsp[0].Queue);
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;
case 18:
#line 327 "ntp_parser.y"
{
                        struct peer_node *my_node = create_peer_node(yyvsp[-1].Integer, yyvsp[0].Address_node, NULL);
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;
case 19:
#line 335 "ntp_parser.y"
{ yyval.Integer = T_Server; }
    break;
case 20:
#line 336 "ntp_parser.y"
{ yyval.Integer = T_Pool; }
    break;
case 21:
#line 337 "ntp_parser.y"
{ yyval.Integer = T_Peer; }
    break;
case 22:
#line 338 "ntp_parser.y"
{ yyval.Integer = T_Broadcast; }
    break;
case 23:
#line 339 "ntp_parser.y"
{ yyval.Integer = T_Manycastclient; }
    break;
case 24:
#line 343 "ntp_parser.y"
{ yyval.Address_node = yyvsp[0].Address_node; }
    break;
case 25:
#line 344 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, default_ai_family); }
    break;
case 26:
#line 346 "ntp_parser.y"
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
case 27:
#line 361 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET); }
    break;
case 28:
#line 362 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET6); }
    break;
case 29:
#line 366 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 30:
#line 367 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 31:
#line 372 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_SKEY | FLAG_AUTHENABLE); }
    break;
case 32:
#line 373 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_BURST); }
    break;
case 33:
#line 374 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_IBURST); }
    break;
case 34:
#line 375 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Key, yyvsp[0].Integer); }
    break;
case 35:
#line 376 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Minpoll, yyvsp[0].Integer); }
    break;
case 36:
#line 377 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Maxpoll, yyvsp[0].Integer); }
    break;
case 37:
#line 378 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_NOSELECT);}
    break;
case 38:
#line 379 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREEMPT); }
    break;
case 39:
#line 380 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREFER); }
    break;
case 40:
#line 381 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_TRUE); }
    break;
case 41:
#line 382 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Ttl, yyvsp[0].Integer); }
    break;
case 42:
#line 383 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Mode, yyvsp[0].Integer); }
    break;
case 43:
#line 384 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Version, yyvsp[0].Integer); }
    break;
case 44:
#line 395 "ntp_parser.y"
{ my_config.broadcastclient = SIMPLE; }
    break;
case 45:
#line 397 "ntp_parser.y"
{ my_config.broadcastclient = NOVOLLEY;  }
    break;
case 46:
#line 399 "ntp_parser.y"
{ append_queue(my_config.manycastserver, yyvsp[0].Queue);  }
    break;
case 47:
#line 401 "ntp_parser.y"
{ append_queue(my_config.multicastclient, yyvsp[0].Queue);  }
    break;
case 48:
#line 412 "ntp_parser.y"
{ my_config.auth.autokey = yyvsp[0].Integer;  }
    break;
case 49:
#line 414 "ntp_parser.y"
{ my_config.auth.control_key = yyvsp[0].Integer;  }
    break;
case 50:
#line 416 "ntp_parser.y"
{ if (my_config.auth.crypto_cmd_list != NULL)
					append_queue(my_config.auth.crypto_cmd_list, yyvsp[0].Queue);
				else
					my_config.auth.crypto_cmd_list = yyvsp[0].Queue;
			}
    break;
case 51:
#line 422 "ntp_parser.y"
{ my_config.auth.keys = yyvsp[0].String;  }
    break;
case 52:
#line 424 "ntp_parser.y"
{ my_config.auth.keysdir = yyvsp[0].String;  }
    break;
case 53:
#line 426 "ntp_parser.y"
{ my_config.auth.requested_key = yyvsp[0].Integer;  }
    break;
case 54:
#line 428 "ntp_parser.y"
{ my_config.auth.trusted_key_list = yyvsp[0].Queue;  }
    break;
case 55:
#line 432 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 56:
#line 433 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 57:
#line 438 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_CERT, yyvsp[0].String); }
    break;
case 58:
#line 440 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_GQPAR, yyvsp[0].String); }
    break;
case 59:
#line 442 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PRIV, yyvsp[0].String); }
    break;
case 60:
#line 444 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_IDENT, yyvsp[0].String); }
    break;
case 61:
#line 446 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_IFFPAR, yyvsp[0].String); }
    break;
case 62:
#line 448 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_LEAP, yyvsp[0].String); }
    break;
case 63:
#line 450 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_MVPAR, yyvsp[0].String); }
    break;
case 64:
#line 452 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PW, yyvsp[0].String); }
    break;
case 65:
#line 454 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_RAND, yyvsp[0].String); }
    break;
case 66:
#line 456 "ntp_parser.y"
{ my_config.auth.revoke = yyvsp[0].Integer;  }
    break;
case 67:
#line 458 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_SIGN, yyvsp[0].String); }
    break;
case 68:
#line 468 "ntp_parser.y"
{ append_queue(my_config.orphan_cmds,yyvsp[0].Queue);  }
    break;
case 69:
#line 472 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 70:
#line 473 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 71:
#line 478 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_CEILING, (double)yyvsp[0].Integer); }
    break;
case 72:
#line 480 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_FLOOR, (double)yyvsp[0].Integer); }
    break;
case 73:
#line 482 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_COHORT, (double)yyvsp[0].Integer); }
    break;
case 74:
#line 484 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_ORPHAN, (double)yyvsp[0].Integer); }
    break;
case 75:
#line 486 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINDISP, yyvsp[0].Double); }
    break;
case 76:
#line 488 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXDIST, yyvsp[0].Double); }
    break;
case 77:
#line 490 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINCLOCK, yyvsp[0].Double); }
    break;
case 78:
#line 492 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXCLOCK, yyvsp[0].Double); }
    break;
case 79:
#line 494 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINSANE, (double)yyvsp[0].Integer); }
    break;
case 80:
#line 496 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_BEACON, (double)yyvsp[0].Integer); }
    break;
case 81:
#line 498 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXHOP, (double)yyvsp[0].Integer); }
    break;
case 82:
#line 508 "ntp_parser.y"
{ append_queue(my_config.stats_list, yyvsp[0].Queue);  }
    break;
case 83:
#line 510 "ntp_parser.y"
{ my_config.stats_dir = yyvsp[0].String;  }
    break;
case 84:
#line 512 "ntp_parser.y"
{
                        enqueue(my_config.filegen_opts,
                                create_filegen_node(yyvsp[-1].VoidPtr, yyvsp[0].Queue));
                    }
    break;
case 85:
#line 519 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); }
    break;
case 86:
#line 520 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].VoidPtr); }
    break;
case 87:
#line 525 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("clockstats"); }
    break;
case 88:
#line 527 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("cryptostats"); }
    break;
case 89:
#line 529 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("loopstats"); }
    break;
case 90:
#line 531 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("peerstats"); }
    break;
case 91:
#line 533 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("rawstats"); }
    break;
case 92:
#line 535 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("sysstats"); }
    break;
case 93:
#line 539 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 94:
#line 540 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 95:
#line 544 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(T_File, yyvsp[0].String); }
    break;
case 96:
#line 545 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Type, yyvsp[0].Integer); }
    break;
case 97:
#line 546 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Link); }
    break;
case 98:
#line 547 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Nolink); }
    break;
case 99:
#line 548 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Enable); }
    break;
case 100:
#line 549 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Disable); }
    break;
case 101:
#line 553 "ntp_parser.y"
{ yyval.Integer = FILEGEN_NONE; }
    break;
case 102:
#line 554 "ntp_parser.y"
{ yyval.Integer = FILEGEN_PID; }
    break;
case 103:
#line 555 "ntp_parser.y"
{ yyval.Integer = FILEGEN_DAY; }
    break;
case 104:
#line 556 "ntp_parser.y"
{ yyval.Integer = FILEGEN_WEEK; }
    break;
case 105:
#line 557 "ntp_parser.y"
{ yyval.Integer = FILEGEN_MONTH; }
    break;
case 106:
#line 558 "ntp_parser.y"
{ yyval.Integer = FILEGEN_YEAR; }
    break;
case 107:
#line 559 "ntp_parser.y"
{ yyval.Integer = FILEGEN_AGE; }
    break;
case 108:
#line 569 "ntp_parser.y"
{   append_queue(my_config.discard_opts, yyvsp[0].Queue); }
    break;
case 109:
#line 571 "ntp_parser.y"
{
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(yyvsp[-1].Address_node, NULL, yyvsp[0].Queue, ip_file->line_no));
                    }
    break;
case 110:
#line 576 "ntp_parser.y"
{
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(NULL, NULL, yyvsp[0].Queue, ip_file->line_no));
                    }
    break;
case 111:
#line 581 "ntp_parser.y"
{
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(yyvsp[-3].Address_node, yyvsp[-1].Address_node, yyvsp[0].Queue, ip_file->line_no));
                    }
    break;
case 112:
#line 588 "ntp_parser.y"
{ yyval.Queue = create_queue(); }
    break;
case 113:
#line 589 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); }
    break;
case 114:
#line 593 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_IGNORE); }
    break;
case 115:
#line 594 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_DEMOBILIZE); }
    break;
case 116:
#line 595 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_LIMITED); }
    break;
case 117:
#line 596 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_LPTRAP); }
    break;
case 118:
#line 597 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOMODIFY); }
    break;
case 119:
#line 598 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOPEER); }
    break;
case 120:
#line 599 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOQUERY); }
    break;
case 121:
#line 600 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_DONTSERVE); }
    break;
case 122:
#line 601 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOTRAP); }
    break;
case 123:
#line 602 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_DONTTRUST); }
    break;
case 124:
#line 603 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RESM_NTPONLY); }
    break;
case 125:
#line 604 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_VERSION); }
    break;
case 126:
#line 608 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 127:
#line 609 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 128:
#line 613 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Average, yyvsp[0].Integer); }
    break;
case 129:
#line 614 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Minimum, yyvsp[0].Integer); }
    break;
case 130:
#line 615 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Monitor, yyvsp[0].Integer); }
    break;
case 131:
#line 624 "ntp_parser.y"
{ enqueue(my_config.fudge, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  }
    break;
case 132:
#line 628 "ntp_parser.y"
{ enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 133:
#line 629 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 134:
#line 634 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(CLK_HAVETIME1, yyvsp[0].Double); }
    break;
case 135:
#line 636 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(CLK_HAVETIME2, yyvsp[0].Double); }
    break;
case 136:
#line 638 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEVAL1,  yyvsp[0].Integer); }
    break;
case 137:
#line 640 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CLK_HAVEVAL2,  yyvsp[0].String); }
    break;
case 138:
#line 642 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG1, yyvsp[0].Integer); }
    break;
case 139:
#line 644 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG2, yyvsp[0].Integer); }
    break;
case 140:
#line 646 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG3, yyvsp[0].Integer); }
    break;
case 141:
#line 648 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG4, yyvsp[0].Integer); }
    break;
case 142:
#line 657 "ntp_parser.y"
{ append_queue(my_config.enable_opts,yyvsp[0].Queue);  }
    break;
case 143:
#line 659 "ntp_parser.y"
{ append_queue(my_config.disable_opts,yyvsp[0].Queue);  }
    break;
case 144:
#line 663 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 145:
#line 664 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 146:
#line 668 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); }
    break;
case 147:
#line 669 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_BROADCLIENT); }
    break;
case 148:
#line 670 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_CAL); }
    break;
case 149:
#line 671 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_KERNEL); }
    break;
case 150:
#line 672 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_MONITOR); }
    break;
case 151:
#line 673 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_NTP); }
    break;
case 152:
#line 675 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_FILEGEN); }
    break;
case 153:
#line 683 "ntp_parser.y"
{ append_queue(my_config.tinker, yyvsp[0].Queue);  }
    break;
case 154:
#line 687 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 155:
#line 688 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 156:
#line 692 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_ALLAN, yyvsp[0].Double); }
    break;
case 157:
#line 693 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_PHI, yyvsp[0].Double); }
    break;
case 158:
#line 694 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_FREQ, yyvsp[0].Double); }
    break;
case 159:
#line 695 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_HUFFPUFF, yyvsp[0].Double); }
    break;
case 160:
#line 696 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_PANIC, yyvsp[0].Double); }
    break;
case 161:
#line 697 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_MAX, yyvsp[0].Double); }
    break;
case 162:
#line 698 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_MINSTEP, yyvsp[0].Double); }
    break;
case 163:
#line 707 "ntp_parser.y"
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
case 164:
#line 723 "ntp_parser.y"
{
                    while (curr_include_level != -1)
                        FCLOSE(fp[curr_include_level--]);
                }
    break;
case 165:
#line 729 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, yyvsp[0].Double));  }
    break;
case 166:
#line 731 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_ival(T_Calldelay, yyvsp[0].Integer));  }
    break;
case 167:
#line 733 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_dval(T_Tick, yyvsp[0].Double));  }
    break;
case 168:
#line 735 "ntp_parser.y"
{ /* Null action, possibly all null parms */ }
    break;
case 169:
#line 737 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Leapfile, yyvsp[0].String)); }
    break;
case 170:
#line 740 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Pidfile, yyvsp[0].String));  }
    break;
case 171:
#line 742 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Logfile, yyvsp[0].String));  }
    break;
case 172:
#line 744 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_ival(T_Automax, yyvsp[0].Integer));  }
    break;
case 173:
#line 747 "ntp_parser.y"
{ append_queue(my_config.logconfig, yyvsp[0].Queue);  }
    break;
case 174:
#line 749 "ntp_parser.y"
{ append_queue(my_config.phone, yyvsp[0].Queue);  }
    break;
case 175:
#line 751 "ntp_parser.y"
{ enqueue(my_config.setvar, yyvsp[0].Set_var);  }
    break;
case 176:
#line 753 "ntp_parser.y"
{ enqueue(my_config.trap, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  }
    break;
case 177:
#line 755 "ntp_parser.y"
{ append_queue(my_config.ttl, yyvsp[0].Queue); }
    break;
case 178:
#line 759 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Driftfile, yyvsp[0].String)); }
    break;
case 179:
#line 761 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_dval(T_WanderThreshold, yyvsp[0].Double));
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, yyvsp[-1].String)); }
    break;
case 180:
#line 763 "ntp_parser.y"
{ /* Null driftfile,  indicated by null string "\0" */
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;
case 181:
#line 769 "ntp_parser.y"
{ yyval.Set_var = create_setvar_node(yyvsp[-3].String, yyvsp[-1].String, DEF); }
    break;
case 182:
#line 771 "ntp_parser.y"
{ yyval.Set_var = create_setvar_node(yyvsp[-2].String, yyvsp[0].String, 0); }
    break;
case 183:
#line 776 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 184:
#line 777 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 185:
#line 781 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Port, yyvsp[0].Integer); }
    break;
case 186:
#line 782 "ntp_parser.y"
{ yyval.Attr_val = create_attr_pval(T_Interface, yyvsp[0].Address_node); }
    break;
case 187:
#line 787 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 188:
#line 788 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 189:
#line 792 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(yyvsp[-1].Integer, yyvsp[0].String); }
    break;
case 190:
#line 794 "ntp_parser.y"
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
#line 810 "ntp_parser.y"
{ yyval.Integer = '+'; }
    break;
case 192:
#line 811 "ntp_parser.y"
{ yyval.Integer = '-'; }
    break;
case 193:
#line 812 "ntp_parser.y"
{ yyval.Integer = '='; }
    break;
case 194:
#line 821 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, create_ival(yyvsp[0].Integer)); }
    break;
case 195:
#line 822 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(create_ival(yyvsp[0].Integer)); }
    break;
case 196:
#line 826 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, create_pval(yyvsp[0].String)); }
    break;
case 197:
#line 827 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(create_pval(yyvsp[0].String)); }
    break;
case 198:
#line 831 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Address_node); }
    break;
case 199:
#line 832 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Address_node); }
    break;
case 200:
#line 837 "ntp_parser.y"
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
#line 845 "ntp_parser.y"
{ yyval.Integer = 1; }
    break;
case 202:
#line 846 "ntp_parser.y"
{ yyval.Integer = 0; }
    break;
case 203:
#line 850 "ntp_parser.y"
{ yyval.Double = (double)yyvsp[0].Integer; }
    break;
case 204:
#line 851 "ntp_parser.y"
{ yyval.Double = yyvsp[0].Double; }
    break;
case 205:
#line 860 "ntp_parser.y"
{
                 my_config.sim_details = create_sim_node(yyvsp[-2].Queue, yyvsp[-1].Queue);

                 /* Reset the old_config_style variable */
                 old_config_style = 1;
             }
    break;
case 206:
#line 874 "ntp_parser.y"
{ old_config_style = 0; }
    break;
case 207:
#line 878 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); }
    break;
case 208:
#line 879 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); }
    break;
case 209:
#line 883 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Beep_Delay, yyvsp[0].Double); }
    break;
case 210:
#line 884 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Sim_Duration, yyvsp[0].Double); }
    break;
case 211:
#line 888 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_server); }
    break;
case 212:
#line 889 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_server); }
    break;
case 213:
#line 894 "ntp_parser.y"
{ yyval.Sim_server = create_sim_server(yyvsp[-4].Address_node, yyvsp[-2].Double, yyvsp[-1].Queue); }
    break;
case 214:
#line 898 "ntp_parser.y"
{ yyval.Double = yyvsp[-1].Double; }
    break;
case 215:
#line 902 "ntp_parser.y"
{ yyval.Address_node = yyvsp[0].Address_node; }
    break;
case 216:
#line 906 "ntp_parser.y"
{ yyval.Address_node = yyvsp[0].Address_node; }
    break;
case 217:
#line 907 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, T_String); }
    break;
case 218:
#line 911 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_script); }
    break;
case 219:
#line 912 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_script); }
    break;
case 220:
#line 917 "ntp_parser.y"
{ yyval.Sim_script = create_sim_script_info(yyvsp[-3].Double, yyvsp[-1].Queue); }
    break;
case 221:
#line 921 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); }
    break;
case 222:
#line 922 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); }
    break;
case 223:
#line 927 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Freq_Offset, yyvsp[0].Double); }
    break;
case 224:
#line 929 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Wander, yyvsp[0].Double); }
    break;
case 225:
#line 931 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Jitter, yyvsp[0].Double); }
    break;
case 226:
#line 933 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Prop_Delay, yyvsp[0].Double); }
    break;
case 227:
#line 935 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Proc_Delay, yyvsp[0].Double); }
    break;
}

#line 705 "/usr/local/gnu/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

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
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
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
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

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

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 939 "ntp_parser.y"


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

