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
     T_Age = 258,
     T_All = 259,
     T_Allan = 260,
     T_Auth = 261,
     T_Autokey = 262,
     T_Automax = 263,
     T_Average = 264,
     T_Bclient = 265,
     T_Beacon = 266,
     T_Broadcast = 267,
     T_Broadcastclient = 268,
     T_Broadcastdelay = 269,
     T_Burst = 270,
     T_Calibrate = 271,
     T_Ceiling = 272,
     T_Clockstats = 273,
     T_Cohort = 274,
     T_ControlKey = 275,
     T_Crypto = 276,
     T_Cryptostats = 277,
     T_Day = 278,
     T_Default = 279,
     T_Digest = 280,
     T_Disable = 281,
     T_Discard = 282,
     T_Dispersion = 283,
     T_Double = 284,
     T_Driftfile = 285,
     T_Drop = 286,
     T_Ellipsis = 287,
     T_Enable = 288,
     T_End = 289,
     T_False = 290,
     T_File = 291,
     T_Filegen = 292,
     T_Flag1 = 293,
     T_Flag2 = 294,
     T_Flag3 = 295,
     T_Flag4 = 296,
     T_Flake = 297,
     T_Floor = 298,
     T_Freq = 299,
     T_Fudge = 300,
     T_Host = 301,
     T_Huffpuff = 302,
     T_Iburst = 303,
     T_Ident = 304,
     T_Ignore = 305,
     T_Incalloc = 306,
     T_Incmem = 307,
     T_Initalloc = 308,
     T_Initmem = 309,
     T_Includefile = 310,
     T_Integer = 311,
     T_Interface = 312,
     T_Intrange = 313,
     T_Ipv4 = 314,
     T_Ipv4_flag = 315,
     T_Ipv6 = 316,
     T_Ipv6_flag = 317,
     T_Kernel = 318,
     T_Key = 319,
     T_Keys = 320,
     T_Keysdir = 321,
     T_Kod = 322,
     T_Mssntp = 323,
     T_Leapfile = 324,
     T_Limited = 325,
     T_Link = 326,
     T_Listen = 327,
     T_Logconfig = 328,
     T_Logfile = 329,
     T_Loopstats = 330,
     T_Lowpriotrap = 331,
     T_Manycastclient = 332,
     T_Manycastserver = 333,
     T_Mask = 334,
     T_Maxage = 335,
     T_Maxclock = 336,
     T_Maxdepth = 337,
     T_Maxdist = 338,
     T_Maxmem = 339,
     T_Maxpoll = 340,
     T_Minclock = 341,
     T_Mindepth = 342,
     T_Mindist = 343,
     T_Minimum = 344,
     T_Minpoll = 345,
     T_Minsane = 346,
     T_Mode = 347,
     T_Monitor = 348,
     T_Month = 349,
     T_Mru = 350,
     T_Multicastclient = 351,
     T_Nic = 352,
     T_Nolink = 353,
     T_Nomodify = 354,
     T_None = 355,
     T_Nonvolatile = 356,
     T_Nopeer = 357,
     T_Noquery = 358,
     T_Noselect = 359,
     T_Noserve = 360,
     T_Notrap = 361,
     T_Notrust = 362,
     T_Ntp = 363,
     T_Ntpport = 364,
     T_NtpSignDsocket = 365,
     T_Orphan = 366,
     T_Orphanwait = 367,
     T_Panic = 368,
     T_Peer = 369,
     T_Peerstats = 370,
     T_Phone = 371,
     T_Pid = 372,
     T_Pidfile = 373,
     T_Pool = 374,
     T_Port = 375,
     T_Preempt = 376,
     T_Prefer = 377,
     T_Protostats = 378,
     T_Pw = 379,
     T_Qos = 380,
     T_Randfile = 381,
     T_Rawstats = 382,
     T_Refid = 383,
     T_Requestkey = 384,
     T_Restrict = 385,
     T_Revoke = 386,
     T_Saveconfigdir = 387,
     T_Server = 388,
     T_Setvar = 389,
     T_Source = 390,
     T_Statistics = 391,
     T_Stats = 392,
     T_Statsdir = 393,
     T_Step = 394,
     T_Stepout = 395,
     T_Stratum = 396,
     T_String = 397,
     T_Sysstats = 398,
     T_Tick = 399,
     T_Time1 = 400,
     T_Time2 = 401,
     T_Timingstats = 402,
     T_Tinker = 403,
     T_Tos = 404,
     T_Trap = 405,
     T_True = 406,
     T_Trustedkey = 407,
     T_Ttl = 408,
     T_Type = 409,
     T_Unconfig = 410,
     T_Unpeer = 411,
     T_Version = 412,
     T_WanderThreshold = 413,
     T_Week = 414,
     T_Wildcard = 415,
     T_Xleave = 416,
     T_Year = 417,
     T_Flag = 418,
     T_EOC = 419,
     T_Simulate = 420,
     T_Beep_Delay = 421,
     T_Sim_Duration = 422,
     T_Server_Offset = 423,
     T_Duration = 424,
     T_Freq_Offset = 425,
     T_Wander = 426,
     T_Jitter = 427,
     T_Prop_Delay = 428,
     T_Proc_Delay = 429
   };
