
/*  A Bison parser, made from ntp_config.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_Age	257
#define	T_Allan	258
#define	T_Auth	259
#define	T_Autokey	260
#define	T_Automax	261
#define	T_Average	262
#define	T_Bclient	263
#define	T_Beacon	264
#define	T_Broadcast	265
#define	T_Broadcastclient	266
#define	T_Broadcastdelay	267
#define	T_Burst	268
#define	T_Calibrate	269
#define	T_Calldelay	270
#define	T_Ceiling	271
#define	T_Cert	272
#define	T_Clockstats	273
#define	T_Cohort	274
#define	T_ControlKey	275
#define	T_Crypto	276
#define	T_Cryptostats	277
#define	T_Day	278
#define	T_Default	279
#define	T_Disable	280
#define	T_Discard	281
#define	T_Dispersion	282
#define	T_Double	283
#define	T_Driftfile	284
#define	T_Enable	285
#define	T_End	286
#define	T_False	287
#define	T_File	288
#define	T_Filegen	289
#define	T_Flag1	290
#define	T_Flag2	291
#define	T_Flag3	292
#define	T_Flag4	293
#define	T_Floor	294
#define	T_Freq	295
#define	T_Fudge	296
#define	T_Gqpar	297
#define	T_Host	298
#define	T_Huffpuff	299
#define	T_IPv4_address	300
#define	T_IPv6_address	301
#define	T_Iburst	302
#define	T_Ident	303
#define	T_Iffpar	304
#define	T_Ignore	305
#define	T_Includefile	306
#define	T_Integer	307
#define	T_Interface	308
#define	T_Kernel	309
#define	T_Key	310
#define	T_Keys	311
#define	T_Keysdir	312
#define	T_Kod	313
#define	T_Leap	314
#define	T_Limited	315
#define	T_Link	316
#define	T_Logconfig	317
#define	T_Logfile	318
#define	T_Loopstats	319
#define	T_Lowpriotrap	320
#define	T_Manycastclient	321
#define	T_Manycastserver	322
#define	T_Mask	323
#define	T_Maxclock	324
#define	T_Maxdist	325
#define	T_Maxhop	326
#define	T_Maxpoll	327
#define	T_Minclock	328
#define	T_Mindist	329
#define	T_Minimum	330
#define	T_Minpoll	331
#define	T_Minsane	332
#define	T_Mode	333
#define	T_Monitor	334
#define	T_Month	335
#define	T_Multicastclient	336
#define	T_Mvpar	337
#define	T_Nolink	338
#define	T_Nomodify	339
#define	T_None	340
#define	T_Nopeer	341
#define	T_Noquery	342
#define	T_Noselect	343
#define	T_Noserve	344
#define	T_Notrap	345
#define	T_Notrust	346
#define	T_Novolley	347
#define	T_Ntp	348
#define	T_Ntpport	349
#define	T_Orphan	350
#define	T_Panic	351
#define	T_Peer	352
#define	T_Peerstats	353
#define	T_Phone	354
#define	T_Pid	355
#define	T_Pidfile	356
#define	T_Pool	357
#define	T_Port	358
#define	T_Pps	359
#define	T_Preempt	360
#define	T_Prefer	361
#define	T_Pw	362
#define	T_RandFile	363
#define	T_Rawstats	364
#define	T_Refid	365
#define	T_Requestkey	366
#define	T_Restrict	367
#define	T_Revoke	368
#define	T_Server	369
#define	T_Setvar	370
#define	T_Sign	371
#define	T_Statistics	372
#define	T_Stats	373
#define	T_Statsdir	374
#define	T_Step	375
#define	T_Stepout	376
#define	T_Stratum	377
#define	T_String	378
#define	T_Sysstats	379
#define	T_Tick	380
#define	T_Time1	381
#define	T_Time2	382
#define	T_Tinker	383
#define	T_Tos	384
#define	T_Trap	385
#define	T_True	386
#define	T_Trustedkey	387
#define	T_Ttl	388
#define	T_Type	389
#define	T_Version	390
#define	T_Week	391
#define	T_Year	392
#define	T_Flag	393
#define	T_Void	394
#define	T_EOC	395
#define	T_Simulate	396
#define	T_Beep_Delay	397
#define	T_Sim_Duration	398
#define	T_Server_Offset	399
#define	T_Duration	400
#define	T_Freq_Offset	401
#define	T_Wander	402
#define	T_Jitter	403
#define	T_Prop_Delay	404
#define	T_Proc_Delay	405


#line 11 "ntp_config.y"
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
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		358
#define	YYFLAG		-32768
#define	YYNTBASE	157

#define YYTRANSLATE(x) ((unsigned)(x) <= 405 ? yytranslate[x] : 218)

static const short yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,   153,     2,   154,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
   152,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,   155,     2,   156,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
    87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
    97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
   117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
   127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
   147,   148,   149,   150,   151
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,     9,    12,    13,    15,    17,    19,    21,
    23,    25,    27,    29,    31,    33,    35,    39,    42,    44,
    46,    48,    50,    52,    54,    56,    59,    61,    63,    66,
    68,    70,    72,    74,    77,    80,    83,    85,    87,    89,
    91,    94,    97,   100,   102,   105,   108,   111,   114,   117,
   120,   123,   126,   129,   132,   135,   138,   140,   143,   146,
   149,   152,   155,   158,   161,   164,   167,   170,   173,   176,
   178,   181,   184,   187,   190,   193,   196,   199,   202,   205,
   208,   211,   214,   217,   221,   224,   226,   228,   230,   232,
   234,   236,   238,   241,   243,   246,   249,   251,   253,   255,
   257,   259,   261,   263,   265,   267,   269,   271,   274,   278,
   282,   288,   289,   292,   294,   296,   298,   300,   302,   304,
   306,   308,   310,   312,   314,   316,   319,   321,   324,   327,
   330,   334,   337,   339,   342,   345,   348,   351,   354,   357,
   360,   363,   366,   369,   372,   374,   376,   378,   380,   382,
   384,   386,   388,   391,   394,   396,   399,   402,   405,   408,
   411,   414,   417,   420,   422,   425,   428,   431,   434,   437,
   440,   443,   446,   449,   452,   456,   459,   464,   468,   471,
   473,   476,   479,   482,   484,   487,   489,   491,   493,   495,
   498,   500,   503,   505,   508,   510,   512,   514,   516,   518,
   520,   526,   528,   532,   535,   539,   543,   546,   548,   554,
   559,   563,   565,   567,   570,   572,   579,   583,   586,   590,
   594,   598,   602
};

static const short yyrhs[] = {   158,
     0,   158,   159,   141,     0,   159,   141,     0,     1,   141,
     0,     0,   160,     0,   166,     0,   167,     0,   173,     0,
   179,     0,   170,     0,   184,     0,   187,     0,   190,     0,
   193,     0,   205,     0,   161,   162,   164,     0,   161,   162,
     0,   115,     0,   103,     0,    98,     0,    11,     0,    67,
     0,   163,     0,   124,     0,    53,   124,     0,    46,     0,
    47,     0,   164,   165,     0,   165,     0,     6,     0,    14,
     0,    48,     0,    56,    53,     0,    77,    53,     0,    73,
    53,     0,    89,     0,   106,     0,   107,     0,   132,     0,
   134,    53,     0,    79,    53,     0,   136,    53,     0,    12,
     0,    12,    93,     0,    68,   202,     0,    82,   202,     0,
     6,    53,     0,    21,    53,     0,    22,   168,     0,    57,
   124,     0,    58,   124,     0,   112,    53,     0,   114,    53,
     0,   133,   200,     0,   168,   169,     0,   169,     0,    18,
   124,     0,    60,   124,     0,   109,   124,     0,    44,   124,
     0,   117,   124,     0,    49,   124,     0,    50,   124,     0,
    43,   124,     0,    83,   124,     0,   108,   124,     0,   130,
   171,     0,   171,   172,     0,   172,     0,    17,    53,     0,
    40,    53,     0,    20,   203,     0,    96,    53,     0,    75,
   204,     0,    71,   204,     0,    74,   204,     0,    70,   204,
     0,    78,    53,     0,    10,    53,     0,    72,    53,     0,
   118,   174,     0,   120,   124,     0,    35,   175,   176,     0,
   174,   175,     0,   175,     0,    19,     0,    23,     0,    65,
     0,    99,     0,   110,     0,   125,     0,   176,   177,     0,
   177,     0,    34,   124,     0,   135,   178,     0,    62,     0,
    84,     0,    31,     0,    26,     0,    86,     0,   101,     0,
    24,     0,   137,     0,    81,     0,   138,     0,     3,     0,
    27,   182,     0,   113,   162,   180,     0,   113,    25,   180,
     0,   113,   163,    69,   163,   180,     0,     0,   180,   181,
     0,    51,     0,    59,     0,    61,     0,    66,     0,    85,
     0,    87,     0,    88,     0,    90,     0,    91,     0,    92,
     0,    95,     0,   136,     0,   182,   183,     0,   183,     0,
     8,    53,     0,    76,    53,     0,    80,    53,     0,    42,
   162,   185,     0,   185,   186,     0,   186,     0,   127,   204,
     0,   128,   204,     0,   123,    53,     0,   111,   124,     0,
    36,   203,     0,    37,   203,     0,    38,   203,     0,    39,
   203,     0,    31,   188,     0,    26,   188,     0,   188,   189,
     0,   189,     0,     5,     0,     9,     0,    15,     0,    55,
     0,    80,     0,    94,     0,   119,     0,   129,   191,     0,
   191,   192,     0,   192,     0,     4,   204,     0,    28,   204,
     0,    41,   204,     0,    45,   204,     0,    97,   204,     0,
   121,   204,     0,   122,   204,     0,    52,   124,     0,    32,
     0,    13,   204,     0,    16,    53,     0,   126,   204,     0,
    30,   124,     0,   102,   124,     0,    64,   124,     0,     7,
    53,     0,    63,   197,     0,   100,   201,     0,   116,   194,
     0,   131,   163,   195,     0,   134,   200,     0,   124,   152,
   124,    25,     0,   124,   152,   124,     0,   195,   196,     0,
   196,     0,   104,    53,     0,    54,   163,     0,   197,   198,
     0,   198,     0,   199,   124,     0,   124,     0,   153,     0,
   154,     0,   152,     0,   200,    53,     0,    53,     0,   201,
   124,     0,   124,     0,   202,   162,     0,   162,     0,    53,
     0,   132,     0,    33,     0,    53,     0,    29,     0,   206,
   155,   207,   209,   156,     0,   142,     0,   207,   208,   141,
     0,   208,   141,     0,   143,   152,   204,     0,   144,   152,
   204,     0,   209,   210,     0,   210,     0,   212,   155,   211,
   214,   156,     0,   145,   152,   204,   141,     0,   115,   152,
   213,     0,   163,     0,   124,     0,   214,   215,     0,   215,
     0,   146,   152,   204,   155,   216,   156,     0,   216,   217,
   141,     0,   217,   141,     0,   147,   152,   204,     0,   148,
   152,   204,     0,   149,   152,   204,     0,   150,   152,   204,
     0,   151,   152,   204,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   238,   242,   243,   248,   256,   257,   258,   259,   260,   261,
   262,   263,   264,   265,   266,   267,   275,   281,   290,   291,
   292,   293,   294,   298,   299,   300,   316,   317,   321,   322,
   327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
   337,   338,   339,   349,   351,   353,   355,   366,   368,   370,
   372,   374,   376,   378,   380,   385,   386,   390,   392,   394,
   396,   398,   400,   402,   404,   406,   408,   418,   423,   424,
   428,   430,   432,   434,   436,   438,   440,   442,   444,   446,
   448,   458,   460,   462,   470,   471,   475,   477,   479,   481,
   483,   485,   490,   491,   495,   496,   497,   498,   499,   500,
   504,   505,   506,   507,   508,   509,   510,   519,   521,   526,
   531,   539,   540,   544,   545,   546,   547,   548,   549,   550,
   551,   552,   553,   554,   555,   559,   560,   564,   565,   566,
   574,   579,   580,   584,   586,   588,   590,   592,   594,   596,
   598,   607,   609,   614,   615,   619,   620,   621,   622,   623,
   624,   626,   634,   638,   639,   643,   644,   645,   646,   647,
   648,   649,   657,   673,   679,   681,   683,   685,   687,   689,
   691,   694,   696,   698,   700,   702,   707,   709,   715,   716,
   720,   721,   726,   727,   731,   732,   749,   750,   751,   760,
   761,   765,   766,   770,   771,   775,   784,   785,   789,   790,
   798,   813,   817,   818,   822,   823,   827,   828,   832,   837,
   841,   845,   846,   850,   851,   855,   860,   861,   865,   867,
   869,   871,   873
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_Age",
"T_Allan","T_Auth","T_Autokey","T_Automax","T_Average","T_Bclient","T_Beacon",
"T_Broadcast","T_Broadcastclient","T_Broadcastdelay","T_Burst","T_Calibrate",
"T_Calldelay","T_Ceiling","T_Cert","T_Clockstats","T_Cohort","T_ControlKey",
"T_Crypto","T_Cryptostats","T_Day","T_Default","T_Disable","T_Discard","T_Dispersion",
"T_Double","T_Driftfile","T_Enable","T_End","T_False","T_File","T_Filegen","T_Flag1",
"T_Flag2","T_Flag3","T_Flag4","T_Floor","T_Freq","T_Fudge","T_Gqpar","T_Host",
"T_Huffpuff","T_IPv4_address","T_IPv6_address","T_Iburst","T_Ident","T_Iffpar",
"T_Ignore","T_Includefile","T_Integer","T_Interface","T_Kernel","T_Key","T_Keys",
"T_Keysdir","T_Kod","T_Leap","T_Limited","T_Link","T_Logconfig","T_Logfile",
"T_Loopstats","T_Lowpriotrap","T_Manycastclient","T_Manycastserver","T_Mask",
"T_Maxclock","T_Maxdist","T_Maxhop","T_Maxpoll","T_Minclock","T_Mindist","T_Minimum",
"T_Minpoll","T_Minsane","T_Mode","T_Monitor","T_Month","T_Multicastclient","T_Mvpar",
"T_Nolink","T_Nomodify","T_None","T_Nopeer","T_Noquery","T_Noselect","T_Noserve",
"T_Notrap","T_Notrust","T_Novolley","T_Ntp","T_Ntpport","T_Orphan","T_Panic",
"T_Peer","T_Peerstats","T_Phone","T_Pid","T_Pidfile","T_Pool","T_Port","T_Pps",
"T_Preempt","T_Prefer","T_Pw","T_RandFile","T_Rawstats","T_Refid","T_Requestkey",
"T_Restrict","T_Revoke","T_Server","T_Setvar","T_Sign","T_Statistics","T_Stats",
"T_Statsdir","T_Step","T_Stepout","T_Stratum","T_String","T_Sysstats","T_Tick",
"T_Time1","T_Time2","T_Tinker","T_Tos","T_Trap","T_True","T_Trustedkey","T_Ttl",
"T_Type","T_Version","T_Week","T_Year","T_Flag","T_Void","T_EOC","T_Simulate",
"T_Beep_Delay","T_Sim_Duration","T_Server_Offset","T_Duration","T_Freq_Offset",
"T_Wander","T_Jitter","T_Prop_Delay","T_Proc_Delay","'='","'+'","'-'","'{'",
"'}'","configuration","command_list","command","server_command","client_type",
"address","ip_address","option_list","option","other_mode_command","authentication_command",
"crypto_command_list","crypto_command","orphan_mode_command","tos_option_list",
"tos_option","monitoring_command","stats_list","stat","filegen_option_list",
"filegen_option","filegen_type","access_control_command","ac_flag_list","access_control_flag",
"discard_option_list","discard_option","fudge_command","fudge_factor_list","fudge_factor",
"system_option_command","system_option_list","system_option","tinker_command",
"tinker_option_list","tinker_option","miscellaneous_command","variable_assign",
"trap_option_list","trap_option","log_config_list","log_config_command","log_config_prefix",
"integer_list","string_list","address_list","boolean","number","simulate_command",
"sim_conf_start","sim_init_statement_list","sim_init_statement","sim_server_list",
"sim_server","sim_server_offset","sim_server_name","sim_address","sim_act_list",
"sim_act","sim_act_stmt_list","sim_act_stmt", NULL
};
#endif

static const short yyr1[] = {     0,
   157,   158,   158,   158,   159,   159,   159,   159,   159,   159,
   159,   159,   159,   159,   159,   159,   160,   160,   161,   161,
   161,   161,   161,   162,   162,   162,   163,   163,   164,   164,
   165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
   165,   165,   165,   166,   166,   166,   166,   167,   167,   167,
   167,   167,   167,   167,   167,   168,   168,   169,   169,   169,
   169,   169,   169,   169,   169,   169,   169,   170,   171,   171,
   172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
   172,   173,   173,   173,   174,   174,   175,   175,   175,   175,
   175,   175,   176,   176,   177,   177,   177,   177,   177,   177,
   178,   178,   178,   178,   178,   178,   178,   179,   179,   179,
   179,   180,   180,   181,   181,   181,   181,   181,   181,   181,
   181,   181,   181,   181,   181,   182,   182,   183,   183,   183,
   184,   185,   185,   186,   186,   186,   186,   186,   186,   186,
   186,   187,   187,   188,   188,   189,   189,   189,   189,   189,
   189,   189,   190,   191,   191,   192,   192,   192,   192,   192,
   192,   192,   193,   193,   193,   193,   193,   193,   193,   193,
   193,   193,   193,   193,   193,   193,   194,   194,   195,   195,
   196,   196,   197,   197,   198,   198,   199,   199,   199,   200,
   200,   201,   201,   202,   202,   203,   203,   203,   204,   204,
   205,   206,   207,   207,   208,   208,   209,   209,   210,   211,
   212,   213,   213,   214,   214,   215,   216,   216,   217,   217,
   217,   217,   217
};

static const short yyr2[] = {     0,
     1,     3,     2,     2,     0,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     3,     2,     1,     1,
     1,     1,     1,     1,     1,     2,     1,     1,     2,     1,
     1,     1,     1,     2,     2,     2,     1,     1,     1,     1,
     2,     2,     2,     1,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     1,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     1,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     3,     2,     1,     1,     1,     1,     1,
     1,     1,     2,     1,     2,     2,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     2,     3,     3,
     5,     0,     2,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     1,     2,     2,     2,
     3,     2,     1,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     2,     1,     2,     2,     2,     2,     2,
     2,     2,     2,     1,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     3,     2,     4,     3,     2,     1,
     2,     2,     2,     1,     2,     1,     1,     1,     1,     2,
     1,     2,     1,     2,     1,     1,     1,     1,     1,     1,
     5,     1,     3,     2,     3,     3,     2,     1,     5,     4,
     3,     1,     1,     2,     1,     6,     3,     2,     3,     3,
     3,     3,     3
};

static const short yydefact[] = {     0,
     0,     0,     0,    22,    44,     0,     0,     0,     0,     0,
     0,     0,     0,   164,     0,     0,     0,     0,     0,     0,
     0,    23,     0,     0,    21,     0,     0,    20,     0,     0,
     0,    19,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   202,     1,     0,     6,     0,     7,     8,    11,     9,
    10,    12,    13,    14,    15,    16,     0,     4,    48,   171,
    45,   200,   199,   165,   166,    49,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    50,    57,   146,   147,
   148,   149,   150,   151,   152,   143,   145,     0,     0,     0,
   108,   127,   168,   142,    87,    88,    89,    90,    91,    92,
     0,    27,    28,     0,    25,     0,    24,   163,    51,    52,
   186,   189,   187,   188,   172,   184,     0,   170,   195,    46,
    47,   193,   173,   169,    53,   112,   112,    24,    54,     0,
   174,    82,    86,    83,   167,     0,     0,     0,     0,     0,
     0,     0,   153,   155,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    68,    70,     0,   191,    55,
   176,     0,     3,    18,     0,    58,    65,    61,    63,    64,
    59,    66,    67,    60,    62,    56,   144,   128,   129,   130,
   126,   100,    99,     0,    97,    98,     0,    84,    94,    26,
     0,     0,     0,     0,     0,     0,     0,     0,   131,   133,
   183,   185,   194,   192,   110,   109,     0,     0,    85,   156,
   157,   158,   159,   160,   161,   162,   154,    80,    71,   198,
   196,   197,    73,    72,    78,    76,    81,    77,    75,    79,
    74,    69,     0,     0,   175,   180,   190,     2,    31,    32,
    33,     0,     0,     0,     0,    37,    38,    39,    40,     0,
     0,    17,    30,     0,     0,     0,     0,    95,   107,   103,
   105,   101,   102,   104,   106,    96,    93,   138,   139,   140,
   141,   137,   136,   134,   135,   132,   114,   115,   116,   117,
   118,   119,   120,   121,   122,   123,   124,   125,   113,   112,
   178,   182,   181,   179,    34,    36,    35,    42,    41,    43,
    29,     0,     0,     0,     0,     0,   208,     0,   204,   111,
   177,   205,   206,     0,   203,   201,   207,     0,   213,   212,
   211,     0,     0,     0,     0,     0,   215,     0,     0,   209,
   214,   210,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   216,     0,   218,   219,
   220,   221,   222,   223,   217,     0,     0,     0
};

static const short yydefgoto[] = {   356,
    43,    44,    45,    46,   119,   107,   252,   253,    47,    48,
    77,    78,    49,   156,   157,    50,   132,   101,   188,   189,
   266,    51,   205,   289,    91,    92,    52,   199,   200,    53,
    86,    87,    54,   143,   144,    55,   131,   235,   236,   115,
   116,   117,   160,   123,   120,   223,    64,    56,    57,   256,
   257,   306,   307,   323,   308,   321,   326,   327,   340,   341
};

static const short yypact[] = {    25,
  -136,   -35,   -18,-32768,   -50,   -14,    12,    15,   229,    14,
    -4,   -54,    14,-32768,   125,   -25,   -53,   -51,   -46,    28,
   -43,-32768,   -25,   -25,-32768,   -40,   -37,-32768,    21,    33,
    38,-32768,   -27,   125,   -26,   -14,    -1,   280,     2,    56,
    56,-32768,   201,   -22,-32768,   -25,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   -33,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   -12,     0,     6,     7,
     8,    10,    11,    18,    39,    40,   229,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    14,-32768,    83,   100,   116,
    -4,-32768,-32768,    14,-32768,-32768,-32768,-32768,-32768,-32768,
   -20,-32768,-32768,    55,-32768,   134,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    28,-32768,    59,-32768,-32768,   -25,
   -25,-32768,    60,-32768,-32768,-32768,-32768,   117,-32768,    35,
-32768,   125,-32768,-32768,-32768,   -14,   -14,   -14,   -14,   -14,
   -14,   -14,    -1,-32768,   135,   139,    -3,   145,   -14,   -14,
   146,   -14,   -14,   147,   150,   280,-32768,   -38,-32768,   152,
   152,    65,-32768,   112,   -81,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,    85,-32768,-32768,     9,   -20,-32768,-32768,
    -3,    -3,    -3,    -3,    86,   158,   -14,   -14,   134,-32768,
-32768,-32768,-32768,-32768,   298,   298,     2,    91,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,     2,   163,   -38,-32768,-32768,-32768,-32768,-32768,
-32768,   167,   168,   172,   173,-32768,-32768,-32768,-32768,   176,
   177,   112,-32768,    82,    97,   -90,   101,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   226,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   -14,   -14,   102,   111,   -95,-32768,   105,-32768,   298,
-32768,-32768,-32768,   -39,-32768,-32768,-32768,   110,-32768,-32768,
-32768,   104,   124,   -14,   119,  -122,-32768,   122,   -14,-32768,
-32768,-32768,   120,    90,   128,   129,   130,   132,   140,    46,
   133,   -14,   -14,   -14,   -14,   -14,-32768,   136,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   276,   286,-32768
};

static const short yypgoto[] = {-32768,
-32768,   244,-32768,-32768,    29,   -29,-32768,    42,-32768,-32768,
-32768,   214,-32768,-32768,   142,-32768,-32768,   -21,-32768,   107,
-32768,-32768,  -125,-32768,-32768,   205,-32768,-32768,   103,-32768,
   292,   -77,-32768,-32768,   175,-32768,-32768,-32768,    76,-32768,
   207,-32768,   282,-32768,   300,   -17,   -36,-32768,-32768,-32768,
    69,-32768,    20,-32768,-32768,-32768,-32768,     3,-32768,    -7
};


#define	YYLAST		434


static const short yytable[] = {   135,
   128,   206,   136,    88,    58,   182,   102,   103,   177,   158,
   183,   259,   133,   184,    62,   233,   177,    59,    79,   304,
   102,   103,    80,   325,   304,     1,   137,   104,    81,   220,
     2,     3,   260,   330,    60,     4,     5,     6,    63,   138,
     7,   185,    61,   139,   106,     8,     9,   102,   103,   221,
    10,    11,   254,   255,    12,    13,    14,   126,   127,    15,
   316,   254,   255,   186,    65,   234,    16,    66,    82,    93,
   108,    89,   109,   125,   164,    90,    17,   110,   102,   103,
   118,    18,    19,   122,   319,   104,   124,    20,    21,   261,
   129,    22,    23,    83,   262,   140,   130,   134,   105,   210,
   211,   212,   213,   214,   215,   216,    24,    84,   159,   263,
   209,   166,   225,   226,   187,   228,   229,   239,   163,   141,
   142,   165,    25,   167,    26,   240,    27,    28,   222,   168,
   169,   170,    85,   171,   172,   178,    29,    30,    31,    32,
    33,   173,    34,    95,    35,   264,   265,    96,   203,   203,
    36,   111,   179,    37,    38,    39,   105,    40,    41,   241,
   274,   275,   174,   175,   310,    -5,    42,   242,   180,   191,
   192,   193,   194,   268,   269,   270,   271,   290,   190,   112,
   113,   114,   202,   204,   243,   207,   208,   218,   244,    97,
   245,   219,   335,   336,   337,   338,   339,   224,   227,   230,
   246,   347,   231,   292,   237,   238,     2,     3,   258,   272,
   273,     4,     5,     6,   291,   293,     7,   247,   248,   295,
   296,     8,     9,    98,   297,   298,    10,    11,   299,   300,
    12,    13,    14,   302,    99,    15,   335,   336,   337,   338,
   339,   309,    16,   249,   195,   250,    67,   251,   303,   100,
   311,   315,    17,   314,   322,   324,   196,    18,    19,   318,
   197,   198,   332,    20,    21,   312,   313,    22,    23,   325,
   329,    68,    69,   349,   334,   357,   355,    70,    71,   342,
   343,   344,    24,   345,   320,   358,   162,   328,    72,   145,
   176,   346,   333,   301,   267,   181,   146,   232,    25,   147,
    26,   276,    27,    28,    94,   350,   351,   352,   353,   354,
   294,    73,    29,    30,    31,    32,    33,   217,    34,   148,
    35,   201,   161,   121,   305,   317,    36,     0,   331,    37,
    38,    39,   348,    40,    41,     0,    74,    75,     0,     0,
     0,    -5,    42,     0,     0,    76,     0,     0,   277,   149,
   150,   151,     0,   152,   153,     0,   278,   154,   279,     0,
     0,     0,     0,   280,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   155,     0,     0,     0,     0,
     0,     0,   281,     0,   282,   283,     0,   284,   285,   286,
     0,     0,   287,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   288
};

static const short yycheck[] = {    36,
    30,   127,     4,     8,   141,    26,    46,    47,    86,    39,
    31,     3,    34,    34,    29,    54,    94,    53,     5,   115,
    46,    47,     9,   146,   115,     1,    28,    53,    15,    33,
     6,     7,    24,   156,    53,    11,    12,    13,    53,    41,
    16,    62,    93,    45,    16,    21,    22,    46,    47,    53,
    26,    27,   143,   144,    30,    31,    32,    25,    30,    35,
   156,   143,   144,    84,    53,   104,    42,    53,    55,   124,
   124,    76,   124,    53,    46,    80,    52,   124,    46,    47,
   124,    57,    58,   124,   124,    53,   124,    63,    64,    81,
    53,    67,    68,    80,    86,    97,   124,   124,   124,   136,
   137,   138,   139,   140,   141,   142,    82,    94,    53,   101,
   132,   124,   149,   150,   135,   152,   153,     6,   141,   121,
   122,   155,    98,   124,   100,    14,   102,   103,   132,   124,
   124,   124,   119,   124,   124,    53,   112,   113,   114,   115,
   116,   124,   118,    19,   120,   137,   138,    23,   120,   121,
   126,   124,    53,   129,   130,   131,   124,   133,   134,    48,
   197,   198,   124,   124,   290,   141,   142,    56,    53,    36,
    37,    38,    39,   191,   192,   193,   194,   207,   124,   152,
   153,   154,   124,   124,    73,    69,   152,    53,    77,    65,
    79,    53,   147,   148,   149,   150,   151,    53,    53,    53,
    89,   156,    53,   233,    53,   141,     6,     7,   124,   124,
    53,    11,    12,    13,   124,    53,    16,   106,   107,    53,
    53,    21,    22,    99,    53,    53,    26,    27,    53,    53,
    30,    31,    32,   152,   110,    35,   147,   148,   149,   150,
   151,   141,    42,   132,   111,   134,    18,   136,   152,   125,
    25,   141,    52,   152,   145,   152,   123,    57,    58,   155,
   127,   128,   141,    63,    64,   302,   303,    67,    68,   146,
   152,    43,    44,   141,   155,     0,   141,    49,    50,   152,
   152,   152,    82,   152,   314,     0,    43,   324,    60,    10,
    77,   152,   329,   252,   188,    91,    17,   156,    98,    20,
   100,   199,   102,   103,    13,   342,   343,   344,   345,   346,
   235,    83,   112,   113,   114,   115,   116,   143,   118,    40,
   120,   115,    41,    24,   256,   306,   126,    -1,   326,   129,
   130,   131,   340,   133,   134,    -1,   108,   109,    -1,    -1,
    -1,   141,   142,    -1,    -1,   117,    -1,    -1,    51,    70,
    71,    72,    -1,    74,    75,    -1,    59,    78,    61,    -1,
    -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    96,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    -1,    87,    88,    -1,    90,    91,    92,
    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   136
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
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
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
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

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 4:
#line 249 "ntp_config.y"
{ 
                    fprintf(stderr, "PARSE ERROR!! At Line: %d\n", 
                            ip_file->line_no);
                    fprintf(stderr, "Line Ignored!!\n");
                ;
    break;}
case 17:
#line 276 "ntp_config.y"
{ 
                        struct peer_node *my_node =  create_peer_node(yyvsp[-2].Integer, yyvsp[-1].Address_node, yyvsp[0].Queue); 
                        if (my_node)
                            enqueue(my_config.peers, my_node);                        
                    ;
    break;}
case 18:
#line 282 "ntp_config.y"
{ 
                        struct peer_node *my_node = create_peer_node(yyvsp[-1].Integer, yyvsp[0].Address_node, NULL); 
                        if (my_node)
                            enqueue(my_config.peers, my_node);                        
                    ;
    break;}
case 19:
#line 290 "ntp_config.y"
{ yyval.Integer = T_Server; ;
    break;}
case 20:
#line 291 "ntp_config.y"
{ yyval.Integer = T_Pool; ;
    break;}
case 21:
#line 292 "ntp_config.y"
{ yyval.Integer = T_Peer; ;
    break;}
case 22:
#line 293 "ntp_config.y"
{ yyval.Integer = T_Broadcast; ;
    break;}
case 23:
#line 294 "ntp_config.y"
{ yyval.Integer = T_Manycastclient; ;
    break;}
case 24:
#line 298 "ntp_config.y"
{ yyval.Address_node = yyvsp[0].Address_node; ;
    break;}
case 25:
#line 299 "ntp_config.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, default_ai_family); ;
    break;}
case 26:
#line 301 "ntp_config.y"
{
                        if (yyvsp[-1].Integer == -4)
                            yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET);
                        else if (yyvsp[-1].Integer == -6)
                            yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET6);
                        else {
                            yyerror("Invalid address type specified. Assuming default.\n");
                            yyval.Address_node = create_address_node(yyvsp[0].String, default_ai_family);
                        }
                    ;
    break;}
case 27:
#line 316 "ntp_config.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET); ;
    break;}
case 28:
#line 317 "ntp_config.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, AF_INET6); ;
    break;}
case 29:
#line 321 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); ;
    break;}
case 30:
#line 322 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); ;
    break;}
case 31:
#line 327 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_SKEY | FLAG_AUTHENABLE); ;
    break;}
case 32:
#line 328 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_BURST); ;
    break;}
case 33:
#line 329 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_IBURST); ;
    break;}
case 34:
#line 330 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Key, yyvsp[0].Integer); ;
    break;}
case 35:
#line 331 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Minpoll, yyvsp[0].Integer); ;
    break;}
case 36:
#line 332 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Maxpoll, yyvsp[0].Integer); ;
    break;}
case 37:
#line 333 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_NOSELECT);;
    break;}
case 38:
#line 334 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREEMPT); ;
    break;}
case 39:
#line 335 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_PREFER); ;
    break;}
case 40:
#line 336 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, FLAG_TRUE); ;
    break;}
case 41:
#line 337 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Ttl, yyvsp[0].Integer); ;
    break;}
case 42:
#line 338 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Mode, yyvsp[0].Integer); ;
    break;}
case 43:
#line 339 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Version, yyvsp[0].Integer); ;
    break;}
case 44:
#line 350 "ntp_config.y"
{ my_config.broadcastclient = SIMPLE; ;
    break;}
case 45:
#line 352 "ntp_config.y"
{ my_config.broadcastclient = NOVOLLEY;  ;
    break;}
case 46:
#line 354 "ntp_config.y"
{ append_queue(my_config.manycastserver, yyvsp[0].Queue);  ;
    break;}
case 47:
#line 356 "ntp_config.y"
{ append_queue(my_config.multicastclient, yyvsp[0].Queue);  ;
    break;}
case 48:
#line 367 "ntp_config.y"
{ my_config.auth.autokey = yyvsp[0].Integer;  ;
    break;}
case 49:
#line 369 "ntp_config.y"
{ my_config.auth.control_key = yyvsp[0].Integer;  ;
    break;}
case 50:
#line 371 "ntp_config.y"
{ my_config.auth.crypto_cmd_list = yyvsp[0].Queue;  ;
    break;}
case 51:
#line 373 "ntp_config.y"
{ my_config.auth.keys = yyvsp[0].String;  ;
    break;}
case 52:
#line 375 "ntp_config.y"
{ my_config.auth.keysdir = yyvsp[0].String;  ;
    break;}
case 53:
#line 377 "ntp_config.y"
{ my_config.auth.requested_key = yyvsp[0].Integer;  ;
    break;}
case 54:
#line 379 "ntp_config.y"
{ my_config.auth.revoke = yyvsp[0].Integer;  ;
    break;}
case 55:
#line 381 "ntp_config.y"
{ my_config.auth.trusted_key_list = yyvsp[0].Queue;  ;
    break;}
case 56:
#line 385 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); ;
    break;}
case 57:
#line 386 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); ;
    break;}
case 58:
#line 391 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_CERT, yyvsp[0].String); ;
    break;}
case 59:
#line 393 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_LEAP, yyvsp[0].String); ;
    break;}
case 60:
#line 395 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_RAND, yyvsp[0].String); ;
    break;}
case 61:
#line 397 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PRIV, yyvsp[0].String); ;
    break;}
case 62:
#line 399 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_SIGN, yyvsp[0].String); ;
    break;}
case 63:
#line 401 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_IDENT, yyvsp[0].String); ;
    break;}
case 64:
#line 403 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_IFFPAR, yyvsp[0].String); ;
    break;}
case 65:
#line 405 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_GQPAR, yyvsp[0].String); ;
    break;}
case 66:
#line 407 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_MVPAR, yyvsp[0].String); ;
    break;}
case 67:
#line 409 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(CRYPTO_CONF_PW, yyvsp[0].String); ;
    break;}
case 68:
#line 419 "ntp_config.y"
{ append_queue(my_config.orphan_cmds,yyvsp[0].Queue);  ;
    break;}
case 69:
#line 423 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); ;
    break;}
case 70:
#line 424 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); ;
    break;}
case 71:
#line 429 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(PROTO_CEILING, (double)yyvsp[0].Integer); ;
    break;}
case 72:
#line 431 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(PROTO_FLOOR, (double)yyvsp[0].Integer); ;
    break;}
case 73:
#line 433 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(PROTO_COHORT, (double)yyvsp[0].Integer); ;
    break;}
case 74:
#line 435 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(PROTO_ORPHAN, (double)yyvsp[0].Integer); ;
    break;}
case 75:
#line 437 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINDISP, yyvsp[0].Double); ;
    break;}
case 76:
#line 439 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXDIST, yyvsp[0].Double); ;
    break;}
case 77:
#line 441 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINCLOCK, yyvsp[0].Double); ;
    break;}
case 78:
#line 443 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXCLOCK, yyvsp[0].Double); ;
    break;}
case 79:
#line 445 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MINSANE, (double)yyvsp[0].Integer); ;
    break;}
case 80:
#line 447 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(PROTO_BEACON, (double)yyvsp[0].Integer); ;
    break;}
case 81:
#line 449 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(PROTO_MAXHOP, (double)yyvsp[0].Integer); ;
    break;}
case 82:
#line 459 "ntp_config.y"
{ append_queue(my_config.stats_list, yyvsp[0].Queue);  ;
    break;}
case 83:
#line 461 "ntp_config.y"
{ my_config.stats_dir = yyvsp[0].String;  ;
    break;}
case 84:
#line 463 "ntp_config.y"
{
                        enqueue(my_config.filegen_opts, 
                                create_filegen_node(yyvsp[-1].VoidPtr, yyvsp[0].Queue));                         
                    ;
    break;}
case 85:
#line 470 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); ;
    break;}
case 86:
#line 471 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].VoidPtr); ;
    break;}
case 87:
#line 476 "ntp_config.y"
{ yyval.VoidPtr = create_pval("clockstats"); ;
    break;}
case 88:
#line 478 "ntp_config.y"
{ yyval.VoidPtr = create_pval("cryptostats"); ;
    break;}
case 89:
#line 480 "ntp_config.y"
{ yyval.VoidPtr = create_pval("loopstats"); ;
    break;}
case 90:
#line 482 "ntp_config.y"
{ yyval.VoidPtr = create_pval("peerstats"); ;
    break;}
case 91:
#line 484 "ntp_config.y"
{ yyval.VoidPtr = create_pval("rawstats"); ;
    break;}
case 92:
#line 486 "ntp_config.y"
{ yyval.VoidPtr = create_pval("sysstats"); ;
    break;}
case 93:
#line 490 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); ;
    break;}
case 94:
#line 491 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); ;
    break;}
case 95:
#line 495 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(T_File, yyvsp[0].String); ;
    break;}
case 96:
#line 496 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Type, yyvsp[0].Integer); ;
    break;}
case 97:
#line 497 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Link); ;
    break;}
case 98:
#line 498 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Nolink); ;
    break;}
case 99:
#line 499 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Enable); ;
    break;}
case 100:
#line 500 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, T_Disable); ;
    break;}
case 101:
#line 504 "ntp_config.y"
{ yyval.Integer = FILEGEN_NONE; ;
    break;}
case 102:
#line 505 "ntp_config.y"
{ yyval.Integer = FILEGEN_PID; ;
    break;}
case 103:
#line 506 "ntp_config.y"
{ yyval.Integer = FILEGEN_DAY; ;
    break;}
case 104:
#line 507 "ntp_config.y"
{ yyval.Integer = FILEGEN_WEEK; ;
    break;}
case 105:
#line 508 "ntp_config.y"
{ yyval.Integer = FILEGEN_MONTH; ;
    break;}
case 106:
#line 509 "ntp_config.y"
{ yyval.Integer = FILEGEN_YEAR; ;
    break;}
case 107:
#line 510 "ntp_config.y"
{ yyval.Integer = FILEGEN_AGE; ;
    break;}
case 108:
#line 520 "ntp_config.y"
{   append_queue(my_config.discard_opts, yyvsp[0].Queue); ;
    break;}
case 109:
#line 522 "ntp_config.y"
{ 
                        enqueue(my_config.restrict_opts, 
                                create_restrict_node(yyvsp[-1].Address_node, NULL, yyvsp[0].Queue, ip_file->line_no));                          
                    ;
    break;}
case 110:
#line 527 "ntp_config.y"
{ 
                        enqueue(my_config.restrict_opts, 
                                create_restrict_node(NULL, NULL, yyvsp[0].Queue, ip_file->line_no)); 
                    ;
    break;}
case 111:
#line 532 "ntp_config.y"
{ 
                        enqueue(my_config.restrict_opts, 
                                create_restrict_node(yyvsp[-3].Address_node, yyvsp[-1].Address_node, yyvsp[0].Queue, ip_file->line_no)); 
                    ;
    break;}
case 112:
#line 539 "ntp_config.y"
{ yyval.Queue = create_queue(); ;
    break;}
case 113:
#line 540 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].VoidPtr); ;
    break;}
case 114:
#line 544 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RES_IGNORE); ;
    break;}
case 115:
#line 545 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RES_DEMOBILIZE); ;
    break;}
case 116:
#line 546 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RES_LIMITED); ;
    break;}
case 117:
#line 547 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RES_LPTRAP); ;
    break;}
case 118:
#line 548 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RES_NOMODIFY); ;
    break;}
case 119:
#line 549 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RES_NOPEER); ;
    break;}
case 120:
#line 550 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RES_NOQUERY); ;
    break;}
case 121:
#line 551 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RES_DONTSERVE); ;
    break;}
case 122:
#line 552 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RES_NOTRAP); ;
    break;}
case 123:
#line 553 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RES_DONTTRUST); ;
    break;}
case 124:
#line 554 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RESM_NTPONLY); ;
    break;}
case 125:
#line 555 "ntp_config.y"
{ yyval.VoidPtr = create_ival(RES_VERSION); ;
    break;}
case 126:
#line 559 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); ;
    break;}
case 127:
#line 560 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); ;
    break;}
case 128:
#line 564 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Average, yyvsp[0].Integer); ;
    break;}
case 129:
#line 565 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Minimum, yyvsp[0].Integer); ;
    break;}
case 130:
#line 566 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Monitor, yyvsp[0].Integer); ;
    break;}
case 131:
#line 575 "ntp_config.y"
{ enqueue(my_config.fudge, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  ;
    break;}
case 132:
#line 579 "ntp_config.y"
{ enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); ;
    break;}
case 133:
#line 580 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); ;
    break;}
case 134:
#line 585 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(CLK_HAVETIME1, yyvsp[0].Double); ;
    break;}
case 135:
#line 587 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(CLK_HAVETIME2, yyvsp[0].Double); ;
    break;}
case 136:
#line 589 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEVAL1,  yyvsp[0].Integer); ;
    break;}
case 137:
#line 591 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(CLK_HAVEVAL2,  yyvsp[0].String); ;
    break;}
case 138:
#line 593 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG1, yyvsp[0].Integer); ;
    break;}
case 139:
#line 595 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG2, yyvsp[0].Integer); ;
    break;}
case 140:
#line 597 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG3, yyvsp[0].Integer); ;
    break;}
case 141:
#line 599 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(CLK_HAVEFLAG4, yyvsp[0].Integer); ;
    break;}
case 142:
#line 608 "ntp_config.y"
{ append_queue(my_config.enable_opts,yyvsp[0].Queue);  ;
    break;}
case 143:
#line 610 "ntp_config.y"
{ append_queue(my_config.disable_opts,yyvsp[0].Queue);  ;
    break;}
case 144:
#line 614 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); ;
    break;}
case 145:
#line 615 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); ;
    break;}
case 146:
#line 619 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); ;
    break;}
case 147:
#line 620 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_BROADCLIENT); ;
    break;}
case 148:
#line 621 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_CAL); ;
    break;}
case 149:
#line 622 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_KERNEL); ;
    break;}
case 150:
#line 623 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_MONITOR); ;
    break;}
case 151:
#line 624 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_NTP); ;
    break;}
case 152:
#line 626 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Flag, PROTO_FILEGEN); ;
    break;}
case 153:
#line 634 "ntp_config.y"
{ append_queue(my_config.tinker, yyvsp[0].Queue);  ;
    break;}
case 154:
#line 638 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); ;
    break;}
case 155:
#line 639 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); ;
    break;}
case 156:
#line 643 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(LOOP_ALLAN, yyvsp[0].Double); ;
    break;}
case 157:
#line 644 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(LOOP_PHI, yyvsp[0].Double); ;
    break;}
case 158:
#line 645 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(LOOP_FREQ, yyvsp[0].Double); ;
    break;}
case 159:
#line 646 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(LOOP_HUFFPUFF, yyvsp[0].Double); ;
    break;}
case 160:
#line 647 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(LOOP_PANIC, yyvsp[0].Double); ;
    break;}
case 161:
#line 648 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(LOOP_MAX, yyvsp[0].Double); ;
    break;}
case 162:
#line 649 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(LOOP_MINSTEP, yyvsp[0].Double); ;
    break;}
case 163:
#line 658 "ntp_config.y"
{
                    if (curr_include_level >= MAXINCLUDELEVEL) {
                        fprintf(stderr, "getconfig: Maximum include file level exceeded.\n");
                        msyslog(LOG_INFO, "getconfig: Maximum include file level exceeded.");
                    }
                    else {
                        fp[curr_include_level + 1] = F_OPEN(FindConfig(yyvsp[0].String), "r");
                        if (fp[curr_include_level + 1] == NULL) {
                            fprintf(stderr, "getconfig: Couldn't open <%s>\n", FindConfig(yyvsp[0].String));
                            msyslog(LOG_INFO, "getconfig: Couldn't open <%s>", FindConfig(yyvsp[0].String));
                        }
                        else
                            ++curr_include_level;
                    }
                ;
    break;}
case 164:
#line 674 "ntp_config.y"
{
                    while (curr_include_level != -1) 
                        FCLOSE(fp[curr_include_level--]);
                ;
    break;}
case 165:
#line 680 "ntp_config.y"
{ enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, yyvsp[0].Double));  ;
    break;}
case 166:
#line 682 "ntp_config.y"
{ enqueue(my_config.vars, create_attr_ival(T_Calldelay, yyvsp[0].Integer));  ;
    break;}
case 167:
#line 684 "ntp_config.y"
{ enqueue(my_config.vars, create_attr_dval(T_Tick, yyvsp[0].Double));  ;
    break;}
case 168:
#line 686 "ntp_config.y"
{ enqueue(my_config.vars, create_attr_sval(T_Driftfile, yyvsp[0].String));  ;
    break;}
case 169:
#line 688 "ntp_config.y"
{ enqueue(my_config.vars, create_attr_sval(T_Pidfile, yyvsp[0].String));  ;
    break;}
case 170:
#line 690 "ntp_config.y"
{ enqueue(my_config.vars, create_attr_sval(T_Logfile, yyvsp[0].String));  ;
    break;}
case 171:
#line 692 "ntp_config.y"
{ enqueue(my_config.vars, create_attr_ival(T_Automax, yyvsp[0].Integer));  ;
    break;}
case 172:
#line 695 "ntp_config.y"
{ append_queue(my_config.logconfig, yyvsp[0].Queue);  ;
    break;}
case 173:
#line 697 "ntp_config.y"
{ append_queue(my_config.phone, yyvsp[0].Queue);  ;
    break;}
case 174:
#line 699 "ntp_config.y"
{ enqueue(my_config.setvar, yyvsp[0].Set_var);  ;
    break;}
case 175:
#line 701 "ntp_config.y"
{ enqueue(my_config.trap, create_addr_opts_node(yyvsp[-1].Address_node, yyvsp[0].Queue));  ;
    break;}
case 176:
#line 703 "ntp_config.y"
{ append_queue(my_config.ttl, yyvsp[0].Queue); ;
    break;}
case 177:
#line 708 "ntp_config.y"
{ yyval.Set_var = create_setvar_node(yyvsp[-3].String, yyvsp[-1].String, DEF); ;
    break;}
case 178:
#line 710 "ntp_config.y"
{ yyval.Set_var = create_setvar_node(yyvsp[-2].String, yyvsp[0].String, 0); ;
    break;}
case 179:
#line 715 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); ;
    break;}
case 180:
#line 716 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); ;
    break;}
case 181:
#line 720 "ntp_config.y"
{ yyval.Attr_val = create_attr_ival(T_Port, yyvsp[0].Integer); ;
    break;}
case 182:
#line 721 "ntp_config.y"
{ yyval.Attr_val = create_attr_pval(T_Interface, yyvsp[0].Address_node); ;
    break;}
case 183:
#line 726 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Attr_val); ;
    break;}
case 184:
#line 727 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Attr_val); ;
    break;}
case 185:
#line 731 "ntp_config.y"
{ yyval.Attr_val = create_attr_sval(yyvsp[-1].Integer, yyvsp[0].String); ;
    break;}
case 186:
#line 733 "ntp_config.y"
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
                ;
    break;}
case 187:
#line 749 "ntp_config.y"
{ yyval.Integer = '+'; ;
    break;}
case 188:
#line 750 "ntp_config.y"
{ yyval.Integer = '-'; ;
    break;}
case 189:
#line 751 "ntp_config.y"
{ yyval.Integer = '='; ;
    break;}
case 190:
#line 760 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, create_ival(yyvsp[0].Integer)); ;
    break;}
case 191:
#line 761 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(create_ival(yyvsp[0].Integer)); ;
    break;}
case 192:
#line 765 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, create_pval(yyvsp[0].String)); ;
    break;}
case 193:
#line 766 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(create_pval(yyvsp[0].String)); ;
    break;}
case 194:
#line 770 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Address_node); ;
    break;}
case 195:
#line 771 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Address_node); ;
    break;}
case 196:
#line 776 "ntp_config.y"
{ 
                    if (yyvsp[0].Integer != 0 && yyvsp[0].Integer != 1) {
                        yyerror("Integer value is not boolean (0 or 1). Assuming 1");
                        yyval.Integer = 1;
                    }
                    else
                        yyval.Integer = yyvsp[0].Integer; 
                ;
    break;}
case 197:
#line 784 "ntp_config.y"
{ yyval.Integer = 1; ;
    break;}
case 198:
#line 785 "ntp_config.y"
{ yyval.Integer = 0; ;
    break;}
case 199:
#line 789 "ntp_config.y"
{ yyval.Double = (double)yyvsp[0].Integer; ;
    break;}
case 200:
#line 790 "ntp_config.y"
{ yyval.Double = yyvsp[0].Double; ;
    break;}
case 201:
#line 799 "ntp_config.y"
{
                 my_config.sim_details = create_sim_node(yyvsp[-2].Queue, yyvsp[-1].Queue);

                 /* Reset the old_config_style variable */
                 old_config_style = 1;
             ;
    break;}
