"""Call SuitCLib from Python with ctypes.

Usage:
    python suitc_example.py <path to SuitCLib.dll / libsuitc.so / libsuitc.dylib> [SuitC arguments]

Example:
    python suitc_example.py ./libsuitc.so -Ls AQ2 543 KJT9876
"""
import ctypes
import json
import sys
from ctypes import POINTER, byref, c_int, c_wchar_p, cast, create_unicode_buffer

# The library writes up to 32768*4 characters plus a terminating null into each
# output buffer and does not check the size, so the buffers must be at least this big.
BUFFER_SIZE = 32768 * 4 + 1


def load(path):
    lib = ctypes.CDLL(path)
    lib.version.restype = ctypes.c_char_p
    lib.call_suitc.argtypes = [POINTER(c_wchar_p), c_int,
                               POINTER(c_wchar_p), POINTER(c_int),
                               POINTER(c_wchar_p), POINTER(c_int)]
    lib.call_suitc.restype = c_int
    return lib


def call_suitc(lib, arguments):
    """Run SuitC with a command line such as '-Ls AQ2 543 KJT9876'.

    Returns (rc, analysis, details): rc is 0 on success and 1 on error,
    analysis is the parsed JSON document (it holds the error message if rc is 1),
    and details is SuitC's text output for the optimum plays.
    """
    inp = c_wchar_p(arguments)
    out = create_unicode_buffer(BUFFER_SIZE)
    det = create_unicode_buffer(BUFFER_SIZE)
    out_ptr, det_ptr = cast(out, c_wchar_p), cast(det, c_wchar_p)
    out_len, det_len = c_int(0), c_int(0)
    rc = lib.call_suitc(byref(inp), len(arguments),
                        byref(out_ptr), byref(out_len),
                        byref(det_ptr), byref(det_len))
    return rc, json.loads(out.value), det.value


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        sys.exit(2)
    lib = load(sys.argv[1])
    print("SuitCLib", lib.version().decode())
    rc, analysis, details = call_suitc(lib, " ".join(sys.argv[2:]))
    if rc != 0:
        print("Error:", analysis["SuitCAnalysis"]["Error"])
        sys.exit(1)
    result = analysis["SuitCAnalysis"]
    print("Expected tricks:", result["Result"])
    for play in result["OptimumPlays"]:
        chances = ", ".join("%d: %.2f%%" % (p["Tricks"], p["Percentage"]) for p in play["Plays"])
        print("Optimum play for %s -> %s" % (play["OptimumPlayFor"], chances))
    print(details)


if __name__ == "__main__":
    main()
