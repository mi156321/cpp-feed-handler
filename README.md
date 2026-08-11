# feed_handler

A C++ feed handler that normalizes raw exchange/vendor tick data into a
single consistent schema and publishes it into a kdb+ tickerplant over IPC,
using the official kdb+ C API (`k.h`/`k.c`).

This is the upstream half of the pipeline described in `interview_cplus.txt`:

```
Exchange/vendor raw feed (binary/FIX)
        |
        v
   C++ feed handler  -->  kdb+ C API (k.h)  -->  Tickerplant (q)  -->  RDB/HDB
```

Downstream q code (tickerplant, RDB/HDB, as-of-join analytics) only ever
sees one schema — sym/time/rate/source — regardless of which raw source or
which process (q or C++) published the row.

## Layout

- `include/normalized_tick.hpp` — the shared output schema.
- `include/source_normalizer.hpp` / `src/source_normalizer.cpp` — one
  `normalizeSourceN` per raw feed shape (mirrors the q-side pattern of the
  same name). Adding a new exchange means adding one more normalizer, not
  touching anything downstream.
- `include/tick_publisher.hpp` / `src/tick_publisher.cpp` — publishing
  sink. `ConsolePublisher` always works; `KdbTickerplantPublisher` sends
  real IPC `upd` messages via the kdb+ C API when built with `-DWITH_KDB=ON`.
- `src/main.cpp` — demo wiring two raw sources through their normalizers
  into one publisher.
- `tests/test_normalize.cpp` — unit tests for the normalization logic.

## Building without kdb+ (default)

No external dependencies. Runs the pipeline and prints normalized ticks to
stdout instead of publishing over IPC.

```sh
cmake -S . -B build
cmake --build build
./build/feed_handler
ctest --test-dir build
```

## Building with a live tickerplant

The kdb+ C API (`k.h`/`k.c`) ships with your kdb+ install
(`$QHOME/c/c/k.h` and `k.c`) and isn't redistributed here. Copy both into
`third_party/kdb/`, then:

```sh
cmake -S . -B build -DWITH_KDB=ON
cmake --build build
./build/feed_handler   # publishes into localhost:5010 via `upd`
```

Point a q tickerplant at port 5010 with a `curve` table
(`sym`,`time`,`rate`,`source`) listening for `upd` before running.
