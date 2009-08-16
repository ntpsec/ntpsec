
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
     T_Allan = 259,
     T_Auth = 260,
     T_Autokey = 261,
     T_Automax = 262,
     T_Average = 263,
     T_Bclient = 264,
     T_Beacon = 265,
     T_Bias = 266,
     T_Broadcast = 267,
     T_Broadcastclient = 268,
     T_Broadcastdelay = 269,
     T_Burst = 270,
     T_Calibrate = 271,
     T_Calldelay = 272,
     T_Ceiling = 273,
     T_Clockstats = 274,
     T_Cohort = 275,
     T_ControlKey = 276,
     T_Crypto = 277,
     T_Cryptostats = 278,
     T_Day = 279,
     T_Default = 280,
     T_Disable = 281,
     T_Discard = 282,
     T_Dispersion = 283,
     T_Double = 284,
     T_Driftfile = 285,
     T_Enable = 286,
     T_End = 287,
     T_False = 288,
     T_File = 289,
     T_Filegen = 290,
     T_Flag1 = 291,
     T_Flag2 = 292,
     T_Flag3 = 293,
     T_Flag4 = 294,
     T_Flake = 295,
     T_Floor = 296,
     T_Freq = 297,
     T_Fudge = 298,
     T_Host = 299,
     T_Huffpuff = 300,
     T_Iburst = 301,
     T_Ident = 302,
     T_Ignore = 303,
     T_Includefile = 304,
     T_Integer = 305,
     T_Interface = 306,
     T_IPv4_flag = 307,
     T_IPv6_flag = 308,
     T_Kernel = 309,
     T_Key = 310,
     T_Keys = 311,
     T_Keysdir = 312,
     T_Kod = 313,
     T_Mssntp = 314,
     T_Leap = 315,
     T_Leapfile = 316,
     T_Limited = 317,
     T_Link = 318,
     T_Logconfig = 319,
     T_Logfile = 320,
     T_Loopstats = 321,
     T_Lowpriotrap = 322,
     T_Manycastclient = 323,
     T_Manycastserver = 324,
     T_Mask = 325,
     T_Maxclock = 326,
     T_Maxdist = 327,
     T_Maxhop = 328,
     T_Maxpoll = 329,
     T_Minclock = 330,
     T_Mindist = 331,
     T_Minimum = 332,
     T_Minpoll = 333,
     T_Minsane = 334,
     T_Mode = 335,
     T_Monitor = 336,
     T_Month = 337,
     T_Multicastclient = 338,
     T_Nolink = 339,
     T_Nomodify = 340,
     T_None = 341,
     T_Nopeer = 342,
     T_Noquery = 343,
     T_Noselect = 344,
     T_Noserve = 345,
     T_Notrap = 346,
     T_Notrust = 347,
     T_Ntp = 348,
     T_Ntpport = 349,
     T_NtpSignDsocket = 350,
     T_Orphan = 351,
     T_Panic = 352,
     T_Peer = 353,
     T_Peerstats = 354,
     T_Phone = 355,
     T_Pid = 356,
     T_Pidfile = 357,
     T_Pool = 358,
     T_Port = 359,
     T_Pps = 360,
     T_Preempt = 361,
     T_Prefer = 362,
     T_Protostats = 363,
     T_Pw = 364,
     T_Qos = 365,
     T_RandFile = 366,
     T_Rawstats = 367,
     T_Refid = 368,
     T_Requestkey = 369,
     T_Restrict = 370,
     T_Revoke = 371,
     T_Server = 372,
     T_Setvar = 373,
     T_Sign = 374,
     T_Statistics = 375,
     T_Stats = 376,
     T_Statsdir = 377,
     T_Step = 378,
     T_Stepout = 379,
     T_Stratum = 380,
     T_String = 381,
     T_Sysstats = 382,
     T_Tick = 383,
     T_Time1 = 384,
     T_Time2 = 385,
     T_Timingstats = 386,
     T_Tinker = 387,
     T_Tos = 388,
     T_Trap = 389,
     T_True = 390,
     T_Trustedkey = 391,
     T_Ttl = 392,
     T_Type = 393,
     T_Unconfig = 394,
     T_Unpeer = 395,
     T_Version = 396,
     T_WanderThreshold = 397,
     T_Week = 398,
     T_Xleave = 399,
     T_Year = 400,
     T_Flag = 401,
     T_Void = 402,
     T_EOC = 403,
     T_Simulate = 404,
     T_Beep_Delay = 405,
     T_Sim_Duration = 406,
     T_Server_Offset = 407,
     T_Duration = 408,
     T_Freq_Offset = 409,
     T_Wander = 410,
     T_Jitter = 411,
     T_Prop_Delay = 412,
     T_Proc_Delay = 413
   };
