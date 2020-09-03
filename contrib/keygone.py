#! /usr/bin/env python
# -*- coding: utf-8 -*-
# SPDX-License-Identifier: BSD-2-Clause
'''keygone.py -- A cruddy improvised NTPsec keyfile generator

The number in the group_iterator range is the number of keys to generate
per key type.

key_number starts as the first key number and is incremented.

max_bytes is the implementation-specific maximum number of entropy bytes
allowed per key.

types_lengths_list is a list of lists. the inner lists store the output
length of the algorithm (block size?) and the standard form of the
algorithms name (in lowercase). Commented out algorithms are believed
broken, non-commented ones are not. This list believed accurate as of
the 6th of May 2020.
'''
import secrets

group_iterator = range(10)
key_number = 100
max_bytes = 32

types_lengths_list = [
    # [16, 'md5'],
     [16, 'aes-128'],
     [16, 'aria-128'],
     [16, 'camellia-128'],
     [16, 'sm4'],

    # [20, 'sha1'],
    # [20, 'rmd160'],

     [24, 'aes-192'],
     [24, 'aria-192'],
     [24, 'camellia-192'],

     [28, 'sha224'],
     [28, 'sha3-224'],

     [32, 'blake2s256'],
     [32, 'sha256'],
     [32, 'sha3-256'],
     [32, 'aes-256'],
     [32, 'aria-256'],
     [32, 'camellia-256'],

     [48, 'sha384'],
     [48, 'sha3-384'],

     [64, 'blake2b512'],
     [64, 'sha512'],
     [64, 'sha3-512']]

for type_length_tuple in types_lengths_list:
    length = min(type_length_tuple[0], max_bytes)
    for _ in group_iterator:
        print("%3d %12s\t%s" %
        (key_number, type_length_tuple[1],
         secrets.token_hex(length)))
        key_number += 1
