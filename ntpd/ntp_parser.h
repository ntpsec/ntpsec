/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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

#ifndef YY_YY_______NTPD_NTP_PARSER_H_INCLUDED
# define YY_YY_______NTPD_NTP_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_Abbrev = 258,
    T_Age = 259,
    T_All = 260,
    T_Allan = 261,
    T_Allpeers = 262,
    T_Auth = 263,
    T_Autokey = 264,
    T_Automax = 265,
    T_Average = 266,
    T_Bclient = 267,
    T_Beacon = 268,
    T_Broadcast = 269,
    T_Broadcastclient = 270,
    T_Broadcastdelay = 271,
    T_Burst = 272,
    T_Calibrate = 273,
    T_Ceiling = 274,
    T_Clockstats = 275,
    T_Cohort = 276,
    T_ControlKey = 277,
    T_Crypto = 278,
    T_Cryptostats = 279,
    T_Ctl = 280,
    T_Day = 281,
    T_Default = 282,
    T_Digest = 283,
    T_Disable = 284,
    T_Discard = 285,
    T_Dispersion = 286,
    T_Double = 287,
    T_Driftfile = 288,
    T_Drop = 289,
    T_Dscp = 290,
    T_Ellipsis = 291,
    T_Enable = 292,
    T_End = 293,
    T_False = 294,
    T_File = 295,
    T_Filegen = 296,
    T_Filenum = 297,
    T_Flag1 = 298,
    T_Flag2 = 299,
    T_Flag3 = 300,
    T_Flag4 = 301,
    T_Flake = 302,
    T_Floor = 303,
    T_Freq = 304,
    T_Fudge = 305,
    T_Host = 306,
    T_Huffpuff = 307,
    T_Iburst = 308,
    T_Ident = 309,
    T_Ignore = 310,
    T_Incalloc = 311,
    T_Incmem = 312,
    T_Initalloc = 313,
    T_Initmem = 314,
    T_Includefile = 315,
    T_Integer = 316,
    T_Interface = 317,
    T_Intrange = 318,
    T_Io = 319,
    T_Ipv4 = 320,
    T_Ipv4_flag = 321,
    T_Ipv6 = 322,
    T_Ipv6_flag = 323,
    T_Kernel = 324,
    T_Key = 325,
    T_Keys = 326,
    T_Keysdir = 327,
    T_Kod = 328,
    T_Mssntp = 329,
    T_Leapfile = 330,
    T_Limited = 331,
    T_Link = 332,
    T_Listen = 333,
    T_Logconfig = 334,
    T_Logfile = 335,
    T_Loopstats = 336,
    T_Lowpriotrap = 337,
    T_Manycastclient = 338,
    T_Manycastserver = 339,
    T_Mask = 340,
    T_Maxage = 341,
    T_Maxclock = 342,
    T_Maxdepth = 343,
    T_Maxdist = 344,
    T_Maxmem = 345,
    T_Maxpoll = 346,
    T_Mdnstries = 347,
    T_Mem = 348,
    T_Memlock = 349,
    T_Minclock = 350,
    T_Mindepth = 351,
    T_Mindist = 352,
    T_Minimum = 353,
    T_Minpoll = 354,
    T_Minsane = 355,
    T_Mode = 356,
    T_Mode7 = 357,
    T_Monitor = 358,
    T_Month = 359,
    T_Mru = 360,
    T_Multicastclient = 361,
    T_Nic = 362,
    T_Nolink = 363,
    T_Nomodify = 364,
    T_Nomrulist = 365,
    T_None = 366,
    T_Nonvolatile = 367,
    T_Nopeer = 368,
    T_Noquery = 369,
    T_Noselect = 370,
    T_Noserve = 371,
    T_Notrap = 372,
    T_Notrust = 373,
    T_Ntp = 374,
    T_Ntpport = 375,
    T_NtpSignDsocket = 376,
    T_Orphan = 377,
    T_Orphanwait = 378,
    T_Panic = 379,
    T_Peer = 380,
    T_Peerstats = 381,
    T_Phone = 382,
    T_Pid = 383,
    T_Pidfile = 384,
    T_Pool = 385,
    T_Port = 386,
    T_Preempt = 387,
    T_Prefer = 388,
    T_Protostats = 389,
    T_Pw = 390,
    T_Randfile = 391,
    T_Rawstats = 392,
    T_Refid = 393,
    T_Requestkey = 394,
    T_Reset = 395,
    T_Restrict = 396,
    T_Revoke = 397,
    T_Rlimit = 398,
    T_Saveconfigdir = 399,
    T_Server = 400,
    T_Setvar = 401,
    T_Source = 402,
    T_Stacksize = 403,
    T_Statistics = 404,
    T_Stats = 405,
    T_Statsdir = 406,
    T_Step = 407,
    T_Stepback = 408,
    T_Stepfwd = 409,
    T_Stepout = 410,
    T_Stratum = 411,
    T_String = 412,
    T_Sys = 413,
    T_Sysstats = 414,
    T_Tick = 415,
    T_Time1 = 416,
    T_Time2 = 417,
    T_Timer = 418,
    T_Timingstats = 419,
    T_Tinker = 420,
    T_Tos = 421,
    T_Trap = 422,
    T_True = 423,
    T_Trustedkey = 424,
    T_Ttl = 425,
    T_Type = 426,
    T_U_int = 427,
    T_Unconfig = 428,
    T_Unpeer = 429,
    T_Version = 430,
    T_WanderThreshold = 431,
    T_Week = 432,
    T_Wildcard = 433,
    T_Xleave = 434,
    T_Year = 435,
    T_Flag = 436,
    T_EOC = 437,
    T_Simulate = 438,
    T_Beep_Delay = 439,
    T_Sim_Duration = 440,
    T_Server_Offset = 441,
    T_Duration = 442,
    T_Freq_Offset = 443,
    T_Wander = 444,
    T_Jitter = 445,
    T_Prop_Delay = 446,
    T_Proc_Delay = 447
  };
