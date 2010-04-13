
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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
     T_Bias = 267,
     T_Broadcast = 268,
     T_Broadcastclient = 269,
     T_Broadcastdelay = 270,
     T_Burst = 271,
     T_Calibrate = 272,
     T_Calldelay = 273,
     T_Ceiling = 274,
     T_Clockstats = 275,
     T_Cohort = 276,
     T_ControlKey = 277,
     T_Crypto = 278,
     T_Cryptostats = 279,
     T_Day = 280,
     T_Default = 281,
     T_Digest = 282,
     T_Disable = 283,
     T_Discard = 284,
     T_Dispersion = 285,
     T_Double = 286,
     T_Driftfile = 287,
     T_Drop = 288,
     T_Ellipsis = 289,
     T_Enable = 290,
     T_End = 291,
     T_False = 292,
     T_File = 293,
     T_Filegen = 294,
     T_Flag1 = 295,
     T_Flag2 = 296,
     T_Flag3 = 297,
     T_Flag4 = 298,
     T_Flake = 299,
     T_Floor = 300,
     T_Freq = 301,
     T_Fudge = 302,
     T_Host = 303,
     T_Huffpuff = 304,
     T_Iburst = 305,
     T_Ident = 306,
     T_Ignore = 307,
     T_Incalloc = 308,
     T_Incmem = 309,
     T_Initalloc = 310,
     T_Initmem = 311,
     T_Includefile = 312,
     T_Integer = 313,
     T_Interface = 314,
     T_Ipv4 = 315,
     T_Ipv4_flag = 316,
     T_Ipv6 = 317,
     T_Ipv6_flag = 318,
     T_Kernel = 319,
     T_Key = 320,
     T_Keys = 321,
     T_Keysdir = 322,
     T_Kod = 323,
     T_Mssntp = 324,
     T_Leapfile = 325,
     T_Limited = 326,
     T_Link = 327,
     T_Listen = 328,
     T_Logconfig = 329,
     T_Logfile = 330,
     T_Loopstats = 331,
     T_Lowpriotrap = 332,
     T_Manycastclient = 333,
     T_Manycastserver = 334,
     T_Mask = 335,
     T_Maxage = 336,
     T_Maxclock = 337,
     T_Maxdepth = 338,
     T_Maxdist = 339,
     T_Maxmem = 340,
     T_Maxpoll = 341,
     T_Minclock = 342,
     T_Mindepth = 343,
     T_Mindist = 344,
     T_Minimum = 345,
     T_Minpoll = 346,
     T_Minsane = 347,
     T_Mode = 348,
     T_Monitor = 349,
     T_Month = 350,
     T_Mru = 351,
     T_Multicastclient = 352,
     T_Nic = 353,
     T_Nolink = 354,
     T_Nomodify = 355,
     T_None = 356,
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
     T_Panic = 367,
     T_Peer = 368,
     T_Peerstats = 369,
     T_Phone = 370,
     T_Pid = 371,
     T_Pidfile = 372,
     T_Pool = 373,
     T_Port = 374,
     T_Preempt = 375,
     T_Prefer = 376,
     T_Protostats = 377,
     T_Pw = 378,
     T_Qos = 379,
     T_Randfile = 380,
     T_Rawstats = 381,
     T_Refid = 382,
     T_Requestkey = 383,
     T_Restrict = 384,
     T_Revoke = 385,
     T_Saveconfigdir = 386,
     T_Server = 387,
     T_Setvar = 388,
     T_Sign = 389,
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
     T_Void = 419,
     T_EOC = 420,
     T_Simulate = 421,
     T_Beep_Delay = 422,
     T_Sim_Duration = 423,
     T_Server_Offset = 424,
     T_Duration = 425,
     T_Freq_Offset = 426,
     T_Wander = 427,
     T_Jitter = 428,
     T_Prop_Delay = 429,
     T_Proc_Delay = 430
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
#define T_Bias 267
#define T_Broadcast 268
#define T_Broadcastclient 269
#define T_Broadcastdelay 270
#define T_Burst 271
#define T_Calibrate 272
#define T_Calldelay 273
#define T_Ceiling 274
#define T_Clockstats 275
#define T_Cohort 276
#define T_ControlKey 277
#define T_Crypto 278
#define T_Cryptostats 279
#define T_Day 280
#define T_Default 281
#define T_Digest 282
#define T_Disable 283
#define T_Discard 284
#define T_Dispersion 285
#define T_Double 286
#define T_Driftfile 287
#define T_Drop 288
#define T_Ellipsis 289
#define T_Enable 290
#define T_End 291
#define T_False 292
#define T_File 293
#define T_Filegen 294
#define T_Flag1 295
#define T_Flag2 296
#define T_Flag3 297
#define T_Flag4 298
#define T_Flake 299
#define T_Floor 300
#define T_Freq 301
#define T_Fudge 302
#define T_Host 303
#define T_Huffpuff 304
#define T_Iburst 305
#define T_Ident 306
#define T_Ignore 307
#define T_Incalloc 308
#define T_Incmem 309
#define T_Initalloc 310
#define T_Initmem 311
#define T_Includefile 312
#define T_Integer 313
#define T_Interface 314
#define T_Ipv4 315
#define T_Ipv4_flag 316
#define T_Ipv6 317
#define T_Ipv6_flag 318
#define T_Kernel 319
#define T_Key 320
#define T_Keys 321
#define T_Keysdir 322
#define T_Kod 323
#define T_Mssntp 324
#define T_Leapfile 325
#define T_Limited 326
#define T_Link 327
#define T_Listen 328
#define T_Logconfig 329
#define T_Logfile 330
#define T_Loopstats 331
#define T_Lowpriotrap 332
#define T_Manycastclient 333
#define T_Manycastserver 334
#define T_Mask 335
#define T_Maxage 336
#define T_Maxclock 337
#define T_Maxdepth 338
#define T_Maxdist 339
#define T_Maxmem 340
#define T_Maxpoll 341
#define T_Minclock 342
#define T_Mindepth 343
#define T_Mindist 344
#define T_Minimum 345
#define T_Minpoll 346
#define T_Minsane 347
#define T_Mode 348
#define T_Monitor 349
#define T_Month 350
#define T_Mru 351
#define T_Multicastclient 352
#define T_Nic 353
#define T_Nolink 354
#define T_Nomodify 355
#define T_None 356
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
#define T_Panic 367
#define T_Peer 368
#define T_Peerstats 369
#define T_Phone 370
#define T_Pid 371
#define T_Pidfile 372
#define T_Pool 373
#define T_Port 374
#define T_Preempt 375
#define T_Prefer 376
#define T_Protostats 377
#define T_Pw 378
#define T_Qos 379
#define T_Randfile 380
#define T_Rawstats 381
#define T_Refid 382
#define T_Requestkey 383
#define T_Restrict 384
#define T_Revoke 385
#define T_Saveconfigdir 386
#define T_Server 387
#define T_Setvar 388
#define T_Sign 389
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
#define T_Void 419
#define T_EOC 420
#define T_Simulate 421
#define T_Beep_Delay 422
#define T_Sim_Duration 423
#define T_Server_Offset 424
#define T_Duration 425
#define T_Freq_Offset 426
#define T_Wander 427
#define T_Jitter 428
#define T_Prop_Delay 429
#define T_Proc_Delay 430




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 50 "ntp_parser.y"

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



/* Line 1676 of yacc.c  */
#line 419 "ntp_parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


