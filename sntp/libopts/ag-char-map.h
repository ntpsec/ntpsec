/*
 *   Character mapping generated 02/26/12 11:08:40
 *
 *  This file contains the character classifications
 *  used by AutoGen and AutoOpts for identifying tokens.
 *  The table is static scope, so %guard is empty.
 *
 *  This file is part of AutoGen.
 *  Copyright (c) 1992-2012 Bruce Korb - all rights reserved
 *
 *  AutoGen is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  AutoGen is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 *  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AG_CHAR_MAP_H_GUARD
#define AG_CHAR_MAP_H_GUARD 1

#ifdef HAVE_CONFIG_H
# if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>

# elif defined(HAVE_STDINT_H)
#   include <stdint.h>

# elif !defined(HAVE_UINT32_T)
#   if SIZEOF_INT == 4
      typedef unsigned int    uint32_t;
#   elif SIZEOF_LONG == 4
      typedef unsigned long   uint32_t;
#   endif
# endif /* HAVE_*INT*_H header */

#else /* not HAVE_CONFIG_H -- */
# include <inttypes.h>
#endif /* HAVE_CONFIG_H */

#if 0 /* mapping specification source (from autogen.map) */
// 
// %guard
// %file           ag-char-map.h
// 
// %comment -- see above
// %
// 
// lower-case      "a-z"
// upper-case      "A-Z"
// alphabetic      +lower-case   +upper-case
// oct-digit       "0-7"
// dec-digit       "89"          +oct-digit
// hex-digit       "a-fA-F"      +dec-digit
// alphanumeric    +alphabetic   +dec-digit
// var-first       "_"           +alphabetic
// variable-name   +var-first    +dec-digit
// option-name     "^-"          +variable-name
// value-name      ":"           +option-name
// horiz-white     "\t "
// name-sep        "[.]"
// compound-name   +value-name   +name-sep +horiz-white
// whitespace      "\v\f\r\n\b"  +horiz-white
// unquotable      "!-~"         -"\"#(),;<=>[\\]`{}?*'"
// end-xml-token   "/>"          +whitespace
// graphic         "!-~"
// plus-n-space    "+"           +whitespace
// punctuation     "!-~"         -alphanumeric -"_"
// suffix          "-._"         +alphanumeric
// suffix-fmt      "%/"          +suffix     
// false-type      "nNfF0\x00"
// file-name       "/"           +suffix
// end-token       "\x00"        +whitespace
// end-list-entry  ","           +end-token
// set-separator   "|+"          +end-list-entry
//
#endif /* 0 -- mapping spec. source */


typedef uint32_t ag_char_map_mask_t;

