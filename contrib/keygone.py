#! /usr/bin/env python
# -*- coding: utf-8 -*-
# SPDX-License-Identifier: BSD-2-Clause
'''keygone.py -- A cruddy improvised NTPsec keyfile generator

The number in the group_iterator range is the number of keys to generate
per key type.

key_number starts as the first key number and is incremented.

max_bytes is the implementation-specific maximum number of entropy bytes
allowed per key.

the variables list_* are lists of lists. the inner lists store the output
length of the algorithm (block size?) and the standard form of the
algorithms name (in lowercase). *_bad list algorithms are believed
broken, *_good ones are not. This list believed accurate as of the
6th of May 2020.
'''
import argparse
import os
import secrets
import stat
import sys

list_md_bad = [
    [16, 16, 'md5'],

    [20, 16, 'sha1'],
    [20, 16, 'rmd160']
]
list_md_good = [
    [16, 16, 'sm4'],

    [28, 16, 'sha224'],
    [28, 16, 'sha3-224'],

    [32, 16, 'blake2s256'],
    [32, 16, 'sha256'],
    [32, 16, 'sha3-256'],

    [48, 16, 'sha384'],
    [48, 16, 'sha3-384'],

    [64, 16, 'blake2b512'],
    [64, 16, 'sha512'],
    [64, 16,  'sha3-512']
]
list_cmac_bad = [
]
list_cmac_good = [
    [16, 16, 'aes-128'],
    [16, 16, 'aria-128'],
    [16, 16, 'camellia-128'],

    [24, 24, 'aes-192'],
    [24, 24, 'aria-192'],
    [24, 24, 'camellia-192'],

    [32, 32, 'aes-256'],
    [32, 32, 'aria-256'],
    [32, 32, 'camellia-256'],
]


class KeyGone():
    'Set up, generate and output keys for NTPsec.'

    def __init__(self, start: int, gap: int):
        'Set up key generation for NTPsec.'
        self.gap = min(0, gap)
        self.index = start
        self.backing = {}

    def __str__(self):
        'Return a string containing the generated keys.'
        _ = ''
        for row in self.backing:
            _ += '%5d\t%13s\t%s\n' % (row, *self.backing[row])
        return _

    def to_file(self, oname: str):
        'Write the current keys to a file.'
        with open(oname, 'w') as of:
            of.write(str(self))

    def do_link(self, oname: str, link: str):
        'Write the current keys to a file and link to it elsewhere.'
        orig_umask = os.umask(stat.S_IWGRP | stat.S_IRWXO)
        os.umask(orig_umask)
        self.to_file(oname)
        if os.path.exists(link):
            os.remove(link)    # The symlink() line below matters
        os.symlink(oname, link)

    def add(self, algo: list, keys: int, hexed: bool = False):
        'Generate a slew of new keys according to specs.'
        length, mlength, algor = algo
        newlength = min(length, 32 if hexed else 20)  # Yuck, hardcoded magic
        if mlength > newlength:
            sys.stderr.write('"%s" excluded because minimum length %d exceeds truncation %d\n' % (algor, mlength, newlength))
            return
        for _ in range(keys):
            self.backing[self.index] = [algor, self._gen_key(newlength, hexed)]
            self.index += 1
        self.index += self.gap

    def _gen_key(self, length: int, hexed: bool) -> str:
        'Generate a single key.'
        if hexed:
            return secrets.token_hex(length)
        result = ''
        for _ in range(length):
            result += chr(0x24 + secrets.randbelow(0x5a))
        return result


def list_algos():
    'List the available algorithms by buckets.'
    chunks = []
    iterable = (('bad CMAC algos:', list_cmac_bad),
                ('good CMAC algos:', list_cmac_good),
                ('bad digest algos:', list_md_bad),
                ('good digest algos:', list_md_good)
                )
    for label, things in iterable:
        _ = label + '\n'
        for thing in things:
            _ += '%3dbit\t%s\n' % (thing[0] * 8, thing[1])
        chunks.append(_)
    sys.stderr.write('\n'.join(chunks))
    sys.exit(0)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='NTPsec key generator.')
    parser.add_argument('-L', '--list', action='store_true',
                        default=False, help='List known algorithms')
    parser.add_argument('-d', '--delete', nargs='+',
                        help='delete algorithm (repeatable) or "everything"')
    parser.add_argument('-a', '--add', nargs='+',
                        help='delete algorithm (repeatable) or "everything"')
    parser.add_argument('-f', '--file', help='Output to a file defaults to "ntp.keygone"',
                        default='ntp.keygone')
    parser.add_argument(
        '-s', '--link', help='create a symlink (requires file)')
    parser.add_argument('-c', '--console', action='store_true',
                        default=False, help='output to console')
    parser.add_argument('-n', '--number', type=int,
                        default=10, help='number of keys per group')
    parser.add_argument('-i', '--initial', type=int,
                        default=1, help='number of initial key')
    parser.add_argument('-g', '--gap', type=int, default=0,
                        help='gap between subsequent groups')
    parser.add_argument('-t', '--text', dest='fmt', action='store_false',
                        default=False, help='generate text keys (default)')
    parser.add_argument('-x', '--hex', dest='fmt',
                        action='store_true', help='generate hexadecimal keys')
    args = parser.parse_args()
    if args.list:
        list_algos()
    kg = KeyGone(args.initial, args.gap)
    set_cur = set(map(lambda x: x[2], list_cmac_good + list_md_good))
    set_all = set(map(lambda x: x[2], list_cmac_bad + list_md_bad))
    set_all.update(set_cur)
    if isinstance(args.delete, str):
        args.delete = [args.delete]
    elif args.delete is None:
        args.delete = []
    for iteM in list(args.delete):
        item = iteM.lower()
        if item in set_cur:
            set_cur.remove(item)
        elif item == 'everything':
            set_cur = set()
        else:
            sys.stderr.write('"%s" not in algos' % item)
    if isinstance(args.add, str):
        args.add = [args.add]
    elif args.add is None:
        args.add = []
    for iteM in list(args.add):
        item = iteM.lower()
        if item in set_cur:
            sys.stderr.write('"%s" already in algos' % item)
        if item in set_all:
            set_cur.add(item)
        elif item == 'everything':
            set_cur = set_all.copy()
        else:
            sys.stderr.write('"%s" not in available algos' % item)
    algos = list_cmac_good + list_md_good + list_cmac_bad + list_md_bad
    fail = False
    if args.initial < 1:
        fail = True
        sys.stderr.write('"initial" key number must be a positive integer')
    if args.number < 1:
        fail = True
        sys.stderr.write('"number" of keys per group must be a positive integer')
    if args.gap < 0:
        fail = True
        sys.stderr.write('the "gap" between groups must be non-negative integer')
    if len(algos) < 1:
        fail = True
        sys.stderr.write('At least one algorithm must be specified')
    if args.initial + (args.gap + args.number) * len(algos) - args.gap > 65535:
        fail = True
        sys.stderr.write('End number must be less than 65,536')
    if fail:
        sys.exit(1)
    for algo in algos:
        if algo[2] in set_cur:
            kg.add(algo, args.number, args.fmt)
    if args.file is not None:
        if args.link is not None:
            kg.do_link(args.file, args.link)
        else:
            kg.to_file(args.file)
    if args.console:
        print(str(kg))
