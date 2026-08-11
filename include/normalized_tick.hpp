#pragma once

#include <cstdint>
#include <string>

// The single common schema every raw source is normalized into, matching
// the q side's `curve` table: sym (symbol), time (timespan), rate (float),
// source (symbol). Everything downstream of the feed handler only ever
// sees this shape, regardless of which raw source produced it.
struct NormalizedTick {
    std::string sym;
    int64_t     timeNs;   // nanoseconds since midnight, matches q's timespan
    double      rate;
    std::string source;
};