#define  IS_LOWER_CASE_CHAR( _c)       is_ag_char_map_char((char)( _c), 0x000001)
#define SPN_LOWER_CASE_CHARS(_s)      spn_ag_char_map_chars((char *)_s, 0x000001)
#define BRK_LOWER_CASE_CHARS(_s)      brk_ag_char_map_chars((char *)_s, 0x000001)
#define  IS_UPPER_CASE_CHAR( _c)       is_ag_char_map_char((char)( _c), 0x000002)
#define SPN_UPPER_CASE_CHARS(_s)      spn_ag_char_map_chars((char *)_s, 0x000002)
#define BRK_UPPER_CASE_CHARS(_s)      brk_ag_char_map_chars((char *)_s, 0x000002)
#define  IS_ALPHABETIC_CHAR( _c)       is_ag_char_map_char((char)( _c), 0x000003)
#define SPN_ALPHABETIC_CHARS(_s)      spn_ag_char_map_chars((char *)_s, 0x000003)
#define BRK_ALPHABETIC_CHARS(_s)      brk_ag_char_map_chars((char *)_s, 0x000003)
#define  IS_OCT_DIGIT_CHAR( _c)        is_ag_char_map_char((char)( _c), 0x000004)
#define SPN_OCT_DIGIT_CHARS(_s)       spn_ag_char_map_chars((char *)_s, 0x000004)
#define BRK_OCT_DIGIT_CHARS(_s)       brk_ag_char_map_chars((char *)_s, 0x000004)
#define  IS_DEC_DIGIT_CHAR( _c)        is_ag_char_map_char((char)( _c), 0x00000C)
#define SPN_DEC_DIGIT_CHARS(_s)       spn_ag_char_map_chars((char *)_s, 0x00000C)
#define BRK_DEC_DIGIT_CHARS(_s)       brk_ag_char_map_chars((char *)_s, 0x00000C)
#define  IS_HEX_DIGIT_CHAR( _c)        is_ag_char_map_char((char)( _c), 0x00001C)
#define SPN_HEX_DIGIT_CHARS(_s)       spn_ag_char_map_chars((char *)_s, 0x00001C)
#define BRK_HEX_DIGIT_CHARS(_s)       brk_ag_char_map_chars((char *)_s, 0x00001C)
#define  IS_ALPHANUMERIC_CHAR( _c)     is_ag_char_map_char((char)( _c), 0x00000F)
#define SPN_ALPHANUMERIC_CHARS(_s)    spn_ag_char_map_chars((char *)_s, 0x00000F)
#define BRK_ALPHANUMERIC_CHARS(_s)    brk_ag_char_map_chars((char *)_s, 0x00000F)
#define  IS_VAR_FIRST_CHAR( _c)        is_ag_char_map_char((char)( _c), 0x000023)
#define SPN_VAR_FIRST_CHARS(_s)       spn_ag_char_map_chars((char *)_s, 0x000023)
#define BRK_VAR_FIRST_CHARS(_s)       brk_ag_char_map_chars((char *)_s, 0x000023)
#define  IS_VARIABLE_NAME_CHAR( _c)    is_ag_char_map_char((char)( _c), 0x00002F)
#define SPN_VARIABLE_NAME_CHARS(_s)   spn_ag_char_map_chars((char *)_s, 0x00002F)
#define BRK_VARIABLE_NAME_CHARS(_s)   brk_ag_char_map_chars((char *)_s, 0x00002F)
#define  IS_OPTION_NAME_CHAR( _c)      is_ag_char_map_char((char)( _c), 0x00006F)
#define SPN_OPTION_NAME_CHARS(_s)     spn_ag_char_map_chars((char *)_s, 0x00006F)
#define BRK_OPTION_NAME_CHARS(_s)     brk_ag_char_map_chars((char *)_s, 0x00006F)
#define  IS_VALUE_NAME_CHAR( _c)       is_ag_char_map_char((char)( _c), 0x0000EF)
#define SPN_VALUE_NAME_CHARS(_s)      spn_ag_char_map_chars((char *)_s, 0x0000EF)
#define BRK_VALUE_NAME_CHARS(_s)      brk_ag_char_map_chars((char *)_s, 0x0000EF)
#define  IS_HORIZ_WHITE_CHAR( _c)      is_ag_char_map_char((char)( _c), 0x000100)
#define SPN_HORIZ_WHITE_CHARS(_s)     spn_ag_char_map_chars((char *)_s, 0x000100)
#define BRK_HORIZ_WHITE_CHARS(_s)     brk_ag_char_map_chars((char *)_s, 0x000100)
#define  IS_NAME_SEP_CHAR( _c)         is_ag_char_map_char((char)( _c), 0x000200)
#define SPN_NAME_SEP_CHARS(_s)        spn_ag_char_map_chars((char *)_s, 0x000200)
#define BRK_NAME_SEP_CHARS(_s)        brk_ag_char_map_chars((char *)_s, 0x000200)
#define  IS_COMPOUND_NAME_CHAR( _c)    is_ag_char_map_char((char)( _c), 0x0003EF)
#define SPN_COMPOUND_NAME_CHARS(_s)   spn_ag_char_map_chars((char *)_s, 0x0003EF)
#define BRK_COMPOUND_NAME_CHARS(_s)   brk_ag_char_map_chars((char *)_s, 0x0003EF)
#define  IS_WHITESPACE_CHAR( _c)       is_ag_char_map_char((char)( _c), 0x000500)
#define SPN_WHITESPACE_CHARS(_s)      spn_ag_char_map_chars((char *)_s, 0x000500)
#define BRK_WHITESPACE_CHARS(_s)      brk_ag_char_map_chars((char *)_s, 0x000500)
#define  IS_UNQUOTABLE_CHAR( _c)       is_ag_char_map_char((char)( _c), 0x000800)
#define SPN_UNQUOTABLE_CHARS(_s)      spn_ag_char_map_chars((char *)_s, 0x000800)
#define BRK_UNQUOTABLE_CHARS(_s)      brk_ag_char_map_chars((char *)_s, 0x000800)
#define  IS_END_XML_TOKEN_CHAR( _c)    is_ag_char_map_char((char)( _c), 0x001500)
#define SPN_END_XML_TOKEN_CHARS(_s)   spn_ag_char_map_chars((char *)_s, 0x001500)
#define BRK_END_XML_TOKEN_CHARS(_s)   brk_ag_char_map_chars((char *)_s, 0x001500)
#define  IS_GRAPHIC_CHAR( _c)          is_ag_char_map_char((char)( _c), 0x002000)
#define SPN_GRAPHIC_CHARS(_s)         spn_ag_char_map_chars((char *)_s, 0x002000)
#define BRK_GRAPHIC_CHARS(_s)         brk_ag_char_map_chars((char *)_s, 0x002000)
#define  IS_PLUS_N_SPACE_CHAR( _c)     is_ag_char_map_char((char)( _c), 0x004500)
#define SPN_PLUS_N_SPACE_CHARS(_s)    spn_ag_char_map_chars((char *)_s, 0x004500)
#define BRK_PLUS_N_SPACE_CHARS(_s)    brk_ag_char_map_chars((char *)_s, 0x004500)
#define  IS_PUNCTUATION_CHAR( _c)      is_ag_char_map_char((char)( _c), 0x008000)
#define SPN_PUNCTUATION_CHARS(_s)     spn_ag_char_map_chars((char *)_s, 0x008000)
#define BRK_PUNCTUATION_CHARS(_s)     brk_ag_char_map_chars((char *)_s, 0x008000)
#define  IS_SUFFIX_CHAR( _c)           is_ag_char_map_char((char)( _c), 0x01000F)
#define SPN_SUFFIX_CHARS(_s)          spn_ag_char_map_chars((char *)_s, 0x01000F)
#define BRK_SUFFIX_CHARS(_s)          brk_ag_char_map_chars((char *)_s, 0x01000F)
#define  IS_SUFFIX_FMT_CHAR( _c)       is_ag_char_map_char((char)( _c), 0x03000F)
#define SPN_SUFFIX_FMT_CHARS(_s)      spn_ag_char_map_chars((char *)_s, 0x03000F)
#define BRK_SUFFIX_FMT_CHARS(_s)      brk_ag_char_map_chars((char *)_s, 0x03000F)
#define  IS_FALSE_TYPE_CHAR( _c)       is_ag_char_map_char((char)( _c), 0x040000)
#define SPN_FALSE_TYPE_CHARS(_s)      spn_ag_char_map_chars((char *)_s, 0x040000)
#define BRK_FALSE_TYPE_CHARS(_s)      brk_ag_char_map_chars((char *)_s, 0x040000)
#define  IS_FILE_NAME_CHAR( _c)        is_ag_char_map_char((char)( _c), 0x09000F)
#define SPN_FILE_NAME_CHARS(_s)       spn_ag_char_map_chars((char *)_s, 0x09000F)
#define BRK_FILE_NAME_CHARS(_s)       brk_ag_char_map_chars((char *)_s, 0x09000F)
#define  IS_END_TOKEN_CHAR( _c)        is_ag_char_map_char((char)( _c), 0x100500)
#define SPN_END_TOKEN_CHARS(_s)       spn_ag_char_map_chars((char *)_s, 0x100500)
#define BRK_END_TOKEN_CHARS(_s)       brk_ag_char_map_chars((char *)_s, 0x100500)
#define  IS_END_LIST_ENTRY_CHAR( _c)   is_ag_char_map_char((char)( _c), 0x300500)
#define SPN_END_LIST_ENTRY_CHARS(_s)  spn_ag_char_map_chars((char *)_s, 0x300500)
#define BRK_END_LIST_ENTRY_CHARS(_s)  brk_ag_char_map_chars((char *)_s, 0x300500)
#define  IS_SET_SEPARATOR_CHAR( _c)    is_ag_char_map_char((char)( _c), 0x700500)
#define SPN_SET_SEPARATOR_CHARS(_s)   spn_ag_char_map_chars((char *)_s, 0x700500)
#define BRK_SET_SEPARATOR_CHARS(_s)   brk_ag_char_map_chars((char *)_s, 0x700500)

