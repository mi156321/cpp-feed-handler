#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

#include "normalized_tick.hpp"

// Raw record as it comes off ExchangeA's wire format: already decoded from
// its binary frame into native types, but on ExchangeA's own conventions -
// epoch milliseconds instead of ns-since-midnight, and rate quoted in bps.
struct RawTickSourceA {
    std::string symbol;
    int64_t     epochMs;
    double      rateBps;
};

// Raw record as parsed from ExchangeB's FIX-like tag=value wire messages,
// e.g. "55=SWAP1|273=52327500000000|270=1.265". ExchangeB already sends
// ns-since-midnight and a decimal rate, but uses its own field tags and
// symbol prefixing convention.
struct RawTickSourceB {
    std::string tag55Symbol;
    int64_t     tag273TimeNs;
    double      tag270Rate;
};

// normalizeSourceA / normalizeSourceB mirror the q-side normalizeSourceN
// functions: each raw source has its own quirks (units, field names,
// symbol conventions), and each gets its own normalizer that maps it onto
// the one shared NormalizedTick schema. Adding a new raw source means
// adding one more normalizeSourceN, not touching anything downstream.
NormalizedTick normalizeSourceA(const RawTickSourceA& raw);
NormalizedTick normalizeSourceB(const RawTickSourceB& raw);

// Parses a raw "55=...|273=...|270=..." FIX-like line into RawTickSourceB.
// Throws std::invalid_argument on a malformed line.
RawTickSourceB parseSourceBLine(const std::string& line);
