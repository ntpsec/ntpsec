/*
 *  This file defines the string_tokenize interface
 *
 *  string_tokenize copyright 2004 Bruce Korb
 *
 *  string_tokenize is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  
 *  string_tokenize is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with string_tokenize; if not, write to:
 *             The Free Software Foundation, Inc.,
 *             59 Temple Place - Suite 330,
 *             Boston,  MA  02111-1307, USA.
 */
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#define cc_t   const unsigned char
#define ch_t   unsigned char

#include "tokenize.h"
#ifndef NUL
#  define NUL '\0'
#endif

static void
copy_cooked( ch_t** ppDest, cc_t** ppSrc )
{
    ch_t* pDest = (ch_t*)*ppDest;
    const ch_t* pSrc  = (const ch_t*)(*ppSrc + 1);

    for (;;) {
        ch_t ch = *(pSrc++);
        switch (ch) {
        case NUL:   pSrc--; /* FALLTHROUGH */
        case '"':   goto done;
        case '\\':
            switch (*pSrc) {
            case '0':
            case '1':
            case '2':
            case '3':
                /*
                 *  One, two or three octal digits
                 */
                ch = (*(pSrc++)) - '0';
                if (! isdigit( *pSrc ))
                    break;
                if (*pSrc >= '8')
                    break;
                ch = (ch << 3) + (*(pSrc++)) - '0';
                if (! isdigit( *pSrc ))
                    break;
                if (*pSrc >= '8')
                    break;
                ch = (ch << 3) + (*(pSrc++)) - '0';
                break;

            case '4':
            case '5':
            case '6':
            case '7':
                /*
                 *  One or two octal digits
                 */
                ch = (*(pSrc++)) - '0';
                if (! isdigit( *pSrc ))
                    break;
                if (*pSrc >= '8')
                    break;
                ch = (ch << 3) + (*(pSrc++)) - '0';
                break;

            case 'x':
                if (! isxdigit( *++pSrc )) {
                    ch = 'x';
                    break;
                }
                {
                    ch_t chars[4];
                    chars[0] = *(pSrc++);
                    if (isxdigit( *pSrc )) {
                        chars[1] = *(pSrc++);
                        chars[2] = NUL;
                    } else {
                        chars[1] = NUL;
                    }
                    ch = (ch_t)strtol( (char*)chars, NULL, 16 );
                }
                break;

            case '\r':
                if (*(++pSrc) == '\n')
                    ++pSrc;
                continue;

            case '\n':
                ++pSrc;
                continue;

            case 'f': ch = '\f'; pSrc++; break;
            case 'n': ch = '\n'; pSrc++; break;
            case 'r': ch = '\r'; pSrc++; break;
            case 't': ch = '\t'; pSrc++; break;
            case 'v': ch = '\v'; pSrc++; break;
            case 'b': ch = '\b'; pSrc++; break;
            case 'a': ch = '\a'; pSrc++; break;

            case NUL:
                break;

            default:
                ch = *(pSrc++);
                break;
            }
            /* FALLTHROUGH */

        default:
            *(pDest++) = ch;
        }
    }

 done:
    *ppDest = (ch_t*)pDest; /* next spot for storing character */
    *ppSrc  = (ch_t*)pSrc;  /* char following closing quote    */
}


static void
copy_raw( ch_t** ppDest, cc_t** ppSrc )
{
    ch_t* pDest = *ppDest;
    cc_t* pSrc  = *ppSrc + 1;

    for (;;) {
        ch_t ch = *(pSrc++);
        switch (ch) {
        case NUL:   pSrc--; /* FALLTHROUGH */
        case '\'':  goto done;
        case '\\':
            /*
             *  *Four* escapes are handled:  newline removal, escape char
             *  quoting and apostrophe quoting
             */
            switch (*pSrc) {
            case '\r':
                if (*(++pSrc) == '\n')
                    ++pSrc;
                continue;

            case '\n':
                ++pSrc;
                continue;

            case '\'':
                ch = '\'';
                /* FALLTHROUGH */

            case '\\':
                ++pSrc;
                break;
            }
            /* FALLTHROUGH */

        default:
            *(pDest++) = ch;
        }
    }

 done:
    *ppDest = pDest; /* next spot for storing character */
    *ppSrc  = pSrc;  /* char following closing quote    */
}


token_list_t*
string_tokenize( const char* str )
{
    int max_token_ct = 1; /* allow for trailing NUL on string */
    token_list_t* res;

    /*
     *  Trim leading white space.  Use "ENOENT" and a NULL return to indicate
     *  an empty string was passed.
     */
    while (isspace( *str ))  str++;
    if (*str == NUL) {
        errno = ENOENT;
        return NULL;
    }

    /*
     *  Take an approximate count of tokens.  If no quoted strings are used,
     *  it will be accurate.  If quoted strings are used, it will be a little
     *  high and we'll squander the space for a few extra pointers.
     */
    {
        cc_t* pz = (cc_t*)str;

        do {
            max_token_ct++;
            while (! isspace( *++pz ))
                if (*pz == NUL) goto found_nul;
            while (isspace( *pz ))  pz++;
        } while (*pz != NUL);

    found_nul:
        ;
    }

    res = malloc( sizeof(*res) + strlen(str) + (max_token_ct * sizeof(ch_t*)) );
    if (res == NULL) {
        errno = ENOMEM;
        return res;
    }

    /*
     *  Now copy each token into the output buffer.
     */
    {
        ch_t* pzDest = (ch_t*)(res->tkn_list + (max_token_ct + 1));
        res->tkn_ct  = 0;

        do  {
            res->tkn_list[ res->tkn_ct++ ] = pzDest;
            for (;;) {
                char ch = *str;
                if (isspace( ch )) {
                found_white_space:
                    while (isspace( *++str ))  ;
                    break;
                }

                switch (ch) {
                case '"':
                    copy_cooked( &pzDest, (cc_t**)&str );
                    if (isspace( *str ))
                        goto found_white_space;
                    break;

                case '\'':
                    copy_raw( &pzDest, (cc_t**)&str );
                    if (isspace( *str ))
                        goto found_white_space;
                    break;

                case NUL:
                    goto copy_done;

                default:
                    str++;
                    *(pzDest++) = ch;
                }
            } copy_done:;

            /*
             * NUL terminate the last token and see if we have any more tokens.
             */
            *(pzDest++) = NUL;
        } while (*str != NUL);

        res->tkn_list[ res->tkn_ct ] = NULL;
    }

    return res;
}

#ifdef TEST
#include <stdio.h>
#include <string.h>

int
main( int argc, char** argv )
{
    if (argc == 1) {
        printf("USAGE:  %s arg [ ... ]\n", *argv);
        return 1;
    }
    while (--argc > 0) {
        char* arg = *(++argv);
        token_list_t* p = string_tokenize( arg );
        if (p == NULL) {
            printf( "Parsing string ``%s'' failed:\n\terrno %d (%s)\n",
                    arg, errno, strerror( errno ));
        } else {
            int ix = 0;
            printf( "Parsed string ``%s''\ninto %d tokens:\n", arg, p->tkn_ct );
            do {
                printf( " %3d:  ``%s''\n", ix+1, p->tkn_list[ix] );
            } while (++ix < p->tkn_ct);
            free(p);
        }
    }
    return 0;
}
#endif

/*
 * Local Variables:
 * mode: C
 * c-file-style: "stroustrup"
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 * end of agen5/autogen.c */
