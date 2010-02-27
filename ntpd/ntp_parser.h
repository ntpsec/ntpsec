
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
     T_Enable = 289,
     T_End = 290,
     T_False = 291,
     T_File = 292,
     T_Filegen = 293,
     T_Flag1 = 294,
     T_Flag2 = 295,
     T_Flag3 = 296,
     T_Flag4 = 297,
     T_Flake = 298,
     T_Floor = 299,
     T_Freq = 300,
     T_Fudge = 301,
     T_Host = 302,
     T_Huffpuff = 303,
     T_Iburst = 304,
     T_Ident = 305,
     T_Ignore = 306,
     T_Includefile = 307,
     T_Integer = 308,
     T_Interface = 309,
     T_Ipv4 = 310,
     T_Ipv4_flag = 311,
     T_Ipv6 = 312,
     T_Ipv6_flag = 313,
     T_Kernel = 314,
     T_Key = 315,
     T_Keys = 316,
     T_Keysdir = 317,
     T_Kod = 318,
     T_Mssntp = 319,
     T_Leapfile = 320,
     T_Limited = 321,
     T_Link = 322,
     T_Listen = 323,
     T_Logconfig = 324,
     T_Logfile = 325,
     T_Loopstats = 326,
     T_Lowpriotrap = 327,
     T_Manycastclient = 328,
     T_Manycastserver = 329,
     T_Mask = 330,
     T_Maxage = 331,
     T_Maxclock = 332,
     T_Maxdepth = 333,
     T_Maxdist = 334,
     T_Maxmem = 335,
     T_Maxpoll = 336,
     T_Minclock = 337,
     T_Mindepth = 338,
     T_Mindist = 339,
     T_Minimum = 340,
     T_Minpoll = 341,
     T_Minsane = 342,
     T_Mode = 343,
     T_Monitor = 344,
     T_Month = 345,
     T_Mru = 346,
     T_Multicastclient = 347,
     T_Nic = 348,
     T_Nolink = 349,
     T_Nomodify = 350,
     T_None = 351,
     T_Nopeer = 352,
     T_Noquery = 353,
     T_Noselect = 354,
     T_Noserve = 355,
     T_Notrap = 356,
     T_Notrust = 357,
     T_Ntp = 358,
     T_Ntpport = 359,
     T_NtpSignDsocket = 360,
     T_Orphan = 361,
     T_Panic = 362,
     T_Peer = 363,
     T_Peerstats = 364,
     T_Phone = 365,
     T_Pid = 366,
     T_Pidfile = 367,
     T_Pool = 368,
     T_Port = 369,
     T_Preempt = 370,
     T_Prefer = 371,
     T_Protostats = 372,
     T_Pw = 373,
     T_Qos = 374,
     T_Randfile = 375,
     T_Rawstats = 376,
     T_Refid = 377,
     T_Requestkey = 378,
     T_Restrict = 379,
     T_Revoke = 380,
     T_Saveconfigdir = 381,
     T_Server = 382,
     T_Setvar = 383,
     T_Sign = 384,
     T_Source = 385,
     T_Statistics = 386,
     T_Stats = 387,
     T_Statsdir = 388,
     T_Step = 389,
     T_Stepout = 390,
     T_Stratum = 391,
     T_String = 392,
     T_Sysstats = 393,
     T_Tick = 394,
     T_Time1 = 395,
     T_Time2 = 396,
     T_Timingstats = 397,
     T_Tinker = 398,
     T_Tos = 399,
     T_Trap = 400,
     T_True = 401,
     T_Trustedkey = 402,
     T_Ttl = 403,
     T_Type = 404,
     T_Unconfig = 405,
     T_Unpeer = 406,
     T_Version = 407,
     T_WanderThreshold = 408,
     T_Week = 409,
     T_Wildcard = 410,
     T_Xleave = 411,
     T_Year = 412,
     T_Flag = 413,
     T_Void = 414,
     T_EOC = 415,
     T_Simulate = 416,
     T_Beep_Delay = 417,
     T_Sim_Duration = 418,
     T_Server_Offset = 419,
     T_Duration = 420,
     T_Freq_Offset = 421,
     T_Wander = 422,
     T_Jitter = 423,
     T_Prop_Delay = 424,
     T_Proc_Delay = 425
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
#define T_Enable 289
#define T_End 290
#define T_False 291
#define T_File 292
#define T_Filegen 293
#define T_Flag1 294
#define T_Flag2 295
#define T_Flag3 296
#define T_Flag4 297
#define T_Flake 298
#define T_Floor 299
#define T_Freq 300
#define T_Fudge 301
#define T_Host 302
#define T_Huffpuff 303
#define T_Iburst 304
#define T_Ident 305
#define T_Ignore 306
#define T_Includefile 307
#define T_Integer 308
#define T_Interface 309
#define T_Ipv4 310
#define T_Ipv4_flag 311
#define T_Ipv6 312
#define T_Ipv6_flag 313
#define T_Kernel 314
#define T_Key 315
#define T_Keys 316
#define T_Keysdir 317
#define T_Kod 318
#define T_Mssntp 319
#define T_Leapfile 320
#define T_Limited 321
#define T_Link 322
#define T_Listen 323
#define T_Logconfig 324
#define T_Logfile 325
#define T_Loopstats 326
#define T_Lowpriotrap 327
#define T_Manycastclient 328
#define T_Manycastserver 329
#define T_Mask 330
#define T_Maxage 331
#define T_Maxclock 332
#define T_Maxdepth 333
#define T_Maxdist 334
#define T_Maxmem 335
#define T_Maxpoll 336
#define T_Minclock 337
#define T_Mindepth 338
#define T_Mindist 339
#define T_Minimum 340
#define T_Minpoll 341
#define T_Minsane 342
#define T_Mode 343
#define T_Monitor 344
#define T_Month 345
#define T_Mru 346
#define T_Multicastclient 347
#define T_Nic 348
#define T_Nolink 349
#define T_Nomodify 350
#define T_None 351
#define T_Nopeer 352
#define T_Noquery 353
#define T_Noselect 354
#define T_Noserve 355
#define T_Notrap 356
#define T_Notrust 357
#define T_Ntp 358
#define T_Ntpport 359
#define T_NtpSignDsocket 360
#define T_Orphan 361
#define T_Panic 362
#define T_Peer 363
#define T_Peerstats 364
#define T_Phone 365
#define T_Pid 366
#define T_Pidfile 367
#define T_Pool 368
#define T_Port 369
#define T_Preempt 370
#define T_Prefer 371
#define T_Protostats 372
#define T_Pw 373
#define T_Qos 374
#define T_Randfile 375
#define T_Rawstats 376
#define T_Refid 377
#define T_Requestkey 378
#define T_Restrict 379
#define T_Revoke 380
#define T_Saveconfigdir 381
#define T_Server 382
#define T_Setvar 383
#define T_Sign 384
#define T_Source 385
#define T_Statistics 386
#define T_Stats 387
#define T_Statsdir 388
#define T_Step 389
#define T_Stepout 390
#define T_Stratum 391
#define T_String 392
#define T_Sysstats 393
#define T_Tick 394
#define T_Time1 395
#define T_Time2 396
#define T_Timingstats 397
#define T_Tinker 398
#define T_Tos 399
#define T_Trap 400
#define T_True 401
#define T_Trustedkey 402
#define T_Ttl 403
#define T_Type 404
#define T_Unconfig 405
#define T_Unpeer 406
#define T_Version 407
#define T_WanderThreshold 408
#define T_Week 409
#define T_Wildcard 410
#define T_Xleave 411
#define T_Year 412
#define T_Flag 413
#define T_Void 414
#define T_EOC 415
#define T_Simulate 416
#define T_Beep_Delay 417
#define T_Sim_Duration 418
#define T_Server_Offset 419
#define T_Duration 420
#define T_Freq_Offset 421
#define T_Wander 422
#define T_Jitter 423
#define T_Prop_Delay 424
#define T_Proc_Delay 425




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
#line 409 "ntp_parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


