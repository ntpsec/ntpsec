/* A Bison parser, made from /usa/utterback/ntp-dev/ntpd/ntp_parser.y
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
# define	T_Bias	265
# define	T_Broadcast	266
# define	T_Broadcastclient	267
# define	T_Broadcastdelay	268
# define	T_Burst	269
# define	T_Calibrate	270
# define	T_Calldelay	271
# define	T_Ceiling	272
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
# define	T_Flake	295
# define	T_Floor	296
# define	T_Freq	297
# define	T_Fudge	298
# define	T_Host	299
# define	T_Huffpuff	300
# define	T_IPv4_address	301
# define	T_IPv6_address	302
# define	T_Iburst	303
# define	T_Ident	304
# define	T_Ignore	305
# define	T_Includefile	306
# define	T_Integer	307
# define	T_Interface	308
# define	T_Kernel	309
# define	T_Key	310
# define	T_Keys	311
# define	T_Keysdir	312
# define	T_Kod	313
# define	T_Leap	314
# define	T_Leapfile	315
# define	T_Limited	316
# define	T_Link	317
# define	T_Logconfig	318
# define	T_Logfile	319
# define	T_Loopstats	320
# define	T_Lowpriotrap	321
# define	T_Manycastclient	322
# define	T_Manycastserver	323
# define	T_Mask	324
# define	T_Maxclock	325
# define	T_Maxdist	326
# define	T_Maxhop	327
# define	T_Maxpoll	328
# define	T_Minclock	329
# define	T_Mindist	330
# define	T_Minimum	331
# define	T_Minpoll	332
# define	T_Minsane	333
# define	T_Mode	334
# define	T_Monitor	335
# define	T_Month	336
# define	T_Multicastclient	337
# define	T_Nolink	338
# define	T_Nomodify	339
# define	T_None	340
# define	T_Nopeer	341
# define	T_Noquery	342
# define	T_Noselect	343
# define	T_Noserve	344
# define	T_Notrap	345
# define	T_Notrust	346
# define	T_Novolley	347
# define	T_Ntp	348
# define	T_Ntpport	349
# define	T_Orphan	350
# define	T_Panic	351
# define	T_Peer	352
# define	T_Peerstats	353
# define	T_Phone	354
# define	T_Pid	355
# define	T_Pidfile	356
# define	T_Pool	357
# define	T_Port	358
# define	T_Pps	359
# define	T_Preempt	360
# define	T_Prefer	361
# define	T_Protostats	362
# define	T_Pw	363
# define	T_Qos	364
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
# define	T_Timingstats	385
# define	T_Tinker	386
# define	T_Tos	387
# define	T_Trap	388
# define	T_True	389
# define	T_Trustedkey	390
# define	T_Ttl	391
# define	T_Type	392
# define	T_Version	393
# define	T_Week	394
# define	T_Xleave	395
# define	T_Year	396
# define	T_Flag	397
# define	T_Void	398
# define	T_EOC	399
# define	T_Simulate	400
# define	T_Beep_Delay	401
# define	T_Sim_Duration	402
# define	T_Server_Offset	403
# define	T_Duration	404
# define	T_Freq_Offset	405
# define	T_Wander	406
# define	T_Jitter	407
# define	T_Prop_Delay	408
# define	T_Proc_Delay	409

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
  extern int cryptosw;

#line 49 "ntp_parser.y"
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



#define	YYFINAL		355
#define	YYFLAG		-32768
#define	YYNTBASE	159

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 409 ? yytranslate[x] : 220)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const short yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   156,     2,     2,     2,     2,     2,     2,     2,     2,
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
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     6,     9,    12,    13,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    39,    42,
      44,    46,    48,    50,    52,    54,    57,    60,    62,    65,
      67,    69,    72,    74,    76,    79,    82,    85,    87,    89,
      91,    93,    95,    98,   101,   104,   106,   109,   112,   115,
     118,   121,   124,   127,   130,   133,   136,   138,   139,   142,
     144,   147,   150,   153,   156,   159,   162,   165,   168,   170,
     173,   176,   179,   182,   185,   188,   191,   194,   197,   200,
     203,   206,   209,   213,   216,   218,   220,   222,   224,   226,
     228,   230,   232,   234,   237,   239,   242,   245,   247,   249,
     251,   253,   255,   257,   259,   261,   263,   265,   267,   270,
     274,   278,   284,   285,   288,   290,   292,   294,   296,   298,
     300,   302,   304,   306,   308,   310,   312,   314,   317,   319,
     322,   325,   328,   332,   335,   337,   340,   343,   346,   349,
     352,   355,   358,   361,   364,   367,   370,   372,   374,   376,
     378,   380,   382,   384,   386,   389,   392,   394,   397,   400,
     403,   406,   409,   412,   415,   419,   421,   424,   427,   430,
     433,   436,   439,   442,   445,   448,   451,   454,   458,   461,
     464,   466,   469,   470,   475,   479,   482,   484,   487,   490,
     493,   495,   497,   500,   502,   505,   507,   510,   512,   514,
     516,   518,   520,   522,   528,   530,   534,   537,   541,   545,
     548,   550,   556,   561,   565,   568,   570,   577,   581,   584,
     588,   592,   596,   600
};
static const short yyrhs[] =
{
     160,     0,   160,   161,   145,     0,   161,   145,     0,     1,
     145,     0,     0,   162,     0,   168,     0,   169,     0,   176,
       0,   182,     0,   173,     0,   187,     0,   190,     0,   193,
       0,   196,     0,   208,     0,   163,   164,   166,     0,   163,
     164,     0,   117,     0,   103,     0,    98,     0,    12,     0,
      68,     0,   165,     0,    47,   126,     0,    48,   126,     0,
     126,     0,   166,   167,     0,   167,     0,     6,     0,    11,
     207,     0,    15,     0,    49,     0,    56,    53,     0,    78,
      53,     0,    74,    53,     0,    89,     0,   106,     0,   107,
       0,   135,     0,   141,     0,   137,    53,     0,    80,    53,
       0,   139,    53,     0,    13,     0,    13,    93,     0,    69,
     205,     0,    83,   205,     0,     6,    53,     0,    21,    53,
       0,    22,   170,     0,    57,   126,     0,    58,   126,     0,
     114,    53,     0,   136,   203,     0,   171,     0,     0,   171,
     172,     0,   172,     0,    45,   126,     0,    50,   126,     0,
     109,   126,     0,   111,   126,     0,   116,    53,     0,   119,
     126,     0,   133,   174,     0,   174,   175,     0,   175,     0,
      18,    53,     0,    42,    53,     0,    20,   206,     0,    96,
      53,     0,    76,   207,     0,    72,   207,     0,    75,   207,
       0,    71,   207,     0,    79,    53,     0,    10,    53,     0,
      73,    53,     0,   120,   177,     0,   122,   126,     0,    36,
     178,   179,     0,   177,   178,     0,   178,     0,    19,     0,
      23,     0,    66,     0,    99,     0,   112,     0,   127,     0,
     131,     0,   108,     0,   179,   180,     0,   180,     0,    35,
     126,     0,   138,   181,     0,    63,     0,    84,     0,    32,
       0,    26,     0,    86,     0,   101,     0,    24,     0,   140,
       0,    82,     0,   142,     0,     3,     0,    27,   185,     0,
     115,   164,   183,     0,   115,    25,   183,     0,   115,   165,
      70,   165,   183,     0,     0,   183,   184,     0,    41,     0,
      51,     0,    59,     0,    62,     0,    67,     0,    85,     0,
      87,     0,    88,     0,    90,     0,    91,     0,    92,     0,
      95,     0,   139,     0,   185,   186,     0,   186,     0,     8,
      53,     0,    77,    53,     0,    81,    53,     0,    44,   164,
     188,     0,   188,   189,     0,   189,     0,   129,   207,     0,
     130,   207,     0,   125,    53,     0,   113,   126,     0,    37,
     206,     0,    38,   206,     0,    39,   206,     0,    40,   206,
       0,    32,   191,     0,    26,   191,     0,   191,   192,     0,
     192,     0,     5,     0,     9,     0,    16,     0,    55,     0,
      81,     0,    94,     0,   121,     0,   132,   194,     0,   194,
     195,     0,   195,     0,     4,   207,     0,    28,   207,     0,
      43,   207,     0,    46,   207,     0,    97,   207,     0,   123,
     207,     0,   124,   207,     0,    52,   126,   161,     0,    33,
       0,    14,   207,     0,    17,    53,     0,   128,   207,     0,
      30,   197,     0,    61,   126,     0,   102,   126,     0,    65,
     126,     0,     7,    53,     0,    64,   201,     0,   100,   204,
       0,   118,   198,     0,   134,   165,   199,     0,   137,   203,
       0,   110,   126,     0,   126,     0,   126,    29,     0,     0,
     126,   156,   126,    25,     0,   126,   156,   126,     0,   199,
     200,     0,   200,     0,   104,    53,     0,    54,   165,     0,
     201,   202,     0,   202,     0,   126,     0,   203,    53,     0,
      53,     0,   204,   126,     0,   126,     0,   205,   164,     0,
     164,     0,    53,     0,   135,     0,    34,     0,    53,     0,
      29,     0,   209,   157,   210,   212,   158,     0,   146,     0,
     210,   211,   145,     0,   211,   145,     0,   147,   156,   207,
       0,   148,   156,   207,     0,   212,   213,     0,   213,     0,
     215,   157,   214,   216,   158,     0,   149,   156,   207,   145,
       0,   117,   156,   164,     0,   216,   217,     0,   217,     0,
     150,   156,   207,   157,   218,   158,     0,   218,   219,   145,
       0,   219,   145,     0,   151,   156,   207,     0,   152,   156,
     207,     0,   153,   156,   207,     0,   154,   156,   207,     0,
     155,   156,   207,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   281,   285,   286,   291,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   321,   327,   336,
     337,   338,   339,   340,   344,   345,   346,   350,   354,   355,
     360,   361,   362,   363,   364,   365,   366,   367,   368,   369,
     370,   371,   372,   373,   374,   384,   386,   388,   390,   401,
     403,   405,   412,   414,   416,   418,   423,   424,   429,   430,
     434,   436,   438,   440,   442,   444,   454,   459,   460,   464,
     466,   468,   470,   472,   474,   476,   478,   480,   482,   484,
     494,   496,   498,   506,   507,   511,   513,   515,   517,   519,
     521,   523,   525,   530,   531,   535,   536,   537,   538,   539,
     540,   544,   545,   546,   547,   548,   549,   550,   559,   561,
     566,   571,   579,   580,   584,   585,   586,   587,   588,   589,
     590,   591,   592,   593,   594,   595,   596,   600,   601,   605,
     606,   607,   615,   620,   621,   625,   627,   629,   631,   633,
     635,   637,   639,   648,   650,   655,   656,   660,   661,   662,
     663,   664,   665,   667,   675,   679,   680,   684,   685,   686,
     687,   688,   689,   690,   698,   714,   720,   722,   724,   726,
     728,   731,   733,   735,   738,   740,   742,   744,   746,   748,
     752,   754,   757,   762,   764,   770,   771,   775,   776,   781,
     782,   786,   803,   804,   808,   809,   813,   814,   818,   827,
     828,   832,   833,   841,   856,   860,   861,   865,   866,   870,
     871,   875,   880,   884,   888,   889,   893,   898,   899,   903,
     905,   907,   909,   911
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "T_Age", "T_Allan", "T_Auth", "T_Autokey", 
  "T_Automax", "T_Average", "T_Bclient", "T_Beacon", "T_Bias", 
  "T_Broadcast", "T_Broadcastclient", "T_Broadcastdelay", "T_Burst", 
  "T_Calibrate", "T_Calldelay", "T_Ceiling", "T_Clockstats", "T_Cohort", 
  "T_ControlKey", "T_Crypto", "T_Cryptostats", "T_Day", "T_Default", 
  "T_Disable", "T_Discard", "T_Dispersion", "T_Double", "T_Driftfile", 
  "T_WanderThreshold", "T_Enable", "T_End", "T_False", "T_File", 
  "T_Filegen", "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4", "T_Flake", 
  "T_Floor", "T_Freq", "T_Fudge", "T_Host", "T_Huffpuff", 
  "T_IPv4_address", "T_IPv6_address", "T_Iburst", "T_Ident", "T_Ignore", 
  "T_Includefile", "T_Integer", "T_Interface", "T_Kernel", "T_Key", 
  "T_Keys", "T_Keysdir", "T_Kod", "T_Leap", "T_Leapfile", "T_Limited", 
  "T_Link", "T_Logconfig", "T_Logfile", "T_Loopstats", "T_Lowpriotrap", 
  "T_Manycastclient", "T_Manycastserver", "T_Mask", "T_Maxclock", 
  "T_Maxdist", "T_Maxhop", "T_Maxpoll", "T_Minclock", "T_Mindist", 
  "T_Minimum", "T_Minpoll", "T_Minsane", "T_Mode", "T_Monitor", "T_Month", 
  "T_Multicastclient", "T_Nolink", "T_Nomodify", "T_None", "T_Nopeer", 
  "T_Noquery", "T_Noselect", "T_Noserve", "T_Notrap", "T_Notrust", 
  "T_Novolley", "T_Ntp", "T_Ntpport", "T_Orphan", "T_Panic", "T_Peer", 
  "T_Peerstats", "T_Phone", "T_Pid", "T_Pidfile", "T_Pool", "T_Port", 
  "T_Pps", "T_Preempt", "T_Prefer", "T_Protostats", "T_Pw", "T_Qos", 
  "T_RandFile", "T_Rawstats", "T_Refid", "T_Requestkey", "T_Restrict", 
  "T_Revoke", "T_Server", "T_Setvar", "T_Sign", "T_Statistics", "T_Stats", 
  "T_Statsdir", "T_Step", "T_Stepout", "T_Stratum", "T_String", 
  "T_Sysstats", "T_Tick", "T_Time1", "T_Time2", "T_Timingstats", 
  "T_Tinker", "T_Tos", "T_Trap", "T_True", "T_Trustedkey", "T_Ttl", 
  "T_Type", "T_Version", "T_Week", "T_Xleave", "T_Year", "T_Flag", 
  "T_Void", "T_EOC", "T_Simulate", "T_Beep_Delay", "T_Sim_Duration", 
  "T_Server_Offset", "T_Duration", "T_Freq_Offset", "T_Wander", 
  "T_Jitter", "T_Prop_Delay", "T_Proc_Delay", "'='", "'{'", "'}'", 
  "configuration", "command_list", "command", "server_command", 
  "client_type", "address", "ip_address", "option_list", "option", 
  "other_mode_command", "authentication_command", "crypto_command_line", 
  "crypto_command_list", "crypto_command", "orphan_mode_command", 
  "tos_option_list", "tos_option", "monitoring_command", "stats_list", 
  "stat", "filegen_option_list", "filegen_option", "filegen_type", 
  "access_control_command", "ac_flag_list", "access_control_flag", 
  "discard_option_list", "discard_option", "fudge_command", 
  "fudge_factor_list", "fudge_factor", "system_option_command", 
  "system_option_list", "system_option", "tinker_command", 
  "tinker_option_list", "tinker_option", "miscellaneous_command", 
  "drift_parm", "variable_assign", "trap_option_list", "trap_option", 
  "log_config_list", "log_config_command", "integer_list", "string_list", 
  "address_list", "boolean", "number", "simulate_command", 
  "sim_conf_start", "sim_init_statement_list", "sim_init_statement", 
  "sim_server_list", "sim_server", "sim_server_offset", "sim_server_name", 
  "sim_act_list", "sim_act", "sim_act_stmt_list", "sim_act_stmt", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,   159,   160,   160,   160,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   162,   162,   163,
     163,   163,   163,   163,   164,   164,   164,   165,   166,   166,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   168,   168,   168,   168,   169,
     169,   169,   169,   169,   169,   169,   170,   170,   171,   171,
     172,   172,   172,   172,   172,   172,   173,   174,   174,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     176,   176,   176,   177,   177,   178,   178,   178,   178,   178,
     178,   178,   178,   179,   179,   180,   180,   180,   180,   180,
     180,   181,   181,   181,   181,   181,   181,   181,   182,   182,
     182,   182,   183,   183,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   185,   185,   186,
     186,   186,   187,   188,   188,   189,   189,   189,   189,   189,
     189,   189,   189,   190,   190,   191,   191,   192,   192,   192,
     192,   192,   192,   192,   193,   194,   194,   195,   195,   195,
     195,   195,   195,   195,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     197,   197,   197,   198,   198,   199,   199,   200,   200,   201,
     201,   202,   203,   203,   204,   204,   205,   205,   206,   206,
     206,   207,   207,   208,   209,   210,   210,   211,   211,   212,
     212,   213,   214,   215,   216,   216,   217,   218,   218,   219,
     219,   219,   219,   219
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     3,     2,     2,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     2,     1,
       1,     1,     1,     1,     1,     2,     2,     1,     2,     1,
       1,     2,     1,     1,     2,     2,     2,     1,     1,     1,
       1,     1,     2,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     0,     2,     1,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       3,     5,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       2,     2,     3,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     3,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     3,     2,     2,
       1,     2,     0,     4,     3,     2,     1,     2,     2,     2,
       1,     1,     2,     1,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     5,     1,     3,     2,     3,     3,     2,
       1,     5,     4,     3,     2,     1,     6,     3,     2,     3,
       3,     3,     3,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     0,     0,     0,    22,    45,     0,     0,     0,    57,
       0,     0,   182,     0,   165,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,    21,     0,     0,    20,
       0,     0,     0,    19,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   204,     1,     0,     6,     0,     7,     8,
      11,     9,    10,    12,    13,    14,    15,    16,     0,     4,
      49,   173,    46,   202,   201,   166,   167,    50,     0,     0,
       0,     0,     0,     0,    51,    56,    59,   147,   148,   149,
     150,   151,   152,   153,   144,   146,     0,     0,     0,   108,
     128,   180,   169,   143,    85,    86,    87,    88,    92,    89,
      90,    91,     0,     0,     0,    27,     0,    24,     5,    52,
      53,   170,   191,   174,   190,   172,   197,    47,    48,   195,
     175,   171,   179,    54,   112,   112,    24,     0,   176,    80,
      84,    81,   168,     0,     0,     0,     0,     0,     0,     0,
     154,   156,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    66,    68,     0,   193,    55,   178,     0,
       3,    18,     0,    60,    61,    62,    63,    64,    65,    58,
     145,   129,   130,   131,   127,   181,   100,    99,     0,    97,
      98,     0,    82,    94,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,   132,   134,   164,   189,   196,   194,
     110,   109,     0,     0,    83,   157,   158,   159,   160,   161,
     162,   163,   155,    78,    69,   200,   198,   199,    71,    70,
      76,    74,    79,    75,    73,    77,    72,    67,     0,     0,
     177,   186,   192,     2,    30,     0,    32,    33,     0,     0,
       0,     0,    37,    38,    39,    40,     0,     0,    41,    17,
      29,     0,     0,     0,     0,    95,   107,   103,   105,   101,
     102,   104,   106,    96,    93,   139,   140,   141,   142,   138,
     137,   135,   136,   133,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   113,   112,   184,
     188,   187,   185,    31,    34,    36,    35,    43,    42,    44,
      28,     0,     0,     0,     0,     0,   210,     0,   206,   111,
     183,   207,   208,     0,   205,   203,   209,     0,   213,     0,
       0,     0,     0,     0,   215,     0,     0,   211,   214,   212,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   216,     0,   218,   219,   220,   221,
     222,   223,   217,     0,     0,     0
};

static const short yydefgoto[] =
{
     353,    44,    45,    46,    47,   116,   107,   249,   250,    48,
      49,    74,    75,    76,    50,   153,   154,    51,   129,   102,
     182,   183,   263,    52,   200,   287,    89,    90,    53,   194,
     195,    54,    84,    85,    55,   140,   141,    56,    92,   128,
     230,   231,   113,   114,   157,   120,   117,   218,    65,    57,
      58,   253,   254,   305,   306,   320,   307,   323,   324,   337,
     338
};

static const short yypact[] =
{
     116,  -124,   -26,   -21,-32768,   -45,     2,    -4,    -1,   -34,
      14,    -3,   -73,    14,-32768,    45,   -33,   -70,   -66,   -65,
     -61,   -60,   -59,-32768,   -33,   -33,-32768,   -56,   -55,-32768,
     -54,    20,    -8,-32768,   -50,    45,   -46,     2,     8,   149,
     -43,    26,    26,-32768,   264,   -64,-32768,   -33,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -71,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -42,   -36,
     -32,   -11,    36,    -7,-32768,   -34,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,    14,-32768,    39,    67,    68,    -3,
  -32768,   105,-32768,    14,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,   -22,    13,    15,-32768,   150,-32768,   399,-32768,
  -32768,-32768,-32768,   -60,-32768,-32768,-32768,   -33,   -33,-32768,
      24,-32768,-32768,-32768,-32768,-32768,    66,   -16,-32768,    45,
  -32768,-32768,-32768,     2,     2,     2,     2,     2,     2,     2,
       8,-32768,    98,   101,   -28,   117,     2,     2,   122,     2,
       2,   125,   126,   149,-32768,   -47,-32768,   129,   129,    38,
  -32768,   466,  -110,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    32,-32768,
  -32768,     5,   -22,-32768,-32768,-32768,   -28,   -28,   -28,   -28,
      60,   139,     2,     2,   150,-32768,-32768,-32768,-32768,-32768,
     268,   268,   -43,    69,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -43,   140,
     -47,-32768,-32768,-32768,-32768,     2,-32768,-32768,   141,   143,
     147,   148,-32768,-32768,-32768,-32768,   151,   152,-32768,   466,
  -32768,    46,    47,   -89,    61,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   182,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,     2,     2,    52,    64,   -95,-32768,    53,-32768,   268,
  -32768,-32768,-32768,   -33,-32768,-32768,-32768,    62,-32768,    56,
      63,     2,    59,  -116,-32768,    72,     2,-32768,-32768,-32768,
      70,    10,    73,    76,    79,    81,    83,  -108,    78,     2,
       2,     2,     2,     2,-32768,    95,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   241,   242,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,   -20,-32768,-32768,   -14,   -31,-32768,    -6,-32768,
  -32768,-32768,-32768,   171,-32768,-32768,    94,-32768,-32768,   -15,
  -32768,    74,-32768,-32768,  -122,-32768,-32768,   162,-32768,-32768,
      65,-32768,   244,   -58,-32768,-32768,   114,-32768,-32768,-32768,
  -32768,    25,-32768,   145,   218,-32768,   243,   -62,   -37,-32768,
  -32768,-32768,    16,-32768,   -39,-32768,-32768,-32768,   -51,-32768,
     -63
};


#define	YYLAST		607


static const short yytable[] =
{
     132,   126,   106,   201,   176,    86,   215,   228,   256,   155,
     177,    68,   133,   178,   103,   104,    69,   124,   125,    77,
     130,    59,   303,    78,   159,   216,   170,    60,   303,   257,
      79,    63,    61,   161,   322,   170,   134,   251,   252,   103,
     104,   179,   327,   332,   333,   334,   335,   336,    62,    66,
     344,   135,    67,    91,   136,    64,   108,   229,   251,   252,
     109,   110,   180,   315,    94,   111,   112,   115,    95,    80,
     119,   121,   122,   123,    87,    70,   127,    71,    88,   156,
     131,   160,    72,   105,   163,    73,   162,   258,   196,   167,
     164,   259,   171,   105,   165,    81,   205,   206,   207,   208,
     209,   210,   211,   198,   198,   137,   260,   217,    82,   220,
     221,    96,   223,   224,   204,   166,   181,     1,   105,   168,
     172,   173,     2,     3,   265,   266,   267,   268,     4,     5,
       6,   138,   139,     7,   175,    83,   202,     8,     9,   184,
     203,   185,    10,    11,    97,   261,    12,   262,    13,    14,
     199,   213,    15,    98,   214,   271,   272,    99,   255,   142,
      16,   332,   333,   334,   335,   336,   309,   143,    17,   144,
     219,   288,   100,    18,    19,   222,   101,    20,   225,   226,
      21,    22,   232,   233,    23,    24,   269,   186,   187,   188,
     189,   145,   270,   291,   294,   289,   295,   290,   293,    25,
     296,   297,   301,   302,   298,   299,   308,   310,   313,   314,
     317,   319,   321,   322,    26,   326,    27,   329,    28,    29,
     146,   147,   148,   346,   149,   150,    30,   331,   151,   339,
      31,    32,   340,    33,    34,   341,    35,   342,    36,   343,
     352,   354,   355,   300,    37,   152,   169,   227,    38,    39,
      40,   174,    41,    42,   212,   292,   264,    93,   197,   273,
     158,    -5,    43,   190,   311,   312,   316,     0,   118,   304,
       2,     3,   328,     0,   345,   191,     4,     5,     6,   192,
     193,     7,     0,     0,   325,     8,     9,     0,     0,   330,
      10,    11,     0,     0,    12,     0,    13,    14,     0,   318,
      15,     0,   347,   348,   349,   350,   351,     0,    16,   274,
       0,     0,     0,     0,     0,     0,    17,     0,     0,   275,
       0,    18,    19,     0,     0,    20,     0,   276,    21,    22,
     277,     0,    23,    24,     0,   278,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    25,     0,     0,
       0,     0,     0,   279,     0,   280,   281,     0,   282,   283,
     284,     0,    26,   285,    27,     0,    28,    29,     0,     0,
       0,     0,     0,     0,    30,     0,     0,     0,    31,    32,
       0,    33,    34,     0,    35,     0,    36,     0,     0,     0,
       0,     0,    37,     0,     0,     0,    38,    39,    40,     0,
      41,    42,     0,     0,     0,     2,     3,   286,     0,    -5,
      43,     4,     5,     6,     0,     0,     7,     0,     0,     0,
       8,     9,     0,     0,     0,    10,    11,     0,     0,    12,
       0,    13,    14,     0,     0,    15,     0,     0,     0,     0,
       0,     0,     0,    16,     0,     0,     0,     0,     0,     0,
       0,    17,     0,     0,     0,     0,    18,    19,     0,     0,
      20,     0,     0,    21,    22,     0,     0,    23,    24,     0,
       0,     0,   234,     0,     0,     0,     0,   235,     0,     0,
       0,   236,    25,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,    27,
       0,    28,    29,     0,     0,     0,     0,     0,     0,    30,
       0,     0,     0,    31,    32,   237,    33,    34,     0,    35,
       0,    36,   238,     0,     0,     0,     0,    37,     0,     0,
       0,    38,    39,    40,     0,    41,    42,     0,     0,     0,
     239,     0,     0,     0,   240,    43,   241,     0,     0,     0,
       0,     0,     0,     0,     0,   242,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   243,   244,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   245,     0,   246,     0,   247,     0,   248
};

static const short yycheck[] =
{
      37,    32,    16,   125,    26,     8,    34,    54,     3,    40,
      32,    45,     4,    35,    47,    48,    50,    25,    32,     5,
      35,   145,   117,     9,    44,    53,    84,    53,   117,    24,
      16,    29,    53,    47,   150,    93,    28,   147,   148,    47,
      48,    63,   158,   151,   152,   153,   154,   155,    93,    53,
     158,    43,    53,   126,    46,    53,   126,   104,   147,   148,
     126,   126,    84,   158,    19,   126,   126,   126,    23,    55,
     126,   126,   126,    53,    77,   109,   126,   111,    81,    53,
     126,   145,   116,   126,   126,   119,   157,    82,   108,    53,
     126,    86,    53,   126,   126,    81,   133,   134,   135,   136,
     137,   138,   139,   117,   118,    97,   101,   135,    94,   146,
     147,    66,   149,   150,   129,   126,   138,     1,   126,   126,
      53,    53,     6,     7,   186,   187,   188,   189,    12,    13,
      14,   123,   124,    17,    29,   121,    70,    21,    22,   126,
     156,   126,    26,    27,    99,   140,    30,   142,    32,    33,
     126,    53,    36,   108,    53,   192,   193,   112,   126,    10,
      44,   151,   152,   153,   154,   155,   288,    18,    52,    20,
      53,   202,   127,    57,    58,    53,   131,    61,    53,    53,
      64,    65,    53,   145,    68,    69,   126,    37,    38,    39,
      40,    42,    53,    53,    53,   126,    53,   228,   235,    83,
      53,    53,   156,   156,    53,    53,   145,    25,   156,   145,
     157,   149,   156,   150,    98,   156,   100,   145,   102,   103,
      71,    72,    73,   145,    75,    76,   110,   157,    79,   156,
     114,   115,   156,   117,   118,   156,   120,   156,   122,   156,
     145,     0,     0,   249,   128,    96,    75,   153,   132,   133,
     134,    89,   136,   137,   140,   230,   182,    13,   113,   194,
      42,   145,   146,   113,   301,   302,   305,    -1,    25,   253,
       6,     7,   323,    -1,   337,   125,    12,    13,    14,   129,
     130,    17,    -1,    -1,   321,    21,    22,    -1,    -1,   326,
      26,    27,    -1,    -1,    30,    -1,    32,    33,    -1,   313,
      36,    -1,   339,   340,   341,   342,   343,    -1,    44,    41,
      -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    51,
      -1,    57,    58,    -1,    -1,    61,    -1,    59,    64,    65,
      62,    -1,    68,    69,    -1,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    -1,    -1,
      -1,    -1,    -1,    85,    -1,    87,    88,    -1,    90,    91,
      92,    -1,    98,    95,   100,    -1,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,   110,    -1,    -1,    -1,   114,   115,
      -1,   117,   118,    -1,   120,    -1,   122,    -1,    -1,    -1,
      -1,    -1,   128,    -1,    -1,    -1,   132,   133,   134,    -1,
     136,   137,    -1,    -1,    -1,     6,     7,   139,    -1,   145,
     146,    12,    13,    14,    -1,    -1,    17,    -1,    -1,    -1,
      21,    22,    -1,    -1,    -1,    26,    27,    -1,    -1,    30,
      -1,    32,    33,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    -1,    -1,    57,    58,    -1,    -1,
      61,    -1,    -1,    64,    65,    -1,    -1,    68,    69,    -1,
      -1,    -1,     6,    -1,    -1,    -1,    -1,    11,    -1,    -1,
      -1,    15,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,   100,
      -1,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,   110,
      -1,    -1,    -1,   114,   115,    49,   117,   118,    -1,   120,
      -1,   122,    56,    -1,    -1,    -1,    -1,   128,    -1,    -1,
      -1,   132,   133,   134,    -1,   136,   137,    -1,    -1,    -1,
      74,    -1,    -1,    -1,    78,   146,    80,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   106,   107,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   135,    -1,   137,    -1,   139,    -1,   141
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
#line 292 "ntp_parser.y"
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
#line 322 "ntp_parser.y"
{
                        struct peer_node *my_node =  create_peer_node(yyvsp[-2].Integer, yyvsp[-1].Address_node, yyvsp[0].Queue);
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;
case 18:
#line 328 "ntp_parser.y"
{
                        struct peer_node *my_node = create_peer_node(yyvsp[-1].Integer, yyvsp[0].Address_node, NULL);
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;
case 19:
#line 336 "ntp_parser.y"
{ yyval.Integer = T_Server; }
    break;
case 20:
#line 337 "ntp_parser.y"
{ yyval.Integer = T_Pool; }
    break;
case 21:
#line 338 "ntp_parser.y"
{ yyval.Integer = T_Peer; }
    break;
case 22:
#line 339 "ntp_parser.y"
{ yyval.Integer = T_Broadcast; }
    break;
case 23:
#line 340 "ntp_parser.y"
{ yyval.Integer = T_Manycastclient; }
    break;
case 24:
#line 344 "ntp_parser.y"
{ yyval.Address_node = yyvsp[0].Address_node; }
    break;
case 25:
#line 345 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET); }
    break;
case 26:
#line 346 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET6); }
    break;
case 27:
#line 350 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, 0); }
    break;
case 28:
#line 354 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 29:
#line 355 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 30:
#line 360 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_SKEY); }
    break;
case 31:
#line 361 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Bias, yyvsp[0].Double); }
    break;
case 32:
#line 362 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_BURST); }
    break;
case 33:
#line 363 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_IBURST); }
    break;
case 34:
#line 364 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Key, yyvsp[0].Integer); }
    break;
case 35:
#line 365 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Minpoll, yyvsp[0].Integer); }
    break;
case 36:
#line 366 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Maxpoll, yyvsp[0].Integer); }
    break;
case 37:
#line 367 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_NOSELECT);}
    break;
case 38:
#line 368 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREEMPT); }
    break;
case 39:
#line 369 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREFER); }
    break;
case 40:
#line 370 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_TRUE); }
    break;
case 41:
#line 371 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_XLEAVE); }
    break;
case 42:
#line 372 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Ttl, yyvsp[0].Integer); }
    break;
case 43:
#line 373 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Mode, yyvsp[0].Integer); }
    break;
case 44:
#line 374 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Version, yyvsp[0].Integer); }
    break;
case 45:
#line 385 "ntp_parser.y"
{ my_config.broadcastclient = SIMPLE; }
    break;
case 46:
#line 387 "ntp_parser.y"
{ my_config.broadcastclient = NOVOLLEY;  }
    break;
case 47:
#line 389 "ntp_parser.y"
{ append_queue(my_config.manycastserver, yyvsp[0].Queue);  }
    break;
case 48:
#line 391 "ntp_parser.y"
{ append_queue(my_config.multicastclient, yyvsp[0].Queue);  }
    break;
case 49:
#line 402 "ntp_parser.y"
{ my_config.auth.autokey = yyvsp[0].Integer;  }
    break;
case 50:
#line 404 "ntp_parser.y"
{ my_config.auth.control_key = yyvsp[0].Integer;  }
    break;
case 51:
#line 406 "ntp_parser.y"
{ if (my_config.auth.crypto_cmd_list != NULL)
					append_queue(my_config.auth.crypto_cmd_list, yyvsp[0].Queue);
				else
					my_config.auth.crypto_cmd_list = yyvsp[0].Queue;
			cryptosw++;
			}
    break;
case 52:
#line 413 "ntp_parser.y"
{ my_config.auth.keys = yyvsp[0].String;  }
    break;
case 53:
#line 415 "ntp_parser.y"
{ my_config.auth.keysdir = yyvsp[0].String;  }
    break;
case 54:
#line 417 "ntp_parser.y"
{ my_config.auth.requested_key = yyvsp[0].Integer;  }
    break;
case 55:
#line 419 "ntp_parser.y"
{ my_config.auth.trusted_key_list = yyvsp[0].Queue;  }
    break;
case 57:
#line 425 "ntp_parser.y"
{ yyval.Queue = NULL; }
    break;
case 58:
#line 429 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 59:
#line 430 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 60:
#line 435 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PRIV, yyvsp[0].String); }
    break;
case 61:
#line 437 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_IDENT, yyvsp[0].String); }
    break;
case 62:
#line 439 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PW, yyvsp[0].String); }
    break;
case 63:
#line 441 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_RAND, yyvsp[0].String); }
    break;
case 64:
#line 443 "ntp_parser.y"
{ my_config.auth.revoke = yyvsp[0].Integer;  }
    break;
case 65:
#line 445 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_SIGN, yyvsp[0].String); }
    break;
case 66:
#line 455 "ntp_parser.y"
{ append_queue(my_config.orphan_cmds,yyvsp[0].Queue);  }
    break;
case 67:
#line 459 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 68:
#line 460 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 69:
#line 465 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_CEILING, (double)yyvsp[0].Integer); }
    break;
case 70:
#line 467 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_FLOOR, (double)yyvsp[0].Integer); }
    break;
case 71:
#line 469 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_COHORT, (double)yyvsp[0].Integer); }
    break;
case 72:
#line 471 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_ORPHAN, (double)yyvsp[0].Integer); }
    break;
case 73:
#line 473 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINDISP, yyvsp[0].Double); }
    break;
case 74:
#line 475 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXDIST, yyvsp[0].Double); }
    break;
case 75:
#line 477 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINCLOCK, yyvsp[0].Double); }
    break;
case 76:
#line 479 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXCLOCK, yyvsp[0].Double); }
    break;
case 77:
#line 481 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINSANE, (double)yyvsp[0].Integer); }
    break;
case 78:
#line 483 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_BEACON, (double)yyvsp[0].Integer); }
    break;
case 79:
#line 485 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXHOP, (double)yyvsp[0].Integer); }
    break;
case 80:
#line 495 "ntp_parser.y"
{ append_queue(my_config.stats_list, yyvsp[0].Queue);  }
    break;
case 81:
#line 497 "ntp_parser.y"
{ my_config.stats_dir = yyvsp[0].String;  }
    break;
case 82:
#line 499 "ntp_parser.y"
{
                        enqueue(my_config.filegen_opts,
                                create_filegen_node(yyvsp[-1].VoidPtr, yyvsp[0].Queue));
                    }
    break;
case 83:
#line 506 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); }
    break;
case 84:
#line 507 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].VoidPtr); }
    break;
case 85:
#line 512 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("clockstats"); }
    break;
case 86:
#line 514 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("cryptostats"); }
    break;
case 87:
#line 516 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("loopstats"); }
    break;
case 88:
#line 518 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("peerstats"); }
    break;
case 89:
#line 520 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("rawstats"); }
    break;
case 90:
#line 522 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("sysstats"); }
    break;
case 91:
#line 524 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("timingstats"); }
    break;
case 92:
#line 526 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("protostats"); }
    break;
case 93:
#line 530 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 94:
#line 531 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 95:
#line 535 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(T_File, yyvsp[0].String); }
    break;
case 96:
#line 536 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Type, yyvsp[0].Integer); }
    break;
case 97:
#line 537 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Link); }
    break;
case 98:
#line 538 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Nolink); }
    break;
case 99:
#line 539 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Enable); }
    break;
case 100:
#line 540 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Disable); }
    break;
case 101:
#line 544 "ntp_parser.y"
{ yyval.Integer = FILEGEN_NONE; }
    break;
case 102:
#line 545 "ntp_parser.y"
{ yyval.Integer = FILEGEN_PID; }
    break;
case 103:
#line 546 "ntp_parser.y"
{ yyval.Integer = FILEGEN_DAY; }
    break;
case 104:
#line 547 "ntp_parser.y"
{ yyval.Integer = FILEGEN_WEEK; }
    break;
case 105:
#line 548 "ntp_parser.y"
{ yyval.Integer = FILEGEN_MONTH; }
    break;
case 106:
#line 549 "ntp_parser.y"
{ yyval.Integer = FILEGEN_YEAR; }
    break;
case 107:
#line 550 "ntp_parser.y"
{ yyval.Integer = FILEGEN_AGE; }
    break;
case 108:
#line 560 "ntp_parser.y"
{   append_queue(my_config.discard_opts, yyvsp[0].Queue); }
    break;
case 109:
#line 562 "ntp_parser.y"
{
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(yyvsp[-1].Address_node, NULL, yyvsp[0].Queue, ip_file->line_no));
                    }
    break;
case 110:
#line 567 "ntp_parser.y"
{
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(NULL, NULL, yyvsp[0].Queue, ip_file->line_no));
                    }
    break;
case 111:
#line 572 "ntp_parser.y"
{
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(yyvsp[-3].Address_node, yyvsp[-1].Address_node, yyvsp[0].Queue, ip_file->line_no));
                    }
    break;
case 112:
#line 579 "ntp_parser.y"
{ yyval.Queue = create_queue(); }
    break;
case 113:
#line 580 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); }
    break;
case 114:
#line 584 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_TIMEOUT); }
    break;
case 115:
#line 585 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_IGNORE); }
    break;
case 116:
#line 586 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_KOD); }
    break;
case 117:
#line 587 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_LIMITED); }
    break;
case 118:
#line 588 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_LPTRAP); }
    break;
case 119:
#line 589 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOMODIFY); }
    break;
case 120:
#line 590 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOPEER); }
    break;
case 121:
#line 591 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOQUERY); }
    break;
case 122:
#line 592 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_DONTSERVE); }
    break;
case 123:
#line 593 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOTRAP); }
    break;
case 124:
#line 594 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_DONTTRUST); }
    break;
case 125:
#line 595 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RESM_NTPONLY); }
    break;
case 126:
#line 596 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_VERSION); }
    break;
case 127:
#line 600 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 128:
#line 601 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 129:
#line 605 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Average, yyvsp[0].Integer); }
    break;
case 130:
#line 606 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Minimum, yyvsp[0].Integer); }
    break;
case 131:
#line 607 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Monitor, yyvsp[0].Integer); }
    break;
case 132:
#line 616 "ntp_parser.y"
{ enqueue(my_config.fudge, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  }
    break;
case 133:
#line 620 "ntp_parser.y"
{ enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 134:
#line 621 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 135:
#line 626 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(CLK_HAVETIME1, yyvsp[0].Double); }
    break;
case 136:
#line 628 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(CLK_HAVETIME2, yyvsp[0].Double); }
    break;
case 137:
#line 630 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEVAL1,  yyvsp[0].Integer); }
    break;
case 138:
#line 632 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CLK_HAVEVAL2,  yyvsp[0].String); }
    break;
case 139:
#line 634 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG1, yyvsp[0].Integer); }
    break;
case 140:
#line 636 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG2, yyvsp[0].Integer); }
    break;
case 141:
#line 638 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG3, yyvsp[0].Integer); }
    break;
case 142:
#line 640 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG4, yyvsp[0].Integer); }
    break;
case 143:
#line 649 "ntp_parser.y"
{ append_queue(my_config.enable_opts,yyvsp[0].Queue);  }
    break;
case 144:
#line 651 "ntp_parser.y"
{ append_queue(my_config.disable_opts,yyvsp[0].Queue);  }
    break;
case 145:
#line 655 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 146:
#line 656 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 147:
#line 660 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); }
    break;
case 148:
#line 661 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_BROADCLIENT); }
    break;
case 149:
#line 662 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_CAL); }
    break;
case 150:
#line 663 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_KERNEL); }
    break;
case 151:
#line 664 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_MONITOR); }
    break;
case 152:
#line 665 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_NTP); }
    break;
case 153:
#line 667 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_FILEGEN); }
    break;
case 154:
#line 675 "ntp_parser.y"
{ append_queue(my_config.tinker, yyvsp[0].Queue);  }
    break;
case 155:
#line 679 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 156:
#line 680 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 157:
#line 684 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_ALLAN, yyvsp[0].Double); }
    break;
case 158:
#line 685 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_PHI, yyvsp[0].Double); }
    break;
case 159:
#line 686 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_FREQ, yyvsp[0].Double); }
    break;
case 160:
#line 687 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_HUFFPUFF, yyvsp[0].Double); }
    break;
case 161:
#line 688 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_PANIC, yyvsp[0].Double); }
    break;
case 162:
#line 689 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_MAX, yyvsp[0].Double); }
    break;
case 163:
#line 690 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_MINSTEP, yyvsp[0].Double); }
    break;
case 164:
#line 699 "ntp_parser.y"
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
#line 715 "ntp_parser.y"
{
                    while (curr_include_level != -1)
                        FCLOSE(fp[curr_include_level--]);
                }
    break;
case 166:
#line 721 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, yyvsp[0].Double));  }
    break;
case 167:
#line 723 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_ival(T_Calldelay, yyvsp[0].Integer));  }
    break;
case 168:
#line 725 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_dval(T_Tick, yyvsp[0].Double));  }
    break;
case 169:
#line 727 "ntp_parser.y"
{ /* Null action, possibly all null parms */ }
    break;
