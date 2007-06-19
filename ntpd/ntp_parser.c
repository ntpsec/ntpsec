/* A Bison parser, made from /usa/neal/neal.ntp-dev/A.pogo/ntpd/../../ntpd/ntp_parser.y
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
# define	T_DriftMinutes	285
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
# define	T_Mvpar	338
# define	T_Nolink	339
# define	T_Nomodify	340
# define	T_None	341
# define	T_Nopeer	342
# define	T_Noquery	343
# define	T_Noselect	344
# define	T_Noserve	345
# define	T_Notrap	346
# define	T_Notrust	347
# define	T_Novolley	348
# define	T_Ntp	349
# define	T_Ntpport	350
# define	T_Orphan	351
# define	T_Panic	352
# define	T_Peer	353
# define	T_Peerstats	354
# define	T_Phone	355
# define	T_Pid	356
# define	T_Pidfile	357
# define	T_Pool	358
# define	T_Port	359
# define	T_Pps	360
# define	T_Preempt	361
# define	T_Prefer	362
# define	T_Pw	363
# define	T_RandFile	364
# define	T_Rawstats	365
# define	T_Refid	366
# define	T_Requestkey	367
# define	T_Restrict	368
# define	T_Revoke	369
# define	T_Server	370
# define	T_Setvar	371
# define	T_Sign	372
# define	T_Statistics	373
# define	T_Stats	374
# define	T_Statsdir	375
# define	T_Step	376
# define	T_Stepout	377
# define	T_Stratum	378
# define	T_String	379
# define	T_Sysstats	380
# define	T_Tick	381
# define	T_Time1	382
# define	T_Time2	383
# define	T_Tinker	384
# define	T_Tos	385
# define	T_Trap	386
# define	T_True	387
# define	T_Trustedkey	388
# define	T_Ttl	389
# define	T_Type	390
# define	T_Version	391
# define	T_Week	392
# define	T_Year	393
# define	T_Flag	394
# define	T_Void	395
# define	T_EOC	396
# define	T_Simulate	397
# define	T_Beep_Delay	398
# define	T_Sim_Duration	399
# define	T_Server_Offset	400
# define	T_Duration	401
# define	T_Freq_Offset	402
# define	T_Wander	403
# define	T_Jitter	404
# define	T_Prop_Delay	405
# define	T_Proc_Delay	406

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

  #define YYERROR_VERBOSE
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

#line 70 "ntp_parser.y"
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



#define	YYFINAL		361
#define	YYFLAG		-32768
#define	YYNTBASE	158

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 406 ? yytranslate[x] : 220)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const short yytranslate[] =
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
     146,   147,   148,   149,   150,   151,   152
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     6,     9,    12,    13,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    39,    42,
      44,    46,    48,    50,    52,    54,    56,    59,    61,    63,
      66,    68,    70,    72,    74,    77,    80,    83,    85,    87,
      89,    91,    94,    97,   100,   102,   105,   108,   111,   114,
     117,   120,   123,   126,   129,   132,   135,   138,   140,   143,
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
     438,   441,   444,   447,   450,   453,   457,   460,   462,   465,
     466,   471,   475,   478,   480,   483,   486,   489,   491,   494,
     496,   498,   500,   502,   505,   507,   510,   512,   515,   517,
     519,   521,   523,   525,   527,   533,   535,   539,   542,   546,
     550,   553,   555,   561,   566,   570,   572,   574,   577,   579,
     586,   590,   593,   597,   601,   605,   609
};
static const short yyrhs[] =
{
     159,     0,   159,   160,   142,     0,   160,   142,     0,     1,
     142,     0,     0,   161,     0,   167,     0,   168,     0,   174,
       0,   180,     0,   171,     0,   185,     0,   188,     0,   191,
       0,   194,     0,   207,     0,   162,   163,   165,     0,   162,
     163,     0,   116,     0,   104,     0,    99,     0,    11,     0,
      68,     0,   164,     0,   125,     0,    54,   125,     0,    47,
       0,    48,     0,   165,   166,     0,   166,     0,     6,     0,
      14,     0,    49,     0,    57,    54,     0,    78,    54,     0,
      74,    54,     0,    90,     0,   107,     0,   108,     0,   133,
       0,   135,    54,     0,    80,    54,     0,   137,    54,     0,
      12,     0,    12,    94,     0,    69,   204,     0,    83,   204,
       0,     6,    54,     0,    21,    54,     0,    22,   169,     0,
      58,   125,     0,    59,   125,     0,   113,    54,     0,   115,
      54,     0,   134,   202,     0,   169,   170,     0,   170,     0,
      18,   125,     0,    61,   125,     0,   110,   125,     0,    45,
     125,     0,   118,   125,     0,    50,   125,     0,    51,   125,
       0,    44,   125,     0,    84,   125,     0,   109,   125,     0,
     131,   172,     0,   172,   173,     0,   173,     0,    17,    54,
       0,    41,    54,     0,    20,   205,     0,    97,    54,     0,
      76,   206,     0,    72,   206,     0,    75,   206,     0,    71,
     206,     0,    79,    54,     0,    10,    54,     0,    73,    54,
       0,   119,   175,     0,   121,   125,     0,    36,   176,   177,
       0,   175,   176,     0,   176,     0,    19,     0,    23,     0,
      66,     0,   100,     0,   111,     0,   126,     0,   177,   178,
       0,   178,     0,    35,   125,     0,   136,   179,     0,    63,
       0,    85,     0,    32,     0,    26,     0,    87,     0,   102,
       0,    24,     0,   138,     0,    82,     0,   139,     0,     3,
       0,    27,   183,     0,   114,   163,   181,     0,   114,    25,
     181,     0,   114,   164,    70,   164,   181,     0,     0,   181,
     182,     0,    52,     0,    60,     0,    62,     0,    67,     0,
      86,     0,    88,     0,    89,     0,    91,     0,    92,     0,
      93,     0,    96,     0,   137,     0,   183,   184,     0,   184,
       0,     8,    54,     0,    77,    54,     0,    81,    54,     0,
      43,   163,   186,     0,   186,   187,     0,   187,     0,   128,
     206,     0,   129,   206,     0,   124,    54,     0,   112,   125,
       0,    37,   205,     0,    38,   205,     0,    39,   205,     0,
      40,   205,     0,    32,   189,     0,    26,   189,     0,   189,
     190,     0,   190,     0,     5,     0,     9,     0,    15,     0,
      56,     0,    81,     0,    95,     0,   120,     0,   130,   192,
       0,   192,   193,     0,   193,     0,     4,   206,     0,    28,
     206,     0,    42,   206,     0,    46,   206,     0,    98,   206,
       0,   122,   206,     0,   123,   206,     0,    53,   125,   160,
       0,    33,     0,    13,   206,     0,    16,    54,     0,   127,
     206,     0,    30,   195,     0,   103,   125,     0,    65,   125,
       0,     7,    54,     0,    64,   199,     0,   101,   203,     0,
     117,   196,     0,   132,   164,   197,     0,   135,   202,     0,
     125,     0,   125,    54,     0,     0,   125,   153,   125,    25,
       0,   125,   153,   125,     0,   197,   198,     0,   198,     0,
     105,    54,     0,    55,   164,     0,   199,   200,     0,   200,
       0,   201,   125,     0,   125,     0,   154,     0,   155,     0,
     153,     0,   202,    54,     0,    54,     0,   203,   125,     0,
     125,     0,   204,   163,     0,   163,     0,    54,     0,   133,
       0,    34,     0,    54,     0,    29,     0,   208,   156,   209,
     211,   157,     0,   143,     0,   209,   210,   142,     0,   210,
     142,     0,   144,   153,   206,     0,   145,   153,   206,     0,
     211,   212,     0,   212,     0,   214,   156,   213,   216,   157,
       0,   146,   153,   206,   142,     0,   116,   153,   215,     0,
     164,     0,   125,     0,   216,   217,     0,   217,     0,   147,
     153,   206,   156,   218,   157,     0,   218,   219,   142,     0,
     219,   142,     0,   148,   153,   206,     0,   149,   153,   206,
       0,   150,   153,   206,     0,   151,   153,   206,     0,   152,
     153,   206,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   301,   305,   306,   311,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   340,   346,   355,
     356,   357,   358,   359,   363,   364,   365,   381,   382,   386,
     387,   392,   393,   394,   395,   396,   397,   398,   399,   400,
     401,   402,   403,   404,   414,   416,   418,   420,   431,   433,
     435,   441,   443,   445,   447,   449,   454,   455,   459,   461,
     463,   465,   467,   469,   471,   473,   475,   477,   487,   492,
     493,   497,   499,   501,   503,   505,   507,   509,   511,   513,
     515,   517,   527,   529,   531,   539,   540,   544,   546,   548,
     550,   552,   554,   559,   560,   564,   565,   566,   567,   568,
     569,   573,   574,   575,   576,   577,   578,   579,   588,   590,
     595,   600,   608,   609,   613,   614,   615,   616,   617,   618,
     619,   620,   621,   622,   623,   624,   628,   629,   633,   634,
     635,   643,   648,   649,   653,   655,   657,   659,   661,   663,
     665,   667,   676,   678,   683,   684,   688,   689,   690,   691,
     692,   693,   695,   703,   707,   708,   712,   713,   714,   715,
     716,   717,   718,   726,   742,   748,   750,   752,   754,   757,
     759,   761,   764,   766,   768,   770,   772,   776,   778,   781,
     786,   788,   794,   795,   799,   800,   805,   806,   810,   811,
     828,   829,   830,   839,   840,   844,   845,   849,   850,   854,
     863,   864,   868,   869,   877,   892,   896,   897,   901,   902,
     906,   907,   911,   916,   920,   924,   925,   929,   930,   934,
     939,   940,   944,   946,   948,   950,   952
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
  "'='", "'+'", "'-'", "'{'", "'}'", "configuration", "command_list", 
  "command", "server_command", "client_type", "address", "ip_address", 
  "option_list", "option", "other_mode_command", "authentication_command", 
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
       0,   158,   159,   159,   159,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   161,   161,   162,
     162,   162,   162,   162,   163,   163,   163,   164,   164,   165,
     165,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   167,   167,   167,   167,   168,   168,
     168,   168,   168,   168,   168,   168,   169,   169,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   171,   172,
     172,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   174,   174,   174,   175,   175,   176,   176,   176,
     176,   176,   176,   177,   177,   178,   178,   178,   178,   178,
     178,   179,   179,   179,   179,   179,   179,   179,   180,   180,
     180,   180,   181,   181,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   183,   183,   184,   184,
     184,   185,   186,   186,   187,   187,   187,   187,   187,   187,
     187,   187,   188,   188,   189,   189,   190,   190,   190,   190,
     190,   190,   190,   191,   192,   192,   193,   193,   193,   193,
     193,   193,   193,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   195,   195,   195,
     196,   196,   197,   197,   198,   198,   199,   199,   200,   200,
     201,   201,   201,   202,   202,   203,   203,   204,   204,   205,
     205,   205,   206,   206,   207,   208,   209,   209,   210,   210,
     211,   211,   212,   213,   214,   215,   215,   216,   216,   217,
     218,   218,   219,   219,   219,   219,   219
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     3,     2,     2,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     2,
       1,     1,     1,     1,     2,     2,     2,     1,     1,     1,
       1,     2,     2,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     2,     2,
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
       2,     2,     2,     2,     2,     3,     2,     1,     2,     0,
       4,     3,     2,     1,     2,     2,     2,     1,     2,     1,
       1,     1,     1,     2,     1,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     5,     1,     3,     2,     3,     3,
       2,     1,     5,     4,     3,     1,     1,     2,     1,     6,
       3,     2,     3,     3,     3,     3,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     0,     0,     0,    22,    44,     0,     0,     0,     0,
       0,     0,   179,     0,   164,     0,     0,     0,     0,     0,
       0,     0,    23,     0,     0,    21,     0,     0,    20,     0,
       0,     0,    19,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   205,     1,     0,     6,     0,     7,     8,    11,
       9,    10,    12,    13,    14,    15,    16,     0,     4,    48,
     171,    45,   203,   202,   165,   166,    49,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    50,    57,   146,
     147,   148,   149,   150,   151,   152,   143,   145,     0,     0,
       0,   108,   127,   177,   168,   142,    87,    88,    89,    90,
      91,    92,     0,    27,    28,     0,    25,     0,    24,     5,
      51,    52,   189,   192,   190,   191,   172,   187,     0,   170,
     198,    46,    47,   196,   173,   169,    53,   112,   112,    24,
      54,     0,   174,    82,    86,    83,   167,     0,     0,     0,
       0,     0,     0,     0,   153,   155,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    68,    70,     0,
     194,    55,   176,     0,     3,    18,     0,    58,    65,    61,
      63,    64,    59,    66,    67,    60,    62,    56,   144,   128,
     129,   130,   126,   178,   100,    99,     0,    97,    98,     0,
      84,    94,    26,     0,     0,     0,     0,     0,     0,     0,
       0,   131,   133,   163,   186,   188,   197,   195,   110,   109,
       0,     0,    85,   156,   157,   158,   159,   160,   161,   162,
     154,    80,    71,   201,   199,   200,    73,    72,    78,    76,
      81,    77,    75,    79,    74,    69,     0,     0,   175,   183,
     193,     2,    31,    32,    33,     0,     0,     0,     0,    37,
      38,    39,    40,     0,     0,    17,    30,     0,     0,     0,
       0,    95,   107,   103,   105,   101,   102,   104,   106,    96,
      93,   138,   139,   140,   141,   137,   136,   134,   135,   132,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   113,   112,   181,   185,   184,   182,    34,    36,
      35,    42,    41,    43,    29,     0,     0,     0,     0,     0,
     211,     0,   207,   111,   180,   208,   209,     0,   206,   204,
     210,     0,   216,   215,   214,     0,     0,     0,     0,     0,
     218,     0,     0,   212,   217,   213,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     219,     0,   221,   222,   223,   224,   225,   226,   220,     0,
       0,     0
};

static const short yydefgoto[] =
{
     359,    43,    44,    45,    46,   120,   108,   255,   256,    47,
      48,    77,    78,    49,   157,   158,    50,   133,   102,   190,
     191,   269,    51,   208,   292,    91,    92,    52,   201,   202,
      53,    86,    87,    54,   144,   145,    55,    94,   132,   238,
     239,   116,   117,   118,   161,   124,   121,   226,    64,    56,
      57,   259,   260,   309,   310,   326,   311,   324,   329,   330,
     343,   344
};

static const short yypact[] =
{
      26,  -125,   -34,   -26,-32768,   -64,   -23,     0,     6,   363,
      78,    -5,   -60,    78,-32768,    44,   -43,   -57,   -52,   -50,
     -55,   -48,-32768,   -43,   -43,-32768,   -47,   -45,-32768,    27,
     140,    32,-32768,   -37,    44,    -9,   -23,    15,   176,    -7,
      35,    35,-32768,   203,   -46,-32768,   -43,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,   -32,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,     1,     7,     8,
      11,    21,    25,    41,    46,    47,    53,   363,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,    78,-32768,    77,    81,
      97,    -5,-32768,    98,-32768,    78,-32768,-32768,-32768,-32768,
  -32768,-32768,   -14,-32768,-32768,    55,-32768,   152,-32768,   336,
  -32768,-32768,-32768,-32768,-32768,-32768,   -55,-32768,    57,-32768,
  -32768,   -43,   -43,-32768,    59,-32768,-32768,-32768,-32768,   127,
  -32768,    52,-32768,    44,-32768,-32768,-32768,   -23,   -23,   -23,
     -23,   -23,   -23,   -23,    15,-32768,   150,   157,   -10,   164,
     -23,   -23,   166,   -23,   -23,   167,   168,   176,-32768,   -41,
  -32768,   169,   169,    64,-32768,   105,  -122,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,   101,-32768,-32768,    10,
     -14,-32768,-32768,   -10,   -10,   -10,   -10,   102,   174,   -23,
     -23,   152,-32768,-32768,-32768,-32768,-32768,-32768,   394,   394,
      -7,   106,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,    -7,   178,   -41,-32768,
  -32768,-32768,-32768,-32768,-32768,   180,   183,   187,   189,-32768,
  -32768,-32768,-32768,   190,   191,   105,-32768,   100,   104,  -109,
     108,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,   229,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,   -23,   -23,   107,   116,   -91,
  -32768,   103,-32768,   394,-32768,-32768,-32768,     3,-32768,-32768,
  -32768,   117,-32768,-32768,-32768,   113,   128,   -23,   121,  -131,
  -32768,   135,   -23,-32768,-32768,-32768,   122,   149,   126,   129,
     130,   131,   132,    51,   145,   -23,   -23,   -23,   -23,   -23,
  -32768,   147,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   290,
     292,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,   -35,-32768,-32768,    -1,   -29,-32768,    38,-32768,
  -32768,-32768,   217,-32768,-32768,   138,-32768,-32768,   -25,-32768,
     115,-32768,-32768,  -126,-32768,-32768,   212,-32768,-32768,   114,
  -32768,   295,   -40,-32768,-32768,   170,-32768,-32768,-32768,-32768,
      83,-32768,   207,-32768,   284,-32768,   302,   -19,   -36,-32768,
  -32768,-32768,    68,-32768,    19,-32768,-32768,-32768,-32768,     2,
  -32768,   -11
};


#define	YYLAST		531


static const short yytable[] =
{
     136,   129,   209,    88,   103,   104,    62,   307,   163,   134,
     159,   105,   184,   262,   236,   107,   328,    58,   185,   137,
      59,   186,   257,   258,   223,   307,   333,     1,    60,   128,
      61,    63,     2,     3,   263,   257,   258,     4,     5,     6,
     103,   104,     7,   138,   224,   165,   178,     8,     9,   187,
     103,   104,    10,    11,    65,   178,    12,   139,    13,    14,
      66,   140,    15,    96,   237,    93,   319,    97,   109,    16,
     112,   188,    89,   110,   203,   111,    90,   119,   123,    17,
     125,   126,   106,    79,    18,    19,   130,    80,   131,   160,
      20,    21,   264,    81,    22,    23,   164,   265,   113,   114,
     115,   213,   214,   215,   216,   217,   218,   219,   212,    24,
      98,   242,   266,   141,   228,   229,   135,   231,   232,   243,
     206,   206,   189,   225,   166,    25,   167,    26,   322,    27,
      28,   179,   168,   169,    82,   180,   170,   142,   143,    29,
      30,    31,    32,    33,    99,    34,   171,    35,   267,   268,
     172,   181,   183,    36,   244,   100,    37,    38,    39,    83,
      40,    41,   245,   277,   278,   127,   173,   313,    -5,    42,
     101,   174,   175,    84,   271,   272,   273,   274,   176,   246,
     192,   293,   205,   247,   207,   248,   146,   103,   104,   193,
     194,   195,   196,   147,   105,   249,   148,   210,    85,   338,
     339,   340,   341,   342,   221,   211,   241,   295,   350,     2,
       3,   222,   250,   251,     4,     5,     6,   149,   227,     7,
     230,   233,   234,   240,     8,     9,   261,   275,   276,    10,
      11,   294,   296,    12,   298,    13,    14,   299,   252,    15,
     253,   300,   254,   301,   302,   303,    16,   150,   151,   152,
     312,   153,   154,   305,   314,   155,    17,   306,   318,   321,
     317,    18,    19,   325,   197,   106,   327,    20,    21,   315,
     316,    22,    23,   156,   332,   328,   198,   335,   337,   345,
     199,   200,   346,   347,   348,   349,    24,   352,   323,   358,
     360,   331,   361,   304,   177,   235,   336,   338,   339,   340,
     341,   342,    25,   182,    26,   270,    27,    28,    95,   353,
     354,   355,   356,   357,   220,   279,    29,    30,    31,    32,
      33,   297,    34,   204,    35,   162,   122,   308,   320,     0,
      36,   334,   351,    37,    38,    39,     0,    40,    41,     0,
       0,     0,     2,     3,     0,    -5,    42,     4,     5,     6,
       0,     0,     7,     0,     0,     0,     0,     8,     9,     0,
       0,     0,    10,    11,     0,     0,    12,     0,    13,    14,
       0,     0,    15,     0,     0,     0,     0,     0,     0,    16,
       0,    67,     0,     0,     0,     0,     0,     0,     0,    17,
       0,     0,     0,     0,    18,    19,     0,     0,     0,     0,
      20,    21,     0,     0,    22,    23,     0,    68,    69,     0,
       0,     0,     0,    70,    71,     0,     0,     0,     0,    24,
       0,     0,     0,     0,    72,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    25,     0,    26,     0,    27,
      28,     0,     0,     0,     0,     0,   280,    73,     0,    29,
      30,    31,    32,    33,   281,    34,   282,    35,     0,     0,
       0,   283,     0,    36,     0,     0,    37,    38,    39,     0,
      40,    41,    74,    75,     0,     0,     0,     0,     0,    42,
     284,    76,   285,   286,     0,   287,   288,   289,     0,     0,
     290,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   291
};

static const short yycheck[] =
{
      36,    30,   128,     8,    47,    48,    29,   116,    43,    34,
      39,    54,    26,     3,    55,    16,   147,   142,    32,     4,
      54,    35,   144,   145,    34,   116,   157,     1,    54,    30,
      94,    54,     6,     7,    24,   144,   145,    11,    12,    13,
      47,    48,    16,    28,    54,    46,    86,    21,    22,    63,
      47,    48,    26,    27,    54,    95,    30,    42,    32,    33,
      54,    46,    36,    19,   105,   125,   157,    23,   125,    43,
     125,    85,    77,   125,   109,   125,    81,   125,   125,    53,
     125,    54,   125,     5,    58,    59,    54,     9,   125,    54,
      64,    65,    82,    15,    68,    69,   142,    87,   153,   154,
     155,   137,   138,   139,   140,   141,   142,   143,   133,    83,
      66,     6,   102,    98,   150,   151,   125,   153,   154,    14,
     121,   122,   136,   133,   156,    99,   125,   101,   125,   103,
     104,    54,   125,   125,    56,    54,   125,   122,   123,   113,
     114,   115,   116,   117,   100,   119,   125,   121,   138,   139,
     125,    54,    54,   127,    49,   111,   130,   131,   132,    81,
     134,   135,    57,   199,   200,    25,   125,   293,   142,   143,
     126,   125,   125,    95,   193,   194,   195,   196,   125,    74,
     125,   210,   125,    78,   125,    80,    10,    47,    48,    37,
      38,    39,    40,    17,    54,    90,    20,    70,   120,   148,
     149,   150,   151,   152,    54,   153,   142,   236,   157,     6,
       7,    54,   107,   108,    11,    12,    13,    41,    54,    16,
      54,    54,    54,    54,    21,    22,   125,   125,    54,    26,
      27,   125,    54,    30,    54,    32,    33,    54,   133,    36,
     135,    54,   137,    54,    54,    54,    43,    71,    72,    73,
     142,    75,    76,   153,    25,    79,    53,   153,   142,   156,
     153,    58,    59,   146,   112,   125,   153,    64,    65,   305,
     306,    68,    69,    97,   153,   147,   124,   142,   156,   153,
     128,   129,   153,   153,   153,   153,    83,   142,   317,   142,
       0,   327,     0,   255,    77,   157,   332,   148,   149,   150,
     151,   152,    99,    91,   101,   190,   103,   104,    13,   345,
     346,   347,   348,   349,   144,   201,   113,   114,   115,   116,
     117,   238,   119,   116,   121,    41,    24,   259,   309,    -1,
     127,   329,   343,   130,   131,   132,    -1,   134,   135,    -1,
      -1,    -1,     6,     7,    -1,   142,   143,    11,    12,    13,
      -1,    -1,    16,    -1,    -1,    -1,    -1,    21,    22,    -1,
      -1,    -1,    26,    27,    -1,    -1,    30,    -1,    32,    33,
      -1,    -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,
      -1,    -1,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,
      64,    65,    -1,    -1,    68,    69,    -1,    44,    45,    -1,
      -1,    -1,    -1,    50,    51,    -1,    -1,    -1,    -1,    83,
      -1,    -1,    -1,    -1,    61,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    99,    -1,   101,    -1,   103,
     104,    -1,    -1,    -1,    -1,    -1,    52,    84,    -1,   113,
     114,   115,   116,   117,    60,   119,    62,   121,    -1,    -1,
      -1,    67,    -1,   127,    -1,    -1,   130,   131,   132,    -1,
     134,   135,   109,   110,    -1,    -1,    -1,    -1,    -1,   143,
      86,   118,    88,    89,    -1,    91,    92,    93,    -1,    -1,
      96,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   137
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
#line 312 "ntp_parser.y"
{ 
					if (input_from_file == 1) {
                    	msyslog(LOG_INFO, "parse error %s line %d ignored\n",
                            ip_file->fname, ip_file->line_no);
                	} else if (input_from_file != 0)
						msyslog(LOG_INFO, "parse: bad boolean input flag\n"); 
                }
    break;
case 17:
#line 341 "ntp_parser.y"
{ 
                        struct peer_node *my_node =  create_peer_node(yyvsp[-2].Integer, yyvsp[-1].Address_node, yyvsp[0].Queue); 
                        if (my_node)
                            enqueue(my_config.peers, my_node);                        
                    }
    break;
case 18:
#line 347 "ntp_parser.y"
{ 
                        struct peer_node *my_node = create_peer_node(yyvsp[-1].Integer, yyvsp[0].Address_node, NULL); 
                        if (my_node)
                            enqueue(my_config.peers, my_node);                        
                    }
    break;
case 19:
#line 355 "ntp_parser.y"
{ yyval.Integer = T_Server; }
    break;
case 20:
#line 356 "ntp_parser.y"
{ yyval.Integer = T_Pool; }
    break;
case 21:
#line 357 "ntp_parser.y"
{ yyval.Integer = T_Peer; }
    break;
case 22:
#line 358 "ntp_parser.y"
{ yyval.Integer = T_Broadcast; }
    break;
case 23:
#line 359 "ntp_parser.y"
{ yyval.Integer = T_Manycastclient; }
    break;
case 24:
#line 363 "ntp_parser.y"
{ yyval.Address_node = yyvsp[0].Address_node; }
    break;
case 25:
#line 364 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, default_ai_family); }
    break;
case 26:
#line 366 "ntp_parser.y"
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
#line 381 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET); }
    break;
case 28:
#line 382 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET6); }
    break;
case 29:
#line 386 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 30:
#line 387 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 31:
#line 392 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_SKEY | FLAG_AUTHENABLE); }
    break;
case 32:
#line 393 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_BURST); }
    break;
case 33:
#line 394 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_IBURST); }
    break;
case 34:
#line 395 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Key, yyvsp[0].Integer); }
    break;
case 35:
#line 396 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Minpoll, yyvsp[0].Integer); }
    break;
case 36:
#line 397 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Maxpoll, yyvsp[0].Integer); }
    break;
case 37:
#line 398 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_NOSELECT);}
    break;
case 38:
#line 399 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREEMPT); }
    break;
case 39:
#line 400 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREFER); }
    break;
case 40:
#line 401 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_TRUE); }
    break;
case 41:
#line 402 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Ttl, yyvsp[0].Integer); }
    break;
case 42:
#line 403 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Mode, yyvsp[0].Integer); }
    break;
case 43:
#line 404 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Version, yyvsp[0].Integer); }
    break;
case 44:
#line 415 "ntp_parser.y"
{ my_config.broadcastclient = SIMPLE; }
    break;
case 45:
#line 417 "ntp_parser.y"
{ my_config.broadcastclient = NOVOLLEY;  }
    break;
case 46:
#line 419 "ntp_parser.y"
{ append_queue(my_config.manycastserver, yyvsp[0].Queue);  }
    break;
case 47:
#line 421 "ntp_parser.y"
{ append_queue(my_config.multicastclient, yyvsp[0].Queue);  }
    break;
case 48:
#line 432 "ntp_parser.y"
{ my_config.auth.autokey = yyvsp[0].Integer;  }
    break;
case 49:
#line 434 "ntp_parser.y"
{ my_config.auth.control_key = yyvsp[0].Integer;  }
    break;
case 50:
#line 436 "ntp_parser.y"
{ if (my_config.auth.crypto_cmd_list != NULL) 
					append_queue(my_config.auth.crypto_cmd_list, yyvsp[0].Queue); 
		 		else 
					my_config.auth.crypto_cmd_list = yyvsp[0].Queue;
			}
    break;
case 51:
#line 442 "ntp_parser.y"
{ my_config.auth.keys = yyvsp[0].String;  }
    break;
case 52:
#line 444 "ntp_parser.y"
{ my_config.auth.keysdir = yyvsp[0].String;  }
    break;
case 53:
#line 446 "ntp_parser.y"
{ my_config.auth.requested_key = yyvsp[0].Integer;  }
    break;
case 54:
#line 448 "ntp_parser.y"
{ my_config.auth.revoke = yyvsp[0].Integer;  }
    break;
case 55:
#line 450 "ntp_parser.y"
{ my_config.auth.trusted_key_list = yyvsp[0].Queue;  }
    break;
case 56:
#line 454 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 57:
#line 455 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 58:
#line 460 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_CERT, yyvsp[0].String); }
    break;
case 59:
#line 462 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_LEAP, yyvsp[0].String); }
    break;
case 60:
#line 464 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_RAND, yyvsp[0].String); }
    break;
case 61:
#line 466 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PRIV, yyvsp[0].String); }
    break;
case 62:
#line 468 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_SIGN, yyvsp[0].String); }
    break;
case 63:
#line 470 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_IDENT, yyvsp[0].String); }
    break;
case 64:
#line 472 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_IFFPAR, yyvsp[0].String); }
    break;
case 65:
#line 474 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_GQPAR, yyvsp[0].String); }
    break;
case 66:
#line 476 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_MVPAR, yyvsp[0].String); }
    break;
case 67:
#line 478 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PW, yyvsp[0].String); }
    break;
case 68:
#line 488 "ntp_parser.y"
{ append_queue(my_config.orphan_cmds,yyvsp[0].Queue);  }
    break;
case 69:
#line 492 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 70:
#line 493 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 71:
#line 498 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_CEILING, (double)yyvsp[0].Integer); }
    break;
case 72:
#line 500 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_FLOOR, (double)yyvsp[0].Integer); }
    break;
case 73:
#line 502 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_COHORT, (double)yyvsp[0].Integer); }
    break;
case 74:
#line 504 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_ORPHAN, (double)yyvsp[0].Integer); }
    break;
case 75:
#line 506 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINDISP, yyvsp[0].Double); }
    break;
case 76:
#line 508 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXDIST, yyvsp[0].Double); }
    break;
case 77:
#line 510 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINCLOCK, yyvsp[0].Double); }
    break;
case 78:
#line 512 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXCLOCK, yyvsp[0].Double); }
    break;
case 79:
#line 514 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINSANE, (double)yyvsp[0].Integer); }
    break;
case 80:
#line 516 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_BEACON, (double)yyvsp[0].Integer); }
    break;
case 81:
#line 518 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXHOP, (double)yyvsp[0].Integer); }
    break;
case 82:
#line 528 "ntp_parser.y"
{ append_queue(my_config.stats_list, yyvsp[0].Queue);  }
    break;
case 83:
#line 530 "ntp_parser.y"
{ my_config.stats_dir = yyvsp[0].String;  }
    break;
case 84:
#line 532 "ntp_parser.y"
{
                        enqueue(my_config.filegen_opts, 
                                create_filegen_node(yyvsp[-1].VoidPtr, yyvsp[0].Queue));                         
                    }
    break;
case 85:
#line 539 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); }
    break;
case 86:
#line 540 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].VoidPtr); }
    break;
case 87:
#line 545 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("clockstats"); }
    break;
case 88:
#line 547 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("cryptostats"); }
    break;
case 89:
#line 549 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("loopstats"); }
    break;
case 90:
#line 551 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("peerstats"); }
    break;
case 91:
#line 553 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("rawstats"); }
    break;
case 92:
#line 555 "ntp_parser.y"
{ yyval.VoidPtr = create_pval("sysstats"); }
    break;
case 93:
#line 559 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 94:
#line 560 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 95:
#line 564 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(T_File, yyvsp[0].String); }
    break;
case 96:
#line 565 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Type, yyvsp[0].Integer); }
    break;
case 97:
#line 566 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Link); }
    break;
case 98:
#line 567 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Nolink); }
    break;
case 99:
#line 568 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Enable); }
    break;
case 100:
#line 569 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Disable); }
    break;
case 101:
#line 573 "ntp_parser.y"
{ yyval.Integer = FILEGEN_NONE; }
    break;
case 102:
#line 574 "ntp_parser.y"
{ yyval.Integer = FILEGEN_PID; }
    break;
case 103:
#line 575 "ntp_parser.y"
{ yyval.Integer = FILEGEN_DAY; }
    break;
case 104:
#line 576 "ntp_parser.y"
{ yyval.Integer = FILEGEN_WEEK; }
    break;
case 105:
#line 577 "ntp_parser.y"
{ yyval.Integer = FILEGEN_MONTH; }
    break;
case 106:
#line 578 "ntp_parser.y"
{ yyval.Integer = FILEGEN_YEAR; }
    break;
case 107:
#line 579 "ntp_parser.y"
{ yyval.Integer = FILEGEN_AGE; }
    break;
case 108:
#line 589 "ntp_parser.y"
{   append_queue(my_config.discard_opts, yyvsp[0].Queue); }
    break;
case 109:
#line 591 "ntp_parser.y"
{ 
                        enqueue(my_config.restrict_opts, 
                                create_restrict_node(yyvsp[-1].Address_node, NULL, yyvsp[0].Queue, ip_file->line_no));                          
                    }
    break;
case 110:
#line 596 "ntp_parser.y"
{ 
                        enqueue(my_config.restrict_opts, 
                                create_restrict_node(NULL, NULL, yyvsp[0].Queue, ip_file->line_no)); 
                    }
    break;
case 111:
#line 601 "ntp_parser.y"
{ 
                        enqueue(my_config.restrict_opts, 
                                create_restrict_node(yyvsp[-3].Address_node, yyvsp[-1].Address_node, yyvsp[0].Queue, ip_file->line_no)); 
                    }
    break;
case 112:
#line 608 "ntp_parser.y"
{ yyval.Queue = create_queue(); }
    break;
case 113:
#line 609 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); }
    break;
case 114:
#line 613 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_IGNORE); }
    break;
case 115:
#line 614 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_DEMOBILIZE); }
    break;
case 116:
#line 615 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_LIMITED); }
    break;
case 117:
#line 616 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_LPTRAP); }
    break;
case 118:
#line 617 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOMODIFY); }
    break;
case 119:
#line 618 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOPEER); }
    break;
case 120:
#line 619 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOQUERY); }
    break;
case 121:
#line 620 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_DONTSERVE); }
    break;
case 122:
#line 621 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_NOTRAP); }
    break;
case 123:
#line 622 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_DONTTRUST); }
    break;
case 124:
#line 623 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RESM_NTPONLY); }
    break;
case 125:
#line 624 "ntp_parser.y"
{ yyval.VoidPtr = create_ival(RES_VERSION); }
    break;
case 126:
#line 628 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 127:
#line 629 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 128:
#line 633 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Average, yyvsp[0].Integer); }
    break;
case 129:
#line 634 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Minimum, yyvsp[0].Integer); }
    break;
case 130:
#line 635 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Monitor, yyvsp[0].Integer); }
    break;
case 131:
#line 644 "ntp_parser.y"
{ enqueue(my_config.fudge, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  }
    break;
case 132:
#line 648 "ntp_parser.y"
{ enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 133:
#line 649 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 134:
#line 654 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(CLK_HAVETIME1, yyvsp[0].Double); }
    break;
case 135:
#line 656 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(CLK_HAVETIME2, yyvsp[0].Double); }
    break;
case 136:
#line 658 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEVAL1,  yyvsp[0].Integer); }
    break;
case 137:
#line 660 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(CLK_HAVEVAL2,  yyvsp[0].String); }
    break;
case 138:
#line 662 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG1, yyvsp[0].Integer); }
    break;
case 139:
#line 664 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG2, yyvsp[0].Integer); }
    break;
case 140:
#line 666 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG3, yyvsp[0].Integer); }
    break;
case 141:
#line 668 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG4, yyvsp[0].Integer); }
    break;
case 142:
#line 677 "ntp_parser.y"
{ append_queue(my_config.enable_opts,yyvsp[0].Queue);  }
    break;
case 143:
#line 679 "ntp_parser.y"
{ append_queue(my_config.disable_opts,yyvsp[0].Queue);  }
    break;
case 144:
#line 683 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 145:
#line 684 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 146:
#line 688 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); }
    break;
case 147:
#line 689 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_BROADCLIENT); }
    break;
case 148:
#line 690 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_CAL); }
    break;
case 149:
#line 691 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_KERNEL); }
    break;
case 150:
#line 692 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_MONITOR); }
    break;
case 151:
#line 693 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_NTP); }
    break;
case 152:
#line 695 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_FILEGEN); }
    break;
case 153:
#line 703 "ntp_parser.y"
{ append_queue(my_config.tinker, yyvsp[0].Queue);  }
    break;
case 154:
#line 707 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 155:
#line 708 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 156:
#line 712 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_ALLAN, yyvsp[0].Double); }
    break;
case 157:
#line 713 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_PHI, yyvsp[0].Double); }
    break;
case 158:
#line 714 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_FREQ, yyvsp[0].Double); }
    break;
case 159:
#line 715 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_HUFFPUFF, yyvsp[0].Double); }
    break;
case 160:
#line 716 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_PANIC, yyvsp[0].Double); }
    break;
case 161:
#line 717 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_MAX, yyvsp[0].Double); }
    break;
case 162:
#line 718 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(LOOP_MINSTEP, yyvsp[0].Double); }
    break;
case 163:
#line 727 "ntp_parser.y"
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
#line 743 "ntp_parser.y"
{
                    while (curr_include_level != -1) 
                        FCLOSE(fp[curr_include_level--]);
                }
    break;
case 165:
#line 749 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, yyvsp[0].Double));  }
    break;
case 166:
#line 751 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_ival(T_Calldelay, yyvsp[0].Integer));  }
    break;
case 167:
#line 753 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_dval(T_Tick, yyvsp[0].Double));  }
    break;
case 168:
#line 755 "ntp_parser.y"
{ /* Null action, possibly all null parms */ }
    break;
