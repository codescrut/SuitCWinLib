# SuitCLib API

The library exports three C functions (`cdecl` calling convention on Windows).

```c
const char* version(void);

int call_suitc(wchar_t** input,   int  inputLength,
               wchar_t** output,  int* outputLength,
               wchar_t** details, int* detailsLength);

void free_memory(void);
```

## version

Returns the library version as a static ASCII string, such as `"0.9.1.0"`.

## call_suitc

Runs one SuitC analysis.

| Parameter | Direction | Meaning |
|---|---|---|
| `input` | in | Pointer to a pointer to the arguments, as a wide-character string (see below). |
| `inputLength` | in | Number of characters in the input. |
| `output` | in/out | Pointer to a pointer to a buffer of at least **131,073** wide characters. Receives the JSON analysis, null-terminated. |
| `outputLength` | out | Number of characters written to `output`, without the terminator. |
| `details` | in/out | Pointer to a pointer to a buffer of at least **131,073** wide characters. Receives SuitC's text output for the optimum plays, null-terminated. |
| `detailsLength` | out | Number of characters written to `details`. |

The return value is `0` on success and `1` on an error. On an error, `output`
holds a JSON error document.

`wchar_t` is 2 bytes on Windows and 4 bytes on Linux and macOS. The library
uses the platform's `wchar_t`, which is what C# `string` marshalling on Windows
and Python's `ctypes.c_wchar_p` produce. The input must be ASCII.

The library does **not** check the size of the output buffers. Each output
can be up to 131,072 characters long, so allocate at least 131,073.

SuitC keeps its state in global variables. Make only **one call at a time**
per process.

## free_memory

Releases SuitC's memory pools. A later call to `call_suitc` allocates them
again. Call it when a long-running host wants the memory back between
analyses.

## The input

The input is a SuitC command line without the program name:

```text
[options] <north> <south> <east-west> [<played trick> ...]
```

