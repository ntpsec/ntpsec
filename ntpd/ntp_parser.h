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


extern YYSTYPE yylval;

#endif /* not BISON_______NTPD_NTP_PARSER_H */
