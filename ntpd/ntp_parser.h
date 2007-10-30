/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     T_Broadcast = 266,
     T_Broadcastclient = 267,
     T_Broadcastdelay = 268,
     T_Burst = 269,
     T_Calibrate = 270,
     T_Calldelay = 271,
     T_Ceiling = 272,
     T_Clockstats = 273,
     T_Cohort = 274,
     T_ControlKey = 275,
     T_Crypto = 276,
     T_Cryptostats = 277,
     T_Day = 278,
     T_Default = 279,
     T_Disable = 280,
     T_Discard = 281,
     T_Dispersion = 282,
     T_Double = 283,
     T_Driftfile = 284,
     T_WanderThreshold = 285,
     T_Enable = 286,
     T_End = 287,
     T_False = 288,
     T_File = 289,
     T_Filegen = 290,
     T_Flag1 = 291,
     T_Flag2 = 292,
     T_Flag3 = 293,
     T_Flag4 = 294,
     T_Floor = 295,
     T_Freq = 296,
     T_Fudge = 297,
     T_Host = 298,
     T_Huffpuff = 299,
     T_IPv4_address = 300,
     T_IPv6_address = 301,
     T_Iburst = 302,
     T_Ident = 303,
     T_Ignore = 304,
     T_Includefile = 305,
     T_Integer = 306,
     T_Interface = 307,
     T_Kernel = 308,
     T_Key = 309,
     T_Keys = 310,
     T_Keysdir = 311,
     T_Kod = 312,
     T_Leap = 313,
     T_Leapfile = 314,
     T_Limited = 315,
     T_Link = 316,
     T_Logconfig = 317,
     T_Logfile = 318,
     T_Loopstats = 319,
     T_Lowpriotrap = 320,
     T_Manycastclient = 321,
     T_Manycastserver = 322,
     T_Mask = 323,
     T_Maxclock = 324,
     T_Maxdist = 325,
     T_Maxhop = 326,
     T_Maxpoll = 327,
     T_Minclock = 328,
     T_Mindist = 329,
     T_Minimum = 330,
     T_Minpoll = 331,
     T_Minsane = 332,
     T_Mode = 333,
     T_Monitor = 334,
     T_Month = 335,
     T_Multicastclient = 336,
     T_Nolink = 337,
     T_Nomodify = 338,
     T_None = 339,
     T_Nopeer = 340,
     T_Noquery = 341,
     T_Noselect = 342,
     T_Noserve = 343,
     T_Notrap = 344,
     T_Notrust = 345,
     T_Novolley = 346,
     T_Ntp = 347,
     T_Ntpport = 348,
     T_Orphan = 349,
     T_Panic = 350,
     T_Peer = 351,
     T_Peerstats = 352,
     T_Phone = 353,
     T_Pid = 354,
     T_Pidfile = 355,
     T_Pool = 356,
     T_Port = 357,
     T_Pps = 358,
     T_Preempt = 359,
     T_Prefer = 360,
     T_Pw = 361,
     T_Qos = 362,
     T_RandFile = 363,
     T_Rawstats = 364,
     T_Refid = 365,
     T_Requestkey = 366,
     T_Restrict = 367,
     T_Revoke = 368,
     T_Server = 369,
     T_Setvar = 370,
     T_Sign = 371,
     T_Statistics = 372,
     T_Stats = 373,
     T_Statsdir = 374,
     T_Step = 375,
     T_Stepout = 376,
     T_Stratum = 377,
     T_String = 378,
     T_Sysstats = 379,
     T_Tick = 380,
     T_Time1 = 381,
     T_Time2 = 382,
     T_Tinker = 383,
     T_Tos = 384,
     T_Trap = 385,
     T_True = 386,
     T_Trustedkey = 387,
     T_Ttl = 388,
     T_Type = 389,
     T_Version = 390,
     T_Week = 391,
     T_Year = 392,
     T_Flag = 393,
     T_Void = 394,
     T_EOC = 395,
     T_Simulate = 396,
     T_Beep_Delay = 397,
     T_Sim_Duration = 398,
     T_Server_Offset = 399,
     T_Duration = 400,
     T_Freq_Offset = 401,
     T_Wander = 402,
     T_Jitter = 403,
     T_Prop_Delay = 404,
     T_Proc_Delay = 405
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
#define T_Broadcast 266
#define T_Broadcastclient 267
#define T_Broadcastdelay 268
#define T_Burst 269
#define T_Calibrate 270
#define T_Calldelay 271
#define T_Ceiling 272
#define T_Clockstats 273
#define T_Cohort 274
#define T_ControlKey 275
#define T_Crypto 276
#define T_Cryptostats 277
#define T_Day 278
#define T_Default 279
#define T_Disable 280
#define T_Discard 281
#define T_Dispersion 282
#define T_Double 283
#define T_Driftfile 284
#define T_WanderThreshold 285
#define T_Enable 286
#define T_End 287
#define T_False 288
#define T_File 289
#define T_Filegen 290
#define T_Flag1 291
#define T_Flag2 292
#define T_Flag3 293
#define T_Flag4 294
#define T_Floor 295
#define T_Freq 296
#define T_Fudge 297
#define T_Host 298
#define T_Huffpuff 299
#define T_IPv4_address 300
#define T_IPv6_address 301
#define T_Iburst 302
#define T_Ident 303
#define T_Ignore 304
#define T_Includefile 305
#define T_Integer 306
#define T_Interface 307
#define T_Kernel 308
#define T_Key 309
#define T_Keys 310
#define T_Keysdir 311
#define T_Kod 312
#define T_Leap 313
#define T_Leapfile 314
#define T_Limited 315
#define T_Link 316
#define T_Logconfig 317
#define T_Logfile 318
#define T_Loopstats 319
#define T_Lowpriotrap 320
#define T_Manycastclient 321
#define T_Manycastserver 322
#define T_Mask 323
#define T_Maxclock 324
#define T_Maxdist 325
#define T_Maxhop 326
#define T_Maxpoll 327
#define T_Minclock 328
#define T_Mindist 329
#define T_Minimum 330
#define T_Minpoll 331
#define T_Minsane 332
#define T_Mode 333
#define T_Monitor 334
#define T_Month 335
#define T_Multicastclient 336
#define T_Nolink 337
#define T_Nomodify 338
#define T_None 339
#define T_Nopeer 340
#define T_Noquery 341
#define T_Noselect 342
#define T_Noserve 343
#define T_Notrap 344
#define T_Notrust 345
#define T_Novolley 346
#define T_Ntp 347
#define T_Ntpport 348
#define T_Orphan 349
#define T_Panic 350
#define T_Peer 351
#define T_Peerstats 352
#define T_Phone 353
#define T_Pid 354
#define T_Pidfile 355
#define T_Pool 356
#define T_Port 357
#define T_Pps 358
#define T_Preempt 359
#define T_Prefer 360
#define T_Pw 361
#define T_Qos 362
#define T_RandFile 363
#define T_Rawstats 364
#define T_Refid 365
#define T_Requestkey 366
#define T_Restrict 367
#define T_Revoke 368
#define T_Server 369
#define T_Setvar 370
#define T_Sign 371
#define T_Statistics 372
#define T_Stats 373
#define T_Statsdir 374
#define T_Step 375
#define T_Stepout 376
#define T_Stratum 377
#define T_String 378
#define T_Sysstats 379
#define T_Tick 380
#define T_Time1 381
#define T_Time2 382
#define T_Tinker 383
#define T_Tos 384
#define T_Trap 385
#define T_True 386
#define T_Trustedkey 387
#define T_Ttl 388
#define T_Type 389
#define T_Version 390
#define T_Week 391
#define T_Year 392
#define T_Flag 393
#define T_Void 394
#define T_EOC 395
#define T_Simulate 396
#define T_Beep_Delay 397
#define T_Sim_Duration 398
#define T_Server_Offset 399
#define T_Duration 400
#define T_Freq_Offset 401
#define T_Wander 402
#define T_Jitter 403
#define T_Prop_Delay 404
#define T_Proc_Delay 405




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 49 "ntp_parser.y"
{
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
}
/* Line 1529 of yacc.c.  */
#line 364 "../../ntpd/ntp_parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

