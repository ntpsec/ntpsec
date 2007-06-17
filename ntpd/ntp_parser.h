#ifndef BISON_______NTPD_NTP_PARSER_H
# define BISON_______NTPD_NTP_PARSER_H

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


extern YYSTYPE yylval;

#endif /* not BISON_______NTPD_NTP_PARSER_H */
