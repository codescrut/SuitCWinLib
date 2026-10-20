# SuitCWinLib

**SuitCWinLib** packages [SuitC](https://github.com/ThorvaldAagaard/SuitC),
Gerrit A. van der Velde's bridge suit combination analyser, as a shared
library with a JSON interface. Programs written in C#, Python, C/C++ or any
other language that can call a C function can use it to find the best way
to play a suit.

It is used by the bridge robots [BEN](https://github.com/lorserker/ben) and
Lia, and by the [SuitC web application](https://bridge.esmarkkappel.dk/suitc/main.html).

| Platform | Library |
|---|---|
| Windows x64 / x86 | `SuitCLib.dll` |
| Linux x64 / arm64 | `libsuitc.so` |
| macOS arm64 / x64 | `libsuitc.dylib` |

Pre-built libraries and the Windows demo are attached to each
[release](../../releases).

## Quick start (Python)

```python
from suitc_example import load, call_suitc     # examples/python/suitc_example.py

lib = load("SuitCLib.dll")                       # or libsuitc.so / libsuitc.dylib
rc, analysis, details = call_suitc(lib, "-Ls AQ2 543 KJT9876")
print(analysis["SuitCAnalysis"]["Result"])      # 1.5048 expected tricks
```

The arguments are those of the SuitC command-line program
([SuitC usage](https://github.com/ThorvaldAagaard/SuitC/blob/main/docs/usage.md)):
the North cards, the South cards and the East-West cards, preceded by
options. The library adds one feature: tricks that have already been played
can be appended. See [docs/api.md](docs/api.md) for the C API, the JSON format
and the played-tricks syntax.

## Contents

| Directory | What it is |
|---|---|
| [SuitCLib/](SuitCLib) | The library: the SuitC core plus the library interface (`lib_main.cpp`), JSON output and played-trick support |
| [Demo/](Demo) | A Windows Forms demo (C#, .NET 8) that calls the DLL and shows the JSON |
| [examples/python/](examples/python) | Calling the library from Python with ctypes |
| [tests/](tests) | Regression tests run through the C API |
| [docs/](docs) | API documentation |

## Building

**Windows** (Visual Studio 2022 or later): open `SuitCLib/SuitCLib.sln` and build
**Release** for x64 or x86, or run `SuitCLib\build_dll.bat [x64|x86]`. The DLL
is written to `SuitCLib\x64\Release\` (x64) or `SuitCLib\Release\` (x86). It
links the C runtime statically, so it needs no Visual C++ Redistributable.
Debug x64 builds give the same results, about three times slower; Win32 Debug
has not been tested.

The demo project copies the x64 DLL when you build it:

```bat
SuitCLib\build_dll.bat x64
dotnet run --project Demo -c Release
```

**Linux and macOS:**

```sh
cd SuitCLib
make                                                   # libsuitc.so
make TARGET_LIB=libsuitc.dylib LDFLAGS="-dynamiclib -fvisibility=hidden"   # macOS
make -f Makefile.cli                                   # the stand-alone SuitC program
```

**Tests** (any platform, Python 3):

```sh
python tests/run_tests.py SuitCLib/x64/Release/SuitCLib.dll    # or SuitCLib/libsuitc.so
```

## Things to know

- **One call at a time.** SuitC keeps its state in global variables, so the
  library is not thread-safe. Serialise calls within a process, or run SuitC
  in worker processes (BEN does this, so it can also enforce a time limit).
- **Output buffers.** Each output buffer must hold at least 131,073 wide
  characters. The library does not check the size you pass.
- **Console output.** The library also writes some diagnostics to stdout. If
  your program uses stdout for other purposes, redirect it.
- **Memory and time.** Most combinations take milliseconds. Some large
  combinations take minutes and need more memory than the library's pools
  allow; you then get an error document. Calling `free_memory()` releases the
  pools between calls.

## Credits and licence

- **Gerrit A. van der Velde** wrote SuitC.
- **Lars Kappel Hansen** ported SuitC to Visual C++ and wrote the JSON
  interface, the demo and the restricted-choice handling of played tricks.
- **Thorvald Aagaard** maintains SuitCWinLib: played tricks, the Linux and
  macOS builds, and robustness fixes.

See [AUTHORS.md](AUTHORS.md). SuitCWinLib is released under the
[MIT licence](LICENSE).
