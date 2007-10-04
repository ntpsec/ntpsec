/* A Bison parser, made from /deacon/backroom/ntp-dev/A.deacon/ntpd/../../ntpd/ntp_parser.y
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
# define	T_Clockstats	272
# define	T_Cohort	273
# define	T_ControlKey	274
# define	T_Crypto	275
# define	T_Cryptostats	276
# define	T_Day	277
# define	T_Default	278
# define	T_Disable	279
# define	T_Discard	280
# define	T_Dispersion	281
# define	T_Double	282
# define	T_Driftfile	283
# define	T_WanderThreshold	284
# define	T_Enable	285
# define	T_End	286
# define	T_False	287
# define	T_File	288
# define	T_Filegen	289
# define	T_Flag1	290
# define	T_Flag2	291
# define	T_Flag3	292
# define	T_Flag4	293
# define	T_Floor	294
# define	T_Freq	295
# define	T_Fudge	296
# define	T_Host	297
# define	T_Huffpuff	298
# define	T_IPv4_address	299
# define	T_IPv6_address	300
# define	T_Iburst	301
# define	T_Ident	302
# define	T_Ignore	303
# define	T_Includefile	304
# define	T_Integer	305
# define	T_Interface	306
# define	T_Kernel	307
# define	T_Key	308
# define	T_Keys	309
# define	T_Keysdir	310
# define	T_Kod	311
# define	T_Leap	312
# define	T_Leapfile	313
# define	T_Limited	314
# define	T_Link	315
# define	T_Logconfig	316
# define	T_Logfile	317
# define	T_Loopstats	318
# define	T_Lowpriotrap	319
# define	T_Manycastclient	320
# define	T_Manycastserver	321
# define	T_Mask	322
# define	T_Maxclock	323
# define	T_Maxdist	324
# define	T_Maxhop	325
# define	T_Maxpoll	326
# define	T_Minclock	327
# define	T_Mindist	328
# define	T_Minimum	329
# define	T_Minpoll	330
# define	T_Minsane	331
# define	T_Mode	332
# define	T_Monitor	333
# define	T_Month	334
# define	T_Multicastclient	335
# define	T_Nolink	336
# define	T_Nomodify	337
# define	T_None	338
# define	T_Nopeer	339
# define	T_Noquery	340
# define	T_Noselect	341
# define	T_Noserve	342
# define	T_Notrap	343
# define	T_Notrust	344
# define	T_Novolley	345
# define	T_Ntp	346
# define	T_Ntpport	347
# define	T_Orphan	348
# define	T_Panic	349
# define	T_Peer	350
# define	T_Peerstats	351
# define	T_Phone	352
# define	T_Pid	353
# define	T_Pidfile	354
# define	T_Pool	355
# define	T_Port	356
# define	T_Pps	357
# define	T_Preempt	358
# define	T_Prefer	359
# define	T_Pw	360
# define	T_RandFile	361
# define	T_Rawstats	362
# define	T_Refid	363
# define	T_Requestkey	364
# define	T_Restrict	365
# define	T_Revoke	366
# define	T_Server	367
# define	T_Setvar	368
# define	T_Sign	369
# define	T_Statistics	370
# define	T_Stats	371
# define	T_Statsdir	372
# define	T_Step	373
# define	T_Stepout	374
# define	T_Stratum	375
# define	T_String	376
# define	T_Sysstats	377
# define	T_Tick	378
# define	T_Time1	379
# define	T_Time2	380
# define	T_Tinker	381
# define	T_Tos	382
# define	T_Trap	383
# define	T_True	384
# define	T_Trustedkey	385
# define	T_Ttl	386
# define	T_Type	387
# define	T_Version	388
# define	T_Week	389
# define	T_Year	390
# define	T_Flag	391
# define	T_Void	392
# define	T_EOC	393
# define	T_Simulate	394
# define	T_Beep_Delay	395
# define	T_Sim_Duration	396
# define	T_Server_Offset	397
# define	T_Duration	398
# define	T_Freq_Offset	399
# define	T_Wander	400
# define	T_Jitter	401
# define	T_Prop_Delay	402
# define	T_Proc_Delay	403

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



#define	YYFINAL		354
#define	YYFLAG		-32768
#define	YYNTBASE	155

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 403 ? yytranslate[x] : 218)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const short yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   151,     2,   152,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   150,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   153,     2,   154,     2,     2,     2,     2,
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
     146,   147,   148,   149
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     6,     9,    12,    13,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    39,    42,
      44,    46,    48,    50,    52,    54,    56,    59,    61,    63,
      66,    68,    70,    72,    74,    77,    80,    83,    85,    87,
      89,    91,    94,    97,   100,   102,   105,   108,   111,   114,
     117,   120,   123,   126,   129,   132,   134,   135,   138,   140,
     143,   146,   149,   152,   155,   158,   161,   164,   166,   169,
     172,   175,   178,   181,   184,   187,   190,   193,   196,   199,
     202,   205,   209,   212,   214,   216,   218,   220,   222,   224,
     226,   229,   231,   234,   237,   239,   241,   243,   245,   247,
     249,   251,   253,   255,   257,   259,   262,   266,   270,   276,
     277,   280,   282,   284,   286,   288,   290,   292,   294,   296,
     298,   300,   302,   304,   307,   309,   312,   315,   318,   322,
     325,   327,   330,   333,   336,   339,   342,   345,   348,   351,
     354,   357,   360,   362,   364,   366,   368,   370,   372,   374,
     376,   379,   382,   384,   387,   390,   393,   396,   399,   402,
     405,   409,   411,   414,   417,   420,   423,   426,   429,   432,
     435,   438,   441,   444,   448,   451,   453,   456,   457,   462,
     466,   469,   471,   474,   477,   480,   482,   485,   487,   489,
     491,   493,   496,   498,   501,   503,   506,   508,   510,   512,
     514,   516,   518,   524,   526,   530,   533,   537,   541,   544,
     546,   552,   557,   561,   563,   565,   568,   570,   577,   581,
     584,   588,   592,   596,   600
};
static const short yyrhs[] =
{
     156,     0,   156,   157,   139,     0,   157,   139,     0,     1,
     139,     0,     0,   158,     0,   164,     0,   165,     0,   172,
       0,   178,     0,   169,     0,   183,     0,   186,     0,   189,
       0,   192,     0,   205,     0,   159,   160,   162,     0,   159,
     160,     0,   113,     0,   101,     0,    96,     0,    11,     0,
      66,     0,   161,     0,   122,     0,    51,   122,     0,    45,
       0,    46,     0,   162,   163,     0,   163,     0,     6,     0,
      14,     0,    47,     0,    54,    51,     0,    76,    51,     0,
      72,    51,     0,    87,     0,   104,     0,   105,     0,   130,
       0,   132,    51,     0,    78,    51,     0,   134,    51,     0,
      12,     0,    12,    91,     0,    67,   202,     0,    81,   202,
       0,     6,    51,     0,    20,    51,     0,    21,   166,     0,
      55,   122,     0,    56,   122,     0,   110,    51,     0,   131,
     200,     0,   167,     0,     0,   167,   168,     0,   168,     0,
      43,   122,     0,    48,   122,     0,   106,   122,     0,   107,
     122,     0,   112,    51,     0,   115,   122,     0,   128,   170,
       0,   170,   171,     0,   171,     0,    17,    51,     0,    40,
      51,     0,    19,   203,     0,    94,    51,     0,    74,   204,
       0,    70,   204,     0,    73,   204,     0,    69,   204,     0,
      77,    51,     0,    10,    51,     0,    71,    51,     0,   116,
     173,     0,   118,   122,     0,    35,   174,   175,     0,   173,
     174,     0,   174,     0,    18,     0,    22,     0,    64,     0,
      97,     0,   108,     0,   123,     0,   175,   176,     0,   176,
       0,    34,   122,     0,   133,   177,     0,    61,     0,    82,
       0,    31,     0,    25,     0,    84,     0,    99,     0,    23,
       0,   135,     0,    80,     0,   136,     0,     3,     0,    26,
     181,     0,   111,   160,   179,     0,   111,    24,   179,     0,
     111,   161,    68,   161,   179,     0,     0,   179,   180,     0,
      49,     0,    57,     0,    60,     0,    65,     0,    83,     0,
      85,     0,    86,     0,    88,     0,    89,     0,    90,     0,
      93,     0,   134,     0,   181,   182,     0,   182,     0,     8,
      51,     0,    75,    51,     0,    79,    51,     0,    42,   160,
     184,     0,   184,   185,     0,   185,     0,   125,   204,     0,
     126,   204,     0,   121,    51,     0,   109,   122,     0,    36,
     203,     0,    37,   203,     0,    38,   203,     0,    39,   203,
       0,    31,   187,     0,    25,   187,     0,   187,   188,     0,
     188,     0,     5,     0,     9,     0,    15,     0,    53,     0,
      79,     0,    92,     0,   117,     0,   127,   190,     0,   190,
     191,     0,   191,     0,     4,   204,     0,    27,   204,     0,
      41,   204,     0,    44,   204,     0,    95,   204,     0,   119,
     204,     0,   120,   204,     0,    50,   122,   157,     0,    32,
       0,    13,   204,     0,    16,    51,     0,   124,   204,     0,
      29,   193,     0,    59,   122,     0,   100,   122,     0,    63,
     122,     0,     7,    51,     0,    62,   197,     0,    98,   201,
       0,   114,   194,     0,   129,   161,   195,     0,   132,   200,
       0,   122,     0,   122,    28,     0,     0,   122,   150,   122,
      24,     0,   122,   150,   122,     0,   195,   196,     0,   196,
       0,   102,    51,     0,    52,   161,     0,   197,   198,     0,
     198,     0,   199,   122,     0,   122,     0,   151,     0,   152,
       0,   150,     0,   200,    51,     0,    51,     0,   201,   122,
       0,   122,     0,   202,   160,     0,   160,     0,    51,     0,
     130,     0,    33,     0,    51,     0,    28,     0,   206,   153,
     207,   209,   154,     0,   140,     0,   207,   208,   139,     0,
     208,   139,     0,   141,   150,   204,     0,   142,   150,   204,
       0,   209,   210,     0,   210,     0,   212,   153,   211,   214,
     154,     0,   143,   150,   204,   139,     0,   113,   150,   213,
       0,   161,     0,   122,     0,   214,   215,     0,   215,     0,
     144,   150,   204,   153,   216,   154,     0,   216,   217,   139,
       0,   217,   139,     0,   145,   150,   204,     0,   146,   150,
     204,     0,   147,   150,   204,     0,   148,   150,   204,     0,
     149,   150,   204,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   278,   282,   283,   288,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   318,   324,   333,
     334,   335,   336,   337,   341,   342,   343,   359,   360,   364,
     365,   370,   371,   372,   373,   374,   375,   376,   377,   378,
     379,   380,   381,   382,   392,   394,   396,   398,   409,   411,
     413,   420,   422,   424,   426,   431,   432,   437,   438,   442,
     444,   446,   448,   450,   452,   462,   467,   468,   472,   474,
     476,   478,   480,   482,   484,   486,   488,   490,   492,   502,
     504,   506,   514,   515,   519,   521,   523,   525,   527,   529,
     534,   535,   539,   540,   541,   542,   543,   544,   548,   549,
     550,   551,   552,   553,   554,   563,   565,   570,   575,   583,
     584,   588,   589,   590,   591,   592,   593,   594,   595,   596,
     597,   598,   599,   603,   604,   608,   609,   610,   618,   623,
     624,   628,   630,   632,   634,   636,   638,   640,   642,   651,
     653,   658,   659,   663,   664,   665,   666,   667,   668,   670,
     678,   682,   683,   687,   688,   689,   690,   691,   692,   693,
     701,   717,   723,   725,   727,   729,   731,   734,   736,   738,
     741,   743,   745,   747,   749,   753,   755,   758,   763,   765,
     771,   772,   776,   777,   782,   783,   787,   788,   805,   806,
     807,   816,   817,   821,   822,   826,   827,   831,   840,   841,
     845,   846,   854,   869,   873,   874,   878,   879,   883,   884,
     888,   893,   897,   901,   902,   906,   907,   911,   916,   917,
     921,   923,   925,   927,   929
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "T_Age", "T_Allan", "T_Auth", "T_Autokey", 
  "T_Automax", "T_Average", "T_Bclient", "T_Beacon", "T_Broadcast", 
  "T_Broadcastclient", "T_Broadcastdelay", "T_Burst", "T_Calibrate", 
  "T_Calldelay", "T_Ceiling", "T_Clockstats", "T_Cohort", "T_ControlKey", 
  "T_Crypto", "T_Cryptostats", "T_Day", "T_Default", "T_Disable", 
  "T_Discard", "T_Dispersion", "T_Double", "T_Driftfile", 
  "T_WanderThreshold", "T_Enable", "T_End", "T_False", "T_File", 
  "T_Filegen", "T_Flag1", "T_Flag2", "T_Flag3", "T_Flag4", "T_Floor", 
  "T_Freq", "T_Fudge", "T_Host", "T_Huffpuff", "T_IPv4_address", 
  "T_IPv6_address", "T_Iburst", "T_Ident", "T_Ignore", "T_Includefile", 
  "T_Integer", "T_Interface", "T_Kernel", "T_Key", "T_Keys", "T_Keysdir", 
  "T_Kod", "T_Leap", "T_Leapfile", "T_Limited", "T_Link", "T_Logconfig", 
  "T_Logfile", "T_Loopstats", "T_Lowpriotrap", "T_Manycastclient", 
  "T_Manycastserver", "T_Mask", "T_Maxclock", "T_Maxdist", "T_Maxhop", 
  "T_Maxpoll", "T_Minclock", "T_Mindist", "T_Minimum", "T_Minpoll", 
  "T_Minsane", "T_Mode", "T_Monitor", "T_Month", "T_Multicastclient", 
  "T_Nolink", "T_Nomodify", "T_None", "T_Nopeer", "T_Noquery", 
  "T_Noselect", "T_Noserve", "T_Notrap", "T_Notrust", "T_Novolley", 
  "T_Ntp", "T_Ntpport", "T_Orphan", "T_Panic", "T_Peer", "T_Peerstats", 
  "T_Phone", "T_Pid", "T_Pidfile", "T_Pool", "T_Port", "T_Pps", 
  "T_Preempt", "T_Prefer", "T_Pw", "T_RandFile", "T_Rawstats", "T_Refid", 
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
       0,   155,   156,   156,   156,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   158,   158,   159,
     159,   159,   159,   159,   160,   160,   160,   161,   161,   162,
     162,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   164,   164,   164,   164,   165,   165,
     165,   165,   165,   165,   165,   166,   166,   167,   167,   168,
     168,   168,   168,   168,   168,   169,   170,   170,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   172,
     172,   172,   173,   173,   174,   174,   174,   174,   174,   174,
     175,   175,   176,   176,   176,   176,   176,   176,   177,   177,
     177,   177,   177,   177,   177,   178,   178,   178,   178,   179,
     179,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   181,   181,   182,   182,   182,   183,   184,
     184,   185,   185,   185,   185,   185,   185,   185,   185,   186,
     186,   187,   187,   188,   188,   188,   188,   188,   188,   188,
     189,   190,   190,   191,   191,   191,   191,   191,   191,   191,
     192,   192,   192,   192,   192,   192,   192,   192,   192,   192,
     192,   192,   192,   192,   192,   193,   193,   193,   194,   194,
     195,   195,   196,   196,   197,   197,   198,   198,   199,   199,
     199,   200,   200,   201,   201,   202,   202,   203,   203,   203,
     204,   204,   205,   206,   207,   207,   208,   208,   209,   209,
     210,   211,   212,   213,   213,   214,   214,   215,   216,   216,
     217,   217,   217,   217,   217
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     3,     2,     2,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     2,
       1,     1,     1,     1,     2,     2,     2,     1,     1,     1,
       1,     2,     2,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     1,     0,     2,     1,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     3,     5,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     2,     2,     2,     3,     2,
       1,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     1,     2,     2,     2,     2,     2,     2,     2,
       3,     1,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     3,     2,     1,     2,     0,     4,     3,
       2,     1,     2,     2,     2,     1,     2,     1,     1,     1,
       1,     2,     1,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     5,     1,     3,     2,     3,     3,     2,     1,
       5,     4,     3,     1,     1,     2,     1,     6,     3,     2,
       3,     3,     3,     3,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     0,     0,     0,    22,    44,     0,     0,     0,    56,
       0,     0,   177,     0,   161,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,    21,     0,     0,    20,
       0,     0,    19,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   203,     1,     0,     6,     0,     7,     8,    11,
       9,    10,    12,    13,    14,    15,    16,     0,     4,    48,
     169,    45,   201,   200,   162,   163,    49,     0,     0,     0,
       0,     0,     0,    50,    55,    58,   143,   144,   145,   146,
     147,   148,   149,   140,   142,     0,     0,     0,   105,   124,
     175,   165,   139,    84,    85,    86,    87,    88,    89,     0,
      27,    28,     0,    25,     0,    24,     5,    51,    52,   166,
     187,   190,   188,   189,   170,   185,     0,   168,   196,    46,
      47,   194,   171,   167,    53,   109,   109,    24,     0,   172,
      79,    83,    80,   164,     0,     0,     0,     0,     0,     0,
       0,   150,   152,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    65,    67,     0,   192,    54,   174,
       0,     3,    18,     0,    59,    60,    61,    62,    63,    64,
      57,   141,   125,   126,   127,   123,   176,    97,    96,     0,
      94,    95,     0,    81,    91,    26,     0,     0,     0,     0,
       0,     0,     0,     0,   128,   130,   160,   184,   186,   195,
     193,   107,   106,     0,     0,    82,   153,   154,   155,   156,
     157,   158,   159,   151,    77,    68,   199,   197,   198,    70,
      69,    75,    73,    78,    74,    72,    76,    71,    66,     0,
       0,   173,   181,   191,     2,    31,    32,    33,     0,     0,
       0,     0,    37,    38,    39,    40,     0,     0,    17,    30,
       0,     0,     0,     0,    92,   104,   100,   102,    98,    99,
     101,   103,    93,    90,   135,   136,   137,   138,   134,   133,
     131,   132,   129,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   110,   109,   179,   183,   182,
     180,    34,    36,    35,    42,    41,    43,    29,     0,     0,
       0,     0,     0,   209,     0,   205,   108,   178,   206,   207,
       0,   204,   202,   208,     0,   214,   213,   212,     0,     0,
       0,     0,     0,   216,     0,     0,   210,   215,   211,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   217,     0,   219,   220,   221,   222,   223,
     224,   218,     0,     0,     0
};

static const short yydefgoto[] =
{
     352,    43,    44,    45,    46,   118,   105,   248,   249,    47,
      48,    73,    74,    75,    49,   154,   155,    50,   130,    99,
     183,   184,   262,    51,   201,   285,    88,    89,    52,   194,
     195,    53,    83,    84,    54,   141,   142,    55,    91,   129,
     231,   232,   114,   115,   116,   158,   122,   119,   219,    64,
      56,    57,   252,   253,   302,   303,   319,   304,   317,   322,
     323,   336,   337
};

static const short yypact[] =
{
       9,  -115,   -20,   -12,-32768,   -33,    -5,    16,    19,   134,
      -1,    71,   -41,    -1,-32768,   -15,   -34,   -28,   -16,    -4,
       4,    41,     7,-32768,   -34,   -34,-32768,    13,    17,-32768,
      26,     2,-32768,    21,   -15,    22,    -5,    39,   376,    28,
      38,    38,-32768,   154,   -18,-32768,   -34,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,   -61,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,    25,    30,    31,
      42,   104,    49,-32768,   134,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,    -1,-32768,   121,   125,   127,    71,-32768,
     156,-32768,    -1,-32768,-32768,-32768,-32768,-32768,-32768,   120,
  -32768,-32768,    65,-32768,   220,-32768,   301,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,    41,-32768,    66,-32768,-32768,   -34,
     -34,-32768,    72,-32768,-32768,-32768,-32768,   129,    45,-32768,
     -15,-32768,-32768,-32768,    -5,    -5,    -5,    -5,    -5,    -5,
      -5,    39,-32768,   149,   152,    12,   157,    -5,    -5,   160,
      -5,    -5,   161,   163,   376,-32768,   -47,-32768,   164,   164,
      68,-32768,   305,   -45,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    83,
  -32768,-32768,    33,   120,-32768,-32768,    12,    12,    12,    12,
      96,   168,    -5,    -5,   220,-32768,-32768,-32768,-32768,-32768,
  -32768,   141,   141,    28,   100,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    28,
     172,   -47,-32768,-32768,-32768,-32768,-32768,-32768,   174,   177,
     181,   182,-32768,-32768,-32768,-32768,   185,   186,   305,-32768,
      88,    89,   -80,   103,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,   219,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    -5,    -5,
      94,   106,  -100,-32768,    95,-32768,   141,-32768,-32768,-32768,
     -27,-32768,-32768,-32768,   108,-32768,-32768,-32768,    97,   116,
      -5,   111,   -94,-32768,   130,    -5,-32768,-32768,-32768,   113,
     150,   123,   124,   126,   137,   138,   224,   132,    -5,    -5,
      -5,    -5,    -5,-32768,   140,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,   277,   290,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,   -37,-32768,-32768,    11,   -30,-32768,    43,-32768,
  -32768,-32768,-32768,   218,-32768,-32768,   146,-32768,-32768,    -2,
  -32768,   118,-32768,-32768,  -124,-32768,-32768,   221,-32768,-32768,
     122,-32768,   297,   -55,-32768,-32768,   179,-32768,-32768,-32768,
  -32768,    84,-32768,   204,-32768,   282,-32768,   299,  -102,   -36,
  -32768,-32768,-32768,    73,-32768,    29,-32768,-32768,-32768,-32768,
       6,-32768,     1
};


#define	YYLAST		470


static const short yytable[] =
{
     133,   127,   202,    93,    76,   229,   160,    94,    77,   156,
       1,   100,   101,   300,    78,     2,     3,   102,   100,   101,
       4,     5,     6,    62,    58,     7,   125,   104,   171,     8,
       9,    59,   131,   300,    10,    11,   255,   171,    12,    60,
      13,    14,   126,   134,    15,   216,    63,   100,   101,    95,
     321,    16,    79,   102,   312,   230,   256,   162,    61,    17,
     326,   250,   251,   217,    18,    19,   135,    65,    20,   196,
      66,    21,    22,   100,   101,    23,    24,   124,    80,    85,
     136,    90,    96,   137,   264,   265,   266,   267,   103,   157,
      25,    81,   163,    97,   106,   315,   250,   251,   206,   207,
     208,   209,   210,   211,   212,    26,   107,    27,    98,    28,
      29,   221,   222,   257,   224,   225,    82,   258,   108,    30,
      31,   161,    32,    33,   103,    34,   109,    35,   205,   117,
     199,   199,   259,    36,   138,   121,    37,    38,    39,   123,
      40,    41,   218,   128,   132,   177,    86,   164,    -5,    42,
      87,   178,   165,   166,   179,   168,   270,   271,   139,   140,
       2,     3,   306,   110,   167,     4,     5,     6,   260,   261,
       7,   169,   172,   286,     8,     9,   173,    67,   174,    10,
      11,   180,    68,    12,   176,    13,    14,   185,   198,    15,
     273,   111,   112,   113,   200,   204,    16,   203,   274,   288,
     214,   275,   181,   215,    17,   254,   276,   234,   220,    18,
      19,   223,   226,    20,   227,   233,    21,    22,   268,   269,
      23,    24,   287,   289,   277,   291,   278,   279,   292,   280,
     281,   282,   293,   294,   283,    25,   295,   296,   298,   299,
      69,    70,   305,   307,   310,   311,    71,   320,   314,    72,
      26,   318,    27,   182,    28,    29,   186,   187,   188,   189,
     321,   325,   308,   309,    30,    31,   330,    32,    33,   328,
      34,   345,    35,   338,   339,   284,   340,   353,    36,   351,
     316,    37,    38,    39,   324,    40,    41,   341,   342,   329,
     354,   297,   170,    -5,    42,   331,   332,   333,   334,   335,
     228,   263,   346,   347,   348,   349,   350,     2,     3,   175,
      92,   235,     4,     5,     6,   290,   272,     7,   197,   236,
     213,     8,     9,   159,   120,   301,    10,    11,   327,   190,
      12,   313,    13,    14,     0,     0,    15,   344,     0,     0,
       0,   191,     0,    16,     0,   192,   193,     0,     0,     0,
       0,    17,   237,     0,     0,     0,    18,    19,     0,   238,
      20,     0,     0,    21,    22,     0,     0,    23,    24,   331,
     332,   333,   334,   335,     0,     0,     0,   239,   343,     0,
       0,   240,    25,   241,     0,     0,   143,     0,     0,     0,
       0,     0,   242,   144,     0,   145,     0,    26,     0,    27,
       0,    28,    29,     0,     0,     0,     0,     0,     0,   243,
     244,    30,    31,     0,    32,    33,   146,    34,     0,    35,
       0,     0,     0,     0,     0,    36,     0,     0,    37,    38,
      39,     0,    40,    41,     0,   245,     0,   246,     0,   247,
       0,    42,     0,     0,     0,   147,   148,   149,     0,   150,
     151,     0,     0,   152,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     153
};

static const short yycheck[] =
{
      36,    31,   126,    18,     5,    52,    43,    22,     9,    39,
       1,    45,    46,   113,    15,     6,     7,    51,    45,    46,
      11,    12,    13,    28,   139,    16,    24,    16,    83,    20,
      21,    51,    34,   113,    25,    26,     3,    92,    29,    51,
      31,    32,    31,     4,    35,    33,    51,    45,    46,    64,
     144,    42,    53,    51,   154,   102,    23,    46,    91,    50,
     154,   141,   142,    51,    55,    56,    27,    51,    59,   106,
      51,    62,    63,    45,    46,    66,    67,    51,    79,     8,
      41,   122,    97,    44,   186,   187,   188,   189,   122,    51,
      81,    92,   153,   108,   122,   122,   141,   142,   134,   135,
     136,   137,   138,   139,   140,    96,   122,    98,   123,   100,
     101,   147,   148,    80,   150,   151,   117,    84,   122,   110,
     111,   139,   113,   114,   122,   116,   122,   118,   130,   122,
     119,   120,    99,   124,    95,   122,   127,   128,   129,   122,
     131,   132,   130,   122,   122,    25,    75,   122,   139,   140,
      79,    31,   122,   122,    34,    51,   192,   193,   119,   120,
       6,     7,   286,   122,   122,    11,    12,    13,   135,   136,
      16,   122,    51,   203,    20,    21,    51,    43,    51,    25,
      26,    61,    48,    29,    28,    31,    32,   122,   122,    35,
      49,   150,   151,   152,   122,   150,    42,    68,    57,   229,
      51,    60,    82,    51,    50,   122,    65,   139,    51,    55,
      56,    51,    51,    59,    51,    51,    62,    63,   122,    51,
      66,    67,   122,    51,    83,    51,    85,    86,    51,    88,
      89,    90,    51,    51,    93,    81,    51,    51,   150,   150,
     106,   107,   139,    24,   150,   139,   112,   150,   153,   115,
      96,   143,    98,   133,   100,   101,    36,    37,    38,    39,
     144,   150,   298,   299,   110,   111,   153,   113,   114,   139,
     116,   139,   118,   150,   150,   134,   150,     0,   124,   139,
     310,   127,   128,   129,   320,   131,   132,   150,   150,   325,
       0,   248,    74,   139,   140,   145,   146,   147,   148,   149,
     154,   183,   338,   339,   340,   341,   342,     6,     7,    88,
      13,     6,    11,    12,    13,   231,   194,    16,   114,    14,
     141,    20,    21,    41,    25,   252,    25,    26,   322,   109,
      29,   302,    31,    32,    -1,    -1,    35,   336,    -1,    -1,
      -1,   121,    -1,    42,    -1,   125,   126,    -1,    -1,    -1,
      -1,    50,    47,    -1,    -1,    -1,    55,    56,    -1,    54,
      59,    -1,    -1,    62,    63,    -1,    -1,    66,    67,   145,
     146,   147,   148,   149,    -1,    -1,    -1,    72,   154,    -1,
      -1,    76,    81,    78,    -1,    -1,    10,    -1,    -1,    -1,
      -1,    -1,    87,    17,    -1,    19,    -1,    96,    -1,    98,
      -1,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,   104,
     105,   110,   111,    -1,   113,   114,    40,   116,    -1,   118,
      -1,    -1,    -1,    -1,    -1,   124,    -1,    -1,   127,   128,
     129,    -1,   131,   132,    -1,   130,    -1,   132,    -1,   134,
      -1,   140,    -1,    -1,    -1,    69,    70,    71,    -1,    73,
      74,    -1,    -1,    77,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94
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
#line 289 "ntp_parser.y"
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
#line 319 "ntp_parser.y"
{
                        struct peer_node *my_node =  create_peer_node(yyvsp[-2].Integer, yyvsp[-1].Address_node, yyvsp[0].Queue);
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;
case 18:
#line 325 "ntp_parser.y"
{
                        struct peer_node *my_node = create_peer_node(yyvsp[-1].Integer, yyvsp[0].Address_node, NULL);
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
    break;
case 19:
#line 333 "ntp_parser.y"
{ yyval.Integer = T_Server; }
    break;
case 20:
#line 334 "ntp_parser.y"
{ yyval.Integer = T_Pool; }
    break;
case 21:
#line 335 "ntp_parser.y"
{ yyval.Integer = T_Peer; }
    break;
case 22:
#line 336 "ntp_parser.y"
{ yyval.Integer = T_Broadcast; }
    break;
case 23:
#line 337 "ntp_parser.y"
{ yyval.Integer = T_Manycastclient; }
    break;
case 24:
#line 341 "ntp_parser.y"
{ yyval.Address_node = yyvsp[0].Address_node; }
    break;
case 25:
#line 342 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, default_ai_family); }
    break;
case 26:
#line 344 "ntp_parser.y"
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
#line 359 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET); }
    break;
case 28:
#line 360 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET6); }
    break;
case 29:
#line 364 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 30:
#line 365 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 31:
#line 370 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_SKEY | FLAG_AUTHENABLE); }
    break;
case 32:
#line 371 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_BURST); }
    break;
case 33:
#line 372 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_IBURST); }
    break;
case 34:
#line 373 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Key, yyvsp[0].Integer); }
    break;
case 35:
#line 374 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Minpoll, yyvsp[0].Integer); }
    break;
case 36:
#line 375 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Maxpoll, yyvsp[0].Integer); }
    break;
case 37:
#line 376 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_NOSELECT);}
    break;
case 38:
#line 377 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREEMPT); }
    break;
case 39:
#line 378 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREFER); }
    break;
case 40:
#line 379 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_TRUE); }
    break;
case 41:
#line 380 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Ttl, yyvsp[0].Integer); }
    break;
case 42:
#line 381 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Mode, yyvsp[0].Integer); }
    break;
case 43:
#line 382 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Version, yyvsp[0].Integer); }
    break;
case 44:
#line 393 "ntp_parser.y"
{ my_config.broadcastclient = SIMPLE; }
    break;
case 45:
#line 395 "ntp_parser.y"
{ my_config.broadcastclient = NOVOLLEY;  }
    break;
case 46:
#line 397 "ntp_parser.y"
{ append_queue(my_config.manycastserver, yyvsp[0].Queue);  }
    break;
case 47:
#line 399 "ntp_parser.y"
{ append_queue(my_config.multicastclient, yyvsp[0].Queue);  }
    break;
case 48:
#line 410 "ntp_parser.y"
{ my_config.auth.autokey = yyvsp[0].Integer;  }
    break;
case 49:
#line 412 "ntp_parser.y"
{ my_config.auth.control_key = yyvsp[0].Integer;  }
    break;
case 50:
#line 414 "ntp_parser.y"
{ if (my_config.auth.crypto_cmd_list != NULL)
					append_queue(my_config.auth.crypto_cmd_list, yyvsp[0].Queue);
				else
					my_config.auth.crypto_cmd_list = yyvsp[0].Queue;
			cryptosw++;
			}
    break;
case 51:
#line 421 "ntp_parser.y"
{ my_config.auth.keys = yyvsp[0].String;  }
    break;
case 52:
#line 423 "ntp_parser.y"
{ my_config.auth.keysdir = yyvsp[0].String;  }
    break;
case 53:
#line 425 "ntp_parser.y"
{ my_config.auth.requested_key = yyvsp[0].Integer;  }
    break;
case 54:
#line 427 "ntp_parser.y"
{ my_config.auth.trusted_key_list = yyvsp[0].Queue;  }
    break;
case 56:
#line 433 "ntp_parser.y"
{ yyval.Queue = NULL; }
    break;
case 57:
#line 437 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 58:
#line 438 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 59:
#line 443 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PRIV, yyvsp[0].String); }
    break;
case 60:
#line 445 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_IDENT, yyvsp[0].String); }
    break;
case 61:
#line 447 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PW, yyvsp[0].String); }
    break;
case 62:
#line 449 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_RAND, yyvsp[0].String); }
    break;
case 63:
#line 451 "ntp_parser.y"
{ my_config.auth.revoke = yyvsp[0].Integer;  }
    break;
case 64:
#line 453 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_SIGN, yyvsp[0].String); }
    break;
case 65:
#line 463 "ntp_parser.y"
{ append_queue(my_config.orphan_cmds,yyvsp[0].Queue);  }
    break;
case 66:
#line 467 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 67:
#line 468 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 68:
#line 473 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_CEILING, (double)yyvsp[0].Integer); }
    break;
case 69:
#line 475 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_FLOOR, (double)yyvsp[0].Integer); }
    break;
case 70:
#line 477 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_COHORT, (double)yyvsp[0].Integer); }
    break;
case 71:
#line 479 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_ORPHAN, (double)yyvsp[0].Integer); }
    break;
case 72:
#line 481 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINDISP, yyvsp[0].Double); }
    break;
case 73:
#line 483 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXDIST, yyvsp[0].Double); }
    break;
case 74:
#line 485 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINCLOCK, yyvsp[0].Double); }
    break;
case 75:
#line 487 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXCLOCK, yyvsp[0].Double); }
    break;
case 76:
#line 489 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINSANE, (double)yyvsp[0].Integer); }
    break;
case 77:
#line 491 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_BEACON, (double)yyvsp[0].Integer); }
    break;
case 78:
#line 493 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXHOP, (double)yyvsp[0].Integer); }
    break;
case 79:
#line 503 "ntp_parser.y"
{ append_queue(my_config.stats_list, yyvsp[0].Queue);  }
    break;
case 80:
#line 505 "ntp_parser.y"
{ my_config.stats_dir = yyvsp[0].String;  }
    break;
case 81:
#line 507 "ntp_parser.y"
{
                        enqueue(my_config.filegen_opts,
                                create_filegen_node(yyvsp[-1].VoidPtr, yyvsp[0].Queue));
                    }
    break;
case 82:
#line 514 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); }
    break;
case 83:
#line 515 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].VoidPtr); }
    break;
case 84:
#line 520 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("clockstats"); }
    break;
case 85:
#line 522 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("cryptostats"); }
    break;
case 86:
#line 524 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("loopstats"); }
    break;
case 87:
#line 526 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("peerstats"); }
    break;
case 88:
#line 528 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("rawstats"); }
    break;
case 89:
#line 530 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("sysstats"); }
    break;
case 90:
#line 534 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 91:
#line 535 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 92:
#line 539 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(T_File, yyvsp[0].String); }
    break;
case 93:
#line 540 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Type, yyvsp[0].Integer); }
    break;
case 94:
#line 541 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Link); }
    break;
case 95:
#line 542 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Nolink); }
    break;
case 96:
#line 543 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Enable); }
    break;
case 97:
#line 544 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Disable); }
    break;
case 98:
#line 548 "ntp_parser.y"
{ yyval.Integer = FILEGEN_NONE; }
    break;
case 99:
#line 549 "ntp_parser.y"
{ yyval.Integer = FILEGEN_PID; }
    break;
case 100:
#line 550 "ntp_parser.y"
{ yyval.Integer = FILEGEN_DAY; }
    break;
case 101:
#line 551 "ntp_parser.y"
{ yyval.Integer = FILEGEN_WEEK; }
    break;
case 102:
#line 552 "ntp_parser.y"
{ yyval.Integer = FILEGEN_MONTH; }
    break;
case 103:
#line 553 "ntp_parser.y"
{ yyval.Integer = FILEGEN_YEAR; }
    break;
case 104:
#line 554 "ntp_parser.y"
{ yyval.Integer = FILEGEN_AGE; }
    break;
case 105:
#line 564 "ntp_parser.y"
{   append_queue(my_config.discard_opts, yyvsp[0].Queue); }
    break;
case 106:
#line 566 "ntp_parser.y"
{
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(yyvsp[-1].Address_node, NULL, yyvsp[0].Queue, ip_file->line_no));
                    }
    break;
case 107:
#line 571 "ntp_parser.y"
{
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(NULL, NULL, yyvsp[0].Queue, ip_file->line_no));
                    }
    break;
case 108:
#line 576 "ntp_parser.y"
{
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(yyvsp[-3].Address_node, yyvsp[-1].Address_node, yyvsp[0].Queue, ip_file->line_no));
                    }
    break;
case 109:
#line 583 "ntp_parser.y"
{ yyval.Queue = create_queue(); }
    break;
case 110:
#line 584 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); }
    break;
case 111:
#line 588 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_IGNORE); }
    break;
case 112:
#line 589 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_DEMOBILIZE); }
    break;
case 113:
#line 590 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_LIMITED); }
    break;
case 114:
#line 591 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_LPTRAP); }
    break;
case 115:
#line 592 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOMODIFY); }
    break;
case 116:
#line 593 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOPEER); }
    break;
case 117:
#line 594 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOQUERY); }
    break;
case 118:
#line 595 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_DONTSERVE); }
    break;
case 119:
#line 596 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOTRAP); }
    break;
case 120:
#line 597 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_DONTTRUST); }
    break;
case 121:
#line 598 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RESM_NTPONLY); }
    break;
case 122:
#line 599 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_VERSION); }
    break;
case 123:
#line 603 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 124:
#line 604 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 125:
#line 608 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Average, yyvsp[0].Integer); }
    break;
case 126:
#line 609 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Minimum, yyvsp[0].Integer); }
    break;
case 127:
#line 610 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Monitor, yyvsp[0].Integer); }
    break;
case 128:
#line 619 "ntp_parser.y"
{ enqueue(my_config.fudge, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  }
    break;
case 129:
#line 623 "ntp_parser.y"
{ enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 130:
#line 624 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 131:
#line 629 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(CLK_HAVETIME1, yyvsp[0].Double); }
    break;
case 132:
#line 631 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(CLK_HAVETIME2, yyvsp[0].Double); }
    break;
case 133:
#line 633 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEVAL1,  yyvsp[0].Integer); }
    break;
case 134:
#line 635 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CLK_HAVEVAL2,  yyvsp[0].String); }
    break;
case 135:
#line 637 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG1, yyvsp[0].Integer); }
    break;
case 136:
#line 639 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG2, yyvsp[0].Integer); }
    break;
case 137:
#line 641 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG3, yyvsp[0].Integer); }
    break;
case 138:
#line 643 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG4, yyvsp[0].Integer); }
    break;
case 139:
#line 652 "ntp_parser.y"
{ append_queue(my_config.enable_opts,yyvsp[0].Queue);  }
    break;
case 140:
#line 654 "ntp_parser.y"
{ append_queue(my_config.disable_opts,yyvsp[0].Queue);  }
    break;
case 141:
#line 658 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 142:
#line 659 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 143:
#line 663 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); }
    break;
case 144:
#line 664 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_BROADCLIENT); }
    break;
case 145:
#line 665 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_CAL); }
    break;
case 146:
#line 666 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_KERNEL); }
    break;
case 147:
#line 667 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_MONITOR); }
    break;
case 148:
#line 668 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_NTP); }
    break;
case 149:
#line 670 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_FILEGEN); }
    break;
case 150:
#line 678 "ntp_parser.y"
{ append_queue(my_config.tinker, yyvsp[0].Queue);  }
    break;
case 151:
#line 682 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 152:
#line 683 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 153:
#line 687 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_ALLAN, yyvsp[0].Double); }
    break;
case 154:
#line 688 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_PHI, yyvsp[0].Double); }
    break;
case 155:
#line 689 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_FREQ, yyvsp[0].Double); }
    break;
case 156:
#line 690 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_HUFFPUFF, yyvsp[0].Double); }
    break;
case 157:
#line 691 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_PANIC, yyvsp[0].Double); }
    break;
case 158:
#line 692 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_MAX, yyvsp[0].Double); }
    break;
case 159:
#line 693 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_MINSTEP, yyvsp[0].Double); }
    break;
case 160:
#line 702 "ntp_parser.y"
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
case 161:
#line 718 "ntp_parser.y"
{
                    while (curr_include_level != -1)
                        FCLOSE(fp[curr_include_level--]);
                }
    break;
case 162:
#line 724 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, yyvsp[0].Double));  }
    break;
case 163:
#line 726 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_ival(T_Calldelay, yyvsp[0].Integer));  }
    break;
case 164:
#line 728 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_dval(T_Tick, yyvsp[0].Double));  }
    break;
case 165:
#line 730 "ntp_parser.y"
{ /* Null action, possibly all null parms */ }
    break;
