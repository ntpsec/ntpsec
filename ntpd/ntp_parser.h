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


extern YYSTYPE yylval;

#endif /* not BISON_______NTPD_NTP_PARSER_H */