#endif
/* Tokens.  */
#define T_Age 258
#define T_All 259
#define T_Allan 260
#define T_Auth 261
#define T_Autokey 262
#define T_Automax 263
#define T_Average 264
#define T_Bclient 265
#define T_Beacon 266
#define T_Broadcast 267
#define T_Broadcastclient 268
#define T_Broadcastdelay 269
#define T_Burst 270
#define T_Calibrate 271
#define T_Ceiling 272
#define T_Clockstats 273
#define T_Cohort 274
#define T_ControlKey 275
#define T_Crypto 276
#define T_Cryptostats 277
#define T_Day 278
#define T_Default 279
#define T_Digest 280
#define T_Disable 281
#define T_Discard 282
#define T_Dispersion 283
#define T_Double 284
#define T_Driftfile 285
#define T_Drop 286
#define T_Ellipsis 287
#define T_Enable 288
#define T_End 289
#define T_False 290
#define T_File 291
#define T_Filegen 292
#define T_Flag1 293
#define T_Flag2 294
#define T_Flag3 295
#define T_Flag4 296
#define T_Flake 297
#define T_Floor 298
#define T_Freq 299
#define T_Fudge 300
#define T_Host 301
#define T_Huffpuff 302
#define T_Iburst 303
#define T_Ident 304
#define T_Ignore 305
#define T_Incalloc 306
#define T_Incmem 307
#define T_Initalloc 308
#define T_Initmem 309
#define T_Includefile 310
#define T_Integer 311
#define T_Interface 312
#define T_Intrange 313
#define T_Ipv4 314
#define T_Ipv4_flag 315
#define T_Ipv6 316
#define T_Ipv6_flag 317
#define T_Kernel 318
#define T_Key 319
#define T_Keys 320
#define T_Keysdir 321
#define T_Kod 322
#define T_Mssntp 323
#define T_Leapfile 324
#define T_Limited 325
#define T_Link 326
#define T_Listen 327
#define T_Logconfig 328
#define T_Logfile 329
#define T_Loopstats 330
#define T_Lowpriotrap 331
#define T_Manycastclient 332
#define T_Manycastserver 333
#define T_Mask 334
#define T_Maxage 335
#define T_Maxclock 336
#define T_Maxdepth 337
#define T_Maxdist 338
#define T_Maxmem 339
#define T_Maxpoll 340
#define T_Minclock 341
#define T_Mindepth 342
#define T_Mindist 343
#define T_Minimum 344
#define T_Minpoll 345
#define T_Minsane 346
#define T_Mode 347
#define T_Monitor 348
#define T_Month 349
#define T_Mru 350
#define T_Multicastclient 351
#define T_Nic 352
#define T_Nolink 353
#define T_Nomodify 354
#define T_None 355
#define T_Nonvolatile 356
#define T_Nopeer 357
#define T_Noquery 358
#define T_Noselect 359
#define T_Noserve 360
#define T_Notrap 361
#define T_Notrust 362
#define T_Ntp 363
#define T_Ntpport 364
#define T_NtpSignDsocket 365
#define T_Orphan 366
#define T_Orphanwait 367
#define T_Panic 368
#define T_Peer 369
#define T_Peerstats 370
#define T_Phone 371
#define T_Pid 372
#define T_Pidfile 373
#define T_Pool 374
#define T_Port 375
#define T_Preempt 376
#define T_Prefer 377
#define T_Protostats 378
#define T_Pw 379
#define T_Qos 380
#define T_Randfile 381
#define T_Rawstats 382
#define T_Refid 383
#define T_Requestkey 384
#define T_Restrict 385
#define T_Revoke 386
#define T_Saveconfigdir 387
#define T_Server 388
#define T_Setvar 389
#define T_Source 390
#define T_Statistics 391
#define T_Stats 392
#define T_Statsdir 393
#define T_Step 394
#define T_Stepout 395
#define T_Stratum 396
#define T_String 397
#define T_Sysstats 398
#define T_Tick 399
#define T_Time1 400
#define T_Time2 401
#define T_Timingstats 402
#define T_Tinker 403
#define T_Tos 404
#define T_Trap 405
#define T_True 406
#define T_Trustedkey 407
#define T_Ttl 408
#define T_Type 409
#define T_Unconfig 410
#define T_Unpeer 411
#define T_Version 412
#define T_WanderThreshold 413
#define T_Week 414
#define T_Wildcard 415
#define T_Xleave 416
#define T_Year 417
#define T_Flag 418
#define T_EOC 419
#define T_Simulate 420
#define T_Beep_Delay 421
#define T_Sim_Duration 422
#define T_Server_Offset 423
#define T_Duration 424
#define T_Freq_Offset 425
#define T_Wander 426
#define T_Jitter 427
#define T_Prop_Delay 428
#define T_Proc_Delay 429




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 49 "ntp_parser.y"

	char *			String;
	double			Double;
	int			Integer;
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
#line 419 "ntp_parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


