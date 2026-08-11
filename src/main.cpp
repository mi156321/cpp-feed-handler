#include <iostream>
#include <vector>

#include "source_normalizer.hpp"
#include "tick_publisher.hpp"

// Demonstrates the feed-handler role: raw records from two differently
// shaped exchange feeds are each run through their own normalizeSourceN,
// then published through one common sink into the `curve` table -
// downstream code never needs to know which exchange a row came from.
int main() {
    auto publisher = makeDefaultPublisher("localhost", 5010, "user:password");

    const std::vector<RawTickSourceA> rawA = {
        {"SWAP1", 52327500000LL, 12650.0},   // epoch ms, rate in bps
        {"SWAP2", 41000000000LL, 8420.5},
    };

    const std::vector<std::string> rawBLines = {
        "55=SWAP1|273=52327500000000|270=1.265",
        "55=SWAP3|273=61234500000000|270=2.010",
    };

    for (const auto& raw : rawA) {
        publisher->publish("curve", normalizeSourceA(raw));
    }

    for (const auto& line : rawBLines) {
        try {
            publisher->publish("curve", normalizeSourceB(parseSourceBLine(line)));
        } catch (const std::invalid_argument& e) {
            std::cerr << "skipping malformed source B record: " << e.what() << std::endl;
        }
    }

    return 0;
}