case 169:
#line 758 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Pidfile, yyvsp[0].String));  }
    break;
case 170:
#line 760 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Logfile, yyvsp[0].String));  }
    break;
case 171:
#line 762 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_ival(T_Automax, yyvsp[0].Integer));  }
    break;
case 172:
#line 765 "ntp_parser.y"
{ append_queue(my_config.logconfig, yyvsp[0].Queue);  }
    break;
case 173:
#line 767 "ntp_parser.y"
{ append_queue(my_config.phone, yyvsp[0].Queue);  }
    break;
case 174:
#line 769 "ntp_parser.y"
{ enqueue(my_config.setvar, yyvsp[0].Set_var);  }
    break;
case 175:
#line 771 "ntp_parser.y"
{ enqueue(my_config.trap, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  }
    break;
case 176:
#line 773 "ntp_parser.y"
{ append_queue(my_config.ttl, yyvsp[0].Queue); }
    break;
case 177:
#line 777 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_sval(T_Driftfile, yyvsp[0].String)); }
    break;
case 178:
#line 779 "ntp_parser.y"
{ enqueue(my_config.vars, create_attr_ival(T_DriftMinutes, yyvsp[0].Integer)); 
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, yyvsp[-1].String)); }
    break;
case 179:
#line 781 "ntp_parser.y"
{ /* Null driftfile,  indicated by null string "\0" */ 
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;
case 180:
#line 787 "ntp_parser.y"
{ yyval.Set_var = create_setvar_node(yyvsp[-3].String, yyvsp[-1].String, DEF); }
    break;
case 181:
#line 789 "ntp_parser.y"
{ yyval.Set_var = create_setvar_node(yyvsp[-2].String, yyvsp[0].String, 0); }
    break;
case 182:
#line 794 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 183:
#line 795 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 184:
#line 799 "ntp_parser.y"
{ yyval.Attr_val = create_attr_ival(T_Port, yyvsp[0].Integer); }
    break;
case 185:
#line 800 "ntp_parser.y"
{ yyval.Attr_val = create_attr_pval(T_Interface, yyvsp[0].Address_node); }
    break;
case 186:
#line 805 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); }
    break;
case 187:
#line 806 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); }
    break;
case 188:
#line 810 "ntp_parser.y"
{ yyval.Attr_val = create_attr_sval(yyvsp[-1].Integer, yyvsp[0].String); }
    break;
case 189:
#line 812 "ntp_parser.y"
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
case 190:
#line 828 "ntp_parser.y"
{ yyval.Integer = '+'; }
    break;
case 191:
#line 829 "ntp_parser.y"
{ yyval.Integer = '-'; }
    break;
case 192:
#line 830 "ntp_parser.y"
{ yyval.Integer = '='; }
    break;
case 193:
#line 839 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, create_ival(yyvsp[0].Integer)); }
    break;
case 194:
#line 840 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(create_ival(yyvsp[0].Integer)); }
    break;
case 195:
#line 844 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, create_pval(yyvsp[0].String)); }
    break;
case 196:
#line 845 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(create_pval(yyvsp[0].String)); }
    break;
case 197:
#line 849 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Address_node); }
    break;
case 198:
#line 850 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Address_node); }
    break;
case 199:
#line 855 "ntp_parser.y"
{ 
                    if (yyvsp[0].Integer != 0 && yyvsp[0].Integer != 1) {
                        yyerror("Integer value is not boolean (0 or 1). Assuming 1");
                        yyval.Integer = 1;
                    }
                    else
                        yyval.Integer = yyvsp[0].Integer; 
                }
    break;
case 200:
#line 863 "ntp_parser.y"
{ yyval.Integer = 1; }
    break;
case 201:
#line 864 "ntp_parser.y"
{ yyval.Integer = 0; }
    break;
case 202:
#line 868 "ntp_parser.y"
{ yyval.Double = (double)yyvsp[0].Integer; }
    break;
case 203:
#line 869 "ntp_parser.y"
{ yyval.Double = yyvsp[0].Double; }
    break;
case 204:
#line 878 "ntp_parser.y"
{
                 my_config.sim_details = create_sim_node(yyvsp[-2].Queue, yyvsp[-1].Queue);

                 /* Reset the old_config_style variable */
                 old_config_style = 1;
             }
    break;
case 205:
#line 892 "ntp_parser.y"
{ old_config_style = 0; }
    break;
case 206:
#line 896 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); }
    break;
case 207:
#line 897 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); }
    break;
case 208:
#line 901 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Beep_Delay, yyvsp[0].Double); }
    break;
case 209:
#line 902 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Sim_Duration, yyvsp[0].Double); }
    break;
case 210:
#line 906 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_server); }
    break;
case 211:
#line 907 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_server); }
    break;
case 212:
#line 912 "ntp_parser.y"
{ yyval.Sim_server = create_sim_server(yyvsp[-4].Address_node, yyvsp[-2].Double, yyvsp[-1].Queue); }
    break;
case 213:
#line 916 "ntp_parser.y"
{ yyval.Double = yyvsp[-1].Double; }
    break;
case 214:
#line 920 "ntp_parser.y"
{ yyval.Address_node = yyvsp[0].Address_node; }
    break;
case 215:
#line 924 "ntp_parser.y"
{ yyval.Address_node = yyvsp[0].Address_node; }
    break;
case 216:
#line 925 "ntp_parser.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, T_String); }
    break;
case 217:
#line 929 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_script); }
    break;
case 218:
#line 930 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_script); }
    break;
case 219:
#line 935 "ntp_parser.y"
{ yyval.Sim_script = create_sim_script_info(yyvsp[-3].Double, yyvsp[-1].Queue); }
    break;
case 220:
#line 939 "ntp_parser.y"
{ yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); }
    break;
case 221:
#line 940 "ntp_parser.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); }
    break;
case 222:
#line 945 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Freq_Offset, yyvsp[0].Double); }
    break;
case 223:
#line 947 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Wander, yyvsp[0].Double); }
    break;
case 224:
#line 949 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Jitter, yyvsp[0].Double); }
    break;
case 225:
#line 951 "ntp_parser.y"
{ yyval.Attr_val = create_attr_dval(T_Prop_Delay, yyvsp[0].Double); }
    break;
case 226:
#line 953 "ntp_parser.y"
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
#line 957 "ntp_parser.y"


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