case 202:
#line 813 "ntp_config.y"
{ old_config_style = 0; ;
    break;}
case 203:
#line 817 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); ;
    break;}
case 204:
#line 818 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); ;
    break;}
case 205:
#line 822 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(T_Beep_Delay, yyvsp[0].Double); ;
    break;}
case 206:
#line 823 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(T_Sim_Duration, yyvsp[0].Double); ;
    break;}
case 207:
#line 827 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_server); ;
    break;}
case 208:
#line 828 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_server); ;
    break;}
case 209:
#line 833 "ntp_config.y"
{  yyval.Sim_server = create_sim_server(yyvsp[-4].Address_node, yyvsp[-2].Double, yyvsp[-1].Queue); ;
    break;}
case 210:
#line 837 "ntp_config.y"
{ yyval.Double = yyvsp[-1].Double; ;
    break;}
case 211:
#line 841 "ntp_config.y"
{ yyval.Address_node = yyvsp[0].Address_node; ;
    break;}
case 212:
#line 845 "ntp_config.y"
{ yyval.Address_node = yyvsp[0].Address_node; ;
    break;}
case 213:
#line 846 "ntp_config.y"
{ yyval.Address_node = create_address_node(yyvsp[0].String, T_String); ;
    break;}
case 214:
#line 850 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-1].Queue, yyvsp[0].Sim_script); ;
    break;}
case 215:
#line 851 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[0].Sim_script); ;
    break;}
case 216:
#line 856 "ntp_config.y"
{ yyval.Sim_script = create_sim_script_info(yyvsp[-3].Double, yyvsp[-1].Queue); ;
    break;}
case 217:
#line 860 "ntp_config.y"
{ yyval.Queue = enqueue(yyvsp[-2].Queue, yyvsp[-1].Attr_val); ;
    break;}
case 218:
#line 861 "ntp_config.y"
{ yyval.Queue = enqueue_in_new_queue(yyvsp[-1].Attr_val); ;
    break;}
case 219:
#line 866 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(T_Freq_Offset, yyvsp[0].Double); ;
    break;}
case 220:
#line 868 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(T_Wander, yyvsp[0].Double); ;
    break;}
case 221:
#line 870 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(T_Jitter, yyvsp[0].Double); ;
    break;}
case 222:
#line 872 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(T_Prop_Delay, yyvsp[0].Double); ;
    break;}
case 223:
#line 874 "ntp_config.y"
{ yyval.Attr_val = create_attr_dval(T_Proc_Delay, yyvsp[0].Double); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

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

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 878 "ntp_config.y"


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