case 170:
#line 729 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Leapfile, yyvsp[0].String)); }
    break;
case 171:
#line 732 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Pidfile, yyvsp[0].String));  }
    break;
case 172:
#line 734 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Logfile, yyvsp[0].String));  }
    break;
case 173:
#line 736 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_ival(T_Automax, yyvsp[0].Integer));  }
    break;
case 174:
#line 739 "ntp_parser.y"
{ append_queue(my_config.logconfig, yyvsp[0].Queue);  }
    break;
case 175:
#line 741 "ntp_parser.y"
{ append_queue(my_config.phone, yyvsp[0].Queue);  }
    break;
case 176:
#line 743 "ntp_parser.y"
{ enqueue(my_config.setvar, yyvsp[0].Set_var);  }
    break;
case 177:
#line 745 "ntp_parser.y"
{ enqueue(my_config.trap, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  }
    break;
case 178:
#line 747 "ntp_parser.y"
{ append_queue(my_config.ttl, yyvsp[0].Queue); }
    break;
case 179:
#line 749 "ntp_parser.y"
{ enqueue(my_config.qos, create_attr_sval(T_Qos, yyvsp[0].String)); }
    break;
case 180:
#line 753 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Driftfile, yyvsp[0].String)); }
    break;
case 181:
#line 755 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_dval(T_WanderThreshold, yyvsp[0].Double));
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, yyvsp[-1].String)); }
    break;
