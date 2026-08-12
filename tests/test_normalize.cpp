#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include "csv_curve_publisher.hpp"
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

void testFormatTimeOfDay() {
    assert(formatTimeOfDay(52327500000000LL) == "14:32:07.500");
    assert(formatTimeOfDay(0) == "00:00:00.000");
}

void testCsvCurvePublisherSchema() {
    const std::string path = "test_curves_tmp.csv";
    std::remove(path.c_str());

    {
        CsvCurvePublisher pub(path, "2026-08-11");
        pub.publish("curve", NormalizedTick{"SWAP1", 52327500000000LL, 1.265, "exchangeA"});
        // Rows for other tables must be filtered out, not written.
        pub.publish("tradeLog", NormalizedTick{"SWAP2", 0, 9.99, "exchangeA"});
    }

    std::ifstream in(path);
    std::string header, row;
    std::getline(in, header);
    std::getline(in, row);
    std::string extra;
    const bool hasExtraRow = static_cast<bool>(std::getline(in, extra));
    in.close();
    std::remove(path.c_str());

    assert(header == "sym,date,time,rate");
    assert(row == "SWAP1,2026-08-11,14:32:07.500,1.265");
    assert(!hasExtraRow);
}

} // namespace

int main() {
    testNormalizeSourceA();
    testNormalizeSourceB();
    testParseSourceBLine();
    testFormatTimeOfDay();
    testCsvCurvePublisherSchema();
    std::cout << "all tests passed" << std::endl;
    return 0;
}