case 166:
#line 732 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Leapfile, yyvsp[0].String)); }
    break;
case 167:
#line 735 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Pidfile, yyvsp[0].String));  }
    break;
case 168:
#line 737 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Logfile, yyvsp[0].String));  }
    break;
case 169:
#line 739 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_ival(T_Automax, yyvsp[0].Integer));  }
    break;
case 170:
#line 742 "ntp_parser.y"
{ append_queue(my_config.logconfig, yyvsp[0].Queue);  }
    break;
case 171:
#line 744 "ntp_parser.y"
{ append_queue(my_config.phone, yyvsp[0].Queue);  }
    break;
case 172:
#line 746 "ntp_parser.y"
{ enqueue(my_config.setvar, yyvsp[0].Set_var);  }
    break;
case 173:
#line 748 "ntp_parser.y"
{ enqueue(my_config.trap, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  }
    break;
case 174:
#line 750 "ntp_parser.y"
{ append_queue(my_config.ttl, yyvsp[0].Queue); }
    break;
case 175:
#line 754 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Driftfile, yyvsp[0].String)); }
    break;
case 176:
#line 756 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_dval(T_WanderThreshold, yyvsp[0].Double));
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, yyvsp[-1].String)); }
    break;
case 177:
#line 758 "ntp_parser.y"
{ /* Null driftfile,  indicated by null string "\0" */
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;
case 178:
#line 764 "ntp_parser.y"
{ yyval.Set_var = create_setvar_node(yyvsp[-3].String, yyvsp[-1].String, DEF); }
    break;
case 179:
#line 766 "ntp_parser.y"
{ yyval.Set_var = create_setvar_node(yyvsp[-2].String, yyvsp[0].String, 0); }
    break;
case 180:
#line 771 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 181:
#line 772 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 182:
#line 776 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Port, yyvsp[0].Integer); }
    break;
case 183:
#line 777 "ntp_parser.y"
{ yyval.Attr_val = create_attr_pval(T_Interface, yyvsp[0].Address_node); }
    break;
case 184:
#line 782 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 185:
#line 783 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 186:
#line 787 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(yyvsp[-1].Integer, yyvsp[0].String); }
    break;
case 187:
#line 789 "ntp_parser.y"
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
case 188:
#line 805 "ntp_parser.y"
{ yyval.Integer = '+'; }
    break;
case 189:
#line 806 "ntp_parser.y"
{ yyval.Integer = '-'; }
    break;
case 190:
#line 807 "ntp_parser.y"
{ yyval.Integer = '='; }
    break;
case 191:
#line 816 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, create_ival(yyvsp[0].Integer)); }
    break;
case 192:
#line 817 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(create_ival(yyvsp[0].Integer)); }
    break;
case 193:
#line 821 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, create_pval(yyvsp[0].String)); }
    break;
case 194:
#line 822 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(create_pval(yyvsp[0].String)); }
    break;
case 195:
#line 826 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Address_node); }
    break;
case 196:
#line 827 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Address_node); }
    break;
case 197:
#line 832 "ntp_parser.y"
{
                    if (yyvsp[0].Integer != 0 && yyvsp[0].Integer != 1) {
                        yyerror("Integer value is not boolean (0 or 1). Assuming 1");
                        yyval.Integer = 1;
                    }
                    else
                        yyval.Integer = yyvsp[0].Integer;
                }
    break;
case 198:
#line 840 "ntp_parser.y"
{ yyval.Integer = 1; }
    break;
case 199:
#line 841 "ntp_parser.y"
{ yyval.Integer = 0; }
    break;
case 200:
#line 845 "ntp_parser.y"
{ yyval.Double = (double)yyvsp[0].Integer; }
    break;
case 201:
#line 846 "ntp_parser.y"
{ yyval.Double = yyvsp[0].Double; }
    break;
case 202:
#line 855 "ntp_parser.y"
{
                 my_config.sim_details = create_sim_node(yyvsp[-2].Queue, yyvsp[-1].Queue);

                 /* Reset the old_config_style variable */
                 old_config_style = 1;
             }
    break;
case 203:
#line 869 "ntp_parser.y"
{ old_config_style = 0; }
    break;
case 204:
#line 873 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); }
    break;
case 205:
#line 874 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); }
    break;
case 206:
#line 878 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Beep_Delay, yyvsp[0].Double); }
    break;
case 207:
#line 879 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Sim_Duration, yyvsp[0].Double); }
    break;
case 208:
#line 883 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_server); }
    break;
case 209:
#line 884 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_server); }
    break;
case 210:
#line 889 "ntp_parser.y"
{ yyval.Sim_server = create_sim_server(yyvsp[-4].Address_node, yyvsp[-2].Double, yyvsp[-1].Queue); }
    break;
case 211:
#line 893 "ntp_parser.y"
{ yyval.Double = yyvsp[-1].Double; }
    break;
case 212:
#line 897 "ntp_parser.y"
{ yyval.Address_node = yyvsp[0].Address_node; }
    break;
case 213:
#line 901 "ntp_parser.y"
{ yyval.Address_node = yyvsp[0].Address_node; }
    break;
case 214:
#line 902 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, T_String); }
    break;
case 215:
#line 906 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_script); }
    break;
case 216:
#line 907 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_script); }
    break;
case 217:
#line 912 "ntp_parser.y"
{ yyval.Sim_script = create_sim_script_info(yyvsp[-3].Double, yyvsp[-1].Queue); }
    break;
case 218:
#line 916 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); }
    break;
case 219:
#line 917 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); }
    break;
case 220:
#line 922 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Freq_Offset, yyvsp[0].Double); }
    break;
case 221:
#line 924 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Wander, yyvsp[0].Double); }
    break;
case 222:
#line 926 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Jitter, yyvsp[0].Double); }
    break;
case 223:
#line 928 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Prop_Delay, yyvsp[0].Double); }
    break;
case 224:
#line 930 "ntp_parser.y"
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
#line 934 "ntp_parser.y"


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