#endif
/* Tokens.  */
#define T_Age 258
#define T_Allan 259
#define T_Auth 260
#define T_Autokey 261
#define T_Automax 262
#define T_Average 263
#define T_Bclient 264
#define T_Beacon 265
#define T_Bias 266
#define T_Broadcast 267
#define T_Broadcastclient 268
#define T_Broadcastdelay 269
#define T_Burst 270
#define T_Calibrate 271
#define T_Calldelay 272
#define T_Ceiling 273
#define T_Clockstats 274
#define T_Cohort 275
#define T_ControlKey 276
#define T_Crypto 277
#define T_Cryptostats 278
#define T_Day 279
#define T_Default 280
#define T_Disable 281
#define T_Discard 282
#define T_Dispersion 283
#define T_Double 284
#define T_Driftfile 285
#define T_Enable 286
#define T_End 287
#define T_False 288
#define T_File 289
#define T_Filegen 290
#define T_Flag1 291
#define T_Flag2 292
#define T_Flag3 293
#define T_Flag4 294
#define T_Flake 295
#define T_Floor 296
#define T_Freq 297
#define T_Fudge 298
#define T_Host 299
#define T_Huffpuff 300
#define T_Iburst 301
#define T_Ident 302
#define T_Ignore 303
#define T_Includefile 304
#define T_Integer 305
#define T_Interface 306
#define T_IPv4_flag 307
#define T_IPv6_flag 308
#define T_Kernel 309
#define T_Key 310
#define T_Keys 311
#define T_Keysdir 312
#define T_Kod 313
#define T_Mssntp 314
#define T_Leap 315
#define T_Leapfile 316
#define T_Limited 317
#define T_Link 318
#define T_Logconfig 319
#define T_Logfile 320
#define T_Loopstats 321
#define T_Lowpriotrap 322
#define T_Manycastclient 323
#define T_Manycastserver 324
#define T_Mask 325
#define T_Maxclock 326
#define T_Maxdist 327
#define T_Maxhop 328
#define T_Maxpoll 329
#define T_Minclock 330
#define T_Mindist 331
#define T_Minimum 332
#define T_Minpoll 333
#define T_Minsane 334
#define T_Mode 335
#define T_Monitor 336
#define T_Month 337
#define T_Multicastclient 338
#define T_Nolink 339
#define T_Nomodify 340
#define T_None 341
#define T_Nopeer 342
#define T_Noquery 343
#define T_Noselect 344
#define T_Noserve 345
#define T_Notrap 346
#define T_Notrust 347
#define T_Ntp 348
#define T_Ntpport 349
#define T_NtpSignDsocket 350
#define T_Orphan 351
#define T_Panic 352
#define T_Peer 353
#define T_Peerstats 354
#define T_Phone 355
#define T_Pid 356
#define T_Pidfile 357
#define T_Pool 358
#define T_Port 359
#define T_Pps 360
#define T_Preempt 361
#define T_Prefer 362
#define T_Protostats 363
#define T_Pw 364
#define T_Qos 365
#define T_RandFile 366
#define T_Rawstats 367
#define T_Refid 368
#define T_Requestkey 369
#define T_Restrict 370
#define T_Revoke 371
#define T_Server 372
#define T_Setvar 373
#define T_Sign 374
#define T_Statistics 375
#define T_Stats 376
#define T_Statsdir 377
#define T_Step 378
#define T_Stepout 379
#define T_Stratum 380
#define T_String 381
#define T_Sysstats 382
#define T_Tick 383
#define T_Time1 384
#define T_Time2 385
#define T_Timingstats 386
#define T_Tinker 387
#define T_Tos 388
#define T_Trap 389
#define T_True 390
#define T_Trustedkey 391
#define T_Ttl 392
#define T_Type 393
#define T_Unconfig 394
#define T_Unpeer 395
#define T_Version 396
#define T_WanderThreshold 397
#define T_Week 398
#define T_Xleave 399
#define T_Year 400
#define T_Flag 401
#define T_Void 402
#define T_EOC 403
#define T_Simulate 404
#define T_Beep_Delay 405
#define T_Sim_Duration 406
#define T_Server_Offset 407
#define T_Duration 408
#define T_Freq_Offset 409
#define T_Wander 410
#define T_Jitter 411
#define T_Prop_Delay 412
#define T_Proc_Delay 413




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 44 "ntp_parser.y"

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
#line 385 "ntp_parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