static ag_char_map_mask_t const ag_char_map_table[128] = {
  /*NUL*/ 0x140000, /*x01*/ 0x000000, /*x02*/ 0x000000, /*x03*/ 0x000000,
  /*x04*/ 0x000000, /*x05*/ 0x000000, /*x06*/ 0x000000, /*BEL*/ 0x000000,
  /* BS*/ 0x000400, /* HT*/ 0x000100, /* NL*/ 0x000400, /* VT*/ 0x000400,
  /* FF*/ 0x000400, /* CR*/ 0x000400, /*x0E*/ 0x000000, /*x0F*/ 0x000000,
  /*x10*/ 0x000000, /*x11*/ 0x000000, /*x12*/ 0x000000, /*x13*/ 0x000000,
  /*x14*/ 0x000000, /*x15*/ 0x000000, /*x16*/ 0x000000, /*x17*/ 0x000000,
  /*x18*/ 0x000000, /*x19*/ 0x000000, /*x1A*/ 0x000000, /*ESC*/ 0x000000,
  /*x1C*/ 0x000000, /*x1D*/ 0x000000, /*x1E*/ 0x000000, /*x1F*/ 0x000000,
  /*   */ 0x000100, /* ! */ 0x00A800, /* " */ 0x00A000, /* # */ 0x00A000,
  /* $ */ 0x00A800, /* % */ 0x02A800, /* & */ 0x00A800, /* ' */ 0x00A000,
  /* ( */ 0x00A000, /* ) */ 0x00A000, /* * */ 0x00A000, /* + */ 0x40E800,
  /* , */ 0x20A000, /* - */ 0x01A840, /* . */ 0x01AA00, /* / */ 0x0AB800,
  /* 0 */ 0x042804, /* 1 */ 0x002804, /* 2 */ 0x002804, /* 3 */ 0x002804,
  /* 4 */ 0x002804, /* 5 */ 0x002804, /* 6 */ 0x002804, /* 7 */ 0x002804,
  /* 8 */ 0x002808, /* 9 */ 0x002808, /* : */ 0x00A880, /* ; */ 0x00A000,
  /* < */ 0x00A000, /* = */ 0x00A000, /* > */ 0x00B000, /* ? */ 0x00A000,
  /* @ */ 0x00A800, /* A */ 0x002812, /* B */ 0x002812, /* C */ 0x002812,
  /* D */ 0x002812, /* E */ 0x002812, /* F */ 0x042812, /* G */ 0x002802,
  /* H */ 0x002802, /* I */ 0x002802, /* J */ 0x002802, /* K */ 0x002802,
  /* L */ 0x002802, /* M */ 0x002802, /* N */ 0x042802, /* O */ 0x002802,
  /* P */ 0x002802, /* Q */ 0x002802, /* R */ 0x002802, /* S */ 0x002802,
  /* T */ 0x002802, /* U */ 0x002802, /* V */ 0x002802, /* W */ 0x002802,
  /* X */ 0x002802, /* Y */ 0x002802, /* Z */ 0x002802, /* [ */ 0x00A200,
  /* \ */ 0x00A000, /* ] */ 0x00A200, /* ^ */ 0x00A840, /* _ */ 0x012820,
  /* ` */ 0x00A000, /* a */ 0x002811, /* b */ 0x002811, /* c */ 0x002811,
  /* d */ 0x002811, /* e */ 0x002811, /* f */ 0x042811, /* g */ 0x002801,
  /* h */ 0x002801, /* i */ 0x002801, /* j */ 0x002801, /* k */ 0x002801,
  /* l */ 0x002801, /* m */ 0x002801, /* n */ 0x042801, /* o */ 0x002801,
  /* p */ 0x002801, /* q */ 0x002801, /* r */ 0x002801, /* s */ 0x002801,
  /* t */ 0x002801, /* u */ 0x002801, /* v */ 0x002801, /* w */ 0x002801,
  /* x */ 0x002801, /* y */ 0x002801, /* z */ 0x002801, /* { */ 0x00A000,
  /* | */ 0x40A800, /* } */ 0x00A000, /* ~ */ 0x00A800, /*x7F*/ 0x000000
};
static inline int
is_ag_char_map_char(char ch, ag_char_map_mask_t mask)
{
    unsigned int ix = (unsigned char)ch;
    return ((ix < 128) && ((ag_char_map_table[ix] & mask) != 0));
}

static inline char *
spn_ag_char_map_chars(char * p, ag_char_map_mask_t mask)
{
    while ((*p != '\0') && is_ag_char_map_char(*p, mask))  p++;
    return p;
}

static inline char *
brk_ag_char_map_chars(char * p, ag_char_map_mask_t mask)
{
    while ((*p != '\0') && (! is_ag_char_map_char(*p, mask)))  p++;
    return p;
}
#endif /* AG_CHAR_MAP_H_GUARD */
