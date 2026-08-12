#include <iostream>
#include <vector>

#include "csv_curve_publisher.hpp"
#include "source_normalizer.hpp"
#include "tick_publisher.hpp"

// Demonstrates the feed-handler role: raw records from two differently
// shaped exchange feeds are each run through their own normalizeSourceN,
// then published through two sinks - the live tickerplant (or console
// fallback) and a CSV in the schema downstream bookSnapshot.q reads via
// ("SDTF"; enlist ",") 0: `:curves.csv.
//
// Written to curves_generated.csv rather than curves.csv: CsvCurvePublisher
// appends on every run, and curves.csv in the repo root is committed
// sample/seed data for bookSnapshot.q - writing straight to it would
// silently grow a tracked fixture file on every run of this demo.
int main() {
    auto publisher = makeDefaultPublisher("localhost", 5010, "user:password");
    CsvCurvePublisher csvPublisher("curves_generated.csv", todayUtcYmd());

    const std::vector<RawTickSourceA> rawA = {
        {"SWAP1", 52327500000LL, 12650.0},   // epoch ms, rate in bps
        {"SWAP2", 41000000000LL, 8420.5},
    };

    const std::vector<std::string> rawBLines = {
        "55=SWAP1|273=52327500000000|270=1.265",
        "55=SWAP3|273=61234500000000|270=2.010",
    };

    for (const auto& raw : rawA) {
        NormalizedTick tick = normalizeSourceA(raw);
        publisher->publish("curve", tick);
        csvPublisher.publish("curve", tick);
    }

    for (const auto& line : rawBLines) {
        try {
            NormalizedTick tick = normalizeSourceB(parseSourceBLine(line));
            publisher->publish("curve", tick);
            csvPublisher.publish("curve", tick);
        } catch (const std::invalid_argument& e) {
            std::cerr << "skipping malformed source B record: " << e.what() << std::endl;
        }
    }

    return 0;
}