The options and the hands are those of the SuitC program, see
[SuitC usage](https://github.com/ThorvaldAagaard/SuitC/blob/main/docs/usage.md).
For example: `-Ls -ls1 AQ2 543 KJT9876`.

The library is built with `SUITCAPP=YES`. Compared with the SuitC program:

- at most 10 plays are reported;
- a cut-off is always applied to merging quirk trees;
- the East-West distributions are listed in Jeroen Warmerdam's order;
- card sequences are printed with one symbol by default.

### Played tricks

Tricks that have already been played can follow the three hands. Each trick
is one argument:

```text
[n|e|s|w]<card><card>[<card>[<card>]]
```

- The optional seat letter gives the leader of that trick. Without it, the
  leader of the previous trick leads again. The first trick needs either a
  seat letter or the `-L` option.
- Then come the cards in playing order, two to four of them. A `-` means
  that the player shows out (has no card in the suit).
- SuitC does not know who won the trick, so give the leader of each trick
  explicitly when it changes.

```text
-Ls AQJ 432 KT98765 s25Q6        South led the 2, West the 5, North the queen, East the 6
-Ls AQJ2 543 KT9876 s36Q7 s48J9  two tricks, both led by South
```

The played cards are removed from the hands. Distributions that are
inconsistent with the cards the defenders played are discarded, and the
probabilities of the remaining distributions are renormalised. The analysis
then continues from the new position.

> **Note.** This is an approximation. SuitC's model fixes North-South's
> strategy before the first card is played. The defenders may be playing a
> mixed strategy, and the cards they choose change the conditional
> probabilities in ways a simple renormalisation does not capture. Gerrit van
> der Velde's view is that a fully correct answer requires following the
> original game tree rather than starting a new analysis.

## The output

### Success

```json
{
  "SuitCAnalysis": {
    "OptimalResults": [ { "Tricks": 2, "Percentage": 50.4845 } ],
    "Result": 1.5048,
    "OptimumPlays": [
      {
        "OptimumPlayFor": "MAX, N2",
        "Plays": [
          { "Tricks": 2, "Percentage": 50.4845 },
          { "Tricks": 1, "Percentage": 49.5155 }
        ],
        "Result": 1.5048,
        "GameTree": [
          { "T": "N:A", "Vs": [
            { "T": "KJ-", "Vs": [
              { "T": "3", "Vs": [
                { "T": "KJ-", "Vs": [
                  { "T": "S:5", "Vs": [
                    { "T": "K",  "Vs": [ { "T": "2", "Vs": [ { "T": "J-",  "Vs": [] } ] } ] },
                    { "T": "J-", "Vs": [ { "T": "Q", "Vs": [ { "T": "KJ-", "Vs": [] } ] } ] }
                  ] }
                ] }
              ] }
            ] }
          ] }
        ],
        "Error": "No errors"
      }
    ]
  }
}
```

| Field | Meaning |
|---|---|
| `OptimalResults` | For each number of tricks, the best probability (percent) of taking at least that many with any play. |
| `Result` | The highest expected number of tricks. |
| `OptimumPlays` | The recommended plays, each optimal for the objectives listed in `OptimumPlayFor`. |
| `OptimumPlayFor` | The objectives this play is optimal for: `MAX` (most expected tricks), `N<k>` (best chance of at least `<k>` tricks), `MP` or `IMP` (with `-M` or `-I`), `S<k>` (as `N<k>`, with `-D`). |
| `Plays` | For this play, the probability (percent) of taking exactly `Tricks` tricks. |
| `GameTree` | The play as a tree. Each node `T` is one card or set of equivalent cards, in playing order. A seat prefix (`N:`, `S:`) marks the leader of a new trick, and `-` means showing out. `Vs` holds the variations that follow; more than one variation means the play branches on the defenders' cards. |
| `Error` | `"No errors"` on success; it appears in the last element of `OptimumPlays`. |

`details` contains the same optimum plays in SuitC's text format, with the
probability per distribution. See
[Reading the output](https://github.com/ThorvaldAagaard/SuitC/blob/main/docs/output.md).

### Error

```json
{"SuitCAnalysis": { "Error": "A leader must be specified with -L when using played tricks"}}
```

`call_suitc` returns 1. Possible errors include invalid arguments, a game tree
that cannot be built (for example, an impossible `-S` rule), no distribution
consistent with the played tricks, and exhausted memory pools.

## Building the input from a request object (C#)

This is how a C# host can turn a request into the input string:

```csharp
static string Add(string s, string parameter) =>
    parameter == "" ? s : (s == "" ? parameter : s + " " + parameter);

static string ToArguments(Request r)
{
    string s = "";
    if (r.West != "") s = Add(s, "-W" + r.West);
    if (r.East != "") s = Add(s, "-E" + r.East);
    if (r.MinCardsWest is > 0 and <= 13) s = Add(s, "-wm" + r.MinCardsWest);
    if (r.MinCardsEast is > 0 and <= 13) s = Add(s, "-em" + r.MinCardsEast);
    if (r.VacantPlacesWest is >= 0 and < 13) s = Add(s, "-wv" + r.VacantPlacesWest);
    if (r.VacantPlacesEast is >= 0 and < 13) s = Add(s, "-ev" + r.VacantPlacesEast);
    if (r.NoOfRuffsNorth is > 0 and <= 13) s = Add(s, "-Rn" + r.NoOfRuffsNorth);
    if (r.NoOfRuffsSouth is > 0 and <= 13) s = Add(s, "-Rs" + r.NoOfRuffsSouth);
    if (r.NoOfRuffsWest is > 0 and <= 13) s = Add(s, "-Rw" + r.NoOfRuffsWest);
    if (r.NoOfRuffsEast is > 0 and <= 13) s = Add(s, "-Re" + r.NoOfRuffsEast);
    if (r.LimitNoOfTricks is >= 0 and < 13) s = Add(s, "-t" + r.LimitNoOfTricks);
    if (r.OnLead == "north") s = Add(s, "-Ln" + r.LeadCard);
    if (r.OnLead == "south") s = Add(s, "-Ls" + r.LeadCard);
    if (r.FreeLeadsNorth < 13) s = Add(s, "-ln" + r.FreeLeadsNorth);
    if (r.FreeLeadsSouth < 13) s = Add(s, "-ls" + r.FreeLeadsSouth);
    s = Add(s, r.North);
    s = Add(s, r.South);
    s = Add(s, r.WestEast);
    return s;
}
```

See [Demo/DemoForm.cs](../Demo/DemoForm.cs) for the P/Invoke declaration and
the buffer handling, and
[examples/python/suitc_example.py](../examples/python/suitc_example.py) for
Python.
