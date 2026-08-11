#include <cassert>
#include <cmath>
#include <iostream>

#include "source_normalizer.hpp"

namespace {

bool nearlyEqual(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) < eps;
}

void testNormalizeSourceA() {
    // epochMs deliberately spans multiple days, to exercise the
    // epoch-ms -> ns-since-midnight wraparound in normalizeSourceA.
    RawTickSourceA raw{"SWAP1", 52327500000LL, 12650.0};
    NormalizedTick tick = normalizeSourceA(raw);

    const int64_t expectedTimeNs = (52327500000LL % 86'400'000LL) * 1'000'000LL;

    assert(tick.sym == "SWAP1");
    assert(tick.timeNs == expectedTimeNs);
    assert(nearlyEqual(tick.rate, 1.265));
    assert(tick.source == "exchangeA");
}

void testNormalizeSourceB() {
    RawTickSourceB raw{"SWAP1", 52327500000000LL, 1.265};
    NormalizedTick tick = normalizeSourceB(raw);

    assert(tick.sym == "SWAP1");
    assert(tick.timeNs == 52327500000000LL);
    assert(nearlyEqual(tick.rate, 1.265));
    assert(tick.source == "exchangeB");
}

void testParseSourceBLine() {
    RawTickSourceB raw = parseSourceBLine("55=SWAP1|273=52327500000000|270=1.265");
    assert(raw.tag55Symbol == "SWAP1");
    assert(raw.tag273TimeNs == 52327500000000LL);
    assert(nearlyEqual(raw.tag270Rate, 1.265));

    bool threw = false;
    try {
        parseSourceBLine("55=SWAP1|270=1.265"); // missing tag 273
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
}

} // namespace

int main() {
    testNormalizeSourceA();
    testNormalizeSourceB();
    testParseSourceBLine();
    std::cout << "all tests passed" << std::endl;
    return 0;
}