#endif
/* Tokens.  */
#define T_Abbrev 258
#define T_Age 259
#define T_All 260
#define T_Allan 261
#define T_Allpeers 262
#define T_Auth 263
#define T_Autokey 264
#define T_Automax 265
#define T_Average 266
#define T_Bclient 267
#define T_Beacon 268
#define T_Broadcast 269
#define T_Broadcastclient 270
#define T_Broadcastdelay 271
#define T_Burst 272
#define T_Calibrate 273
#define T_Ceiling 274
#define T_Clockstats 275
#define T_Cohort 276
#define T_ControlKey 277
#define T_Crypto 278
#define T_Cryptostats 279
#define T_Ctl 280
#define T_Day 281
#define T_Default 282
#define T_Digest 283
#define T_Disable 284
#define T_Discard 285
#define T_Dispersion 286
#define T_Double 287
#define T_Driftfile 288
#define T_Drop 289
#define T_Dscp 290
#define T_Ellipsis 291
#define T_Enable 292
#define T_End 293
#define T_False 294
#define T_File 295
#define T_Filegen 296
#define T_Filenum 297
#define T_Flag1 298
#define T_Flag2 299
#define T_Flag3 300
#define T_Flag4 301
#define T_Flake 302
#define T_Floor 303
#define T_Freq 304
#define T_Fudge 305
#define T_Host 306
#define T_Huffpuff 307
#define T_Iburst 308
#define T_Ident 309
#define T_Ignore 310
#define T_Incalloc 311
#define T_Incmem 312
#define T_Initalloc 313
#define T_Initmem 314
#define T_Includefile 315
#define T_Integer 316
#define T_Interface 317
#define T_Intrange 318
#define T_Io 319
#define T_Ipv4 320
#define T_Ipv4_flag 321
#define T_Ipv6 322
#define T_Ipv6_flag 323
#define T_Kernel 324
#define T_Key 325
#define T_Keys 326
#define T_Keysdir 327
#define T_Kod 328
#define T_Mssntp 329
#define T_Leapfile 330
#define T_Limited 331
#define T_Link 332
#define T_Listen 333
#define T_Logconfig 334
#define T_Logfile 335
#define T_Loopstats 336
#define T_Lowpriotrap 337
#define T_Manycastclient 338
#define T_Manycastserver 339
#define T_Mask 340
#define T_Maxage 341
#define T_Maxclock 342
#define T_Maxdepth 343
#define T_Maxdist 344
#define T_Maxmem 345
#define T_Maxpoll 346
#define T_Mdnstries 347
#define T_Mem 348
#define T_Memlock 349
#define T_Minclock 350
#define T_Mindepth 351
#define T_Mindist 352
#define T_Minimum 353
#define T_Minpoll 354
#define T_Minsane 355
#define T_Mode 356
#define T_Mode7 357
#define T_Monitor 358
#define T_Month 359
#define T_Mru 360
#define T_Multicastclient 361
#define T_Nic 362
#define T_Nolink 363
#define T_Nomodify 364
#define T_Nomrulist 365
#define T_None 366
#define T_Nonvolatile 367
#define T_Nopeer 368
#define T_Noquery 369
#define T_Noselect 370
#define T_Noserve 371
#define T_Notrap 372
#define T_Notrust 373
#define T_Ntp 374
#define T_Ntpport 375
#define T_NtpSignDsocket 376
#define T_Orphan 377
#define T_Orphanwait 378
#define T_Panic 379
#define T_Peer 380
#define T_Peerstats 381
#define T_Phone 382
#define T_Pid 383
#define T_Pidfile 384
#define T_Pool 385
#define T_Port 386
#define T_Preempt 387
#define T_Prefer 388
#define T_Protostats 389
#define T_Pw 390
#define T_Randfile 391
#define T_Rawstats 392
#define T_Refid 393
#define T_Requestkey 394
#define T_Reset 395
#define T_Restrict 396
#define T_Revoke 397
#define T_Rlimit 398
#define T_Saveconfigdir 399
#define T_Server 400
#define T_Setvar 401
#define T_Source 402
#define T_Stacksize 403
#define T_Statistics 404
#define T_Stats 405
#define T_Statsdir 406
#define T_Step 407
#define T_Stepback 408
#define T_Stepfwd 409
#define T_Stepout 410
#define T_Stratum 411
#define T_String 412
#define T_Sys 413
#define T_Sysstats 414
#define T_Tick 415
#define T_Time1 416
#define T_Time2 417
#define T_Timer 418
#define T_Timingstats 419
#define T_Tinker 420
#define T_Tos 421
#define T_Trap 422
#define T_True 423
#define T_Trustedkey 424
#define T_Ttl 425
#define T_Type 426
#define T_U_int 427
#define T_Unconfig 428
#define T_Unpeer 429
#define T_Version 430
#define T_WanderThreshold 431
#define T_Week 432
#define T_Wildcard 433
#define T_Xleave 434
#define T_Year 435
#define T_Flag 436
#define T_EOC 437
#define T_Simulate 438
#define T_Beep_Delay 439
#define T_Sim_Duration 440
#define T_Server_Offset 441
#define T_Duration 442
#define T_Freq_Offset 443
#define T_Wander 444
#define T_Jitter 445
#define T_Prop_Delay 446
#define T_Proc_Delay 447

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 51 "ntp_parser.y" /* yacc.c:1909  */

	char *			String;
	double			Double;
	int			Integer;
	unsigned		U_int;
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

#line 457 "../../ntpd/ntp_parser.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_______NTPD_NTP_PARSER_H_INCLUDED  */
