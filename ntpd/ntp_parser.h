/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
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
     T_Ellipsis = 290,
     T_Enable = 291,
     T_End = 292,
     T_False = 293,
     T_File = 294,
     T_Filegen = 295,
     T_Flag1 = 296,
     T_Flag2 = 297,
     T_Flag3 = 298,
     T_Flag4 = 299,
     T_Flake = 300,
     T_Floor = 301,
     T_Freq = 302,
     T_Fudge = 303,
     T_Host = 304,
     T_Huffpuff = 305,
     T_Iburst = 306,
     T_Ident = 307,
     T_Ignore = 308,
     T_Incalloc = 309,
     T_Incmem = 310,
     T_Initalloc = 311,
     T_Initmem = 312,
     T_Includefile = 313,
     T_Integer = 314,
     T_Interface = 315,
     T_Intrange = 316,
     T_Io = 317,
     T_Ipv4 = 318,
     T_Ipv4_flag = 319,
     T_Ipv6 = 320,
     T_Ipv6_flag = 321,
     T_Kernel = 322,
     T_Key = 323,
     T_Keys = 324,
     T_Keysdir = 325,
     T_Kod = 326,
     T_Mssntp = 327,
     T_Leapfile = 328,
     T_Limited = 329,
     T_Link = 330,
     T_Listen = 331,
     T_Logconfig = 332,
     T_Logfile = 333,
     T_Loopstats = 334,
     T_Lowpriotrap = 335,
     T_Manycastclient = 336,
     T_Manycastserver = 337,
     T_Mask = 338,
     T_Maxage = 339,
     T_Maxclock = 340,
     T_Maxdepth = 341,
     T_Maxdist = 342,
     T_Maxmem = 343,
     T_Maxpoll = 344,
     T_Mem = 345,
     T_Minclock = 346,
     T_Mindepth = 347,
     T_Mindist = 348,
     T_Minimum = 349,
     T_Minpoll = 350,
     T_Minsane = 351,
     T_Mode = 352,
     T_Mode7 = 353,
     T_Monitor = 354,
     T_Month = 355,
     T_Mru = 356,
     T_Multicastclient = 357,
     T_Nic = 358,
     T_Nolink = 359,
     T_Nomodify = 360,
     T_Nomrulist = 361,
     T_None = 362,
     T_Nonvolatile = 363,
     T_Nopeer = 364,
     T_Noquery = 365,
     T_Noselect = 366,
     T_Noserve = 367,
     T_Notrap = 368,
     T_Notrust = 369,
     T_Ntp = 370,
     T_Ntpport = 371,
     T_NtpSignDsocket = 372,
     T_Orphan = 373,
     T_Orphanwait = 374,
     T_Panic = 375,
     T_Peer = 376,
     T_Peerstats = 377,
     T_Phone = 378,
     T_Pid = 379,
     T_Pidfile = 380,
     T_Pool = 381,
     T_Port = 382,
     T_Preempt = 383,
     T_Prefer = 384,
     T_Protostats = 385,
     T_Pw = 386,
     T_Randfile = 387,
     T_Rawstats = 388,
     T_Refid = 389,
     T_Requestkey = 390,
     T_Reset = 391,
     T_Restrict = 392,
     T_Revoke = 393,
     T_Saveconfigdir = 394,
     T_Server = 395,
     T_Setvar = 396,
     T_Source = 397,
     T_Statistics = 398,
     T_Stats = 399,
     T_Statsdir = 400,
     T_Step = 401,
     T_Stepout = 402,
     T_Stratum = 403,
     T_String = 404,
     T_Sys = 405,
     T_Sysstats = 406,
     T_Tick = 407,
     T_Time1 = 408,
     T_Time2 = 409,
     T_Timer = 410,
     T_Timingstats = 411,
     T_Tinker = 412,
     T_Tos = 413,
     T_Trap = 414,
     T_True = 415,
     T_Trustedkey = 416,
     T_Ttl = 417,
     T_Type = 418,
     T_U_int = 419,
     T_Unconfig = 420,
     T_Unpeer = 421,
     T_Version = 422,
     T_WanderThreshold = 423,
     T_Week = 424,
     T_Wildcard = 425,
     T_Xleave = 426,
     T_Year = 427,
     T_Flag = 428,
     T_EOC = 429,
     T_Simulate = 430,
     T_Beep_Delay = 431,
     T_Sim_Duration = 432,
     T_Server_Offset = 433,
     T_Duration = 434,
     T_Freq_Offset = 435,
     T_Wander = 436,
     T_Jitter = 437,
     T_Prop_Delay = 438,
     T_Proc_Delay = 439
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
#define T_Ellipsis 290
#define T_Enable 291
#define T_End 292
#define T_False 293
#define T_File 294
#define T_Filegen 295
#define T_Flag1 296
#define T_Flag2 297
#define T_Flag3 298
#define T_Flag4 299
#define T_Flake 300
#define T_Floor 301
#define T_Freq 302
#define T_Fudge 303
#define T_Host 304
#define T_Huffpuff 305
#define T_Iburst 306
#define T_Ident 307
#define T_Ignore 308
#define T_Incalloc 309
#define T_Incmem 310
#define T_Initalloc 311
#define T_Initmem 312
#define T_Includefile 313
#define T_Integer 314
#define T_Interface 315
#define T_Intrange 316
#define T_Io 317
#define T_Ipv4 318
#define T_Ipv4_flag 319
#define T_Ipv6 320
#define T_Ipv6_flag 321
#define T_Kernel 322
#define T_Key 323
#define T_Keys 324
#define T_Keysdir 325
#define T_Kod 326
#define T_Mssntp 327
#define T_Leapfile 328
#define T_Limited 329
#define T_Link 330
#define T_Listen 331
#define T_Logconfig 332
#define T_Logfile 333
#define T_Loopstats 334
#define T_Lowpriotrap 335
#define T_Manycastclient 336
#define T_Manycastserver 337
#define T_Mask 338
#define T_Maxage 339
#define T_Maxclock 340
#define T_Maxdepth 341
#define T_Maxdist 342
#define T_Maxmem 343
#define T_Maxpoll 344
#define T_Mem 345
#define T_Minclock 346
#define T_Mindepth 347
#define T_Mindist 348
#define T_Minimum 349
#define T_Minpoll 350
#define T_Minsane 351
#define T_Mode 352
#define T_Mode7 353
#define T_Monitor 354
#define T_Month 355
#define T_Mru 356
#define T_Multicastclient 357
#define T_Nic 358
#define T_Nolink 359
#define T_Nomodify 360
#define T_Nomrulist 361
#define T_None 362
#define T_Nonvolatile 363
#define T_Nopeer 364
#define T_Noquery 365
#define T_Noselect 366
#define T_Noserve 367
#define T_Notrap 368
#define T_Notrust 369
#define T_Ntp 370
#define T_Ntpport 371
#define T_NtpSignDsocket 372
#define T_Orphan 373
#define T_Orphanwait 374
#define T_Panic 375
#define T_Peer 376
#define T_Peerstats 377
#define T_Phone 378
#define T_Pid 379
#define T_Pidfile 380
#define T_Pool 381
#define T_Port 382
#define T_Preempt 383
#define T_Prefer 384
#define T_Protostats 385
#define T_Pw 386
#define T_Randfile 387
#define T_Rawstats 388
#define T_Refid 389
#define T_Requestkey 390
#define T_Reset 391
#define T_Restrict 392
#define T_Revoke 393
#define T_Saveconfigdir 394
#define T_Server 395
#define T_Setvar 396
#define T_Source 397
#define T_Statistics 398
#define T_Stats 399
#define T_Statsdir 400
#define T_Step 401
#define T_Stepout 402
#define T_Stratum 403
#define T_String 404
#define T_Sys 405
#define T_Sysstats 406
#define T_Tick 407
#define T_Time1 408
#define T_Time2 409
#define T_Timer 410
#define T_Timingstats 411
#define T_Tinker 412
#define T_Tos 413
#define T_Trap 414
#define T_True 415
#define T_Trustedkey 416
#define T_Ttl 417
#define T_Type 418
#define T_U_int 419
#define T_Unconfig 420
#define T_Unpeer 421
#define T_Version 422
#define T_WanderThreshold 423
#define T_Week 424
#define T_Wildcard 425
#define T_Xleave 426
#define T_Year 427
#define T_Flag 428
#define T_EOC 429
#define T_Simulate 430
#define T_Beep_Delay 431
#define T_Sim_Duration 432
#define T_Server_Offset 433
#define T_Duration 434
#define T_Freq_Offset 435
#define T_Wander 436
#define T_Jitter 437
#define T_Prop_Delay 438
#define T_Proc_Delay 439




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 49 "ntp_parser.y"

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



/* Line 1685 of yacc.c  */
#line 440 "../../ntpd/ntp_parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


