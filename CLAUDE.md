# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SuitCWinLib is a Windows DLL port of **SuitC**, a Bridge card game suit combination analyzer. It calculates optimal card plays, trick probabilities, and scoring comparisons (matchpoint vs IMP) for specific suit combinations.

- **Author:** Gerrit A. van der Velde (original SuitC), Lars Kappel Hansen (Windows port)
- **Version:** 0.9.0.4

## Build Commands

### Windows (Primary) — Visual Studio 2022

Open `SuitCLib.sln` in VS2022 and build in **Release** configuration (x64 or x86).

**Important: Debug builds are broken. Always use Release mode.**

The solution contains two projects:
- **SuitCLib** (C++ DLL) → produces `SuitCLib.dll`
- **Demo** (C# WinForms, .NET 8.0) → produces `Demo.exe`, depends on `SuitCLib.dll`

### Linux — Makefile

```bash
cd SuitCLib
make clean && make          # SUITCAPP defaults to 1
make SUITCAPP=0             # Exhaustive analysis mode (uses 80M quirk entries instead of 4M)
```

Produces `libsuitc.so`.

## Architecture

### DLL Interface

The public API is defined in [lib_main.h](SuitCLib/lib_main.h) and implemented in [lib_main.cpp](SuitCLib/lib_main.cpp):

- `call_suitc()` — Main entry point. Takes wide-char command-line parameters, returns two JSON buffers (results + optimum details). Called from C# via P/Invoke.
- `version()` — Returns version string.

`call_suitc()` parses the wide-char input into argc/argv, then delegates to `suitc_main()` in [suitc.c](SuitCLib/suitc.c).

### Core Algorithm Flow (suitc_main)

1. Parse command-line args (`process_args.c`)
2. Allocate static memory pools for nodes, indices, quirks
3. Create game tree of all possible plays (`create_ply1-4.c`, `add_node.c`)
4. Convert tree to optimized quirk representation (`convert_tree_to_quirk.c`)
5. Fold/merge trees to remove dominated plays (`fold_*.c`, `merge_nodes.c`)
6. Evaluate results via minimax (`minimax.c`, `evaluate_*.c`, `check_zermelo.c`)
7. Format output as JSON (`create_game_tree_json_value.c`, `print_*.c`)

### Key Data Structures (defined in [header.h](SuitCLib/header.h))

- **COLOR** — `unsigned short` bitmask, each bit represents one card
- **NODE** — Game tree node with parent/sibling/child pointers, card played, trick info
- **INDEX** — Board configuration tracking remaining cards per player
- **QUIRKD/QUIRKH** — Compact optimized tree representation (quirk tree)
- **RESULT** — Analysis output: optimal play code, per-trick probabilities

### Memory Configuration

Large static allocations controlled by defines in `header.h`:
- `NNODES` = 10M (game tree nodes)
- `NINDICES` = 16M (board indices)
- `NQUIRKS` = 4M (app mode) or 80M (normal mode), controlled by `SUITCAPP` macro
- `JSON_BUFFER_SIZE` = 128KB per output buffer

### Source File Organization

Each `.c` file implements one logical function. ~52 C source files grouped by purpose:
- **Tree construction:** `add_node`, `copy_node`, `create_ply1-4`, `create_tricks`, `build_selected_tree`
- **Quirk tree:** `add_quirk`, `clean_quirk`, `fold_quirk`, `merge_quirk`, `convert_tree_to_quirk`
- **Play evaluation:** `check_ew/ns_plays`, `select_ew/ns_plays`, `minimax`, `check_zermelo`
- **Tree optimization:** `fold_ew/ns_tree`, `fold_tree`, `limit_matchpoint_nodes`, `merge_nodes`
- **Output:** `print_results`, `print_tree`, `create_game_tree_json_value`
- **Config:** `process_args`, `suitc_globals` (global variable definitions)

### Platform Abstraction

- [header_win.h](SuitCLib/header_win.h) / [header_linux.h](SuitCLib/header_linux.h) — Platform-specific config
- `SUITC_API` macro handles `__declspec(dllexport)` (Windows) vs `__attribute__((visibility))` (GCC)
- [dllmain.cpp](SuitCLib/dllmain.cpp) — Windows-only DLL entry point
- [Makefile](SuitCLib/Makefile) excludes `dllmain.cpp` and `pch.cpp` for Linux builds

### Demo Application

C# Windows Forms app (.NET 8.0) in [Demo/](Demo/). Uses P/Invoke to call `SuitCLib.dll`. Features JSON syntax highlighting via Newtonsoft.Json. Default input: `-wv9 -ev8 K3 2 AQJT987654`.

## Command-Line Parameters

Passed as a string to `call_suitc()`, parsed in `process_args.c`:

- **Cards:** positional args = North, South, WestEast holdings
- **Fixed cards:** `-W<cards>`, `-E<cards>` (e.g., `-WAK`)
- **Constraints:** `-wm<n>`/`-em<n>` (min cards), `-wv<n>`/`-ev<n>` (vacant places)
- **Ruffs:** `-Rn<n>`, `-Rs<n>`, `-Rw<n>`, `-Re<n>`
- **Lead:** `-Ln<card>`/`-Ls<card>` (lead restriction), `-ln<n>`/`-ls<n>` (free leads)
- **Tricks:** `-t<n>` (trick limit)
- **Scoring:** `-M` (matchpoint), `-I` (IMP)
- **Defense:** `-D[W]` (defense mode)
- **Output:** `-A` (all results), `-a` (summary), `-b` (boards), `-f` (full tree)
- **Analysis:** `-z` (Zermelo), `-c<value>` (cutoff nodes)

## Key Conventions

- Extensive use of global variables (~100, declared `extern` in `header.h`, defined in `suitc_globals.c`)
- Heavy recursion in tree building/traversal — watch stack depth
- Compiler warnings 4996 (deprecated CRT) and 4703 (uninitialized) are suppressed in the MSVC project
- No external C/C++ dependencies; the core library uses only the C standard library
