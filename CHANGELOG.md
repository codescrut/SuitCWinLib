# Changelog

## Unreleased

Changes since 0.9.1.0, for the open-source release. The analysis results are
unchanged. The version will become 1.0.0 in a later release.

- MIT licence, README, API documentation and credits.
- `call_suitc` returns valid JSON when the command line is invalid; the error
  message was not escaped and broke the document.
- Demo: the output buffers were too small for large analyses (buffer
  overflow); error documents are now shown.
- Project files work from a clean checkout: the demo copies the DLL itself,
  and the Win32 configurations no longer write to a leftover directory.
- The Linux and macOS libraries are built with `-O3`; they were unoptimised.
  Large combinations run about 2.5 times faster, with identical results.
- Python example and regression tests (31 cases, identical on Windows and
  Linux).
- Build outputs are no longer kept in the repository; they are published as
  release assets.

## 0.9.1.0 (2026-10-10)

- Link the C runtime statically (`/MT`), so `SuitCLib.dll` depends only on
  KERNEL32.dll and loads without the Visual C++ Redistributable.
- Move the 128 KB JSON buffers and `print_tree`'s 40 KB line buffers off the
  stack, and drop an unused 128 KB buffer. A call now needs less than 96 KB of
  stack instead of about 650 KB, so it no longer overflows a 256 KB IIS
  request thread (0xC00000FD).

## 0.9.0.12 (2026-09-22)

- Report board-enumeration failures ("No boards are left") in the JSON result
  instead of on stdout.
- `call_suitc` returns the result code of the analysis; it always returned 0.

## 0.9.0.11 (2026-09-02)

- Fix a stack buffer overrun (0xC0000409) that killed the host process.
  `TRICKS` entries that `convert_tree_to_quirk` never filled were read, and the
  stale trick count was used to index arrays far past their end. Unfilled
  entries are now marked, and `evaluate_best_results` refuses a trick count
  above `ntricks`.
- Fix `best_N` being one entry short.
- Bound every walk over the NODE tree, so a malformed tree cannot walk off the
  end of the `bnode`/`lnode`/`tnode` stacks.
- Pool exhaustion no longer hands out entries outside the pool: the last entry
  of each pool is a sentinel and the call is refused.
- `bump_quirkh` no longer calls `exit(99)`; a library must not stop the host
  process.
- Pool exhaustion, depth overflow and an out-of-range trick count return an
  error document.
- Fix the version resource, which had reported 0.9.0.5 since that release.

## 0.9.0.10 (2026-09-02)

- Fix an access violation (0xC0000005) on empty game trees: `-wv`/`-ev` or
  `-wm`/`-em` combined with played tricks can filter every board away. An error
  document is returned instead.
- Release memory on both `create_tricks` failure paths.

## 0.9.0.9 (2026-06-12)

- Fix malformed JSON on SuitC error and success output.

## 0.9.0.8 (2026-04-26)

- Fix a native heap leak (about 430 KB per `call_suitc`) and a double-free
  crash. Export `free_memory()` so hosts can release the internal pools.
- Platform-aware `wchar_t` marshalling (2 bytes on Windows, 4 on Linux and
  macOS).
- Fix memory leaks in `evaluate_mp_scores`, `limit_matchpoint_nodes` and
  `limit_quirk`, which accumulated to gigabytes over long sessions.

## 0.9.0.7 (2026-03-25)

- Fix a crash when the leader has no cards left after the played tricks (the
  lead passes to partner).
- The leader is no longer derived from the trick winner; the caller decides
  who leads.
- Linux and macOS builds.

## 0.9.0.5 (2026-03-10)

- Played tricks can be passed after the hands.
- Updated to SuitC Rev 2.011 from Gerrit van der Velde.

## 2024

- 2024-09-07: Updated to SuitC of 28 July 2024 (Rev 1.039). Output is
  suppressed with `SUITCAPP`.
- 2024-05-18: First version: Lars Kappel Hansen's Visual C++ port of SuitC as
  a DLL with a JSON interface, and the C# demo.
