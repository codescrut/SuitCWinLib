"""Regression tests for SuitCLib.

Runs every case in cases.txt through call_suitc in a single process and
compares the JSON analysis and the text details with tests/expected.
Running all cases in one process also checks that no state leaks from one
call to the next.

    python tests/run_tests.py <path to SuitCLib.dll / libsuitc.so / libsuitc.dylib>
    python tests/run_tests.py <library> --update     # regenerate tests/expected
"""
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'examples', 'python'))
from suitc_example import BUFFER_SIZE, load  # noqa: E402

import ctypes  # noqa: E402
from ctypes import byref, c_int, c_wchar_p, cast, create_unicode_buffer  # noqa: E402


def read_cases():
    cases = []
    with open(os.path.join(HERE, 'cases.txt'), encoding='utf-8') as f:
        for line in f:
            m = re.match(r'^([a-z0-9-]+):\s*(.*?)\s*$', line)
            if m:
                cases.append(m.groups())
    return cases


def call(lib, arguments):
    inp = c_wchar_p(arguments)
    out = create_unicode_buffer(BUFFER_SIZE)
    det = create_unicode_buffer(BUFFER_SIZE)
    out_ptr, det_ptr = cast(out, c_wchar_p), cast(det, c_wchar_p)
    out_len, det_len = c_int(0), c_int(0)
    rc = lib.call_suitc(byref(inp), len(arguments), byref(out_ptr), byref(out_len),
                        byref(det_ptr), byref(det_len))
    return 'rc=%d\n%s\n' % (rc, out.value), det.value


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(2)
    lib = load(sys.argv[1])
    update = '--update' in sys.argv
    expected_dir = os.path.join(HERE, 'expected')
    os.makedirs(expected_dir, exist_ok=True)
    failed = 0
    cases = read_cases()
    for name, arguments in cases:
        output, details = call(lib, arguments)
        try:
            json.loads(output.split('\n', 1)[1])
        except ValueError as e:
            failed += 1
            print('FAIL %s: invalid JSON (%s)' % (name, e))
        for suffix, actual in (('.json', output), ('.txt', details)):
            path = os.path.join(expected_dir, name + suffix)
            if update:
                with open(path, 'w', encoding='utf-8', newline='\n') as f:
                    f.write(actual)
                continue
            with open(path, encoding='utf-8', newline='') as f:
                expected = f.read().replace('\r\n', '\n')
            if actual.replace('\r\n', '\n') != expected:
                failed += 1
                print('FAIL %s%s: %s' % (name, suffix, arguments))
    if update:
        print('updated %d cases' % len(cases))
    else:
        print('%d of %d cases passed' % (len(cases) - failed, len(cases)))
    sys.exit(1 if failed else 0)


if __name__ == '__main__':
    main()
