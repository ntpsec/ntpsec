#!/usr/bin/env python

"""Find all Pythons in the command path, and corresponding exe/lib locations.

An optional argument specifies an SSH host (or user@host) to use for the run.
Note that the ssh-based version will use the system-defult PATH rather than
the PATH set up by any login scripts, so it may not find all Pythons that the
user would see.

It is known to work with all versions of Python >=2.4.
"""

import glob
import operator
import os
import subprocess
import sys
import traceback

try:
    reduce
except NameError:  # For Python 3
    from functools import reduce  # pylint: disable=redefined-builtin

PYTHON_PATTERNS = ['python', 'python[1-9]', 'python[1-9].[0-9]']

PYTHON_COMMAND_MAP = [
    ['', 'import sys'],
    ['', 'from distutils import sysconfig'],
    ['exec', 'print(sys.executable)'],
    ['lib-noarch', 'print(sysconfig.get_python_lib(plat_specific=0))'],
    ['lib-arch', 'print(sysconfig.get_python_lib(plat_specific=1))'],
    ['sys-prefix', 'print(sys.prefix)'],
    ['std-prefix', 'print(sysconfig.PREFIX)'],
    ['exec-prefix', 'print(sysconfig.EXEC_PREFIX)'],
    ]

PYTHON_COMMANDS = [xx[1] for xx in PYTHON_COMMAND_MAP]
PYTHON_VALUE_NAMES = [xx[0] for xx in PYTHON_COMMAND_MAP if xx[0]]
PYTHON_PREFIX_NAMES = 'sys-prefix std-prefix exec-prefix'.split()


class BadReturn(Exception):
    """Bad return from subprocess."""
    pass


def _print(arg):
    """Python3-compatible print, without depending on future import."""
    print(arg)  # pylint: disable=superfluous-parens


def GetPaths():
    """Get list of directories in PATH."""
    return os.environ['PATH'].split(':')


def MakePatterns(paths, patterns):
    """Construct Cartesian product of paths and file patterns."""
    return [os.path.join(path, pat) for path in paths for pat in patterns]


def FileList(patterns):
    """Get list of files from list of glob patterns."""
    return reduce(operator.add, map(glob.glob, patterns), [])


def ExeFilter(files):
    """Filter list of files based on executability."""
    return [f for f in files if os.access(f, os.X_OK)]


def PythonCommands(python, commands):
    """Run a multiline Python command string in a specified Python."""
    proc = subprocess.Popen(python,
                            stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                            universal_newlines=True)  # Force text mode in Python3
    try:
        result, _ = proc.communicate(commands)
    except TypeError:  # For Python 3.1 only
        result, _ = proc.communicate(bytes(commands, encoding='latin-1'))
    if proc.returncode:
        raise BadReturn(proc.returncode)
    return result.splitlines()


def PrintExe(prefix, exe, real):
    """Print executable path with optional symlink reporting."""
    if real == exe:
        _print('%s: %s' % (prefix, exe))
    else:
        _print('%s: %s -> %s' % (prefix, exe, real))


def main(argv=None):  # pylint: disable=too-many-locals
    """Top-level main function."""
    if argv and len(argv) >= 2:  # If arg is given, run entire script remotely
        sys.stderr.write('NOTE:  Remote PATH may be incomplete.\n')
        this_file = open(__file__)
        this_script = this_file.read()
        this_file.close()
        result = PythonCommands(['ssh', '-T', argv[1], 'python'], this_script)
        if result and result[-1]:
            result += ['']  # Add trailing EOL if needed
        sys.stdout.write('\n'.join(result))
        return 0
    python_list = ExeFilter(FileList(MakePatterns(GetPaths(),
                            PYTHON_PATTERNS)))
    done = set()
    unique = total = 0
    for python in python_list:
        try:
            values = PythonCommands([python], '\n'.join(PYTHON_COMMANDS))
        except (OSError, BadReturn):  # Avoid 'as' for <2.6 compatibility
            exmsg = traceback.format_exception_only(*sys.exc_info()[:2])[-1].strip()
            _print('Skipping %s due to %s' % (python, exmsg))
            continue
        if len(values) != len(PYTHON_VALUE_NAMES):
            _print('Skipping %s due to number of results (%d) != %d'
                   % (python, len(values), len(PYTHON_VALUE_NAMES)))
            continue
        valdict = dict(zip(PYTHON_VALUE_NAMES, values))
        total += 1
        exe = valdict['exec']
        real = os.path.realpath(exe)
        if real in done:
            PrintExe('Redundant', python, os.path.realpath(python))
            PrintExe(' (Executable)', exe, real)
            continue
        done |= set([real])
        unique += 1
        PrintExe('Command', python, os.path.realpath(python))
        PrintExe(' Executable', exe, real)
        if valdict['lib-noarch'] == valdict['lib-arch']:
            _print(' Libs(all): %s' % valdict['lib-noarch'])
        else:
            _print(' Libs(arch=any): %s' % valdict['lib-noarch'])
            _print(' Libs(arch=specific): %s' % valdict['lib-arch'])
        if len(set([valdict[x] for x in PYTHON_PREFIX_NAMES])) == 1:
            _print(' Prefix(all): %s' % valdict['sys-prefix'])
        else:
            _print(' Prefix(sys): %s' % valdict['sys-prefix'])
            _print(' Prefix(std): %s' % valdict['std-prefix'])
            _print(' Prefix(exec): %s' % valdict['exec-prefix'])
    plural = unique != 1 and 's' or ''
    # pylint: disable=consider-using-ternary
    print('Found %d unique Python installation%s out of %d total'
          % (unique, plural, total))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))  # pragma: no cover