case 182:
#line 757 "ntp_parser.y"
{ /* Null driftfile,  indicated by null string "\0" */
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;
case 183:
#line 763 "ntp_parser.y"
{ yyval.Set_var = create_setvar_node(yyvsp[-3].String, yyvsp[-1].String, DEF); }
    break;
case 184:
#line 765 "ntp_parser.y"
{ yyval.Set_var = create_setvar_node(yyvsp[-2].String, yyvsp[0].String, 0); }
    break;
case 185:
#line 770 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 186:
#line 771 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 187:
#line 775 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Port, yyvsp[0].Integer); }
    break;
case 188:
#line 776 "ntp_parser.y"
{ yyval.Attr_val = create_attr_pval(T_Interface, yyvsp[0].Address_node); }
    break;
case 189:
#line 781 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 190:
#line 782 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 191:
#line 787 "ntp_parser.y"
{
                    char prefix = yyvsp[0].String[0];
                    char *type = &(yyvsp[0].String[1]);
                    if (prefix != '+' && prefix != '-' && prefix != '=') {
                        yyerror("Logconfig prefix is not '+', '-' or '='\n");
                    }
                    else
                        yyval.Attr_val = create_attr_sval(prefix, type);
                }
    break;
case 192:
#line 803 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, create_ival(yyvsp[0].Integer)); }
    break;
case 193:
#line 804 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(create_ival(yyvsp[0].Integer)); }
    break;
case 194:
#line 808 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, create_pval(yyvsp[0].String)); }
    break;
case 195:
#line 809 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(create_pval(yyvsp[0].String)); }
    break;
case 196:
#line 813 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Address_node); }
    break;
case 197:
#line 814 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Address_node); }
    break;
case 198:
#line 819 "ntp_parser.y"
{
                    if (yyvsp[0].Integer != 0 && yyvsp[0].Integer != 1) {
                        yyerror("Integer value is not boolean (0 or 1). Assuming 1");
                        yyval.Integer = 1;
                    }
                    else
                        yyval.Integer = yyvsp[0].Integer;
                }
    break;
case 199:
#line 827 "ntp_parser.y"
{ yyval.Integer = 1; }
    break;
case 200:
#line 828 "ntp_parser.y"
{ yyval.Integer = 0; }
    break;
case 201:
#line 832 "ntp_parser.y"
{ yyval.Double = (double)yyvsp[0].Integer; }
    break;
case 202:
#line 833 "ntp_parser.y"
{ yyval.Double = yyvsp[0].Double; }
    break;
case 203:
#line 842 "ntp_parser.y"
{
                 my_config.sim_details = create_sim_node(yyvsp[-2].Queue, yyvsp[-1].Queue);

                 /* Reset the old_config_style variable */
                 old_config_style = 1;
             }
    break;
case 204:
#line 856 "ntp_parser.y"
{ old_config_style = 0; }
    break;
case 205:
#line 860 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); }
    break;
case 206:
#line 861 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); }
    break;
case 207:
#line 865 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Beep_Delay, yyvsp[0].Double); }
    break;
case 208:
#line 866 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Sim_Duration, yyvsp[0].Double); }
    break;
case 209:
#line 870 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_server); }
    break;
case 210:
#line 871 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_server); }
    break;
case 211:
#line 876 "ntp_parser.y"
{ yyval.Sim_server = create_sim_server(yyvsp[-4].Address_node, yyvsp[-2].Double, yyvsp[-1].Queue); }
    break;
case 212:
#line 880 "ntp_parser.y"
{ yyval.Double = yyvsp[-1].Double; }
    break;
case 213:
#line 884 "ntp_parser.y"
{ yyval.Address_node = yyvsp[0].Address_node; }
    break;
case 214:
#line 888 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_script); }
    break;
case 215:
#line 889 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_script); }
    break;
case 216:
#line 894 "ntp_parser.y"
{ yyval.Sim_script = create_sim_script_info(yyvsp[-3].Double, yyvsp[-1].Queue); }
    break;
case 217:
#line 898 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); }
    break;
case 218:
#line 899 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); }
    break;
case 219:
#line 904 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Freq_Offset, yyvsp[0].Double); }
    break;
case 220:
#line 906 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Wander, yyvsp[0].Double); }
    break;
case 221:
#line 908 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Jitter, yyvsp[0].Double); }
    break;
case 222:
#line 910 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Prop_Delay, yyvsp[0].Double); }
    break;
case 223:
#line 912 "ntp_parser.y"
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
#line 916 "ntp_parser.y"


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

